
#include "CairnAPI.h"
#include "CairnCore.h"
#include "BusCompo.h"
#include "CairnAPIUtils.h"
#include <filesystem>
namespace fs = std::filesystem;
using namespace CairnAPIUtils;

CairnAPI::OptimProblemAPI::OptimProblemAPI()
{
	m_Problem = nullptr;
}

void CairnAPI::OptimProblemAPI::set_Problem(class OptimProblem* ap_Problem)
{
	m_Problem = ap_Problem;
	// Par défaut par d'exportResults
	SimulationControl* vSimulationControl = m_Problem->getSimulationControl();
	if (vSimulationControl) {
		CairnAPIUtils::setParameters({ vSimulationControl->getCompoInputParam(), vSimulationControl->getCompoInputSettings() },
			{ {"ExportResults", 1} });
	}
}

void CairnAPI::OptimProblemAPI::set_StudyName(const std::string& a_Name)
{
	if (m_Problem) {
		CairnCore* vCairn = (CairnCore*)m_Problem->parent();
		//TODO: vCairn->set_Study(a_filename);
	}
	else
		CairnAPIUtils::setError(noCairn);
}


void CairnAPI::OptimProblemAPI::save_Study(const std::string& a_filename, const std::string& a_posAlgorithm)
{
	ECodeError vErr = noCairn;
	if (m_Problem)	{
		CairnCore* vCairn = (CairnCore*)m_Problem->parent();
		QString filename(a_filename.c_str());
		if (filename == "") {
			filename = vCairn->archFile();
		}

		int iErr = m_Problem->SaveFullArchitecture(filename, QString(a_posAlgorithm.c_str()));

		if(iErr == -1) {
			CairnAPIUtils::setError(errWrite, filename.toStdString());
		}
		else {
			vErr = noError;
		}
	}
	CairnAPIUtils::setError(vErr);
}

// -- EnergyCarriers ---
t_list CairnAPI::OptimProblemAPI::get_EnergyCarriers()
{
	t_list vRet;	
	if (m_Problem) {
		QList<EnergyVector*> vQList = m_Problem->getEnergyVectorList();
		for (auto& vComp : vQList) {						
			vRet.push_back(vComp->Name().toStdString());			
		}
	}
	return vRet;
}

CairnAPI::EnergyVectorAPI CairnAPI::OptimProblemAPI::get_EnergyCarrier(const std::string &a_Name)
{
	EnergyVectorAPI vRet;	
	if (m_Problem) {
		QString vComponentName = QString(a_Name.c_str());
		EnergyVector* vEnergyVector = m_Problem->getEnergyVector(vComponentName);
		if (vEnergyVector) {
			vRet.set_EnergyVector(vEnergyVector);			
		}
		else
			CairnAPIUtils::setError(errNotFound, "EnergyVector " + a_Name);
	}	
	else
		CairnAPIUtils::setError(noCairn);
	return vRet;
}

void CairnAPI::OptimProblemAPI::add(CairnAPI::EnergyVectorAPI& a_EnergyVector)
{
	ECodeError vErr = noCairn;
	std::string vErrMsg = "";
	if (m_Problem) {		
		QString vComponentName = QString(a_EnergyVector.get_Name().c_str());
		vErrMsg = "EnergyVector " + a_EnergyVector.get_Name();
		EnergyVector* vEnergyVector = m_Problem->getEnergyVector(vComponentName);
		if (!vEnergyVector) {
			// le vecteur n'existe pas, création
			if (m_Problem->createEnergyVector(vComponentName, QString(a_EnergyVector.get_Type().c_str()))) {
				vEnergyVector = m_Problem->getEnergyVector(vComponentName);
				t_dict vParams = a_EnergyVector.get_SettingValues();
				a_EnergyVector.set_EnergyVector(vEnergyVector);
				a_EnergyVector.set_SettingValues(vParams);
				vErr = noError;
			}
			else
				vErr = errCreate;
		}		
		else
			vErr = errAlreadyExist;
	}
	CairnAPIUtils::setError(vErr, vErrMsg);
}

