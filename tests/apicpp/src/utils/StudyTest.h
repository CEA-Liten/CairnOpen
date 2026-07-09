#pragma once
#include "Utils.h"

class StudyTest
{
public:
	StudyTest(const std::string& a_Study,
		const fs::path& a_PathStudy);
	
	const std::string& get_FileName() { return mFileName; }
	const std::string& get_TimeseriesFileName() { return mTimeseriesFileName; }
	std::string get_ExtraFileName(size_t a_Idx) { return ((a_Idx < mExtraFiles.size()) ? mExtraFiles[a_Idx] : ""); }
	const std::string& get_Study() { return mStudy; }
	const fs::path& get_StudyPath() { return mStudyPath; }
	const std::string& get_ResPrefixFile() { return mResPrefixFile; }
	const std::string& get_SrcPrefixFile() { return mSrcPrefixFile; }
	virtual std::string get_RefPath() { return mStudyPath.string(); }

	bool checkResults(const std::string &a_RefName = "", 
		bool a_CheckHist = false, bool a_CheckPlan = false, bool a_CheckLP = false,
		bool a_isRollingHorizon = false, const std::string &a_Scenario="", const std::string& a_Study = "");
	
	std::string prepareCheckLP(CairnAPI& a_Cairn);
	bool runSensitivity(CairnAPI& a_Cairn, const std::string& a_Sampling = "sampling", int a_max_time = -1, const std::string& a_kpi = "kpi_sampling");

	void writeResult(const std::map<std::string, std::string>& a_Msgs);

	void updateResults(const std::string& a_RefName = "", bool a_isRollingHorizon = false);

protected:
	std::string mStudy;	
	std::vector<std::string> mExtraFiles;

	std::string mFileName;
	std::string mTimeseriesFileName;
	std::string mResPrefixFile;
	std::string mSrcPrefixFile;
	fs::path mStudyPath;
	virtual void initPath(const std::string& a_PathStudy) {};

	typedef std::map<std::string, t_dict> t_mapDict;
	t_mapDict readCSV(const fs::path& a_filename);
	double getDiff(double a_Value1, double a_Value2);
	void writeResultSampling(const std::vector<std::string> &a_Msgs);

	const fs::path makeFileName(const std::string& a_Type = "Results", const fs::path& a_Path = "", const std::string& a_Suffix = "");
	bool updateFile(const fs::path& a_Src, const fs::path& a_Dest);
};
