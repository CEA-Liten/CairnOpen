#if defined(WIN32) || defined(_WIN32)
#include <Windows.h>
#else
#include <dlfcn.h>
#endif

#include "ModelFactory.h"
#include <filesystem>
namespace fs = std::filesystem;

std::string ModelFactory::sModuleName = "createModel";
std::map<std::string, ModelFactory::ModelDescriptor> ModelFactory::m_PlugIns;
std::shared_ptr<spdlog::logger> ModelFactory::m_logger;

ModelFactory::ModelFactory(std::shared_ptr<spdlog::logger> default_logger) {
    spdlog::set_default_logger(default_logger); 
}

std::vector<std::string> ModelFactory::getModelList()
{
	std::vector<std::string> aModelList;
    t_mapPlugIns::iterator end = m_PlugIns.end();
    for (t_mapPlugIns::iterator it = m_PlugIns.begin(); it != end; it++) {
        aModelList.push_back(it->second.getModelName());
    }
	return aModelList;
}

CairnObject* ModelFactory::createModel(CairnObject* aParent, const std::string& modelName, const std::string& instanceName)
{
    t_mapPlugIns::iterator vIter = m_PlugIns.find(modelName);
    if (vIter != m_PlugIns.end()) {
        return vIter->second.createModel(aParent, instanceName);
    }
    else {        
        spdlog::warn("Cannot find model " + modelName + ", " + instanceName);
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
        spdlog::warn("Cannot find model " + modelName + ", " + instanceName);
    }
}

void ModelFactory::findModels(){
	// Search for models
    if (!lookupModels(fs::current_path().string())) {
        if (const char* env_p = std::getenv("CAIRN_BIN"))
            lookupModels(env_p);
        else {
            spdlog::critical("environment variable CAIRN_BIN does not exist!");
        }
    }
}

static bool ends_with(std::string_view str, std::string_view suffix)
{
    return str.size() >= suffix.size() && str.compare(str.size() - suffix.size(), suffix.size(), suffix) == 0;
}

static bool starts_with(std::string_view str, std::string_view prefix)
{
    return str.size() >= prefix.size() && str.compare(0, prefix.size(), prefix) == 0;
}

bool ModelFactory::lookupModels(const std::string& a_Path)
{
    bool vRet = false;
    std::string filterExt, filterStart = "", filterEnd = "CairnModel";
#if (defined (_WIN32) || defined (_WIN64))
    filterExt = ".dll";    
#else
    filterExt = ".so";    
    filterStart = "lib";
#endif
    spdlog::debug("Search models in: " + a_Path);
    fs::path vPath(a_Path);
    for (auto const& dir_entry : fs::directory_iterator{ vPath }) {
        if (!dir_entry.is_directory()) {
            const fs::path& vFile = dir_entry.path();
            if (vFile.extension() == filterExt) {
                std::string vModelName = vFile.stem().string();
                if (starts_with(vModelName, filterStart) && ends_with(vModelName, filterEnd) ) {                
                    size_t vPos = vModelName.rfind(filterEnd);                    
                        
                    ModelDescriptor modelDesc;
                    modelDesc.setDllPath(fs::absolute(vFile).string());
                    vModelName.replace(vPos, vPos + filterEnd.size(), "");
                    vModelName.replace(0, filterStart.size(), "");
                    modelDesc.setModelName(vModelName);
                    m_PlugIns[vModelName] = modelDesc;
                    spdlog::info("Found model "  + vModelName + "(" + fs::absolute(vFile).string() + ")");
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

CairnObject* ModelFactory::ModelDescriptor::loadModel(CairnObject* aParent)
{
    CairnObject* vRet = nullptr;
    spdlog::debug("Load Model" + mDLLAbsoluteName);
         
#if defined(WIN32) || defined(_WIN32)   
    HINSTANCE hGetProcIDDLL = LoadLibraryEx(mDLLAbsoluteName.c_str(), 0, LOAD_LIBRARY_SEARCH_DEFAULT_DIRS);
    if (!hGetProcIDDLL) {
        // 2ieme essai
        hGetProcIDDLL = LoadLibraryEx(mDLLAbsoluteName.c_str(), 0, LOAD_WITH_ALTERED_SEARCH_PATH);
    }    
#else   
    void* hGetProcIDDLL = dlopen((const char*)mDLLAbsoluteName.c_str(), RTLD_NOW);
    if (!hGetProcIDDLL) {
        spdlog::critical(dlerror());
    }
#endif
    if (!hGetProcIDDLL) {        
        spdlog::critical("could not load the dynamic library " + mDLLAbsoluteName);
        throw(std::exception_ptr());
    }
    typedef CairnObject* (*f_privateModel)(CairnObject* aParent);
    f_privateModel vFunct;

    // resolve function address here
#if defined(WIN32) || defined(_WIN32)
    vFunct = (f_privateModel)GetProcAddress(hGetProcIDDLL, sModuleName.c_str());
#else
    vFunct = (f_privateModel)dlsym(hGetProcIDDLL, sModuleName.c_str());
#endif

    if (!vFunct) {        
        spdlog::critical("could not locate the function createModel");
        throw(std::exception_ptr());
    }
    vRet = (*vFunct)(aParent);

    if (!vRet) {
        spdlog::critical("could not create the Model");
        throw(std::exception_ptr());
    }
    return vRet;
}

CairnObject* ModelFactory::ModelDescriptor::createModel(CairnObject* aParent, const std::string& instanceName)
{
    CairnObject* vRet = findModel(instanceName);    
    if (vRet == nullptr) {        
        vRet = loadModel(aParent);
        if (vRet)
            mModels.push_back(vRet);
    }    
    return vRet;
}

void ModelFactory::ModelDescriptor::deleteModel(const std::string& instanceName)
{
    std::vector<CairnObject*>::iterator vIter;
    for (vIter = mModels.begin(); vIter != mModels.end(); vIter++) {
        if ((*vIter)->parent()->objectName() == instanceName) {
            mModels.erase(vIter);
            break;
        }
    }    
}

CairnObject* ModelFactory::ModelDescriptor::findModel(const std::string& instanceName)
{
    CairnObject* vRet = nullptr;    
    for (auto& vModel : mModels) {
        if (vModel->parent()->objectName() == instanceName) {
            vRet = vModel;
            break;
        }
    }
    return vRet;
}