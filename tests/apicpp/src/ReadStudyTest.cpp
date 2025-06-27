#include "TEST_CairnCore.h"
#include <iostream>
#include "Utils.h"
#include "UtilsJson.h"

using namespace std;

/* This test reads study formation_persee.json and formation_persee_dataseries.csv,
   and compare the values of some parameters before executing a simulation.
   
   Then, it executes a simulation and compares the result to the reference formation_persee_Results_Reference.csv 
   which is generated using the GUI.
*/

int main()
{
	CairnAPI m_Persee;
	CairnAPI::OptimProblemAPI m_Problem;

	//File Paths
	string const StudyRoot = TEST_RESULTS + (std::string)"/readStudy/";
	std::string vFileName = StudyRoot + (std::string)"/formation_persee.json";
	string const TimeseriesFileName = StudyRoot + (std::string)"/formation_persee_dataseries.csv";
	string const ResultFileName = StudyRoot + "formation_persee_Results.csv";

	if (fs::exists(StudyRoot)) {
		fs::remove_all(StudyRoot);
	}
	if (!fs::exists(TEST_RESULTS)) {
		fs::create_directory(TEST_RESULTS);
	}
	fs::create_directory(StudyRoot);
	fs::copy_file(TEST_DATA + (std::string)"/formation_persee.json", vFileName);
	fs::copy_file(TEST_DATA + (std::string)"/formation_persee_dataseries.csv", TimeseriesFileName);

	string const ReferenceResultFileName = TEST_DATA + (std::string)"/formation_persee_Results_Reference.csv";

	TESTAPI("read study file from the file path: " + vFileName,
		m_Problem = m_Persee.read_Study(vFileName)
	)

	//Verify the values of some parameters with static ref values

	CairnAPI::MilpComponentAPI ely_pem = m_Problem.get_Component("ELY_PEM");
	TESTAPI2("Verify the value of ELY_PEM.Capex.",
		TestUtils::compare_scalar(ely_pem.get_SettingValue("Capex"), 480000.0, eDouble)
	)

	TESTAPI2("Verify the value of ELY_PEM.GWP#EnvGreyContentCoefficient_A.",
		TestUtils::compare_scalar(ely_pem.get_SettingValue("Climate change#Global Warming Potential 100 EnvGreyContentCoefficient_A"), 100.0, eDouble)
	)

	TESTAPI2("Verify the value of ELY_PEM.ModelClass.",
		TestUtils::compare_scalar(ely_pem.get_SettingValue("ModelClass"), std::string("Electrolyzer"), eString)
	)

	CairnAPI::MilpPortAPI ely_pem_PortL0 = ely_pem.get_Port("PortL0");
	TESTAPI2("Verify the value of ELY_PEM.PortL0.coeff",
		TestUtils::compare_scalar(ely_pem_PortL0.get_SettingValue("Coeff"), 1.0, eDouble)
	)

	TESTAPI2("Verify the value of ELY_PEM.PortL0.offset",
		TestUtils::compare_scalar(ely_pem_PortL0.get_SettingValue("Offset"), 0.0, eDouble)
	)

	CairnAPI::MilpComponentAPI elec_grid = m_Problem.get_Component("Elec_Grid");
	TESTAPI2("Verify the value of Elec_Grid.PortR0.GWP#EnvContentCoefficient_A.",
		TestUtils::compare_scalar(elec_grid.get_SettingValue("PortR0.Climate change#Global Warming Potential 100 EnvContentCoefficient_A"), 20.0, eDouble)
	)

	CairnAPI::MilpComponentAPI h2_tank = m_Problem.get_Component("H2_Tank");
	TESTAPI2("Verify the value of H2_Tank.MaxFlowCharge.",
		TestUtils::compare_scalar(h2_tank.get_SettingValue("MaxFlowCharge"), 1100.0, eDouble)
	)

	CairnAPI::EnergyVectorAPI evH2 = m_Problem.get_EnergyCarrier("H2");
	TESTAPI2("Verify the value of H2.LHV.",
		TestUtils::compare_scalar(evH2.get_SettingValue("LHV"), 0.03332, eDouble)
	)

	TESTAPI2("Verify the value of H2.RHO.",
		TestUtils::compare_scalar(evH2.get_SettingValue("RHO"), 0.0899, eDouble)
	)

	TESTAPI2("Verify the value of SimulationControl.UseExtrapolationFactor.",
		TestUtils::compare_scalar(m_Problem.get_SimulationControlSettings()["UseExtrapolationFactor"], true, eBool)
	)//true is the default value. Parameter UseExtrapolationFactor doesn't exist in formation_persee.json

	TESTAPI2("Verify the value of Solver.NbSolToKeep.",
		TestUtils::compare_scalar(m_Problem.get_MIPSolverSettings()["NbSolToKeep"], 1, eInt)
	)//1 is the default value. Parameter NbSolToKeep doesn't exist in formation_persee.json

	TESTAPI2("Verify the value of Solver.Gap.",
		TestUtils::compare_scalar(m_Problem.get_MIPSolverSettings()["Gap"], 0.001, eDouble)
	)

	std::vector<std::string> ConsideredEnvironmentalImpacts = { "Climate change#Global Warming Potential 100",
																"Acidification#Accumulated Exceedance" };
	TESTAPI2("Verify the value of TecEco.ConsideredEnvironmentalImpacts.",
		TestUtils::compare_scalar(m_Problem.get_TechEcoAnalysisSettings()["ConsideredEnvironmentalImpacts"], ConsideredEnvironmentalImpacts, eStringList)
	)
		

	//Execute a simulation then compare the result with the referance

	TESTAPI("Read the Timeseries from the file path: " + TimeseriesFileName,
		m_Problem.add_TimeSeries(TimeseriesFileName)
	)

	CairnAPI::SolutionAPI vSolution;

	TESTAPI("Run",
		vSolution = m_Problem.run()
	)
	vSolution.exportTimeSeries();

	TESTAPI2("Compare results", 
		TestUtils::ComparaisonCsvFile(ResultFileName, ReferenceResultFileName)
	)
	
	return noError;
}