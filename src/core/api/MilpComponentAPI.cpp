#include "CairnAPIUtils.h"
#include "CairnCore.h"
#include "OptimProblem.h"
#include "Cairn_Exception.h"
#include "BusCompo.h"
#include "CairnAPI.h"

using namespace CairnAPIUtils;

CairnAPI::MilpComponentAPI::MilpComponentAPI(MilpComponent* ap_Component)
{	
	set_MilpComponent(ap_Component);
}

CairnAPI::MilpComponentAPI::MilpComponentAPI(const OptimProblemAPI& a_Problem, const std::string& a_Name, const std::string& a_ModelName)
{
	*this = a_Problem.create_Component(a_Name, a_ModelName);
}

MilpComponent* CairnAPI::MilpComponentAPI::get_MilpComponent() const
{
	return m_Component;
}

void CairnAPI::MilpComponentAPI::set_MilpComponent(MilpComponent* ap_Component)
{
	m_Component = ap_Component;
}

std::string CairnAPI::MilpComponentAPI::get_Name() const
{
	std::string name = "";
	if (m_Component) {
		name = m_Component->Name();
	}
	return name;
}

std::string CairnAPI::MilpComponentAPI::get_Type() const
{
	std::string type = "";
	if (m_Component) {
		type = m_Component->Type();
	}
	return type;
}

const std::string CairnAPI::MilpComponentAPI::get_ModelClass()
{
	std::string modelClass = "";
	if (m_Component) {
		modelClass = m_Component->ModelClassName();
	}
	return modelClass;
}

void CairnAPI::MilpComponentAPI::rename(const std::string& name)
{
	if (m_Component) {
		m_Component->setName((name));
	}
}

std::string CairnAPI::MilpComponentAPI::get_Direction()
{
	std::string vErrMsg = "Not valid: component " + get_Name() + " of type " + get_Type() + " doesn't have a direction!";
	if (get_Type() == "Grid")
	{
		vErrMsg = "Error: not able to obtain the direction of Grid " + get_Name();
		if (m_Component)
		{
			GridSubModel* gridModel = dynamic_cast<GridSubModel*> (m_Component->compoModel());
			if (gridModel) {
				if (gridModel->Sens() < 0) return "InjectToGrid"; //default port is INPUT
				else return "ExtractFromGrid"; //default port is OUTPUT or DATAEXCHANGE
			}
		}
	}
	else if (get_Type() == "SourceLoad")
	{
		vErrMsg = "Error: not able to obtain the direction of SourceLoad " + get_Name();
		if (m_Component)
		{
			SourceLoadSubModel* sourceLoadModel = dynamic_cast<SourceLoadSubModel*> (m_Component->compoModel());
			if (sourceLoadModel) {
				if (sourceLoadModel->Sens() < 0) return "Load"; //default port is INPUT
				else return "Source"; //default port is OUTPUT or DATAEXCHANGE
			}
		}
	}
	CairnAPIUtils::setError(errDefault, vErrMsg);
	return "";
}

std::string CairnAPI::MilpComponentAPI::get_LabelValue(const std::string& a_Label) const
{
	std::string vRet = "";
	if (m_Component)
	{
		OptimProblem* vOptimProblem = (OptimProblem*)m_Component->parent();
		TecEcoAnalysis* vTecEcoAnalysis = vOptimProblem->getTecEcoAnalysis();
		if (vTecEcoAnalysis->isValidLabel(a_Label)) {
			vRet = m_Component->compoModel()->getLabelValue(a_Label);
		}
		else {
			CairnAPIUtils::setError(errDefault, "Label " + a_Label + " is not defined. Please, add the label to the problem first!");
		}
	}
	return vRet;
}

std::map<std::string, std::string> CairnAPI::MilpComponentAPI::get_LabelValues() const
{
	std::map<std::string, std::string> vRet = {};
	if (m_Component)
	{
		//return m_Component->compoModel()->getLabelMap();

		OptimProblem* vOptimProblem = (OptimProblem*)m_Component->parent();
		TecEcoAnalysis* vTecEcoAnalysis = vOptimProblem->getTecEcoAnalysis();
		if (vTecEcoAnalysis) {
			for (auto const& label : vTecEcoAnalysis->getLabelList())//referance list
			{
				vRet[label] = m_Component->compoModel()->getLabelValue(label);
			}
		}
	}
	return vRet;
}

