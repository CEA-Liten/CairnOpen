#include "CairnAPI.h"
#include "CairnObject.h"
#include "CairnAPIUtils.h"
using namespace CairnAPIUtils;

CairnAPI::ObjectAPI::ObjectAPI(CairnObject* ap_Object)
{
	set_Object(ap_Object);
}

CairnObject* CairnAPI::ObjectAPI::get_Object() const
{
	return m_Object;
}

void CairnAPI::ObjectAPI::set_Object(CairnObject* ap_Object)
{
	m_Object = ap_Object;
}

std::string CairnAPI::ObjectAPI::get_ObjectType() const
{
	if (m_Object) {
		return m_Object->objectType();
	}
	return "";
}

std::string CairnAPI::ObjectAPI::get_Name() const
{
	if (m_Object) {
		return m_Object->objectName();
	}
	return "";
}

void CairnAPI::ObjectAPI::rename(const std::string& name)
{
	if (m_Object) {
		m_Object->setObjectName(name);
	}
}

std::vector<InputParam*> CairnAPI::ObjectAPI::get_InputParams()
{
	if (m_Object) {
		std::vector<InputParam*> vInputParams = m_Object->get_InputParams();
		vInputParams.erase(
			std::remove(vInputParams.begin(), vInputParams.end(), nullptr),
			vInputParams.end()
		);
		return vInputParams;
	}
	else
		return{};
}

// Returns the list of parameter names 
t_list CairnAPI::ObjectAPI::get_SettingsList()
{
	/*
	* property in CairnBind.cpp
	*/
	return get_SettingsListByType(ESettingsLimited::all);
}
t_list CairnAPI::ObjectAPI::get_SettingsListByType(ESettingsLimited a_setLimited)
{	
	return CairnAPIUtils::getParametersName(get_InputParams(), a_setLimited);
}

// Returns the value of a parameter 
t_value CairnAPI::ObjectAPI::get_SettingValue(const std::string& a_SettingName)
{
	return CairnAPIUtils::getParameter(get_InputParams(), a_SettingName);
}

// Returns a dict of all parameter values
t_dict CairnAPI::ObjectAPI::get_SettingValues()
{	
	t_dict vRet = {};
	CairnAPIUtils::getParameters(get_InputParams(), vRet);
	return vRet;
}

// Set the value of a parameter
void CairnAPI::ObjectAPI::set_SettingValue(const std::string& a_SettingName, const t_value& a_SettingValue, bool checkExistance)
{
	ECodeError vRet = noError;
	if (m_Object) {
		bool vOk = CairnAPIUtils::setParameter(get_InputParams(), a_SettingName, a_SettingValue);		
		vRet = (vOk) ? noError : errParam;
	}
	CairnAPIUtils::setError(vRet);
}

// Set the values of several parameters
void CairnAPI::ObjectAPI::set_SettingValues(const t_dict& a_SettingValues)
{
	ECodeError vRet = noError;
	if (m_Object) {
		bool vOk = CairnAPIUtils::setParameters(get_InputParams(), a_SettingValues);
		vRet = (vOk) ? noError : errParam;
	}
	CairnAPIUtils::setError(vRet);
}

bool CairnAPI::ObjectAPI::get_SettingMandatoryValue(const std::string& a_SettingName)
{
	return CairnAPIUtils::getParamMandatoryValue(get_InputParams(), a_SettingName);
}

bool CairnAPI::ObjectAPI::is_DependentSetting(const std::string& a_SettingName)
{
	return CairnAPIUtils::isDependentParam(get_InputParams(), a_SettingName);	
}

std::string CairnAPI::ObjectAPI::get_SettingUnit(const std::string& a_SettingName)
{	
	return CairnAPIUtils::getParamUnit(get_InputParams(), a_SettingName);
}

std::string CairnAPI::ObjectAPI::get_SettingShowConfig(const std::string& a_SettingName)
{
	return CairnAPIUtils::getParamShowConfig(get_InputParams(), a_SettingName);
}

t_list CairnAPI::ObjectAPI::get_ShowConfigList()
{
	return CairnAPIUtils::getShowConfigList(get_InputParams());
}