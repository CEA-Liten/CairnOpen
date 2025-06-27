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
std::map<QString, ModelFactory::ModelDescriptor> ModelFactory::m_PlugIns;

ModelFactory::ModelFactory() { }

QStringList ModelFactory::getModelList()
{
	QStringList aModelList;
    t_mapPlugIns::iterator end = m_PlugIns.end();
    for (t_mapPlugIns::iterator it = m_PlugIns.begin(); it != end; it++) {
        aModelList.push_back(it->second.getModelName());
    }
	return aModelList;
}

QObject* ModelFactory::createModel(QObject* aParent, const QString& modelName, const QString& instanceName)
{
    t_mapPlugIns::iterator vIter = m_PlugIns.find(modelName);
    if (vIter != m_PlugIns.end()) {
        return vIter->second.createModel(aParent, instanceName);
    }
    else {
        qWarning() << "Cannot find private model " << modelName << ", " << instanceName;
    }
    return nullptr;
}

void ModelFactory::deleteModel(const QString& modelName, const QString& instanceName)
{
    t_mapPlugIns::iterator vIter = m_PlugIns.find(modelName);
    if (vIter != m_PlugIns.end()) {
        return vIter->second.deleteModel(instanceName);
    }
    else {
        qWarning() << "Cannot find private model " << modelName << ", " << instanceName;
    }
}

void ModelFactory::findModels(){
	// Search for private models
    if (!lookupModels(QCoreApplication::applicationDirPath())) {
        QString appDir = qEnvironmentVariable("CAIRN_BIN", QDir::currentPath());
        lookupModels(appDir);
    }
}

bool ModelFactory::lookupModels(const QString& a_Path)
{
    bool vRet = false;
    QString filterExt, filterEnd = "CairnModel";
#if (defined (_WIN32) || defined (_WIN64))
    filterExt = ".dll";    
#else
    filterExt = ".so";    
#endif
    QDir vDir(a_Path, "*"+filterExt);
    qInfo() << "Search for models in: " << a_Path;
    QFileInfoList vFiles(vDir.entryInfoList());
    for (auto& vFile : vFiles) {
        if (vFile.baseName().endsWith(filterEnd)) {
            ModelDescriptor modelDesc;
            modelDesc.setDllPath(vFile.absoluteFilePath());
            QStringList pathCuts = vFile.absoluteFilePath().split("/");
            QString dllName = pathCuts.takeLast();
            QString aModelName = dllName;
            aModelName = aModelName.replace(filterExt, "").replace(filterEnd, "");
            modelDesc.setModelName(aModelName);
            m_PlugIns[aModelName] = modelDesc;
            qInfo() << "Found model " << aModelName << "(" << dllName << ")";
            vRet = true;
        }
    }
    return vRet;
}

//*************************************** ModelDescriptor *********************************************
ModelFactory::ModelDescriptor::ModelDescriptor()
{       
}

const QString ModelFactory::ModelDescriptor::getModelName()
{
    return mModelName;
}

void ModelFactory::ModelDescriptor::setModelName(const QString aModelName) 
{
    mModelName = aModelName;
}

void ModelFactory::ModelDescriptor::setDllPath(const QString& a_Path) 
{
    mDLLAbsoluteName = a_Path.toStdString();
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

QObject* ModelFactory::ModelDescriptor::createModel(QObject* aParent, const QString& instanceName)
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

void ModelFactory::ModelDescriptor::deleteModel(const QString& instanceName)
{
    t_mapModels::iterator vIter = mModels.find(instanceName);
    if (vIter != mModels.end()) {
        mModels.erase(vIter);
    }
}