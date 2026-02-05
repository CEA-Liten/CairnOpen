
#include "CairnAPI.h"
#include "CairnCore.h"
#include "BusCompo.h"
#include "CairnAPIUtils.h"

#include <unordered_set>

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
	// Par d�faut par d'exportResults
	SimulationControl* vSimulationControl = m_Problem->getSimulationControl();
	if (vSimulationControl) {
		CairnAPIUtils::setParameters({ 
			vSimulationControl->getCompoInputParam(), 
			vSimulationControl->getCompoInputSettings(),
			vSimulationControl->getGUIData()->getGuiInputParam()
			}, { {"ExportResults", 1} });
	}

	// Default TecEcoAnalysis
	cInfo() << "Init default TecEcoAnalysis...";
	TecEcoAnalysis* vTecEcoAnalysis = m_Problem->getTecEcoAnalysis();
	TecEcoCompo* pTecEco = dynamic_cast<TecEcoCompo*> (vTecEcoAnalysis->parent());
	if (pTecEco) {
		pTecEco->initSubModelConfiguration();
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
		std::string filename(a_filename);
		if (filename == "") {
			filename = vCairn->archFile();
		}

		int iErr = m_Problem->SaveFullArchitecture(filename, a_posAlgorithm);

		if(iErr == -1) {
			CairnAPIUtils::setError(errWrite, filename);
		}
		else {
			vErr = noError;
		}
	}
	CairnAPIUtils::setError(vErr);
}

void CairnAPI::OptimProblemAPI::export_Parameters(const std::string& fileName, const std::map<std::string, bool>& optionsMap)
{
	if (m_Problem) {
		try {
			m_Problem->exportParameters_all_files(fileName, optionsMap);
		}
		catch (const Cairn_Exception& cairn_error) {
			CairnAPIUtils::setError(CairnAPIUtils::errDefault, cairn_error.message());
		}
	}
	else {
		CairnAPIUtils::setError(noCairn);
	}
}

void CairnAPI::OptimProblemAPI::export_PLAN(const std::string& fileName, const int& aNsol)
{
	if (m_Problem) {
		try {
			m_Problem->exportResultsPLAN(fileName, aNsol);
		}
		catch (const Cairn_Exception& cairn_error) {
			CairnAPIUtils::setError(CairnAPIUtils::errDefault, cairn_error.message());
		}
	}
	else {
		CairnAPIUtils::setError(noCairn);
	}
}

void CairnAPI::OptimProblemAPI::add_Label(const std::string& a_Label)
{
	TecEcoAnalysis* vTecEcoAnalysis = m_Problem->getTecEcoAnalysis();
	if (vTecEcoAnalysis) {
		t_list vLabels = get_Labels();
		if (std::find(vLabels.begin(), vLabels.end(), a_Label) == vLabels.end()) {
			vTecEcoAnalysis->addLabel(a_Label);
		}
		else {
			CairnAPIUtils::setError(errDefault, "Label " + a_Label + " already exists!");
		}
	}
}

void CairnAPI::OptimProblemAPI::remove_Label(const std::string& a_Label)
{
	TecEcoAnalysis* vTecEcoAnalysis = m_Problem->getTecEcoAnalysis();
	if (vTecEcoAnalysis) {
		return vTecEcoAnalysis->removeLabel(a_Label);
	}
}


t_list CairnAPI::OptimProblemAPI::get_Labels() const
{
	TecEcoAnalysis* vTecEcoAnalysis = m_Problem->getTecEcoAnalysis();
	if (vTecEcoAnalysis) {
		return vTecEcoAnalysis->getLabelList();
	}
	return {};
}

void CairnAPI::OptimProblemAPI::set_Labels(const t_list& a_Labels)
{
	TecEcoAnalysis* vTecEcoAnalysis = m_Problem->getTecEcoAnalysis();
	if (vTecEcoAnalysis) {
		//remove duplicates
		std::unordered_set<std::string> seen;
		std::vector<std::string> result;
		for (std::string label : a_Labels) {
			if (seen.find(label) == seen.end()) {
				seen.insert(label);
				result.push_back(label);
			}
		}
		vTecEcoAnalysis->setLabelList(result);
	}
}

// -------------------------- Objects ---------------------
t_list CairnAPI::OptimProblemAPI::get_Objects()
{
	t_list vRet = {};
	if (m_Problem) {
		std::vector<CairnObject*> vList = m_Problem->children();
		for (auto& vComp : vList) {
			if (vComp->objectType()!="")
				vRet.push_back(vComp->objectName());
		}
	}
	return vRet;
}

