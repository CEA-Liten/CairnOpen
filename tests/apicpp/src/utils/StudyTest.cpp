#include "StudyTest.h"



StudyTest::StudyTest(const std::string& a_Study, const fs::path& a_PathStudy)
	: mStudy(a_Study)
{
	mStudyPath = a_PathStudy;
	mResPrefixFile = (mStudyPath / fs::path(mStudy)).string();
	mSrcPrefixFile = mResPrefixFile;
	mFileName = mResPrefixFile + ".json";
	mTimeseriesFileName = mResPrefixFile + "_dataseries.csv";
}

std::string StudyTest::prepareCheckLP(CairnAPI& a_Cairn)
{
	std::string vStatus = "";
	CairnAPI::OptimProblemAPI vProblem = a_Cairn.get_Study();
	std::shared_ptr < CairnAPI::SimulationControlAPI> vSim = vProblem.get_SimulationControl();
	t_dict vValues = vSim->get_SettingValues();
	
	vSim->set_SettingValues({
		{"PastSize", 1},
		{"NbCycle", 1},
		{"TimeShift", 1},
		{"FutureSize", 10}
		}
	);

	CairnAPI::SolutionAPI vSolution;
	try
	{
		vSolution = vProblem.run("checklp");
		vStatus = vSolution.get_Status();
	}
	catch (const std::exception&e)
	{
		vStatus = "";
	}	
	vSim->set_SettingValues(vValues);

	return vStatus;
}

bool StudyTest::runSensitivity(CairnAPI& a_Cairn, const std::string& a_Sampling, int a_max_time, const std::string& a_kpi)
{
	bool vRet = true;
	fs::path vSamplingFile = mStudyPath / std::string(a_Sampling + ".csv");
	if (fs::exists(vSamplingFile)) {
		CairnAPI::OptimProblemAPI vProblem = a_Cairn.get_Study();
		fs::path vKpiFile = "";
		if (a_kpi != "") {
			vKpiFile = mStudyPath / std::string(a_kpi + ".csv");
			if (!fs::exists(vKpiFile)) {
				vKpiFile = "";
			}
		}
		vProblem.runSensitivityCSV(vSamplingFile.string(), a_max_time, vKpiFile.string());

		// vérification des résultats
		t_mapDict vRef = readCSV(get_StudyPath() / std::string("sampling_results_ref.csv"));
		t_mapDict vRes = readCSV(get_StudyPath() / std::string("sampling_results.csv"));
		
		std::vector<std::string> vErr;
		for (auto& [vCaseName, vCase] : vRef) {
			t_mapDict::iterator vIter = vRes.find(vCaseName);
			if (vIter != vRes.end()) {
				t_dict& vResCase = vIter->second;
				for (auto& [vKpi, vValue] : vCase) {
					t_dict::iterator vIter2 = vResCase.find(vKpi);
					if (vIter2 != vResCase.end()) {
						if (std::holds_alternative<double>(vValue) && std::holds_alternative<double>(vIter2->second)) {							
							double diff = getDiff(std::get<double>(vValue), std::get<double>(vIter2->second));
							if (diff > 0.001) {
								vErr.push_back("Case " + vCaseName + ", kpi " + vKpi + ", difference: " + std::to_string(diff));
							}
						}						
						else
							vErr.push_back("Case " + vCaseName + ", kpi " + vKpi + ", bad format");
					}
					else {
						vErr.push_back("Case " + vCaseName + ", no kpi " + vKpi + " in sampling results");
					}
				}
			}
			else {
				vErr.push_back("No case " + vCaseName + " in sampling results");
			}
		}		
		writeResultSampling(vErr);
		vRet = (vErr.size()==0);
	}	
	return vRet;
}

void StudyTest::writeResult(const std::map<std::string, std::string>& a_Msgs)
{
	fs::path vResults = mStudyPath / std::string(mStudy + "_checkResults.txt");
	if (fs::exists(vResults)) {
		std::ofstream vFile;
		vFile.open(vResults.string(), std::ios_base::app);
		for (auto& [k, v] : a_Msgs) {
			vFile << k << ";" << v << std::endl;
		}		
	}
}