void CairnAPI::MilpComponentAPI::set_LabelValue(const std::string& a_Label, const std::string& a_Value)
{
	if (m_Component)
	{
		OptimProblem* vOptimProblem = (OptimProblem*)m_Component->parent();
		TecEcoAnalysis* vTecEcoAnalysis = vOptimProblem->getTecEcoAnalysis();
		if (vTecEcoAnalysis->isValidLabel(a_Label)) {
			m_Component->compoModel()->setLabel(a_Label, a_Value);
		}
		else {
			CairnAPIUtils::setError(errDefault, "Label " + a_Label + " is not defined. Please, add the label to the problem first!");
		}
	}
}

void CairnAPI::MilpComponentAPI::set_LabelValues(const std::map<std::string, std::string>& a_Labels)
{

	if (m_Component)
	{
		/*
		* No need to verify that all the labels are valid. It is ok!
		* Labels are filtered when using get_method and when writing to json file
		*/
		m_Component->compoModel()->setLabelMap(a_Labels);
	}
}

void CairnAPI::MilpComponentAPI::checkDefaultPortCarriers()
{
	if (!m_Component->allDefaultPortsHaveCarriers()) {
		CairnAPIUtils::setError(errDefault, "Please, configure the carriers of all default ports of component " + get_Name() + " first.");
	}
}

bool CairnAPI::MilpComponentAPI::isTimeSeriesParam(const std::string& a_TimeSeriesName)
{
	checkDefaultPortCarriers();

	t_value tsValue;
	if (m_Component) {
		if (m_Component->compoModel()->getInputDataTS()->getParameterValue(std::string(a_TimeSeriesName.c_str()), tsValue)
			|| m_Component->compoModel()->getInputPortImpactsParamTS()->getParameterValue(std::string(a_TimeSeriesName.c_str()), tsValue))
		{
			return true;
		}
	}
	return false;
}

bool CairnAPI::MilpComponentAPI::get_SettingMandatoryValue(const std::string& a_SettingName)
{
	checkDefaultPortCarriers();

	bool vRet = true;
	if (m_Component) {
		vRet = CairnAPIUtils::getParamMandatoryValue({
			m_Component->compoModel()->getInputParam(), // params	 
			m_Component->getCompoInputParam(),
			m_Component->compoModel()->getInputDataTS(), // timeseries			
			m_Component->compoModel()->getInputEnvImpactsParam(),
			m_Component->compoModel()->getInputPortImpactsParam(),
			m_Component->compoModel()->getInputPortImpactsParamTS(),
			m_Component->getGUIData()->getGuiInputParam() //GuiData 
			},
			a_SettingName);
	}
	return vRet;
}

bool CairnAPI::MilpComponentAPI::is_DependentSetting(const std::string& a_SettingName)
{
	bool vRet = false;
	if (m_Component) {
		vRet = CairnAPIUtils::isDependentParam({
			m_Component->compoModel()->getInputParam(), // params	 
			m_Component->getCompoInputParam(),
			m_Component->compoModel()->getInputDataTS(), // timeseries			
			m_Component->compoModel()->getInputEnvImpactsParam(),
			m_Component->compoModel()->getInputPortImpactsParam(),
			m_Component->compoModel()->getInputPortImpactsParamTS(),
			m_Component->getGUIData()->getGuiInputParam() //GuiData 
			},
			a_SettingName);
	}
	return vRet;
}

std::string CairnAPI::MilpComponentAPI::get_SettingUnit(const std::string& a_SettingName)
{
	checkDefaultPortCarriers();

	std::string vRet = "-";
	if (m_Component) {
		vRet = CairnAPIUtils::getParamUnit({
			m_Component->compoModel()->getInputParam(), // params	 
			m_Component->getCompoInputParam(),
			m_Component->compoModel()->getInputDataTS(), // timeseries			
			m_Component->compoModel()->getInputEnvImpactsParam(),
			m_Component->compoModel()->getInputPortImpactsParam(),
			m_Component->compoModel()->getInputPortImpactsParamTS(),
			m_Component->getGUIData()->getGuiInputParam() //GuiData 
			},
			a_SettingName);
	}
	return vRet;
}