void CairnAPI::OptimProblemAPI::remove(EnergyVectorAPI& a_EnergyVector)
{
	if (m_Problem) {
		// L'EnergyVector ne doit pas être utilisé par d'autres composants du problème
		// Vérification dans les Bus
		t_list vBuses = get_Buses();
		std::string vEVName = a_EnergyVector.get_Name();
		bool vFound = false;
		for (auto& vBus : vBuses) {
			if (get_Bus(vBus).get_EnergyVector() == vEVName) {
				vFound = true;
				break;
			}
		}
		// Vérification dans les ports des composants
		if (!vFound) {
			t_list vComps = get_Components();
			for (auto& vComp : vComps) {				
				if (get_Component(vComp).useEnergyVector(vEVName)) {
					vFound = true;
					break;
				}
			}
		}

		if (!vFound) {
			// Suppression
			m_Problem->deleteEnergyVector(QString(vEVName.c_str()));
			a_EnergyVector.set_EnergyVector(nullptr);
		}
		else {
			CairnAPIUtils::setError(errErase, vEVName + ", using in other components");
		}
	}
}

// -- Components ---
t_list CairnAPI::OptimProblemAPI::get_Components(const std::string &a_Category)
{
	t_list vRet;	
	if (m_Problem) {
		QList<MilpComponent*> vQList = m_Problem->findChildren<MilpComponent*>();;
		for (auto& vComp : vQList) {
			BusCompo* lptrIsBus = dynamic_cast<BusCompo*> (vComp);
			if (!lptrIsBus) {
				if (a_Category == "")	{
					vRet.push_back(vComp->Name().toStdString());
				}
				else {
					if (vComp->ModelClassName().toStdString() == a_Category)	{
						vRet.push_back(vComp->Name().toStdString());
					}
				}
			}			
		}
	}
	else
		CairnAPIUtils::setError(noCairn);
	return vRet;
}

// -- Indicators ---
t_list CairnAPI::OptimProblemAPI::get_Indicators(const std::string& a_ComponentName)
{
	t_list vRet;
	if (m_Problem) {
		QList<MilpComponent*> vQList = m_Problem->findChildren<MilpComponent*>();;
		for (auto& vComp : vQList) {
			BusCompo* lptrIsBus = dynamic_cast<BusCompo*> (vComp);
			if (!lptrIsBus) {
				if (vComp->Name().toStdString() == a_ComponentName) {
					InputParam::t_Indicators vectIndicators = vComp->compoModel()->getInputIndicators()->getIndicators();
					for (int i=0;i< vectIndicators.size();i++)
					{
						vRet.push_back(vectIndicators[i]->getName().toStdString());
					}
				}
			}
			//User-defined indicators
		}
	}
	else
		CairnAPIUtils::setError(noCairn);
	return vRet;
}

// -- optimized components ---
t_list CairnAPI::OptimProblemAPI::get_optimized_components()
{
	t_list vRet;
	if (m_Problem) {
		QList<MilpComponent*> vQList = m_Problem->findChildren<MilpComponent*>();;
		for (auto& vComp : vQList) {
			if (vComp->compoModel()->isSizeOptimized()) {
				vRet.push_back(vComp->Name().toStdString());
			}
		}
	}
	else {
		CairnAPIUtils::setError(noCairn);
	}
	return vRet;
}

CairnAPI::MilpComponentAPI CairnAPI::OptimProblemAPI::get_Component(const std::string &a_Name)
{
	MilpComponentAPI vRet;
	if (m_Problem) {
		QString vQName(a_Name.c_str());
		MilpComponent* vComp = m_Problem->findChild<MilpComponent*>(vQName);
		if (vComp) {
			vRet.set_MilpComponent(vComp);
		}
		else {
			CairnAPIUtils::setError(errNotFound, "component " + a_Name);
		}
	}	
	else
		CairnAPIUtils::setError(noCairn);
	return vRet;
}

