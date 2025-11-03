#ifndef MODELTS_H
#define MODELTS_H
#include "ModelVar.h"

class ModelTS : public ModelVar
{
public:
    ModelTS(const std::string& aName = "", const UnitParam* a_Unit = nullptr);
    ModelTS(const std::string& aName, const UnitParam* a_Unit, InputParam::ModelParam* ap_Variable);
    ~ModelTS();
   
    void setName(const std::string& a_Name);
    const std::string& getDescriptions() const { return m_Comment; };
    void set_Values(uint aNpdtPast);
    void set_Values(size_t a_npdtTot, double a_Value);

    bool checkProfile();
    void setDefault(double a_Value);
    double getDefault() const { return m_default; };
   
    void subscribeTS(const std::string& a_exName, t_mapExchange& a_Import, size_t a_npdtTot);

protected:
    std::string m_Comment;      
    double m_default;
    double m_min;
    double m_max;

    InputParam::ModelParam* p_Variable{ nullptr };
    class ZEVariables* p_ZEVariable{ nullptr };
};

#endif