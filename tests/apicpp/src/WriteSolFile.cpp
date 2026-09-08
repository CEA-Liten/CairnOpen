#include "TEST_CairnCore.h"
#include <iostream>
#include "StudyCTest.h"


using namespace std;

/* This test reads study formation_cairn.json, change the cplex solver parameter WriteSol to true,
* runs the study and check if the file cairn_training_cpxsol.sol has been created.
*/

int main()
{
	CairnAPI m_Cairn;
	StudyCTest vTest("", "");
	std::string vSolverType = vTest.TrySolver(m_Cairn, "Cplex");
	if (vSolverType == "Highs") return noError; // No test if solver is Highs

	CairnAPI::OptimProblemAPI m_Problem;

	//File Paths
	const std::string StudyRoot = TEST_RESULTS + (std::string)"/writeSol/";
	const std::string vFileName = StudyRoot + (std::string)"/formation_cairn.json";
	const std::string TimeseriesFileName = StudyRoot + (std::string)"/formation_cairn_dataseries.csv";
	const std::string SolFileName = StudyRoot + (std::string)"/formation_cairn_cpxsol.sol";

	if (fs::exists(StudyRoot)) {
		fs::remove_all(StudyRoot);
	}
	if (!fs::exists(TEST_RESULTS)) {
		fs::create_directory(TEST_RESULTS);
	}
	fs::create_directory(StudyRoot);
	fs::copy_file(TEST_DATA + (std::string)"/formation_cairn.json", vFileName);
	fs::copy_file(TEST_DATA + (std::string)"/formation_cairn_dataseries.csv", TimeseriesFileName);

	TESTAPI("read study file from the data: " + vFileName,
		m_Problem = m_Cairn.read_Study(vFileName)
	)
	TESTAPI("Read the Timeseries from the file path : " + TimeseriesFileName,
		m_Problem.add_TimeSeries(TimeseriesFileName)
	)


	CairnAPI::SolutionAPI vSolution;

	

	std::shared_ptr<CairnAPI::SolverAPI> solver = m_Problem.get_Solver();
	TESTAPI("ChangeParamSolver",
		solver->set_SettingValue("WriteSol", "YES", true)
	)
		TESTAPI("Run",
			m_Problem.save_Study("formation_saved.json");
		vSolution = m_Problem.run()
	)

	TESTAPIBOOL("Check that the Cplex .sol file has been created: " + SolFileName,
		fs::exists(SolFileName)
	)
	
	return noError;
}