#include "TEST_CairnCore.h"
#include <iostream>
#include "StudyCTest.h"


#include "OrJsonUtils.h"

using namespace std;

int main()
{
	CairnAPI m_Cairn; // no param, 
	StudyCTest vTest("", "");
	std::string vSolverType = vTest.TrySolver(m_Cairn, "Cplex");
	if (vSolverType == "Highs") return noError; // No test if solver is Highs

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
	TESTAPIBOOL("test " + Study + ".log", (vLogs.size() > 0))


	// ----- Get level from logger preferences and verify the log file, default value = "info" -----
	std::string level = "info";
	const std::string prefFileName = std::getenv("CAIRN_BIN") + (std::string)"/../resources/Prefs.json";
	json input;
	std::ifstream file(prefFileName);
	if (file.is_open()) {
		try {
			file >> input;
			if (input.contains("logs")) {
				const json& vLogsCfg = input["logs"];
				std::string vStr;
				if (orjson::from_json(vLogsCfg, "level", vStr)) {
					if (vStr != "") level = vStr;
				}
			}
		}
		catch (const std::exception& e) {
			//std::cout << e.what();
		}
	}

	CairnAPI m_Cairn2(false); // flag active or not console logs
	CairnAPI::OptimProblemAPI m_Problem2 = m_Cairn2.create_Study(StudyRoot + Study + "2.json");
	m_Problem2.initialize();
	std::vector<std::string> vLogs2 = TestUtils::readCSV(StudyRoot + Study + "2.log");
	TESTAPIBOOL("test " + Study + "2.log", (vLogs2.size() > 0))		
	TESTAPIBOOL("test " + Study + "2.log, verify " + level + "exist", (vLogs2[0].find(level + "]") != std::string::npos))


	// ----- Configure log level to "info" and verify that "debug" doesn't appear in the log ----- //
	t_dict vDefLogs = { {"level", "info"} };
	CairnAPI m_Cairn3(vDefLogs); // paramètres
	CairnAPI::OptimProblemAPI m_Problem3 = m_Cairn3.create_Study(StudyRoot + Study + "3.json");
	m_Problem3.initialize();
	std::vector<std::string> vLogs3 = TestUtils::readCSV(StudyRoot + Study + "3.log");
	TESTAPIBOOL("test " + Study + "3.log", (vLogs3.size() > 0))
	TESTAPIBOOL("test " + Study + "3.log, verify no debug", (vLogs3[0].find("debug]") == std::string::npos))


	// ----- Configure log level to "debug" and verify that "debug" appears in the log ----- //
	vDefLogs = { {"level", "debug"} };
	CairnAPI m_Cairn4(vDefLogs); // paramètres
	CairnAPI::OptimProblemAPI m_Problem4 = m_Cairn4.create_Study(StudyRoot + Study + "3.json");
	m_Problem4.initialize();
	std::vector<std::string> vLogs4 = TestUtils::readCSV(StudyRoot + Study + "3.log");
	TESTAPIBOOL("test " + Study + "3.log", (vLogs4.size() > 0))
	TESTAPIBOOL("test " + Study + "3.log, verify debug exist", (vLogs4[0].find("debug]") != std::string::npos))

	return noError;
}