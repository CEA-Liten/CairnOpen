
#include "CairnAPI.h"
#include "CairnCore.h"
#include "BusCompo.h"
#include "CairnAPIUtils.h"

#include <unordered_set>

#include <filesystem>
namespace fs = std::filesystem;
using namespace CairnAPIUtils;
constexpr auto SAMPLINGRES = "sampling_results.csv";


CairnAPI::OptimProblemAPI::OptimProblemAPI()
{
	m_Problem = nullptr;
}

void CairnAPI::OptimProblemAPI::set_Problem(class OptimProblem* ap_Problem)
{
	m_Problem = ap_Problem;
	// By default, no exportResults
	SimulationControl* vSimulationControl = m_Problem->getSimulationControl();
	if (vSimulationControl) {
		CairnAPIUtils::setParameters({ 
			vSimulationControl->getCompoInputParam(), 
			vSimulationControl->getCompoInputSettings(),
			vSimulationControl->getGUIData()->getGuiInputParam()
			}, { {"ExportResults", 1} });
	}

	// Default TecEcoAnalysis
	cDebug() << "Init default TecEcoAnalysis...";
	TecEcoCompo* pTecEco = m_Problem->findChild<TecEcoCompo>();
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

	if (m_Problem) {
		CairnCore* vCairn = static_cast<CairnCore*>(m_Problem->parent());

		std::string filename = a_filename;

		// If empty => use archFile()
		if (filename.empty()) {
			filename = vCairn->archFile();
		}
		else {
			// If relative => resolve relative to archFile() directory
			fs::path inputPath(filename);
			if (!inputPath.is_absolute()) {
				fs::path archPath(vCairn->archFile());
				fs::path baseDir = archPath.parent_path();
				inputPath = baseDir / inputPath;
			}
			// Normalize the final path
			filename = fs::weakly_canonical(inputPath).string();
		}

		// Ensure the directory exists
		fs::path studyPath(filename);
		fs::path dir = studyPath.parent_path();
		if (!dir.empty() && !fs::exists(dir)) {
			std::error_code ec;
			fs::create_directories(dir, ec);
			if (ec) {
				CairnAPIUtils::setError(errWrite, dir.string());
				return;
			}
		}

		// Save
		//int iErr = m_Problem->SaveFullArchitecture(filename, a_posAlgorithm);
		int iErr = vCairn->saveStudy(filename, a_posAlgorithm); // use vCairn to register time

		if (iErr == -1) {
			CairnAPIUtils::setError(errWrite, filename);
		}
		else {
			vErr = noError;
		}
	}

	CairnAPIUtils::setError(vErr);
}

t_list CairnAPI::OptimProblemAPI::import_Group(const std::string& a_filename)
{
	const std::map <std::string, std::string> nameMapping = import_Group_GUI(a_filename);

	t_list names;
	names.reserve(nameMapping.size());

	for (const auto& [key, value] : nameMapping)
		names.push_back(key);

	return names;
}

std::map <std::string, std::string> CairnAPI::OptimProblemAPI::import_Group_GUI(const std::string& a_filename)
{
	if (!m_Problem) {
		CairnAPIUtils::setError(noCairn);
		return {};
	}

	std::string groupName;
	std::string mainNode;
	//std::map<std::string, std::string> components; 
	std::vector<CompoData> importedComponents;

	m_Problem->createComponentsFromJsonData(a_filename, &importedComponents,
		true /* isGroup */, &groupName, &mainNode);

	// TODO: use OptimProblem::initProblem() ?
	// TODO: move initProblem to CairnObject

	std::map <std::string, std::string> nameMapping;

	for (auto& component : importedComponents)
	{
		const std::string nameFromJsonFile = component.rawName;

		const std::string type = component.type;
		const std::string name = component.name;

		if (type == "TecEcoAnalysis" || type == "SimulationControl" || type == "Solver")
			continue; // it is not the case for a group

		if (CairnUtils::isEnergyVector(type)) {
			EnergyVector* pCarrier = m_Problem->findChild<EnergyVector>(name);
			if (pCarrier->initProblem() < 0) {
				throw Cairn_Exception("ERROR in initialization of carrier: " + pCarrier->Name(), -1);
			}
			continue;
		}

		MilpComponent* pComponent = m_Problem->findChild<MilpComponent>(name);

		if (pComponent) {
			m_Problem->createLinksToBus(pComponent);
			if (pComponent->initProblem() < 0) {
				throw Cairn_Exception("ERROR in initialization of component: " + pComponent->Name(), -1);
			}
		}
		else {
			BusCompo* pBus = m_Problem->findChild<BusCompo>(name);
			if (pBus) {
				m_Problem->createLinksToBus(pBus);
				if (pBus->initProblem() < 0) {
					throw Cairn_Exception("ERROR in initialization of Bus component: " + pBus->Name(), -1);
				}
				
			}
			else {
				// Error ?!
			}
		}

		nameMapping.emplace(nameFromJsonFile, name);
	}

	// Add group
	t_list names;
	names.reserve(nameMapping.size());

	for (const auto& [key, value] : nameMapping)
		names.push_back(key);

	m_Problem->addGroup(names, mainNode, groupName);

	// TODO: export/import vars for a single componenet?!
	m_Problem->createImportZEVariablesList();
	m_Problem->createExportZEVariablesList();

	return nameMapping; // Doesn't include EnergyVectors
}

