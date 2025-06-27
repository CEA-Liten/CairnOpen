#pragma once
#include <exception>
#include <QString>
#include <map>

#include "ModelInterface_global.h"

class MODELINTERFACESHARED_EXPORT ModelFactory
{
public:
    ModelFactory();
	
	static void findModels();
    static QStringList getModelList();
    static QObject* createModel(QObject* aParent, const QString& modelName, const QString& instanceName);
    static void deleteModel(const QString& modelName, const QString& instanceName);

protected:
    class ModelDescriptor
    {
    public:
        ModelDescriptor();
        const QString getModelName();
        void setModelName(const QString aModelName);
        void setDllPath(const QString& a_Path);
        QObject* createModel(QObject* aParent, const QString& instanceName);
        void deleteModel(const QString& instanceName);

    protected:        
        QObject* loadModel(QObject* aParent);

        typedef std::map<QString, QObject*> t_mapModels;
        t_mapModels mModels;
        std::string mDLLAbsoluteName;
        QString mModelName;
    };

    static bool lookupModels(const QString& a_Path);
    typedef std::map<QString, ModelDescriptor> t_mapPlugIns;
    static t_mapPlugIns m_PlugIns;
    static std::string sModuleName;
};
