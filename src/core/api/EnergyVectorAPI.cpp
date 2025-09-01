#include "CairnAPI.h"
#include "CairnCore.h"
#include "CairnAPIUtils.h"
using namespace CairnAPIUtils;

CairnAPI::EnergyVectorAPI::EnergyVectorAPI(EnergyVector* ap_EnergyVector)
{	
	set_EnergyVector(ap_EnergyVector);
}

CairnAPI::EnergyVectorAPI::EnergyVectorAPI(const OptimProblemAPI& a_Problem, 
	const std::string& a_Name, const std::string& a_Type)
{
	*this = a_Problem.create_EnergyCarrier(a_Name, a_Type);
}

EnergyVector* CairnAPI::EnergyVectorAPI::get_EnergyVector() const
{
	return m_EnergyVector;
}

void CairnAPI::EnergyVectorAPI::set_EnergyVector(EnergyVector* ap_EnergyVector)
{
	m_EnergyVector = ap_EnergyVector;
}

std::string CairnAPI::EnergyVectorAPI::get_Name() const
{
	if (m_EnergyVector) {
		return m_EnergyVector->Name().toStdString();
	}
	return "";
}

std::string CairnAPI::EnergyVectorAPI::get_Type() const
{
	if (m_EnergyVector) {
		return m_EnergyVector->Type().toStdString();
	}
	return "";
}

// Returns the list of parameter names 
t_list CairnAPI::EnergyVectorAPI::get_SettingsList()
{
	/*
	* property in CairnBind.cpp
	*/
	return get_SettingsListByType(ESettingsLimited::all);
}
t_list CairnAPI::EnergyVectorAPI::get_SettingsListByType(ESettingsLimited a_setLimited)
{
	t_list vRet = {};
	if (m_EnergyVector) {
		vRet = CairnAPIUtils::getParametersName({
			m_EnergyVector->getCompoInputParam(),
			m_EnergyVector->getCompoInputSettings(),
			m_EnergyVector->getTimeSeriesParam() }
		, a_setLimited);
	}
	return vRet;
}

// Returns the value of a parameter 
t_value CairnAPI::EnergyVectorAPI::get_SettingValue(const std::string& a_SettingName)
{	
	t_value vRet = "";
	if (m_EnergyVector) {
		vRet = CairnAPIUtils::getParameter({
			m_EnergyVector->getCompoInputParam(),
			m_EnergyVector->getCompoInputSettings(),
			m_EnergyVector->getTimeSeriesParam() }
		, a_SettingName);	
	}
	return vRet;
}

// Returns a dict of all parameter values
t_dict CairnAPI::EnergyVectorAPI::get_SettingValues()
{
	t_dict vRet = {};
	if (m_EnergyVector) {
		CairnAPIUtils::getParameters({
			m_EnergyVector->getCompoInputParam(),
			m_EnergyVector->getCompoInputSettings(),
			m_EnergyVector->getTimeSeriesParam() }
			, vRet);		
	}
	return vRet;
}

// Set the value of a parameter
void CairnAPI::EnergyVectorAPI::set_SettingValue(const std::string& a_SettingName, const t_value& a_SettingValue)
{
	ECodeError vRet = noError;
	if (m_EnergyVector) {
		bool vOk = CairnAPIUtils::setParameter({
			m_EnergyVector->getCompoInputParam(),
			m_EnergyVector->getCompoInputSettings(),
			m_EnergyVector->getTimeSeriesParam()
			}, a_SettingName, a_SettingValue);

		if (vOk) {
			vOk = m_EnergyVector->InitEnergyVectorParam();
		}
		vRet = (vOk) ? noError : errParam;
	}
	CairnAPIUtils::setError(vRet);	
}

// Set the values of several parameters
void CairnAPI::EnergyVectorAPI::set_SettingValues(const t_dict& a_SettingValues)
{
	ECodeError vRet = noError;
	if (m_EnergyVector) {				
		bool vOk = CairnAPIUtils::setParameters({ 
			m_EnergyVector->getCompoInputParam(), 
			m_EnergyVector->getCompoInputSettings(),
			m_EnergyVector->getTimeSeriesParam()
			}, a_SettingValues);

		if (vOk) {
			vOk = m_EnergyVector->InitEnergyVectorParam();
		}
		vRet = (vOk) ? noError : errParam;		
	}
	CairnAPIUtils::setError(vRet);	
}
