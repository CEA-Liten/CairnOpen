#include "TEST_CairnCore.h"
#include <iostream>
#include "StudyCTest.h"
#include "Constants.h"

using namespace CairnConstants;
using namespace std;

/* This test reads a study, changes the solver then executes a run
*/

int main()
{
	CairnAPI m_Cairn;
	StudyCTest vTest("", "");
	std::string vSolverType = vTest.TrySolver(m_Cairn, "Cplex");
	if (vSolverType == "Highs") return noError;

	CairnAPI::OptimProblemAPI m_Problem;

	//File Paths
	string const StudyRoot = TEST_RESULTS + (std::string)"/changeSolver/";
	std::string vFileName = StudyRoot + (std::string)"/formation_cairn.json";
	string const TimeseriesFileName = StudyRoot + (std::string)"/formation_cairn_dataseries.csv";
	string const ResultFileName = StudyRoot + "formation_cairn_results_Results.csv";

	if (fs::exists(StudyRoot)) {
		fs::remove_all(StudyRoot);
	}
	if (!fs::exists(TEST_RESULTS)) {
		fs::create_directory(TEST_RESULTS);
	}
	fs::create_directory(StudyRoot);

	fs::copy_file(TEST_DATA + (std::string)"/formation_cairn.json", vFileName);
	fs::copy_file(TEST_DATA + (std::string)"/formation_cairn_dataseries.csv", TimeseriesFileName);
	string const ReferenceResultFileName = TEST_DATA + (std::string)"/formation_cairn_highs_Results_Reference.csv";

	TESTAPI("Read study file: " + vFileName, m_Problem = m_Cairn.read_Study(vFileName))
	TESTAPI("Read the Timeseries: " + TimeseriesFileName, m_Problem.add_TimeSeries(TimeseriesFileName))

	TESTAPIBOOL("Verify available solvers: ", TestUtils::contains(m_Cairn.get_Solvers(), "Highs"))

	std::shared_ptr < CairnAPI::SolverAPI> vSolver = m_Problem.get_Solver();
	const t_value originalGab = vSolver->get_SettingValue("Gap");
	const t_value gab = 0.005;

	TESTAPI("Change solver gab", vSolver->set_SettingValue("Gap", gab))
	TESTAPI("Change solver", vSolver->set_SettingValue(PARAM_SOLVER_NAME, "Highs"))
	TESTAPI("Verify gab value", TestUtils::compare_scalar(vSolver->get_SettingValue("Gap"), gab, EParamType::eDouble))
	TESTAPI("Set gab back to the original value", vSolver->set_SettingValue("Gap", originalGab))

	TESTAPI("Save study", m_Problem.save_Study(StudyRoot + "/formation_cairn_Highs.json"));

	TESTAPI("Run simulation", m_Problem.run())

	TESTAPIBOOL("Compare results", TestUtils::ComparaisonCsvFile(ResultFileName, ReferenceResultFileName))
	
	return noError;
}