#include "TEST_CairnCore.h"
#include <iostream>
#include "StudyCTest.h"

using namespace std;

int main()
{	
	std::string vFileName = TEST_DATA + (std::string)"/formation_cairn.json";

	CairnAPI m_Cairn;	
	StudyCTest vTest("", "");
	std::string vSolverType = vTest.TrySolver(m_Cairn, "Cplex");
	if (vSolverType == "Highs") return noError; // No test if solver is Highs

	CairnAPI::OptimProblemAPI m_Problem;
	TESTAPI("read study", m_Problem = m_Cairn.read_Study(vFileName))

	t_list vRef = { "H2", "ElectricityDistrib" };
	t_list vEnergyVectors = m_Problem.get_EnergyCarriers();
	TestUtils::Display_list(vRef, "reference");
	TestUtils::Display_list(vEnergyVectors, "read study");
	int vErr = TestUtils::compare_lists(vEnergyVectors, vRef);

	if (vErr == noError) {
		for (auto& vEnergyVector : vEnergyVectors) {
			std::shared_ptr < CairnAPI::EnergyVectorAPI> vEnergy = m_Problem.get_EnergyCarrier(vEnergyVector);
			std::cout << vEnergy->get_Name() << std::endl;
			TESTAPIFALSE("remove EnergyVector", m_Problem.remove_EnergyCarrier(*vEnergy))
		}
	}

	std::shared_ptr < CairnAPI::EnergyVectorAPI> vElec;
	TESTAPI("create EV", vElec = m_Problem.create_EnergyCarrier("Elec2", "ElectricalCarrier"))
	TESTAPI("remove EV", m_Problem.remove_EnergyCarrier(*vElec))

	std::shared_ptr < CairnAPI::EnergyVectorAPI> vMaterial;
	TESTAPI("create EV", vMaterial = m_Problem.create_EnergyCarrier("Mat", "MaterialCarrier"))
	
	/*CairnAPI::EnergyVectorAPI vFluid;
	TESTAPI("create EV Fluid", vFluid = m_Problem.create_EnergyCarrier("Fluid", "FluidH2"))
	TESTAPI("remove EV Fluid", m_Problem.remove_EnergyCarrier(vFluid))*/

	return vErr;
}