void CairnAPI::OptimProblemAPI::add(MilpComponentAPI& a_Component)
{
	ECodeError vErr = noCairn;
	std::string vErrMsg = "";
	if (m_Problem) {
		QString vComponentName(QString(a_Component.get_Name().c_str()));
		MilpComponent* vComp = m_Problem->findChild<MilpComponent*>(vComponentName);
		if (!vComp) {
			//Set the essential parameters of the componenet			
			QMap<QString, QString> vComponent;
			vComponent["id"] = vComponentName;
			vComponent["type"] = QString(a_Component.get_Type().c_str());
			vComponent["ModelClass"] = QString(a_Component.get_ModelClass().c_str());

			//Add ports
			t_list vPorts = a_Component.get_Ports();
			if (!vPorts.size()) {
				vErrMsg = "component " + a_Component.get_Name() + ". The component doesn't has any port added.";
				setError(errAdd, vErrMsg);
				return;
			}
			else {
				QMap < QString, QMap<QString, QString> > vListPorts;
				int n = 1;
				for (auto& vPortName : vPorts) {
					MilpPortAPI vPort = a_Component.get_Port(vPortName);
					QString vQPortName = QString(vPortName.c_str());
					QString vPortId = "port" + QString::number(n);
					n++;
					QMap<QString, QString> vPortParams;
					//Get mandatory parameters of the port
					vPortParams["CompoName"] = vComponentName;
					vPortParams["Name"] = vQPortName;
					vPortParams["Carrier"] = QString(vPort.get_Type().c_str());
					vPortParams["Direction"] = QString(getParamValue(vPort.get_SettingValue("Direction")).c_str());
					vPortParams["Variable"] = QString(getParamValue(vPort.get_SettingValue("Variable")).c_str());
					//Get optional parameters of the port
					t_list vPortParamNames = vPort.get_SettingsList(CairnAPI::optional);
					t_dict vSettingsMap = vPort.get_SettingValues();
					for (auto& [vName, vValue] : vSettingsMap) {
						t_list::iterator vIter = find(vPortParamNames.begin(), vPortParamNames.end(), vName);
						if (vIter != vPortParamNames.end()) {
							vPortParams[QString(vName.c_str())] = QString(getParamValue(vValue).c_str());
						}						
					}
					vListPorts.insert(vPortId, vPortParams);
				}
				
				//Get componenet parameters 
				t_dict vCompoSettingMap = a_Component.get_SettingValues();
				for (auto& vOption : vCompoSettingMap) {
					vComponent[QString(vOption.first.c_str())] = QString(getParamValue(vOption.second).c_str());
				}

				//Create componenet	
				try {
					if (m_Problem->createComponent(vComponent, vListPorts)) {
						vComp = m_Problem->findChild<MilpComponent*>(vComponentName);
						if (vComp) {							
							a_Component.set_MilpComponent(vComp);
							int ierr = vComp->initProblem();
							vErr = (ierr >= 0) ? noError : errAdd;
							vErrMsg = (ierr >= 0) ? "" : "error while initializing the component (possibly a mandatory parameter is missing or a necessary port is missing!)";
						}
						else {
							vErr = errAdd;
							vErrMsg = "component " + a_Component.get_Name();
						}
					}
				}
				catch (const std::exception& e) {
					qCritical() << "Error : Creation of the component " << vComponent << "is Failed";
					qCritical() << " Error : " << e.what();
				}
			}
		}
		else {
			vErr = errAlreadyExist;
			vErrMsg = "component " + a_Component.get_Name();
		}
	}
	CairnAPIUtils::setError(vErr, vErrMsg);
}

void CairnAPI::OptimProblemAPI::remove(MilpComponentAPI& a_Component)
{
	ECodeError vErr = noCairn;
	std::string vErrMsg = "";
	if (m_Problem) {
		// Suppression des ports
		t_list vPorts = a_Component.get_Ports();
		for (const std::string& vPort : vPorts) {
			MilpPortAPI vPortObj = a_Component.get_Port(vPort);
			a_Component.removePort(vPortObj, false);
		}
		// Suppression du composant
		MilpComponent* vComp = a_Component.get_MilpComponent();
		try
		{
			m_Problem->deleteComponent(vComp);
			a_Component.set_MilpComponent(nullptr);
			vErr = noError;
		}
		catch (const std::exception&)
		{
			vErr = errErase;
			vErrMsg = a_Component.get_Name();
		}
	}
	CairnAPIUtils::setError(vErr, vErrMsg);
}


