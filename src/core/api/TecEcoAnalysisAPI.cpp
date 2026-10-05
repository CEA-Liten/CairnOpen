#include "CairnAPI.h"
#include "CairnCore.h"
#include "CairnAPIUtils.h"
#include "InputParam.h"
using namespace CairnAPIUtils;

CairnAPI::TecEcoAnalysisAPI::TecEcoAnalysisAPI()
	: CairnAPI::ObjectAPI()
{
}

TecEcoCompo* CairnAPI::TecEcoAnalysisAPI::get_TecEcoComponent() const
{
	return m_Object ? dynamic_cast<TecEcoCompo*>(m_Object) : nullptr;
}

t_list CairnAPI::TecEcoAnalysisAPI::get_PossibleOptimModels()
{
	t_list possibleModels = { "OptimNPV", "OptimManualObjective", "OptimEnvImpact", "OptimizeControlOnly" };

	const t_value selectedImpacts = get_SettingValue("ConsideredEnvironmentalImpacts");
	const auto* impactNames = std::get_if<std::vector<std::string>>(&selectedImpacts);
	if (!impactNames)
		return possibleModels;

	const TecEcoCompo* pComp = get_TecEcoComponent();
	for (const std::string& name : *impactNames) {
		const std::string shortName = pComp ? pComp->EnvImpactShortName(name) : name;
		possibleModels.push_back("OptimEnvImpact-" + shortName);
	}

	return possibleModels;
}

// Set the value of a parameter
void CairnAPI::TecEcoAnalysisAPI::set_SettingValue(const std::string& a_SettingName, const t_value& a_SettingValue, bool checkExistance)
{		
	set_SettingValues({ {a_SettingName, a_SettingValue} });
}

// Set the values of several parameters
void CairnAPI::TecEcoAnalysisAPI::set_SettingValues(const t_dict& a_Settings)
{
	if (!m_Object) {
		CairnAPIUtils::setError(errDefault, "No TecEco object available");
		return;
	}

	// resolve TecEcoComponent 
	auto* tecEcoCompo = dynamic_cast<TecEcoCompo*>(m_Object);
	if (!tecEcoCompo) {
		CairnAPIUtils::setError(errDefault, "set_SettingValues: m_Object is not a TecEcoCompo");
		return;
	}

	auto* problem = dynamic_cast<OptimProblem*>(tecEcoCompo->parent());
	if (!problem) {
		CairnAPIUtils::setError(errDefault, "set_SettingValues: TecEcoCompo has no OptimProblem parent");
		return;
	}

	auto* model = dynamic_cast<TecEcoAnalysis*>(tecEcoCompo->compoModel());
	if (!model) {
		CairnAPIUtils::setError(errDefault, "set_SettingValues: TecEcoCompo has no TecEcoAnalysis model");
		return;
	}

	// Save current env impacts selection before applying settings
	std::string prevEnvImpacts;
	model->getConfigParam()->getParameterValue("ConsideredEnvironmentalImpacts", prevEnvImpacts, EParamType::eStringList);

	// Split settings into config vs non-config
	std::vector<std::string> configNames;
	model->getConfigParam()->getParameters(configNames);

	t_dict configSettings;    /** Parameters belonging to the config InputParam */
	t_dict nonConfigSettings; /** All other parameters */
	for (const auto& [name, value] : a_Settings)
	{
		if (CairnUtils::contains(configNames, name))
			configSettings[name] = value;
		else
			nonConfigSettings[name] = value;
	}

	// Apply config parameters
	const bool configOk = CairnAPIUtils::setParameters(
		{ model->getConfigParam() }, configSettings);

	if (configOk && configSettings.count("ConsideredEnvironmentalImpacts"))
		model->declareEnvImpactParam();

	// Apply non-config parameters
	const bool nonConfigOk = CairnAPIUtils::setParameters(
		{
			model->getCompoInputParam(),
			model->getCompoInputSettings(),
			model->getCompoEnvImpactsParam(),
			tecEcoCompo->getGUIData()->getGuiInputParam()
		},
		nonConfigSettings);

	if (!configOk || !nonConfigOk) {
		CairnAPIUtils::setError(errParam,
			std::string(!configOk ? "Failed to set config parameters" : "")
			+ std::string(!nonConfigOk ? " Failed to set non-config parameters" : ""));
		return;
	}

	cDebug() << "set_SettingValues: computing extrapolation factor";
	problem->computeExtrapolationFactor();

	// Redeclare env impact parameters if the selection changed
	std::string newEnvImpacts;
	model->getConfigParam()->getParameterValue(
		"ConsideredEnvironmentalImpacts", newEnvImpacts, EParamType::eStringList);

	if (newEnvImpacts == prevEnvImpacts)
	{
		CairnAPIUtils::setError(noError, "");
		return;
	}

	cDebug() << "ConsideredEnvironmentalImpacts changed — redeclaring env impact parameters";

	try
	{
		problem->redeclareEnvImpactParameters();
		tecEcoCompo->declareIOVariables();
		tecEcoCompo->declareIndicators();
		CairnAPIUtils::setError(noError, "");
	}
	catch (const Cairn_Exception& e)
	{
		cError() << "set_SettingValues: failed to redeclare env impact parameters:"
			<< e.message();
		CairnAPIUtils::setError(errParam, "Failed to redeclare env impact parameters: " + e.message());
	}
	catch (const std::exception& e)
	{
		cError() << "set_SettingValues: unexpected exception:" << e.what();
		CairnAPIUtils::setError(errParam, std::string("Unexpected error: ") + e.what());
	}
}

