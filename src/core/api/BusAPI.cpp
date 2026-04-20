#include "CairnAPI.h"
#include "CairnCore.h"
#include "BusCompo.h"
#include "CairnAPIUtils.h"
using namespace CairnAPIUtils;

CairnAPI::BusAPI::BusAPI(class BusCompo* ap_Bus)
	: CairnAPI::ObjectAPI(ap_Bus)
{	
}

CairnAPI::BusAPI::BusAPI(const OptimProblemAPI& a_Problem, const std::string& a_Name, 
	const std::string& a_ModelName, const EnergyVectorAPI& a_EnergyVector)
	: CairnAPI::ObjectAPI()
{
	*this = a_Problem.create_Bus(a_Name, a_ModelName, a_EnergyVector);
}

BusCompo* CairnAPI::BusAPI::get_BusCompo() const
{
	return (BusCompo*)get_Object();
}

void CairnAPI::BusAPI::set_BusCompo(BusCompo* ap_Bus)
{
	set_Object(ap_Bus);	
	BusCompo* pBus = get_BusCompo();
	if (pBus && !pBus->getMainCarrier()) {
		CairnAPIUtils::setError(errDefault, "The EnergyCarrier of the Bus " + get_Name() + " must be defined!");
	}
}

std::string CairnAPI::BusAPI::get_Type() const
{
	BusCompo* pBus = get_BusCompo();
	if (pBus) {
		return pBus->Type();
	}
	return "";
}

std::string CairnAPI::BusAPI::get_ModelClass() const
{
	BusCompo* pBus = get_BusCompo();
	if (pBus) {
		return pBus->ModelClassName();
	}
	return "";
}

t_list CairnAPI::BusAPI::get_PossibleModelClasses() const
{
	t_list vRet = {};

	const BusCompo* pBus = get_BusCompo();
	if (pBus) {
		vRet = pBus->possibleModelClasses();
	}

	const std::string model = get_ModelClass();
	if (!model.empty() && std::find(vRet.cbegin(), vRet.cend(), model) == vRet.cend()) {
		vRet.insert(vRet.begin(), model);
	}

	return vRet;
}

t_list CairnAPI::BusAPI::get_PossibleControlValues() const
{
	t_list vRet = {};

	const BusCompo* pBus = get_BusCompo();
	if (pBus) {
		vRet = pBus->getPossibleControlValues();
	}

	return vRet;
}

t_list CairnAPI::BusAPI::get_PossibleObjectiveTypes() const
{
	t_list vRet = {};

	const BusCompo* pBus = get_BusCompo();
	if (pBus) {
		vRet = pBus->getPossibleObjectiveTypes();
	}

	return vRet;
}


std::string CairnAPI::BusAPI::get_CarrierName() const
{
	BusCompo* pBus = get_BusCompo();
	if (pBus && pBus->getMainCarrier()) {
		return pBus->getMainCarrier()->Name();
	}
	return "";
}

EnergyVector* CairnAPI::BusAPI::get_Carrier() const
{
	BusCompo* pBus = get_BusCompo();
	if (pBus) {
		return pBus->getMainCarrier();
	}
}

void CairnAPI::BusAPI::set_Carrier(const std::string& a_CarrierName) 
{
	BusCompo* pBus = get_BusCompo();
	if (pBus) {
		// check if there is an EnergyCarrier with this name
		OptimProblem* vOptimProblem = (OptimProblem*)pBus->parent();
		if (vOptimProblem) {
			EnergyVector* vEnergyVector = vOptimProblem->findChild<EnergyVector>(a_CarrierName);
			if (!vEnergyVector) {
				CairnAPIUtils::setError(errDefault, "There is no EnergyCarrier with name " + a_CarrierName);
			}
			else {
				configure_Carrier(vEnergyVector);
			}
		}
	}
}

void CairnAPI::BusAPI::set_Carrier(const EnergyVectorAPI& EnergyCarrier)
{
	configure_Carrier(EnergyCarrier.get_EnergyVector());
}

void CairnAPI::BusAPI::configure_Carrier(EnergyVector* vEnergyVector)
{
	BusCompo* pBus = get_BusCompo();

	// check if already has links
	if (pBus->PortList().size() != 0) {
		CairnAPIUtils::setError(errDefault, "Cannot change carrier because the Bus already has links!");
	}

	// set Carrier
	if (vEnergyVector) {
		if (pBus->getMainCarrier() != vEnergyVector) {
			pBus->setMainCarrier(vEnergyVector);
		}
	}
	else {
		CairnAPIUtils::setError(errDefault, "EnergyCarrier is not defined!");
	}
}

