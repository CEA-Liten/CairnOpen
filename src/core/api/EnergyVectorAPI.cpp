#include "CairnAPI.h"
#include "CairnCore.h"
#include "CairnAPIUtils.h"
using namespace CairnAPIUtils;

CairnAPI::EnergyVectorAPI::EnergyVectorAPI()
{	
}

CairnAPI::EnergyVectorAPI::EnergyVectorAPI(const std::string& a_Name, const std::string& a_Type)
	: CairnAPI::ObjectAPI(a_Name, a_Type)
{
}

void CairnAPI::EnergyVectorAPI::set_EnergyVector(EnergyVector* ap_EnergyVector)
{
	m_EnergyVector = ap_EnergyVector;
	if (ap_EnergyVector) {
		m_Name = ap_EnergyVector->Name().toStdString();
		m_Type = ap_EnergyVector->Type().toStdString();
	}
}

EnergyVector* CairnAPI::EnergyVectorAPI::get_EnergyVector() const
{
	return m_EnergyVector;
}

// -- Settings ---
// Returns the list of names 
t_list CairnAPI::EnergyVectorAPI::get_SettingsList(ESettingsLimited a_setLimited)
{
	t_list vRet;
	if (m_EnergyVector) {
		vRet = CairnAPIUtils::getParametersName({
			m_EnergyVector->getCompoInputParam(),
			m_EnergyVector->getCompoInputSettings(),
			m_EnergyVector->getTimeSeriesParam() }
		, a_setLimited);
	}
	return vRet;
}

// Returns the value of an existing setting
t_value CairnAPI::EnergyVectorAPI::get_SettingValue(const std::string& a_SettingName)
{	
	t_value vRet;
	if (m_EnergyVector) {
		vRet = CairnAPIUtils::getParameter({
			m_EnergyVector->getCompoInputParam(),
			m_EnergyVector->getCompoInputSettings(),
			m_EnergyVector->getTimeSeriesParam() }
		, a_SettingName);	
	}
	else {
		vRet = ObjectAPI::get_SettingValue(a_SettingName);
	}	
	return vRet;
}

t_dict CairnAPI::EnergyVectorAPI::get_SettingValues()
{
	t_dict vRet;
	if (m_EnergyVector) {
		CairnAPIUtils::getParameters({
			m_EnergyVector->getCompoInputParam(),
			m_EnergyVector->getCompoInputSettings(),
			m_EnergyVector->getTimeSeriesParam() }
			, vRet);		
	}
	else {
		vRet = ObjectAPI::get_SettingValues();
	}
	return vRet;
}

// Set the value of an existing setting
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
	else {
		ObjectAPI::set_SettingValue(a_SettingName, a_SettingValue);
	}
	CairnAPIUtils::setError(vRet);	
}

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
	else {
		ObjectAPI::set_SettingValues(a_SettingValues);
	}
	CairnAPIUtils::setError(vRet);	
}
