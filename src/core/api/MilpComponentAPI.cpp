#include "CairnAPIUtils.h"
#include "CairnCore.h"
#include "Cairn_Exception.h"
#include "CairnAPI.h"

using namespace CairnAPIUtils;

CairnAPI::MilpComponentAPI::MilpComponentAPI(MilpComponent* ap_Component)
	: CairnAPI::ObjectAPI(ap_Component)
{		
}

CairnAPI::MilpComponentAPI::MilpComponentAPI(const OptimProblemAPI& a_Problem, const std::string& a_Name, const std::string& a_ModelName)
	: CairnAPI::ObjectAPI()
{
	*this = a_Problem.create_Component(a_Name, a_ModelName);
}

MilpComponent* CairnAPI::MilpComponentAPI::get_MilpComponent() const
{
	return  (MilpComponent*)get_Object();
}

void CairnAPI::MilpComponentAPI::set_MilpComponent(MilpComponent* ap_Component)
{
	set_Object(ap_Component);
}


std::string CairnAPI::MilpComponentAPI::get_Type() const
{
	std::string type = "";
	MilpComponent* pComponent = get_MilpComponent();
	if (pComponent) {
		type = pComponent->Type();
	}
	return type;
}

std::string CairnAPI::MilpComponentAPI::get_ModelClass() const
{
	std::string modelClass = "";
	MilpComponent* pComponent = get_MilpComponent();
	if (pComponent) {
		modelClass = pComponent->ModelClassName();
	}
	return modelClass;
}

t_list CairnAPI::MilpComponentAPI::get_PossibleModelClasses() const
{
	t_list vRet = {};

	const MilpComponent* pComponent = get_MilpComponent();
	if (pComponent) {
		vRet = pComponent->possibleModelClasses();
	}

	const std::string model = get_ModelClass();
	if (!model.empty() && std::find(vRet.cbegin(), vRet.cend(), model) == vRet.cend()) {
		vRet.insert(vRet.begin(), model);
	}

	return vRet;
}

t_list CairnAPI::MilpComponentAPI::get_PossibleControlValues() const
{
	t_list vRet = {};

	const MilpComponent* pComponent = get_MilpComponent();
	if (pComponent) {
		vRet = pComponent->getPossibleControlValues();
	}

	return vRet;
}

std::string CairnAPI::MilpComponentAPI::get_Direction()
{
	MilpComponent* pComponent = get_MilpComponent();
	std::string vErrMsg = "Not valid: component " + get_Name() + " of type " + get_Type() + " doesn't have a direction!";
	if (get_Type() == "Grid") {
		vErrMsg = "Error: not able to obtain the direction of Grid " + get_Name();
		if (pComponent) {
			GridSubModel* gridModel = dynamic_cast<GridSubModel*> (pComponent->compoModel());
			if (gridModel) {
				return gridModel->Direction();
			}
		}
	}
	else if (get_Type() == "SourceLoad") {
		vErrMsg = "Error: not able to obtain the direction of SourceLoad " + get_Name();
		if (pComponent) {
			SourceLoadSubModel* sourceLoadModel = dynamic_cast<SourceLoadSubModel*> (pComponent->compoModel());
			if (sourceLoadModel) {
				return sourceLoadModel->Direction();
			}
		}
	}
	CairnAPIUtils::setError(errDefault, vErrMsg);
	return "";
}

std::string CairnAPI::MilpComponentAPI::get_LabelValue(const std::string& a_Label) const
{
	std::string vRet = "";
	MilpComponent* pComponent = get_MilpComponent();
	if (pComponent)
	{
		OptimProblem* vOptimProblem = (OptimProblem*)pComponent->parent();
		TecEcoAnalysis* vTecEcoAnalysis = vOptimProblem->getTecEcoAnalysis();
		if (vTecEcoAnalysis->isValidLabel(a_Label)) {
			vRet = pComponent->compoModel()->getLabelValue(a_Label);
		}
		else {
			CairnAPIUtils::setError(errDefault, "Label " + a_Label + " is not defined. Please, add the label to the problem first!");
		}
	}
	return vRet;
}

