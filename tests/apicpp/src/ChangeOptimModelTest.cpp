#include "TEST_CairnCore.h"
#include <iostream>
#include "StudyCTest.h"


using namespace std;

/* This test reads study formation_cairn.json and formation_cairn_dataseries.csv,
   then change the value of Optim Model before executing a simulation.
   
   Then, it compares the result to the reference formation_cairn_EnvImpactGWP_results_Reference.csv 
   which is generated using the GUI for the same Optim Model.
*/

int main()
{
	CairnAPI m_Cairn;
	StudyCTest vTest("", "");
	std::string vSolverType = vTest.TrySolver(m_Cairn, "Cplex");
	if (vSolverType == "Highs") return noError;

	CairnAPI::OptimProblemAPI m_Problem;

	//File Paths
	string const StudyRoot = TEST_RESULTS + (std::string)"/changeOptimModel/";
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

	string const ReferenceResultFileName = TEST_DATA + (std::string)"/formation_cairn_EnvImpactGWP_results_Reference.csv";

	TESTAPI("Read study file: " + vFileName, m_Problem = m_Cairn.read_Study(vFileName))

	std::shared_ptr < CairnAPI::TecEcoAnalysisAPI> vTecEcoAnalysis = m_Problem.get_TecEcoAnalysis();
	TESTAPI("Change Optim Model", vTecEcoAnalysis->set_SettingValues( { {"Model", "OptimEnvImpact-GWP"} } ))

	TESTAPI("Read the Timeseries: " + TimeseriesFileName, m_Problem.add_TimeSeries(TimeseriesFileName))

	CairnAPI::SolutionAPI vSolution;
	TESTAPI("Run simulation", vSolution = m_Problem.run())

	TESTAPIBOOL("Compare results", TestUtils::ComparaisonCsvFile(ResultFileName, ReferenceResultFileName))
	
	return noError;
}