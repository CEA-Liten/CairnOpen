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
		name = m_Component->Name().toStdString();
	}
	return name;
}

std::string CairnAPI::MilpComponentAPI::get_Type() const
{
	std::string type = "";
	if (m_Component) {
		type = m_Component->Type().toStdString();
	}
	return type;
}

const std::string CairnAPI::MilpComponentAPI::get_ModelClass()
{
	std::string modelClass = "";
	if (m_Component) {
		modelClass = m_Component->ModelClassName().toStdString();
	}
	return modelClass;
}

bool CairnAPI::MilpComponentAPI::isTimeSeriesParam(const std::string& a_TimeSeriesName)
{
	t_value tsValue;
	if (m_Component) {
		if (m_Component->compoModel()->getInputDataTS()->getParameterValue(QString(a_TimeSeriesName.c_str()), tsValue)
			|| m_Component->compoModel()->getInputPortImpactsParamTS()->getParameterValue(QString(a_TimeSeriesName.c_str()), tsValue))
		{
			return true;
		}
	}
	return false;
}

t_value CairnAPI::MilpComponentAPI::get_ShowConfig(const std::string& a_SettingName)
{
	t_value vRet = "";
	if (m_Component) {
		vRet = CairnAPIUtils::getShowConfig({
			m_Component->compoModel()->getInputParam(), // params	 
			m_Component->getCompoInputParam(),
			m_Component->compoModel()->getInputDataTS(), // timeseries			
			m_Component->compoModel()->getInputEnvImpactsParam(),
			m_Component->compoModel()->getInputPortImpactsParam(),
			m_Component->compoModel()->getInputPortImpactsParamTS() }, 
			a_SettingName);
	}
	return vRet;
}

t_list CairnAPI::MilpComponentAPI::get_ShowConfigList()
{
	t_list vRet = {};
	if (m_Component) {
		vRet = CairnAPIUtils::getShowConfigList({
			m_Component->compoModel()->getInputParam(), // params	 
			m_Component->getCompoInputParam(),
			m_Component->compoModel()->getInputDataTS(), // timeseries			
			m_Component->compoModel()->getInputEnvImpactsParam(),
			m_Component->compoModel()->getInputPortImpactsParam(),
			m_Component->compoModel()->getInputPortImpactsParamTS() }
		);
	}
	return vRet;
}

