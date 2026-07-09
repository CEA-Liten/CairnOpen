#include "CairnAPIUtils.h"
#include "TEST_CairnCore.h"
#include "StudyCTest.h"

using namespace std;

/* This test verifies that add/remove port of a Bus, 
*  and that add/remove bus-bus links works. 
*/

int main()
{
	string const StudyRoot = TEST_RESULTS + (std::string)"/busBusLink/";
	string const vFileName = StudyRoot + "busBusLink.json";

	if (fs::exists(StudyRoot)) {
		fs::remove_all(StudyRoot);
	}
	if (!fs::exists(TEST_RESULTS)) {
		fs::create_directory(TEST_RESULTS);
	}
	fs::create_directory(StudyRoot);

	// Create study 
	CairnAPI m_Cairn;
	CairnAPI::OptimProblemAPI m_Problem = m_Cairn.create_Study(vFileName);

	// Create EnergyVector
	CairnAPI::EnergyVectorAPI vElec(m_Problem, "ElectricityDistrib", "Electrical");

	// Create Buses
	CairnAPI::BusAPI vBus1(m_Problem, "Bus1", "NodeLaw", vElec);
	CairnAPI::BusAPI vBus2(m_Problem, "Bus2", "NodeLaw", vElec);

	// Add port to Bus1
	std::shared_ptr < CairnAPI::MilpPortAPI> vPortBus;
	TESTAPI("Add port: ", vPortBus = vBus1.add_Port("PortBus", vElec))

	t_list vBus1_Ports = vBus1.get_Ports();
	t_list vPorts_Ref = { "PortBus" };
	TESTAPIBOOL("Bus1 ports", TestUtils::compare_lists(vBus1_Ports, vPorts_Ref));

	vPortBus->set_SettingValues({
		{"Direction", "OUTPUT"},
		{"Variable", "BusBalance"}
	});

	// Add link
	TESTAPI("Add link: ", m_Problem.add(*vPortBus, vBus2))

	// Remove link
	TESTAPI("Remove link: ", m_Problem.remove(*vPortBus, vBus2))

	// Remove port
	TESTAPI("Remove port: ",  vBus1.remove_Port(*vPortBus))

	return noError;
}