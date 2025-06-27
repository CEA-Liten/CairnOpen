#include "CairnAPIUtils.h"
#include "CairnCore.h"
#include "OptimProblem.h"
#include "Cairn_Exception.h"
#include "BusCompo.h"
#include "CairnAPI.h"

using namespace CairnAPIUtils;

CairnAPI::MilpComponentAPI::MilpComponentAPI()
{	
}

CairnAPI::MilpComponentAPI::MilpComponentAPI(const std::string& a_Name, const std::string& a_Type, const std::string& a_ModelName)
	: CairnAPI::ObjectAPI(a_Name, a_Type)
{
	set_ModelClass(a_ModelName);
}

CairnAPI::MilpComponentAPI::MilpComponentAPI(const std::string& a_Name, const std::string& a_ModelName)
	: CairnAPI::ObjectAPI(a_Name, "Unknown")
{
	t_list possibleTypes = CairnAPIUtils::get_Possible_Component_Types();
	std::string type = CairnAPIUtils::get_Component_Type(a_ModelName);
	if (std::find(possibleTypes.begin(), possibleTypes.end(), type) == possibleTypes.end()) {
		CairnAPIUtils::setError(errDefault, "Something went wrong! The type of " + a_ModelName + " is unknown!\n"
			+ "Please, try to provide the component type e.g. Component(" + a_Name + ", a_ComponentType, " + a_ModelName + ")");
	}
	set_Type(type);
	set_ModelClass(a_ModelName);
}

void CairnAPI::MilpComponentAPI::set_MilpComponent(class MilpComponent* ap_Component)
{
	m_Component = ap_Component;

	if (m_Component) {
		m_Name = m_Component->Name().toStdString();
		m_Type = m_Component->Type().toStdString();
		m_ModelClass = m_Component->ModelClassName().toStdString();

		for (auto& vPort : m_Ports) {
			MilpPort* vMilpPort = m_Component->getPortByName(QString(vPort->get_Name().c_str()));
			if (vMilpPort) {				
				vPort->set_Port(vMilpPort);
			}
		}
	}

	// vider la liste temporaire des ports
	m_Ports.clear();
}

MilpComponent* CairnAPI::MilpComponentAPI::get_MilpComponent() const
{
	return m_Component;
}

void CairnAPI::MilpComponentAPI::set_ModelClass(const std::string& a_ModelName)
{
	// TODO: Vérification si il s'agit d'un ModelClass valide ?
	m_ModelClass = a_ModelName;
}

const std::string CairnAPI::MilpComponentAPI::get_ModelClass()
{
	return m_ModelClass;
}

