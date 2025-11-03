#pragma once
#include <exception>
#include <map>
#include <memory>
#include "ModelInterface_global.h"
#include "CairnObject.h"
#include "spdlog/spdlog.h"


class MODELINTERFACESHARED_EXPORT ModelFactory
{
public:
    ModelFactory(std::shared_ptr<spdlog::logger> default_logger);
	
	static void findModels();
    static std::vector<std::string> getModelList();
    static CairnObject* createModel(CairnObject* aParent, const std::string& modelName, const std::string& instanceName);
    static void deleteModel(const std::string& modelName, const std::string& instanceName);

protected:
    class ModelDescriptor
    {
    public:
        ModelDescriptor();
        const std::string getModelName();
        void setModelName(const std::string aModelName);
        void setDllPath(const std::string& a_Path);
        CairnObject* createModel(CairnObject* aParent, const std::string& instanceName);
        void deleteModel(const std::string& instanceName);

    protected:        
        CairnObject* loadModel(CairnObject* aParent);

        typedef std::map<std::string, CairnObject*> t_mapModels;
        t_mapModels mModels;
        std::string mDLLAbsoluteName;
        std::string mModelName;
    };

    static bool lookupModels(const std::string& a_Path);
    typedef std::map<std::string, ModelDescriptor> t_mapPlugIns;
    static t_mapPlugIns m_PlugIns;
    static std::string sModuleName;
    static std::shared_ptr<spdlog::logger> m_logger;
};