std::string CairnAPI::BusAPI::get_LabelValue(const std::string& a_Label) const
{
	std::string vRet = "";
	BusCompo* pBus = get_BusCompo();
	if (pBus)
	{
		OptimProblem* vOptimProblem = (OptimProblem*)pBus->parent();
		TecEcoAnalysis* vTecEcoAnalysis = vOptimProblem->getTecEcoAnalysis();
		if (vTecEcoAnalysis->isValidLabel(a_Label)) {
			vRet = pBus->compoModel()->getLabelValue(a_Label);
		}
		else {
			CairnAPIUtils::setError(errDefault, "Label " + a_Label + " is not defined. Please, add the label to the problem first!");
		}
	}
	return vRet;
}

t_dict CairnAPI::BusAPI::get_LabelValues() const
{
	t_dict vRet = {};
	BusCompo* pBus = get_BusCompo();
	if (pBus)
	{
		//return pBus->compoModel()->getLabelMap();

		OptimProblem* vOptimProblem = (OptimProblem*)pBus->parent();
		TecEcoAnalysis* vTecEcoAnalysis = vOptimProblem->getTecEcoAnalysis();
		if (vTecEcoAnalysis) {
			for (auto const& label : vTecEcoAnalysis->getLabelList())//referance list
			{
				vRet[label] = pBus->compoModel()->getLabelValue(label);
			}
		}
	}
	return vRet;
}

void CairnAPI::BusAPI::set_LabelValue(const std::string& a_Label, const std::string& a_Value)
{
	BusCompo* pBus = get_BusCompo();
	if (pBus)
	{
		OptimProblem* vOptimProblem = (OptimProblem*)pBus->parent();
		TecEcoAnalysis* vTecEcoAnalysis = vOptimProblem->getTecEcoAnalysis();
		if (vTecEcoAnalysis->isValidLabel(a_Label)) {
			pBus->compoModel()->setLabel(a_Label, a_Value);
		}
		else {
			CairnAPIUtils::setError(errDefault, "Label " + a_Label + " is not defined. Please, add the label to the problem first!");
		}
	}
}

void CairnAPI::BusAPI::set_LabelValues(const t_dict& a_Labels)
{
	BusCompo* pBus = get_BusCompo();
	if (pBus)
	{
		/*
		* No need to verify that all the labels are valid. It is ok!
		* Labels are filtered when using get_method and when writing to json file
		*/
		std::map<std::string, std::string> labels;
		for (const auto& [key, value] : a_Labels) {
			labels[key] = CairnAPIUtils::valueToString(value);
		}
		pBus->compoModel()->setLabelMap(labels);
	}
}

// Set the value of a parameter
void CairnAPI::BusAPI::set_SettingValue(const std::string& a_SettingName, const t_value& a_SettingValue, bool checkExistance)
{
	if (a_SettingName == "ModelClass") {
		t_value modelClass = get_SettingValue("ModelClass");
		if (modelClass != a_SettingValue) {
			CairnAPIUtils::setError(errDefault, "The ModelClass of a Bus cannot be changed!");
		}
		else {
			return;
		}
	}

	ECodeError vRet = noError;
	BusCompo* pBus = get_BusCompo();
	if (pBus) {
		bool vOk = CairnAPIUtils::setParameter(get_InputParams()
		, a_SettingName, a_SettingValue);

		//Update MilpComponent::mComponent as it is used to re-initialize the component parameters
		pBus->updateCompoParamMap(a_SettingName, a_SettingValue);

		vRet = (vOk) ? noError : errParam;
	}
	CairnAPIUtils::setError(vRet);
}

// Set the values of several parameters
void CairnAPI::BusAPI::set_SettingValues(const t_dict& a_SettingValues)
{
	for (auto& vParam : a_SettingValues) {
		set_SettingValue(vParam.first, vParam.second);
	}
}

// -- Ports ---
std::map<std::string, std::string>
CairnAPI::BusAPI::get_DefaultPortData(const std::string& portId) const
{
	const BusCompo* pBus = get_BusCompo();
	if (!pBus)
		return {};

	const SubModel* subModel = pBus->compoModel();
	if (!subModel)
		return {};

	return subModel->getDefaultPortData(portId);
}

t_list CairnAPI::BusAPI::get_DefaultPortIDs() const
{
	t_list vRet = {};
	BusCompo* pBus = get_BusCompo();
	if (pBus) {
		for (MilpPort* vPort : pBus->PortList())
		{
			if (vPort->IsDefaultPort()) {
				vRet.push_back(vPort->ID());
			}
		}
	}
	return vRet;
}

