#include "CairnAPI.h"
#include "CairnAPIUtils.h"
#include "CairnCore.h"
#include "Cairn_Exception.h"
#include "MIPSolverFactory.h"
#include "GlobalSettings.h"
#include <filesystem>
namespace fs = std::filesystem;

CairnAPI::CairnAPI()
{	
	CairnLogger::CreateLogger();

	//create a map of model types	
	CairnAPIUtils::initModelTypesMap();
}

CairnAPI::CairnAPI(bool a_Log)
{	
	CairnLogger::CreateLogger(a_Log);

	//create a map of model types	
	CairnAPIUtils::initModelTypesMap();
}

CairnAPI::CairnAPI(t_dict a_DefLogs)
{
	CairnLogger::CreateLogger(a_DefLogs);

	//create a map of model types	
	CairnAPIUtils::initModelTypesMap();
}

CairnAPI::~CairnAPI()
{
	close_Study();
}

//------------ Get Lists ------------------------
t_list CairnAPI::get_PossibleModelNames()
{
	return CairnAPIUtils::get_Possible_Model_Names();
}

t_list CairnAPI::get_PossibleComponentTypes()
{
	return CairnAPIUtils::get_Possible_Component_Types();
}

std::string CairnAPI::get_ComponentType(const std::string& a_Model)
{
	t_list possibleNames = get_PossibleModelNames();
	if (std::find(possibleNames.begin(), possibleNames.end(), a_Model) == possibleNames.end()) {
		return "Invalid: the model " + a_Model + " is not available!";
	}
	return CairnAPIUtils::get_Component_Type(a_Model);
}

t_list CairnAPI::get_TechnoTypes(const std::string& a_ComponentCategory)
{
	// dynamics list ?
	return { a_ComponentCategory };
}

t_list CairnAPI::get_Models(const std::string& a_TechnoType)
{
	t_list vRet;
	return vRet;
	// TODO
	// Composants avec des constructeurs simplifés	
	CairnCore* vCairn = new CairnCore("Cairn", "__CairnAPI");
	if (vCairn) {
		const std::string compoName = "__Component";

		t_mapParamData vComponentDescrp = CairnUtils::buildParamMap({
				{"type", a_TechnoType},
				{"ListPorts", "Port0"}
			});

		if (vCairn->getProblem()->createMilpComponent(compoName, a_TechnoType, vComponentDescrp, {})) {// {} is a nest map of ports list
			MilpComponent* vComp = vCairn->getComponent(compoName);
			if (vComp) {
				// dynamics list ?
				vRet = vComp->get_ModelClassList();
			}
		}
		delete vCairn;
	}
	return vRet;
}

t_list CairnAPI::get_EnergyCarrierTypes()
{
	// dynamics list ?
	t_list vRet = { "Electrical", "Thermal", "FluidH2", "FluidCH4", "Fluid", "Material" };
	return vRet;
}

t_list CairnAPI::get_ModelAttributs(const std::string& a_ModelClass, const std::string& a_SettingsType, ESettingsLimited a_setLimited)
{
	t_list vRet;
	return vRet;

	//TODO	
	CairnCore* vCairn = new CairnCore("Cairn", "__CairnAPI");
	if (vCairn) {
		const std::string compoName = "__Component";
		const std::string compoType; //TODO

		t_mapParamData vComponentDescrp = CairnUtils::buildParamMap({
			{"type",  compoType},
			{"ModelType",  a_ModelClass},
			{"ModelClass", a_ModelClass},
			{"ListPorts",  "Port0"}
		});

		if (vCairn->getProblem()->createMilpComponent(compoName, compoType, vComponentDescrp, {} /*a nest map of ports list*/))
		{
			MilpComponent* vComp = vCairn->getComponent(compoName);
			if (vComp) {
				int ierr = vComp->initProblem();
			}
		}
		delete vCairn;
	}
	return t_list();
}

t_value CairnAPI::get_DefaultParameter(const std::string& a_ModelClass, const std::string& a_attributeName)
{
	// TO DO, ajouter le paramètre par défaut dans 'addParameter'
	return t_value();
}

t_list CairnAPI::get_Solvers() const
{
	t_list vRet;
	MIPSolverFactory vSolvers;	
	vSolvers.getAllInfos(vRet);
	
	return vRet;
}

//------------- Create Study ------------------------------------------
CairnAPI::OptimProblemAPI CairnAPI::create_Study(const std::string& a_StudyName)
{	
	cDebug() << "Creating a study...";
	OptimProblemAPI vRet;
	if (m_Cairn) 
		CairnAPIUtils::setError(CairnAPIUtils::errDefault, "Study already exist");
	else {		
		std::string vQStudyName(a_StudyName.c_str());
	
		fs::path vLogFile(a_StudyName);
		vLogFile.replace_extension("log");

		// delete log file if it already exists ---
		//if (fs::exists(vLogFile)) {
		//	try {
		//		fs::remove(vLogFile);
		//	}
		//	catch (const std::exception& e) {
		//		cWarning() << "Could not delete existing log file: " << e.what();
		//		// Not fatal, continue
		//	}
		//}

		CairnLogger::ChangeFileLogger(vLogFile.string());
		m_Cairn = new CairnCore("Cairn", vQStudyName);
		
		vRet.set_Problem(m_Cairn->getProblem());
	}	
	return vRet;
}

