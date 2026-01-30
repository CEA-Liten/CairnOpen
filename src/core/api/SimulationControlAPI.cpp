#include "CairnAPI.h"
#include "CairnCore.h"
#include "CairnAPIUtils.h"
#include "InputParam.h"
using namespace CairnAPIUtils;

CairnAPI::SimulationControlAPI::SimulationControlAPI()
	: CairnAPI::ObjectAPI()
{
}

// Set the value of a parameter
void CairnAPI::SimulationControlAPI::set_SettingValue(const std::string& a_SettingName, const t_value& a_SettingValue, bool checkExistance)
{
	ECodeError vRet = errNotFound;
	std::string vErrMsg = "SimulationControl";
	if (m_Object) {
		CairnAPI::ObjectAPI::set_SettingValue(a_SettingName, a_SettingValue);
		updateMilpData();
		vRet = noError;
		vErrMsg = "";

	}
	CairnAPIUtils::setError(vRet, vErrMsg);
}

// Set the values of several parameters
void CairnAPI::SimulationControlAPI::set_SettingValues(const t_dict& a_SettingValues)
{
	ECodeError vRet = errNotFound;
	std::string vErrMsg = "SimulationControl";
	if (m_Object) {				
		bool vOk = CairnAPIUtils::setParameters(get_InputParams(), a_SettingValues);
		vRet = (vOk) ? noError : errParam;
		vErrMsg = (vOk) ? "" : "parameter";
		if (vOk) {
			updateMilpData();
		}				
	}
	CairnAPIUtils::setError(vRet, vErrMsg);
}

void CairnAPI::SimulationControlAPI::updateMilpData()
{
	ECodeError vRet = errNotFound;
	std::string vErrMsg = "SimulationControl";
	if (m_Object) {
		OptimProblem* pProblem = (OptimProblem*)m_Object->parent();
		if (pProblem) {
			//update MilpData			
			cDebug() << "Update MilpData from SimulationControl parameters";
			MilpData* pMilpData = pProblem->getMilpData();
			SimulationControl* vSimulationControl = (SimulationControl*)m_Object;
			bool vOk = pMilpData->setMilpDataFromSettings(vSimulationControl->getParameters());
			pProblem->setExtrapolationFactor();
			vRet = (vOk) ? noError : errParam;
			vErrMsg = (vOk) ? "" : "parameter";
		}
		else {
			vRet = noCairn;
			vErrMsg = "";
		}
	}
	CairnAPIUtils::setError(vRet);
}
