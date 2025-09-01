#pragma once
#include <exception>
#include <map>

#include "ModelInterface_global.h"

class MODELINTERFACESHARED_EXPORT ModelFactory
{
public:
    ModelFactory();
	
	static void findModels();
    static std::vector<std::string> getModelList();
    static QObject* createModel(QObject* aParent, const std::string& modelName, const std::string& instanceName);
    static void deleteModel(const std::string& modelName, const std::string& instanceName);

protected:
    class ModelDescriptor
    {
    public:
        ModelDescriptor();
        const std::string getModelName();
        void setModelName(const std::string aModelName);
        void setDllPath(const std::string& a_Path);
        QObject* createModel(QObject* aParent, const std::string& instanceName);
        void deleteModel(const std::string& instanceName);

    protected:        
        QObject* loadModel(QObject* aParent);

        typedef std::map<std::string, QObject*> t_mapModels;
        t_mapModels mModels;
        std::string mDLLAbsoluteName;
        std::string mModelName;
    };

    static bool lookupModels(const std::string& a_Path);
    typedef std::map<std::string, ModelDescriptor> t_mapPlugIns;
    static t_mapPlugIns m_PlugIns;
    static std::string sModuleName;
};
