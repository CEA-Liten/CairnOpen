#include "CairnAPI.h"
#include "CairnCore.h"
#include "CairnAPIUtils.h"
#include "InputParam.h"
using namespace CairnAPIUtils;

CairnAPI::SolverAPI::SolverAPI()
	: CairnAPI::ObjectAPI()
{
}

// Set the value of a parameter
void CairnAPI::SolverAPI::set_SettingValue(const std::string& a_SettingName, const t_value& a_SettingValue, bool checkExistance)
{
	ECodeError vErr = noCairn;
	std::string vErrMsg = "";
	if (m_Object) {
		bool vOk = true;
		// particular case of the property "solver"
		if (a_SettingName == "Solver") {
			std::string vSolverName = CairnAPIUtils::getParamValue(a_SettingValue);
			if (vSolverName != m_Object->objectName()) {
				// create new solver
				OptimProblem* pProblem = (OptimProblem * )m_Object->parent();
				if (pProblem) {
					vOk = pProblem->createSolver(vSolverName);
					if (vOk) {
						Solver* vSolver = pProblem->getSolver();
						if (vSolver) {
							set_Object(vSolver);
						}
						else {
							vOk = false;
							vErr = errNotFound;
							vErrMsg = "Problem to create solver " + vSolverName;
						}
					}
					else {
						vErr = errCreate;
						vErrMsg = "Problem to create solver " + vSolverName;
					}
				}
				else {
					vOk = false;
					vErr = errCreate;
					vErrMsg = "Problem to initialize solver " + vSolverName;
				}				
			}
		}
		if (vOk) {
			vErr = noError;
			CairnAPI::ObjectAPI::set_SettingValue(a_SettingName, a_SettingValue);			
		}		
	}
	CairnAPIUtils::setError(vErr, vErrMsg);
}

// Set the values of several parameters
void CairnAPI::SolverAPI::set_SettingValues(const t_dict& a_SettingValues)
{
	ECodeError vRet = noError;
	if (m_Object) {
		t_dict::const_iterator vIter = a_SettingValues.find("Solver");
		if (vIter != a_SettingValues.end()) {			
			CairnAPI::SolverAPI::set_SettingValue(vIter->first, vIter->second);						
		}
		CairnAPI::ObjectAPI::set_SettingValues(a_SettingValues);					
	}
	CairnAPIUtils::setError(vRet);
}
