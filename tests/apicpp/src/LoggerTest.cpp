#include "TEST_CairnCore.h"
#include <iostream>
#include "Utils.h"
#include "UtilsJson.h"

using namespace std;

int main()
{
	CairnAPI m_Cairn; // no param, 
	CairnAPI::OptimProblemAPI m_Problem;

	string const Study = "formation_cairn";
	string const StudyRoot = TEST_RESULTS + (std::string)"/logger/";

	std::string vFileName = StudyRoot + Study + ".json";
	string const TimeseriesFileName = StudyRoot + Study + "_dataseries.csv";
	string const ResultFileName = StudyRoot + Study + "_results_Results.csv";

	if (fs::exists(StudyRoot)) {
		fs::remove_all(StudyRoot);
	}
	if (!fs::exists(TEST_RESULTS)) {
		fs::create_directory(TEST_RESULTS);
	}
	fs::create_directory(StudyRoot);
	fs::copy_file(TEST_DATA + (std::string)"/" + Study + ".json", vFileName);
	fs::copy_file(TEST_DATA + (std::string)"/" + Study + "_dataseries.csv", TimeseriesFileName);

	string const ReferenceResultFileName = TEST_DATA + (std::string)"/" + Study + (std::string)"_Results_Reference.csv";


	TESTAPI("read study file from the file path: " + vFileName,
		m_Problem = m_Cairn.read_Study(vFileName)
	)
	std::vector<std::string> vLogs = TestUtils::readCSV(StudyRoot + Study + ".log");
	TESTAPI2FALSE("test " + Study + ".log", (vLogs.size() > 0))


	CairnAPI m_Cairn2(false); // flag active or not console logs
	CairnAPI::OptimProblemAPI m_Problem2 = m_Cairn2.create_Study(StudyRoot + Study + "2.json");
	m_Problem2.initialize();
	std::vector<std::string> vLogs2 = TestUtils::readCSV(StudyRoot + Study + "2.log");
	TESTAPI2FALSE("test " + Study + "2.log", (vLogs2.size() > 0))		
	TESTAPI2FALSE("test " + Study + "2.log, find debug", (vLogs2[0].find("debug]") != std::string::npos))

	t_dict vDefLogs = { {"level", "info"} };
	CairnAPI m_Cairn3(vDefLogs); // paramètres
	CairnAPI::OptimProblemAPI m_Problem3 = m_Cairn3.create_Study(StudyRoot + Study + "3.json");
	m_Problem3.initialize();
	std::vector<std::string> vLogs3 = TestUtils::readCSV(StudyRoot + Study + "3.log");
	TESTAPI2FALSE("test " + Study + "3.log", (vLogs3.size() > 0))
	TESTAPI2FALSE("test " + Study + "3.log, find debug", (vLogs3[0].find("debug]") == std::string::npos))

	return noError;
}