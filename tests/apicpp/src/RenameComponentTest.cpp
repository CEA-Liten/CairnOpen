#include "TEST_CairnCore.h"
#include <iostream>
#include "Utils.h"
#include "UtilsJson.h"

using namespace std;

/* This test reads study formation_cairn_compoRenamed.json which differ from formation_cairn.json only by 
   renaming some components. 
   
   Then, it renames the components and executes a simulation befor comparing the result to the reference
   generated using formation_cairn.json
*/

int main()
{
	CairnAPI m_Cairn;
	CairnAPI::OptimProblemAPI m_Problem;

	//File PathsPersee
	string const StudyRoot = TEST_RESULTS + (std::string)"/renameComponent/";
	std::string vFileName = StudyRoot + (std::string)"/formation_cairn_compoRenamed.json";
	string const TimeseriesFileName = StudyRoot + (std::string)"/formation_cairn_dataseries.csv";
	string const ResultFileName = StudyRoot + "formation_cairn_compoRenamed_results_Results.csv";

	if (fs::exists(StudyRoot)) {
		fs::remove_all(StudyRoot);
	}
	if (!fs::exists(TEST_RESULTS)) {
		fs::create_directory(TEST_RESULTS);
	}
	fs::create_directory(StudyRoot);
	fs::copy_file(TEST_DATA + (std::string)"/formation_cairn_compoRenamed.json", vFileName);
	fs::copy_file(TEST_DATA + (std::string)"/formation_cairn_dataseries.csv", TimeseriesFileName);

	string const ReferenceResultFileName = TEST_DATA + (std::string)"/formation_cairn_Results_Reference.csv";

	TESTAPI("read study: " + vFileName, m_Problem = m_Cairn.read_Study(vFileName))

	CairnAPI::TecEcoAnalysisAPI vTecEcoAnalysis = m_Problem.get_TecEcoAnalysis();
	TESTAPI("rename TecEcoAnalysis: ", vTecEcoAnalysis.rename("TecEco"))

	CairnAPI::SolverAPI vSolver = m_Problem.get_Solver();
	TESTAPI("rename Solver: ", vSolver.rename("Cplex"))

	CairnAPI::SimulationControlAPI vSimulationControl = m_Problem.get_SimulationControl();
	TESTAPI("rename SimulationControl: ", vSimulationControl.rename("Cairn"))

	CairnAPI::MilpComponentAPI electrolyzer = m_Problem.get_Component("electrolyzer");
	TESTAPI("rename electrolyzer: ", electrolyzer.rename("ELY_PEM"))

	CairnAPI::BusAPI electrical_bus = m_Problem.get_Bus("Electrical_Bus");
	TESTAPI("rename Electrical_Bus: ", electrical_bus.rename("Elec_Bus"))

	CairnAPI::EnergyVectorAPI h2_vector = m_Problem.get_EnergyCarrier("H2Vector");
	TESTAPI("rename H2Vector: ", h2_vector.rename("H2"))

	//Execute a simulation then compare the result with the referance
	TESTAPI("Read the Timeseries: " + TimeseriesFileName, m_Problem.add_TimeSeries(TimeseriesFileName))

	TESTAPI("Run: ", m_Problem.run() )

	TESTAPI2("Compare results", TestUtils::ComparaisonCsvFile(ResultFileName, ReferenceResultFileName))

	return noError;
}