CairnAPI::OptimProblemAPI CairnAPI::read_Study(const std::string& a_filename)
{
	OptimProblemAPI vRet;
	fs::path vPath(a_filename);
	if (fs::exists(vPath)) {	
		vRet = create_Study(a_filename);		
		try
		{
			cInfo() << "============================================================================";
			cInfo() << "Reading study file by the api: " << a_filename;
			cInfo() << "============================================================================";

			m_Cairn->doInit();
		}
		catch (Cairn_Exception& error)
		{
			CairnAPIUtils::setError(CairnAPIUtils::errRead, "study" + a_filename + ".\n\n" + error.message());
		}
	}
	else
		CairnAPIUtils::setError(CairnAPIUtils::errFile, a_filename);
	return vRet;
}

void CairnAPI::close_Study()
{
	if (m_Cairn) {
		delete m_Cairn;
		m_Cairn = nullptr;
	}
}

CairnAPI::OptimProblemAPI CairnAPI::get_Study()
{
	OptimProblemAPI vRet;
	if (!m_Cairn)
		CairnAPIUtils::setError(CairnAPIUtils::errDefault, "No study exists!");
	else {
		vRet.set_Problem(m_Cairn->getProblem());
	}
	return vRet;
}

CairnAPI::OptimProblemAPI CairnAPI::apply_Compatibility_Script()
{
	if (!m_Cairn) 
		CairnAPIUtils::setError(CairnAPIUtils::errDefault, "No study exists! Read a study first!");

	const std::string scriptsDir =
		std::string(std::getenv("CAIRN_BIN")) + "/../resources/version_compatibility";

	const std::string studyVersion = m_Cairn->studyVersion();
	const auto fromVersion = CairnUtils::parseVersion(studyVersion);   // { major, minor, patch }

	const std::string currentRaw = CairnUtils::extractVersion(GS::Cairn_Release);
	const auto toVersion = CairnUtils::parseVersion(currentRaw);

	std::vector<std::string> scripts;

	// --- Scan directory -------------------------------------------------------
	for (const auto& entry : std::filesystem::directory_iterator(scriptsDir))
	{
		if (!entry.is_regular_file())
			continue;

		// Only accept .py files
		const std::filesystem::path p = entry.path();
		if (p.extension() != ".py")
			continue;

		const std::string scriptname = entry.path().filename().string();

		auto range = CairnAPIUtils::extractVersionRange(scriptname);
		if (!range.has_value())
			continue;

		const auto& [vStart, vEnd] = *range;

		// --- Check if script applies -----------------------------------------
		const std::pair<int, int> fromMajorMinor = CairnAPIUtils::versionToMajorMinor(fromVersion);
		const std::pair<int, int> toMajorMinor   = CairnAPIUtils::versionToMajorMinor(toVersion);

		if (CairnAPIUtils::versionRangeIntersects(fromMajorMinor, toMajorMinor, vStart, vEnd))
			scripts.push_back(entry.path().string());
	}

	// --- Sort scripts by start version (deterministic order) ------------------
	std::sort(scripts.begin(), scripts.end(),
		[&](const std::string& a, const std::string& b)
		{
			auto ra = CairnAPIUtils::extractVersionRange(std::filesystem::path(a).filename().string());
			auto rb = CairnAPIUtils::extractVersionRange(std::filesystem::path(b).filename().string());
			return ra->first < rb->first;   // compare start version
		});

	// --- Execute scripts ------------------------------------------------------
	const std::string archFile = m_Cairn->archFile();
	for (const auto& script : scripts)
	{
		const std::string cmd =
			"python \"" + script + "\" \"" + archFile + "\"";

		int ret = std::system(cmd.c_str());
		if (ret != 0)
			cWarning() << "Compatibility script failed: " + script;
	}

	// --- Update Json version number --------------------------------------------
	const std::string script = scriptsDir + "/update_version_number.py";
	const std::string cmd =
		"python \"" + script + "\" \"" + archFile + "\" \"" + currentRaw + "\"";

	const int ret = std::system(cmd.c_str());
	if (ret != 0)
		cWarning() << "update_version_number script failed: "
			<< script << " (exit code " << ret << ")";

	// If script fail, the number of version might not updated => infinite loop
	OptimProblem* problem = m_Cairn->getProblem();
	if(problem)
		problem->setStudyVersionMatchesCairn(true);

	// Re-load study
	close_Study();
	return read_Study(archFile);
}