// -- Bus ---
t_list CairnAPI::OptimProblemAPI::get_Buses()
{
	t_list vRet;
	if (m_Problem) {
		QList<MilpComponent*> vQList = m_Problem->findChildren<MilpComponent*>();;
		for (auto& vComp : vQList) {
			BusCompo* lptrIsBus = dynamic_cast<BusCompo*> (vComp);
			if (lptrIsBus) {				
				vRet.push_back(vComp->Name().toStdString());
			}			
		}
	}	
	else
		CairnAPIUtils::setError(noCairn);
	return vRet;
}

CairnAPI::BusAPI CairnAPI::OptimProblemAPI::get_Bus(const std::string& a_Name)
{
	BusAPI vRet;
	if (m_Problem) {
		QString vQName(a_Name.c_str());
		BusCompo* vComp = m_Problem->findChild<BusCompo*>(vQName);
		if (vComp) {
			vRet.set_Bus(vComp);
		}
		else {
			CairnAPIUtils::setError(errNotFound, "Bus " + a_Name);
		}
	}
	else
		CairnAPIUtils::setError(noCairn);
	return vRet;	
}

void CairnAPI::OptimProblemAPI::add(BusAPI& a_Bus)
{
	ECodeError vErr = noCairn;
	std::string vErrMsg = "";
	if (m_Problem) {
		QString vComponentName(QString(a_Bus.get_Name().c_str()));
		BusCompo* vComp = m_Problem->findChild<BusCompo*>(vComponentName);
		if (!vComp) {
			// création du composant			
			QMap<QString, QString> vComponent;
			vComponent["id"] = vComponentName;
			vComponent["type"] = QString(a_Bus.get_Type().c_str());
			vComponent["Model"] = QString(a_Bus.get_ModelClass().c_str());
			vComponent["ModelClass"] = QString(a_Bus.get_ModelClass().c_str());

			std::string vEnergyVectorName = a_Bus.get_EnergyVector();
			if (vEnergyVectorName == "") {
				vErrMsg = "Bus " + a_Bus.get_Name() + " because it doesn't have an EnergyVector assigned";
				CairnAPIUtils::setError(errAdd, vErrMsg);
			}
			vComponent["VectorName"] = QString(vEnergyVectorName.c_str());

			// ajoute les options
			t_dict vParams = a_Bus.get_SettingValues();
			for (auto& vOption : vParams) {
				vComponent[QString(vOption.first.c_str())] = QString(getParamValue(vOption.second).c_str());
			}

			// création
			try {
				if (m_Problem->createComponent(vComponent, {})) {
					vComp = m_Problem->findChild<BusCompo*>(vComponentName);
					if (vComp) {
						a_Bus.set_Bus(vComp);
						int ierr = vComp->initProblem();
						vErr = (ierr >= 0) ? noError : errParam;		
						vErrMsg = (ierr >= 0) ? "" : "error while initializing the component (possibly a mandatory parameter is missing or a necessary port is missing!)";
					}
				}
			}
			catch (const std::exception& e)
			{
				qCritical() << "Error : Creation of the bus " << vComponent << "is Failed";
				qCritical() << " Error : " << e.what();
			}
		}
		else {
			vErr = errAlreadyExist;
			vErrMsg = "Bus " + a_Bus.get_Name();
		}
	}
	CairnAPIUtils::setError(vErr, vErrMsg);
}

void CairnAPI::OptimProblemAPI::remove(BusAPI& a_Bus)
{
	ECodeError vErr = noCairn;
	std::string vErrMsg = "";
	if (m_Problem) {
		// Suppression des liens
		BusCompo* vBus = a_Bus.get_BusCompo();
		if (vBus) {			
			for (auto& vPort : vBus->PortList()) {
				vPort->DeleteptrLinkedComponent();
			}
		}		
		// Suppression du composant		
		try
		{
			m_Problem->deleteComponent(vBus);
			a_Bus.set_Bus(nullptr);
			vErr = noError;
		}
		catch (const std::exception&)
		{
			vErr = errErase;
			vErrMsg = a_Bus.get_Name();
		}
	}
	CairnAPIUtils::setError(vErr, vErrMsg);
}

