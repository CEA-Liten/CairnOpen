#include "TEST_CairnCore.h"
#include <iostream>
#include "StudyCTest.h"


int main()
{
	// TODO: add more checks about parameters and IOs
	string const StudyName = "getCompoData";
	std::string const StudyRoot = TEST_RESULTS + (std::string)"/getCompoData/";

	if (fs::exists(StudyRoot)) {
		fs::remove_all(StudyRoot);
	}
	if (!fs::exists(TEST_RESULTS)) {
		fs::create_directory(TEST_RESULTS);
	}
	fs::create_directory(StudyRoot);

	// Create Study
	CairnAPI m_Cairn;
	CairnAPI::OptimProblemAPI m_Problem = m_Cairn.create_Study(StudyRoot + StudyName);

	std::shared_ptr < CairnAPI::TecEcoAnalysisAPI> tecEco = m_Problem.get_TecEcoAnalysis();
	t_list vTecEcoIndicators = tecEco->get_IndicatorNames();
	TESTAPIBOOL("Check that Total CAPEX is in list of indicators", TestUtils::contains(vTecEcoIndicators, "Total CAPEX"))

	//Create EnergyVectors
	CairnAPI::EnergyVectorAPI vElec(m_Problem, "ElectricityDistrib", "Electrical");
	CairnAPI::EnergyVectorAPI vH2(m_Problem, "H2", "FluidH2");

	// Create Component
	CairnAPI::MilpComponentAPI vELY_PEM(m_Problem, "ELY_PEM", "Electrolyzer");

	t_list vELY_PEM_DefaultPorts = vELY_PEM.get_DefaultPorts();

	std::shared_ptr < CairnAPI::MilpPortAPI> vELY_PEM_R0;
	TESTAPI("get default port PortR0 of ELY_PEM", vELY_PEM_R0 = vELY_PEM.get_Port("PortR0"))
	TESTAPI("set the EnergyCarrier of the port", vELY_PEM_R0->set_EnergyCarrier(vH2))
	vELY_PEM_R0->set_SettingValues({
		{"Direction", "OUTPUT"},
		{"Variable", "H2MassFlowRate"}
	});

	std::shared_ptr < CairnAPI::MilpPortAPI> vELY_PEM_L0 = vELY_PEM.get_Port("PortL0");
	vELY_PEM_L0->set_EnergyCarrier(vElec);
	vELY_PEM_L0->set_SettingValues({
		{"Direction", "INPUT"},
		{"Variable", "UsedPower"}
	});

	t_list vParams = vELY_PEM.get_SettingsList();
	TESTAPIBOOL("Check that EcoInvestModel is in list of params", TestUtils::contains(vParams, "EcoInvestModel"))

	t_list vIOs = vELY_PEM.get_VarList();
	TESTAPIBOOL("Check that isInstalled is in list of IOs", TestUtils::contains(vIOs, "isInstalled"))

	t_list vIndicators = vELY_PEM.get_IndicatorNames();
	TESTAPIBOOL("Check that Annual consumption of ElectricalEnergy UsedPower is in list of indicators", 
		TestUtils::contains(vIndicators, "Annual consumption of ElectricalEnergy UsedPower"))

	// Select EnvImpacts GWP and AP
	std::shared_ptr < CairnAPI::TecEcoAnalysisAPI> vTecEcoAnalysis = m_Problem.get_TecEcoAnalysis();
	TESTAPI("Select EnvImpacts GWP and AP",
		vTecEcoAnalysis->set_SettingValues({
			{"ConsideredEnvironmentalImpacts", "Climate change#Global Warming Potential 100, Acidification#Accumulated Exceedance"}
		})
	)

	vTecEcoIndicators = tecEco->get_IndicatorNames();
	TESTAPIBOOL("Check that Total Project env impact of GWP is in list of indicators",
		TestUtils::contains(vTecEcoIndicators, "Total Project env impact of Climate change#Global Warming Potential 100")
	)

	TESTAPIBOOL("Check that Total Project env impact of AP is in list of indicators",
		TestUtils::contains(vTecEcoIndicators, "Total Project env impact of Acidification#Accumulated Exceedance")
	)

	vIndicators = vELY_PEM.get_IndicatorNames();
	TESTAPIBOOL("Check that Installed Optimal Size is in list of indicators", TestUtils::contains(vIndicators, "Installed Optimal Size"))
	TESTAPIBOOL("Check that GWP Operational impact mass is in list of indicators", TestUtils::contains(vIndicators, "Climate change#Global Warming Potential 100 Operational impact mass"))
	TESTAPIBOOL("Check that AP Operational impact mass is in list of indicators", TestUtils::contains(vIndicators, "Acidification#Accumulated Exceedance Operational impact mass"))

	// Unselect EnvImpacts AP
	TESTAPI("Unselect EnvImpacts AP",
		vTecEcoAnalysis->set_SettingValues({
			{"ConsideredEnvironmentalImpacts", "Climate change#Global Warming Potential 100"}
		})
	)

	vTecEcoIndicators = tecEco->get_IndicatorNames();
	TESTAPIBOOL("Check that Total Project env impact of GWP is in list of indicators",
		TestUtils::contains(vTecEcoIndicators, "Total Project env impact of Climate change#Global Warming Potential 100")
	)

	TESTAPIBOOLFALSE("Check that Total Project env impact of AP is NOT in list of indicators",
		TestUtils::contains(vTecEcoIndicators, "Total Project env impact of Acidification#Accumulated Exceedance")
	)

	vIndicators = vELY_PEM.get_IndicatorNames();
	TESTAPIBOOL("Check that GWP Operational impact mass is in list of indicators", TestUtils::contains(vIndicators, "Climate change#Global Warming Potential 100 Operational impact mass"))
	TESTAPIBOOLFALSE("Check that AP Operational impact mass is NOT in list of indicators", TestUtils::contains(vIndicators, "Acidification#Accumulated Exceedance Operational impact mass"))

	//Change the variable of port vELY_PEM_L0 
	vELY_PEM_L0->set_SettingValues({
		{"Variable", "MaxUsablePower"}
	});

	vIndicators = vELY_PEM.get_IndicatorNames();
	TESTAPIBOOL("Check that Annual consumption of ElectricalEnergy MaxUsablePower is in list of indicators",
		TestUtils::contains(vIndicators, "Annual consumption of ElectricalEnergy MaxUsablePower"))

	return noError;
}


