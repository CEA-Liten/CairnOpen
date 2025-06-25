#include "CairnAPI.h"
#include "CairnCore.h"
#include "BusCompo.h"
#include "CairnAPIUtils.h"
using namespace CairnAPIUtils;

CairnAPI::BusAPI::BusAPI()
{
}

CairnAPI::BusAPI::BusAPI(const std::string& a_Name, const std::string& a_Type, const std::string& a_ModelName, const EnergyVectorAPI& a_EnergyVector)
	: CairnAPI::ObjectAPI(a_Name, a_Type)
{
	set_ModelClass(a_ModelName);
	m_EnergyVector = a_EnergyVector.get_EnergyVector();
}

CairnAPI::BusAPI::BusAPI(const std::string& a_Name, const std::string& a_ModelName, const EnergyVectorAPI& a_EnergyVector)
	: CairnAPI::ObjectAPI(a_Name, "Unknown")
{
	t_list possibleTypes = CairnAPIUtils::get_Possible_Component_Types();
	std::string type = CairnAPIUtils::get_Component_Type(a_ModelName);
	if (std::find(possibleTypes.begin(), possibleTypes.end(), type) == possibleTypes.end()) {
		CairnAPIUtils::setError(errDefault, "Something went wrong! The type of " + a_ModelName + " is unknown!\n"
			+ "Please, try to provide the component type e.g. Bus(" + a_Name+ ", a_BusType, " + a_ModelName + ", a_EnergyVector)");
	}
	set_Type(type);
	set_ModelClass(a_ModelName);
	m_EnergyVector = a_EnergyVector.get_EnergyVector();
}

void CairnAPI::BusAPI::set_Bus(BusCompo* ap_Bus)
{
	m_Bus = ap_Bus;
	if (m_Bus) {
		m_Name = m_Bus->Name().toStdString();
		m_Type = m_Bus->Type().toStdString();
		m_ModelClass = m_Bus->ModelClassName().toStdString();
		if (!m_EnergyVector) {
			m_EnergyVector = m_Bus->getEnergyVector();
		}
		if (!m_EnergyVector) {
			CairnAPIUtils::setError(errDefault, "The EnergyVector of Bus " + m_Name + " doesn't exist");
		}
		m_Bus->setEnergyVector(m_EnergyVector);
	}
}

void CairnAPI::BusAPI::set_ModelClass(const std::string& a_ModelName)
{
	m_ModelClass = a_ModelName;
}

const std::string CairnAPI::BusAPI::get_ModelClass()
{
	return m_ModelClass;
}

std::string CairnAPI::BusAPI::get_EnergyVector() const
{
	if (m_EnergyVector)
		return m_EnergyVector->Name().toStdString();
	else
		return "";
}

BusCompo* CairnAPI::BusAPI::get_BusCompo() const
{
	return m_Bus;
}



// -- Settings ---
// Returns the list of names 
t_list CairnAPI::BusAPI::get_SettingsList(ESettingsLimited a_setLimited)
{
	t_list vRet;
	if (m_Bus) {
		vRet = CairnAPIUtils::getParametersName({
			m_Bus->compoModel()->getInputParam(), // params	
			m_Bus->getCompoInputParam(),
			m_Bus->compoModel()->getInputDataTS() // timeseries	
			}
		, a_setLimited);
	}
	return vRet;
}

// Returns the value of an existing setting
t_value CairnAPI::BusAPI::get_SettingValue(const std::string& a_SettingName)
{
	t_value vRet;
	if (m_Bus) {
		vRet = CairnAPIUtils::getParameter({
			m_Bus->compoModel()->getInputParam(), // params	
			m_Bus->getCompoInputParam(),
			m_Bus->compoModel()->getInputDataTS() // timeseries	
			}
		, a_SettingName);
	}
	else {
		ObjectAPI::get_SettingValue(a_SettingName);
	}
	return vRet;
}

t_dict CairnAPI::BusAPI::get_SettingValues()
{
	t_dict vRet;
	if (m_Bus) {
		CairnAPIUtils::getParameters({
			m_Bus->compoModel()->getInputParam(), // params	
			m_Bus->getCompoInputParam(), // options
			m_Bus->compoModel()->getInputDataTS() // timeseries	
			}
		, vRet);
	}
	else {
		vRet = ObjectAPI::get_SettingValues();
	}
	return vRet;
}

// Set the value of an existing setting
void CairnAPI::BusAPI::set_SettingValue(const std::string& a_SettingName, const t_value& a_SettingValue)
{
	ECodeError vRet = noError;
	if (m_Bus) {
		bool vOk = CairnAPIUtils::setParameter({
			m_Bus->compoModel()->getInputParam(), // params	
			m_Bus->getCompoInputParam(), // options
			m_Bus->compoModel()->getInputDataTS() // timeseries			
			}
		, a_SettingName, a_SettingValue);

		vRet = (vOk) ? noError : errParam;
	}
	else {
		ObjectAPI::set_SettingValue(a_SettingName, a_SettingValue);
	}
	CairnAPIUtils::setError(vRet);
}

void CairnAPI::BusAPI::set_SettingValues(const t_dict& a_SettingValues)
{
	ECodeError vRet = noError;
	if (m_Bus) {
		bool vOk = CairnAPIUtils::setParameters({
			m_Bus->compoModel()->getInputParam(), // params	
			m_Bus->getCompoInputParam(), // options
			m_Bus->compoModel()->getInputDataTS() // timeseries
			 }
		, a_SettingValues);

		vRet = (vOk) ? noError : errParam;
	}
	else {
		ObjectAPI::set_SettingValues(a_SettingValues);
	}
	CairnAPIUtils::setError(vRet);
}
