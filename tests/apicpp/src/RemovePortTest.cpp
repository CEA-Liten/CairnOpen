#include "TEST_CairnCore.h"
#include <iostream>
#include "Utils.h"
#include "UtilsJson.h"

using namespace std;

/* This test reads study formation_cairn.json and formation_cairn_dataseries.csv,
   and add then remove a port.
   
   Then, it executes a simulation and compares the result to the reference formation_cairn_Results_Reference.csv 
   which is generated using the GUI.
*/

int main()
{
	CairnAPI m_Cairn;
	CairnAPI::OptimProblemAPI m_Problem;

	//File PathsPersee
	string const StudyRoot = TEST_RESULTS + (std::string)"/readStudy/";
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

	string const ReferenceResultFileName = TEST_DATA + (std::string)"/formation_cairn_Results_Reference.csv";

	TESTAPI("read study file from the file path: " + vFileName,
		m_Problem = m_Cairn.read_Study(vFileName)
	)

	// Add a port to ELY_PEM
	CairnAPI::MilpPortAPI vELY_PEM_Port;
	CairnAPI::MilpComponentAPI vELY_PEM = m_Problem.get_Component("ELY_PEM");
	CairnAPI::EnergyVectorAPI vElec = m_Problem.get_EnergyCarrier("ElectricityDistrib");
	TESTAPI("Add port", vELY_PEM_Port = vELY_PEM.add_Port("ELY_PEM_Port", vElec))

	// Verify if related EnvImpacts have been added
	TESTAPI2("Check if ELY_PEM_Port-related EnvImpact exists", 
		TestUtils::contains(vELY_PEM.get_SettingsList(), "ELY_PEM_Port.Climate change#Global Warming Potential 100 EnvContentCoefficient_A")
	)

	// Remove port
	
	TESTAPI("Get ELY_PEM_Port object", vELY_PEM_Port = vELY_PEM.get_Port("ELY_PEM_Port"))
	TESTAPI("Remove ELY_PEM_Port", vELY_PEM.remove_Port(vELY_PEM_Port))

	// Verify if related EnvImpacts have been deleted
	TESTAPI2FALSE("Check if ELY_PEM_Port-related EnvImpact is deleted",
		TestUtils::contains(vELY_PEM.get_SettingsList(), "ELY_PEM_Port.Climate change#Global Warming Potential 100 EnvContentCoefficient_A")
	)

	// Add a port to TecEcoAnalysis 
	CairnAPI::MilpPortAPI vTecEcoPort;
	CairnAPI::TecEcoAnalysisAPI vTecEcoAnalysis = m_Problem.get_TecEcoAnalysis();
	TESTAPI("Add port to TecEcoAnalysis", vTecEcoPort = vTecEcoAnalysis.add_Port("TecEcoPort", vElec))
	vTecEcoPort.set_SettingValues({
		{"Direction", "OUTPUT"},
		{"Variable", "Total Capex"}
	});

	TESTAPI2("Check the ports of TecEcoAnalysis", TestUtils::contains(vTecEcoAnalysis.get_Ports(), "TecEcoPort"))

	CairnAPI::BusAPI vElec_Bus = m_Problem.get_Bus("Elec_Bus");
	TESTAPI("Add link to TecEcoAnalysis", m_Problem.add(vTecEcoPort, vElec_Bus))

	t_dict vTecEcoLinksRef = { {vTecEcoAnalysis.get_Name() + (std::string)".TecEcoPort",  "Elec_Bus"} };
	t_dict vTecEcoLinks;
	vTecEcoAnalysis.get_Links(vTecEcoLinks);
	TESTAPI2("Check the links of TecEcoAnalysis", TestUtils::compare_dict(vTecEcoLinks, vTecEcoLinksRef))

	TESTAPI("Remove the TecEcoAnalysis link", m_Problem.remove(vTecEcoPort, vElec_Bus))
	vTecEcoLinks = {};
	vTecEcoAnalysis.get_Links(vTecEcoLinks);
	TESTAPI2("Check the TecEcoAnalysis link has been removed", TestUtils::compare_dict(vTecEcoLinks, {}))

	TESTAPI("Remove the TecEcoAnalysis port", vTecEcoAnalysis.remove_Port(vTecEcoPort))
	TESTAPI2("Check the TecEcoAnalysis port has been removed", TestUtils::compare_lists(vTecEcoAnalysis.get_Ports(), {}))

	//Execute a simulation then compare the result with the referance

	TESTAPI("Read the Timeseries from the file path: " + TimeseriesFileName,
		m_Problem.add_TimeSeries(TimeseriesFileName)
	)

	TESTAPI("Run", m_Problem.run() )

	TESTAPI2("Compare results", 
		TestUtils::ComparaisonCsvFile(ResultFileName, ReferenceResultFileName)
	)

	return noError;
}