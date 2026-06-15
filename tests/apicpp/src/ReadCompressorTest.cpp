#include "TEST_CairnCore.h"
#include <iostream>
#include "StudyCTest.h"
#include "UtilsJson.h"

using namespace std;

/* This test reads study test_compressor.json and test_compressor_dataseries.csv
   
   Then, it executes a simulation and compares the result to the reference test_compressor_Results_Reference.csv
   which is generated using the GUI.
*/

int main()
{
	CairnAPI m_Cairn;
	StudyCTest vTest("", "");
	std::string vSolverType = vTest.TrySolver(m_Cairn, "Cplex");
	if (vSolverType == "Highs") return noError; // No test if solver is Highs

	CairnAPI::OptimProblemAPI m_Problem;

	string const Study = "test_compressor";
	string const StudyRoot = TEST_RESULTS + (std::string)"/readCompressor/";



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

	string const ReferenceResultFileName = TEST_DATA + (std::string)"/" + Study+ (std::string)"_results_Reference.csv";
	

	TESTAPI("read study file from the file path: " + vFileName,
		m_Problem = m_Cairn.read_Study(vFileName)
	)
			
	TESTAPI("Read the Timeseries from the file path: " + TimeseriesFileName,
		m_Problem.add_TimeSeries(TimeseriesFileName)
	)

	CairnAPI::SolutionAPI vSolution;

	TESTAPI("Run 1",
		vSolution = m_Problem.run()
	)

	TESTAPI2("Compare results",
		TestUtils::ComparaisonCsvFile(ResultFileName, ReferenceResultFileName)
	)
	
	return noError;
}