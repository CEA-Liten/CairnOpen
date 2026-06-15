#include "TEST_CairnCore.h"
#include <iostream>
#include "StudyCTest.h"

using namespace std;


int main()
{
	StudyCTest vTest("formation_cairn", "");
	CairnAPI m_Cairn;
	// Test with solver Cplex, if not exist test with solver Highs
	int vRet = vTest.readStudyChangeSolver(m_Cairn, "Cplex");
	if (vRet != noError && vRet != errType) return vRet;
	CairnAPI::OptimProblemAPI m_Problem = m_Cairn.get_Study();
	
	t_list vComponents = m_Problem.get_Components();
	TestUtils::Display_list(vComponents);

	for (auto& vComponent : vComponents) {
		std::shared_ptr <CairnAPI::MilpComponentAPI> vComp = m_Problem.get_Component(vComponent);
		t_dict vParams = vComp->get_SettingValues();

		t_list vPorts = vComp->get_Ports();
		for (auto& vPortName : vPorts) {
			std::shared_ptr < CairnAPI::MilpPortAPI> vPort =  vComp->get_Port(vPortName);
			t_dict vPortParams = vPort->get_SettingValues();
		}
	}
	return noError;
}