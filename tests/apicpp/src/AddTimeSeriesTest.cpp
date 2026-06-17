#include "TEST_CairnCore.h"
#include <iostream>
#include "StudyCTest.h"
#include "UtilsJson.h"

using namespace std;

/* Test l'ajout de TimeSeries sous forme de fichier ou de dictionnaire
*  Utilise le projet formation cairn
*/
int testNoFileTS(StudyTest& a_Test, CairnAPI& a_Cairn, const std::string& a_SolverName)
{
	CairnAPI::OptimProblemAPI m_Problem = a_Cairn.get_Study();

	std::vector<std::string> vTimeSeries = TestUtils::readCSV(a_Test.get_TimeseriesFileName());
	std::vector<std::string> vNames = TestUtils::parseLineCSV(vTimeSeries[0]);
	std::vector<std::string> vUnits = TestUtils::parseLineCSV(vTimeSeries[2]);
	std::vector < std::vector<std::string>> vStrValues;
	for (size_t j = 4; j < vTimeSeries.size(); j++) {
		std::vector<std::string> vLineValues = TestUtils::parseLineCSV(vTimeSeries[j]);
		vStrValues.push_back(vLineValues);

	}
	for (size_t i = 0; i < vNames.size(); i++) {
		t_dict vTS = { {"Name", vNames[i]}, { "Unit", vUnits[i] } };
		std::vector<double> vValues;
		for (auto& vStrLineValues : vStrValues) {
			vValues.push_back(std::stod(vStrLineValues[i]));
		}
		vTS["Values"] = vValues;

		m_Problem.add_TimeSeries(vTS);

	}
	m_Problem.run(a_SolverName);
	return noError;
}

int testMixteFileTS(StudyTest& a_Test, CairnAPI& a_Cairn, const std::string& a_SolverName)
{
	CairnAPI::OptimProblemAPI m_Problem = a_Cairn.get_Study();

	std::vector<std::string> vTimeSeries = TestUtils::readCSV(a_Test.get_TimeseriesFileName());
	std::vector<std::string> vNames = TestUtils::parseLineCSV(vTimeSeries[0]);
	std::vector<std::string> vUnits = TestUtils::parseLineCSV(vTimeSeries[2]);
	std::vector < std::vector<std::string>> vStrValues;
	for (size_t j = 4; j < vTimeSeries.size(); j++) {
		std::vector<std::string> vLineValues = TestUtils::parseLineCSV(vTimeSeries[j]);
		vStrValues.push_back(vLineValues);
	}
	t_dict vTS = { {"Name", vNames[4]},  { "Unit", vUnits[4] } };
	std::vector<double> vValues;
	std::vector<int> vTimes;
	for (auto& vStrLineValues : vStrValues) {
		vValues.push_back(std::stod(vStrLineValues[4]));
		vTimes.push_back(std::stoi(vStrLineValues[0]));
	}
	vTS["Values"] = vValues;
	vTS["Times"] = vTimes;
	CairnAPI::OptimProblemAPI m_Problem2 = a_Cairn.get_Study();
	m_Problem2.add_TimeSeries(vTS);

	TESTAPI("Read the Timeseries from the file path: " + a_Test.get_ExtraFileName(0),
		m_Problem2.add_TimeSeries(a_Test.get_ExtraFileName(0))
	)

	m_Problem.run(a_SolverName);
	return noError;
}

int main()
{	
	StudyCTest vTest("formation_cairn", "addTimeSeries", { "dataseries_noWindFarmProduction.csv"});
	CairnAPI m_Cairn;
	// Test with solver Cplex, if not exist test with solver Highs
	int vRet = vTest.readStudyChangeSolver(m_Cairn, "Cplex");
	if (vRet != noError && vRet != errType) return vRet;
		
	vRet = vTest.runAndcheck(m_Cairn, "", testNoFileTS);
	if (vRet != noError) return vRet;

	m_Cairn.close_Study();

	// mixte TS file and TS dict
	CairnAPI m_Cairn2;
	vRet = vTest.readStudyChangeSolver(m_Cairn2, "Cplex");
	if (vRet != noError && vRet != errType) return vRet;
	
	vRet = vTest.runAndcheck(m_Cairn2, vTest.get_ExtraFileName(0), testMixteFileTS);
	if (vRet != noError) return vRet;


	return noError;
}