std::shared_ptr <CairnAPI::MilpComponentAPI> CairnAPI::OptimProblemAPI::copy_Component(const std::string& name,
	const std::string& newName, bool connexions)
{
	// TODO: Generalize into copy object

	// Retrieve original component
	std::shared_ptr <MilpComponentAPI> compo = get_Component(name);

	// Create the new component with same model class
	std::string mo = compo->get_ModelClass();
	std::shared_ptr < CairnAPI::MilpComponentAPI> compo2 = create_Component(newName, compo->get_ModelClass());

	// ---------------------------------------------------------
	// Copy carriers + settings of default ports
	// ---------------------------------------------------------
	for (const auto& portID : compo->get_DefaultPortIDs())
	{
		std::shared_ptr < CairnAPI::MilpPortAPI> port = compo->get_Port(portID);
		std::shared_ptr<EnergyVectorAPI> carrier = get_EnergyCarrier(port->get_CarrierName());

		std::shared_ptr < CairnAPI::MilpPortAPI> port2 = compo2->get_Port(portID);
		port2->set_EnergyCarrier(*carrier);
		port2->set_SettingValues(port->get_SettingValues());
	}

	// ---------------------------------------------------------
	// Copy component-level settings and labels
	// ---------------------------------------------------------
	compo2->set_SettingValues(compo->get_SettingValues());
	compo2->set_LabelValues(compo->get_LabelValues());

	// ---------------------------------------------------------
	// Copy connections (optional)
	// ---------------------------------------------------------
	t_list existingPorts = compo2->get_Ports();
	if (connexions)
	{
		for (const auto& portName : compo->get_Ports())
		{
			std::shared_ptr < CairnAPI::MilpPortAPI> port = compo->get_Port(portName);
			std::shared_ptr<EnergyVectorAPI> carrier = get_EnergyCarrier(port->get_CarrierName());

			std::shared_ptr < CairnAPI::MilpPortAPI> port2;

			// If port does not exist in compo2 -> add it
			if (!CairnUtils::contains(existingPorts, portName))
			{
				port2 = compo2->add_Port(portName, *carrier); 
				port2->set_SettingValues(port->get_SettingValues());
			}
			else
			{
				port2 = compo2->get_Port(portName);
			}

			// Create links
			t_dict links{};
			compo->get_Links(links);
			for (const auto& [linkName, busName] : links) 
			{
				// linkName format: "ComponentName.PortName"
				const t_list parts = CairnUtils::split(linkName, '.');
				if (parts.size() == 2) {
					const std::string compoNameLink = parts[0];
					const std::string portNameLink = parts[1];

					if (compoNameLink == name && portNameLink == port->get_Name())
					{
						if (auto p = std::get_if<std::string>(&busName)) { // t_value -> std::string
							std::string bName = *p;
							std::shared_ptr < CairnAPI::BusAPI> bus = get_Bus(bName);
							add(*port2, *bus);
						}
					}
				}
			}
		}
	}

	return compo2;
}

