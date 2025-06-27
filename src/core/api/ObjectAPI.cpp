#include "CairnAPI.h"
#include "CairnCore.h"
#include "CairnAPIUtils.h"
using namespace CairnAPIUtils;

CairnAPI::ObjectAPI::ObjectAPI()
{
}

CairnAPI::ObjectAPI::ObjectAPI(const std::string& a_Name, const std::string& a_Type)
{
	m_Name = a_Name;
	m_Type = a_Type;
}

const std::string& CairnAPI::ObjectAPI::get_Name() const
{
	return m_Name;
}

const std::string& CairnAPI::ObjectAPI::get_Type()
{
	return m_Type;
}

void CairnAPI::ObjectAPI::set_Type(const std::string& a_Type)
{
	m_Type = a_Type;
}

// Returns the value of an existing setting
t_value CairnAPI::ObjectAPI::get_SettingValue(const std::string& a_SettingName)
{
	t_dict::iterator vIter = m_Params.find(a_SettingName);
	if (vIter != m_Params.end()) {
		return m_Params[a_SettingName];
	}
	else {
		setError(errParam, a_SettingName);
		return "";
	}	
}

t_dict CairnAPI::ObjectAPI::get_SettingValues()
{	
	return m_Params;
}

// Set the value of an existing setting
void CairnAPI::ObjectAPI::set_SettingValue(const std::string& a_SettingName, const t_value& a_SettingValue)
{	
	m_Params[a_SettingName] = a_SettingValue;	
}

void CairnAPI::ObjectAPI::set_SettingValues(const t_dict& a_SettingValues)
{
	for (auto const& [key, val] : a_SettingValues)
		m_Params[key] = val;
}
