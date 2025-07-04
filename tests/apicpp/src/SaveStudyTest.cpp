#include "TEST_CairnCore.h"
#include <iostream>
#include "Utils.h"
#include "UtilsJson.h"

using namespace std;

/* This test reads study formation_persee.json, then save it as formation_persee_saved.json.

   After, it reads formation_persee_saved.json and formation_persee_dataseries.csv,
   then executes a simulation for the study formation_persee_saved.json.

   At the end, it compares the result formation_persee_saved_Results.csv to the reference 
   formation_persee_Results_Reference.csv which is generated from formation_persee.json using the GUI.
*/

int main()
{
	CairnAPI m_Persee;
	CairnAPI::OptimProblemAPI m_Problem;

	//File Paths
	string const StudyRoot = TEST_RESULTS + (std::string)"/saveStudy/";
	std::string vFileName = StudyRoot + (std::string)"/formation_persee_saved.json";
	string const TimeseriesFileName = StudyRoot + (std::string)"/formation_persee_dataseries.csv";
	string const ResultFileName = StudyRoot + "formation_persee_saved_Results.csv";

	if (fs::exists(StudyRoot)) {
		fs::remove_all(StudyRoot);
	}
	if (!fs::exists(TEST_RESULTS)) {
		fs::create_directory(TEST_RESULTS);
	}
	fs::create_directory(StudyRoot);
	fs::copy_file(TEST_DATA + (std::string)"/formation_persee_dataseries.csv", TimeseriesFileName);

	string const ReferenceStudyFileName = TEST_DATA + (std::string)"/formation_persee.json";
	string const ReferenceResultFileName = TEST_DATA + (std::string)"/formation_persee_Results_Reference.csv";

	TESTAPI("read study file from the data: " + ReferenceStudyFileName,
		m_Problem = m_Persee.read_Study(ReferenceStudyFileName)
	)

	//Test setting port attribute
	CairnAPI::MilpComponentAPI ely_pem = m_Problem.get_Component("ELY_PEM");
	CairnAPI::MilpPortAPI ely_pem_PortL0 = ely_pem.get_Port("PortL0");

	ely_pem_PortL0.set_SettingValue("Coeff", 2.0);

	TESTAPI2("Verify the value of ELY_PEM.PortL0.coeff",
		TestUtils::compare_scalar(ely_pem_PortL0.get_SettingValue("Coeff"), 2.0, eDouble)
	)

	//Re-set the coeff value to 1.0
	ely_pem_PortL0.set_SettingValue("Coeff", 1.0);

	t_list ReferenceListOfComponent = m_Problem.get_Components();
	TESTAPI("write study file in the file path: " + vFileName,
		m_Problem.save_Study(vFileName)
	)

	//CompareJsonFiles doesn't accept missing parameters
	//TESTAPI("Compare json",
	//	TestUtilsJson::CompareJsonFiles(vFileName, ReferenceStudyFileName)
	//)

	CairnAPI m_Persee2;
	CairnAPI::OptimProblemAPI m_Problem2;

	TESTAPI("read study file generated by m_Problem.save_Study: " + vFileName,
		m_Problem2 = m_Persee2.read_Study(vFileName)
	)

	TESTAPI("Read the Timeseries from the file path : " + TimeseriesFileName,
		m_Problem2.add_TimeSeries(TimeseriesFileName)
	)

	CairnAPI::SolutionAPI vSolution;

	TESTAPI("Run",
		vSolution = m_Problem2.run()
	)
	vSolution.exportTimeSeries();

	CairnAPI::MilpComponentAPI ely_pem_2 = m_Problem2.get_Component("ELY_PEM");

	m_Problem2.get_Indicators("ELY_PEM");
	ely_pem_2.get_IndicatorNames();
	ely_pem_2.get_IndicatorValues("PLAN");

	TESTAPI2("Compare results", 
		TestUtils::ComparaisonCsvFile(ResultFileName, ReferenceResultFileName)
	)
	
	return noError;
}