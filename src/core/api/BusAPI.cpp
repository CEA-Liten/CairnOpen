#include "CairnAPI.h"
#include "CairnCore.h"
#include "BusCompo.h"
#include "CairnAPIUtils.h"
using namespace CairnAPIUtils;

CairnAPI::BusAPI::BusAPI(class BusCompo* ap_Bus)
{
	set_BusCompo(ap_Bus);
}

CairnAPI::BusAPI::BusAPI(const OptimProblemAPI& a_Problem, const std::string& a_Name, 
	const std::string& a_ModelName, const EnergyVectorAPI& a_EnergyVector)
{
	*this = a_Problem.create_Bus(a_Name, a_ModelName, a_EnergyVector);
}

BusCompo* CairnAPI::BusAPI::get_BusCompo() const
{
	return m_Bus;
}

void CairnAPI::BusAPI::set_BusCompo(BusCompo* ap_Bus)
{
	m_Bus = ap_Bus;
	if (m_Bus && !m_Bus->getMainCarrier()) {
		CairnAPIUtils::setError(errDefault, "The EnergyCarrier of the Bus " + get_Name() + " must be defined!");
	}
}

std::string CairnAPI::BusAPI::get_Name() const
{
	if (m_Bus) {
		return m_Bus->Name();
	}
	return "";
}

std::string CairnAPI::BusAPI::get_Type() const
{
	if (m_Bus) {
		return m_Bus->Type();
	}
	return "";
}

std::string CairnAPI::BusAPI::get_ModelClass() const
{
	if (m_Bus) {
		return m_Bus->ModelClassName();
	}
	return "";
}

std::string CairnAPI::BusAPI::get_CarrierName() const
{
	if (m_Bus && m_Bus->getMainCarrier()) {
		return m_Bus->getMainCarrier()->Name();
	}
	return "";
}

void CairnAPI::BusAPI::rename(const std::string& name)
{
	if (m_Bus) {
		m_Bus->setName((name));
	}
}

std::string CairnAPI::BusAPI::get_LabelValue(const std::string& a_Label) const
{
	std::string vRet = "";
	if (m_Bus)
	{
		OptimProblem* vOptimProblem = (OptimProblem*)m_Bus->parent();
		TecEcoAnalysis* vTecEcoAnalysis = vOptimProblem->getTecEcoAnalysis();
		if (vTecEcoAnalysis->isValidLabel(a_Label)) {
			vRet = m_Bus->compoModel()->getLabelValue(a_Label);
		}
		else {
			CairnAPIUtils::setError(errDefault, "Label " + a_Label + " is not defined. Please, add the label to the problem first!");
		}
	}
	return vRet;
}

std::map<std::string, std::string> CairnAPI::BusAPI::get_LabelValues() const
{
	std::map<std::string, std::string> vRet = {};
	if (m_Bus)
	{
		//return m_Bus->compoModel()->getLabelMap();

		OptimProblem* vOptimProblem = (OptimProblem*)m_Bus->parent();
		TecEcoAnalysis* vTecEcoAnalysis = vOptimProblem->getTecEcoAnalysis();
		if (vTecEcoAnalysis) {
			for (auto const& label : vTecEcoAnalysis->getLabelList())//referance list
			{
				vRet[label] = m_Bus->compoModel()->getLabelValue(label);
			}
		}
	}
	return vRet;
}

void CairnAPI::BusAPI::set_LabelValue(const std::string& a_Label, const std::string& a_Value)
{
	if (m_Bus)
	{
		OptimProblem* vOptimProblem = (OptimProblem*)m_Bus->parent();
		TecEcoAnalysis* vTecEcoAnalysis = vOptimProblem->getTecEcoAnalysis();
		if (vTecEcoAnalysis->isValidLabel(a_Label)) {
			m_Bus->compoModel()->setLabel(a_Label, a_Value);
		}
		else {
			CairnAPIUtils::setError(errDefault, "Label " + a_Label + " is not defined. Please, add the label to the problem first!");
		}
	}
}

void CairnAPI::BusAPI::set_LabelValues(const std::map<std::string, std::string>& a_Labels)
{

	if (m_Bus)
	{
		/*
		* No need to verify that all the labels are valid. It is ok!
		* Labels are filtered when using get_method and when writing to json file
		*/
		m_Bus->compoModel()->setLabelMap(a_Labels);
	}
}
// Returns the list of parameter names 

t_list CairnAPI::BusAPI::get_SettingsList()
{
	/*
	* property in CairnBind.cpp
	*/
	return get_SettingsListByType(ESettingsLimited::all);
}

t_list CairnAPI::BusAPI::get_SettingsListByType(ESettingsLimited a_setLimited)
{
	t_list vRet = {};
	if (m_Bus) {
		vRet = CairnAPIUtils::getParametersName({
			m_Bus->compoModel()->getInputParam(), // params	
			m_Bus->getCompoInputParam(), // options
			m_Bus->compoModel()->getInputDataTS(), // timeseries
			m_Bus->getGUIData()->getGuiInputParam() //GuiData
			}
		, a_setLimited);
	}
	return vRet;
}

// Returns the value of a parameter 
t_value CairnAPI::BusAPI::get_SettingValue(const std::string& a_SettingName)
{
	t_value vRet = "";
	if (m_Bus) {
		vRet = CairnAPIUtils::getParameter({
			m_Bus->compoModel()->getInputParam(), // params	
			m_Bus->getCompoInputParam(), // options
			m_Bus->compoModel()->getInputDataTS(), // timeseries	
			m_Bus->getGUIData()->getGuiInputParam() //GuiData
			}
		, a_SettingName);
	}
	return vRet;
}

