#include "CairnAPI.h"
#include "CairnCore.h"
#include "CairnAPIUtils.h"
#include "InputParam.h"
using namespace CairnAPIUtils;

CairnAPI::TecEcoAnalysisAPI::TecEcoAnalysisAPI()
	: CairnAPI::ObjectAPI()
{
}

// Set the value of a parameter
void CairnAPI::TecEcoAnalysisAPI::set_SettingValue(const std::string& a_SettingName, const t_value& a_SettingValue, bool checkExistance)
{		
	set_SettingValues({ {a_SettingName, a_SettingValue} });
}

// Set the values of several parameters
void CairnAPI::TecEcoAnalysisAPI::set_SettingValues(const t_dict& a_Settings)
{
	ECodeError vErr = noCairn;
	std::string vErrMsg = "";
	if (m_Object) {
		TecEcoCompo* pTecEco = (TecEcoCompo*)m_Object;
		if (pTecEco) {
			OptimProblem* pProblem = (OptimProblem*)pTecEco->parent();
			if (pProblem) {
				TecEcoAnalysis* vTecEcoAnalysis = (TecEcoAnalysis*)pTecEco->compoModel();
				//save the previous value of "ConsideredEnvironmentalImpacts"
				std::string preSelectedEnvImpacts;
				vTecEcoAnalysis->getConfigParam()->getParameterValue("ConsideredEnvironmentalImpacts", preSelectedEnvImpacts, EParamType::eStringList);

				//split between configuration and non-configuration parameters
				std::vector<std::string> configParamNames;
				vTecEcoAnalysis->getConfigParam()->getParameters(configParamNames);
				t_dict config_settings = {};
				t_dict settings = a_Settings;
				for (auto& vParam : a_Settings) {
					if (CairnUtils::contains(configParamNames, vParam.first)) {
						config_settings[vParam.first] = vParam.second;
						settings.erase(vParam.first);
					}
				}

				//set configuration parameters
				bool vOk1 = CairnAPIUtils::setParameters({ vTecEcoAnalysis->getConfigParam() }, config_settings);
				if (vOk1 && config_settings.find("ConsideredEnvironmentalImpacts") != config_settings.end()) {
					//re-declare EnvImpact parameters of TecEcoAnalysis
					vTecEcoAnalysis->declareEnvImpactParam();
				}

				//set non-Configuration parameters
				bool vOk2 = CairnAPIUtils::setParameters({
					vTecEcoAnalysis->getCompoInputParam(),
					vTecEcoAnalysis->getCompoInputSettings(),
					vTecEcoAnalysis->getCompoEnvImpactsParam(),
					pTecEco->getGUIData()->getGuiInputParam() },
					settings);

				if (vOk1 && vOk2) {
					cDebug() << "initialize the Optim Problem From Tec Eco Analysis";
					pProblem->initOptimProblemFromTecEcoAnalysis();
					//
					std::string selectedEnvImpacts;
					vTecEcoAnalysis->getConfigParam()->getParameterValue("ConsideredEnvironmentalImpacts", selectedEnvImpacts, EParamType::eStringList);
					if (selectedEnvImpacts != preSelectedEnvImpacts) {
						//Reinitialize added/created componenets
						try {
							pProblem->redeclareEnvImpactParameters();
							TecEcoCompo* pTecEco = dynamic_cast<TecEcoCompo*> (vTecEcoAnalysis->parent());
							if (pTecEco) {
								pTecEco->declareIOVariables();
								pTecEco->declareIndicators();
							}
						}
						catch (Cairn_Exception& error)
						{
							vOk1 = false;
						}
					}
				}
				vErr = (vOk1 && vOk2) ? noError : errParam;
				vErrMsg = (vOk1 && vOk2) ? "" : "parameter";
			}
		}
	}
	CairnAPIUtils::setError(vErr, vErrMsg);
}
