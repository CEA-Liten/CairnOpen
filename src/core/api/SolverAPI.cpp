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
	if (!m_Object) {
		CairnAPIUtils::setError(errNotFound, "Solver object is null");
		return;
	}

	// Special case: switching solver
	if (a_SettingName == PARAM_SOLVER_NAME)
	{
		const std::string newName = CairnAPIUtils::getParamValue(a_SettingValue);
		const std::string currentName =
			CairnAPIUtils::getParamValue(get_SettingValue(PARAM_SOLVER_NAME));

		if (newName != currentName) 
		{
			auto* problem = dynamic_cast<OptimProblem*>(m_Object->parent());
			if (!problem) {
				CairnAPIUtils::setError(errDefault, "Parent OptimProblem is null");
				return;
			}

			auto* solver = problem->getSolver();
			if (!solver) {
				CairnAPIUtils::setError(errNotFound, "Solver not found in OptimProblem");
				return;
			}

			try {
				solver->solverNameChanged();
			}
			catch (const Cairn_Exception& error) {
				CairnAPIUtils::setError(errParam, "Error changing solver '" + newName + "': " + error.message());
				return;
			}
			catch (...) {
				CairnAPIUtils::setError(errParam, "Unknown error changing solver '" + newName + "'");
				return;
			}
		}
	}

	// Update setting value
	CairnAPI::ObjectAPI::set_SettingValue(a_SettingName, a_SettingValue);
}

// Set the values of several parameters
void CairnAPI::SolverAPI::set_SettingValues(const t_dict& a_SettingValues)
{
	if (!m_Object) {
		CairnAPIUtils::setError(errNotFound, "Solver object is null");
		return;
	}

	// Apply solver name once (if present)
	auto it = a_SettingValues.find(PARAM_SOLVER_NAME);
	if (it != a_SettingValues.end()) {
		set_SettingValue(it->first, it->second);
	}

	// Forward all other settings to ObjectAPI
	for (const auto& kv : a_SettingValues) {
		if (kv.first != PARAM_SOLVER_NAME) {
			CairnAPI::ObjectAPI::set_SettingValue(kv.first, kv.second);
		}
	}

	CairnAPIUtils::setError(noError);
}



Solver* CairnAPI::SolverAPI::get_Solver() const
{
	return (Solver*)get_Object();
}

t_list CairnAPI::SolverAPI::get_ProblemTypes() const
{
	const Solver* solver = get_Solver();
	if (solver) {
		return solver->getProblemTypes();
	}
	return {};
}

t_list CairnAPI::SolverAPI::get_PossibleModelTypes() const
{
	const Solver* solver = get_Solver();
	if (solver) {
		return solver->getPossibleModelTypes();
	}
	return {};
}