// -- Links ---
t_dict CairnAPI::OptimProblemAPI::get_Links()
{
	t_dict vRet;
	// retourne liste de paires : <componantName>.<PortName> , <BusName>
	t_list vComps = get_Components();
	for (auto& vComp : vComps) {
		get_Component(vComp).get_Links(vRet);
	}
	return vRet;
}

void CairnAPI::OptimProblemAPI::add(MilpPortAPI& a_port, BusAPI& a_bus)
{	
	ECodeError vErr = noCairn;
	std::string vErrMsg = "";
	if (m_Problem) {
		vErr = errLink;
		// bus et port doivent avoir le même energyVector
		if (a_port.get_EnergyVector() == a_bus.get_EnergyVector()) {
			MilpPort* vPort = a_port.get_MilpPort();
			BusCompo* vBus = a_bus.get_BusCompo();
			if (vPort && vBus) {
				vPort->setptrLinkedComponent(vBus);
				vBus->addPort(vPort);

				MilpComponentAPI vCompAPI = get_Component(vPort->CompoName().toStdString());
				MilpComponent* vComp = vCompAPI.get_MilpComponent();
				if (vComp) {
					vBus->addComponent(vComp);
					vErr = noError;
				}
				else {
					vErrMsg = "Component null pointer";
				}
			}						
			else {
				vErrMsg = "Port or Bus null pointer";
			}
		}
		else {
			vErrMsg = "Energy vector must be the same in bus and port";
		}
	}
	CairnAPIUtils::setError(vErr, vErrMsg);
}

void CairnAPI::OptimProblemAPI::add(BusAPI& a_bus, MilpPortAPI& a_port)
{
	add(a_port, a_bus);
}

void CairnAPI::OptimProblemAPI::remove(MilpPortAPI& a_port, BusAPI& a_bus)
{
	ECodeError vErr = noCairn;
	std::string vErrMsg = "";
	if (m_Problem) {
		vErr = errLink;		
		if (a_port.get_EnergyVector() == a_bus.get_EnergyVector()) {
			MilpPort* vPort = a_port.get_MilpPort();
			BusCompo* vBus = a_bus.get_BusCompo();
			if (vPort && vBus) {
				vPort->DeleteptrLinkedComponent();
				vBus->DeleteBusPort(vPort);
				MilpComponentAPI vCompAPI = get_Component(vPort->CompoName().toStdString());
				MilpComponent* vComp = vCompAPI.get_MilpComponent();
				if (vComp) {
					vBus->RemoveLinkComponent(vComp);
					vErr = noError;
				}
				else
					vErrMsg = "Component null pointer";			
			}
			else {
				vErrMsg = "Port or Bus null pointer";
			}
		}
		else {
			vErrMsg = "Energy vector must be the same in bus and port";
		}
	}
	CairnAPIUtils::setError(vErr, vErrMsg);
}

void CairnAPI::OptimProblemAPI::remove(BusAPI& a_bus, MilpPortAPI& a_port)
{
	remove(a_port, a_bus);
}



// -- Others ---
t_dict CairnAPI::OptimProblemAPI::get_TechEcoAnalysisSettings()
{
	t_dict vRet;
	ECodeError vErr = noCairn;
	if (m_Problem) {
		TecEcoAnalysis* vTecEcoAnalysis = m_Problem->getTecEcoAnalysis();
		if (vTecEcoAnalysis) {
			// récupère les paramètres						
			// Get dans CompoInputSettings et CompoInputParam 
			// récupère les noms et valeurs des paramètres	
			CairnAPIUtils::getParameters({
				vTecEcoAnalysis->getConfigParam(),
				vTecEcoAnalysis->getCompoInputParam(),
				vTecEcoAnalysis->getCompoInputSettings(),
				vTecEcoAnalysis->getCompoEnvImpactsParam()},
				vRet);			
			vErr = noError;			
		}
	}
	CairnAPIUtils::setError(vErr);
	return vRet;
}

