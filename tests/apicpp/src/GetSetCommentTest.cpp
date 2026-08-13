#include "TEST_CairnCore.h"
#include <iostream>
#include "StudyCTest.h"


using namespace std;

/* This test verifies get/set comment(s) */

int main()
{
	StudyCTest vTest("formation_cairn", "getSetComment");
	CairnAPI m_Cairn;
	// Test with solver Cplex, if not exist test with solver Highs
	int vRet = vTest.readStudyChangeSolver(m_Cairn, "Cplex");
	if (vRet != noError && vRet != errType) return vRet;
	std::string vFileNameSaved = vTest.get_ResPrefixFile() + (std::string)"_saved.json";

	CairnAPI::OptimProblemAPI m_Problem = m_Cairn.get_Study();

	// Set param comments 

	std::shared_ptr < CairnAPI::SimulationControlAPI> vSimulationControl = m_Problem.get_SimulationControl();
	TESTAPI("Set the comment of SimulationControl.FutureSize",
		vSimulationControl->set_SettingComment("FutureSize", "This is FutureSize")
	) 

	std::shared_ptr < CairnAPI::TecEcoAnalysisAPI> vTecEcoAnalysis = m_Problem.get_TecEcoAnalysis();
	TESTAPI("Set the comment of TecEcoAnalysis.MaxConstraint",
		vTecEcoAnalysis->set_SettingComment("MaxConstraint", "This is MaxConstraint")
	)

	std::shared_ptr < CairnAPI::SolverAPI> vSolver = m_Problem.get_Solver();
	TESTAPI("Set the comment of Solver.Gab",
		vSolver->set_SettingComment("Gap", "This is Gap")
	)

	std::shared_ptr < CairnAPI::EnergyVectorAPI> vH2 = m_Problem.get_EnergyCarrier("H2");
	TESTAPI("Set the comment of H2.LHV",
		vH2->set_SettingComment("LHV", "This is LHV")
	)

	TESTAPI("Set the comments of H2 params",
		vH2->set_SettingComments({
			{"GHV", "This is GHV"},
			{"RHO", "This is RHO"}
		})
	)

	std::shared_ptr<CairnAPI::BusAPI> vElec_Bus = m_Problem.get_Bus("Elec_Bus");
	TESTAPI("Set the comment of Elec_Bus.UseExtrapolationFactor",
		vElec_Bus->set_SettingComment("UseExtrapolationFactor", "This is UseExtrapolationFactor")
	)

	TESTAPI("Set the comments of Elec_Bus params",
		vElec_Bus->set_SettingComments({
			{"InitBusValue", "This is InitBusValue"},
			{"StrictConstraintBusValue", "This is StrictConstraintBusValue"}
		})
	)

	std::shared_ptr<CairnAPI::MilpComponentAPI> vELY_PEM = m_Problem.get_Component("ELY_PEM");
	TESTAPI("Set the comment of ELY_PEM.Capex",
		vELY_PEM->set_SettingComment("Capex", "This is Capex")
	)

	TESTAPI("Set the comments of ELY_PEM params",
		vELY_PEM->set_SettingComments({
			{"FixedOpex", "This is FixedOpex"},
			{"Efficiency", "This is Efficiency"}
		})
	)

	std::shared_ptr < CairnAPI::MilpPortAPI> vELY_PEM_PortL0 = vELY_PEM->get_Port("PortL0");
	TESTAPI("Set the comment of ELY_PEM.PortL0.VariableOpex",
		vELY_PEM_PortL0->set_SettingComment("VariableOpex", "This is VariableOpex")
	)

	//Execute a simulation 

	TESTAPI("Read the Timeseries from the file path: " + vTest.get_TimeseriesFileName(),
		m_Problem.add_TimeSeries(vTest.get_TimeseriesFileName())
	)

	TESTAPI("Run", m_Problem.run())

	// Verify comments after simulation

	TESTAPI("Save study: " + vFileNameSaved,
		m_Problem.save_Study(vFileNameSaved)
	)

	TESTAPI("Close study:",
		m_Cairn.close_Study()
	)

	TESTAPI("Read saved study from the file path: " + vFileNameSaved,
		m_Problem = m_Cairn.read_Study(vFileNameSaved)
	)

	vSimulationControl = m_Problem.get_SimulationControl();
	TESTAPIBOOL("Verify the comment of SimulationControl.FutureSize",
		TestUtils::compare_scalar(vSimulationControl->get_SettingComment("FutureSize"), "This is FutureSize", eString)
	)

	vTecEcoAnalysis = m_Problem.get_TecEcoAnalysis();
	TESTAPIBOOL("Verify the comment of TecEcoAnalysis.MaxConstraint",
		TestUtils::compare_scalar(vTecEcoAnalysis->get_SettingComment("MaxConstraint"), "This is MaxConstraint", eString)
	)

	vSolver = m_Problem.get_Solver();
	TESTAPIBOOL("Verify the comment of Solver.Gab",
		TestUtils::compare_scalar(vSolver->get_SettingComment("Gap"), "This is Gap", eString)
	)

	vH2 = m_Problem.get_EnergyCarrier("H2");
	TESTAPIBOOL("Verify the comment of H2.LHV",
		TestUtils::compare_scalar(vH2->get_SettingComment("LHV"), "This is LHV", eString)
	)

	TESTAPIBOOL("Verify the comment of H2.GHV",
		TestUtils::compare_scalar(vH2->get_SettingComment("GHV"), "This is GHV", eString)
	)

	vElec_Bus = m_Problem.get_Bus("Elec_Bus");
	TESTAPIBOOL("Verify the comment of Elec_Bus.UseExtrapolationFactor",
		TestUtils::compare_scalar(vElec_Bus->get_SettingComment("UseExtrapolationFactor"), "This is UseExtrapolationFactor", eString)
	)

	TESTAPIBOOL("Verify the comment of Elec_Bus.MaxConstraint",
		TestUtils::compare_scalar(vElec_Bus->get_SettingComment("InitBusValue"), "This is InitBusValue", eString)
	)

	vELY_PEM = m_Problem.get_Component("ELY_PEM");
	TESTAPIBOOL("Verifythe comment of ELY_PEM.Capex",
		TestUtils::compare_scalar(vELY_PEM->get_SettingComment("Capex"), "This is Capex", eString)
	)

	TESTAPIBOOL("Verify the comments of ELY_PEM.vELY_PEM",
		TestUtils::compare_scalar(vELY_PEM->get_SettingComment("FixedOpex"), "This is FixedOpex", eString)
	)

	vELY_PEM_PortL0 = vELY_PEM->get_Port("PortL0");
	TESTAPIBOOL("Verify the comment of ELY_PEM.PortL0.VariableOpex",
		TestUtils::compare_scalar(vELY_PEM_PortL0->get_SettingComment("VariableOpex"), "This is VariableOpex", eString)
	)

	return noError;
}