std::string CairnAPI::MilpComponentAPI::get_SettingShowConfig(const std::string& a_SettingName)
{
	checkDefaultPortCarriers();

	std::string vRet = "";
	if (m_Component) {
		vRet = CairnAPIUtils::getParamShowConfig({
			m_Component->compoModel()->getInputParam(), // params	 
			m_Component->getCompoInputParam(),
			m_Component->compoModel()->getInputDataTS(), // timeseries			
			m_Component->compoModel()->getInputEnvImpactsParam(),
			m_Component->compoModel()->getInputPortImpactsParam(),
			m_Component->compoModel()->getInputPortImpactsParamTS(),
			m_Component->getGUIData()->getGuiInputParam() },
			a_SettingName);
	}
	return vRet;
}

t_list CairnAPI::MilpComponentAPI::get_ShowConfigList()
{
	checkDefaultPortCarriers();

	t_list vRet = {};
	if (m_Component) {
		vRet = CairnAPIUtils::getShowConfigList({
			m_Component->compoModel()->getInputParam(), // params	 
			m_Component->getCompoInputParam(),
			m_Component->compoModel()->getInputDataTS(), // timeseries			
			m_Component->compoModel()->getInputEnvImpactsParam(),
			m_Component->compoModel()->getInputPortImpactsParam(),
			m_Component->compoModel()->getInputPortImpactsParamTS(),
			m_Component->getGUIData()->getGuiInputParam() }
		);
	}
	return vRet;
}

t_value CairnAPI::MilpComponentAPI::get_OptimalSizeExpression()
{
	checkDefaultPortCarriers();

	t_value vRet = "";
	if (m_Component) {
		vRet = m_Component->compoModel()->getOptimalSizeExpression();
	}
	return vRet;
}

t_list CairnAPI::MilpComponentAPI::get_SettingsList()
{
	/*
	* property in CairnBind.cpp
	*/
	return get_SettingsListByType(ESettingsLimited::all);
}

t_list CairnAPI::MilpComponentAPI::get_SettingsListByType(ESettingsLimited a_setLimited)
{
	checkDefaultPortCarriers();

	t_list vRet = {};
	if (m_Component) {
		vRet = CairnAPIUtils::getParametersName({
			m_Component->compoModel()->getInputParam(), // params	
			m_Component->getCompoInputParam(),
			m_Component->compoModel()->getInputDataTS(), // timeseries			
			m_Component->compoModel()->getInputEnvImpactsParam(),
			m_Component->compoModel()->getInputPortImpactsParam(),
			m_Component->compoModel()->getInputPortImpactsParamTS(),
			m_Component->getGUIData()->getGuiInputParam() }
		, a_setLimited);
	}
	return vRet;
}

t_value CairnAPI::MilpComponentAPI::get_SettingValue(const std::string& a_SettingName)
{
	checkDefaultPortCarriers();

	t_value vRet = "";
	if (m_Component) {
		if (isTimeSeriesParam(a_SettingName))
		{
			//timeseries: return the name (value) of the timeseries; vRet is the vector value!
			return m_Component->getTimeSeriesName(std::string(a_SettingName.c_str()));
		}
		else {
			vRet = CairnAPIUtils::getParameter({
			m_Component->compoModel()->getInputParam(), // params	
			m_Component->getCompoInputParam(), // options
			m_Component->compoModel()->getInputEnvImpactsParam(),
			m_Component->compoModel()->getInputPortImpactsParam(),
			m_Component->getGUIData()->getGuiInputParam() }
			, a_SettingName);
		}
	}
	return vRet;
}