// -- Ports ---
std::map<std::string, std::string>
CairnAPI::TecEcoAnalysisAPI::get_DefaultPortData(const std::string& portId) const
{
	const TecEcoCompo* pTecEcoComp = get_TecEcoComponent();
	if (!pTecEcoComp)
		return {};

	const SubModel* subModel = pTecEcoComp->compoModel();
	if (!subModel)
		return {};

	return subModel->getDefaultPortData(portId);
}

t_list CairnAPI::TecEcoAnalysisAPI::get_DefaultPortIDs() const
{
	t_list vRet = {};
	if (m_Object) {
		TecEcoCompo* pTecEco = (TecEcoCompo*)m_Object;
		if (pTecEco) {
			for (MilpPort* vPort : pTecEco->PortList())
			{
				if (vPort->IsDefaultPort()) {
					vRet.push_back(vPort->ID());
				}
			}
		}
	}
	return vRet;
}

t_list CairnAPI::TecEcoAnalysisAPI::get_DefaultPorts() const 
{
	t_list vRet = {};
	if (m_Object) {
		TecEcoCompo* pTecEco = (TecEcoCompo*)m_Object;
		if (pTecEco) {
			for (MilpPort* vPort : pTecEco->PortList())
			{
				if (vPort->IsDefaultPort()) {
					vRet.push_back(vPort->Name());
				}
			}
		}
	}
	return vRet;
}

t_list CairnAPI::TecEcoAnalysisAPI::get_Ports() const
{
	t_list vRet = {};
	if (m_Object) {
		TecEcoCompo* pTecEco = (TecEcoCompo*)m_Object;
		if (pTecEco) {
			for (MilpPort* vPort : pTecEco->PortList())
			{
				vRet.push_back(vPort->Name());
			}
		}
	}
	return vRet;
}

std::shared_ptr < CairnAPI::MilpPortAPI> CairnAPI::TecEcoAnalysisAPI::get_Port(const std::string& a_Name)
{
	std::shared_ptr < MilpPortAPI> vRet;
	const std::string vPortName = std::string(a_Name.c_str());
	if (m_Object) {
		TecEcoCompo* pTecEco = (TecEcoCompo*)m_Object;
		if (pTecEco) {
			// Get port by Id
			MilpPort* vPort = pTecEco->getPort(vPortName);

			if (!vPort) {
				// Get port by name
				vPort = pTecEco->getPortByName(vPortName);
			}

			if (vPort) {
				vRet = std::make_shared<MilpPortAPI>(vPort);
			}
			else {
				CairnAPIUtils::setError(errNotFound, "port " + a_Name);
			}
		}
	}
	return vRet;
}

