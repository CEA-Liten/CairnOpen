#ifndef CAIRNOBJECT_H
#define CAIRNOBJECT_H
#include "ModelInterface_global.h"
#include <algorithm>
#include <typeinfo>

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

    template<typename T>
    inline T* findChild(const std::string& a_Name = "") const
    {        
        for (auto& vChild : m_children) {            
            if (vChild->objectName() == a_Name) {
                if (vChild->objectType()!="") {                    
                    if (typeid(T).name() == ("class " + vChild->objectType()))
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
                if (typeid(T).name() == ("class " + vChild->objectType()))
                    vRet.push_back((T*)vChild);
            }            
        }
        return vRet;
    }
private:
	CairnObject* p_Parent{ nullptr };
    std::vector<CairnObject*> m_children;
	std::string m_Name;
    std::string m_Type;
};



#endif