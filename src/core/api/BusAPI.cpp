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
	if (m_Bus && !m_Bus->getEnergyVector()) {
		CairnAPIUtils::setError(errDefault, "The EnergyCarrier of the Bus " + get_Name() + " must be defined!");
	}
}

std::string CairnAPI::BusAPI::get_Name() const
{
	if (m_Bus) {
		return m_Bus->Name().toStdString();
	}
	return "";
}

std::string CairnAPI::BusAPI::get_Type() const
{
	if (m_Bus) {
		return m_Bus->Type().toStdString();
	}
	return "";
}

std::string CairnAPI::BusAPI::get_ModelClass() const
{
	if (m_Bus) {
		return m_Bus->ModelClassName().toStdString();
	}
	return "";
}

std::string CairnAPI::BusAPI::get_CarrierName() const
{
	if (m_Bus && m_Bus->getEnergyVector()) {
		return m_Bus->getEnergyVector()->Name().toStdString();
	}
	return "";
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
			m_Bus->compoModel()->getInputDataTS() // timeseries	
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
			m_Bus->compoModel()->getInputDataTS() // timeseries	
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
			m_Bus->compoModel()->getInputDataTS() // timeseries	
			}
		, vRet);
	}
	return vRet;
}

// Set the value of a parameter
void CairnAPI::BusAPI::set_SettingValue(const std::string& a_SettingName, const t_value& a_SettingValue)
{
	if (a_SettingName == "ModelClass") {
		CairnAPIUtils::setError(errDefault, "The ModelClass of a component cannot be changed!");
		return;
	}

	ECodeError vRet = noError;
	if (m_Bus) {
		bool vOk = CairnAPIUtils::setParameter({
			m_Bus->compoModel()->getInputParam(), // params	
			m_Bus->getCompoInputParam(), // options
			m_Bus->compoModel()->getInputDataTS() // Bus doesn't have timeseries 		
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
		if (find(CairnAPIUtils::mNonModifiableParams.begin(), CairnAPIUtils::mNonModifiableParams.end(), vParam.first) != CairnAPIUtils::mNonModifiableParams.end())
		{
			qWarning() << (vParam.first + " cannot be modified!").c_str();
			continue;
		}
		set_SettingValue(vParam.first, vParam.second);
	}
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
				vRet.push_back(vName.toStdString());
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
		QString vVarName(a_VarName.c_str());
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
			vRet.push_back(vectIndicators[i]->getName().toStdString());
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
			vRet.push_back(vectIndicators[i]->getUnit().toStdString());
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
			vRet.push_back(vectIndicators[i]->getShortName().toStdString());
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

double CairnAPI::BusAPI::get_IndicatorValue(const std::string& name, const std::string range)
{
	double vRet = NAN;
	if (m_Bus) {
		InputParam::t_Indicators vectIndicators = m_Bus->compoModel()->getInputIndicators()->getIndicators();
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