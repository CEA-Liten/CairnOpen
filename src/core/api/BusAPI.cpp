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

std::string CairnAPI::BusAPI::get_CarrierName() const
{
	BusCompo* pBus = get_BusCompo();
	if (pBus && pBus->getMainCarrier()) {
		return pBus->getMainCarrier()->Name();
	}
	return "";
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

std::map<std::string, std::string> CairnAPI::BusAPI::get_LabelValues() const
{
	std::map<std::string, std::string> vRet = {};
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

void CairnAPI::BusAPI::set_LabelValues(const std::map<std::string, std::string>& a_Labels)
{
	BusCompo* pBus = get_BusCompo();
	if (pBus)
	{
		/*
		* No need to verify that all the labels are valid. It is ok!
		* Labels are filtered when using get_method and when writing to json file
		*/
		pBus->compoModel()->setLabelMap(a_Labels);
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

t_dict CairnAPI::BusAPI::get_IndicatorValues(const std::string range)
{
	t_dict vRet = {};
	BusCompo* pBus = get_BusCompo();
	if (pBus) {
		InputParam::t_Indicators vectIndicators = pBus->compoModel()->getInputIndicators()->getIndicators();
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
	BusCompo* pBus = get_BusCompo();
	if (pBus) {
		InputParam::t_Indicators vectIndicators = pBus->compoModel()->getInputIndicators()->getIndicators();
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