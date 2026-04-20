#include "CairnAPI.h"
#include "InputParam.h"
#include "CairnAPIUtils.h"
using namespace CairnAPIUtils;



CairnAPI::ParamAPI::ParamAPI(CairnAPI::ObjectAPI* ap_Parent, ModelParam* ap_Param)
{
	m_Parent = ap_Parent;
	m_Param = ap_Param;
}

std::string CairnAPI::ParamAPI::get_Name() const
{
	if (m_Param) {
		return m_Param->getName();
	}
	return "";
}

std::string CairnAPI::ParamAPI::get_Type() const
{
	if (m_Param) {
		switch (m_Param->getType())
		{
		case eDouble:       return "double";
		case eInt:          return "int";
		case eBool:         return "bool";
		case eString:       return "string";
		case eStringList:   return "stringlist";
		case eVectorDouble: return "vector";
		case eVectorEigen:  return "vector";
		default:            return "unknown";
		}
	}
	return "string";
}

std::string CairnAPI::ParamAPI::get_Description() const
{
	if (m_Param) {
		return m_Param->getDescription();
	}
	return "";
}

std::string CairnAPI::ParamAPI::get_Unit() const
{
	if (m_Param) {
		return m_Param->getUnit();
	}
	return "";
}

t_value CairnAPI::ParamAPI::get_Value() const
{
	if (m_Param) {
		return m_Param->getValue();
	}
	return "";
}

std::string CairnAPI::ParamAPI::get_StrValue() const
{
	if (m_Param) {
		return m_Param->toString();
	}
	return "";
}

void CairnAPI::ParamAPI::set_Value(const t_value& a_SettingValue)
{	
	if (m_Param && m_Parent) {
		m_Parent->set_SettingValue(get_Name(), a_SettingValue);		
	}	
}

t_value CairnAPI::ParamAPI::get_Default() const
{
	if (m_Param) {
		return m_Param->getDefault();
	}
	return "";
}

std::string CairnAPI::ParamAPI::get_StrDefaultValue() const
{
	if (m_Param) {
		if (auto strValue = m_Param->getStrDefaultValue()) {
			return *strValue;
		}
		else {
			// Default value is a vector, can't convert to string
			return "";
		}
	}
	return "";
}

t_value CairnAPI::ParamAPI::get_Min() const
{
	if (m_Param) {
		return m_Param->getMin();
	}
	return std::nan("1");
}

t_value CairnAPI::ParamAPI::get_Max() const
{
	if (m_Param) {
		return m_Param->getMax();
	}
	return std::nan("1");
}

bool  CairnAPI::ParamAPI::isMandatory() const
{
	if (m_Param) {
		return m_Param->IsBlocking();
	}
	return false;
}

bool CairnAPI::ParamAPI::isDependent() const
{
	if (m_Param) {
		return m_Param->isDependent();
	}
	return false;
}

bool  CairnAPI::ParamAPI::isUsed() const
{
	if (m_Param) {
		return m_Param->IsUsed();
	}
	return true;
}

std::string CairnAPI::ParamAPI::getShowConfig() const
{
	if (m_Param) {
		return m_Param->getShowConfig();
	}
	return "";
}

std::string CairnAPI::ParamAPI::get_Comment() const
{
	if (m_Param) {
		return m_Param->getComment();
	}
	return "";
}

void CairnAPI::ParamAPI::set_Comment(const std::string& a_SettingComment)
{
	if (m_Param && m_Parent) {
		m_Parent->set_SettingComment(get_Name(), a_SettingComment);
	}
}