#if defined(WIN32) || defined(_WIN32)
#include <Windows.h>
#else
#include <dlfcn.h>
#endif

#include <QCoreApplication>
#include <QDebug>
#include <QDir>

#include "ModelFactory.h"
#include <filesystem>
namespace fs = std::filesystem;

std::string ModelFactory::sModuleName = "createModel";
std::map<std::string, ModelFactory::ModelDescriptor> ModelFactory::m_PlugIns;

ModelFactory::ModelFactory() { }

std::vector<std::string> ModelFactory::getModelList()
{
	std::vector<std::string> aModelList;
    t_mapPlugIns::iterator end = m_PlugIns.end();
    for (t_mapPlugIns::iterator it = m_PlugIns.begin(); it != end; it++) {
        aModelList.push_back(it->second.getModelName());
    }
	return aModelList;
}

QObject* ModelFactory::createModel(QObject* aParent, const std::string& modelName, const std::string& instanceName)
{
    t_mapPlugIns::iterator vIter = m_PlugIns.find(modelName);
    if (vIter != m_PlugIns.end()) {
        return vIter->second.createModel(aParent, instanceName);
    }
    else {
        qWarning() << "Cannot find private model " << QString(modelName.c_str()) << ", " << QString(instanceName.c_str());
    }
    return nullptr;
}

void ModelFactory::deleteModel(const std::string& modelName, const std::string& instanceName)
{
    t_mapPlugIns::iterator vIter = m_PlugIns.find(modelName);
    if (vIter != m_PlugIns.end()) {
        return vIter->second.deleteModel(instanceName);
    }
    else {
        qWarning() << "Cannot find private model " << QString(modelName.c_str()) << ", " << QString(instanceName.c_str());
    }
}

void ModelFactory::findModels(){
	// Search for models
    if (!lookupModels(fs::current_path().string())) {
        lookupModels(std::getenv("CAIRN_BIN"));
    }
}

bool ModelFactory::lookupModels(const std::string& a_Path)
{
    bool vRet = false;
    std::string filterExt, filterEnd = "CairnModel";
#if (defined (_WIN32) || defined (_WIN64))
    filterExt = ".dll";    
#else
    filterExt = ".so";    
#endif
    qDebug() << "Search models in: " << QString(a_Path.c_str());
    fs::path vPath(a_Path);
    for (auto const& dir_entry : fs::directory_iterator{ vPath }) {
        if (!dir_entry.is_directory()) {
            const fs::path& vFile = dir_entry.path();
            if (vFile.extension() == filterExt) {
                std::string vModelName = vFile.stem().string();
                size_t vPos = vModelName.rfind(filterEnd);
                if (vPos != std::string::npos) {
                    ModelDescriptor modelDesc;
                    modelDesc.setDllPath(fs::absolute(vFile).string());
                    vModelName.replace(vPos, vPos + filterEnd.size(), "");
                    modelDesc.setModelName(vModelName);
                    m_PlugIns[vModelName] = modelDesc;
                    qInfo() << "Found model " << QString(vModelName.c_str()) << "(" << QString(fs::absolute(vFile).string().c_str()) << ")";
                    vRet = true;
                }
            }
        }   
    }
    return vRet;
}

//*************************************** ModelDescriptor *********************************************
ModelFactory::ModelDescriptor::ModelDescriptor()
{       
}

const std::string ModelFactory::ModelDescriptor::getModelName()
{
    return mModelName;
}

void ModelFactory::ModelDescriptor::setModelName(const std::string aModelName) 
{
    mModelName = aModelName;
}

void ModelFactory::ModelDescriptor::setDllPath(const std::string& a_Path) 
{
    mDLLAbsoluteName = a_Path;
}

QObject* ModelFactory::ModelDescriptor::loadModel(QObject* aParent)
{
    QObject* vRet = nullptr;
    qDebug() << "Load Model" << QString(mDLLAbsoluteName.c_str());
         
#if defined(WIN32) || defined(_WIN32)   
    HINSTANCE hGetProcIDDLL = LoadLibraryEx(mDLLAbsoluteName.c_str(), 0, LOAD_LIBRARY_SEARCH_DEFAULT_DIRS);
    if (!hGetProcIDDLL) {
        // 2ieme essai
        hGetProcIDDLL = LoadLibraryEx(mDLLAbsoluteName.c_str(), 0, LOAD_WITH_ALTERED_SEARCH_PATH);
    }    
#else   
    void* hGetProcIDDLL = dlopen((const char*)mDLLAbsoluteName.c_str(), RTLD_NOW);
#endif
    if (!hGetProcIDDLL) {
        DWORD dError = GetLastError();
        qCritical() << "could not load the dynamic library " << QString(mDLLAbsoluteName.c_str()) << ", error: " << dError;
        throw(std::exception_ptr());
    }
    typedef QObject* (*f_privateModel)(QObject* aParent);
    f_privateModel vFunct;

    // resolve function address here
#if defined(WIN32) || defined(_WIN32)
    vFunct = (f_privateModel)GetProcAddress(hGetProcIDDLL, sModuleName.c_str());
#else
    vFunct = (f_privateModel)dlsym(hGetProcIDDLL, sModuleName.c_str());
#endif

    if (!vFunct) {
        DWORD dError = GetLastError();
        qCritical() << "could not locate the function createModel" << ", error: " << dError;
        throw(std::exception_ptr());
    }
    vRet = (*vFunct)(aParent);

    if (!vRet) {
        qCritical() << "could not create the Model";
        throw(std::exception_ptr());
    }
    return vRet;
}

QObject* ModelFactory::ModelDescriptor::createModel(QObject* aParent, const std::string& instanceName)
{
    QObject* vRet = nullptr;
    t_mapModels::iterator vIter = mModels.find(instanceName);
    if (vIter != mModels.end()) {
        vRet = vIter->second;
    }
    else {
        vRet = loadModel(aParent);
        if (vRet)
            mModels[instanceName] = vRet;
    }    
    return vRet;
}

void ModelFactory::ModelDescriptor::deleteModel(const std::string& instanceName)
{
    t_mapModels::iterator vIter = mModels.find(instanceName);
    if (vIter != mModels.end()) {
        mModels.erase(vIter);
    }
}