t_value CairnAPI::MilpComponentAPI::get_OptimalSizeExpression()
{
	t_value vRet = "";
	if (m_Component) {
		vRet = m_Component->compoModel()->getOptimalSizeExpression().toStdString();
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
	t_list vRet = {};
	if (m_Component) {
		vRet = CairnAPIUtils::getParametersName({
			m_Component->compoModel()->getInputParam(), // params	
			m_Component->getCompoInputParam(),
			m_Component->compoModel()->getInputDataTS(), // timeseries			
			m_Component->compoModel()->getInputEnvImpactsParam(),
			m_Component->compoModel()->getInputPortImpactsParam(),
			m_Component->compoModel()->getInputPortImpactsParamTS() } 
		, a_setLimited);
	}
	return vRet;
}

t_value CairnAPI::MilpComponentAPI::get_SettingValue(const std::string& a_SettingName)
{
	t_value vRet = "";
	if (m_Component) {
		if (isTimeSeriesParam(a_SettingName))
		{
			//timeseries: return the name (value) of the timeseries; vRet is the vector value!
			return m_Component->getTimeSeriesName(QString(a_SettingName.c_str())).toStdString();
		}
		else {
			vRet = CairnAPIUtils::getParameter({
			m_Component->compoModel()->getInputParam(), // params	
			m_Component->getCompoInputParam(), // options
			m_Component->compoModel()->getInputEnvImpactsParam(),
			m_Component->compoModel()->getInputPortImpactsParam()}
			, a_SettingName);
		}
	}
	return vRet;
}

t_dict CairnAPI::MilpComponentAPI::get_SettingValues()
{
	t_dict vRet = {};
	if (m_Component) {
		CairnAPIUtils::getParameters({
			m_Component->compoModel()->getInputParam(), // params	
			m_Component->getCompoInputParam(), // options
			m_Component->compoModel()->getInputDataTS(), // timeseries			
			m_Component->compoModel()->getInputEnvImpactsParam(),
			m_Component->compoModel()->getInputPortImpactsParam()}
		, vRet);
		//Add timeseries names (not vector values)	
		QList<QString> tsList;
		m_Component->compoModel()->getInputDataTS()->getParameters(tsList);
		m_Component->compoModel()->getInputPortImpactsParamTS()->getParameters(tsList);
		for (auto& tsParamName : tsList) {
			t_value tsName = get_SettingValue(tsParamName.toStdString());
			vRet[tsParamName.toStdString()] = tsName;
		}
	}
	return vRet;
}

t_value CairnAPI::MilpComponentAPI::get_TimeSeriesVector(const std::string& a_SettingName)
{
	if (m_Component) {
		t_value vRet;
		if (m_Component->compoModel()->getInputDataTS()->getParameterValue(QString(a_SettingName.c_str()), vRet)
			|| m_Component->compoModel()->getInputPortImpactsParamTS()->getParameterValue(QString(a_SettingName.c_str()), vRet))
		{
			return vRet;
		}
	}
	return {};
}

void CairnAPI::MilpComponentAPI::set_SettingValue(const std::string& a_SettingName, const t_value& a_SettingValue, const bool& checkExistance)
{
	ECodeError vRet = noError;
	if (m_Component) {
		//Set parameter value
		bool vOk = false;
		if (isTimeSeriesParam(a_SettingName))
		{
			m_Component->setTimeSeriesName(QString(a_SettingName.c_str()), QString(CairnAPIUtils::getParamValue(a_SettingValue).c_str()));
			vOk = true;
		}
		else {
			vOk = CairnAPIUtils::setParameter({
				m_Component->compoModel()->getInputParam(), // params	
				m_Component->getCompoInputParam(), // options
				m_Component->compoModel()->getInputEnvImpactsParam(),
				m_Component->compoModel()->getInputPortImpactsParam() }
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

		if (a_SettingName == "NbInputFlux" || a_SettingName == "NbOutputFlux") 
		{
			m_Component->declareIOVariables();
		}

		if (a_SettingName == "Direction") {
			m_Component->setCompoSens(QString(CairnAPIUtils::getParamValue(a_SettingValue).c_str()));
		}

		//Update MilpComponent::mComponent as it is used to re-initialize the component parameters
		m_Component->updateCompoParamMap(a_SettingName, a_SettingValue);

		vRet = (vOk || !checkExistance) ? noError : errParam;
	}
	CairnAPIUtils::setError(vRet);
}

void CairnAPI::MilpComponentAPI::set_SettingValues(const t_dict& a_SettingValues)
{
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
		if (vParam.first == "NbInputFlux" || vParam.first == "NbOutputFlux")
		{
			//already set
			continue;
		}
		if (find(CairnAPIUtils::mNonModifiableParams.begin(), CairnAPIUtils::mNonModifiableParams.end(), vParam.first) != CairnAPIUtils::mNonModifiableParams.end())
		{
			//qWarning() << (vParam.first + " cannot be modified!").c_str();
			continue;
		}
		set_SettingValue(vParam.first, vParam.second, false);
	}
}

void CairnAPI::MilpComponentAPI::set_TimeSeriesVector(const std::string& a_TimeSeriesName, const std::vector<double> a_TimeSeriesValue)
{
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
	t_list vRet = {};
	if (m_Component) {
		// le composant existe, retourne une liste des variables (model expressions)		
		const SubModel::t_mapIOs& vIOMap = m_Component->compoModel()->getMapIOExpression();
		for (auto& [vName, vIO] : vIOMap) {
			if (vIO->IsUsed()) {
				vRet.push_back(vName.toStdString());
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
	t_value vRet = NAN;
	if (m_Component) {
		QString vVarName(a_VarName.c_str());
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
		foreach(MilpPort * vPort, m_Component->PortList())
		{
			if (vPort->IsDefaultPort()) {
				vRet.push_back(vPort->Name().toStdString());
			}
		}
	}
	return vRet;
}

t_list CairnAPI::MilpComponentAPI::get_Ports()
{
	t_list vRet = {};
	if (m_Component) {
		foreach(MilpPort* vPort, m_Component->PortList())
		{
			vRet.push_back(vPort->Name().toStdString());
		}
	}
	return vRet;
}

CairnAPI::MilpPortAPI CairnAPI::MilpComponentAPI::get_Port(const std::string& a_Name)
{
	MilpPortAPI vRet;
	QString vPortName = QString(a_Name.c_str());
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
	const std::string& a_Direction, const std::string& a_Variable)
{
	MilpPortAPI vPort;
	ECodeError vErr = noError;
	std::string vErrMsg = "";
	QString vPortName = QString(a_Name.c_str());
	if (m_Component) {
		//check if the port with the same name already exist		
		MilpPort* vMilpPort = m_Component->getPortByName(vPortName);
		if (vMilpPort) {
			vErr = errAlreadyExist;
			vErrMsg = "port " + a_Name;
		}
		else {
			QString vComponentName(QString(get_Name().c_str()));
			QString vPortId = m_Component->getUniquePortID();
			QMap<QString, QString> vPortParams;
			vPortParams["CompoName"] = vComponentName;
			vPortParams["Name"] = vPortName;
			vPortParams["Carrier"] = QString(a_EnergyVector.get_Name().c_str());
			vPortParams["Direction"] = QString(a_Direction.c_str()).toUpper();
			vPortParams["Variable"] = QString(a_Variable.c_str());

			//create port
			m_Component->createOnePort(vPortId, vPortParams);
			vMilpPort = m_Component->getPort(vPortId);  
			if (vMilpPort) {
				vPort.set_MilpPort(vMilpPort);
				vPort.set_EnergyCarrier(a_EnergyVector.get_EnergyVector());
				reinitialize();
				
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
		QString vPortName(QString(a_Port.get_Name().c_str()));
		MilpPort* vMilpPort = m_Component->getPortByName(vPortName);
		if (vMilpPort) {
			//check if it is a default port
			if (!isDeleteCompo && vMilpPort->IsDefaultPort()) {
				qInfo() << "Port "+ vPortName + " of component " + m_Component->Name() + " is a default port."
					+ " It is not possible to delete default ports!";
				return false;
			}
			else {
				//check if there is a link or Not
				BusCompo* vBus = dynamic_cast<BusCompo*> (vMilpPort->ptrLinkedComponent());
				if (vBus) {
					vMilpPort->DeleteptrLinkedComponent();
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
			QString selectedEnvImpacts;
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
			BusCompo* vBus = dynamic_cast<BusCompo*> (vPort->ptrLinkedComponent());
			if (vBus) {
				a_Links[get_Name() + "." + vPortName] = vBus->Name().toStdString();
			}
		}		
	}
}

// -- Indicators ---
t_list CairnAPI::MilpComponentAPI::get_IndicatorNames()
{
	t_list vRet = {};
	if (m_Component) {
		InputParam::t_Indicators vectIndicators = m_Component->compoModel()->getInputIndicators()->getIndicators();
		for (int i = 0; i < vectIndicators.size(); i++)
		{
			vRet.push_back(vectIndicators[i]->getName().toStdString());
		}
	}
	else {
		CairnAPIUtils::setError(noCairn);
	}
	return vRet;
}

t_list CairnAPI::MilpComponentAPI::get_IndicatorUnits()
{
	t_list vRet = {};
	if (m_Component) {
		InputParam::t_Indicators vectIndicators = m_Component->compoModel()->getInputIndicators()->getIndicators();
		for (int i = 0; i < vectIndicators.size(); i++)
		{
			vRet.push_back(vectIndicators[i]->getUnit().toStdString());
		}
	}
	else {
		CairnAPIUtils::setError(noCairn);
	}
	return vRet;
}

t_list CairnAPI::MilpComponentAPI::get_IndicatorShortNames()
{
	t_list vRet = {};
	if (m_Component) {
		InputParam::t_Indicators vectIndicators = m_Component->compoModel()->getInputIndicators()->getIndicators();
		for (int i = 0; i < vectIndicators.size(); i++)
		{
			vRet.push_back(vectIndicators[i]->getShortName().toStdString());
		}
	}
	else {
		CairnAPIUtils::setError(noCairn);
	}
	return vRet;
}

t_dict CairnAPI::MilpComponentAPI::get_IndicatorValues(const std::string range)
{
	t_dict vRet = {};
	if (m_Component) {
		InputParam::t_Indicators vectIndicators = m_Component->compoModel()->getInputIndicators()->getIndicators();
		for (int i = 0; i < vectIndicators.size(); i++)
		{
			if (range == "PLAN") {
				vRet.insert({ vectIndicators[i]->getName().toStdString(), vectIndicators[i]->getValue(0) });
			}
			else if (range == "HIST") {
				vRet.insert({ vectIndicators[i]->getName().toStdString(), vectIndicators[i]->getValue(1) });
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
	double vRet=NAN;
	if (m_Component) {
		InputParam::t_Indicators vectIndicators = m_Component->compoModel()->getInputIndicators()->getIndicators();
		for (int i = 0; i < vectIndicators.size(); i++) {
			if (vectIndicators[i]->getName().toStdString() == name) {
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
	return int(m_Component->compoModel()->isSizeOptimized());
}

t_value CairnAPI::MilpComponentAPI::get_dimParam() {
	return (m_Component->compoModel()->getOptimalSizeExpression().toStdString());
}