t_dict CairnAPI::MilpComponentAPI::get_LabelValues() const
{
	t_dict vRet = {};
	MilpComponent* pComponent = get_MilpComponent();
	if (pComponent)
	{
		//return pComponent->compoModel()->getLabelMap();

		OptimProblem* vOptimProblem = (OptimProblem*)pComponent->parent();
		TecEcoAnalysis* vTecEcoAnalysis = vOptimProblem->getTecEcoAnalysis();
		if (vTecEcoAnalysis) {
			for (auto const& label : vTecEcoAnalysis->getLabelList())//referance list
			{
				vRet[label] = pComponent->compoModel()->getLabelValue(label);
			}
		}
	}
	return vRet;
}

void CairnAPI::MilpComponentAPI::set_LabelValue(const std::string& a_Label, const std::string& a_Value)
{
	MilpComponent* pComponent = get_MilpComponent();
	if (pComponent)
	{
		OptimProblem* vOptimProblem = (OptimProblem*)pComponent->parent();
		TecEcoAnalysis* vTecEcoAnalysis = vOptimProblem->getTecEcoAnalysis();
		if (vTecEcoAnalysis->isValidLabel(a_Label)) {
			pComponent->compoModel()->setLabel(a_Label, a_Value);
		}
		else {
			CairnAPIUtils::setError(errDefault, "Label " + a_Label + " is not defined. Please, add the label to the problem first!");
		}
	}
}

void CairnAPI::MilpComponentAPI::set_LabelValues(const t_dict& a_Labels)
{
	MilpComponent* pComponent = get_MilpComponent();
	if (pComponent)
	{
		/*
		* No need to verify that all the labels are valid. It is ok!
		* Labels are filtered when using get_method and when writing to json file
		*/
		std::map<std::string, std::string> labels;
		for (const auto& [key, value] : a_Labels) {
			labels[key] = CairnAPIUtils::valueToString(value);
		}

		pComponent->compoModel()->setLabelMap(labels);
	}
}

void CairnAPI::MilpComponentAPI::checkDefaultPortCarriers() const
{
	MilpComponent* pComponent = get_MilpComponent();
	if (pComponent) {
		if (!pComponent->allDefaultPortsHaveCarriers()) {
			CairnAPIUtils::setError(errDefault, "Please, configure the carriers of all default ports of component " + get_Name() + " first.");
		}
	}
}


bool CairnAPI::MilpComponentAPI::isTimeSeriesParam(const std::string& a_TimeSeriesName)
{
	checkDefaultPortCarriers();

	t_value tsValue;
	MilpComponent* pComponent = get_MilpComponent();
	if (pComponent) {
		if (pComponent->compoModel()->getInputTimeSeries()->getParameterValue(std::string(a_TimeSeriesName.c_str()), tsValue)
			|| pComponent->compoModel()->getInputPortImpactsParamTS()->getParameterValue(std::string(a_TimeSeriesName.c_str()), tsValue))
		{
			return true;
		}
	}
	return false;
}

t_value CairnAPI::MilpComponentAPI::get_OptimalSizeExpression()
{
	checkDefaultPortCarriers();

	t_value vRet = "";
	MilpComponent* pComponent = get_MilpComponent();
	if (pComponent) {
		vRet = pComponent->compoModel()->getOptimalSizeExpression();
	}
	return vRet;
}

t_value CairnAPI::MilpComponentAPI::get_SettingValue(const std::string& a_SettingName)
{	
	t_value vRet = "";
	MilpComponent* pComponent = get_MilpComponent();
	if (pComponent) {
		if (isTimeSeriesParam(a_SettingName))
		{
			//timeseries: return the name (value) of the timeseries; vRet is the vector value!
			return pComponent->getTimeSeriesName(std::string(a_SettingName.c_str()));
		}
		else {
			vRet = CairnAPI::ObjectAPI::get_SettingValue(a_SettingName);
		}
	}
	return vRet;
}

