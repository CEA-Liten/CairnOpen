#include "TEST_CairnCore.h"
#include <iostream>
#include "StudyCTest.h"

using namespace std;

/* Test get/set param comment */

int main()
{
	CairnAPI m_Cairn;
	StudyCTest vTest("", "");
	std::string vSolverType = vTest.TrySolver(m_Cairn, "Cplex");
	if (vSolverType == "Highs") return noError; // No test if solver is Highs

	CairnAPI::OptimProblemAPI m_Problem;

	//File PathsPersee
	string const StudyRoot = TEST_RESULTS + (std::string)"/paramComment/";
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

	// Get component ELY_PEM
	std::shared_ptr < CairnAPI::MilpComponentAPI> vELY_PEM;
	TESTAPI("Get component ELY_PEM", vELY_PEM = m_Problem.get_Component("ELY_PEM"))

	// Get/Set a param comment
	const std::string fixedOpexComment = "This is FixedOpex"; // Should match the Opex comment in vFileName
	//TESTAPIBOOL("Verify FixedOpex comment:",
	//	TestUtils::compare_scalar(vELY_PEM.get_SettingComment("FixedOpex"), fixedOpexComment, eString)
	//)

	const std::string capexComment = "This is Capex";
	TESTAPI("Set Capex comment", vELY_PEM->set_SettingComment("Capex", capexComment))

	TESTAPIBOOL("Verify Capex comment",
		TestUtils::compare_scalar(vELY_PEM->get_SettingComment("Capex"), capexComment, eString)
	)

	// Get/Set the comments of all params
	const std::string maxPowerComment = "This is MaxPower";
	TESTAPI("Set comments of ELY_PEM params", vELY_PEM->set_SettingComments({ {"MaxPower", maxPowerComment} }))

	t_dictComment vELY_PEMComments;
	TESTAPI("Get comments of ELY_PEM params", vELY_PEMComments = vELY_PEM->get_SettingComments())

	TESTAPIBOOL("Verify MaxPower comment",
		TestUtils::compare_scalar(vELY_PEMComments["MaxPower"], maxPowerComment, eString)
	)

	return noError;
}