void CairnAPI::OptimProblemAPI::set_TechEcoAnalysisSettings(const t_dict& a_Settings)
{
	ECodeError vRet = noCairn;
	std::string vErrMsg = "";
	if (m_Problem) {
		vRet = errNotFound;
		vErrMsg = "TecEcoAnalysis";
		TecEcoAnalysis* vTecEcoAnalysis = m_Problem->getTecEcoAnalysis();
		if (vTecEcoAnalysis) {
			//save the previous value of "ConsideredEnvironmentalImpacts"
			QString preSelectedEnvImpacts;
			vTecEcoAnalysis->getConfigParam()->getParameterValue("ConsideredEnvironmentalImpacts", preSelectedEnvImpacts, EParamType::eStringList);
			
			//split between configuration and non-configuration parameters
			QList<QString> configParamNames;
			vTecEcoAnalysis->getConfigParam()->getParameters(configParamNames);
			t_dict config_settings = {};
			t_dict settings = a_Settings;
			for (auto& vParam : a_Settings) {
				if (configParamNames.contains(QString(vParam.first.c_str()))) {
					config_settings[vParam.first] = vParam.second;
					settings.erase(vParam.first);
				}
			}

			//set configuration parameters
			bool vOk1 = CairnAPIUtils::setParameters({ vTecEcoAnalysis->getConfigParam() }, config_settings);
			if (vOk1 && config_settings.find("ConsideredEnvironmentalImpacts") != config_settings.end()) {
				//re-declare EnvImpact parameters of TecEcoAnalysis
				vTecEcoAnalysis->declareEnvImpactParam();
			}

			//set non-Configuration parameters
			bool vOk2 = CairnAPIUtils::setParameters(
				{ vTecEcoAnalysis->getCompoInputParam(), 
				vTecEcoAnalysis->getCompoInputSettings(), 
				vTecEcoAnalysis->getCompoEnvImpactsParam() }, 
				settings);
			
			if (vOk1 && vOk2) {
				qDebug() << "initialize the Optim Problem From Tec Eco Analysis";
				m_Problem->initOptimProblemFromTecEcoAnalysis();
				//
				QString selectedEnvImpacts;
				vTecEcoAnalysis->getConfigParam()->getParameterValue("ConsideredEnvironmentalImpacts", selectedEnvImpacts, EParamType::eStringList);
				if (selectedEnvImpacts != preSelectedEnvImpacts) {
					//Reinitialize added/created componenets
					initialize();
				}
			}
			vRet = (vOk1 && vOk2) ? noError : errParam;
			vErrMsg = (vOk1 && vOk2) ? "" : "parameter";
		}
	}
	CairnAPIUtils::setError(vRet, vErrMsg);
}

t_dict CairnAPI::OptimProblemAPI::get_MIPSolverSettings()
{
	t_dict vRet;	
	ECodeError vErr = noCairn;
	std::string vErrMsg = "";
	if (m_Problem) {
		Solver* vSolver = m_Problem->getSolver();
		if (vSolver) {
			// un solver existe, récupère les paramètres										
			CairnAPIUtils::getParameters({
				vSolver->getCompoInputParam(),
				vSolver->getCompoInputSettings() },
				vRet);
			vErr = noError;			
		}
		else {
			vErr = errNotFound;
			vErrMsg = "Solver";
		}
	}	
	CairnAPIUtils::setError(vErr, vErrMsg);
	return vRet;	
}

void CairnAPI::OptimProblemAPI::set_MIPSolverSettings(const t_dict& a_Settings)
{
	ECodeError vErr = noCairn;
	std::string vErrMsg = "";
	if (m_Problem) {
		vErr = errNotFound;
		vErrMsg = "Solver";
		Solver* vSolver = m_Problem->getSolver();
		if (vSolver) {
			// Vérification du nom
			bool vOk = true;			
			t_dict::const_iterator vIter = a_Settings.find("Solver");
			if (vIter != a_Settings.end()) {
				QString vSolverName = QString(CairnAPIUtils::getParamValue(vIter->second).c_str());
				if (vSolver->Name() != vSolverName) {
					// nom différent, création d'un nouveau solveur
					vOk = m_Problem->createSolver(vSolverName);
					if (vOk) {
						vSolver = m_Problem->getSolver();
						if (!vSolver) vOk = false;
					}
					else {
						vErr = errCreate;
						vErrMsg = "Problem to initialize solver " + vSolverName.toStdString();
					}
				}
			}												
			if (vOk) {
				vOk = CairnAPIUtils::setParameters({ vSolver->getCompoInputParam(), vSolver->getCompoInputSettings() }, a_Settings);
				vErr = (vOk) ? noError : errParam;
				vErrMsg = (vOk) ? "" : "parameter";
			}
		}
	}
	CairnAPIUtils::setError(vErr, vErrMsg);
}