StudyTest::t_mapDict StudyTest::readCSV(const fs::path& a_filename)
{
	t_mapDict vRet;
	if (fs::exists(a_filename)) {
		std::ifstream vFile;
		vFile.open(a_filename.string(), std::ios_base::in);		
		std::vector<std::string> vHeaders;

		for (auto& row : CSVRange(vFile)) {
			if (!vHeaders.size()) {
				for (size_t i = 0; i < row.size(); i++) {
					if (row(i) != "")
						vHeaders.push_back(row(i));
				}
			}
			else {
				if (row.size() > 2) {
					t_mapDict::iterator vIter = vRet.find(row(1));
					if (vIter == vRet.end()) {
						vRet[row(1)] = {};
						vIter = vRet.find(row(1));
					}
					t_dict& vCase = vIter->second;
					for (size_t i = 2; i < row.size(); i++) {
						const std::string& vKpi = vHeaders[i - 1];
						t_dict::iterator vIter2 = vCase.find(vKpi);
						if (vIter2 == vIter->second.end()) {
							vCase[vKpi] = {};
							vIter2 = vCase.find(vKpi);
						}
						vIter2->second = row[i];
					}
				}
			}
		}
	}	
	return vRet;
}

double StudyTest::getDiff(double a_Value1, double a_Value2)
{
	double diff = std::fabs(a_Value1 - a_Value2);
	if (a_Value1)
		diff /= std::fabs(a_Value1);
	else if (a_Value2)
		diff /= std::fabs(a_Value2);

	return diff;
}

void StudyTest::writeResultSampling(const std::vector<std::string>& a_Msgs)
{
	fs::path vResults = mStudyPath / std::string(mStudy + "_checkResults.txt");
	if (fs::exists(vResults)) {
		std::ofstream vFile;
		vFile.open(vResults.string(), std::ios_base::app);
		vFile << "SAMPLING";
		if (a_Msgs.size()) {
			vFile << ";Failed";
			for (auto& vMsg : a_Msgs)
				vFile << ";" << vMsg;
		}
		else {
			vFile << ";Success";
		}
		vFile << std::endl;
	}
}

const fs::path StudyTest::makeFileName(const std::string& a_Type, const fs::path& a_Path, const std::string& a_Suffix)
{
	fs::path vRet;
	std::string ext = "csv";
	std::string vName;
	std::string vSuffix;
	if (a_Type == "Results") {		
		if (a_Suffix == "")
			vName = mStudy + "_results_" + a_Type;
		else
			vName = mStudy + "_" + a_Type;
	}
	else if (a_Type == "LP") {
		vName = mStudy + "_model";
		ext = "lp";
	}
	else if (a_Type == "sampling") {
		vName = a_Type + "_results";
	}
	else {
		vName = mStudy + "_results_" + a_Type;
	}
	if (a_Suffix != "")
		vName += "_";
	vName += a_Suffix + "." + ext;
		
	if (a_Path != "") {
		vRet = a_Path / vName;
	}
	else {
		vRet = mStudyPath / vName;
	}			
	return vRet;
}

bool StudyTest::updateFile(const fs::path& a_Src, const fs::path& a_Dest)
{
	bool vRet = false;
	if (!fs::exists(a_Dest) && fs::exists(a_Src)) {
		fs::rename(a_Src, a_Dest);
		vRet = true;
	}
	else if (fs::exists(a_Src)) {
		fs::remove(a_Dest);
		fs::rename(a_Src, a_Dest);
		vRet = true;
	}
	return vRet;
}