t_list CairnAPI::MilpComponentAPI::get_SettingsList(ESettingsLimited a_setLimited)
{
	t_list vRet;
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

t_value CairnAPI::MilpComponentAPI::get_SettingValue(const std::string& a_SettingName)
{
	t_value vRet;
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
	else {
		vRet = ObjectAPI::get_SettingValue(a_SettingName);
	}
	return vRet;
}

t_dict CairnAPI::MilpComponentAPI::get_SettingValues()
{
	t_dict vRet;
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
	else {
		vRet = ObjectAPI::get_SettingValues();
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


void CairnAPI::MilpComponentAPI::set_SettingValue(const std::string& a_SettingName, const t_value& a_SettingValue)
{
	ECodeError vRet = noError;
	//The ModelClass of a component cannot be changed!
	if (a_SettingName == "ModelClass") {
		CairnAPIUtils::setError(errDefault, "The ModelClass of a component cannot be changed!");
		return;
	}

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
		
		if (a_SettingName == "NbInputFlux") {
			//Re-declare InputFluxIOs
			m_Component->compoModel()->declareInputFluxIOs();
		}
		if (a_SettingName == "NbOutputFlux") {
			//Re-declare OutputFluxIOs
			m_Component->compoModel()->declareOutputFluxIOs();
		}

		//Keep MilpComponent::mComponent updated because it the the map used by initialize() -- by initProblem to set parameter values
		m_Component->updateCompoParamMap(a_SettingName, a_SettingValue);

		vRet = (vOk) ? noError : errParam;
	}
	else {
		ObjectAPI::set_SettingValue(a_SettingName, a_SettingValue);
	}
	CairnAPIUtils::setError(vRet);
}

void CairnAPI::MilpComponentAPI::set_SettingValues(const t_dict& a_SettingValues)
{
	for (auto& vParam : a_SettingValues) {
		if (vParam.first == "ModelClass")
			continue;
		set_SettingValue(vParam.first, vParam.second);
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
			CairnAPIUtils::setError(errDefault, a_TimeSeriesName + " is not a valid timeseries of the componenet " + m_Name);
		}
	}
	else {
		CairnAPIUtils::setError(errDefault, m_Name + " must be added to the problem before setting its timeseries!");
	}
}

t_list CairnAPI::MilpComponentAPI::get_Ports()
{
	t_list vRet;
	if (m_Component) {
		QList<MilpPort*> vQList = m_Component->findChildren<MilpPort*>();
		for (auto& vElem : vQList) {
			vRet.push_back(vElem->Name().toStdString());
		}
	}
	else {
		for (auto& vPort : m_Ports) {
			vRet.push_back(vPort->get_Name());
		}
	}
	return vRet;
}

CairnAPI::MilpPortAPI CairnAPI::MilpComponentAPI::get_Port(const std::string& a_Name)
{
	MilpPortAPI vRet;
	QString vPortName = QString(a_Name.c_str());
	if (m_Component) {
		MilpPort* vPort = m_Component->findChild<MilpPort*>(vPortName);
		if (vPort) {
			vRet.set_Port(vPort);
		}
		else
			CairnAPIUtils::setError(errNotFound, "port " + a_Name);
	}
	else {
		for (auto& vPort : m_Ports) {
			if (vPort->get_Name() == a_Name) {
				return *vPort;
				break;
			}
		}
		CairnAPIUtils::setError(errNotFound, "port " + a_Name);
	}
	return vRet;
}

void CairnAPI::MilpComponentAPI::add(MilpPortAPI& a_Port)
{
	ECodeError vErr = noError;
	std::string vErrMsg = "";
	QString vPortName = QString(a_Port.get_Name().c_str());
	if (!a_Port.checkSettings(vErrMsg)) {
		vErrMsg = "port " + a_Port.get_Name() + vErrMsg;
		vErr = errDefault;
	}
	else {
		if (m_Component) {
			// vérification si le port n'existe pas déjà		
			MilpPort* vPort = m_Component->findChild<MilpPort*>(vPortName);
			if (vPort) {
				vErr = errAlreadyExist;
				vErrMsg = "port " + a_Port.get_Name();
			}
			else {
				QString vComponentName(QString(get_Name().c_str()));
				QString vPortId = m_Component->getUniquePortID();
				QMap<QString, QString> vPortParams;
				vPortParams["CompoName"] = vComponentName;
				vPortParams["Name"] = vPortName;
				vPortParams["Carrier"] = QString(a_Port.get_Type().c_str());
				vPortParams["Direction"] = QString(getParamValue(a_Port.get_SettingValue("Direction")).c_str());
				vPortParams["Variable"] = QString(getParamValue(a_Port.get_SettingValue("Variable")).c_str());

				// récupère les options des ports
				t_list vOptions = a_Port.get_SettingsList(CairnAPI::optional);
				t_dict vParams = a_Port.get_SettingValues();
				for (auto& [vParamName, vParamValue] : vParams) {
					t_list::iterator vIter = find(vOptions.begin(), vOptions.end(), vParamName);
					if (vIter != vOptions.end()) {
						vPortParams[QString(vParamName.c_str())] = QString(getParamValue(vParamValue).c_str());
					}
				}

				//create port
				m_Component->createOnePort(vPortId, vPortParams);

				//affect to the energy vector				
				MilpPort* vMilpPort = m_Component->getPortByName(QString(a_Port.get_Name().c_str()));
				if (vMilpPort) {
					a_Port.set_Port(vMilpPort);			
					//Reinitialize component
					reinitialize();
				}
				else {
					vErr = errAdd;
					vErrMsg = "port " + a_Port.get_Name();
				}
			}
		}
		else {
			m_Ports.push_back(&a_Port);
		}
	}			
	CairnAPIUtils::setError(vErr, vErrMsg);
}

void CairnAPI::MilpComponentAPI::remove(MilpPortAPI& a_Port) {
	removePort(a_Port, true);
}

void CairnAPI::MilpComponentAPI::removePort(MilpPortAPI& a_Port, const bool reinitializeCompo)
{
	ECodeError vErr = noError;
	std::string vErrMsg = "";
	if (m_Component) {		
		QString vPortName(QString(a_Port.get_Name().c_str()));
		MilpPort* vPort = m_Component->findChild<MilpPort*>(vPortName);
		if (vPort) {
			//check if there is a link or Not
			BusCompo* vBus = dynamic_cast<BusCompo*> (vPort->ptrLinkedComponent());
			if (vBus) {
				vPort->DeleteptrLinkedComponent();
				vBus->DeleteBusPort(vPort);
				vBus->RemoveLinkComponent(m_Component);
			}
			m_Component->removePort(vPort);
			//Reinitialize component
			if (reinitializeCompo) {
				reinitialize();
			}
		}
		else {
			vErr = errNotFound;
			vErrMsg = a_Port.get_Name();
		}
	}
	else {
		std::vector<MilpPortAPI*>::iterator vIter = find(m_Ports.begin(), m_Ports.end(), &a_Port);
		if (vIter != m_Ports.end()) {
			m_Ports.erase(vIter);
		}
		else {
			vErr = errNotFound;
			vErrMsg = a_Port.get_Name();
		}
	}
	CairnAPIUtils::setError(vErr, vErrMsg);
}


void CairnAPI::MilpComponentAPI::reinitialize() 
{
	//only if there is a selected Env Impact
	OptimProblem* vProblem = (OptimProblem*)m_Component->parent();
	if (vProblem) {
		TecEcoAnalysis* vTecEcoAnalysis = vProblem->getTecEcoAnalysis();
		if (vTecEcoAnalysis) {
			QString selectedEnvImpacts;
			vTecEcoAnalysis->getConfigParam()->getParameterValue("ConsideredEnvironmentalImpacts", selectedEnvImpacts, EParamType::eStringList);
			if (selectedEnvImpacts != "") {
				m_Component->initProblem();
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
		if (get_Port(vPort).get_EnergyVector() == a_EnergyVectorName) {
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
	t_list vRet;
	if (m_Component) {
		InputParam::t_Indicators vectIndicators = m_Component->compoModel()->getInputIndicators()->getIndicators();
		for (int i = 0; i < vectIndicators.size(); i++)
		{
			vRet.push_back(vectIndicators[i]->getName().toStdString());
		}
	}
	else
		CairnAPIUtils::setError(noCairn);
	return vRet;
}

t_list CairnAPI::MilpComponentAPI::get_IndicatorUnits()
{
	t_list vRet;
	if (m_Component) {
		InputParam::t_Indicators vectIndicators = m_Component->compoModel()->getInputIndicators()->getIndicators();
		for (int i = 0; i < vectIndicators.size(); i++)
		{
			vRet.push_back(vectIndicators[i]->getUnit().toStdString());
		}
	}
	else
		CairnAPIUtils::setError(noCairn);
	return vRet;
}

t_list CairnAPI::MilpComponentAPI::get_IndicatorShortNames()
{
	t_list vRet;
	if (m_Component) {
		InputParam::t_Indicators vectIndicators = m_Component->compoModel()->getInputIndicators()->getIndicators();
		for (int i = 0; i < vectIndicators.size(); i++)
		{
			vRet.push_back(vectIndicators[i]->getShortName().toStdString());
		}
	}
	else
		CairnAPIUtils::setError(noCairn);
	return vRet;
}

t_dict CairnAPI::MilpComponentAPI::get_IndicatorValues(const std::string range)
{
	t_dict vRet;
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
	else
		CairnAPIUtils::setError(noCairn);
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
	else
		CairnAPIUtils::setError(noCairn);
	return vRet;
}

t_list CairnAPI::MilpComponentAPI::get_VarList()
{
	t_list vRet;	
	if (m_Component) {		
		// le composant existe, retourne une liste des variables (model expressions)		
		const SubModel::t_mapIOs &vIOs = m_Component->compoModel()->getMapIOExpression();
		for (auto& [vElem, vExpr] : vIOs) {
			vRet.push_back(vElem.toStdString());
		}				
	}
	else
		CairnAPIUtils::setError(noCairn);
	return vRet;
}

t_value CairnAPI::MilpComponentAPI::get_varValue(const std::string& a_VarName) // , const SolutionAPI& a_solution, int a_numSol)
{
	if (m_Component) {
		//const double* vOptSolution = a_solution.getOptimalSolution(a_numSol);
		//if (vOptSolution) {
			// le composant existe, retourne une liste des variables (model expressions)				
			QString vVarName(a_VarName.c_str());
			ModelIO *vIO = m_Component->compoModel()->getIOExpression(vVarName);
			if (vIO) {				
				/*
				* After the end of the simulation, all expressions get closed, and thus they evaluate to 0
				* => use a stored copy of the expression value
				*/
				return vIO->getValue(); //vIO->evaluate(vOptSolution); 
			}			
			CairnAPIUtils::setError(errNotFound, "variable " + a_VarName);
		//}
		//else
		//	CairnAPIUtils::setError(errNotFound, "optimal solution");		
	}
	else
		CairnAPIUtils::setError(noCairn);
}

t_dict CairnAPI::MilpComponentAPI::get_varValues()  // (const SolutionAPI& a_solution, int a_numSol)
{
	t_dict vRet;
	t_list varList = get_VarList();
	for (auto& varName : varList) {
		vRet[varName] = get_varValue(varName); // , a_solution, a_numSol);
	}
	return vRet;
}

t_value CairnAPI::MilpComponentAPI::isOptimized() {
	return int(m_Component->compoModel()->isSizeOptimized());
}


