#include "TEST_CairnCore.h"
#include <iostream>
#include "StudyCTest.h"

int test(StudyCTest& a_Test, CairnAPI& a_Cairn, const std::string& a_SolverName)
{
	CairnAPI::OptimProblemAPI vProblem = a_Cairn.get_Study();
	TESTAPI("Ru",
		vProblem.run(a_SolverName)
	)

	std::shared_ptr<CairnAPI::MilpComponentAPI> vELY_PEM = vProblem.get_Component("ELY_PEM");
	CairnAPI::VariableAPI vVar = vELY_PEM->get_Variable("MaxPower");

	double vValue = std::get<eDouble>(vVar.get_Value());
	TESTVALUE(vValue, 2.483921)
	
	return noError;
}

int main()
{
	StudyCTest vTest("formation_cairn", "readVariable");

	CairnAPI m_Cairn;

	// Test with solver Cplex, if not exist test with solver Highs
	int vRet = vTest.readStudyChangeSolver(m_Cairn, "Cplex");
	if (vRet != noError && vRet != errType) return vRet;
	bool vTestCplexHighs = (vRet == noError);

	CairnAPI::OptimProblemAPI m_Problem = m_Cairn.get_Study();

	std::shared_ptr<CairnAPI::MilpComponentAPI> vELY_PEM = m_Problem.get_Component("ELY_PEM");
	t_list vVars = vELY_PEM->get_VarList();
	t_list::iterator vIter = find(vVars.begin(), vVars.end(), "MaxPower");
	TESTAPIBOOL("MaxPower must be a variable of ELY_PEM", vIter != vVars.end());

	CairnAPI::VariableAPI vVar = vELY_PEM->get_Variable("MaxPower");
	std::string vUnit = vVar.get_Unit();
	TESTAPIBOOL("MW must be the unit of MaxPower ", vUnit == "MW");

	CairnAPI::VariableAPI vVarErr = vELY_PEM->get_Variable("NoVar");
	std::string vCmt = vVarErr.get_Description();
	TESTAPIBOOL("The variable NoVar must not exist", vCmt == "");

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