void CairnAPI::OptimProblemAPI::export_Parameters(const std::string& fileName, const std::string& encoding,
	const std::map<std::string, bool>& optionsMap,
	const std::map< std::string, std::vector<ExtraParameterData> >& extraData)
{
	if (m_Problem) {
		try {
			m_Problem->exportParameters_all_files(fileName, encoding, optionsMap, extraData);
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

std::shared_ptr<CairnAPI::ObjectAPI> CairnAPI::OptimProblemAPI::get_Object(const std::string& a_Name)
{
	std::shared_ptr<CairnAPI::ObjectAPI> vRet = nullptr;
	if (m_Problem) {
		CairnObject* vObject = m_Problem->findChild(a_Name);
		if (vObject) {
			std::string vType = vObject->objectType();
			if (vType == "MilpComponent") {
				vRet = std::make_shared<CairnAPI::MilpComponentAPI>();
			}
			else if (vType == "BusCompo") {
				vRet = std::make_shared<CairnAPI::BusAPI>();
			}
			else if (vType == "EnergyVector") {
				vRet = std::make_shared<CairnAPI::EnergyVectorAPI>();
			}
			else if (vType == "MilpPort") {
				vRet = std::make_shared<CairnAPI::MilpPortAPI>();
			}
			else if (vType == "Solver") {
				vRet = std::make_shared<CairnAPI::SolverAPI>();
			}
			else if (vType == "TecEcoCompo") {
				vRet = std::make_shared<CairnAPI::TecEcoAnalysisAPI>();
			}
			else if (vType == "SimulationControl") {
				vRet = std::make_shared<CairnAPI::SimulationControlAPI>();
			}
			if (vRet)
				vRet->set_Object(vObject);
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
std::shared_ptr<CairnAPI::EnergyVectorAPI> CairnAPI::OptimProblemAPI::create_EnergyCarrier(const std::string& a_Name, 
	const std::string& a_Type, const std::string& a_TechnoType) const
{
	std::shared_ptr<CairnAPI::EnergyVectorAPI> vCarrier;
	ECodeError vErr = noError;
	std::string vErrMsg = "EnergyCarrier " + a_Name;
	if (m_Problem) {
		vErr = noError;		
		EnergyVector* vEnergyVector = m_Problem->findChild<EnergyVector>(a_Name);
		if (!vEnergyVector) {
			//Create EnergyVector
			bool vOK = m_Problem->createEnergyVector(a_Name, a_Type, a_TechnoType);
			if (vOK)
			{
				vEnergyVector = m_Problem->findChild<EnergyVector>(a_Name);
				vCarrier = std::make_shared<CairnAPI::EnergyVectorAPI>();
				vCarrier->set_EnergyVector(vEnergyVector);				
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

void CairnAPI::OptimProblemAPI::remove_EnergyCarrier(const std::string& a_Name, bool forceDeletion)
{
	std::shared_ptr<EnergyVectorAPI> vVector = get_EnergyCarrier(a_Name);
	remove_EnergyCarrier(*vVector, forceDeletion);
}

void CairnAPI::OptimProblemAPI::remove_EnergyCarrier(EnergyVectorAPI& a_EnergyVector)
{
	remove_EnergyCarrier(a_EnergyVector, false);
}

void CairnAPI::OptimProblemAPI::remove_EnergyCarrier(EnergyVectorAPI& a_EnergyVector, bool forceDeletion)
{
	if (m_Problem) {
		bool used = false;
		const std::string vEVName = a_EnergyVector.get_Name();

		if (!forceDeletion) {
			// The EnergyVector must not be used by other components of the problem
			
			// Verification in TecEcoAnalysis
			if (get_TecEcoAnalysis()->useEnergyVector(vEVName)) {
				used = true;
			}

			// Verification in the Buses
			if (!used) {
				t_list vBuses = get_Buses();
				for (auto& vBus : vBuses) {
					if (get_Bus(vBus)->get_CarrierName() == vEVName)
					{
						used = true;
						break;
					}
				}
			}

			// Verification in the component ports
			if (!used) {
				t_list vComps = get_Components();
				for (auto& vComp : vComps) {
					if (get_Component(vComp)->useEnergyVector(vEVName)) {
						used = true;
						break;
					}
				}
			}
		}

		if (!used) {
			// Deletion
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
t_list CairnAPI::OptimProblemAPI::get_EnergyCarriers() const
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
std::shared_ptr<CairnAPI::EnergyVectorAPI> CairnAPI::OptimProblemAPI::get_EnergyCarrier(const std::string &a_Name) const
{
	std::shared_ptr<EnergyVectorAPI> vCarrier;
	if (m_Problem) {
		EnergyVector* vEnergyVector = m_Problem->findChild<EnergyVector>(a_Name);
		if (vEnergyVector) {
			vCarrier = std::make_shared<EnergyVectorAPI>();
			vCarrier->set_EnergyVector(vEnergyVector);
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
t_list CairnAPI::OptimProblemAPI::get_Components() const
{
	return get_ComponentsByCategory();
}

t_list CairnAPI::OptimProblemAPI::get_ComponentsByCategory(const std::string &a_Category) const
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

// -- Indicators --
t_dict CairnAPI::OptimProblemAPI::get_All_IndicatorValues(const std::string& range) const
{
	t_dict vRet;

	if (!m_Problem) {
		CairnAPIUtils::setError(noCairn, "Problem not available");
		return vRet;
	}

	// TecEco indicators
	std::shared_ptr < TecEcoAnalysisAPI> tecEco = get_TecEcoAnalysis();
	t_dict tecEcoIndicators = tecEco->get_IndicatorValues(range);
	vRet.insert(tecEcoIndicators.begin(), tecEcoIndicators.end());

	// Component indicators
	const t_list& compoNames = get_Components();
	for (const auto& name : compoNames) {
		std::shared_ptr <MilpComponentAPI> compo = get_Component(name);
		t_dict compoIndicators = compo->get_IndicatorValues(range);
		vRet.insert(compoIndicators.begin(), compoIndicators.end());
	}

	// Bus indicators
	const t_list& busNames = get_Buses();
	for (const auto& name : busNames) {
		std::shared_ptr < BusAPI> bus = get_Bus(name);
		t_dict busIndicators = bus->get_IndicatorValues(range);
		vRet.insert(busIndicators.begin(), busIndicators.end());
	}

	return vRet;
}

t_list CairnAPI::OptimProblemAPI::get_optimized_components() const
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

std::shared_ptr<CairnAPI::MilpComponentAPI> CairnAPI::OptimProblemAPI::get_Component(const std::string &a_Name) const
{
	std::shared_ptr<CairnAPI::MilpComponentAPI> vRet = nullptr;
	if (m_Problem) {		
		MilpComponent* vComp = m_Problem->findChild<MilpComponent>(a_Name);

		if (vComp) {
			vRet = std::make_shared<CairnAPI::MilpComponentAPI>();
			vRet->set_MilpComponent(vComp);
		}
		else {
			CairnAPIUtils::setError(errNotFound, "component " + a_Name);
		}
	}	
	else
		CairnAPIUtils::setError(noCairn);
	return vRet;
}

std::shared_ptr < CairnAPI::MilpComponentAPI> CairnAPI::OptimProblemAPI::create_Component(const std::string& a_Name, const std::string& a_ModelName) const
{
	std::shared_ptr < MilpComponentAPI> vRetCompo;
	ECodeError vErr = noError;
	std::string vErrMsg = "";

	if (m_Problem) {
		MilpComponent* vComponent = m_Problem->findChild<MilpComponent>(a_Name);
		if (!vComponent) {
			//Set the essential parameters of the componenet	
			const std::string compoType = CairnAPIUtils::get_Component_Type(a_ModelName);
			const std::string compoName(a_Name);

			auto paramMap = CairnUtils::buildParamMap({
				{"type",  compoType},
				{"ModelType",  a_ModelName},
				{"ModelClass", a_ModelName}
			});

			//Create component
			try {
				if (m_Problem->createMilpComponent(compoName, compoType, paramMap, {})) {
					vComponent = m_Problem->findChild<MilpComponent>(a_Name);
					if (vComponent) {
						int ierr = vComponent->initProblem(false);
						if (ierr >= 0) {
							vRetCompo = std::make_shared<MilpComponentAPI>();
							vRetCompo->set_MilpComponent(vComponent);							
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
	std::shared_ptr <MilpComponentAPI> vCompAPI = get_Component(a_Name);
	remove_Component(*vCompAPI);
}

void CairnAPI::OptimProblemAPI::remove_Component(MilpComponentAPI& a_Component)
{
	ECodeError vErr = noCairn;
	std::string vErrMsg = "";
	if (m_Problem) {
		// Suppression des ports
		t_list vPorts = a_Component.get_Ports();
		for (const std::string& vPort : vPorts) {
			std::shared_ptr < MilpPortAPI> vPortObj = a_Component.get_Port(vPort);
			a_Component.remove_Port(*vPortObj, true);
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
t_list CairnAPI::OptimProblemAPI::get_Buses() const
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

std::shared_ptr < CairnAPI::BusAPI> CairnAPI::OptimProblemAPI::get_Bus(const std::string& a_Name) const
{
	std::shared_ptr < BusAPI> vRet;
	if (m_Problem) {
		BusCompo* vComp = m_Problem->findChild<BusCompo>(a_Name);
		if (vComp) {
			vRet = std::make_shared<BusAPI>();
			vRet->set_BusCompo(vComp);
		}
		else {
			CairnAPIUtils::setError(errNotFound, "Bus " + a_Name);
		}
	}
	else
		CairnAPIUtils::setError(noCairn);
	return vRet;	
}

std::shared_ptr < CairnAPI::BusAPI> CairnAPI::OptimProblemAPI::create_Bus(const std::string& a_Name,
	const std::string& a_ModelName, const EnergyVectorAPI& a_EnergyVector) const
{
	std::shared_ptr < BusAPI> vBus;

	if (!a_EnergyVector.get_EnergyVector()) {
		CairnAPIUtils::setError(errDefault, "The EnergyCarrier must be defined!");
	}

	const std::string compoType = CairnAPIUtils::get_Bus_Type(a_ModelName);

	ECodeError vErr = noError;
	std::string vErrMsg = "";
	if (m_Problem) {
		std::string vBusName(std::string(a_Name.c_str()));
		BusCompo* vBusCompo = m_Problem->findChild<BusCompo>(a_Name);
		if (!vBusCompo) {

			auto paramMap = CairnUtils::buildParamMap({
				{"type", compoType},
				{"ModelType", a_ModelName},
				{"ModelClass", a_ModelName},
				{"componentCarrier", a_EnergyVector.get_Name()}
			});

			//Create Bus component
			try {
				if (m_Problem->createMilpComponent(vBusName, compoType, paramMap, {})) {
					vBusCompo = m_Problem->findChild<BusCompo>(a_Name);
					if (vBusCompo) {
						vBusCompo->setMainCarrier(a_EnergyVector.get_EnergyVector());
						int ierr = vBusCompo->initProblem(false);
						//vBusCompo->declareIOVariables();
						if (ierr >= 0) {
							vBus = std::make_shared<BusAPI>();
							vBus->set_BusCompo(vBusCompo);
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
	std::shared_ptr < CairnAPI::BusAPI> vBusAPI = get_Bus(a_Name);
	remove_Bus(*vBusAPI);
}

void CairnAPI::OptimProblemAPI::remove_Bus(BusAPI& a_Bus)
{
	ECodeError vErr = noCairn;
	std::string vErrMsg = "";
	if (m_Problem) {
		// Suppression des liens
		BusCompo* vBus = a_Bus.get_BusCompo();
		if (vBus) {			
			for (auto& vPort : vBus->LinkedPorts()) {
				vPort->unlinkBus();
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
	
	// Use get_Objects() ?!

	t_list vComps = get_Components();
	for (auto& vComp : vComps) {
		get_Component(vComp)->get_Links(vRet);
	}

	// Add TecEcoAnalysis links
	get_TecEcoAnalysis()->get_Links(vRet);

	return vRet;
}

void CairnAPI::OptimProblemAPI::add(MilpPortAPI& a_port, BusAPI& a_bus)
{
	if (!m_Problem) {
		CairnAPIUtils::setError(noCairn, "");
		return;
	}

	// --- Validate port configuration ---
	if (a_port.get_Variable().empty()) {
		CairnAPIUtils::setError(errLink, "Cannot connect a port before configuring its variable");
		return;
	}

	// --- Validate carrier compatibility ---
	if (a_port.get_CarrierName() != a_bus.get_CarrierName()) {
		CairnAPIUtils::setError(errLink, "Bus and port must have the same carrier");
		return;
	}

	// --- Retrieve underlying objects ---
	MilpPort* vPort = a_port.get_MilpPort();
	BusCompo* vBus = a_bus.get_BusCompo();

	if (!vPort || !vBus) {
		CairnAPIUtils::setError(errLink, "Port or Bus null pointer");
		return;
	}

	// --- Resolve component owning the port ---
	const std::string& compName = vPort->CompoName();
	MilpComponent* vComp = nullptr;

	// Use get_Object() ?!

	// Direct MilpComponent
	vComp = m_Problem->findChild<MilpComponent>(compName);

	// TecEcoCompo 
	if (!vComp) {
		if (auto* vTecEco = m_Problem->findChild<TecEcoCompo>();
			vTecEco && vTecEco->Name() == compName)
		{
			vComp = dynamic_cast<MilpComponent*>(vTecEco);
		}
	}

	// BusCompo  
	if (!vComp) {
		if (auto* bus = m_Problem->findChild<BusCompo>(compName)) {
			vComp = static_cast<MilpComponent*>(bus);
		}
	}

	if (!vComp) {
		CairnAPIUtils::setError(errLink, "Component null pointer");
		return;
	}

	// --- Create the link ---
	vBus->addLink(vComp, vPort);

	CairnAPIUtils::setError(noError, "");
}


void CairnAPI::OptimProblemAPI::add(BusAPI& a_bus, MilpPortAPI& a_port)
{
	add(a_port, a_bus);
}

void CairnAPI::OptimProblemAPI::remove(MilpPortAPI& a_port, BusAPI& a_bus)
{
	if (!m_Problem) {
		CairnAPIUtils::setError(noCairn, "");
		return;
	}

	// --- Validate carrier compatibility ---
	if (a_port.get_CarrierName() != a_bus.get_CarrierName()) {
		CairnAPIUtils::setError(errLink, "Bus and port must have the same carrier");
		return;
	}

	// --- Retrieve underlying objects ---
	MilpPort* vPort = a_port.get_MilpPort();
	BusCompo* vBus = a_bus.get_BusCompo();

	if (!vPort || !vBus) {
		CairnAPIUtils::setError(errLink, "Port or Bus null pointer");
		return;
	}

	// --- Resolve component owning the port ---
	const std::string& compName = vPort->CompoName();
	MilpComponent* vComp = nullptr;

	// Use get_Object() ?!

	// Direct MilpComponent
	vComp = m_Problem->findChild<MilpComponent>(compName);

	// TecEcoCompo 
	if (!vComp) {
		if (auto* vTecEco = m_Problem->findChild<TecEcoCompo>();
			vTecEco && vTecEco->Name() == compName)
		{
			vComp = dynamic_cast<MilpComponent*>(vTecEco);
		}
	}

	// BusCompo  
	if (!vComp) {
		if (auto* bus = m_Problem->findChild<BusCompo>(compName)) {
			vComp = static_cast<MilpComponent*>(bus);
		}
	}

	if (!vComp) {
		CairnAPIUtils::setError(errLink, "Component null pointer");
		return;
	}

	// --- Remove the link ---
	vBus->removeLink(vComp, vPort);

	CairnAPIUtils::setError(noError, "");
}

void CairnAPI::OptimProblemAPI::remove(BusAPI& a_bus, MilpPortAPI& a_port)
{
	remove(a_port, a_bus);
}

// -- TecEcoAnalysis ---
std::shared_ptr < CairnAPI::TecEcoAnalysisAPI> CairnAPI::OptimProblemAPI::get_TecEcoAnalysis() const
{
	std::shared_ptr < TecEcoAnalysisAPI> vRet;
	if (m_Problem) {
		TecEcoCompo* pTecEco = m_Problem->findChild<TecEcoCompo>();
		if (pTecEco) {
			vRet = std::make_shared<TecEcoAnalysisAPI>();
			vRet->set_Object(pTecEco);
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
std::shared_ptr < CairnAPI::SolverAPI>  CairnAPI::OptimProblemAPI::get_Solver() const
{
	std::shared_ptr < SolverAPI> vRet;
	if (m_Problem) {
		Solver* vSolver = m_Problem->getSolver();
		if (vSolver) {
			vRet = std::make_shared<SolverAPI>();
			vRet->set_Object(vSolver);
		}
		else {
			CairnAPIUtils::setError(errNotFound, "Solver");
		}
	}
	else
		CairnAPIUtils::setError(noCairn);
	return vRet;
}

void CairnAPI::OptimProblemAPI::set_Solver(const std::string& name) const
{
	if (!m_Problem) {
		CairnAPIUtils::setError(noCairn);
		return;
	}

	Solver* solver = m_Problem->getSolver();
	if (!solver) {
		CairnAPIUtils::setError(errNotFound, "Solver");
		return;
	}

	t_list solverList;
	MIPSolverFactory vSolvers;
	vSolvers.getAllInfos(solverList);

	bool valid = std::find(solverList.begin(), solverList.end(), name) != solverList.end();
	if (!valid) {
		CairnAPIUtils::setError(errDefault, "Solver " + name + " is not valid. Available solvers are: " 
			+ CairnUtils::joinStrings(solverList));
		return;
	}

	SolverAPI solverAPI;
	solverAPI.set_Object(solver);
	solverAPI.set_SettingValue(PARAM_SOLVER_NAME, name);
}

//------------ SimulationControl ---------
std::shared_ptr < CairnAPI::SimulationControlAPI> CairnAPI::OptimProblemAPI::get_SimulationControl() const
{
	std::shared_ptr < SimulationControlAPI> vRet;
	if (m_Problem) {
		SimulationControl* vSimulationControl = m_Problem->getSimulationControl();
		if (vSimulationControl) {
			vRet = std::make_shared<SimulationControlAPI>();
			vRet->set_Object(vSimulationControl);
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
	ECodeError vErr = noCairn;
	if (m_Problem) {
		try {
			CairnCore* vCairn = (CairnCore*)m_Problem->parent();
			vCairn->addTS(CairnUtils::toWString(a_fileName));
			vErr = noError;
		}
		catch (Cairn_Exception& error)
		{
			vErr = errInit;
		}
	}
	CairnAPIUtils::setError(vErr);	
}

void CairnAPI::OptimProblemAPI::add_TimeSeries(const t_dict& a_TS)
{
	ECodeError vErr = noCairn;
	if (m_Problem) {
		try {
			CairnCore* vCairn = (CairnCore*)m_Problem->parent();
			vCairn->addTS(a_TS);
			vErr = noError;
		}
		catch (Cairn_Exception& error)
		{
			vErr = errInit;
		}
	}
	CairnAPIUtils::setError(vErr);
}

void CairnAPI::OptimProblemAPI::initialize()
{	
	ECodeError vErr = noCairn;
	if (m_Problem) {
		try {
			CairnCore* vCairn = (CairnCore*)m_Problem->parent();
			//vCairn->clearWarningANDErrors();
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

CairnAPI::SolutionAPI CairnAPI::OptimProblemAPI::run(const std::string& a_resultsPath, const bool& a_coSim)
{	
	if (!a_coSim) {
		//initialize problem
		initialize();
	}

	//Solve
	SolutionAPI vRet;
	ECodeError vErr = noCairn;
	std::string vErrMsg = "";
	
	if (m_Problem) {
		vRet.set_Problem(m_Problem);

		CairnCore* vCairn = (CairnCore*)m_Problem->parent();
		if (!a_coSim) {
			if (!vCairn->checkTS(vErrMsg)) {
				CairnAPIUtils::setError(errDefault, vErrMsg);
			}
		}

		int numCycle = vCairn->getNumCycle();
		if (numCycle < 0) {
			//init
			m_Problem->closeExpressions();
			m_Problem->resetFlags();
			if (a_coSim) vCairn->setStdAloneMode(false);
		}						
		
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
			
		int vNbCycle = a_coSim ? 1 : vCairn->nbcycle();
		for (int icycle = 0; icycle < vNbCycle; icycle++)
		{
			if (!a_coSim) {
				try {
					vCairn->importTS(iShift);
				}
				catch (Cairn_Exception cairn_error) {
					vErr = errRun;
					ierr = -1;
					break;
				}
			}

			CairnResult result = vCairn->doStep();

			if (result.status < 0)
			{
				cError() << result.error;
				ierr = result.status;
				break;
			}

			numCycle = vCairn->getNumCycle();

			if (ierr < 0) {
				vErrMsg = "Error in doStep of Cairn at cycle #" + std::to_string(numCycle);
				break;
			}
			if (ierr == 2 && !(vCairn->runUntilEnd()))	{
				cCritical() << "Error : No solution found by Cairn in cycle # " << numCycle;
				break;
			}
			//cInfo() << "Cycle " << std::to_string(numCycle) << " has finished.";
			CairnLogger::Flush();
			if (!persistent) iShift += timeShift;

			vRet.set_Results(icycle);
		}
		vRet.set_Status(ierr);

		if (ierr < 0)	{
			CairnAPIUtils::setError(errRun, vErrMsg);
		}			

		vCairn->saveStudy(vCairn->StudyName());

		// --- [PROFILING] Flush profiling data to the results directory --------------
		// If the results directory is not yet known, "."is used.
		{
			std::string outDir = vCairn->resultsDir();
			if (outDir.empty()) outDir = ".";
			CAIRN_PROFILE_FLUSH(outDir, vCairn->StudyName());
		}

		vErr = noError;
	}

	CairnAPIUtils::setError(vErr, vErrMsg);
	return vRet;
}


// -- Interfaces --
t_list CairnAPI::OptimProblemAPI::getSubscribedVariables()
{
	ECodeError vErr = noCairn;
	std::vector<string> varNames;
	if (m_Problem) {
		const t_mapExchange& sub = m_Problem->ListSubscribedVariables();

		for (const auto& elem : sub) {
			varNames.push_back(elem.first);
		}
		vErr = noError;
	}
	CairnAPIUtils::setError(vErr);
	return varNames;
}

t_list CairnAPI::OptimProblemAPI::getPublishedVariables()
{
	ECodeError vErr = noCairn;
	std::vector<string> varNames;
	if (m_Problem) {
		const t_mapExchange &sub = m_Problem->ListPublishedVariables();
	
		for (const auto& elem : sub) {
			if (!elem.second->IsUsed())
				continue;
			varNames.push_back(elem.first);
		}
		vErr = noError;
	}
	CairnAPIUtils::setError(vErr);
	return varNames;
}

void CairnAPI::OptimProblemAPI::setSubscribedVariableValue(const std::string& a_name, const std::vector<double>& a_values)
{	
	if (m_Problem) {
		const t_mapExchange& sub = m_Problem->ListSubscribedVariables();
		t_mapExchange::const_iterator vIter = sub.find(a_name);
		if (vIter != sub.end()) {
			vector<double>& vVar = *vIter->second->ptrVariable();
			if (vVar.size() == a_values.size()) {
				vVar = a_values;
			}
			else {
				cWarning() << "Set Subscribed variable " << a_name
					<< ", bad size of input values (" << a_values.size() << "), expected size = " << vVar.size();

				if (vVar.size() > a_values.size()) {
					size_t vStart = vVar.size() - a_values.size();
					for (size_t i = 0; i < a_values.size(); i++) {
						vVar[i+vStart] = a_values[i];
					}
				}
				else {
					size_t vStart = a_values.size() - vVar.size();
					for (size_t i = 0; i < vVar.size(); i++) {
						vVar[i] = a_values[i + vStart];
					}
				}				
			}			
		}
		else
			CairnAPIUtils::setError(errNotFound, "Subscribed variable " + a_name);				
	}
	else
		CairnAPIUtils::setError(noCairn);
}

std::vector<double> CairnAPI::OptimProblemAPI::getPublishedVariableValue(const std::string& a_name)
{
	std::vector<double> vRet;
	if (m_Problem) {
		const t_mapExchange& sub = m_Problem->ListPublishedVariables();
		t_mapExchange::const_iterator vIter = sub.find(a_name);
		if (vIter != sub.end()) {
			vector<double>& vVar = *vIter->second->ptrVariable();
			vRet = vVar;
		}
		else
			CairnAPIUtils::setError(errNotFound, "Published variable " + a_name);
	}
	else
		CairnAPIUtils::setError(noCairn);
	return vRet;
}

// -----------------------  run_sensitivity  -----------------------
CairnAPI::OptimProblemAPI::ModelValue::ModelValue(const t_dict& a_values)
{
	t_dict::const_iterator vIter = a_values.find("model");
	if (vIter != a_values.end()) {
		if (const std::string* pSrc = std::get_if<std::string>(&vIter->second)) {
			m_Model = *pSrc;
		}
	}
	vIter = a_values.find("property");
	if (vIter != a_values.end()) {
		if (const std::string* pSrc = std::get_if<std::string>(&vIter->second)) {
			m_Setting = *pSrc;
			if (CairnUtils::contains(m_Setting, "--")) {
				std::vector<std::string> vStrs = CairnUtils::split(m_Setting, "--");
				if (vStrs.size() > 1) {
					m_Port = vStrs[0];
					m_Setting = vStrs[1];
				}
			}
		}
	}
	vIter = a_values.find("port");
	if (vIter != a_values.end()) {
		if (const std::string* pSrc = std::get_if<std::string>(&vIter->second)) {
			m_Port = *pSrc;
		}
	}
	cDebug() << "run sensitivity, model: " << m_Model << ", setting: " << m_Setting << ", port: " << m_Port;

	vIter = a_values.find("value");
	if (vIter != a_values.end()) {
		m_Value = vIter->second;
	}	
}

CairnAPI::OptimProblemAPI::ModelValue::ModelValue(const std::string& a_Model, const std::string& a_Setting)
{
	m_Model = a_Model;
	
	if (CairnUtils::contains(a_Setting, "--")) {
		std::vector<std::string> vStrs = CairnUtils::split(a_Setting, "--");
		if (vStrs.size() > 1) {
			m_Port = vStrs[0];
			m_Setting = vStrs[1];
		}
	}
	else
		m_Setting = a_Setting;
}

bool CairnAPI::OptimProblemAPI::ModelValue::setValue(CairnAPI::OptimProblemAPI& a_Problem)
{
	bool vRet = true;	
	m_Component = a_Problem.get_Object(m_Model);
	if (m_Port == "") {
		try
		{
			m_SaveValue = m_Component->get_SettingValue(m_Setting);
			m_Component->set_SettingValue(m_Setting, m_Value);
		}
		catch (const std::exception&)
		{
			cError() << "run_sensitivity, set_value, parameter " << m_Setting;
			vRet = false;
		}
	}
	else {
		CairnAPI::MilpPortAPI vPort;
		if (get_Port(vPort)) {
			try
			{
				m_SaveValue = vPort.get_SettingValue(m_Setting);
				vPort.set_SettingValue(m_Setting, m_Value);
			}
			catch (const std::exception&)
			{
				cError() << "run_sensitivity, set_value, port " << m_Port << ", parameter " << m_Setting;
				vRet = false;
			}
		}
		else {
			cError() << "run_sensitivity, set_value, port not found " << m_Port;
			vRet = false;
		}
	}	
	return vRet;
}

bool CairnAPI::OptimProblemAPI::ModelValue::setValue(CairnAPI::OptimProblemAPI& a_Problem, const std::string& a_Value)
{
	m_Value = a_Value;
	return setValue(a_Problem);
}

void CairnAPI::OptimProblemAPI::ModelValue::reset()
{
	if (m_Port == "") {
		m_Component->set_SettingValue(m_Setting, m_SaveValue);
	}
	else {
		CairnAPI::MilpPortAPI vPort;
		if (get_Port(vPort)) {
			vPort.set_SettingValue(m_Setting, m_SaveValue);
		}
	}
}

bool CairnAPI::OptimProblemAPI::ModelValue::get_Port(CairnAPI::MilpPortAPI& a_Port) {
	bool vRet = false;
	std::string  vType = m_Component->get_ObjectType();
	if (vType == "MilpComponent") {
		auto vComp = std::dynamic_pointer_cast<CairnAPI::MilpComponentAPI>(m_Component);
		a_Port = *vComp->get_Port(m_Port);
		vRet = true;
	}
	else if (vType == "BusCompo") {
		auto vComp = std::dynamic_pointer_cast<CairnAPI::BusAPI>(m_Component);
		a_Port = *vComp->get_Port(m_Port);
		vRet = true;
	}
	else if (vType == "TecEcoCompo") {
		auto vComp = std::dynamic_pointer_cast<CairnAPI::TecEcoAnalysisAPI>(m_Component);
		a_Port = *vComp->get_Port(m_Port);
		vRet = true;
	}
	return vRet;
}

CairnAPI::OptimProblemAPI::KPI::KPI(const t_dict& a_values)
{
	t_dict::const_iterator vIter = a_values.find("model");
	if (vIter != a_values.end()) {
		if (const std::string* pSrc = std::get_if<std::string>(&vIter->second)) {
			m_Model = *pSrc;
		}		
	}
	vIter = a_values.find("indicator");
	if (vIter != a_values.end()) {
		if (const std::string* pSrc = std::get_if<std::string>(&vIter->second)) {
			m_Indicator = *pSrc;
		}
	}
}

CairnAPI::OptimProblemAPI::KPI::KPI(const std::string& a_Model, const std::string& a_Indicator)
{
	m_Model = a_Model;
	m_Indicator = a_Indicator;
}

double CairnAPI::OptimProblemAPI::KPI::getValue(OptimProblemAPI& a_Problem)
{
	double vValue = 0;
	try
	{
		std::shared_ptr <MilpComponentAPI> vComp = a_Problem.get_Component(m_Model);
		vValue = vComp->get_IndicatorValue(m_Indicator);
	}
	catch (const std::exception&)
	{
		try
		{
			std::shared_ptr < CairnAPI::BusAPI> vBus = a_Problem.get_Bus(m_Model);
			vValue = vBus->get_IndicatorValue(m_Indicator);
		}
		catch (const std::exception&)
		{
			try
			{
				std::shared_ptr < CairnAPI::TecEcoAnalysisAPI> vTecEco = a_Problem.get_TecEcoAnalysis();
				vValue = vTecEco->get_IndicatorValue(m_Indicator);
			}
			catch (const std::exception&)
			{
				// Indicator not found!
				vValue = 0;
			}
		}
	}
	return vValue;
}


void CairnAPI::OptimProblemAPI::KPI::printValue(OptimProblemAPI& a_Problem, t_dict& a_res)
{	
	a_res[m_Model + "." + m_Indicator] = getValue(a_Problem);	
}

void CairnAPI::OptimProblemAPI::KPI::printHeader(std::fstream& f)
{
	f << ";" << m_Model << "." << m_Indicator;
}

void CairnAPI::OptimProblemAPI::KPI::printValue(OptimProblemAPI& a_Problem, std::fstream& f)
{	
	f << ";" << getValue(a_Problem);
}

void CairnAPI::OptimProblemAPI::runSensitivityCSV(const std::string& a_samplingFileName, int a_max_time, const std::string& a_indicatorsFileName)
{
	CairnAPIUtils::ECodeError vErr = CairnAPIUtils::errFile;
	std::string vErrMsg = a_samplingFileName;
	fs::path vSamplingFilePath(a_samplingFileName);
	
	if (fs::exists(vSamplingFilePath)) {
		std::vector<std::vector<std::string>> vSamplings = CairnUtils::readFromCsvFile(a_samplingFileName);
		if (vSamplings.size() > 2) {
			if (vSamplings[0].size() > 1 && vSamplings[0].size() == vSamplings[1].size()) {
				if (a_max_time != -1) {
					std::shared_ptr <SolverAPI> vSolver = get_Solver();
					vSolver->set_SettingValue("TimeLimit", a_max_time);					
				}
				std::vector<std::vector<std::string>> vSamplingsKpi;
				if (fs::exists(a_indicatorsFileName)) {
					vSamplingsKpi = CairnUtils::readFromCsvFile(a_indicatorsFileName);
				}
				if (vSamplingsKpi.size() < 2) {
					vSamplingsKpi = { {"Model", "Indicator"}, {"TecEco", "OBJECTIVE"} };
				}
				std::vector< KPI > vKPIs;
				for (size_t i = 1; i < vSamplingsKpi.size(); i++) {					
					if (vSamplingsKpi[i].size() > 1)
						vKPIs.push_back({ vSamplingsKpi[i][0], vSamplingsKpi[i][1] });
				}
				std::fstream outfile;
				fs::path vResultSampling = vSamplingFilePath.replace_filename(SAMPLINGRES);
				if (!CairnUtils::openFileForWriting(outfile, vResultSampling.string(), std::ios_base::out)) {
					cWarning() << "OptimProblem, run_sensitivity: couldn't open result file for writing: " << vResultSampling.string();
				}
				else {
					outfile << ";Case";
					for (auto& vKPI : vKPIs) {
						 vKPI.printHeader(outfile);
					}
					outfile << std::endl;
				}

				std::vector< ModelValue > vModels;
				vModels.reserve(vSamplings[0].size() - 1);
				for (size_t i = 1; i < vSamplings[0].size(); i++) {
					vModels.push_back({ vSamplings[0][i], vSamplings[1][i] });
				}

				// loop on lines (one line = one case)
				for (size_t i = 2; i < vSamplings.size(); i++) {
					if (vSamplings[i].size() == vSamplings[0].size()) {
						// loop on rows (values)
						for (size_t j = 1; j < vSamplings[0].size(); j++) {
							if (!vModels[j - 1].setValue(*this, vSamplings[i][j])) {
								CairnAPIUtils::setError(CairnAPIUtils::errSet, vSamplings[0][i]);
								break;
							}										
						}
						run("Report_s" + vSamplings[i][0]);

						if (outfile.is_open()) {
							outfile << i-2 << ";" << vSamplings[i][0];
							for (auto& vKPI : vKPIs) {
								vKPI.printValue(*this, outfile);
							}
							outfile << std::endl;
						}

						for (auto& vModel : vModels) {
							vModel.reset();
						}
					}
				}
				vErr = CairnAPIUtils::noError;
			}
		}

	}	
	CairnAPIUtils::setError(vErr, vErrMsg);
}

t_dicts CairnAPI::OptimProblemAPI::runSensitivity(const t_dictsValues& a_sampling, int a_max_time, 
	const t_dicts& a_indicators, std::function<void(int)> on_iter)
{
	/* a_sampling: table: one line = one case, 
		one case: several maps, 
				first map= name of the case, 
				other map= model,property,value to change 
	*	[ 
			[ {"Case": "case1"},
			  {"model": "model1", "property":"prop1", "value":<value> }, 
			  {"model": "model2", "property":"prop1", "port":"port1", "value":<value> },
			  {"model": "model3", "property":"prop1", "value":<value> }
			],
			[ {"Case": "case2"},
			  {"model": "model1", "property":"prop1", "value":<value> },
			  {"model": "model1", "property":"prop1", "value":<value> }
			]		  
		],	
	*	
	* a_indicators (kpi)
	*	[ {"model": "model1", "indicator": "ind1" },
	*	  {"model": "model2", "indicator": "ind1" }
	*	]
	* 
	* return value (kpi results)
	*	[ {"Case": "case1", "model1.ind1": <value>, "model2.ind1":<value> },
	* 	  {"Case": "case2", "model1.ind1": <value>, "model2.ind1":<value> }
	*	],	
	*/	
	t_dicts vRet = {};
	if (a_sampling.size()) {
		if (a_max_time != -1) {
			std::shared_ptr <SolverAPI> vSolver = get_Solver();
			vSolver->set_SettingValue("TimeLimit", a_max_time);
		}
		std::vector< KPI > vKPIs;
		if (!a_indicators.size()) {
			vKPIs.push_back({ "TecEco", "OBJECTIVE" });
		}
		else {			
			for (auto& vKPI : a_indicators) {
				vKPIs.push_back(vKPI);
			}			
		}
		
		// loop on case
		int vIdx = 0;
		for (auto &vSampling: a_sampling) {
			// loop on properties to change for the case
			// first: get the name of the case
			std::string vCase = "";
			std::vector< ModelValue > vModels;						
			for (auto& vModel : vSampling) {
				t_dict::const_iterator vIter = vModel.find("model");
				if (vIter != vModel.end()) {
					vModels.push_back(vModel);					
					vModels[vModels.size() - 1].setValue(*this);
				}
				else {
					t_dict::const_iterator vIter = vModel.find("Case");
					if (vIter != vModel.end()) {
						if (const std::string* pSrc = std::get_if<std::string>(&vIter->second)) {
							vCase = *pSrc;			
						}
						else if (const int* pSrc = std::get_if<int>(&vIter->second)) {
							vCase = std::to_string(*pSrc);
						}
					}
				}
			}			
			if (vCase == "") {
				vCase = "Case" + std::to_string(vIdx);
			}

			cInfo() << "  ";
			cInfo() << " ############################################################################ ";
			cInfo() << "  ";
			cInfo() << " RunSensitivity, case: " << vCase;

			t_dict vResult = { { "Case", vCase } };

			run("Report_s" + vCase);

			for (auto& vKPI : vKPIs) {
				vKPI.printValue(*this, vResult);
			}

			for (auto& vModel : vModels) {
				vModel.reset();
			}

			vRet.push_back(vResult);

			vIdx++;

			if (on_iter) {
				on_iter(static_cast<int>(vIdx));   // notify caller
			}
		}
	}
	return vRet;
}