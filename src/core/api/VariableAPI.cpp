#include "CairnAPI.h"
#include "ModelVar.h"
#include "CairnAPIUtils.h"
using namespace CairnAPIUtils;


CairnAPI::VariableAPI::VariableAPI(std::shared_ptr < CairnAPI::ObjectAPI> ap_Parent, ModelIO* ap_Var)
{
	m_Parent = ap_Parent;
	m_Variable = ap_Var;
}

std::string CairnAPI::VariableAPI::get_Name() const
{
	if (m_Variable) {
		return m_Variable->getName();
	}
	return "";
}

std::string CairnAPI::VariableAPI::get_Description() const
{
	if (m_Variable) {
		return m_Variable->getDescription();
	}
	return "";
}

std::string CairnAPI::VariableAPI::get_Unit() const
{
	if (m_Variable) {
		return m_Variable->getUnit();
	}
	return "";
}

bool  CairnAPI::VariableAPI::isUsed() const
{
	if (m_Variable) {
		return m_Variable->IsUsed();
	}
	return true;
}

std::string CairnAPI::VariableAPI::get_Type() const
{
	if (m_Variable) {
		switch (m_Variable->getType())
		{
		case eMIPUndefined:       return "unknown";
		case eMIPExpression:          return "MIPExpression";
		case eMIPExpression1D:         return "MIPExpression1D";		
		default:            return "unknown";
		}
	}
	return "unknown";
}


t_value CairnAPI::VariableAPI::get_Value() const
{
	t_value vRet = NAN;
	if (m_Variable) {
		if (m_Variable->IsUsed())
			return m_Variable->getValue();
	}
	return vRet;
}