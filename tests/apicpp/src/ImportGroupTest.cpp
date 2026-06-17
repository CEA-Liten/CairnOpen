#include "TEST_CairnCore.h"
#include <iostream>
#include "StudyCTest.h"
#include "UtilsJson.h"

using namespace std;

/* This test reads formation_cairn_incomplete.json, then import group formation_cairn_group.json 
*  and adds a link to have a complete study to simulate formation_cairn.json.

   After, it executes a simulation and compares the result to the reference formation_cairn_Results_Reference.csv 
   which is generated from formation_cairn.json using the GUI.
*/
int testImportGroup(StudyTest& a_Test, CairnAPI& a_Cairn, const std::string& a_SolverName)
{
	CairnAPI::OptimProblemAPI m_Problem = a_Cairn.get_Study();

	TESTAPI("Import group: " + a_Test.get_ExtraFileName(0),
		m_Problem.import_Group(a_Test.get_ExtraFileName(0))
	)

		std::shared_ptr<CairnAPI::MilpComponentAPI> vELY_PEM;
	TESTAPI("Get component ELY_PEM.",
		vELY_PEM = m_Problem.get_Component("ELY_PEM")
	)

		std::shared_ptr < CairnAPI::MilpPortAPI> vELY_PEM_PortR0;
	TESTAPI("Get port PortR0 of ELY_PEM.",
		vELY_PEM_PortR0 = vELY_PEM->get_Port("PortR0")
	)

		std::shared_ptr < CairnAPI::BusAPI> vH2_Bus;
	TESTAPI("Get bus H2_Bus.",
		vH2_Bus = m_Problem.get_Bus("H2_Bus")
	)

	TESTAPI("Add link ELY_PEM -> H2_Bus.",
		m_Problem.add(*vELY_PEM_PortR0, *vH2_Bus)
	)

	//Execute a simulation then compare the result with the referance

	TESTAPI("Read the Timeseries from the file path: " + a_Test.get_TimeseriesFileName(),
		m_Problem.add_TimeSeries(a_Test.get_TimeseriesFileName())
	)

	m_Problem.run(a_SolverName);
	return noError;
}

int main()
{
	StudyCTest vTest("formation_cairn", "importGroup", { "group.json" }, "_incomplete");
	CairnAPI m_Cairn;
	// Test with solver Cplex, if not exist test with solver Highs
	int vRet = vTest.readStudyChangeSolver(m_Cairn, "Cplex");
	if (vRet != noError && vRet != errType) return vRet;
	
	vRet = vTest.runAndcheck(m_Cairn, "", testImportGroup);
	if (vRet != noError) return vRet;
	
	return noError;
}