t_dict CairnAPI::MilpComponentAPI::get_SettingValues()
{	
	t_dict vRet = CairnAPI::ObjectAPI::get_SettingValues();
	MilpComponent* pComponent = get_MilpComponent();
	if (pComponent) {
		//Add timeseries names (not vector values)	
		std::vector<std::string> tsList;
		pComponent->compoModel()->getInputTimeSeries()->getParameters(tsList);
		pComponent->compoModel()->getInputPortImpactsParamTS()->getParameters(tsList);
		for (auto& tsParamName : tsList) {
			t_value tsName = get_SettingValue(tsParamName);
			vRet[tsParamName] = tsName;
		}
	}
	return vRet;
}

t_value CairnAPI::MilpComponentAPI::get_TimeSeriesVector(const std::string& a_SettingName)
{
	checkDefaultPortCarriers();
	MilpComponent* pComponent = get_MilpComponent();
	if (pComponent) {
		t_value vRet;
		if (pComponent->compoModel()->getInputTimeSeries()->getParameterValue(std::string(a_SettingName.c_str()), vRet)
			|| pComponent->compoModel()->getInputPortImpactsParamTS()->getParameterValue(std::string(a_SettingName.c_str()), vRet))
		{
			return vRet;
		}
	}
	return {};
}

void CairnAPI::MilpComponentAPI::modify_ModelClass(const std::string& a_prevModelClass, const std::string& a_newModelClass)
{
	checkDefaultPortCarriers();

	t_list possibleClasses = get_PossibleModelClasses();
	if (std::find(possibleClasses.cbegin(), possibleClasses.cend(), a_newModelClass) == possibleClasses.cend())
	{
		CairnAPIUtils::setError(errDefault, a_newModelClass + " is not a valid ModelClass!");
	}

	if (a_prevModelClass != a_newModelClass) {
		/* Changing ModelClass requires deleting then creating a new mCompoModel */
		try {
			//Save a copy of the ports
			std::map<std::string, t_dict> vPortsData = {};
			std::map<std::string, CairnAPI::EnergyVectorAPI> vPortCarriers = {};
			for (auto& vPortName : get_Ports()) {
				MilpPortAPI vPort = get_Port(vPortName);
				vPortsData[vPortName] = vPort.get_SettingValues();
				vPortCarriers[vPortName] = vPort.get_EnergyCarrier();
			}

			//Save a copy of the links
			t_dict vLinks;
			get_Links(vLinks);

			//Set ModelClass
			MilpComponent* pComponent = get_MilpComponent();
			bool vOk = CairnAPIUtils::setParameter({
				pComponent->compoModel()->getInputParam(), // params	
				pComponent->getCompoInputParam(), // options
				pComponent->compoModel()->getInputEnvImpactsParam(),
				pComponent->compoModel()->getInputPortImpactsParam(),
				pComponent->getGUIData()->getGuiInputParam() }, "ModelClass", a_newModelClass);
			pComponent->updateCompoParamMap("ModelClass", a_newModelClass);
			if (!vOk) CairnAPIUtils::setError(errParam);

			//Save a copy of param values 
			t_dict paramMap = get_SettingValues();
			t_dict labelMap = get_LabelValues();

			//Delete then create a new compoModel (new ModelClass)
			pComponent->deleteCompoModel();
			pComponent->createCompoModel();

			//Re-create and re-configure the ports
			OptimProblem* vOptimProblem = (OptimProblem*)pComponent->parent();
			CairnAPI::OptimProblemAPI vOptimProblemAPI;
			vOptimProblemAPI.set_Problem(vOptimProblem);

			t_list vDefaultPortNames = get_DefaultPorts();
			for (auto& [vPortName, vParams] : vPortsData) {
				auto it = find(vDefaultPortNames.begin(), vDefaultPortNames.end(), vPortName);
				if (it != vDefaultPortNames.end()) {
					//A default port
					CairnAPI::MilpPortAPI vDefaultPort = get_Port(vPortName);
					vDefaultPort.set_EnergyCarrier(vPortCarriers[vPortName]);
					vDefaultPort.set_SettingValues(vPortsData[vPortName]);
					//Re-construct link, if applicable
					for (auto& [vID, vBusName] : vLinks) {
						if (vID == get_Name() + "." + vPortName) {/* should be coherent with the ID used in MilpComponentAPI::get_Links */
							CairnAPI::BusAPI vBus = vOptimProblemAPI.get_Bus(CairnAPIUtils::getParamValue(vBusName));
							vOptimProblemAPI.add(vDefaultPort, vBus);
							break;
						}
					}
				}
				else {
					//A non-default port
					CairnAPI::MilpPortAPI vPort = add_Port(vPortName, vPortCarriers[vPortName], "", "", "", false); //direction, variable, id
					vPort.set_SettingValues(vPortsData[vPortName]);
					//Re-construct link, if applicable
					for (auto& [vID, vBusName] : vLinks) {
						if (vID == get_Name() + "." + vPortName) {/* should be coherent with the ID used in MilpComponentAPI::get_Links */
							CairnAPI::BusAPI vBus = vOptimProblemAPI.get_Bus(CairnAPIUtils::getParamValue(vBusName));
							vOptimProblemAPI.add(vPort, vBus);
							break;
						}
					}
				}
			}

			//Re-initialize the component
			pComponent->initProblem(false);

			//set param values after re-initialization
			set_SettingValues(paramMap);
			set_LabelValues(labelMap);
		}
		catch (...) {
			CairnAPIUtils::setError(errDefault, "Error while trying to change the ModelClass to " + a_prevModelClass + " to " + a_newModelClass);
		}
	}
}

