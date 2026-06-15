#include "TEST_CairnCore.h"
#include <iostream>
#include "StudyCTest.h"
#include "UtilsJson.h"

using namespace std;

/* This test reads study formation_cairn.json and formation_cairn_dataseries.csv
	use class ParamAPI to display and change value of a parameter
*/

int main()
{
	CairnAPI m_Cairn;
	StudyCTest vTest("", "");
	std::string vSolverType = vTest.TrySolver(m_Cairn, "Cplex");
	if (vSolverType == "Highs") return noError; // No test if solver is Highs

	CairnAPI::OptimProblemAPI m_Problem;

	string const Study = "formation_cairn";
	string const StudyRoot = TEST_RESULTS + (std::string)"/readParam/";



	std::string vFileName = StudyRoot + Study + ".json";
	string const TimeseriesFileName = StudyRoot + Study + "_dataseries.csv";
	string const ResultFileName = StudyRoot + Study + "_results_Results.csv";

	if (fs::exists(StudyRoot)) {
		fs::remove_all(StudyRoot);
	}
	if (!fs::exists(TEST_RESULTS)) {
		fs::create_directory(TEST_RESULTS);
	}
	fs::create_directory(StudyRoot);
	fs::copy_file(TEST_DATA + (std::string)"/" + Study + ".json", vFileName);
	fs::copy_file(TEST_DATA + (std::string)"/" + Study + "_dataseries.csv", TimeseriesFileName);

	string const ReferenceResultFileName = TEST_DATA + (std::string)"/" + Study + (std::string)"_Results_Reference.csv";
	string const ReferenceResultFileName_2 = TEST_DATA + (std::string)"/formation_cairn_Results_Reference_run2.csv";

	TESTAPI("read study file from the file path: " + vFileName,
		m_Problem = m_Cairn.read_Study(vFileName)
	)

	std::shared_ptr<CairnAPI::MilpComponentAPI> vELY_PEM = m_Problem.get_Component("ELY_PEM");
	CairnAPI::ParamAPI vELY_PEM_P1 = vELY_PEM->get_Setting("Capex");
	TESTAPI2("Verify the value of ELY_PEM.Capex.",
		TestUtils::compare_scalar(vELY_PEM_P1.get_Value(), 480000.0, eDouble)
	)


	TESTAPI("Read the Timeseries from the file path: " + TimeseriesFileName,
		m_Problem.add_TimeSeries(TimeseriesFileName)
	)

	CairnAPI::SolutionAPI vSolution;

	TESTAPI("Run 1",
		vSolution = m_Problem.run()
	)

	TESTAPI2("Compare results",
		TestUtils::ComparaisonCsvFile(ResultFileName, ReferenceResultFileName)
	)
		TESTAPI("Export time series", vSolution.exportTimeSeries())


	std::shared_ptr < CairnAPI::SimulationControlAPI> vSimulationControl = m_Problem.get_SimulationControl();
	CairnAPI::ParamAPI vSimulationControl_P1 = vSimulationControl->get_Setting("FutureSize");
	
	vSimulationControl_P1.set_Value(156);
	TESTAPI2("Verify the value of SimulationControl",
		TestUtils::compare_scalar(vSimulationControl->get_SettingValue("FutureSize"), 156, eInt)
	)
	TESTAPI2("Verify the value of SimulationControl",
		TestUtils::compare_scalar(vSimulationControl_P1.get_Value(), 156, eInt)
	)
	
	TESTAPI("Run 2",
		m_Problem.run()
	)

	TESTAPI2("Compare results for run 2",
		TestUtils::ComparaisonCsvFile(ResultFileName, ReferenceResultFileName_2)
	)
	return noError;
}