// Returns a dict of all parameter values
t_dict CairnAPI::BusAPI::get_SettingValues()
{
	t_dict vRet = {};
	if (m_Bus) {
		CairnAPIUtils::getParameters({
			m_Bus->compoModel()->getInputParam(), // params	
			m_Bus->getCompoInputParam(), // options
			m_Bus->compoModel()->getInputDataTS(), // timeseries	
			m_Bus->getGUIData()->getGuiInputParam() //GuiData
			}
		, vRet);
	}
	return vRet;
}

// Set the value of a parameter
void CairnAPI::BusAPI::set_SettingValue(const std::string& a_SettingName, const t_value& a_SettingValue)
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
	if (m_Bus) {
		bool vOk = CairnAPIUtils::setParameter({
			m_Bus->compoModel()->getInputParam(), // params	
			m_Bus->getCompoInputParam(), // options
			m_Bus->compoModel()->getInputDataTS(), // Bus doesn't have timeseries 	
			m_Bus->getGUIData()->getGuiInputParam() //GuiData  		
			}
		, a_SettingName, a_SettingValue);

		//Update MilpComponent::mComponent as it is used to re-initialize the component parameters
		m_Bus->updateCompoParamMap(a_SettingName, a_SettingValue);

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

bool CairnAPI::BusAPI::get_SettingMandatoryValue(const std::string& a_SettingName)
{
	bool vRet = true;
	if (m_Bus) {
		vRet = CairnAPIUtils::getParamMandatoryValue({
			m_Bus->compoModel()->getInputParam(), // params	 
			m_Bus->getCompoInputParam(),
			m_Bus->compoModel()->getInputDataTS(), // timeseries 	
			m_Bus->getGUIData()->getGuiInputParam() //GuiData  				
			},
			a_SettingName);
	}
	return vRet;
}

bool CairnAPI::BusAPI::is_DependentSetting(const std::string& a_SettingName)
{
	bool vRet = false;
	if (m_Bus) {
		vRet = CairnAPIUtils::isDependentParam({
			m_Bus->compoModel()->getInputParam(), // params	 
			m_Bus->getCompoInputParam(),
			m_Bus->compoModel()->getInputDataTS(), // timeseries 	
			m_Bus->getGUIData()->getGuiInputParam() //GuiData  				
			},
			a_SettingName);
	}
	return vRet;
}

std::string CairnAPI::BusAPI::get_SettingUnit(const std::string& a_SettingName)
{
	std::string vRet = "-";
	if (m_Bus) {
		vRet = CairnAPIUtils::getParamUnit({
			m_Bus->compoModel()->getInputParam(), // params	 
			m_Bus->getCompoInputParam(),
			m_Bus->compoModel()->getInputDataTS(), // timeseries 	
			m_Bus->getGUIData()->getGuiInputParam() //GuiData  				
			},
			a_SettingName);
	}
	return vRet;
}

std::string CairnAPI::BusAPI::get_SettingShowConfig(const std::string& a_SettingName)
{
	std::string vRet = "";
	if (m_Bus) {
		vRet = CairnAPIUtils::getParamShowConfig({
			m_Bus->compoModel()->getInputParam(), // params	 
			m_Bus->getCompoInputParam(),
			m_Bus->compoModel()->getInputDataTS(), // timeseries 	
			m_Bus->getGUIData()->getGuiInputParam() //GuiData  		
		    },
			a_SettingName);
	}
	return vRet;
}

t_list CairnAPI::BusAPI::get_ShowConfigList()
{
	t_list vRet = {};
	if (m_Bus) {
		vRet = CairnAPIUtils::getShowConfigList({
			m_Bus->compoModel()->getInputParam(), // params	 
			m_Bus->getCompoInputParam(),
			m_Bus->compoModel()->getInputDataTS(), // timeseries 	
			m_Bus->getGUIData()->getGuiInputParam() //GuiData  			
			}
		);
	}
	return vRet;
}

// -- IOs ---
t_list CairnAPI::BusAPI::get_VarList()
{
	t_list vRet = {};
	if (m_Bus) {
		// le composant existe, retourne une liste des variables (model expressions)		
		const SubModel::t_mapIOs& vIOMap = m_Bus->compoModel()->getMapIOExpression();
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
	if (m_Bus) {
		std::string vVarName(a_VarName.c_str());
		ModelIO* vIO = m_Bus->compoModel()->getIOExpression(vVarName);
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
	if (m_Bus) {
		InputParam::t_Indicators vectIndicators = m_Bus->compoModel()->getInputIndicators()->getIndicators();
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
	if (m_Bus) {
		InputParam::t_Indicators vectIndicators = m_Bus->compoModel()->getInputIndicators()->getIndicators();
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
	if (m_Bus) {
		InputParam::t_Indicators vectIndicators = m_Bus->compoModel()->getInputIndicators()->getIndicators();
		for (int i = 0; i < vectIndicators.size(); i++)
		{
			vRet.push_back(vectIndicators[i]->getShortName());
		}
	}
	else
		CairnAPIUtils::setError(noCairn);
	return vRet;
}

t_dict CairnAPI::BusAPI::get_IndicatorValues(const std::string range)
{
	t_dict vRet = {};
	if (m_Bus) {
		InputParam::t_Indicators vectIndicators = m_Bus->compoModel()->getInputIndicators()->getIndicators();
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
	else
		CairnAPIUtils::setError(noCairn);
	return vRet;
}

double CairnAPI::BusAPI::get_IndicatorValue(const std::string& name, const std::string range)
{
	double vRet = NAN;
	if (m_Bus) {
		InputParam::t_Indicators vectIndicators = m_Bus->compoModel()->getInputIndicators()->getIndicators();
		for (int i = 0; i < vectIndicators.size(); i++) {
			if (vectIndicators[i]->getName() == name) {
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