void CairnAPI::MilpComponentAPI::set_SettingValue(const std::string& a_SettingName, const t_value& a_SettingValue, bool checkExistance)
{
	checkDefaultPortCarriers();
	MilpComponent* pComponent = get_MilpComponent();
	if (a_SettingName == "ModelClass") {
		t_value prevModelClass = get_SettingValue("ModelClass"); //get_ModelClass()
		modify_ModelClass(CairnAPIUtils::getParamValue(prevModelClass), CairnAPIUtils::getParamValue(a_SettingValue));
		return;
	}

	ECodeError vRet = noError;
	if (pComponent) {
		//Set parameter value
		bool vOk = false;
		if (isTimeSeriesParam(a_SettingName))
		{
			pComponent->setTimeSeriesName(a_SettingName, CairnAPIUtils::getParamValue(a_SettingValue));
			vOk = true;
		}
		else {
			vOk = CairnAPIUtils::setParameter(get_InputParams(), a_SettingName, a_SettingValue);			
		}

		/*
		*  It is problematic as parameters can be set 
		*  before setting the EnergyVectors of all default ports 
		* 
			if (a_SettingName == "NbInputFlux") {  
				//Re-declare InputFluxIOs
				pComponent->compoModel()->declareInputFluxIOs();
			}
			if (a_SettingName == "NbOutputFlux") {
				//Re-declare OutputFluxIOs
				pComponent->compoModel()->declareOutputFluxIOs();
			}
		*
		*/

		if (a_SettingName == "NbInputFlux" || a_SettingName == "NbOutputFlux") {
			pComponent->declareIOVariables();
		}

		/* Update MilpComponent::mComponent as it is used to re - initialize the component parameters */
		pComponent->updateCompoParamMap(a_SettingName, a_SettingValue);

		vRet = (vOk || !checkExistance) ? noError : errParam;
	}
	CairnAPIUtils::setError(vRet);
}

