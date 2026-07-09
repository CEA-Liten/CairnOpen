#include "TEST_CairnCore.h"
#include <iostream>
#include "StudyCTest.h"


int main()
{
	StudyCTest vTest("formation_cairn", "getObjects");
	CairnAPI m_Cairn;
	// Test with solver Cplex, if not exist test with solver Highs
	int vRet = vTest.readStudyChangeSolver(m_Cairn, "Cplex");
	if (vRet != noError && vRet != errType) return vRet;

	CairnAPI::OptimProblemAPI m_Problem = m_Cairn.get_Study();
	

	// tests list objects
	t_list vObjectNames = m_Problem.get_Objects();
	t_list vBuses, vCarriers, vComps, vSolver, vSimCtrl, vTecEco, vOthers;
	for (auto& vObjectName : vObjectNames) {
		std::shared_ptr<CairnAPI::ObjectAPI> vObject=m_Problem.get_Object(vObjectName);
		if (vObject->get_ObjectType() == "BusCompo")
			vBuses.push_back(vObjectName);
		else if (vObject->get_ObjectType() == "EnergyVector")
			vCarriers.push_back(vObjectName);
		else if (vObject->get_ObjectType() == "MilpComponent")
			vComps.push_back(vObjectName);
		else if (vObject->get_ObjectType() == "Solver")
			vSolver.push_back(vObjectName);
		else if (vObject->get_ObjectType() == "SimulationControl")
			vSimCtrl.push_back(vObjectName);
		else if (vObject->get_ObjectType() == "TecEcoCompo")
			vTecEco.push_back(vObjectName);
		else
			vOthers.push_back(vObjectName);
	}
	t_list vBuses2 = m_Problem.get_Buses();
	TESTAPI("compare list buses", TestUtils::compare_lists(vBuses, vBuses2))
	t_list vCarriers2 = m_Problem.get_EnergyCarriers();
	TESTAPI("compare list carriers", TestUtils::compare_lists(vCarriers, vCarriers2))
	t_list vComps2 = m_Problem.get_Components();
	TESTAPI("compare list components", TestUtils::compare_lists(vComps, vComps2))

	TESTAPIBOOL("test solver", (vSolver.size() == 1))
	std::string vSolverName = m_Problem.get_Solver()->get_Name();
	TESTAPIBOOL("compare solver name", (vSolverName == vSolver[0]))

		TESTAPIBOOL("test simulation control", (vSimCtrl.size() == 1))
	std::string vSimName = m_Problem.get_SimulationControl()->get_Name();
	TESTAPIBOOL("compare simulation name", (vSimName == vSimCtrl[0]))

		TESTAPIBOOL("test tecEco", (vTecEco.size() == 1))
	std::string vTecEcoName = m_Problem.get_TecEcoAnalysis()->get_Name();
	TESTAPIBOOL("compare tecEco name", (vTecEcoName == vTecEco[0]))

		TESTAPIBOOL("test others", (vOthers.size() == 0))

	TESTAPIFALSE("object doesn't exist", std::shared_ptr < CairnAPI::ObjectAPI> vObject = m_Problem.get_Object("Hello"));

	// tests settings object
	std::shared_ptr < CairnAPI::ObjectAPI> vObjectComp = m_Problem.get_Object("H2_Load");
	t_list vSettings = vObjectComp->get_SettingsList();	
	TESTAPIBOOL("Verify the value of MaxFlow.",
		TestUtils::compare_scalar(vObjectComp->get_SettingValue("MaxFlow"), 1000.0, eDouble)
	)

	t_dict vValues = m_Problem.get_Object(vSimCtrl[0])->get_SettingValues();
	TESTAPIBOOL("Verify the value of MaxFlow.",
		TestUtils::compare_scalar(vValues["FutureSize"], 48, eInt)
	)
	TESTAPI("set value", m_Problem.get_Object(vSimCtrl[0])->set_SettingValue("FutureSize", 24))

	// test TechEco
	std::shared_ptr < CairnAPI::ObjectAPI> vObjectTecEco = m_Problem.get_Object(vTecEco[0]);
	t_list vSettings2 = vObjectTecEco->get_SettingsList();
	TESTAPIBOOL("test TecEco settings", (vSettings2.size() > 2))

	/*
	Faire un test qui vérifie que le set_value dérivé a bient été appelé (cas tecEco, ...)

	Attention, un run appelle la méthode initialize qui détruit et reconstruit tous les ModelParam
	L'objet ParamAPI n'a plus le bon pointer vers ModelParam
	!!! Il faut recréer le ParamAPI 
	
	Pour lire/changer les valeurs d'un paramètre, il faut mieux pour le moment passer par l'objet et les méthodes get/set setting
	
	*/
	return noError;
}


