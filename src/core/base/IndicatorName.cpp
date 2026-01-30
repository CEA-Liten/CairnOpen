/*
* \file		IndicatorName.cpp
* \brief	A class for a descriptive indicator name
* \version	1.0
* \author	Ali KASSEM
* \date		18/12/2025
*/

#include "IndicatorName.h"

IndicatorName::IndicatorName()
{
    m_Value = "";
    p_Value = nullptr;
}

void IndicatorName::set_Value(t_Name a_Name)
{
    if (const std::string* pval = std::get_if<std::string>(&a_Name)) {
        m_Value = *pval;
    }
    else if (const std::string* const* pval = std::get_if<const std::string*>(&a_Name)) {
        p_Value = *pval;
    }
    else if (const SExtFunctionName* pval = std::get_if<SExtFunctionName>(&a_Name)) {
        m_ExtFunct.pModel = pval->pModel;
        m_ExtFunct.pPort = pval->pPort;
        m_ExtFunct.pFunct = pval->pFunct;
        m_ExtFunct.nameParts = pval->nameParts;
    }
}

std::string IndicatorName::get_Value() const
{
    if (m_ExtFunct.pFunct) {// && m_ExtFunct.pModel && m_ExtFunct.pPort
        return (*m_ExtFunct.pFunct)(m_ExtFunct.pModel, m_ExtFunct.pPort, m_ExtFunct.nameParts);
    }
    else if (p_Value) {
        return *p_Value;
    }
    else {
        return m_Value;
    }
}
