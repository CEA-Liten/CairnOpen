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
		std::map<std::string, std::string> vComponentDescrp;
		vComponentDescrp["id"] = "__Component";
		vComponentDescrp["type"] = std::string(a_TechnoType.c_str());
		vComponentDescrp["ListPorts"] = "Port0";
		if (vCairn->getProblem()->createComponent(vComponentDescrp["type"], vComponentDescrp, {})) {// {} is a nest map of ports list
			MilpComponent* vComp = vCairn->getComponent(vComponentDescrp["id"]);
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
		std::map<std::string, std::string> vComponentDescrp;
		vComponentDescrp["id"] = "__Component";
		//vComponentDescrp["type"] = std::string(a_ComponentType.c_str());
		vComponentDescrp["Model"] = std::string(a_ModelClass.c_str());
		vComponentDescrp["ModelClass"] = std::string(a_ModelClass.c_str());
		vComponentDescrp["ListPorts"] = "Port0";
		if (vCairn->getProblem()->createComponent(vComponentDescrp["type"], vComponentDescrp, {})) {// {} is a nest map of ports list
			MilpComponent* vComp = vCairn->getComponent(vComponentDescrp["id"]);
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
	cInfo() << "Creating a study...";
	OptimProblemAPI vRet;
	if (m_Cairn) 
		CairnAPIUtils::setError(CairnAPIUtils::errDefault, "Study already exist");
	else {		
		std::string vQStudyName(a_StudyName.c_str());
	
		fs::path vLogFile(a_StudyName);
		vLogFile.replace_extension("log");
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
			cInfo() << "===================================================";
			cInfo() << "Reading study file by the api: " << a_filename;
			cInfo() << "===================================================";

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