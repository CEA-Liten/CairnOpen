#include "CairnAPI.h"
#include "CairnCore.h"
#include "CairnAPIUtils.h"
using namespace CairnAPIUtils;

CairnAPI::MilpPortAPI::MilpPortAPI(MilpPort* ap_Port)
	: CairnAPI::ObjectAPI(ap_Port)
{	
}

MilpPort* CairnAPI::MilpPortAPI::get_MilpPort() const
{
	return (MilpPort*)get_Object();
}

CairnAPI::MilpPortAPI::MilpPortAPI(MilpComponentAPI& a_Component, const std::string& a_Name, const EnergyVectorAPI& a_EnergyVector,
	const std::string& a_Direction, const std::string& a_Variable)
	: CairnAPI::ObjectAPI()
{
	*this = a_Component.add_Port(a_Name, a_EnergyVector, a_Direction, a_Variable);
}

void CairnAPI::MilpPortAPI::set_MilpPort(MilpPort* ap_Port)
{
	set_Object(ap_Port);
}

std::string CairnAPI::MilpPortAPI::get_ID() const
{
	MilpPort* pPort = get_MilpPort();
	if (pPort) {
		return pPort->ID();
	}
	return "";
}

std::string CairnAPI::MilpPortAPI::get_CarrierName() const
{
	MilpPort* pPort = get_MilpPort();
	if (pPort && pPort->getCarrier()) {
		return pPort->getCarrier()->Name();
	}
	return "";
}

std::string CairnAPI::MilpPortAPI::get_Variable() const
{
	MilpPort* pPort = get_MilpPort();
	if (pPort) {
		return pPort->Variable();
	}
	return "";
}

// Set the value of a parameter
void CairnAPI::MilpPortAPI::set_SettingValue(const std::string& a_SettingName, const t_value& a_SettingValue, bool checkExistance)
{
	MilpPort* pPort = get_MilpPort(); 
	if (!pPort) { 
		CairnAPIUtils::setError(errParam, "Invalid port"); 
		return; 
	}

	// Check carrier requirement for default ports 
	if (pPort->IsDefaultPort() && !pPort->getCarrier()) { 
		CairnAPIUtils::setError(errDefault, "Please, configure port carrier first!"); 
		return; 
	}

	const bool vOk = CairnAPIUtils::setParameter( { pPort->getInputParam() }, a_SettingName, a_SettingValue);
	if(!vOk && checkExistance) {
		CairnAPIUtils::setError(errParam); 
		return;
	}

	// Handle special case for "Variable"
	if (a_SettingName == "Variable") { 
		/* 
		* Always declare although the variables are dynamic ?
		* Yes! because :
		* 1- if variable changed from 1D to 0D, the indicator should be removed
		* 2- how to check if the indicator already declared for non-default ports?
		* Improve ?!
		*/
		auto* pComponent = dynamic_cast<MilpComponent*>(pPort->parent());
		if (pComponent && pComponent->allDefaultPortsHaveVariables()) {
			if (auto* pModel = pComponent->compoModel()) {
				if (pModel->getMIPExpression1D(pPort->Variable())) { //only declare for 1D Expressions
					pComponent->declareIndicators();
				} 
			} 
		} 
	}

	CairnAPIUtils::setError(noError);
}

// Set the value of several parameter
void CairnAPI::MilpPortAPI::set_SettingValues(const t_dict& a_SettingValues)
{
	MilpPort* pPort = get_MilpPort();
	if (pPort) {
		for (auto& [vAttrName, vAttrValue] : a_SettingValues) {
			set_SettingValue(vAttrName, vAttrValue);
		}
	}
}

CairnAPI::EnergyVectorAPI CairnAPI::MilpPortAPI::get_EnergyCarrier()
{
	CairnAPI::EnergyVectorAPI vEnergyCarrier;
	MilpPort* pPort = get_MilpPort();
	if (pPort && pPort->getCarrier()) {
		vEnergyCarrier.set_EnergyVector(pPort->getCarrier());
	}
	return vEnergyCarrier;
}

void CairnAPI::MilpPortAPI::set_EnergyCarrier(const EnergyVectorAPI& a_EnergyVector)
{
	MilpPort* pPort = get_MilpPort();
	if (pPort) {
		pPort->setCarrier(a_EnergyVector.get_EnergyVector());
		if (pPort->IsDefaultPort()) {
			MilpComponent* lptrCompo = (MilpComponent*)pPort->parent();
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
