#ifndef MODELTS_H
#define MODELTS_H
#include "ModelVar.h"

class ModelTS : public ModelVar
{
public:
    ModelTS(const std::string& aName = "", const t_unit& aUnit = nullptr, const std::string& aDescription = "");
    ModelTS(const std::string& aName, ModelParam* ap_Variable);

    ~ModelTS();
   
    void setName(const std::string& a_Name);
    const std::string& getDescriptions() const { return m_Description; };

    std::string getUnit() const override;
    const UnitParam* pUnitParam() const override;

    bool IsUsed() const override;
    const FlagParam* pIsUsed() const override;

    const std::vector<double>* get_Values(size_t aNpdtPast = 0) const;
    void set_Values(uint aNpdtPast);
    void set_Values(size_t a_npdtTot, double a_Value);

    bool checkProfile();
    void setDefault(double a_Value);
    double getDefault() const { return m_default; };
   
    void subscribeTS(const std::string& a_exName, t_mapExchange& a_Import, size_t a_npdtTot);

protected:
    std::string m_Description;      
    double m_default;
    double m_min;
    double m_max;

    mutable std::vector<double> m_cachedValues;
    mutable const std::vector<double>* m_cachedSrc = nullptr;
    mutable size_t m_cachedOffset = 0;

    ModelParam* p_Variable{ nullptr };
    class ZEVariables* p_ZEVariable{ nullptr };
};

#endif