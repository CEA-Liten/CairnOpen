#include "TEST_CairnCore.h"
#include <iostream>
#include "Utils.h"

using namespace std;

int main()
{	
	std::string vFileName = TEST_DATA + (std::string)"/formation_cairn.json";

	CairnAPI m_Cairn;			
	CairnAPI::OptimProblemAPI m_Problem;
	TESTAPI("read study", m_Problem = m_Cairn.read_Study(vFileName))

	t_list vRef = { "H2", "ElectricityDistrib" };
	t_list vEnergyVectors = m_Problem.get_EnergyCarriers();
	TestUtils::Display_list(vEnergyVectors);
	int vErr = TestUtils::compare_lists(vRef, vEnergyVectors);

	if (vErr == noError) {
		for (auto& vEnergyVector : vEnergyVectors) {
			CairnAPI::EnergyVectorAPI vEnergy = m_Problem.get_EnergyCarrier(vEnergyVector);
			std::cout << vEnergy.get_Name() << std::endl;
			TESTAPIFALSE("remove EnergyVector", m_Problem.remove_EnergyCarrier(vEnergy))
		}
	}

	CairnAPI::EnergyVectorAPI vElec;
	TESTAPI("create EV", vElec = m_Problem.create_EnergyCarrier("Elec2", "Electrical"))
	TESTAPI("remove EV", m_Problem.remove_EnergyCarrier(vElec))
	
	return vErr;
}