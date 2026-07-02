#include "TEST_CairnCore.h"
#include <iostream>
#include "StudyCTest.h"
#include "UtilsJson.h"

using namespace std;

/* This test reads study formation_cairn_Edited.json and formation_cairn_dataseries.csv.
   formation_cairn_Edited.json differ from formation_cairn.json by :
   1- FutureSize is 24 instead of 48
   2- DiscountRate is 0.09 instead of 0.07
   2- Climate change#Global Warming Potential 100 and Ozone depletion#Ozone Depletion Potential are selected in TecEcoAnalysis (instead of GWP and AP)
   3- EnvironmentModel of Wind_farm is false
   4- Climate change#Global Warming Potential 100 EnvGreyContentCoefficient_A of Wind_farm is 100 (instead of 250)

   The test modifies the study to be idientical to formation_cairn.json.  The objective is to ensure that :
   1- All the componenets are correctly re-initialized after SimulationControl or TecEcoAnalysis parameters are modified
   2- EnvImpact parameters are correctly updated for all components when an EnvImpact is selected/unselected (ConsideredEnvironmentalImpacts is modified)
   3- EnvImpact parameters of a componeent are accessible after setting EnvironmentModel to true.

   Then, it executes a simulation and compares the result to the reference formation_cairn_Results_Reference.csv 
   which is generated using formation_cairn.json.
*/

int main()
{
	CairnAPI m_Cairn;
	StudyCTest vTest("", "");
	std::string vSolverType = vTest.TrySolver(m_Cairn, "Cplex");
	if (vSolverType == "Highs") return noError; // No test if solver is Highs

	CairnAPI::OptimProblemAPI m_Problem;

	//File Paths
	string const StudyRoot = TEST_RESULTS + (std::string)"/setGlParams/";
	std::string vFileName = StudyRoot + (std::string)"/formation_cairn_Edited.json";
	string const TimeseriesFileName = StudyRoot + (std::string)"/formation_cairn_dataseries.csv";
	string const ResultFileName = StudyRoot + "formation_cairn_Edited_results_Results.csv";

	if (fs::exists(StudyRoot)) {
		fs::remove_all(StudyRoot);
	}
	if (!fs::exists(TEST_RESULTS)) {
		fs::create_directory(TEST_RESULTS);
	}
	fs::create_directory(StudyRoot);
	fs::copy_file(TEST_DATA + (std::string)"/formation_cairn_Edited.json", vFileName);
	fs::copy_file(TEST_DATA + (std::string)"/formation_cairn_dataseries.csv", TimeseriesFileName);

	string const ReferenceResultFileName = TEST_DATA + (std::string)"/formation_cairn_Results_Reference.csv";

	TESTAPI("read study file from the file path: " + vFileName,
		m_Problem = m_Cairn.read_Study(vFileName)
	)

	//Modify SimulationControl parameters (FutureSize)
	std::shared_ptr < CairnAPI::SimulationControlAPI> vSimulationControl = m_Problem.get_SimulationControl();
	TESTAPI("create_simulationcontrol",
		vSimulationControl->set_SettingValue("FutureSize", 48)
	)

	//Modify TecEcoAnalysis parameters
	std::shared_ptr < CairnAPI::TecEcoAnalysisAPI> vTecEcoAnalysis = m_Problem.get_TecEcoAnalysis();
	TESTAPI("set_TecEcoAnalysisSettings",
		vTecEcoAnalysis->set_SettingValues({
			{"DiscountRate", 0.07},
			{"ConsideredEnvironmentalImpacts", "Climate change#Global Warming Potential 100,Acidification#Accumulated Exceedance"}
		})
	)

	//Check if ELY_PEM has GWP-related parameters after selection of GWP in TecEcoAnalysis
	std::shared_ptr<CairnAPI::MilpComponentAPI> vELY_PEM = m_Problem.get_Component("ELY_PEM");
	TESTAPI2("Check if ELY_PEM has GWP parameter after selection",
		TestUtils::contains(vELY_PEM->get_SettingsList(), "Climate change#Global Warming Potential 100 EmbodiedCoefficient_A")
	)

		//Check if ELY_PEM has GWP-related IO vars after selection of GWP in TecEcoAnalysis
	TESTAPI2("Check if ELY_PEM has GWP IO var after selection",
		TestUtils::contains(vELY_PEM->get_VarList(), "Climate change#Global Warming Potential 100 Env impact mass")
	)

	//Check if the ODP-related parameters of ELY_PEM have been removed after unselection of ODP in TecEcoAnalysis
	TESTAPI2FALSE("Check if ODP param is removed",
		TestUtils::contains(vELY_PEM->get_SettingsList(), "Ozone depletion#Ozone Depletion Potential EnvContentCoefficient_A")
	)

	//Check if the ODP-related IO vars of ELY_PEM have been removed after unselection of ODP in TecEcoAnalysis
	TESTAPI2FALSE("Check if ODP IO var is removed",
		TestUtils::contains(vELY_PEM->get_VarList(), "Ozone depletion#Ozone Depletion Potential Env impact mass")
	)

	//Modify the parameters of Wind_farm
	std::shared_ptr<CairnAPI::MilpComponentAPI> vWind_farm = m_Problem.get_Component("Wind_farm");
	TESTAPI("Modify the parameters of Wind_farm",
		vWind_farm->set_SettingValues({
			{"EnvironmentModel", true},  
			{"Climate change#Global Warming Potential 100 EmbodiedCoefficient_A", 250}
		})
	)
		
	//Execute a simulation then compare the result with the referance
	TESTAPI("Read the Timeseries from the file path: " + TimeseriesFileName,
		m_Problem.add_TimeSeries(TimeseriesFileName)
	)

	CairnAPI::SolutionAPI vSolution;
	TESTAPI("Run",
		vSolution = m_Problem.run()
	)

	TESTAPI2("Compare results", 
		TestUtils::ComparaisonCsvFile(ResultFileName, ReferenceResultFileName)
	)
	
	return noError;
}