#include "CairnAPIUtils.h"
#include "CairnCore.h"
#include "Cairn_Exception.h"
#include "MIPSolverFactory.h"
#include "GlobalSettings.h"

static bool outLogs = true;
static QFile outLogFile;

void myMessageHandler(QtMsgType type, const QMessageLogContext&, const QString& msg)
{	
	if (outLogs) {
		QString txt;
		if (type == QtDebugMsg)
			txt = QString("Debug: %1").arg(msg);
		else if (type == QtWarningMsg)
			txt = QString("Warning: %1").arg(msg);
		else if (type == QtCriticalMsg)
			txt = QString("Critical: %1").arg(msg);
		else if (type == QtFatalMsg)
			txt = QString("Fatal: %1").arg(msg);
		else if (type == QtInfoMsg)
			txt = QString("Info: %1").arg(msg);
		else
			txt = QString(": %1").arg(msg);

		QTextStream ts(&outLogFile);		
		outLogFile.open(QIODevice::WriteOnly | QIODevice::Append);
#if defined(_MSC_VER)
		ts << txt << endl;
		std::cout << txt.toStdString() << endl;
#else
		ts << txt << Qt::endl;
		if (outLogs)
			std::cout << txt.toStdString() << Qt::endl;
#endif	
		outLogFile.close();
	}
}

CairnAPI::CairnAPI(bool a_Log)
{
	outLogs = a_Log;
	if (a_Log) {
		outLogFile.setFileName("Cairn.log");		
		outLogFile.open(QIODevice::WriteOnly);
		outLogFile.close();
	}	
	qInstallMessageHandler(myMessageHandler);
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
	CairnCore* vCairn = new CairnCore(nullptr, "Cairn", "__CairnAPI");
	if (vCairn) {
		QMap<QString, QString> vComponentDescrp;
		vComponentDescrp["id"] = "__Component";
		vComponentDescrp["type"] = QString(a_TechnoType.c_str());
		vComponentDescrp["ListPorts"] = "Port0";
		if (vCairn->getProblem()->createComponent(vComponentDescrp, {})) {// {} is a nest map of ports list
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
	CairnCore* vCairn = new CairnCore(nullptr, "Cairn", "__CairnAPI");
	if (vCairn) {
		QMap<QString, QString> vComponentDescrp;
		vComponentDescrp["id"] = "__Component";
		//vComponentDescrp["type"] = QString(a_ComponentType.c_str());
		vComponentDescrp["Model"] = QString(a_ModelClass.c_str());
		vComponentDescrp["ModelClass"] = QString(a_ModelClass.c_str());
		vComponentDescrp["ListPorts"] = "Port0";
		if (vCairn->getProblem()->createComponent(vComponentDescrp, {})) {// {} is a nest map of ports list
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

t_list CairnAPI::get_Solvers()
{
	t_list vRet;
	MIPSolverFactory vSolvers;
	QStringList vSolverLoaded;
	vSolvers.getAllInfos(vSolverLoaded);
	for (auto& vSolver : vSolverLoaded) {
		vRet.push_back(vSolver.toStdString());
	}
	return vRet;
}

//------------- Create Study ------------------------------------------
CairnAPI::OptimProblemAPI CairnAPI::create_Study(const std::string& a_StudyName)
{
	OptimProblemAPI vRet;
	QString vQStudyName(a_StudyName.c_str());
	//vQStudyName += ".json"; // force la lecture json
	// TODO: sépare nom de l'étude et nom complet du fichier (path+filename) ?	
	if (m_Cairn) 
		CairnAPIUtils::setError(CairnAPIUtils::errDefault, "Problem already exist");
	else {
		m_Cairn = new CairnCore(nullptr, "Cairn", vQStudyName);
		vRet.set_Problem(m_Cairn->getProblem());
	}	
	return vRet;
}


CairnAPI::OptimProblemAPI CairnAPI::read_Study(const std::string& a_filename)
{
	OptimProblemAPI vRet;
	QFileInfo vFileInfos(a_filename.c_str());
	if (vFileInfos.exists()) {
		vRet = create_Study(a_filename);		
		try
		{
			m_Cairn->doInit();
		}
		catch (Cairn_Exception& error)
		{
			CairnAPIUtils::setError(CairnAPIUtils::errRead, a_filename);
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