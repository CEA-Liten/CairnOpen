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
	*this = *a_Component.add_Port(a_Name, a_EnergyVector, a_Direction, a_Variable);
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

	const bool vOk = CairnAPIUtils::setParameter( pPort->get_InputParams() , a_SettingName, a_SettingValue);
	if(!vOk && checkExistance) {
		CairnAPIUtils::setError(errParam); 
		return;
	}

	auto* pComponent = dynamic_cast<MilpComponent*>(pPort->parent());

	if (pComponent) {
		// Handle special case for "Variable"
		if (a_SettingName == "Variable") {
			/*
			* Always declare although the variables are dynamic ?
			* Yes! because :
			* 1- if variable changed from 1D to 0D, the indicator should be removed
			* 2- how to check if the indicator already declared for non-default ports?
			* Improve ?!
			*/
			if (pComponent->allDefaultPortsHaveVariables()) {
				if (auto* pModel = pComponent->compoModel()) {
					if (pModel->getMIPExpression1D(pPort->Variable())) { //only declare for 1D Expressions
						pComponent->declareIndicators();
					}
				}
			}
		}

		/* Update MilpComponent::mPorts as it is used to re-initialize the port parameters on run() */
		const std::string value = CairnAPIUtils::getParamValue(a_SettingValue);
		pComponent->updatePortParamMap(pPort->ID(), pPort->Name(), a_SettingName, "value", value);
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

// Set the comment of a comment
void CairnAPI::MilpPortAPI::set_SettingComment(const std::string& a_SettingName, const std::string& a_SettingComment, 
	bool checkExistence)
{
	ECodeError vRet = noError;
	if (m_Object) {
		try {
			CairnAPI::ObjectAPI::set_SettingComment(a_SettingName, a_SettingComment, checkExistence);
			MilpPort* pPort = (MilpPort*)m_Object;
			auto* pComponent = dynamic_cast<MilpComponent*>(pPort->parent());
			if (pComponent && pPort) {
				pComponent->updatePortParamMap(pPort->ID(), pPort->Name(), a_SettingName, "comment", a_SettingComment);
			}
		}
		catch (const std::exception&) {
			vRet = errParam;
		}
	}
	CairnAPIUtils::setError(vRet);
}

std::shared_ptr < CairnAPI::EnergyVectorAPI> CairnAPI::MilpPortAPI::get_EnergyCarrier()
{
	std::shared_ptr < CairnAPI::EnergyVectorAPI> vEnergyCarrier;
	MilpPort* pPort = get_MilpPort();
	if (pPort && pPort->getCarrier()) {
		vEnergyCarrier = std::make_shared<EnergyVectorAPI>(pPort->getCarrier());
	}
	return vEnergyCarrier;
}

bool CairnAPI::MilpPortAPI::set_EnergyCarrier(const EnergyVectorAPI& a_EnergyVector)
{
	bool configured = false;
	MilpPort* pPort = get_MilpPort();
	if (pPort) {
		pPort->setCarrier(a_EnergyVector.get_EnergyVector());
		configured = configureParentComponent();
	}
	return configured; // Might be confusing because the return value might be false even if the carrier is correctly set!
}

bool CairnAPI::MilpPortAPI::configureParentComponent() {
	MilpPort* pPort = get_MilpPort();
	if (!pPort) return false;

	if (!pPort->IsDefaultPort()) return false;

	MilpComponent* comp = dynamic_cast<MilpComponent*>(pPort->parent());
	if (!comp)
		return false;

	const bool allCarriersReady = comp->allDefaultPortsHaveCarriers();
	const bool hasModel = comp->compoModel() != nullptr;
	const bool firstInit = hasModel &&
		comp->compoModel()->getInputParam()->getMapParams().empty(); // TODO: replace with robust flag
	const bool noMainCarrier = comp->getMainCarrier() == nullptr;

	if (allCarriersReady && hasModel && firstInit && noMainCarrier)
	{
		/*
		 * Declare parameters and IOs only after EnergyVectors of all default ports are set.
		 * Should be called only once (first EnergyVector set).
		 * Should not be called if the EnergyVector is changed later on.
		 * Otherwise, parameter values should be saved.
		 */
		try {
			comp->initSubModelConfiguration(false);
		}
		catch (...) {
			return false;
		}

		return true;
	}

	return false;
}