t_list CairnAPI::BusAPI::get_DefaultPorts() const 
{
	t_list vRet = {};
	BusCompo* pBus = get_BusCompo();
	if (pBus) {
		for (MilpPort* vPort : pBus->PortList())
		{
			if (vPort->IsDefaultPort()) {
				vRet.push_back(vPort->Name());
			}
		}
	}
	return vRet;
}

t_list CairnAPI::BusAPI::get_Ports() const
{
	t_list vRet = {};
	BusCompo* pBus = get_BusCompo();
	if (pBus) {
		for (MilpPort* vPort : pBus->PortList())
		{
			vRet.push_back(vPort->Name());
		}
	}
	return vRet;
}

CairnAPI::MilpPortAPI CairnAPI::BusAPI::get_Port(const std::string& a_Name)
{
	MilpPortAPI vRet;
	BusCompo* pBus = get_BusCompo();
	std::string vPortName = std::string(a_Name.c_str());
	if (pBus) {
		// Get port by Id
		MilpPort* vPort = pBus->getPort(vPortName);

		if (!vPort) {
			// Get port by name
			vPort = pBus->getPortByName(vPortName);
		}

		if (vPort) {
			vRet.set_MilpPort(vPort);
		}
		else {
			CairnAPIUtils::setError(errNotFound, "port " + a_Name);
		}
	}
	return vRet;
}

