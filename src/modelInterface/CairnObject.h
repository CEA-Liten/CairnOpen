#ifndef CAIRNOBJECT_H
#define CAIRNOBJECT_H
#include "ModelInterface_global.h"
#include "spdlog/spdlog.h"

class MODELINTERFACESHARED_EXPORT CairnObject
{
public:
	CairnObject(CairnObject* ap_Parent, const std::string& a_Name="");
    ~CairnObject();
    void setObjectName(const std::string& a_Name);
    const std::string &objectName() const;
    void setObjectType(const std::string& a_Type);
    const std::string& objectType() const;
    CairnObject* parent() const{ return p_Parent; } ;

    void addChild(CairnObject* ap_Child);      
    void removeChild(CairnObject* ap_Child);
    CairnObject* findChild(const std::string& a_Name, const std::string& a_Type = "");
    static bool ends_with(std::string_view str, std::string_view suffix)
    {
        return str.size() >= suffix.size() && str.compare(str.size() - suffix.size(), suffix.size(), suffix) == 0;
    }
    template<typename T>
    inline T* findChild(const std::string& a_Name = "") const
    {                
        for (auto& vChild : m_children) {                        
            if (vChild->objectType()!="") { 
                if (a_Name == "" || vChild->objectName() == a_Name) {
                    if (ends_with(typeid(T).name(), vChild->objectType()))
                        return (T*)vChild;
                }                    
            }                        
        }
        return nullptr;
    }

    template<typename T>
    inline std::vector<T*> findChildren() const
    {
        std::vector<T*> vRet;
        for (auto& vChild : m_children) {
            if (vChild->objectType() != "") {                
                if (ends_with(typeid(T).name(), vChild->objectType()))
                    vRet.push_back((T*)vChild);
            }            
        }
        return vRet;
    }

    const std::vector<CairnObject*>& children() const;
    std::vector<std::string> childrenNames() const;

    virtual std::vector<class InputParam*> get_InputParams() { return {}; };

    virtual std::vector<class InputParam*> get_ParamInputParams() { return {}; };
    virtual std::vector<class InputParam*> get_OptionInputParams() { return {}; };
    virtual std::vector<class InputParam*> get_TimeSeriesInputParams() { return {}; };
    virtual std::vector<class InputParam*> get_EnvImpactInputParams() { return {}; };
    virtual std::vector<class InputParam*> get_PortEnvImpactInputParams() { return {}; };
    virtual class InputParam* get_PerfParam() { return nullptr; };

    virtual std::vector<std::string> get_IOVarNames() const { return {}; } //t_list
    virtual std::string get_IOVarDescription(const std::string& varName) const { return {}; };

private:
	CairnObject* p_Parent{ nullptr };
    std::vector<CairnObject*> m_children;
	std::string m_Name;
    std::string m_Type;
};

#endif