t_dict CairnAPI::MilpComponentAPI::get_SettingValues()
{
	checkDefaultPortCarriers();
	t_dict vRet = {};
	if (m_Component) {
		CairnAPIUtils::getParameters({
			m_Component->compoModel()->getInputParam(), // params	
			m_Component->getCompoInputParam(), // options
			m_Component->compoModel()->getInputDataTS(), // timeseries			
			m_Component->compoModel()->getInputEnvImpactsParam(),
			m_Component->compoModel()->getInputPortImpactsParam(),
			m_Component->getGUIData()->getGuiInputParam() }
		, vRet);
		//Add timeseries names (not vector values)	
		std::vector<std::string> tsList;
		m_Component->compoModel()->getInputDataTS()->getParameters(tsList);
		m_Component->compoModel()->getInputPortImpactsParamTS()->getParameters(tsList);
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
	if (m_Component) {
		t_value vRet;
		if (m_Component->compoModel()->getInputDataTS()->getParameterValue(std::string(a_SettingName.c_str()), vRet)
			|| m_Component->compoModel()->getInputPortImpactsParamTS()->getParameterValue(std::string(a_SettingName.c_str()), vRet))
		{
			return vRet;
		}
	}
	return {};
}

void CairnAPI::MilpComponentAPI::modify_ModelClass(const std::string& a_prevModelClass, const std::string& a_newModelClass)
{
	checkDefaultPortCarriers();

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
			bool vOk = CairnAPIUtils::setParameter({
				m_Component->compoModel()->getInputParam(), // params	
				m_Component->getCompoInputParam(), // options
				m_Component->compoModel()->getInputEnvImpactsParam(),
				m_Component->compoModel()->getInputPortImpactsParam(),
				m_Component->getGUIData()->getGuiInputParam() }, "ModelClass", a_newModelClass);
			m_Component->updateCompoParamMap("ModelClass", a_newModelClass);
			if (!vOk) CairnAPIUtils::setError(errParam);

			//Save a copy of param values 
			t_dict paramMap = get_SettingValues();
			std::map<std::string, std::string> labelMap = get_LabelValues();

			//Delete then create a new compoModel (new ModelClass)
			m_Component->deleteCompoModel();
			m_Component->createCompoModel();

			//Re-create and re-configure the ports
			OptimProblem* vOptimProblem = (OptimProblem*)m_Component->parent();
			CairnAPI::OptimProblemAPI vOptimProblemAPI;
			vOptimProblemAPI.set_Problem(vOptimProblem);

			t_list vDefaultPortNames = get_DefaultPorts();
			for (auto& [vPortName, vParams] : vPortsData) {
				auto it = find(vDefaultPortNames.begin(), vDefaultPortNames.end(), vPortName);
				if (it != vDefaultPortNames.end()) {
					//A default port
					CairnAPI::MilpPortAPI vDefaultPort = get_Port(vPortName);
					vDefaultPort.set_SettingValues(vPortsData[vPortName]);
					vDefaultPort.set_EnergyCarrier(vPortCarriers[vPortName]);
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
					CairnAPI::MilpPortAPI vPort = add_Port(vPortName, vPortCarriers[vPortName], "", "", false);
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
			m_Component->initProblem(false);

			//set param values after re-initialization
			set_SettingValues(paramMap);
			set_LabelValues(labelMap);
		}
		catch (...) {
			CairnAPIUtils::setError(errDefault, "Error while trying to change the ModelClass to " + a_prevModelClass + " to " + a_newModelClass);
		}
	}
}

void CairnAPI::MilpComponentAPI::set_SettingValue(const std::string& a_SettingName, const t_value& a_SettingValue, const bool& checkExistance)
{
	checkDefaultPortCarriers();

	if (a_SettingName == "ModelClass") {
		t_value prevModelClass = get_SettingValue("ModelClass");
		modify_ModelClass(CairnAPIUtils::getParamValue(prevModelClass), CairnAPIUtils::getParamValue(a_SettingValue));
		return;
	}

	ECodeError vRet = noError;
	if (m_Component) {
		//Set parameter value
		bool vOk = false;
		if (isTimeSeriesParam(a_SettingName))
		{
			m_Component->setTimeSeriesName(std::string(a_SettingName.c_str()), std::string(CairnAPIUtils::getParamValue(a_SettingValue).c_str()));
			vOk = true;
		}
		else {
			vOk = CairnAPIUtils::setParameter({
				m_Component->compoModel()->getInputParam(), // params	
				m_Component->getCompoInputParam(), // options
				m_Component->compoModel()->getInputEnvImpactsParam(),
				m_Component->compoModel()->getInputPortImpactsParam(),
				m_Component->getGUIData()->getGuiInputParam() }
			, a_SettingName, a_SettingValue);
		}

		/*
		*  It is problematic as parameters can be set 
		*  before setting the EnergyVectors of all default ports 
		* 
			if (a_SettingName == "NbInputFlux") {  
				//Re-declare InputFluxIOs
				m_Component->compoModel()->declareInputFluxIOs();
			}
			if (a_SettingName == "NbOutputFlux") {
				//Re-declare OutputFluxIOs
				m_Component->compoModel()->declareOutputFluxIOs();
			}
		*
		*/

		if (a_SettingName == "NbInputFlux" || a_SettingName == "NbOutputFlux") {
			m_Component->declareIOVariables();
		}

		/* Update MilpComponent::mComponent as it is used to re - initialize the component parameters */
		m_Component->updateCompoParamMap(a_SettingName, a_SettingValue);

		vRet = (vOk || !checkExistance) ? noError : errParam;
	}
	CairnAPIUtils::setError(vRet);
}

void CairnAPI::MilpComponentAPI::set_SettingValues(const t_dict& a_SettingValues)
{
	checkDefaultPortCarriers();

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
	if (m_Component) {
		if (isTimeSeriesParam(a_TimeSeriesName))
		{
			bool vOk = CairnAPIUtils::setParameter({
				m_Component->compoModel()->getInputDataTS(), 
				m_Component->compoModel()->getInputPortImpactsParamTS()
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

	t_list vRet = {};
	if (m_Component) {
		// le composant existe, retourne une liste des variables (model expressions)		
		const SubModel::t_mapIOs& vIOMap = m_Component->compoModel()->getMapIOExpression();
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

	t_value vRet = NAN;
	if (m_Component) {
		std::string vVarName(a_VarName.c_str());
		ModelIO* vIO = m_Component->compoModel()->getIOExpression(vVarName);
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
t_list CairnAPI::MilpComponentAPI::get_DefaultPorts() {
	t_list vRet = {};
	if (m_Component) {
		for(MilpPort * vPort: m_Component->PortList())
		{
			if (vPort->IsDefaultPort()) {
				vRet.push_back(vPort->Name());
			}
		}
	}
	return vRet;
}

t_list CairnAPI::MilpComponentAPI::get_Ports()
{
	t_list vRet = {};
	if (m_Component) {
		for(MilpPort* vPort: m_Component->PortList())
		{
			vRet.push_back(vPort->Name());
		}
	}
	return vRet;
}

CairnAPI::MilpPortAPI CairnAPI::MilpComponentAPI::get_Port(const std::string& a_Name)
{
	MilpPortAPI vRet;
	std::string vPortName = std::string(a_Name.c_str());
	if (m_Component) {
		MilpPort* vPort = m_Component->getPortByName(vPortName);
		if (vPort) {
			vRet.set_MilpPort(vPort);
		}
		else {
			CairnAPIUtils::setError(errNotFound, "port " + a_Name);
		}
	}
	return vRet;
}

CairnAPI::MilpPortAPI CairnAPI::MilpComponentAPI::add_Port(const std::string& a_Name, const EnergyVectorAPI& a_EnergyVector,
	const std::string& a_Direction, const std::string& a_Variable, const bool& reinitializeCompo)
{
	MilpPortAPI vPort;
	ECodeError vErr = noError;
	std::string vErrMsg = "";
	std::string vPortName = a_Name;
	if (m_Component) {
		//check if the port with the same name already exist		
		MilpPort* vMilpPort = m_Component->getPortByName(vPortName);
		if (vMilpPort) {
			vErr = errAlreadyExist;
			vErrMsg = "port " + a_Name;
		}
		else {
			std::string vComponentName(get_Name());
			std::string vPortId = m_Component->getUniquePortID();
			std::map<std::string, std::string> vPortParams;
			vPortParams["CompoName"] = vComponentName;
			vPortParams["Name"] = vPortName;
			vPortParams["Carrier"] = a_EnergyVector.get_Name();
			vPortParams["Direction"] = CairnUtils::toUpper(a_Direction);
			vPortParams["Variable"] = a_Variable;

			//create port
			m_Component->createOnePort(vPortId, vPortParams);
			vMilpPort = m_Component->getPort(vPortId);  
			if (vMilpPort) {
				vPort.set_MilpPort(vMilpPort);
				vPort.set_EnergyCarrier(a_EnergyVector.get_EnergyVector());
				if (reinitializeCompo) {
					reinitialize();
				}
			}
			else {
				vErr = errAdd;
				vErrMsg = "port " + a_Name;
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

bool CairnAPI::MilpComponentAPI::remove_Port(MilpPortAPI& a_Port, const bool isDeleteCompo)
{
	ECodeError vErr = noError;
	std::string vErrMsg = "";
	if (m_Component) {		
		std::string vPortName(std::string(a_Port.get_Name().c_str()));
		MilpPort* vMilpPort = m_Component->getPortByName(vPortName);
		if (vMilpPort) {
			//check if it is a default port
			if (!isDeleteCompo && vMilpPort->IsDefaultPort()) {
				cInfo() << "Port "+ vPortName + " of component " + m_Component->Name() + " is a default port."
					+ " It is not possible to delete default ports!";
				return false;
			}
			else {
				//check if there is a link or Not
				BusCompo* vBus = dynamic_cast<BusCompo*> (vMilpPort->getLinkedBus());
				if (vBus) {
					vMilpPort->DeleteLinkedBus();
					vBus->DeleteBusPort(vMilpPort);
					vBus->RemoveLinkComponent(m_Component);
				}
				m_Component->removePort(vMilpPort);
				if (!isDeleteCompo) {
					reinitialize();
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

void CairnAPI::MilpComponentAPI::reinitialize() 
{
	//Save a copy of param values before re-initialization
	t_dict paramMap = get_SettingValues();

	//Only if there is a selected Env Impact
	OptimProblem* vProblem = (OptimProblem*)m_Component->parent();
	if (vProblem) {
		TecEcoAnalysis* vTecEcoAnalysis = vProblem->getTecEcoAnalysis();
		if (vTecEcoAnalysis) {
			std::string selectedEnvImpacts;
			vTecEcoAnalysis->getConfigParam()->getParameterValue("ConsideredEnvironmentalImpacts", selectedEnvImpacts, EParamType::eStringList);
			if (selectedEnvImpacts != "") {// !!! This condition should be removed when needed !!!
				m_Component->initProblem(false);
				//set param values after re-initialization
				set_SettingValues(paramMap);
			}
		}
	}
	else {
		CairnAPIUtils::setError(errNotFound, "OptimProblem");
	}
}

bool CairnAPI::MilpComponentAPI::useEnergyVector(const std::string& a_EnergyVectorName)
{
	bool vRet = false;
	t_list vPorts = get_Ports();
	for (auto& vPort : vPorts) {
		if (get_Port(vPort).get_CarrierName() == a_EnergyVectorName) {
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
			BusCompo* vBus = dynamic_cast<BusCompo*> (vPort->getLinkedBus());
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
	if (m_Component) {
		InputParam::t_Indicators vectIndicators = m_Component->compoModel()->getInputIndicators()->getIndicators();
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
	if (m_Component) {
		InputParam::t_Indicators vectIndicators = m_Component->compoModel()->getInputIndicators()->getIndicators();
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
	if (m_Component) {
		InputParam::t_Indicators vectIndicators = m_Component->compoModel()->getInputIndicators()->getIndicators();
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

t_dict CairnAPI::MilpComponentAPI::get_IndicatorValues(const std::string range)
{
	checkDefaultPortCarriers();

	t_dict vRet = {};
	if (m_Component) {
		InputParam::t_Indicators vectIndicators = m_Component->compoModel()->getInputIndicators()->getIndicators();
		for (int i = 0; i < vectIndicators.size(); i++)
		{
			if (range == "PLAN") {
				vRet.insert({ vectIndicators[i]->getName(), vectIndicators[i]->getValue(0) });
			}
			else if (range == "HIST") {
				vRet.insert({ vectIndicators[i]->getName(), vectIndicators[i]->getValue(1) });
			}
		}
	}
	else {
		CairnAPIUtils::setError(noCairn);
	}
	return vRet;
}

double CairnAPI::MilpComponentAPI::get_IndicatorValue(const std::string &name, const std::string range)
{
	checkDefaultPortCarriers();

	double vRet=NAN;
	if (m_Component) {
		InputParam::t_Indicators vectIndicators = m_Component->compoModel()->getInputIndicators()->getIndicators();
		for (int i = 0; i < vectIndicators.size(); i++) {
			if (vectIndicators[i]->getName() == name) {
				if (range == "PLAN") vRet = vectIndicators[i]->getValue(0);
				else if (range == "HIST") vRet = vectIndicators[i]->getValue(1);
			}
		}
		if (isnan(vRet)) CairnAPIUtils::setError(errGet, "Indicator name not found");
	}
	else {
		CairnAPIUtils::setError(noCairn);
	}
	return vRet;
}

t_value CairnAPI::MilpComponentAPI::isOptimized() {
	checkDefaultPortCarriers();
	return int(m_Component->compoModel()->isSizeOptimized());
}

t_value CairnAPI::MilpComponentAPI::get_dimParam() {
	checkDefaultPortCarriers();
	return (m_Component->compoModel()->getOptimalSizeExpression());
}

