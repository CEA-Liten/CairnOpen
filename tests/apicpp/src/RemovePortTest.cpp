#include "TEST_CairnCore.h"
#include <iostream>
#include "StudyCTest.h"
#include "UtilsJson.h"

using namespace std;

/* This test reads study formation_cairn.json and formation_cairn_dataseries.csv,
   and add then remove a port.
   
   Then, it executes a simulation and compares the result to the reference formation_cairn_Results_Reference.csv 
   which is generated using the GUI.
*/
int test(StudyCTest& a_Test, CairnAPI& a_Cairn, const std::string& a_SolverName)
{
	CairnAPI::OptimProblemAPI vProblem = a_Cairn.get_Study();

	TESTAPI("Run",
		vProblem.run(a_SolverName)
	)

	TESTAPI2FALSE("Check Run",
		a_Test.checkResults("Reference", true, true)
	)

	return 0;
}

int main()
{
	StudyCTest vTest("formation_cairn", "removePort");
	
	CairnAPI m_Cairn;
	// Test with solver Cplex, if not exist test with solver Highs
	int vRet = vTest.readStudyChangeSolver(m_Cairn, "Cplex");
	if (vRet != noError && vRet != errType) return vRet;
	bool vTestCplexHighs = (vRet == noError);

	CairnAPI::OptimProblemAPI m_Problem = m_Cairn.get_Study();
	
	// Add a port to ELY_PEM
	std::shared_ptr < CairnAPI::MilpPortAPI> vELY_PEM_Port;
	std::shared_ptr<CairnAPI::MilpComponentAPI> vELY_PEM = m_Problem.get_Component("ELY_PEM");
	std::shared_ptr <CairnAPI::EnergyVectorAPI> vElec = m_Problem.get_EnergyCarrier("ElectricityDistrib");
	TESTAPI("Add port", vELY_PEM_Port = vELY_PEM->add_Port("ELY_PEM_Port", *vElec))

	// Verify if related EnvImpacts have been added
	TESTAPI2("Check if ELY_PEM_Port-related EnvImpact exists", 
		TestUtils::contains(vELY_PEM->get_SettingsList(), "ELY_PEM_Port.Climate change#Global Warming Potential 100 EnvContentCoefficient_A")
	)

	// Remove port
	
	TESTAPI("Get ELY_PEM_Port object", vELY_PEM_Port = vELY_PEM->get_Port("ELY_PEM_Port"))
	TESTAPI("Remove ELY_PEM_Port", vELY_PEM->remove_Port(*vELY_PEM_Port))

	// Verify if related EnvImpacts have been deleted
	TESTAPI2FALSE("Check if ELY_PEM_Port-related EnvImpact is deleted",
		TestUtils::contains(vELY_PEM->get_SettingsList(), "ELY_PEM_Port.Climate change#Global Warming Potential 100 EnvContentCoefficient_A")
	)

	// Add a port to TecEcoAnalysis 
	std::shared_ptr < CairnAPI::MilpPortAPI> vTecEcoPort;
	std::shared_ptr < CairnAPI::TecEcoAnalysisAPI> vTecEcoAnalysis = m_Problem.get_TecEcoAnalysis();
	TESTAPI("Add port to TecEcoAnalysis", vTecEcoPort = vTecEcoAnalysis->add_Port("TecEcoPort", *vElec))
	vTecEcoPort->set_SettingValues({
		{"Direction", "OUTPUT"},
		{"Variable", "Total Capex"}
	});

	TESTAPI2("Check the ports of TecEcoAnalysis", TestUtils::contains(vTecEcoAnalysis->get_Ports(), "TecEcoPort"))

	std::shared_ptr < CairnAPI::BusAPI> vElec_Bus = m_Problem.get_Bus("Elec_Bus");
	TESTAPI("Add link to TecEcoAnalysis", m_Problem.add(*vTecEcoPort, *vElec_Bus))

	t_dict vTecEcoLinksRef = { {vTecEcoAnalysis->get_Name() + (std::string)".TecEcoPort",  "Elec_Bus"} };
	t_dict vTecEcoLinks;
	vTecEcoAnalysis->get_Links(vTecEcoLinks);
	TESTAPI2("Check the links of TecEcoAnalysis", TestUtils::compare_dict(vTecEcoLinks, vTecEcoLinksRef))

	TESTAPI("Remove the TecEcoAnalysis link", m_Problem.remove(*vTecEcoPort, *vElec_Bus))
	vTecEcoLinks = {};
	vTecEcoAnalysis->get_Links(vTecEcoLinks);
	TESTAPI2("Check the TecEcoAnalysis link has been removed", TestUtils::compare_dict(vTecEcoLinks, {}))

	TESTAPI("Remove the TecEcoAnalysis port", vTecEcoAnalysis->remove_Port(*vTecEcoPort))
	TESTAPI2("Check the TecEcoAnalysis port has been removed", TestUtils::compare_lists(vTecEcoAnalysis->get_Ports(), {}))

	//Execute a simulation then compare the result with the referance


	TESTAPI("Read the Timeseries from the file path: " + vTest.get_TimeseriesFileName(),
		m_Problem.add_TimeSeries(vTest.get_TimeseriesFileName())
	)

	if (vTestCplexHighs) {
		// Test Cplex and Highs
			
		int vRetTest = test(vTest, m_Cairn, "");
		if (vRetTest) return vRetTest;

		vRetTest = vTest.readStudyChangeSolver(m_Cairn, "Highs", true);
		if (vRetTest) return vRetTest;

		vRetTest = test(vTest, m_Cairn, "Highs");
		if (vRetTest) return vRetTest;
	}
	else {
		// Test only Highs
		int vRetTest = test(vTest, m_Cairn, "");
		if (vRetTest) return vRetTest;
	}

	return noError;
}