CairnAPI::MilpPortAPI CairnAPI::BusAPI::add_Port(const std::string& a_PortName, const EnergyVectorAPI& a_EnergyVector,
	const std::string& a_Direction, const std::string& a_Variable, const std::string& a_PortId)
{
	EnergyVector* pCarrier = get_Carrier();

	const EnergyVector* pVector = a_EnergyVector.get_EnergyVector();
	if (pVector && pVector != pCarrier) {
		cWarning() << "The provided carrier " + pVector->Name() + " is different from the Bus carrier" + pCarrier->Name()
			+ ". The bus carrier will be used!";
	}
	
	MilpPortAPI vPort;
	BusCompo* pBus = get_BusCompo();
	ECodeError vErr = noError;
	std::string vErrMsg = "";
	if (pBus) {
		//check if the port with the same name already exist		
		MilpPort* vMilpPort = pBus->getPortByName(a_PortName);
		if (vMilpPort) {
			vErr = errAlreadyExist;
			vErrMsg = "port " + a_PortName;
		}
		else {
			std::string vComponentName(get_Name());
			std::string vPortId = a_PortId;
			if (vPortId.empty()) vPortId = pBus->getUniquePortID();
			std::map<std::string, std::string> vPortParams;
			vPortParams["CompoName"] = vComponentName;
			vPortParams["Name"] = a_PortName;
			vPortParams["Carrier"] = pCarrier->Name();
			vPortParams["Direction"] = CairnUtils::toUpper(a_Direction);
			vPortParams["Variable"] = a_Variable;

			//create port
			pBus->createOnePort(vPortId, vPortParams);
			vMilpPort = pBus->getPort(vPortId);
			if (vMilpPort) {
				vPort.set_MilpPort(vMilpPort);
				vPort.set_EnergyCarrier(pCarrier);
			}
			else {
				vErr = errAdd;
				vErrMsg = "port " + a_PortName;
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

bool CairnAPI::BusAPI::remove_Port(MilpPortAPI& a_Port, const bool isDeleteCompo)
{
	ECodeError vErr = noError;
	std::string vErrMsg = "";
	BusCompo* pBus = get_BusCompo();
	if (pBus) {
		std::string vPortName(a_Port.get_Name());
		MilpPort* pMilpPort = pBus->getPortByName(vPortName);
		if (pMilpPort) {
			//check if it is a default port
			if (!isDeleteCompo && pMilpPort->IsDefaultPort()) {
				cInfo() << "Port " + vPortName + " of component " + pBus->Name() + " is a default port."
					+ " It is not possible to delete default ports!";
				return false;
			}
			else {
				//check if there is a link or Not
				BusCompo* vBus = pMilpPort->getLinkedBus();
				if (vBus) {
					vBus->removeLink(pBus, pMilpPort);
				}
				pBus->removePort(pMilpPort);
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

//bool CairnAPI::BusAPI::useEnergyVector(const std::string& a_EnergyCarrierName)
//{
//	bool vRet = false;
//	t_list vPorts = get_Ports();
//	for (auto& vPort : vPorts) {
//		if (get_Port(vPort).get_CarrierName() == a_EnergyCarrierName) {
//			vRet = true;
//			break;
//		}
//	}
//	return vRet;
//}

void CairnAPI::BusAPI::get_Links(t_dict& a_Links)
{
	t_list vPorts = get_Ports();
	for (auto& vPortName : vPorts) {
		MilpPort* vPort = get_Port(vPortName).get_MilpPort();
		if (vPort) {
			BusCompo* vBus = vPort->getLinkedBus();
			if (vBus) {
				a_Links[get_Name() + "." + vPortName] = vBus->Name();
			}
		}
	}
}

// -- IOs ---
t_list CairnAPI::BusAPI::get_VarList()
{
	t_list vRet = {};
	BusCompo* pBus = get_BusCompo();
	if (pBus) {
		// le composant existe, retourne une liste des variables (model expressions)		
		const SubModel::t_mapIOs& vIOMap = pBus->compoModel()->getMapIOExpression();
		for (auto& [vName, vIO] : vIOMap) {
			if (vIO->IsUsed()) {
				vRet.push_back(vName);
			}
		}
	}
	else {
		CairnAPIUtils::setError(noCairn);
	}
	return vRet;
}

t_value CairnAPI::BusAPI::get_varValue(const std::string& a_VarName)
{
	t_value vRet = NAN;
	BusCompo* pBus = get_BusCompo();
	if (pBus) {
		std::string vVarName(a_VarName.c_str());
		ModelIO* vIO = pBus->compoModel()->getIOExpression(vVarName);
		if (vIO) {
			if (vIO->IsUsed()) {
				// a stored value of vIO->evaluate(a_solution.getOptimalSolution(0))
				vRet = vIO->getValue();
			}
			else {
				CairnAPIUtils::setError(errDefault, "Variable " + a_VarName + " is not activated.");
			}
		}
		else {
			CairnAPIUtils::setError(errNotFound, "variable " + a_VarName);
		}
	}
	else {
		CairnAPIUtils::setError(noCairn);
	}
	return vRet;
}

t_dict CairnAPI::BusAPI::get_varValues()
{
	t_dict vRet = {};
	t_list varList = get_VarList();
	for (auto& varName : varList) {
		vRet[varName] = get_varValue(varName);
	}
	return vRet;
}
// -- Indicators ---
t_list CairnAPI::BusAPI::get_IndicatorNames()
{
	t_list vRet = {};
	BusCompo* pBus = get_BusCompo();
	if (pBus) {
		InputParam::t_Indicators vectIndicators = pBus->compoModel()->getInputIndicators()->getIndicators();
		for (int i = 0; i < vectIndicators.size(); i++)
		{
			vRet.push_back(vectIndicators[i]->getName());
		}
	}
	else
		CairnAPIUtils::setError(noCairn);
	return vRet;
}

t_list CairnAPI::BusAPI::get_IndicatorUnits()
{
	t_list vRet = {};
	BusCompo* pBus = get_BusCompo();
	if (pBus) {
		InputParam::t_Indicators vectIndicators = pBus->compoModel()->getInputIndicators()->getIndicators();
		for (int i = 0; i < vectIndicators.size(); i++)
		{
			vRet.push_back(vectIndicators[i]->getUnit());
		}
	}
	else
		CairnAPIUtils::setError(noCairn);
	return vRet;
}

t_list CairnAPI::BusAPI::get_IndicatorShortNames()
{
	t_list vRet = {};
	BusCompo* pBus = get_BusCompo();
	if (pBus) {
		InputParam::t_Indicators vectIndicators = pBus->compoModel()->getInputIndicators()->getIndicators();
		for (int i = 0; i < vectIndicators.size(); i++)
		{
			vRet.push_back(vectIndicators[i]->getShortName());
		}
	}
	else
		CairnAPIUtils::setError(noCairn);
	return vRet;
}

t_dict CairnAPI::BusAPI::get_IndicatorValues(const std::string& range) const
{
	t_dict result;

	MilpComponent* pBus = get_BusCompo();
	if (!pBus) {
		CairnAPIUtils::setError(noCairn, "Component not available");
		return result;  // Return empty dict
	}

	// Get all indicators
	const auto& indicatorList = pBus->compoModel()->getInputIndicators()->getIndicators();

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

double CairnAPI::BusAPI::get_IndicatorValue(const std::string& name, const std::string& range) const
{
	MilpComponent* pBus = get_BusCompo();
	if (!pBus) {
		CairnAPIUtils::setError(noCairn, "Bus not available");
		return std::numeric_limits<double>::quiet_NaN();
	}

	auto value = pBus->getIndicatorValue(name, range);
	if (value) {
		return *value;
	}

	CairnAPIUtils::setError(errGet, "Indicator '" + name + "' not found");
	return std::numeric_limits<double>::quiet_NaN();
}