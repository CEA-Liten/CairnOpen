#pragma once
#include "StudyTest.h"

typedef int (*runTest)(StudyTest &a_Test, CairnAPI& a_Cairn, const std::string& a_SolverName);

class StudyCTest : public StudyTest
{
public:
	// Ctest
	StudyCTest(const std::string& a_Study,
		const std::string& a_ResPathStudy,
		const std::vector<std::string>& a_ExtraFiles = {}, const std::string& a_SuffixStudy = "");

	int readStudyChangeSolver(CairnAPI& a_Cairn, const std::string& a_SolverName, bool a_alreadyRead = false, const std::string& a_TimeSeries = "");
	std::string TrySolver(CairnAPI& a_Cairn, const std::string& a_SolverName);
	int runAndcheck(CairnAPI& a_Cairn, const std::string& a_TimeSeries = "", runTest ap_funcRun = nullptr);

	virtual std::string get_RefPath() { return mRefPath.string(); }
	void set_RefPath(const fs::path& a_Path) { mRefPath = a_Path; }
protected:
	std::string mOrgStudy;
	fs::path mOrgStudyPath;
	fs::path mRefPath;
	bool m_TestCplexHighs{ true };
	int runAndcheckSimple(CairnAPI& a_Cairn, const std::string& a_SolverName, runTest ap_funcRun);
};