std::shared_ptr <CairnAPI::MilpPortAPI> CairnAPI::TecEcoAnalysisAPI::add_Port(const std::string& a_PortName, const EnergyVectorAPI& a_EnergyVector,
	const std::string& a_Direction, const std::string& a_Variable, const std::string& a_PortId)
{
	std::shared_ptr <MilpPortAPI> vPort;
	ECodeError vErr = noError;
	std::string vErrMsg = "";

	if (m_Object) {
		TecEcoCompo* pTecEco = (TecEcoCompo*)m_Object;
		if (pTecEco) {
			//check if the port with the same name already exist		
			MilpPort* vMilpPort = pTecEco->getPortByName(a_PortName);
			if (vMilpPort) {
				vErr = errAlreadyExist;
				vErrMsg = "port " + a_PortName;
			}
			else {
				std::string vComponentName(get_Name());
				std::string vPortId = a_PortId;
				if(vPortId.empty()) vPortId =  pTecEco->getUniquePortID();

				t_mapParamData vPortParams;
				CairnUtils::setParamValue(vPortParams, "CompoName", vComponentName);
				CairnUtils::setParamValue(vPortParams, "Name", a_PortName);
				CairnUtils::setParamValue(vPortParams, "Carrier", a_EnergyVector.get_Name());
				CairnUtils::setParamValue(vPortParams, "Direction", CairnUtils::toUpper(a_Direction));
				CairnUtils::setParamValue(vPortParams, "Variable", a_Variable);

				//create port
				pTecEco->createOnePort(vPortId, vPortParams, a_EnergyVector.get_EnergyVector());
				vMilpPort = pTecEco->getPort(vPortId);
				if (vMilpPort) {
					vPort = std::make_shared<MilpPortAPI>(vMilpPort);					
					//vPort.set_EnergyCarrier(a_EnergyVector.get_EnergyVector());
				}
				else {
					vErr = errAdd;
					vErrMsg = "port " + a_PortName;
				}
			}
		}
	}
	else {
		vErr = errDefault;
		vErrMsg = "The component doesn't exist!";
	}
	CairnAPIUtils::setError(vErr, vErrMsg);
	return vPort;
}

bool CairnAPI::TecEcoAnalysisAPI::remove_Port(MilpPortAPI& a_Port, const bool isDeleteCompo)
{
	ECodeError vErr = noError;
	std::string vErrMsg = "";
	if (m_Object) {
		TecEcoCompo* pTecEco = (TecEcoCompo*)m_Object;
		if (pTecEco) {
			std::string vPortName(a_Port.get_Name());
			MilpPort* vMilpPort = pTecEco->getPortByName(vPortName);
			if (vMilpPort) {
				//check if it is a default port
				if (!isDeleteCompo && vMilpPort->IsDefaultPort()) {
					cInfo() << "Port " + vPortName + " of component " + pTecEco->Name() + " is a default port."
						+ " It is not possible to delete default ports!";
					return false;
				}
				else {
					//check if there is a link or Not
					BusCompo* vBus = vMilpPort->getLinkedBus();
					if (vBus) {
						vBus->removeLink(pTecEco, vMilpPort);
					}
					pTecEco->removePort(vMilpPort);
				}
			}
		}
		else {
			vErr = errNotFound;
			vErrMsg = a_Port.get_Name();
		}
	}
	else {
		vErr = errDefault;
		vErrMsg = "The component doesn't exist!";
	}
	CairnAPIUtils::setError(vErr, vErrMsg);
	return true;
}

