#include "TEST_CairnCore.h"
#include <iostream>
#include "Utils.h"
#include "UtilsJson.h"

int main()
{
	CairnAPI m_Cairn;
	CairnAPI::OptimProblemAPI m_Problem;

	//File PathsPersee
	std::string const StudyRoot = TEST_RESULTS + (std::string)"/getObjects/";
	std::string vFileName = StudyRoot + (std::string)"/formation_cairn.json";

	if (fs::exists(StudyRoot)) {
		fs::remove_all(StudyRoot);
	}
	if (!fs::exists(TEST_RESULTS)) {
		fs::create_directory(TEST_RESULTS);
	}
	fs::create_directory(StudyRoot);
	fs::copy_file(TEST_DATA + (std::string)"/formation_cairn.json", vFileName);

	TESTAPI("read study file from the file path: " + vFileName,
		m_Problem = m_Cairn.read_Study(vFileName)
	)

	// tests list objects
	t_list vObjectNames = m_Problem.get_Objects();
	t_list vBuses, vCarriers, vComps, vSolver, vSimCtrl, vTecEco, vOthers;
	for (auto& vObjectName : vObjectNames) {
		CairnAPI::ObjectAPI vObject=m_Problem.get_Object(vObjectName);
		if (vObject.get_ObjectType() == "BusCompo")
			vBuses.push_back(vObjectName);
		else if (vObject.get_ObjectType() == "EnergyVector")
			vCarriers.push_back(vObjectName);
		else if (vObject.get_ObjectType() == "MilpComponent")
			vComps.push_back(vObjectName);
		else if (vObject.get_ObjectType() == "Solver")
			vSolver.push_back(vObjectName);
		else if (vObject.get_ObjectType() == "SimulationControl")
			vSimCtrl.push_back(vObjectName);
		else if (vObject.get_ObjectType() == "TecEcoCompo")
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

	TESTAPI2FALSE("test solver", (vSolver.size() == 1))
	std::string vSolverName = m_Problem.get_Solver().get_Name();
	TESTAPI2FALSE("compare solver name", (vSolverName == vSolver[0]))

	TESTAPI2FALSE("test simulation control", (vSimCtrl.size() == 1))
	std::string vSimName = m_Problem.get_SimulationControl().get_Name();
	TESTAPI2FALSE("compare simulation name", (vSimName == vSimCtrl[0]))

	TESTAPI2FALSE("test tecEco", (vTecEco.size() == 1))
	std::string vTecEcoName = m_Problem.get_TecEcoAnalysis().get_Name();
	TESTAPI2FALSE("compare tecEco name", (vTecEcoName == vTecEco[0]))

	TESTAPI2FALSE("test others", (vOthers.size() == 0))

	TESTAPIFALSE("object doesn't exist", CairnAPI::ObjectAPI vObject = m_Problem.get_Object("Hello"));

	// tests settings object
	CairnAPI::ObjectAPI vObjectComp = m_Problem.get_Object("H2_Load");
	t_list vSettings = vObjectComp.get_SettingsList();	
	TESTAPI2("Verify the value of MaxFlow.",
		TestUtils::compare_scalar(vObjectComp.get_SettingValue("MaxFlow"), 1000.0, eDouble)
	)

	t_dict vValues = m_Problem.get_Object(vSimCtrl[0]).get_SettingValues();
	TESTAPI2("Verify the value of MaxFlow.",
		TestUtils::compare_scalar(vValues["FutureSize"], 48, eInt)
	)
	TESTAPI("set value", m_Problem.get_Object(vSimCtrl[0]).set_SettingValue("FutureSize", 24))

	// test TechEco
	CairnAPI::ObjectAPI vObjectTecEco = m_Problem.get_Object(vTecEco[0]);
	t_list vSettings2 = vObjectTecEco.get_SettingsList();
	TESTAPI2FALSE("test TecEco settings", (vSettings2.size() > 2))

	return noError;
}


