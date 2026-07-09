#include "StudyCTest.h"


StudyCTest::StudyCTest(const std::string& a_Study, const std::string& a_ResPathStudy,
	const std::vector<std::string>& a_ExtraFiles, const std::string& a_SuffixStudy)
	: StudyTest(a_Study, a_ResPathStudy)
{
	mOrgStudy = a_Study;
	if (a_ResPathStudy == "") {
		mStudyPath = TEST_DATA;		
		mSrcPrefixFile = (mStudyPath / fs::path(mStudy)).string();
		mResPrefixFile = mSrcPrefixFile;
	}
	else if (a_Study != "" && a_ResPathStudy != "") {		
		mStudyPath = fs::path(TEST_RESULTS) / fs::path(a_ResPathStudy);
		mOrgStudyPath = mStudyPath;
		if (fs::exists(mStudyPath)) {
			fs::remove_all(mStudyPath);
		}
		if (!fs::exists(TEST_RESULTS)) {
			fs::create_directory(TEST_RESULTS);
		}
		fs::create_directory(mStudyPath);

		mResPrefixFile = (mStudyPath / fs::path(mStudy)).string();
		mSrcPrefixFile = (fs::path(TEST_DATA) / fs::path(mStudy)).string();

		std::vector<std::string> vCpyFiles = { a_SuffixStudy + ".json", a_SuffixStudy + "_highs.json", "_dataseries.csv" };
		std::vector<std::string> vDestFiles = { ".json", "_highs.json", "_dataseries.csv" };
		for (auto& vFile : a_ExtraFiles) {
			vCpyFiles.push_back("_" + vFile);
			vDestFiles.push_back("_" + vFile);
			mExtraFiles.push_back(mResPrefixFile + "_" + vFile);
		}
		
		for (size_t i = 0; i < vCpyFiles.size(); i++) {
			const std::string& vFile = vCpyFiles[i];
			const std::string& vDestFile = vDestFiles[i];
			if (fs::exists(mSrcPrefixFile + vFile)) {
				fs::copy_file(mSrcPrefixFile + vFile, mResPrefixFile + vDestFile);
			}
		}				
	}	
	mFileName = mResPrefixFile + ".json";
	mTimeseriesFileName = mResPrefixFile + "_dataseries.csv";
	mRefPath = TEST_DATA;
}

std::string toLower(const std::string& a_string) {
	std::string vTmp(a_string);
	std::transform(a_string.begin(), a_string.end(), vTmp.begin(), ::tolower);
	return vTmp;
}

int StudyCTest::readStudyChangeSolver(CairnAPI& a_Cairn, const std::string& a_SolverName, bool a_alreadyRead, const std::string& a_TimeSeries)
{
	int vRet = noError;
	t_list vSolvers = a_Cairn.get_Solvers();
	t_list::iterator vIter = find(vSolvers.begin(), vSolvers.end(), a_SolverName);
	if (vIter != vSolvers.end()) {
		// the solver exists, read the study
		CairnAPI::OptimProblemAPI vProblem;
		if (a_alreadyRead) {			
			std:string vName = toLower(a_SolverName);			
			mFileName = mResPrefixFile + "_" + vName + ".json";

			mStudyPath = mOrgStudyPath / a_SolverName;
			mStudy = mOrgStudy + "_" + vName;
			fs::create_directory(mStudyPath);
			a_Cairn.close_Study();
		}
		else {
			mFileName = mResPrefixFile + ".json";
			mStudyPath = mOrgStudyPath;
			mStudy = mOrgStudy;
		}
		TESTAPI("read study file from the file path: " + get_FileName(),
			vProblem = a_Cairn.read_Study(get_FileName())
		)

		if (a_alreadyRead) {
			std::string vTimeSeries = a_TimeSeries;
			if (vTimeSeries == "") vTimeSeries = get_TimeseriesFileName();
			TESTAPI("Read the Timeseries from the file path: " + vTimeSeries,
				vProblem.add_TimeSeries(vTimeSeries)
			)
		}
	}
	else {
		// solver does not exist, try with Highs
		mStudy = mOrgStudy + "_highs";
		mFileName = mResPrefixFile + "_highs.json";
		TESTAPI("read study file from the file path: " + get_FileName(),
			a_Cairn.read_Study(get_FileName())
		)		
		vRet = errType;
	}
	m_TestCplexHighs = (vRet == noError);
	return vRet;
}

std::string StudyCTest::TrySolver(CairnAPI& a_Cairn, const std::string& a_SolverName)
{
	std::string vRet = a_SolverName;
	t_list vSolvers = a_Cairn.get_Solvers();
	t_list::iterator vIter = find(vSolvers.begin(), vSolvers.end(), a_SolverName);
	if (vIter == vSolvers.end()) {		
		// solver does not exist, try with Highs
		mStudy = mOrgStudy + "_highs";
		vRet = "Highs";
	}
	return vRet;
}

int StudyCTest::runAndcheck(CairnAPI& a_Cairn, const std::string& a_TimeSeries, runTest ap_funcRun)
{
	int vRet = noError;
	if (m_TestCplexHighs) {
		// Test Cplex and Highs
		// Cplex : run/check
		vRet = runAndcheckSimple(a_Cairn, "", ap_funcRun);
		if (vRet) return vRet;

		// Change solver Cplex->Highs, use file a_TimeSeries if <>"" (else use file ends with _highs_dataseries.csv)
		vRet = readStudyChangeSolver(a_Cairn, "Highs", true, a_TimeSeries);
		if (vRet) return vRet;

		// Highs : run/check
		vRet = runAndcheckSimple(a_Cairn, "Highs", ap_funcRun);
	}
	else {
		// Test only Highs
		vRet = runAndcheckSimple(a_Cairn, "", ap_funcRun);
	}
	return vRet;
}

int StudyCTest::runAndcheckSimple(CairnAPI& a_Cairn, const std::string& a_SolverName, runTest ap_funcRun)
{
	if (ap_funcRun == nullptr) {
		CairnAPI::OptimProblemAPI vProblem = a_Cairn.get_Study();

		TESTAPI("Run",
			vProblem.run(a_SolverName)
		)
	}
	else {
		TESTAPI("Run",
			ap_funcRun(*this, a_Cairn, a_SolverName)
		)
	}

	TESTAPIBOOL("Check Run",
		checkResults("Reference", true, true)
	)

	return noError;
}
