#include "TEST_PerseeLib.h"
#include <iostream>
#include "Utils.h"

using namespace std;


int main()
{
	//Files Paths
	std::string vFileName = TEST_DATA + (std::string)"/formation_persee.json";

	CairnAPI m_Persee;
	CairnAPI::OptimProblemAPI m_Problem;

	TESTAPI("read study",
		m_Problem = m_Persee.read_Study(vFileName)
	)

	t_list vComponents = m_Problem.get_Components();
	TestUtils::Display_list(vComponents);

	for (auto& vComponent : vComponents) {
		CairnAPI::MilpComponentAPI vComp = m_Problem.get_Component(vComponent);
		t_dict vParams = vComp.get_SettingValues();

		t_list vPorts = vComp.get_Ports();
		for (auto& vPortName : vPorts) {
			CairnAPI::MilpPortAPI vPort =  vComp.get_Port(vPortName);
			t_dict vPortParams = vPort.get_SettingValues();
		}
	}
	return noError;
}