CairnAPI::ObjectAPI CairnAPI::OptimProblemAPI::get_Object(const std::string& a_Name)
{
	ObjectAPI vRet;
	if (m_Problem) {
		CairnObject* vObject = m_Problem->findChild(a_Name);
		if (vObject) {
			vRet.set_Object(vObject);
		}
		else {
			CairnAPIUtils::setError(errNotFound, "Object " + a_Name);
		}
	}
	else {
		CairnAPIUtils::setError(noCairn);
	}
	return vRet;
}

// -------------------------- EnergyCarriers ---------------------
CairnAPI::EnergyVectorAPI CairnAPI::OptimProblemAPI::create_EnergyCarrier(const std::string& a_Name, const std::string& a_Type) const
{
	EnergyVectorAPI vCarrier;
	ECodeError vErr = noError;
	std::string vErrMsg = "EnergyCarrier " + a_Name;
	if (m_Problem) {
		vErr = noError;
		std::string name = std::string(a_Name.c_str());
		std::string type = std::string(a_Type.c_str());
		EnergyVector* vEnergyVector = m_Problem->findChild<EnergyVector>(a_Name);
		if (!vEnergyVector) {
			//Create EnergyVector
			bool vOK = m_Problem->createEnergyVector(name, type);
			if (vOK)
			{
				vEnergyVector = m_Problem->findChild<EnergyVector>(a_Name);
				vCarrier.set_EnergyVector(vEnergyVector);				
			}
			else {
				vErr = errCreate;
			}
		}
		else {
			//An EnergyVector with the same name already exists 
			vErr = errAlreadyExist;
		}
	}
	else{
		vErr = noCairn;
	}
	CairnAPIUtils::setError(vErr, vErrMsg);
	return vCarrier;
}

void CairnAPI::OptimProblemAPI::remove_EnergyCarrier(const std::string& a_Name)
{
	EnergyVectorAPI vVector = get_EnergyCarrier(a_Name); 
	remove_EnergyCarrier(vVector);
}

void CairnAPI::OptimProblemAPI::remove_EnergyCarrier(EnergyVectorAPI& a_EnergyVector)
{
	if (m_Problem) {
		// L'EnergyVector ne doit pas �tre utilis� par d'autres composants du probl�me
		// V�rification dans les Bus
		t_list vBuses = get_Buses();
		std::string vEVName = a_EnergyVector.get_Name();
		bool vFound = false;
		for (auto& vBus : vBuses) {
			if (get_Bus(vBus).get_CarrierName() == vEVName) {
				vFound = true;
				break;
			}
		}
		// V�rification dans les ports des composants
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
			for (auto vEV : m_Problem->EnergyVectors()) {
				if (vEV->Name() == vEVName) {
					delete vEV;
					break;
				}
			}
			a_EnergyVector.set_EnergyVector(nullptr);
		}
		else {
			CairnAPIUtils::setError(errErase, vEVName + ", using in other components");
		}
	}
}

//Returns the list of all energy carrier in the problem
t_list CairnAPI::OptimProblemAPI::get_EnergyCarriers()
{
	t_list vRet;
	if (m_Problem) {
		std::vector<EnergyVector*> vList = m_Problem->findChildren<EnergyVector>();
		for (auto& vComp : vList) {						
			vRet.push_back(vComp->objectName());			
		}
	}
	return vRet;
}

//Returns the energy carrier of a given name 
CairnAPI::EnergyVectorAPI CairnAPI::OptimProblemAPI::get_EnergyCarrier(const std::string &a_Name)
{
	EnergyVectorAPI vCarrier;
	if (m_Problem) {
		EnergyVector* vEnergyVector = m_Problem->findChild<EnergyVector>(a_Name);
		if (vEnergyVector) {
			vCarrier.set_EnergyVector(vEnergyVector);
		}
		else {
			CairnAPIUtils::setError(errNotFound, "EnergyVector " + a_Name);
		}
	}	
	else {
		CairnAPIUtils::setError(noCairn);
	}
	return vCarrier;
}

