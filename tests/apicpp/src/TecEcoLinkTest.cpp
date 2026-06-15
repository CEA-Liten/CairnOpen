#include "TEST_CairnCore.h"
#include <iostream>
#include "StudyCTest.h"
#include "UtilsJson.h"

using namespace std;

/* This test reads study test_tececo.json and test_tececo_dataseries.csv, it then :
*  - executes first run and compare the results to a reference
*  - change the FutureSize to 10
*  - executed a second run and compare the results to a reference
*/

int main()
{
	CairnAPI m_Cairn;
	StudyCTest vTest("", "");
	std::string vSolverType = vTest.TrySolver(m_Cairn, "Cplex");
	if (vSolverType == "Highs") return noError; // No test if solver is Highs

	CairnAPI::OptimProblemAPI m_Problem;

	//File PathsPersee
	string const StudyRoot = TEST_RESULTS + (std::string)"/tecEcoLink/";
	std::string vFileName = StudyRoot + (std::string)"/test_tececo.json";
	string const TimeseriesFileName = StudyRoot + (std::string)"/test_tececo_dataseries.csv";
	string const ResultFileName = StudyRoot + "test_tececo_results_Results.csv";
	string const run2Dir = "futureSize10";
	string const ResultSize10FileName = StudyRoot + "/" + run2Dir + "/test_tececo_results_Results.csv";

	if (fs::exists(StudyRoot)) {
		fs::remove_all(StudyRoot);
	}
	if (!fs::exists(TEST_RESULTS)) {
		fs::create_directory(TEST_RESULTS);
	}
	fs::create_directory(StudyRoot);
	fs::copy_file(TEST_DATA + (std::string)"/test_tececo.json", vFileName);
	fs::copy_file(TEST_DATA + (std::string)"/test_tecEco_dataseries.csv", TimeseriesFileName);

	string const ReferenceResultFileName = TEST_DATA + (std::string)"/test_tececo_results_Results_Reference.csv";
	string const ReferenceResultSize10FileName = TEST_DATA + (std::string)"/test_tececo_results_Results_Size10_Reference.csv";

	// Read study
	TESTAPI("read study file from the file path: " + vFileName,
		m_Problem = m_Cairn.read_Study(vFileName)
	)

	// Read input timeseries
	TESTAPI("Read the Timeseries from the file path: " + TimeseriesFileName,
		m_Problem.add_TimeSeries(TimeseriesFileName)
	)

	// -------------- Run 1 --------------
	CairnAPI::SolutionAPI vSolution;
	TESTAPI("Run 1", vSolution = m_Problem.run())

	// Check status
	TESTAPI2("Check status of run 1", TestUtils::compare_scalar(vSolution.get_Status(), "Optimal", eString))

	// Compare results
	TESTAPI2("Compare results 1",
		TestUtils::ComparaisonCsvFile(ResultFileName, ReferenceResultFileName)
	)

	// -------------- Run 2 with FutureSize = 10 --------------

	// Change FutureSize to 10
	std::shared_ptr < CairnAPI::SimulationControlAPI> vSimulationControl = m_Problem.get_SimulationControl();
	TESTAPI("Change FutureSize to 10", vSimulationControl->set_SettingValue("FutureSize", 10))

	// Run 2
	TESTAPI("Run 2", vSolution = m_Problem.run(run2Dir))

	// Check status
	TESTAPI2("Check status of run 2", TestUtils::compare_scalar(vSolution.get_Status(), "Optimal", eString))

	// Compare results
	TESTAPI2("Compare results 2",
		TestUtils::ComparaisonCsvFile(ResultSize10FileName, ReferenceResultSize10FileName)
	)

	return noError;
}