bool StudyTest::checkResults(const std::string& a_RefName, 
	bool a_CheckHist, bool a_CheckPlan, bool a_CheckLP, bool a_isRollingHorizon,
	const std::string& a_Scenario, const std::string& a_Study)
{
	// appel d'un script python pour établir les comparaisons
	// 
	// checkResults.py
	// arguments:
	// obligatoire
	//	 <pathTest>: chemin complet du test 
	//	 <nameTest>: nom du test 
	// option
	//	 <pathRef>: chemin complet de la référence (si différent de <pathTest>)
	// 	 <nameRef>: nom de la référence (si différent de Ref)
	//   <checkPLAN>
	//   <checkHIST>
	//   <checkLP>
	// 	
	// Le script vérifie:
	//	les résultats temporels:
	//		<pathTest>/<nameTest>_results_Results.csv et <pathRef>/<nameTest>_Results_<nameRef>.csv
	//	le PLAN
	//		<pathTest>/<nameTest>_results_PLAN.csv et <pathRef>/<nameTest>_results_PLAN_<nameRef>.csv
	//  le HIST
	//		<pathTest>/<nameTest>_results_HIST.csv et <pathRef>/<nameTest>_results_HIST_<nameRef>.csv
	// 
	// récupère les résultats
	bool vRet = false;
	fs::path vStudyPath = mStudyPath;
	std::string vStudy = mStudy;
	if (a_Study != "") vStudy = a_Study;
	if (a_Scenario != "") vStudyPath = mStudyPath / a_Scenario;
	std::vector<std::string> vCmds = { PYTHON_CMD_PROJECT,
						TEST_SCRIPTS + std::string("/checkResults.py"),
						vStudyPath.string(),
						vStudy
	};
	fs::path vRefPath(get_RefPath());
	if (a_isRollingHorizon) {
		vCmds.push_back(makeFileName("Results", vRefPath, "rh_" + a_RefName).string());
	}
	else {
		vCmds.push_back(makeFileName("Results", vRefPath, a_RefName).string());		
	}
	vCmds.push_back(a_CheckPlan ? makeFileName("PLAN", vRefPath, a_RefName).string() : "0");
	vCmds.push_back(a_CheckHist ? makeFileName("HIST", vRefPath, a_RefName).string() : "0");
	vCmds.push_back(a_CheckLP ? makeFileName("LP", vRefPath, a_RefName).string() : "0");
	if (a_isRollingHorizon) {
		vCmds.push_back(makeFileName("rollinghorizon").string());
	}


	std::string vCmd = "";
	for (auto& vElem : vCmds) {
		vCmd += vElem + " ";
	}
	if (!system(vCmd.c_str())) {
		// le script de vérification a bien été exécuté		
		// récupération des résultats
		fs::path vResults = vStudyPath / std::string(vStudy +  "_checkResults.txt");
		std::vector<std::string> vRes = TestUtils::readCSV(vResults.string());
		vRet = (vRes.size()!=0);
		for (auto& vLine : vRes) {
			std::vector<std::string> vValues = TestUtils::parseLineCSV(vLine);
			if (vValues.size() == 2) {
				cout << vValues[0] << "=" << vValues[1] << std::endl;
				if (vValues[0] != "LPFILE" && vValues[1] != "True") {
					vRet = false;
					break;
				}
			}			
		}
	}
	cout << "checkResults= " << vRet << std::endl;
	return vRet;
}

void StudyTest::updateResults(const std::string& a_RefName, bool a_isRollingHorizon)
{
	fs::path vRefPath(get_RefPath());
	
	updateFile(makeFileName("PLAN"), makeFileName("PLAN", vRefPath, a_RefName));
	updateFile(makeFileName("HIST"), makeFileName("HIST", vRefPath, a_RefName));
	updateFile(makeFileName("LP", mStudyPath/"checklp"), makeFileName("LP", vRefPath, a_RefName));

	if (a_isRollingHorizon) {
		updateFile(makeFileName("rollinghorizon"), makeFileName("Results", vRefPath, "rh_" + a_RefName));
	}
	else {
		updateFile(makeFileName(), makeFileName("Results", vRefPath, a_RefName));
	}
	updateFile(makeFileName("sampling"), makeFileName("sampling", vRefPath, "ref"));
}