// -------------------------- Components -----------------------
t_list CairnAPI::OptimProblemAPI::get_Components(const std::string &a_Category)
{
	t_list vRet;	
	if (m_Problem) {
		std::vector<MilpComponent*> vQList = m_Problem->findChildren<MilpComponent>();
		for (auto& vComp : vQList) {
			BusCompo* lptrIsBus = dynamic_cast<BusCompo*> (vComp);
			if (!lptrIsBus) {
				if (a_Category == "")	{
					vRet.push_back(vComp->Name());
				}
				else {
					if (vComp->ModelClassName() == a_Category)	{
						vRet.push_back(vComp->Name());
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
t_list CairnAPI::OptimProblemAPI::get_TecEco_IndicatorNames()
{
	t_list vRet = {};
	if (m_Problem) {
		InputParam::t_Indicators vectIndicators = m_Problem->getTecEcoAnalysis()->getInputIndicators()->getIndicators();
		for (int i = 0; i < vectIndicators.size(); i++)
		{
			vRet.push_back(vectIndicators[i]->getName());
		}
	}
	else {
		CairnAPIUtils::setError(noCairn);
	}
	return vRet;
}

t_list CairnAPI::OptimProblemAPI::get_TecEco_IndicatorUnits()
{
	t_list vRet = {};
	if (m_Problem) {
		InputParam::t_Indicators vectIndicators = m_Problem->getTecEcoAnalysis()->getInputIndicators()->getIndicators();
		for (int i = 0; i < vectIndicators.size(); i++)
		{
			vRet.push_back(vectIndicators[i]->getUnit());
		}
	}
	else {
		CairnAPIUtils::setError(noCairn);
	}
	return vRet;
}

t_list CairnAPI::OptimProblemAPI::get_TecEco_IndicatorShortNames()
{
	t_list vRet = {};
	if (m_Problem) {
		InputParam::t_Indicators vectIndicators = m_Problem->getTecEcoAnalysis()->getInputIndicators()->getIndicators();
		for (int i = 0; i < vectIndicators.size(); i++)
		{
			vRet.push_back(vectIndicators[i]->getShortName());
		}
	}
	else {
		CairnAPIUtils::setError(noCairn);
	}
	return vRet;
}

t_dict CairnAPI::OptimProblemAPI::get_TecEco_IndicatorValues(const std::string range)
{
	t_dict vRet = {};
	if (m_Problem) {
		InputParam::t_Indicators vectIndicators = m_Problem->getTecEcoAnalysis()->getInputIndicators()->getIndicators();
		for (int i = 0; i < vectIndicators.size(); i++)
		{
			if (range == "PLAN") {
				vRet.insert({ vectIndicators[i]->getName(), vectIndicators[i]->getValue(0) });
			}
			else if (range == "HIST") {
				vRet.insert({ vectIndicators[i]->getName(), vectIndicators[i]->getValue(1) });
			}
		}
	}
	else {
		CairnAPIUtils::setError(noCairn);
	}
	return vRet;
}

double CairnAPI::OptimProblemAPI::get_TecEco_IndicatorValue(const std::string& name, const std::string range)
{
	double vRet = NAN;
	if (m_Problem) {
		InputParam::t_Indicators vectIndicators = m_Problem->getTecEcoAnalysis()->getInputIndicators()->getIndicators();
		for (int i = 0; i < vectIndicators.size(); i++) {
			if (vectIndicators[i]->getName() == name) {
				if (range == "PLAN") vRet = vectIndicators[i]->getValue(0);
				else if (range == "HIST") vRet = vectIndicators[i]->getValue(1);
			}
		}
		if (isnan(vRet)) CairnAPIUtils::setError(errGet, "Indicator name not found");
	}
	else {
		CairnAPIUtils::setError(noCairn);
	}
	return vRet;
}

t_dict CairnAPI::OptimProblemAPI::get_All_IndicatorValues(const std::string range)
{
	t_dict vRet = {};
	if (m_Problem) {
		//TecEco indicators
		t_dict tecEco_indicators = get_TecEco_IndicatorValues(range);
		std::string tecEcoName = m_Problem->getTecEcoAnalysis()->Name();
		for (auto& [key, value] : tecEco_indicators) {
			vRet.insert({tecEcoName+"."+key, value});
		}

		//Component indicators (including Bus)
		std::map<std::string, MilpComponent*> milpComponents = m_Problem->MilpComponents();
		for (auto& [key, value] : milpComponents) {		
			InputParam::t_Indicators vectIndicators = value->compoModel()->getInputIndicators()->getIndicators();
			std:string compoName = value->Name();
			for (int i = 0;i < vectIndicators.size();i++) {
				if (range == "PLAN") vRet.insert({ compoName+"."+vectIndicators[i]->getName(), vectIndicators[i]->getValue(0) });
				else if (range == "HIST") vRet.insert({ compoName+"."+vectIndicators[i]->getName(), vectIndicators[i]->getValue(1) });
			}
		}
	}
	else {
		CairnAPIUtils::setError(noCairn);
	}
	return vRet;
}

t_list CairnAPI::OptimProblemAPI::get_optimized_components()
{
	t_list vRet = {};
	if (m_Problem) {
		std::vector<MilpComponent*> vQList = m_Problem->findChildren<MilpComponent>();
		for (auto& vComp : vQList) {
			if (vComp->compoModel()->isSizeOptimized()) {
				vRet.push_back(vComp->Name());
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
		MilpComponent* vComp = m_Problem->findChild<MilpComponent>(a_Name);

		//BusCompo* vBus = dynamic_cast<BusCompo*> (vComp);
		//if (vBus) {
		//	CairnAPIUtils::setError(errDefault, a_Name + " is a bus component. Please, use method get_bus");
		//}

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

CairnAPI::MilpComponentAPI CairnAPI::OptimProblemAPI::create_Component(const std::string& a_Name, const std::string& a_ModelName) const
{
	MilpComponentAPI vRetCompo;
	std::string type = CairnAPIUtils::get_Component_Type(a_ModelName);
	ECodeError vErr = noError;
	std::string vErrMsg = "";
	if (m_Problem) {
		std::string vCompoName(a_Name);
		MilpComponent* vComponent = m_Problem->findChild<MilpComponent>(a_Name);
		if (!vComponent) {
			//Set the essential parameters of the componenet			
			std::map<std::string, std::string> paramMap;
			paramMap["id"] = vCompoName;
			paramMap["type"] = type;
			paramMap["ModelClass"] = a_ModelName;

			//Create component
			try {
				if (m_Problem->createComponent(type, paramMap, {})) {
					vComponent = m_Problem->findChild<MilpComponent>(a_Name);
					if (vComponent) {
						int ierr = vComponent->initProblem(false);
						if (ierr >= 0) {
							vRetCompo.set_MilpComponent(vComponent);
							vRetCompo.set_ModelClass(vComponent->ModelClassName());
						}
						else {
							vErr = errParam;
							vErrMsg = "error while initializing the component (possibly a mandatory parameter is missing!)";
						}
					}
					else {
						vErr = errDefault;
						vErrMsg = "Error : Creation of the component" + a_Name;
					}
				}
			}
			catch (const std::exception& e) {
				cCritical() << " Error : " << e.what();
				vErr = errDefault;
				vErrMsg = "Error : Creation of the component " + a_Name + "is Failed";
			}
		}
		else {
			vErr = errAlreadyExist;
			vErrMsg = "component " + a_Name;
		}
	}
	else {
		vErr = noCairn;
	}
	CairnAPIUtils::setError(vErr, vErrMsg);
	return vRetCompo;
}

void CairnAPI::OptimProblemAPI::remove_Component(const std::string& a_Name)
{
	MilpComponentAPI vCompAPI = get_Component(a_Name);
	remove_Component(vCompAPI);
}

void CairnAPI::OptimProblemAPI::remove_Component(MilpComponentAPI& a_Component)
{
	ECodeError vErr = noCairn;
	std::string vErrMsg = "";
	if (m_Problem) {
		// Suppression des ports
		t_list vPorts = a_Component.get_Ports();
		for (const std::string& vPort : vPorts) {
			MilpPortAPI vPortObj = a_Component.get_Port(vPort);
			a_Component.remove_Port(vPortObj, true);
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


// ----------------------- Bus -----------------------
t_list CairnAPI::OptimProblemAPI::get_Buses()
{
	t_list vRet;
	if (m_Problem) {
		std::vector<BusCompo*> vList = m_Problem->findChildren<BusCompo>();
		for (auto& vComp : vList) {			
			if (vComp) {				
				vRet.push_back(vComp->Name());
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
		BusCompo* vComp = m_Problem->findChild<BusCompo>(a_Name);
		if (vComp) {
			vRet.set_BusCompo(vComp);
		}
		else {
			CairnAPIUtils::setError(errNotFound, "Bus " + a_Name);
		}
	}
	else
		CairnAPIUtils::setError(noCairn);
	return vRet;	
}

CairnAPI::BusAPI CairnAPI::OptimProblemAPI::create_Bus(const std::string& a_Name, 
	const std::string& a_ModelName, const EnergyVectorAPI& a_EnergyVector) const
{
	BusAPI vBus;

	if (!a_EnergyVector.get_EnergyVector()) {
		CairnAPIUtils::setError(errDefault, "The EnergyCarrier must be defined!");
	}

	//std::string type = CairnAPIUtils::get_Component_Type(a_ModelName);
	std::string type = CairnAPIUtils::get_Bus_Type(a_ModelName);

	ECodeError vErr = noError;
	std::string vErrMsg = "";
	if (m_Problem) {
		std::string vBusName(std::string(a_Name.c_str()));
		BusCompo* vBusCompo = m_Problem->findChild<BusCompo>(a_Name);
		if (!vBusCompo) {
			//Build params map	
			std::map<std::string, std::string> paramMap;
			paramMap["id"] = vBusName;
			paramMap["type"] = std::string(type.c_str());
			paramMap["ModelClass"] = std::string(a_ModelName.c_str());
			paramMap["componentCarrier"] = std::string(a_EnergyVector.get_Name().c_str());

			//Create Bus component
			try {
				if (m_Problem->createComponent(paramMap["type"], paramMap, {})) {
					vBusCompo = m_Problem->findChild<BusCompo>(a_Name);
					if (vBusCompo) {
						vBusCompo->setMainCarrier(a_EnergyVector.get_EnergyVector());
						int ierr = vBusCompo->initProblem(false);
						//vBusCompo->declareIOVariables();
						if (ierr >= 0) {
							vBus.set_BusCompo(vBusCompo);
						}
						else {
							vErr = errParam;
							vErrMsg = "error while initializing the component (possibly a mandatory parameter is missing!)";
						}
					}
				}
			}
			catch (const std::exception& e)
			{
				cCritical() << " Error : " << e.what();
				vErr = errDefault;
				vErrMsg = "Error : Creation of the bus " + a_Name + "is Failed";
			}
		}
		else {
			vErr = errAlreadyExist;
			vErrMsg = "Bus " + a_Name;
		}
	}
	else {
		vErr = noCairn;
	}
	CairnAPIUtils::setError(vErr, vErrMsg);
	return vBus;
}

void CairnAPI::OptimProblemAPI::remove_Bus(const std::string& a_Name)
{	
	CairnAPI::BusAPI vBusAPI = get_Bus(a_Name);
	remove_Bus(vBusAPI);
}

void CairnAPI::OptimProblemAPI::remove_Bus(BusAPI& a_Bus)
{
	ECodeError vErr = noCairn;
	std::string vErrMsg = "";
	if (m_Problem) {
		// Suppression des liens
		BusCompo* vBus = a_Bus.get_BusCompo();
		if (vBus) {			
			for (auto& vPort : vBus->PortList()) {
				vPort->DeleteLinkedBus();
			}
		}		
		// Suppression du composant		
		try
		{
			m_Problem->deleteComponent(vBus);
			a_Bus.set_BusCompo(nullptr);
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
		if (a_port.get_Variable() == "") {
			vErrMsg = "Cannot connect a port before configuring its variable";
		}
		else if (a_port.get_CarrierName() != a_bus.get_CarrierName()) {
			vErrMsg = "Bus and port must have the same carrier";
		}
		else {
			MilpPort* vPort = a_port.get_MilpPort();
			BusCompo* vBus = a_bus.get_BusCompo();
			if (vPort && vBus) {
				vPort->setLinkedBus(vBus);
				vBus->addPort(vPort);

				MilpComponentAPI vCompAPI = get_Component(vPort->CompoName());
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
		if (a_port.get_CarrierName() == a_bus.get_CarrierName()) {
			MilpPort* vPort = a_port.get_MilpPort();
			BusCompo* vBus = a_bus.get_BusCompo();
			if (vPort && vBus) {
				vPort->DeleteLinkedBus();
				vBus->DeleteBusPort(vPort);
				MilpComponentAPI vCompAPI = get_Component(vPort->CompoName());
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



// -- TecEcoAnalysis ---
CairnAPI::TecEcoAnalysisAPI CairnAPI::OptimProblemAPI::get_TecEcoAnalysis()
{
	TecEcoAnalysisAPI vRet;
	if (m_Problem) {
		TecEcoCompo* vTecEcoAnalysis = m_Problem->findChild<TecEcoCompo>();
		if (vTecEcoAnalysis) {
			vRet.set_Object(vTecEcoAnalysis);
		}
		else {
			CairnAPIUtils::setError(errNotFound, "TecEcoAnalysis");
		}
	}
	else
		CairnAPIUtils::setError(noCairn);
	return vRet;
}

//----------- Solver ---------
CairnAPI::SolverAPI  CairnAPI::OptimProblemAPI::get_Solver()
{
	SolverAPI vRet;
	if (m_Problem) {
		Solver* vSolver = m_Problem->getSolver();
		if (vSolver) {
			vRet.set_Object(vSolver);
		}
		else {
			CairnAPIUtils::setError(errNotFound, "Solver");
		}
	}
	else
		CairnAPIUtils::setError(noCairn);
	return vRet;
}

//------------ SimulationControl ---------
CairnAPI::SimulationControlAPI CairnAPI::OptimProblemAPI::get_SimulationControl()
{
	SimulationControlAPI vRet;
	if (m_Problem) {
		SimulationControl* vSimulationControl = m_Problem->getSimulationControl();
		if (vSimulationControl) {
			vRet.set_Object(vSimulationControl);
		}
		else {
			CairnAPIUtils::setError(errNotFound, "SimulationControl");
		}
	}
	else
		CairnAPIUtils::setError(noCairn);
	return vRet;
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
		CairnAPIUtils::setError(errDefault, "Please, load at least one timeseries file before running");
	}
	else {
		// v�rification des fichiers
		for (auto& vFileName : m_timestepfileList) {
			fs::path vTimeStepFile(vFileName);
			std::error_code vErrFile;
			if (!fs::exists(vTimeStepFile, vErrFile)) {			
				CairnAPIUtils::setError(errDefault, "Error : timeseries file: "  + vFileName + " does not exist");
			}
		}
	}

	if (m_Problem) {
		m_Problem->closeExpressions();
		m_Problem->resetFlags();
		vRet.set_Problem(m_Problem);
		CairnCore* vCairn = (CairnCore*)m_Problem->parent();
		int iShift = 0;		
		int ierr = 0;
		int timeShift = vCairn->npdtTimeshift();
		bool persistent = false;

		// Prepare results path
		if (a_resultsPath != "") {						
			vCairn->setResultsDir(std::string(a_resultsPath.c_str()));

			// Save Study
			fs::path vResultsPath(vCairn->resultsDir());
			vResultsPath /= (vCairn->StudyName() + ".json");
			save_Study(vResultsPath.string());
		}

		//Clear _optim.log file
		cInfo() << "Clear _optim.log file";
		fs::path vResultsPath(vCairn->resultsDir());
		fs::path optimLogFileName = vResultsPath / (vCairn->StudyName() + "_optim.log");
		std::ofstream optimLogFile;
		optimLogFile.open(optimLogFileName.string(), std::ofstream::out | std::ofstream::trunc);
		optimLogFile.close();
			
		int vNbCycle = vCairn->nbcycle();
		for (int icycle = 0; icycle < vNbCycle; icycle++)
		{
			try {
				vCairn->importTS(m_timestepfileList, iShift);
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
				cCritical() << "ERROR : An Exception is detected in CairnCore::doStep!";
				cCritical() << "Error : Exit simulation!";
				ierr = -1;
				break;
			}
			if (ierr < 0) {
				vErrMsg = "Error in doStep of Cairn at cycle #" + std::to_string(icycle);				
				break;
			}
			if (ierr == 2 && !(vCairn->runUntilEnd()))	{
				cCritical() << "Error : No solution found by Cairn in cycle # " << icycle;
				break;
			}
			cInfo() << "Cycle" << icycle << "has finished.";
			CairnLogger::Flush();
			if (!persistent) iShift += timeShift;

			vRet.set_Results(icycle);
		}
		vRet.set_Status(ierr);

		if (ierr < 0)	{
			CairnAPIUtils::setError(errRun, vErrMsg);
		}			

		SimulationControl* vSimulationControl = m_Problem->getSimulationControl();
		if (vSimulationControl) {
			t_value vExportJson = CairnAPIUtils::getParameter({ 
				vSimulationControl->getCompoInputParam(), 
				vSimulationControl->getCompoInputSettings(),
				vSimulationControl->getGUIData()->getGuiInputParam() }, "ExportJson");
			if (CairnAPIUtils::getParamValue(vExportJson) == "1")
				m_Problem->SaveFullArchitecture();
		}

		vErr = noError;
	}
	CairnAPIUtils::setError(vErr, vErrMsg);
	return vRet;
}