void CairnAPI::MilpComponentAPI::set_SettingValues(const t_dict& a_SettingValues)
{
	checkDefaultPortCarriers();
	MilpComponent* pComponent = get_MilpComponent();
	/* Changing ModelClass requires deleting then creating a new mCompoModel */
	t_dict::const_iterator paramModelClass = a_SettingValues.find("ModelClass");
	if (paramModelClass != a_SettingValues.end()) {
		set_SettingValue(paramModelClass->first, paramModelClass->second, false);
	}

	/* NbInputFlux and NbOutputFlux should be set before the other parameters because 
	*  they are essential configuration parameters. Some parameters are (re-)declared 
	*  when the value of NbInputFlux or NbOutputFlux is changed. Those potential new  
	*  parameters cannot be set at the same time, otherwise.
	*/
	t_dict::const_iterator paramNbInputFlux = a_SettingValues.find("NbInputFlux");
	if (paramNbInputFlux != a_SettingValues.end()) {
		set_SettingValue(paramNbInputFlux->first, paramNbInputFlux->second, false);
	}
	t_dict::const_iterator paramNbOutputFlux = a_SettingValues.find("NbOutputFlux");
	if (paramNbOutputFlux != a_SettingValues.end()) {
		set_SettingValue(paramNbOutputFlux->first, paramNbOutputFlux->second, false);
	}

	for (auto& vParam : a_SettingValues) {
		if (vParam.first ==  "ModelClass" || vParam.first == "NbInputFlux" || vParam.first == "NbOutputFlux")
		{
			//already set
			continue;
		}
		set_SettingValue(vParam.first, vParam.second, false);
	}
}

void CairnAPI::MilpComponentAPI::set_TimeSeriesVector(const std::string& a_TimeSeriesName, const std::vector<double> a_TimeSeriesValue)
{
	checkDefaultPortCarriers();
	MilpComponent* pComponent = get_MilpComponent();
	if (pComponent) {
		if (isTimeSeriesParam(a_TimeSeriesName))
		{
			bool vOk = CairnAPIUtils::setParameter({
				pComponent->compoModel()->getInputTimeSeries(), 
				pComponent->compoModel()->getInputPortImpactsParamTS()
				}, 
				a_TimeSeriesName, a_TimeSeriesValue);

			if (!vOk) {
				CairnAPIUtils::setError(errParam);
			}
		}
		else {
			CairnAPIUtils::setError(errDefault, a_TimeSeriesName + " is not a valid timeseries of the componenet " + get_Name());
		}
	}
	else {
		CairnAPIUtils::setError(errDefault, get_Name() + " must be added to the problem before setting its timeseries!");
	}
}

