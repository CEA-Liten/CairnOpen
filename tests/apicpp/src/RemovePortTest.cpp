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
	CairnAPI::MilpComponentAPI vELY_PEM = m_Problem.get_Component("ELY_PEM");
	CairnAPI::EnergyVectorAPI vElec = m_Problem.get_EnergyCarrier("ElectricityDistrib");
	TESTAPI("Add port", vELY_PEM.add_Port("myPort", vElec))

	// Verify if related EnvImpacts have been added
	TESTAPI2("Check if myPort-related EnvImpact exists", 
		TestUtils::contains(vELY_PEM.get_SettingsList(), "myPort.Climate change#Global Warming Potential 100 EnvContentCoefficient_A")
	)

	// Remove port
	CairnAPI::MilpPortAPI myPort;
	TESTAPI("Get myPort object", myPort = vELY_PEM.get_Port("myPort"))
	TESTAPI("Remove myPort", vELY_PEM.remove_Port(myPort))

	// Verify if related EnvImpacts have been deleted
	TESTAPI2FALSE("Check if myPort-related EnvImpact is deleted",
		TestUtils::contains(vELY_PEM.get_SettingsList(), "myPort.Climate change#Global Warming Potential 100 EnvContentCoefficient_A")
	)

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