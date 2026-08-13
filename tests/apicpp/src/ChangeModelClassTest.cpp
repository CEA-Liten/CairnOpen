#include "TEST_CairnCore.h"
#include <iostream>
#include "StudyCTest.h"

using namespace std;

/* This test :
   1- reads study formation_cairn_ModelClass.json (the only differance from formation_cairn.json is that ModelClass = ElectrolyzerDetailed) 
   2- loads formation_cairn_dataseries.csv
   3- changes the value of ModelClass from ElectrolyzerDetailed to Electrolyzer 
   4- executs a simulation
   5- compares the result to the reference formation_cairn__results_Reference.csv 
   which is generated using the GUI from formation_cairn.json (ModelClass = Electrolyzer).
*/

int main()
{
	CairnAPI m_Cairn;
	StudyCTest vTest("", "");
	std::string vSolverType = vTest.TrySolver(m_Cairn, "Cplex");
	if (vSolverType == "Highs") return noError;

	CairnAPI::OptimProblemAPI m_Problem;

	//File Paths
	string const StudyRoot = TEST_RESULTS + (std::string)"/changeModelClass/";
	std::string vFileName = StudyRoot + (std::string)"/formation_cairn_ModelClass.json";
	string const TimeseriesFileName = StudyRoot + (std::string)"/formation_cairn_dataseries.csv";
	string const ResultFileName = StudyRoot + "formation_cairn_ModelClass_results_Results.csv";

	if (fs::exists(StudyRoot)) {
		fs::remove_all(StudyRoot);
	}
	if (!fs::exists(TEST_RESULTS)) {
		fs::create_directory(TEST_RESULTS);
	}
	fs::create_directory(StudyRoot);
	fs::copy_file(TEST_DATA + (std::string)"/formation_cairn_ModelClass.json", vFileName);
	fs::copy_file(TEST_DATA + (std::string)"/formation_cairn_dataseries.csv", TimeseriesFileName);

	string const ReferenceResultFileName = TEST_DATA + (std::string)"/formation_cairn_Results_Reference.csv";

	TESTAPI("Read study file: " + vFileName, m_Problem = m_Cairn.read_Study(vFileName))

	std::shared_ptr<CairnAPI::MilpComponentAPI> vELY_PEM = m_Problem.get_Component("ELY_PEM");
	t_list refList = { "Electrolyzer", "ElectrolyzerDetailed" };
	t_list classList = vELY_PEM->get_PossibleModelClasses();
	TESTAPIBOOL("Check list of model classes", TestUtils::compare_lists(refList, classList))

	TESTAPIFALSE("Try using non-supported model class Converter", vELY_PEM->set_SettingValue("ModelClass", "Converter"))

	TESTAPI("Change ModelClass of ELY_PEM", vELY_PEM->set_SettingValue("ModelClass", "Electrolyzer"))
	vELY_PEM->set_SettingValues({
		{ "Capex", 480000 },
		{ "Opex", "0.04" },
		{ "Efficiency", "0.6667" },
		{ "MaxPower", "-30" },
		{ "MinPower", "0" },
		{"EnvironmentModel", true},
		{"Climate change#Global Warming Potential 100 EnvGreyContentCoefficient_A", 100},
		{"Climate change#Global Warming Potential 100 EnvGreyReplacement", 10}
	});

	TESTAPI("Read the Timeseries: " + TimeseriesFileName, m_Problem.add_TimeSeries(TimeseriesFileName))

	CairnAPI::SolutionAPI vSolution;
	TESTAPI("Run simulation", vSolution = m_Problem.run())

	TESTAPIBOOL("Compare results", TestUtils::ComparaisonCsvFile(ResultFileName, ReferenceResultFileName))
	
	return noError;
}