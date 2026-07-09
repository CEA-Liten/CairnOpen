#include "TEST_CairnCore.h"
#include <iostream>
#include "StudyCTest.h"


using namespace std;

/* This test reads study formation_cairn.json and formation_cairn_dataseries.csv,
   and compare the values of some parameters before executing a simulation.
   
   Then, it executes a simulation and compares the result to the reference formation_cairn_Results_Reference.csv 
   which is generated using the GUI.
*/
int test(StudyCTest &a_Test, CairnAPI& a_Cairn, const std::string& a_SolverName)
{	
	CairnAPI::OptimProblemAPI vProblem = a_Cairn.get_Study();
	CairnAPI::SolutionAPI vSolution;

	TESTAPI("Run 1",
		vSolution = vProblem.run(a_SolverName)
	)

	TESTAPIBOOL("Check Run 1",
		a_Test.checkResults("Reference", true, true)
	)


	TESTAPI("Export time series", vSolution.exportTimeSeries())

	std::shared_ptr < CairnAPI::SimulationControlAPI> vSimulationControl = vProblem.get_SimulationControl();
	TESTAPI("Modify FutureSize",
		vSimulationControl->set_SettingValues({
			{"FutureSize", 156}
			})
	)

	TESTAPI("Run 2",
		vProblem.run(a_SolverName)
	)

	TESTAPIBOOL("Check Run 2",
		a_Test.checkResults("Reference_run2")
	)

	TESTAPI("Run 3",
		vSolution = vProblem.run(a_SolverName)
	)

	TESTAPIBOOL("Check Run 3",
		a_Test.checkResults("Reference_run2")
	)
	
	return 0;
}

int main()
{
	StudyCTest vTest("formation_cairn", "readStudy");

	CairnAPI m_Cairn;
	
	// Test with solver Cplex, if not exist test with solver Highs
	int vRet = vTest.readStudyChangeSolver(m_Cairn, "Cplex");
	if (vRet != noError && vRet != errType) return vRet;
	bool vTestCplexHighs = (vRet == noError);

	CairnAPI::OptimProblemAPI m_Problem = m_Cairn.get_Study();

	//Verify the values of some parameters with static ref values

	std::shared_ptr<CairnAPI::MilpComponentAPI> vELY_PEM = m_Problem.get_Component("ELY_PEM");
	TESTAPIBOOL("Verify the value of ELY_PEM.Capex.",
		TestUtils::compare_scalar(vELY_PEM->get_SettingValue("Capex"), 480000.0, eDouble)
	)

	TESTAPIBOOL("Verify the value of ELY_PEM.GWP#EmbodiedCoefficient_A.",
		TestUtils::compare_scalar(vELY_PEM->get_SettingValue("Climate change#Global Warming Potential 100 EmbodiedCoefficient_A"), 100.0, eDouble)
	)

	TESTAPIBOOL("Verify the value of ELY_PEM.ModelClass.",
		TestUtils::compare_scalar(vELY_PEM->get_SettingValue("ModelClass"), std::string("Electrolyzer"), eString)
	)

	std::shared_ptr < CairnAPI::MilpPortAPI> ely_pem_PortL0 = vELY_PEM->get_Port("PortL0");
	TESTAPIBOOL("Verify the value of ELY_PEM.PortL0.coeff",
		TestUtils::compare_scalar(ely_pem_PortL0->get_SettingValue("Coeff"), 1.0, eDouble)
	)

	TESTAPIBOOL("Verify the value of ELY_PEM.PortL0.offset",
		TestUtils::compare_scalar(ely_pem_PortL0->get_SettingValue("Offset"), 0.0, eDouble)
	)

	std::shared_ptr<CairnAPI::MilpComponentAPI> elec_grid = m_Problem.get_Component("Elec_Grid");
	TESTAPIBOOL("Verify the value of Elec_Grid.PortR0.GWP#EnvContentCoefficient_A.",
		TestUtils::compare_scalar(elec_grid->get_SettingValue("PortR0.Climate change#Global Warming Potential 100 EnvContentCoefficient_A"), 20.0, eDouble)
	)

	std::shared_ptr<CairnAPI::MilpComponentAPI> h2_tank = m_Problem.get_Component("H2_Tank");
	TESTAPIBOOL("Verify the value of H2_Tank.MaxFlowCharge.",
		TestUtils::compare_scalar(h2_tank->get_SettingValue("MaxFlowCharge"), 1100.0, eDouble)
	)

	std::shared_ptr < CairnAPI::EnergyVectorAPI> evH2 = m_Problem.get_EnergyCarrier("H2");
	TESTAPIBOOL("Verify the value of H2.LHV.",
		TestUtils::compare_scalar(evH2->get_SettingValue("LHV"), 0.03332, eDouble)
	)

	TESTAPIBOOL("Verify the value of H2.RHO.",
		TestUtils::compare_scalar(evH2->get_SettingValue("RHO"), 0.0899, eDouble)
	)

	std::shared_ptr < CairnAPI::SimulationControlAPI> vSimulationControl = m_Problem.get_SimulationControl();
	TESTAPIBOOL("Verify the value of SimulationControl.UseExtrapolationFactor.",
		TestUtils::compare_scalar(vSimulationControl->get_SettingValue("UseExtrapolationFactor"), true, eBool)
	)//true is the default value. Parameter UseExtrapolationFactor doesn't exist in formation_cairn.json

	std::shared_ptr < CairnAPI::SolverAPI> vSolver = m_Problem.get_Solver();
	TESTAPIBOOL("Verify the value of Solver.NbSolToKeep.",
		TestUtils::compare_scalar(vSolver->get_SettingValue("NbSolToKeep"), 1, eInt)
	)//1 is the default value. Parameter NbSolToKeep doesn't exist in formation_cairn.json

	

	std::vector<std::string> ConsideredEnvironmentalImpacts = { "Climate change#Global Warming Potential 100",
																"Acidification#Accumulated Exceedance" };

	std::shared_ptr < CairnAPI::TecEcoAnalysisAPI> vTecEcoAnalysis = m_Problem.get_TecEcoAnalysis();
	TESTAPIBOOL("Verify the value of TecEco.ConsideredEnvironmentalImpacts.",
		TestUtils::compare_scalar(vTecEcoAnalysis->get_SettingValue("ConsideredEnvironmentalImpacts"), ConsideredEnvironmentalImpacts, eStringList)
	)
		

	//Execute a simulation then compare the result with the referance

	TESTAPI("Read the Timeseries from the file path: " + vTest.get_TimeseriesFileName(),
		m_Problem.add_TimeSeries(vTest.get_TimeseriesFileName())
	)

	if (vTestCplexHighs) {
		// Test Cplex and Highs

		TESTAPIBOOL("Verify the value of Solver.Gap.",
			TestUtils::compare_scalar(vSolver->get_SettingValue("Gap"), 0.001, eDouble)
		)
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