// -- IOs --
t_list CairnAPI::MilpComponentAPI::get_VarList()
{
	checkDefaultPortCarriers();
	MilpComponent* pComponent = get_MilpComponent();
	t_list vRet = {};
	if (pComponent) {
		// le composant existe, retourne une liste des variables (model expressions)		
		const SubModel::t_mapIOs& vIOMap = pComponent->compoModel()->getMapIOExpression();
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

t_value CairnAPI::MilpComponentAPI::get_varValue(const std::string& a_VarName)
{
	checkDefaultPortCarriers();
	MilpComponent* pComponent = get_MilpComponent();
	t_value vRet = NAN;
	if (pComponent) {
		std::string vVarName(a_VarName.c_str());
		ModelIO* vIO = pComponent->compoModel()->getIOExpression(vVarName);
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

t_dict CairnAPI::MilpComponentAPI::get_varValues()
{
	checkDefaultPortCarriers();
	
	t_dict vRet = {};
	t_list varList = get_VarList();
	for (auto& varName : varList) {
		vRet[varName] = get_varValue(varName);
	}
	return vRet;
}

// -- Ports ---
std::map<std::string, std::string>
CairnAPI::MilpComponentAPI::get_DefaultPortData(const std::string& portId) const
{
	const MilpComponent* pCompo = get_MilpComponent();
	if (!pCompo)
		return {};

	const SubModel* subModel = pCompo->compoModel();
	if (!subModel)
		return {};

	return subModel->getDefaultPortData(portId);
}

t_list CairnAPI::MilpComponentAPI::get_DefaultPortIDs() const
{
	t_list vRet = {};
	MilpComponent* pComponent = get_MilpComponent();
	if (pComponent) {
		for (MilpPort* vPort : pComponent->PortList())
		{
			if (vPort->IsDefaultPort()) {
				vRet.push_back(vPort->ID());
			}
		}
	}
	return vRet;
}

t_list CairnAPI::MilpComponentAPI::get_DefaultPorts() const 
{
	t_list vRet = {};
	MilpComponent* pComponent = get_MilpComponent();
	if (pComponent) {
		for(MilpPort * vPort: pComponent->PortList())
		{
			if (vPort->IsDefaultPort()) {
				vRet.push_back(vPort->Name());
			}
		}
	}
	return vRet;
}

t_list CairnAPI::MilpComponentAPI::get_Ports() const
{
	t_list vRet = {};
	MilpComponent* pComponent = get_MilpComponent();
	if (pComponent) {
		for(MilpPort* vPort: pComponent->PortList())
		{
			vRet.push_back(vPort->Name());
		}
	}
	return vRet;
}

CairnAPI::MilpPortAPI CairnAPI::MilpComponentAPI::get_Port(const std::string& a_Name)
{
	MilpPortAPI vRet;
	MilpComponent* pComponent = get_MilpComponent();
	std::string vPortName = std::string(a_Name.c_str());
	if (pComponent) {
		// Get port by Id
		MilpPort* vPort = pComponent->getPort(vPortName);  

		if (!vPort) {
			// Get port by name
			vPort = pComponent->getPortByName(vPortName);
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

CairnAPI::MilpPortAPI CairnAPI::MilpComponentAPI::add_Port(const std::string& a_PortName, const EnergyVectorAPI& a_EnergyVector,
	const std::string& a_Direction, const std::string& a_Variable, const std::string& a_PortId, const bool& reinitializeCompo)
{
	MilpPortAPI vPort;
	MilpComponent* pComponent = get_MilpComponent();
	ECodeError vErr = noError;
	std::string vErrMsg = "";
	if (pComponent) {
		//check if the port with the same name already exist		
		MilpPort* vMilpPort = pComponent->getPortByName(a_PortName);
		if (vMilpPort) {
			vErr = errAlreadyExist;
			vErrMsg = "port " + a_PortName;
		}
		else {
			std::string vComponentName(get_Name());
			std::string vPortId = a_PortId;
			if(vPortId.empty()) vPortId = pComponent->getUniquePortID();
			std::map<std::string, std::string> vPortParams;
			vPortParams["CompoName"] = vComponentName;
			vPortParams["Name"] = a_PortName;
			vPortParams["Carrier"] = a_EnergyVector.get_Name();
			vPortParams["Direction"] = CairnUtils::toUpper(a_Direction);
			vPortParams["Variable"] = a_Variable;

			//create port
			pComponent->createOnePort(vPortId, vPortParams);
			vMilpPort = pComponent->getPort(vPortId);  
			if (vMilpPort) {
				vPort.set_MilpPort(vMilpPort);
				vPort.set_EnergyCarrier(a_EnergyVector.get_EnergyVector());
				if (reinitializeCompo) {
					redeclarePortImpactParameters();
					if (auto* pModel = pComponent->compoModel()) {
						if (pModel->getMIPExpression1D(vMilpPort->Variable())) {
							pComponent->declareIndicators(); // improve ?!
						}
					}
				}
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


void CairnAPI::MilpComponentAPI::redeclarePortImpactParameters()
{
	MilpComponent* pComponent = get_MilpComponent();
	if (!pComponent) return;

	TechnicalSubModel* pTechnicalModel = dynamic_cast<TechnicalSubModel*> (pComponent->compoModel());
	if (!pTechnicalModel) return;

	// Save a copy of param values before redeclaration because a reallocation of 
	// EnvImpact param vectors may happen, and thus the values of the existing parameters may lost. 
	t_dict paramMap = get_SettingValues();

	// Filter for port EnvImpact params
	std::vector< std::string> portNames = get_Ports();
	t_dict portParamMap;
	for (const auto& [key, val] : paramMap) {
		// Check if key contains any port name
		bool isPortParam = std::any_of(portNames.begin(), portNames.end(),
			[&](std::string portName) {
				return CairnUtils::contains(key, portName);
			});
		if (isPortParam) {
			portParamMap.emplace(key, val);
		}
	}

	//Only add the parameters related to new port?! => results in a problem due to reallocation!
	for (const auto& impact : pTechnicalModel->getEnvImpacts()) {
		impact->resizeCoeffs(pTechnicalModel->PortList().size()); //results in vectors reallocation!!
		std::size_t j = 0;
		for (const auto& port : pTechnicalModel->PortList()) {
			impact->addConfigParameters(port->Name(), j);
			impact->addPortParameters(port->Name(), j, pTechnicalModel->getMainCarrier());
			j++;
		}
	}
	set_SettingValues(portParamMap);
}

bool CairnAPI::MilpComponentAPI::remove_Port(MilpPortAPI& a_Port, const bool isDeleteCompo)
{
	ECodeError vErr = noError;
	std::string vErrMsg = "";
	MilpComponent* pComponent = get_MilpComponent();
	if (pComponent) {		
		std::string vPortName(a_Port.get_Name());
		MilpPort* pMilpPort = pComponent->getPortByName(vPortName);
		if (pMilpPort) {
			//check if it is a default port
			if (!isDeleteCompo && pMilpPort->IsDefaultPort()) {
				cInfo() << "Port "+ vPortName + " of component " + pComponent->Name() + " is a default port."
					+ " It is not possible to delete default ports!";
				return false;
			}
			else {
				//check if there is a link or Not
				BusCompo* vBus = pMilpPort->getLinkedBus();
				if (vBus) {
					vBus->removeLink(pComponent, pMilpPort);
				}
				pComponent->removePort(pMilpPort);
				if (!isDeleteCompo) {
					removePortImpactParameters(vPortName);
					pComponent->declareIndicators(); // improve ?!
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

void CairnAPI::MilpComponentAPI::removePortImpactParameters(const std::string& portName)
{
	MilpComponent* pComponent = get_MilpComponent();
	if (!pComponent) return;

	TechnicalSubModel* pTechnicalModel = dynamic_cast<TechnicalSubModel*> (pComponent->compoModel());
	if (pTechnicalModel) {
		pTechnicalModel->getInputPortImpactsParam()->removePortImpactParameters(portName);
		pTechnicalModel->getInputPortImpactsParamTS()->removePortImpactParameters(portName);
	}
}

bool CairnAPI::MilpComponentAPI::useEnergyVector(const std::string& a_EnergyCarrierName)
{
	bool vRet = false;
	t_list vPorts = get_Ports();
	for (auto& vPort : vPorts) {
		if (get_Port(vPort).get_CarrierName() == a_EnergyCarrierName) {
			vRet = true;
			break;
		}
	}
	return vRet;
}

void CairnAPI::MilpComponentAPI::get_Links(t_dict& a_Links)
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

// -- Indicators ---
t_list CairnAPI::MilpComponentAPI::get_IndicatorNames()
{
	checkDefaultPortCarriers();

	t_list vRet = {};
	MilpComponent* pComponent = get_MilpComponent();
	if (pComponent) {
		InputParam::t_Indicators vectIndicators = pComponent->compoModel()->getInputIndicators()->getIndicators();
		for (int i = 0; i < vectIndicators.size(); i++)
		{
			vRet.push_back(vectIndicators[i]->getName());
		}
	}
	else {
		CairnAPIUtils::setError(noCairn);
	}
	return vRet;
}

t_list CairnAPI::MilpComponentAPI::get_IndicatorUnits()
{
	checkDefaultPortCarriers();

	t_list vRet = {};
	MilpComponent* pComponent = get_MilpComponent();
	if (pComponent) {
		InputParam::t_Indicators vectIndicators = pComponent->compoModel()->getInputIndicators()->getIndicators();
		for (int i = 0; i < vectIndicators.size(); i++)
		{
			vRet.push_back(vectIndicators[i]->getUnit());
		}
	}
	else {
		CairnAPIUtils::setError(noCairn);
	}
	return vRet;
}

t_list CairnAPI::MilpComponentAPI::get_IndicatorShortNames()
{
	checkDefaultPortCarriers();

	t_list vRet = {};
	MilpComponent* pComponent = get_MilpComponent();
	if (pComponent) {
		InputParam::t_Indicators vectIndicators = pComponent->compoModel()->getInputIndicators()->getIndicators();
		for (int i = 0; i < vectIndicators.size(); i++)
		{
			vRet.push_back(vectIndicators[i]->getShortName());
		}
	}
	else {
		CairnAPIUtils::setError(noCairn);
	}
	return vRet;
}

t_dict CairnAPI::MilpComponentAPI::get_IndicatorValues(const std::string& range) const
{
	checkDefaultPortCarriers();

	t_dict result;

	MilpComponent* pComponent = get_MilpComponent();
	if (!pComponent) {
		CairnAPIUtils::setError(noCairn, "Component not available");
		return result;  // Return empty dict
	}

	// Get all indicators
	const auto& indicatorList = pComponent->compoModel()->getInputIndicators()->getIndicators();

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

double CairnAPI::MilpComponentAPI::get_IndicatorValue(const std::string& name, const std::string& range) const  
{
	checkDefaultPortCarriers();

	MilpComponent* pComponent = get_MilpComponent();
	if (!pComponent) {
		CairnAPIUtils::setError(noCairn, "Component not available");
		return std::numeric_limits<double>::quiet_NaN(); 
	}

	auto value = pComponent->getIndicatorValue(name, range);
	if (value) {
		return *value;
	}

	CairnAPIUtils::setError(errGet, "Indicator '" + name + "' not found");
	return std::numeric_limits<double>::quiet_NaN();
}

bool CairnAPI::MilpComponentAPI::isInstalled() const {
	MilpComponent* pComponent = get_MilpComponent();
	if (!pComponent) {
		CairnAPIUtils::setError(noCairn, "Component not available");
		return std::numeric_limits<double>::quiet_NaN();
	}
	return pComponent->isInstalled();
}

t_value CairnAPI::MilpComponentAPI::isOptimized() {
	checkDefaultPortCarriers();
	MilpComponent* pComponent = get_MilpComponent();
	return int(pComponent->compoModel()->isSizeOptimized());
}

t_value CairnAPI::MilpComponentAPI::get_dimParam() {
	checkDefaultPortCarriers();
	MilpComponent* pComponent = get_MilpComponent();
	return (pComponent->compoModel()->getOptimalSizeExpression());
}

std::vector<double> CairnAPI::MilpComponentAPI::getControlVarHistValues(const std::string& a_name)
{
	std::vector<double> vRet(0);
	MilpComponent* pComponent = get_MilpComponent();
	if (pComponent) {
		const SubModel::t_mapRHs& sub = pComponent->compoModel()->getListControlIO();
		SubModel::t_mapRHs::const_iterator vIter = sub.find(a_name);
		if (vIter != sub.end()) {
			vRet = vIter->second->getValues();
			return vRet;
		}
		else {
			CairnAPIUtils::setError(errNotFound, "Control variable " + a_name);
			return vRet;
		}
	}
	CairnAPIUtils::setError(noCairn, "Component not available");
	return vRet;
}
