#include "TEST_CairnCore.h"
#include <iostream>
#include "StudyCTest.h"


using namespace std;

/* 
		
This test reads study formation_cairn_err.json 
	The solver type is Cplex2, so the 'read study' method should return an error
*/

int main()
{
	CairnAPI m_Cairn;
	CairnAPI::OptimProblemAPI m_Problem;

	string const Study = "formation_cairn_err";
	string const StudyRoot = TEST_RESULTS + (std::string)"/solverErr/";

	std::string vFileName = StudyRoot + Study + ".json";
	std::string vTimeseriesFile = StudyRoot + "formation_cairn_dataseries.csv";

	if (fs::exists(StudyRoot)) {
		fs::remove_all(StudyRoot);
	}
	if (!fs::exists(TEST_RESULTS)) {
		fs::create_directory(TEST_RESULTS);
	}
	fs::create_directory(StudyRoot);
	fs::copy_file(TEST_DATA + (std::string)"/" + Study + ".json", vFileName);
	fs::copy_file(TEST_DATA + (std::string)"/" + "formation_cairn_dataseries.csv", vTimeseriesFile);

	TESTAPI("Read study: " + vFileName, 
		m_Problem = m_Cairn.read_Study(vFileName)
	)

	TESTAPI("Read the Timeseries: " + vTimeseriesFile,
		m_Problem.add_TimeSeries(vTimeseriesFile)
	)

	TESTAPIFALSE("Run: ",
		m_Problem.run()
	)

	return noError;
}