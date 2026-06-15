#include "CairnAPIUtils.h"
#include "TEST_CairnCore.h"
#include <iostream>
#include "StudyCTest.h"
#include <iomanip>

int main()
{
	CairnAPI m_Cairn;
	StudyCTest vTest("", "");
	std::string vSolverType = vTest.TrySolver(m_Cairn, "Cplex");
	if (vSolverType == "Highs") return noError; // No test if solver is Highs

	CairnAPI::OptimProblemAPI m_Problem;

	string const StudyRoot = TEST_RESULTS + (std::string)"/testLabels/";
	std::string vFileName = StudyRoot + (std::string)"/formation_cairn.json";
	string const TimeseriesFileName = StudyRoot + (std::string)"/formation_cairn_dataseries.csv";

	std::string vFileName_saved = StudyRoot + (std::string)"/formation_cairn_saved.json";

	if (fs::exists(StudyRoot)) {
		fs::remove_all(StudyRoot);
	}
	if (!fs::exists(TEST_RESULTS)) {
		fs::create_directory(TEST_RESULTS);
	}
	fs::create_directory(StudyRoot);
	fs::copy_file(TEST_DATA + (std::string)"/formation_cairn.json", vFileName);
	fs::copy_file(TEST_DATA + (std::string)"/formation_cairn_dataseries.csv", TimeseriesFileName);

	TESTAPI("read study file: " + vFileName, m_Problem = m_Cairn.read_Study(vFileName))

	TESTAPI2("check list of labels 1", TestUtils::compare_lists(m_Problem.get_Labels(), {}))

	m_Problem.set_Labels({ "country", "year", "cost"});
	m_Problem.add_Label("site");	
	m_Problem.remove_Label("cost");
	TESTAPI2("check list of labels 2", TestUtils::compare_lists(m_Problem.get_Labels(), { "country", "year", "site" }))

	std::shared_ptr<CairnAPI::MilpComponentAPI> vELY_PEM = m_Problem.get_Component("ELY_PEM");
	TESTAPI("set labels: ", vELY_PEM->set_LabelValues({ {"country", "France"}, {"year", "1990"} }))
	TESTAPI("set label: ", vELY_PEM->set_LabelValue("year", "2000"))
	TESTAPI2("Verify the value of country of ELY_PEM", TestUtils::compare_scalar(vELY_PEM->get_LabelValue("country"), "France", eString))
	TESTAPI2("check ELY_PEM labels", TestUtils::compare_dict(vELY_PEM->get_LabelValues(), { {"country", "France"}, {"year", "2000"}, {"site", ""} }))

	std::shared_ptr < CairnAPI::BusAPI> vElec_Bus = m_Problem.get_Bus("Elec_Bus");
	TESTAPI("set labels: ", vElec_Bus->set_LabelValues({ {"country", "UK"}, {"site", "London"} }))
	TESTAPI("set label: ", vElec_Bus->set_LabelValue("year", "1995"))
	TESTAPI2("Verify the value of year of Elec_Bus", TestUtils::compare_scalar(vElec_Bus->get_LabelValue("year"), "1995", eString))
	TESTAPI2("check Elec_Bus labels", TestUtils::compare_dict(vElec_Bus->get_LabelValues(), { {"country", "UK"}, {"year", "1995"}, {"site", "London"} }))

	TESTAPI("Read the Timeseries: " + TimeseriesFileName, m_Problem.add_TimeSeries(TimeseriesFileName))

	TESTAPI("Run: ", m_Problem.run())

	TESTAPI("Save Study: ", m_Problem.save_Study(vFileName_saved))
	
	TESTAPI("Close Study: ", m_Cairn.close_Study())

	// ----- Read the saved file and verify that label values are correct ----
	TESTAPI("read saved file: " + vFileName_saved, m_Problem = m_Cairn.read_Study(vFileName_saved))
	TESTAPI2("check list of labels 3", TestUtils::compare_lists(m_Problem.get_Labels(), { "country", "year", "site" }))

	vELY_PEM = m_Problem.get_Component("ELY_PEM");
	TESTAPI2("check ELY_PEM labels 2", TestUtils::compare_dict(vELY_PEM->get_LabelValues(), { {"country", "France"}, {"year", "2000"}, {"site", ""} }))

	vElec_Bus = m_Problem.get_Bus("Elec_Bus");
	TESTAPI2("check Elec_Bus labels 2", TestUtils::compare_dict(vElec_Bus->get_LabelValues(), { {"country", "UK"}, {"year", "1995"}, {"site", "London"} }))

	return noError;
}