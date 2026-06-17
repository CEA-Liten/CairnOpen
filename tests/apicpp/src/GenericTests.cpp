#include "TEST_CairnCore.h"
#include "StudyTest.h"
#include "UtilsJson.h"
#include "CairnAPIUtils.h"



// Lancement et vérification d'un seul cas test
// ce test peut contenir un fichier sampling permettant de lancer une étude sensibilité
// a_casepath : chemin complet du cas à tester
// a_case : nom du cas (nom du fichier json sans l'extension .json)
// a_update: si vrai, copie le résultat après simulation dans la référence
int testCase(const fs::path& a_casepath, const fs::path& a_case, bool a_update)
{
	bool a_checkLP = true; // TODO : parametre?
	int vRet = noError;
	StudyTest vTest(a_case.string(), a_casepath);

	CairnAPI m_Cairn;
	CairnAPI::OptimProblemAPI m_Problem;
	TESTAPI("read study file from the file path: " + vTest.get_FileName(),
		m_Problem = m_Cairn.read_Study(vTest.get_FileName())
	)
	TESTAPI("Read the Timeseries from the file path: " + vTest.get_TimeseriesFileName(),
		m_Problem.add_TimeSeries(vTest.get_TimeseriesFileName())
	)

	bool isRollingHorizon = false;
	std::shared_ptr<CairnAPI::SimulationControlAPI> simulation_control = m_Problem.get_SimulationControl();
	std::string vNbCycle = CairnAPIUtils::getParamValue(simulation_control->get_SettingValue("NbCycle"));
	if (std::stoi(vNbCycle) > 1) {
		isRollingHorizon = true;
	}

	CairnAPI::SolutionAPI vSolution;
	TESTAPI("Run",
		vSolution = m_Problem.run()
	)
	std::string vStatusLP = "";
	if (a_checkLP) {
		vStatusLP = vTest.prepareCheckLP(m_Cairn);
	}

	bool vCheckResults = true;
	if (!a_update) {		
		// vérificaiton des résultats
		bool vCheckLP = a_checkLP;
		if (vStatusLP == "")
			vCheckLP = false;

		vCheckResults = vTest.checkResults("Ref", true, true, vCheckLP, isRollingHorizon);

		vTest.writeResult({
			{"OPTIM", vSolution.get_Status()},
			{"RUNLPFILE", vStatusLP}
			}
		);

	}
	
	// =============== Sampling ===============================
	bool vCheckSampling = vTest.runSensitivity(m_Cairn);
	
	// update
	if (a_update) {
		// mise à jour de la réfarence, pas de vérification
		vTest.updateResults("Ref", isRollingHorizon);
	}

	return (vCheckResults&&vCheckSampling)? 0 : 1;
}

// Lancement et vérification de tous les tests contenus dans le répertoire a_testpath
// Un test correspond à un fichier json de projet 
// Un test est un sous-répertoire commencant par TEST_FUNCPREFIX et contenant un fichier projet json
int test(const fs::path& a_testpath, bool a_update)
{
	int vRet = noError;

	for (auto const& f : fs::directory_iterator{ a_testpath }) {
		// loop on json files
		if (!f.is_directory()) {
			// un fichier json = un test
			if (f.path().extension() == ".json") {
				int vTest = testCase(a_testpath, f.path().stem(), a_update);
				if (vTest != noError) vRet = vTest;
			}
		}
		else {
			std::string vPath = f.path().stem().string();
			// un répertoire commencant par TEST_FUNCPREFIX = un test
			if (starts_with(f.path().stem().string(), TEST_FUNCPREFIX)) {
				int vTest = test(f.path(), a_update);
				if (vTest != noError) vRet = vTest;
			}
		}
	}
	
	return vRet;
}

int tests(const fs::path& a_rootpath, bool a_update)
{
	int vRet = noError;
	
	// loop on directories
	for (auto const& dir_entry : fs::directory_iterator{ a_rootpath }) {
		if (dir_entry.is_directory()) {
			int vTest = test(dir_entry, a_update);
			if (vTest != noError) vRet = vTest;
		}
	}

	return vRet;
}

void getAllPaths(std::vector < std::string >& aPaths)
{
	aPaths = {
		"/../../models/",
#ifdef PRIVATE_MODELS
		"/../../privateTests/models/",
		"/../../privateTests/integration/",
#endif
		"/../../integration/"
	};
}


int main(int argc, char* argv[])
{
	int vRet = noError;
	std::vector < std::string > vPaths;
	std::vector < std::string > vPathCases;
	std::vector < std::string > vCases;
	bool vUpdate = false;
	if (argc == 1) {
		getAllPaths(vPaths);
	}
	else {
		for (int i = 1; i < argc; i++) {			
			std::string s(argv[i]);
			std::cout << "arg: " << s << std::endl;
			if (s.rfind("--all", 0) == 0) {
				getAllPaths(vPaths);
			}
			else if (s.rfind("--paths:", 0) == 0) {
				vPaths.push_back(s.substr(8));
			}
			else if (s.rfind("--path:", 0) == 0) {
				vPathCases.push_back(s.substr(7));
			}
			else if (s.rfind("--case:", 0) == 0) {
				vCases.push_back(s.substr(7));
			}
			else if (s.rfind("--update", 0) == 0) {
				vUpdate = true;
			}
		}
	}

	for (auto& vPath : vPaths) {
		int vTest = tests(vPath, vUpdate);
		if (vTest != noError) vRet = vTest;
	}

	for (auto& vPath : vPathCases) {
		int vTest = test(vPath, vUpdate);
		if (vTest != noError) vRet = vTest;
	}

	for (auto& vCase : vCases) {
		fs::path vPath = vCase;
		int vTest = testCase(vPath.parent_path(), vPath.stem(), vUpdate);
		if (vTest != noError) vRet = vTest;
	}

	return vRet;
}


