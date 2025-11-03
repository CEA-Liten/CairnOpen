
#include "FlagParam.h"

FlagParam::FlagParam()
{
    m_Value = false;
    p_Value = nullptr;
    m_Function.Type = eFTypeUndefined;
}

void FlagParam::set_Value(t_flag a_Flag)
{
    if (const bool* pval = std::get_if<bool>(&a_Flag)) {
        m_Value = *pval;
    }
    else if (bool** pval = std::get_if<bool*>(&a_Flag)) {
        p_Value = *pval;
    }
    else if (SFunctionFlag* pval = std::get_if<SFunctionFlag>(&a_Flag)) {
        m_Function.Type = pval->Type;
        m_Function.Flags.assign(pval->Flags.begin(), pval->Flags.end());
        m_Function.Flags2.assign(pval->Flags2.begin(), pval->Flags2.end());
        m_Function.ExtFunct = pval->ExtFunct;
    }
    else if (SExtFunctionFlag* pval = std::get_if<SExtFunctionFlag>(&a_Flag)) {
        m_ExtFunct.pFunct = pval->pFunct;
        m_ExtFunct.pModel = pval->pModel;
    }
}

bool FlagParam::get_Value()
{
    if (m_ExtFunct.pModel && m_ExtFunct.pFunct) {
        return (*m_ExtFunct.pFunct)(m_ExtFunct.pModel);
    }
    else if (m_Function.Type == eFTypeUndefined) {
        if (p_Value)
            return *p_Value;
        else
            return m_Value;
    }
    else {
        bool vRet = false;
        switch (m_Function.Type)
        {
        case eFTypeOrNot:
            for (auto& vFlag : m_Function.Flags) {
                vRet |= *vFlag;
            }
            for (auto& vFlag : m_Function.Flags2) {
                vRet |= !*vFlag;
            }
            break;
        case eFTypeNotAnd:
            vRet = true;
            for (auto& vFlag : m_Function.Flags) {
                vRet &= !*vFlag;
            }
            for (auto& vFlag : m_Function.Flags2) {
                vRet &= *vFlag;
            }
            break;
        case eFTypeNotAndOr:
            for (auto& vFlag : m_Function.Flags2) {
                vRet |= *vFlag;
            }
            for (auto& vFlag : m_Function.Flags) {
                vRet &= !*vFlag;
            }
            break;
        default:
            break;
        }
        if (m_Function.ExtFunct.pFunct && m_Function.ExtFunct.pModel) {
            vRet &= (*m_Function.ExtFunct.pFunct)(m_Function.ExtFunct.pModel);
        }
        return vRet;
    }
}

bool FlagParam::is_Scalar() {
    if (p_Value || m_ExtFunct.pModel
            || m_Function.ExtFunct.pModel || m_Function.Flags.size() || m_Function.Flags2.size()
        ) {
        return false;
    }
    return true;
}