bool CairnAPI::TecEcoAnalysisAPI::useEnergyVector(const std::string& a_EnergyCarrierName)
{
	bool vRet = false;
	t_list vPorts = get_Ports();
	for (auto& vPort : vPorts) {
		if (get_Port(vPort)->get_CarrierName() == a_EnergyCarrierName) {
			vRet = true;
			break;
		}
	}
	return vRet;
}

void CairnAPI::TecEcoAnalysisAPI::get_Links(t_dict& a_Links)
{
	t_list vPorts = get_Ports();
	for (auto& vPortName : vPorts) {
		MilpPort* vPort = get_Port(vPortName)->get_MilpPort();
		if (vPort) {
			BusCompo* vBus = vPort->getLinkedBus();
			if (vBus) {
				a_Links[get_Name() + "." + vPortName] = vBus->Name();
			}
		}
	}
}
// -- Indicators ---
t_list CairnAPI::TecEcoAnalysisAPI::getIndicatorProperty(IndicatorProperty property) const
{
	t_list result;

	TecEcoCompo* pTecEco = get_TecEcoComponent();
	if (!pTecEco) {
		CairnAPIUtils::setError(noCairn, "TecEco component not available");
		return result;
	}

	auto* model = pTecEco->compoModel();
	if (!model) {
		CairnAPIUtils::setError(errGet, "Component model not available");
		return result;
	}

	auto* inputIndicators = model->getInputIndicators();
	if (!inputIndicators) {
		CairnAPIUtils::setError(errGet, "Input indicators not available");
		return result;
	}

	const auto& indicators = inputIndicators->getIndicators();
	result.reserve(indicators.size()); // Pre-allocate for efficiency

	for (const auto* indicator : indicators) {
		if (!indicator) continue;

		switch (property) {
		case IndicatorProperty::Name:
			result.push_back(indicator->getName());
			break;
		case IndicatorProperty::Unit:
			result.push_back(indicator->getUnit());
			break;
		case IndicatorProperty::ShortName:
			result.push_back(indicator->getShortName());
			break;
		}
	}

	return result;
}

t_list CairnAPI::TecEcoAnalysisAPI::get_IndicatorNames() const
{
	return getIndicatorProperty(IndicatorProperty::Name);
}

t_list CairnAPI::TecEcoAnalysisAPI::get_IndicatorUnits() const
{
	return getIndicatorProperty(IndicatorProperty::Unit);
}

t_list CairnAPI::TecEcoAnalysisAPI::get_IndicatorShortNames() const
{
	return getIndicatorProperty(IndicatorProperty::ShortName);
}

t_dict CairnAPI::TecEcoAnalysisAPI::get_IndicatorValues(const std::string& range) const
{
	t_dict result;

	TecEcoCompo* pTecEco = get_TecEcoComponent();
	if (!pTecEco) {
		CairnAPIUtils::setError(noCairn, "TecEcoAnalysis not available");
		return result;  // Return empty dict
	}

	// Get all indicators
	const auto& indicatorList = pTecEco->compoModel()->getInputIndicators()->getIndicators();

	// Get value for each indicator 
	for (const auto* indicator : indicatorList) {
		if (!indicator) continue;

		std::string name = indicator->getName();
		double value = get_IndicatorValue(name, range);

		// Only add if not NaN (i.e., found)
		if (!std::isnan(value)) {
			result[name] = value;
		}
	}

	return result;
}

double CairnAPI::TecEcoAnalysisAPI::get_IndicatorValue(const std::string& name, const std::string& range) const
{
	TecEcoCompo* pTecEco = get_TecEcoComponent();
	if (!pTecEco) {
		CairnAPIUtils::setError(noCairn, "TecEcoAnalysis not available");
		return std::numeric_limits<double>::quiet_NaN();
	}

	auto value = pTecEco->getIndicatorValue(name, range);
	if (value) {
		return *value;
	}

	CairnAPIUtils::setError(errGet, "Indicator '" + name + "' not found");
	return std::numeric_limits<double>::quiet_NaN();
}
