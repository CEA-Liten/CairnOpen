#include "CairnAPI.h"
#include "CairnCore.h"
#include "CairnAPIUtils.h"
using namespace CairnAPIUtils;

CairnAPI::MilpPortAPI::MilpPortAPI()
{
}

CairnAPI::MilpPortAPI::MilpPortAPI(const std::string& a_Name, const EnergyVectorAPI& a_EnergyVector)
	: CairnAPI::ObjectAPI(a_Name, a_EnergyVector.get_Name())
{
	m_EnergyVector = a_EnergyVector.get_EnergyVector();
}

void CairnAPI::MilpPortAPI::set_Port(MilpPort* ap_Port)
{
	m_Port = ap_Port;
	if (ap_Port) {
		m_Name = ap_Port->Name().toStdString();
		if (!m_EnergyVector) {
			m_EnergyVector = ap_Port->ptrEnergyVector();
		}
		if (!m_EnergyVector) {
			CairnAPIUtils::setError(errAdd, "port " + m_Name + " because EnergyVector doesn't exist");
		}
		m_Type = get_EnergyVector();
		ap_Port->setEnergyVector(m_EnergyVector);
	}	
}

std::string CairnAPI::MilpPortAPI::get_EnergyVector() const
{
	if (m_EnergyVector)
		return m_EnergyVector->Name().toStdString();
	else
		return "";	
}

MilpPort* CairnAPI::MilpPortAPI::get_MilpPort() const
{
	return m_Port;
}


// -- Settings ---
// Returns the list of names 
t_list CairnAPI::MilpPortAPI::get_SettingsList(ESettingsLimited a_setLimited)
{
	t_list vRet;	
	if (m_Port) {
		vRet = CairnAPIUtils::getParametersName({
			m_Port->getInputParam() }
		, a_setLimited);
	}
	return vRet;
}

// Returns the value of an existing setting
t_value CairnAPI::MilpPortAPI::get_SettingValue(const std::string& a_SettingName)
{
	t_value vRet;
	if (m_Port) {		
		vRet = CairnAPIUtils::getParameter({
			m_Port->getInputParam() }
		, a_SettingName);
	}
	else {
		vRet = ObjectAPI::get_SettingValue(a_SettingName);
	}
	return vRet;
}

t_dict CairnAPI::MilpPortAPI::get_SettingValues()
{
	t_dict vRet;
	if (m_Port) {
		t_list vAttrNames = get_SettingsList();
		for (auto& vAttrName : vAttrNames) {
			vRet[vAttrName] = get_SettingValue(vAttrName);
		}
	}
	else {
		vRet = ObjectAPI::get_SettingValues();
	}
	return vRet;
}


// Set the value of an existing setting
void CairnAPI::MilpPortAPI::set_SettingValue(const std::string& a_SettingName, const t_value& a_SettingValue)
{
	ECodeError vRet = noError;
	if (m_Port) {
		bool vOk = CairnAPIUtils::setParameter({
			m_Port->getInputParam() }
		, a_SettingName, a_SettingValue);

		vRet = (vOk) ? noError : errParam;
	}
	else {
		ObjectAPI::set_SettingValue(a_SettingName, a_SettingValue);
	}
	CairnAPIUtils::setError(vRet);
}

void CairnAPI::MilpPortAPI::set_SettingValues(const t_dict& a_SettingValues)
{
	ECodeError vRet = noError;
	if (m_Port) {
		for (auto& [vAttrName, vAttrValue] : a_SettingValues) {
			set_SettingValue(vAttrName, vAttrValue);
		}
	}
	else {
		ObjectAPI::set_SettingValues(a_SettingValues);
	}
	CairnAPIUtils::setError(vRet);
}

bool CairnAPI::MilpPortAPI::checkSettings(std::string& a_ErrMsg)
{
	bool vRet = false;
	t_list vMandatories = get_SettingsList(ESettingsLimited::mandatory);
	for (const auto& [key, value] : m_Params) {
		t_list::iterator vIter = find(vMandatories.begin(), vMandatories.end(), key);
		if (vIter != vMandatories.end()) {
			vMandatories.erase(vIter);
		}		
	}
	if (vMandatories.size()) {
		for (auto& vParam : vMandatories) {
			// messages
			a_ErrMsg += " - mandatory setting " + vParam + " is missing!";
		}
		return false;
	}		
	return true;
}
