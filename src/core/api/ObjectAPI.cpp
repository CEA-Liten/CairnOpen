#include "CairnAPI.h"
#include "CairnObject.h"
#include "CairnAPIUtils.h"
#include "InputParam.h"
#include "ModelParam.h"
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

std::vector<InputParam*> CairnAPI::ObjectAPI::get_InputParams(ESettingsCategory category)
{
	if (!m_Object)
		return {};

	std::vector<InputParam*> vInputParams;

	switch (category)
	{
	case eAll:
		vInputParams = m_Object->get_InputParams();
		break;
	case eParameters:
		vInputParams = m_Object->get_ParamInputParams();
		break;
	case eOptions:
		vInputParams = m_Object->get_OptionInputParams();
		break;
	case eTimeSeries:
		vInputParams = m_Object->get_TimeSeriesInputParams();
		break;
	case eEnvImpacts:
		vInputParams = m_Object->get_EnvImpactInputParams();
		break;
	case ePortEnvImpacts:
		vInputParams = m_Object->get_PortEnvImpactInputParams();
		break;
	}

	// Remove null pointers
	vInputParams.erase(
		std::remove(vInputParams.begin(), vInputParams.end(), nullptr),
		vInputParams.end()
	);

	return vInputParams;
}


t_list CairnAPI::ObjectAPI::get_PerfParamList() const
{
	if (!m_Object)
		return {};

	const InputParam* perfParam = m_Object->get_PerfParam();
	if (!perfParam)
		return {};

	t_list result;
	const auto& mapParams = perfParam->getMapParams();
	result.reserve(mapParams.size());

	std::transform(mapParams.cbegin(), mapParams.cend(), std::back_inserter(result),
		[](const auto& pair) { return pair.first; });

	return result;
}

t_list CairnAPI::ObjectAPI::get_VarList() const 
{
	if (!m_Object)
		return {};

	return m_Object->get_IOVarNames();
}

std::string CairnAPI::ObjectAPI::get_VarDescription(const std::string& varName) const
{
	if (!m_Object)
		return {};

	return m_Object->get_IOVarDescription(varName);
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

t_list CairnAPI::ObjectAPI::get_SettingsListByCategory(ESettingsCategory category)
{
	return CairnAPIUtils::getParametersName(get_InputParams(category), ESettingsLimited::all);
}

// Return a parameter
CairnAPI::ParamAPI CairnAPI::ObjectAPI::get_Setting(const std::string& a_SettingName)
{
	ParamAPI vRet;	
	for (auto& vInput : get_InputParams()) {
		if (vInput) {
			ModelParam* vParam = vInput->getParameter(a_SettingName);
			if (vParam) {
				return ParamAPI(shared_from_this(), vParam);
				break;
			}				
		}
	}
	return vRet;
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

// Returns the comment of a parameter 
std::string CairnAPI::ObjectAPI::get_SettingComment(const std::string& a_SettingName)
{
	return CairnAPIUtils::getParamComment(get_InputParams(), a_SettingName);
}

// Returns a dict of all parameter comments
t_dictComment CairnAPI::ObjectAPI::get_SettingComments()
{
	t_dictComment vRet = {};
	CairnAPIUtils::getParamComments(get_InputParams(), vRet);
	return vRet;
}

// Set the comment of a parameter
void CairnAPI::ObjectAPI::set_SettingComment(const std::string& a_SettingName, const std::string& a_SettingComment, 
	bool checkExistence)
{
	ECodeError vRet = noError;
	if (m_Object) {
		bool vOk = CairnAPIUtils::setParamComment(get_InputParams(), a_SettingName, a_SettingComment);
		vRet = (vOk || !checkExistence) ? noError : errParam;
	}
	CairnAPIUtils::setError(vRet);
}

// Set the comments of several parameters
void CairnAPI::ObjectAPI::set_SettingComments(const t_dictComment& a_SettingComments)
{
	for (const auto& [name, comment] : a_SettingComments) {
		set_SettingComment(name, comment, false);
	}
}

bool CairnAPI::ObjectAPI::is_MandatorySetting(const std::string& a_SettingName)
{
	return CairnAPIUtils::isMandatoryParam(get_InputParams(), a_SettingName);
}

bool CairnAPI::ObjectAPI::is_UsedSetting(const std::string& a_SettingName)
{
	return CairnAPIUtils::isUsedParam(get_InputParams(), a_SettingName);
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