t_dict CairnAPI::OptimProblemAPI::get_SimulationControlSettings()
{
	t_dict vRet;	
	ECodeError vErr = noCairn;
	if (m_Problem)	{
		SimulationControl* vSimulationControl = m_Problem->getSimulationControl();
		if (vSimulationControl)		{
			// une Simulation Control existe, récupère les paramètres									
			// Get dans CompoInputSettings et CompoInputParam 
			// récupère les noms et valeurs des paramètres	
			CairnAPIUtils::getParameters({
				vSimulationControl->getCompoInputParam(),
				vSimulationControl->getCompoInputSettings() },
				vRet);
			vErr = noError;
		}
	}
	CairnAPIUtils::setError(vErr);
	return vRet;	
}

void CairnAPI::OptimProblemAPI::set_SimulationControlSettings(const t_dict& a_Settings)
{
	ECodeError vRet = noCairn;
	std::string vErrMsg = "";
	if (m_Problem) {
		vRet = errNotFound;
		vErrMsg = "SimulationControl";
		SimulationControl* vSimulationControl = m_Problem->getSimulationControl();
		if (vSimulationControl) {
			bool vOk = CairnAPIUtils::setParameters({ vSimulationControl->getCompoInputParam(), vSimulationControl->getCompoInputSettings() }, a_Settings);
			//update MilpData
			if (vOk) {
				t_dict simuParams = get_SimulationControlSettings();
				qDebug() << "Update MilpData from SimulationControl parameters";
				MilpData * pMilpData = m_Problem->getMilpData();
				QMap<QString, QString> vParamMap;
				for (auto& [vParamName, vParamValue] : a_Settings) {
					vParamMap[QString(vParamName.c_str())] = QString(getParamValue(vParamValue).c_str());
				}
				//Take the (default) values of the remaining parameters 
				for (auto& [vSimuParamName, vSimuParamValue] : simuParams) {
					if (!vParamMap.contains(QString(vSimuParamName.c_str()))) {
						vParamMap[QString(vSimuParamName.c_str())] = QString(getParamValue(vSimuParamValue).c_str());
					}
				}
				vOk = pMilpData->setMilpDataFromSettings(true, vParamMap);
				m_Problem->setExtrapolationFactor();
			}
			vRet = (vOk) ? noError : errParam;
			vErrMsg = (vOk) ? "" : "parameter";
		}
	}
	CairnAPIUtils::setError(vRet, vErrMsg);
}


// -- Run ---
void CairnAPI::OptimProblemAPI::add_TimeSeries(const std::string& a_fileName)
{	
	// si filename = "" , efface la liste	
	if (a_fileName == "")	{	
		m_timestepfileList.clear();
	}
	else	{
		// ajoute un fichier time series dans la liste des fichiers
		m_timestepfileList.push_back(a_fileName);
	}
}

void CairnAPI::OptimProblemAPI::add_TimeSeries(const std::string& a_serie_name, const std::string& a_description, const std::string& a_unit, const t_values& a_times, const t_values& a_values)
{
}

void CairnAPI::OptimProblemAPI::initialize()
{	
	ECodeError vErr = noCairn;
	if (m_Problem) {
		try {
			CairnCore* vCairn = (CairnCore*)m_Problem->parent();
			vCairn->doInit(false);			
			vErr = noError;
		}
		catch (Cairn_Exception& error)
		{			
			vErr = errInit;
		}
	}
	CairnAPIUtils::setError(vErr);
}

