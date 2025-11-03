#include "CairnAPI.h"
#include "CairnCore.h"
#include "CairnAPIUtils.h"
using namespace CairnAPIUtils;

CairnAPI::MilpPortAPI::MilpPortAPI(MilpPort* ap_Port)
{
	set_MilpPort(ap_Port);
}


MilpPort* CairnAPI::MilpPortAPI::get_MilpPort() const
{
	return m_Port;
}

CairnAPI::MilpPortAPI::MilpPortAPI(MilpComponentAPI& a_Component, const std::string& a_Name, const EnergyVectorAPI& a_EnergyVector,
	const std::string& a_Direction, const std::string& a_Variable)
{
	*this = a_Component.add_Port(a_Name, a_EnergyVector, a_Direction, a_Variable);
}

void CairnAPI::MilpPortAPI::set_MilpPort(MilpPort* ap_Port)
{
	m_Port = ap_Port;
}

std::string CairnAPI::MilpPortAPI::get_ID() const
{
	if (m_Port) {
		return m_Port->ID();
	}
	return "";
}

std::string CairnAPI::MilpPortAPI::get_Name() const
{
	if (m_Port) {
		return m_Port->Name();
	}
	return "";
}


void CairnAPI::MilpPortAPI::rename(const std::string& name)
{
	if (m_Port) {
		m_Port->setName(name);
	}
}

std::string CairnAPI::MilpPortAPI::get_CarrierName() const
{
	if (m_Port && m_Port->getCarrier()) {
		return m_Port->getCarrier()->Name();
	}
	return "";
}

CairnAPI::EnergyVectorAPI CairnAPI::MilpPortAPI::get_EnergyCarrier()
{
	CairnAPI::EnergyVectorAPI vEnergyCarrier;
	if (m_Port && m_Port->getCarrier()) {
		vEnergyCarrier.set_EnergyVector(m_Port->getCarrier());
	}
	return vEnergyCarrier;
}

void CairnAPI::MilpPortAPI::set_EnergyCarrier(const EnergyVectorAPI& a_EnergyVector)
{
	if (m_Port) {
		m_Port->setCarrier(a_EnergyVector.get_EnergyVector());
		if (m_Port->IsDefaultPort()) {
			MilpComponent* lptrCompo = (MilpComponent*)m_Port->parent();
			if (lptrCompo && lptrCompo->allDefaultPortsHaveCarriers()
				&& lptrCompo->compoModel() 
				&& lptrCompo->compoModel()->getInputParam()->getMapParams().size() == 0 //only once. TODO: use a more robust condition
				&& !lptrCompo->getMainCarrier())
			{
				/*
				* Declare parametersand IOs only after EnergyVectors of all default ports are set
				* Should be called only once (first EnergyVector set). 
				* Should not be called if the EnergyVector is changed later on.
				* Otherwise, parameter values should be saved.
				*/
				lptrCompo->initSubModelConfiguration(false);
			}
		}
	}
}

// ------------------ Parameters ------------------
// Returns the list of parameter names 

t_list CairnAPI::MilpPortAPI::get_SettingsList()
{
 /*
  * property in CairnBind.cpp
 */
	return get_SettingsListByType(ESettingsLimited::all);
}

t_list CairnAPI::MilpPortAPI::get_SettingsListByType(ESettingsLimited a_setLimited)
{
	t_list vRet = {};
	if (m_Port) {
		vRet = CairnAPIUtils::getParametersName({
			m_Port->getInputParam() }
		, a_setLimited);
	}
	return vRet;
}

// Returns the value of a parameter
t_value CairnAPI::MilpPortAPI::get_SettingValue(const std::string& a_SettingName)
{
	t_value vRet = "";
	if (m_Port) {		
		vRet = CairnAPIUtils::getParameter({
			m_Port->getInputParam() }
		, a_SettingName);
	}
	return vRet;
}

// Returns the value of all parameters
t_dict CairnAPI::MilpPortAPI::get_SettingValues()
{
	t_dict vRet = {};
	if (m_Port) {
		t_list vAttrNames = get_SettingsList();
		for (auto& vAttrName : vAttrNames) {
			vRet[vAttrName] = get_SettingValue(vAttrName);
		}
	}
	return vRet;
}


// Set the value of a parameter
void CairnAPI::MilpPortAPI::set_SettingValue(const std::string& a_SettingName, const t_value& a_SettingValue)
{
	ECodeError vRet = noError;
	if (m_Port) {
		bool vOk = CairnAPIUtils::setParameter({
			m_Port->getInputParam() }
		, a_SettingName, a_SettingValue);

		vRet = (vOk) ? noError : errParam;
	}
	CairnAPIUtils::setError(vRet);
}

// Set the value of several parameter
void CairnAPI::MilpPortAPI::set_SettingValues(const t_dict& a_SettingValues)
{
	ECodeError vRet = noError;
	if (m_Port) {
		for (auto& [vAttrName, vAttrValue] : a_SettingValues) {
			set_SettingValue(vAttrName, vAttrValue);
		}
	}
	CairnAPIUtils::setError(vRet);
}
