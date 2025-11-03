
#include "UnitParam.h"

UnitParam::UnitParam()
{
    m_Value = "";
    p_Value = nullptr;
    m_Function.Type = eFTypeUnknown;
}

void UnitParam::set_Value(t_unit a_Unit)
{
    if (const std::string* pval = std::get_if<std::string>(&a_Unit)) 
    {
        m_Value = *pval;
    }
    else if(const std::string* const* ppval = std::get_if<const std::string*>(&a_Unit))
    {
        p_Value = *ppval;
    }
    else if (const SFunctionUnit* pval = std::get_if<SFunctionUnit>(&a_Unit)) 
    {
        m_Function.Type = pval->Type;
        m_Function.Units.assign(pval->Units.begin(), pval->Units.end());
        m_Function.suffixUnit = pval->suffixUnit;
        m_Function.prefixUnit = pval->prefixUnit;
    }
}

std::string UnitParam::get_Value() const
{
    std::string vRet;
    if (m_Function.Type == eFTypeUnknown) {
        if (p_Value) {
            vRet = *p_Value; //dynamic unit
        }
        else {
            vRet = m_Value; //scalar unit
        }
    }
    else { 
        //composite unit
        vRet = m_Function.prefixUnit;
        switch (m_Function.Type)
        {
            case eFTypeDivision:
                for (auto& vUnit : m_Function.Units) 
                {
                    if (vUnit  == nullptr || *vUnit == "" || *vUnit == "-") continue; //for example, case of WeightUnit
                    if (vRet != "") vRet += "/";
                    vRet += *vUnit;
                }
                if (m_Function.suffixUnit != "") {
                    if (vRet != "") vRet += "/";
                    vRet += m_Function.suffixUnit;
                }
                break;
            case eFTypeMultiplication:
                for (auto& vUnit : m_Function.Units) 
                {
                    if (vUnit == nullptr || *vUnit == "" || *vUnit == "-") continue;
                    if (vRet != "") vRet += ".";
                    vRet += *vUnit;
                }
                if (m_Function.suffixUnit != "") {
                    if (vRet != "") vRet += ".";
                    vRet += m_Function.suffixUnit;
                }
                break;
            default:
                break;
        }
    }
    //if (vRet == "") vRet = "-";
    return vRet;
}