CairnAPI::SolutionAPI CairnAPI::OptimProblemAPI::run(const std::string& a_resultsPath)
{	
	//initialize problem
	initialize();

	//Solve
	SolutionAPI vRet;
	ECodeError vErr = noCairn;
	std::string vErrMsg = "";

	// il faut au moins un fichier timeseries
	if (!m_timestepfileList.size()) {
		CairnAPIUtils::setError(errParam, "Please, load at least one timeseries file before running");
	}
	else {
		// vérificaton des fichiers
		for (auto& vFileName : m_timestepfileList) {
			QFile Input_File(QString(vFileName.c_str()));
			if (!Input_File.open(QIODevice::ReadOnly))
			{
				CairnAPIUtils::setError(errParam, "Error : this TimeSeries file name : "  + vFileName + " does not exist");								
			}
			Input_File.close();
		}
	}

	if (m_Problem) {		
		vRet.set_Problem(m_Problem);
		CairnCore* vCairn = (CairnCore*)m_Problem->parent();
		int iShift = 0;		
		int ierr = 0;
		int timeShift = vCairn->npdtTimeshift();
		bool persistent = false;

		// Prepare results path
		if (a_resultsPath != "") {						
			vCairn->setResultsDir(QString(a_resultsPath.c_str()));

			// Save Study
			fs::path vResultsPath(vCairn->resultsDir().toStdString());
			vResultsPath /= (vCairn->StudyName().toStdString() + ".json");
			save_Study(vResultsPath.string());
		}

		//Clear _optim.log file
		qInfo() << "Clear _optim.log file";
		fs::path vResultsPath(vCairn->resultsDir().toStdString());
		fs::path optimLogFileName = vResultsPath / (vCairn->StudyName().toStdString() + "_optim.log");
		std::ofstream optimLogFile;
		optimLogFile.open(optimLogFileName.string(), std::ofstream::out | std::ofstream::trunc);
		optimLogFile.close();

		QStringList QStimestepfileList;
		for (auto& vFile : m_timestepfileList) QStimestepfileList.push_back(QString(vFile.c_str()));
		
		int vNbCycle = vCairn->nbcycle();
		for (int icycle = 0; icycle < vNbCycle; icycle++)
		{
			try {
				vCairn->importTS(QStimestepfileList, iShift);
			}
			catch (Cairn_Exception cairn_error) {
				vErr = errRun;				
				ierr = -1;
				break;
			}
			try {
				ierr = vCairn->doStep();
			}
			catch (Cairn_Exception cairn_error) {
				qCritical() << "ERROR : An Exception is detected in CairnCore::doStep!";
				qCritical() << "Error : Exit simulation!";
				ierr = -1;
				break;
			}
			if (ierr < 0) {
				vErrMsg = "Error in doStep of Cairn at cycle #" + std::to_string(icycle);				
				break;
			}
			if (ierr == 2 && !(vCairn->runUntilEnd()))	{
				qCritical() << "Error : No solution found by Cairn in cycle # " << QString(icycle);
				break;
			}
			qInfo() << "Cycle" << icycle << "has finished.";
			
			if (!persistent) iShift += timeShift;

			vRet.set_Results(icycle);
		}
		vRet.set_Status(ierr);

		if (ierr < 0)	{
			CairnAPIUtils::setError(errRun, vErrMsg);
		}			

		SimulationControl* vSimulationControl = m_Problem->getSimulationControl();
		if (vSimulationControl) {
			// export OUTPUT Time series
			t_value vExport = CairnAPIUtils::getParameter({ vSimulationControl->getCompoInputParam(), vSimulationControl->getCompoInputSettings() }, "ExportResults");
			if (CairnAPIUtils::getParamValue(vExport) == "1") 
				vCairn->exportTS(vCairn->resultFile() + ".csv");

			//ExportJson: generate _self.json //Don't throw an error if failed 
			t_value vExportJson = CairnAPIUtils::getParameter({ vSimulationControl->getCompoInputParam(), vSimulationControl->getCompoInputSettings() }, "ExportJson");
			if (CairnAPIUtils::getParamValue(vExportJson) == "1")
				m_Problem->SaveFullArchitecture();
		}

		//reset flags
		m_Problem->resetFlags();

		vErr = noError;
	}
	CairnAPIUtils::setError(vErr, vErrMsg);
	return vRet;
}

