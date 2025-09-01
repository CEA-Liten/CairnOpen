#include "TEST_CairnCore.h"
#include <iostream>
#include "Utils.h"
#include "UtilsJson.h"

using namespace std;

/* This test reads study formation_cairn.json and formation_cairn_dataseries.csv, 
   and a new EnergyVector, SourceLoad and Bus. 
   
   It the set the parameter StrictConstraint of the new Bus to false.
   
   Then, it executes a simulation and compares the result to the reference formation_cairn_BusParam_results_Reference.csv 
   which is generated using the GUI.

   The goal is to check if the value of Bus parameter StrictConstraint is taken into account. 
   When StrictConstraint = True (the default value) the study doesn't have a solution.
*/

int main()
{
	CairnAPI m_Cairn;
	CairnAPI::OptimProblemAPI m_Problem;

	//File PathsPersee
	string const StudyRoot = TEST_RESULTS + (std::string)"/modifyBusParam/";
	std::string vFileName = StudyRoot + (std::string)"/formation_cairn.json";
	string const TimeseriesFileName = StudyRoot + (std::string)"/formation_cairn_dataseries_2.csv";
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

	string const ReferenceResultFileName = TEST_DATA + (std::string)"/formation_cairn_BusParam_results_Reference.csv";

	TESTAPI("read study file from the file path: " + vFileName,
		m_Problem = m_Cairn.read_Study(vFileName)
	)

	//Create an energyVector
	CairnAPI::EnergyVectorAPI vCO2;
	TESTAPI("create EnergyVector CO2", vCO2 = m_Problem.create_EnergyCarrier("CO2", "Material"))

	//---------------- Create a SourceLoad ------------------------------
	CairnAPI::MilpComponentAPI vPV = m_Problem.create_Component("PV", "SourceLoad");
	vPV.set_SettingValues({
		{"Direction", "Source" },
		{"Weight", "1"},
		{"Opex", "0" },
		{"Capex", "1000" },
		{"MaxFlow", -10},
		{"EcoInvestModel", "1"},
		{"UseProfileLoadFlux","PVProduction"}
		}
	);

	//Configure the default port
	t_list vPV_DefaultPorts = vPV.get_DefaultPorts();
	CairnAPI::MilpPortAPI vPV_dPort;
	TESTAPI("get default port of ELY_PEM", vPV_dPort = vPV.get_Port(vPV_DefaultPorts[0]))

	CairnAPI::EnergyVectorAPI vElec = m_Problem.get_EnergyCarrier("ElectricityDistrib");
	TESTAPI("set the EnergyCarrier of the port", vPV_dPort.set_EnergyCarrier(vElec))

	vPV_dPort.set_SettingValues({
		{"Direction", "OUTPUT"},
		{"Variable", "SourceLoadFlow"}
	});

    //Add link from the default port to Elec_Bus
	CairnAPI::BusAPI vElec_Bus = m_Problem.get_Bus("Elec_Bus");
	m_Problem.add(vPV_dPort, vElec_Bus);

	//Add a new port to the SourceLoad "PV"
	CairnAPI::MilpPortAPI vPV_CarbonPort = vPV.add_Port("Port_CO2_content", vCO2);
	vPV_CarbonPort.set_SettingValues({
		{"Direction", "OUTPUT"},
		{"Variable", "SourceLoadFlow"},
		{"Coeff", 10},
		{"CheckUnit", "No"}
	});

	//Create a new Bus and add link to the new port of the SourceLoad PV
	CairnAPI::BusAPI vCO2_Bus;
	TESTAPI("add CO2_Bus", vCO2_Bus = m_Problem.create_Bus("CO2_Bus", "NodeLaw", vCO2))
	vCO2_Bus.set_SettingValue("StrictConstraint", false);

	m_Problem.add(vPV_CarbonPort, vCO2_Bus);

	//------ Execute a simulation then compare the result with the referance -----

	TESTAPI("Read the Timeseries from the file path: " + TimeseriesFileName,
		m_Problem.add_TimeSeries(TimeseriesFileName)
	)

	CairnAPI::SolutionAPI vSolution;

	TESTAPI("Run", vSolution = m_Problem.run())

	TESTAPI2("Compare results", TestUtils::ComparaisonCsvFile(ResultFileName, ReferenceResultFileName))

	return noError;
}