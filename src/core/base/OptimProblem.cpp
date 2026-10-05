#if (!defined(WIN32) && !defined(_WIN32))
#include <dlfcn.h>
#endif

#include "OptimProblem.h"
#include "MIPModeler.h"
#include "CairnUtils.h"

#include "ElectricalCarrier.h"
#include "MaterialCarrier.h"

using namespace CairnUtils;

using Eigen::Map;
using namespace GS ;

OptimProblem::OptimProblem(CairnObject* aParent, std::string aName, MilpData* aMilpData, const bool& aStdAloneMode) :
    CairnObject(aParent),
    mMilpData(aMilpData),
    mSolver(nullptr),
    mSimulationControl(nullptr),
    mExpObjective(nullptr),
    mStdAloneMode(aStdAloneMode),
    mExportIndicators(true),  
    mOptimStatus(2)
{
    this->setObjectType("OptimProblem");
    this->setObjectName(aName);

    //Retrieve the list of private submodels
    mModelFactory = new ModelFactory(spdlog::default_logger());
    mModelFactory->findModels();

    //Default TecEcoAnalysis
    if (!createTecEcoAnalysis()) {
        throw Cairn_Exception("Error creating the default TecEcoAnalysis!", -1);
    }

    //Default Solver 
    try {
        createSolver();
    }
    catch (const Cairn_Exception& error) {
        throw Cairn_Exception("Error creating Solver: " + error.message(), -1);
    }

    //Default SimulationControl 
    if (!createSimulationControl()) {
        throw Cairn_Exception("Error creating the default SimulationControl!", -1);
    }

} // OptimProblem()

OptimProblem::~OptimProblem()
{
    //delete mTecEcoAnalysis; mTecEcoAnalysis == mCompoModel which is deleted in ~TecEcoComponent()
    delete mSolver;
    delete mSimulationControl;
    delete mModelFactory;

    // Avoid dangling pointer
    mTecEcoAnalysis = nullptr;
    mSolver = nullptr;
    mSimulationControl = nullptr;
    mModelFactory = nullptr;

    // Clean up published variables
    for (auto& [key, var] : mListPublishedVars) {
        delete var;
    }
    mListPublishedVars.clear();
    mListSubscribedVars.clear();

    // Clean up MILP components
    for (auto& [key, value] : MilpComponents()) {
        delete value;
    }

    // Clean up energy vectors
    for (auto* value : EnergyVectors()) {
        delete value;
    }

    // Clean up dynamic indicators
    for (auto* indicator : mDynamicIndicators) {
        delete indicator;
    }
    mDynamicIndicators.clear();
}

std::vector<MilpComponent*> OptimProblem::NonBusMilpComponents()
{
    return findChildren<MilpComponent>();
}

std::vector<BusCompo*> OptimProblem::BusComponents()
{
    return findChildren<BusCompo>();
}

std::map<std::string, MilpComponent*> OptimProblem::MilpComponents()
{
    //TODO: return std::vector instead of std::map
    std::map<std::string, MilpComponent*> componentsMap = {};
    for (auto& component : NonBusMilpComponents())
    {
        componentsMap[component->Name()] = component;
    }
    for (auto& component : BusComponents())
    {
        componentsMap[component->Name()] = dynamic_cast<MilpComponent*> (component);
    }
    return componentsMap;
}

std::vector<EnergyVector*> OptimProblem::EnergyVectors()
{
    return findChildren<EnergyVector>();
}

//------------------------------------------------------------------------------
//  init Problem
//------------------------------------------------------------------------------

void OptimProblem::doInit(const StudyPathManager& aStudy, bool aLoad)
{
    mStudyPathManager = &aStudy;

    if (aLoad) {     
        /* Case of GUI */
        try {
            createComponentsFromJsonData(mStudyPathManager->archFile());
            createLinksToBus();
            createDynamicIndicators();
        }
        catch (Cairn_Exception& cairn_error) {
            cCritical() << "Error while creating in components in OptimProblem!";
            throw cairn_error;
        }
    }

    computeExtrapolationFactor();

    try {
        initProblem();
    }
    catch (Cairn_Exception& cairn_error) {
        throw cairn_error;
    }

    //TODO: move to MilpComponent::initProblem so a component-related vars are published at the component creation
    createImportZEVariablesList(); 

    createExportZEVariablesList();
}

void OptimProblem::createComponentsFromJsonData(const std::string& vJsonFile, 
    std::vector<CompoData>* importedComponents, bool isGroup,
    std::string* groupName, std::string* mainNode)
{
    // Use smart pointer for automatic memory management
    std::unique_ptr<JsonDescription> jsonDesc;
    try {
        jsonDesc = std::make_unique<JsonDescription>(vJsonFile);
    }
    catch (const Cairn_Exception& error) {
        throw Cairn_Exception(std::string("Failed to load JSON file. ") + error.message(), -1);
    }

    mStudyVersion = jsonDesc->CairnVersionJson();

    if (!isGroup && !mStudyVersion.empty()) {
        if (checkVersion() < 0)
            mStudyVersionMatchesCairn = false;
    }

    // Must be created in the following order
    if (!isGroup) {
        // Base components: TecEcoAnalysis, SimulationControl, Solver
        createBaseComponents(jsonDesc.get());

        // Extract user-defined indicators (only in case of read study)
        mDynamicIndicatorsData = jsonDesc->dynamicIndicators();

        // Extract groups (only in case of read study)
        mGroups = jsonDesc->extractGroupData();
    }

    // Carriers
    createEnergyVectors(jsonDesc.get(), importedComponents);

    // Create remaining components
    if (isGroup) {
        createComponents(jsonDesc.get(), importedComponents);
    }
    else {
        createUniqueComponents(jsonDesc.get()); // collect components ?!
    }

    // Populate group name if needed
    if (isGroup) {
        if(groupName)
            *groupName = jsonDesc->groupName();

        if(mainNode)
            *mainNode = jsonDesc->groupMainNode();
    }
}

void OptimProblem::createBaseComponents(JsonDescription* jsonDesc)
{
    if (!jsonDesc) {
        throw Cairn_Exception("JsonDescription is null", -1);
    }

    // TecEcoAnalysis — must be created first
    const auto& tecEcoParamData = jsonDesc->TecEcoParamData();
    if (!tecEcoParamData.empty()) {
        const std::string compoType = CairnUtils::getParamValue(tecEcoParamData, "type");
        const std::string compoName = CairnUtils::getParamValue(tecEcoParamData, "name");
        const auto ports = jsonDesc->extractPortParamData(compoName);

        if (!createTecEcoAnalysis(compoType, tecEcoParamData, ports, jsonDesc->LabelList())) {
            throw Cairn_Exception("Error creating the default TecEcoAnalysis!", -1);
        }
    }

    // SimulationControl — must be created second
    const auto& simControlParamData = jsonDesc->SimulationControlParamData();
    if (!simControlParamData.empty()) {
        const std::string compoType = CairnUtils::getParamValue(simControlParamData, "type");
        const std::string compoName = CairnUtils::getParamValue(simControlParamData, "name");

        if (!createSimulationControl(compoName, simControlParamData)) {
            throw Cairn_Exception("Error creating SimulationControl: " + compoName, -1);
        }
    }
    else {
        cInfo() << "No SimulationControl found. The default SimulationControl will be used.";
    }

    // Solver - order not important
    const auto& solverParamData = jsonDesc->SolverParamData();
    if (!solverParamData.empty()) {
        const std::string compoType = CairnUtils::getParamValue(solverParamData, "type");
        const std::string compoName = CairnUtils::getParamValue(solverParamData, "name");

        try {
            createSolver(compoName, solverParamData);
        }
        catch (const Cairn_Exception& error) {
            throw Cairn_Exception("Error creating Solver: " + error.message(), -1);
        }
    }
}

void OptimProblem::createEnergyVectors(JsonDescription* jsonDesc,
    std::vector<CompoData>* importedComponents)
{
    if (!jsonDesc) {
        throw Cairn_Exception("JsonDescription is null", -1);
    }

    const auto& carrierParamDataList = jsonDesc->CarrierParamDataList();
    for (const auto& carrierParamData : carrierParamDataList) {
        const std::string compoType = CairnUtils::getParamValue(carrierParamData, "type");
        const std::string compoName = CairnUtils::getParamValue(carrierParamData, "name");
        const std::string model = CairnUtils::getParamValue(carrierParamData, "ModelType");

        //if (!CairnUtils::isEnergyVector(compoType))
        //    continue;
 
        // Skip if already exists (only case of group)
        if (findChild<EnergyVector>(compoName)) {
            continue; // TODO: verify that the two EnergyVectors are identical 
                               // => have same type and parameter values
        }

        if (!createEnergyVector(compoName, compoType, model, carrierParamData)) {
            throw Cairn_Exception("Error creating EnergyVector " + compoName, -1);
        }

        if (importedComponents) {
            addImportedComponent(importedComponents, compoName, compoName, compoType); /* EnergyVector name doesn't change */
        }
    }
}

void OptimProblem::createUniqueComponents(JsonDescription* jsonDesc)
{
    /** Assumes that the names of all the components are unique (no collision).
        Used while reading a study (to improve performance).
    */

    if (!jsonDesc) {
        throw Cairn_Exception("JsonDescription is null in createUniqueComponents", -1);
    }

    static const std::map<std::string, t_mapLabels> emptyLabelMap{};
    const auto& labels = jsonDesc->LabelMap();

    auto& busParamDataList = jsonDesc->BusParamDataList();
    auto& componentParamDataList = jsonDesc->ComponentParamDataList();

    // Helper function to check if component should be skipped
    auto shouldSkipComponent = [](const std::string& compoType) -> bool {
        return compoType == "TecEcoAnalysis"
            || compoType == "SimulationControl"
            || compoType == "Solver"
            || CairnUtils::isEnergyVector(compoType);
    };

    // Helper lambda to process any map of components
    auto processComponentMap = [&](auto& paramDataList)
    {
        for (auto& compoParamData : paramDataList) 
        {
            const std::string compoType = CairnUtils::getParamValue(compoParamData, "type");

            if (shouldSkipComponent(compoType))
                continue;

            const std::string compoName = CairnUtils::getParamValue(compoParamData, "name");
            auto ports = jsonDesc->extractPortParamData(compoName);

            createComponent(compoParamData, ports, labels, nullptr); // , true /* isUniqueName */);
        }
    };

    // Process both maps (order is not important)
    processComponentMap(busParamDataList);
    processComponentMap(componentParamDataList);
}

void OptimProblem::createComponents(JsonDescription* jsonDesc,
    std::vector<CompoData>* importedComponents)
{
    /** A component may have the same name as an existing component.
        Used while importing a group (to resolve collision).
    */

    if (!jsonDesc) {
        throw Cairn_Exception("JsonDescription is null in createComponents", -1);
    }

    const std::vector<std::string> existingComponents = childrenNames();
    std::map<std::string, std::string> nameMapping{};

    const auto& labels = jsonDesc->LabelMap();

    // Helper lambda to extract linked components from ports
    auto extractLinkedComponents = [](const std::map<std::string, t_mapParamData>& ports)
        -> std::vector<std::string> {
        std::vector<std::string> linkedComponents;
        linkedComponents.reserve(ports.size());

        for (const auto& [portId, portData] : ports) {
            linkedComponents.push_back(
                CairnUtils::getParamValue(portData, "LinkedComponent")
            );
        }

        return linkedComponents;
    };

    // Helper lambda to check if all linked components are already created
    //auto areAllLinkedComponentsCreated = [&](const std::vector<std::string>& linkedComponents) -> bool {
    //    for (const auto& compo : linkedComponents) {
    //        if (nameMapping.find(compo) == nameMapping.end()) {
    //            return false;
    //        }
    //    }
    //    return true;
    //};

    auto areAllLinkedComponentsCreated = [&](const std::vector<std::string>& linkedComponents) -> bool
    {
        for (const auto& compo : linkedComponents) { /* linkedComponents contain raw names from the input file */
            // Look for a matching rawName
            auto it = std::find_if(importedComponents->begin(), importedComponents->end(),
                [&](const CompoData& c) { return c.rawName == compo; }
            );

            if (it == importedComponents->end())
                return false;
        }

        return true;
    };


    //TODO: split between busesWithoutPorts vs busesWithPorts or define hasPorts() inside JsonDescription

    // Buses (must be created before other components)
    const auto& busParamData = jsonDesc->BusParamDataList();
    std::vector<t_mapParamData> busesWithoutPorts{};
    std::vector<t_mapParamData> busesWithPorts{};

    // Separate buses based on whether they have ports
    for (const auto& bus : busParamData) {
        const std::string compoType = CairnUtils::getParamValue(bus, "type");
        //if (!CairnUtils::isBus(compoType))
        //    continue;

        const std::string compoName = CairnUtils::getParamValue(bus, "name");
        auto ports = jsonDesc->extractPortParamData(compoName);

        if (ports.empty()) {
            busesWithoutPorts.push_back(bus);
        }
        else {
            busesWithPorts.push_back(bus);
        }
    }

    // Create buses without ports first (they have no dependencies)
    for (auto& bus : busesWithoutPorts) {
        std::map<std::string, t_mapParamData> emptyPorts{};
        createComponent(bus, emptyPorts, labels, importedComponents, existingComponents);
    }

    // Create buses with ports in dependency order
    while (!busesWithPorts.empty()) {
        for (auto it = busesWithPorts.begin(); it != busesWithPorts.end(); ) {
            auto& bus = *it;
            const std::string compoName = CairnUtils::getParamValue(bus, "name");
            auto ports = jsonDesc->extractPortParamData(compoName);

            auto linkedComponents = extractLinkedComponents(ports);

            if (areAllLinkedComponentsCreated(linkedComponents)) {
                createComponent(bus, ports, labels, importedComponents, existingComponents);
                it = busesWithPorts.erase(it);  // Safe erase
            }
            else {
                ++it;
            }
        }
    }

    // Helper function to check if component should be skipped in main processing
    auto shouldSkipComponent = [](const std::string& compoType) -> bool {
        return compoType == "TecEcoAnalysis"
            || compoType == "SimulationControl"
            || compoType == "Solver"
            || CairnUtils::isEnergyVector(compoType)
            || CairnUtils::isBus(compoType);
    };

    // Create remaining MILP components (non-bus)
    auto& componentParamData = jsonDesc->ComponentParamDataList();
    for (auto& component : componentParamData) {  
        const std::string compoType = CairnUtils::getParamValue(component, "type");

        if (shouldSkipComponent(compoType))
            continue;

        const std::string compoName = CairnUtils::getParamValue(component, "name");
        auto ports = jsonDesc->extractPortParamData(compoName);

        createComponent(component, ports, labels, importedComponents, existingComponents);
    }
}

void OptimProblem::createComponent(
    const t_mapParamData& compoParamData, 
    const std::map<std::string, t_mapParamData>& ports, 
    const std::map<std::string, t_mapLabels>& labels,
    std::vector<CompoData>* importedComponents,
    // bool isUniqueName,
    const std::vector<std::string>& existingComponents)  
{
    // Validate parameters
    //if (!isUniqueName && !nameMapping) {
    //    throw Cairn_Exception(
    //        "nameMapping must be provided when isUniqueName is false", -1);
    //}

    // Extract component information (done before !)
    const std::string compoType = CairnUtils::getParamValue(compoParamData, "type");
    const std::string compoName = CairnUtils::getParamValue(compoParamData, "name");

    if (compoType.empty() || compoName.empty()) {
        throw Cairn_Exception(
            "Component missing required 'type' or 'name' field", -1);
    }

    std::string uniqueName = compoName;

    // Ensure that the component has a unique name, if needed
    if (importedComponents) {
        const std::string model = CairnUtils::getParamValue(compoParamData, "ModelType");

        // Collect names to exclude
        std::vector<std::string> excludeNames;
        //if (nameMapping) {
        //    excludeNames.reserve(nameMapping->size());
        //    for (const auto& [originalname, newName] : *nameMapping) {
        //        excludeNames.push_back(newName);
        //    }
        //}

        excludeNames.reserve(importedComponents->size());
        for (const auto& component : *importedComponents) {
            excludeNames.push_back(component.name);
        }

        uniqueName = CairnUtils::getAutoCompoName(existingComponents, model, compoName, true, excludeNames);

        // Update the component's name in the parameter data (for safety)
        CairnUtils::setParamValue(const_cast<t_mapParamData&>(compoParamData), "name", uniqueName);

        // importedComponents is also used for name Mapping and to generate excludeNames
        addImportedComponent(importedComponents, compoName, uniqueName, compoType);

        // Update linked component references in ports
        for (const auto& [portId, portData] : ports)
        {
            const std::string linkedComponent = CairnUtils::getParamValue(portData, "LinkedComponent");

            // Find matching CompoData in the vector
            auto it = std::find_if(importedComponents->begin(), importedComponents->end(),
                [&](const CompoData& c) { return c.rawName == linkedComponent; }
            );

            if (it != importedComponents->end() && linkedComponent != it->name)
            {
                CairnUtils::setParamValue(const_cast<t_mapParamData&>(portData),
                    "LinkedComponent", it->name);
            }
        }
    }

    // Create the actual component
    MilpComponent* pComponent = createMilpComponent(uniqueName, compoType, compoParamData, ports);

    if (!pComponent) {
        throw Cairn_Exception(
            "Failed to create component of type '" + compoType +
            "' with name '" + compoName + "'", -1);
    }

    // Set labels 
    SubModel* pModel = pComponent->compoModel();
    if (pModel) {
        auto it = labels.find(compoName); /* original name */
        if (it != labels.end()) {
            pModel->setLabelMap(it->second);
        }
    }
}

void OptimProblem::computeExtrapolationFactor() {
    if (!mTecEcoAnalysis || !mMilpData)
        return;
    mTecEcoAnalysis->computeExtrapolationFactor(mMilpData);
}

bool OptimProblem::createTecEcoAnalysis(const std::string& componentType, 
    const t_mapParamData& paramsMap,
    const std::map < std::string, t_mapParamData>& portsMap, 
    const std::vector<std::string>& labelList)
{
    const std::string compoName = CairnUtils::getParamValue(paramsMap, "name");

    createMilpComponent(compoName, componentType, paramsMap, portsMap);

    if (!mTecEcoAnalysis)
        return false;

    computeExtrapolationFactor();
    mTecEcoAnalysis->setLabelList(labelList);

    return true;
}

bool OptimProblem::createSimulationControl(const std::string& aName, const t_mapParamData& paramMap)
{
    delete mSimulationControl;
    mSimulationControl = new SimulationControl(this, aName, paramMap);

    if (mSimulationControl) {
        //Set MilpData from SimulationControl params
        mMilpData->setMilpDataFromSettings(mSimulationControl->getParameters(), mStdAloneMode);
        computeExtrapolationFactor();
        return true;
    }
    return false;
}

void OptimProblem::createSolver(const std::string& aName, const t_mapParamData& paramMap)
{
    // TODO: use unique_ptr
    delete mSolver;
    mSolver = nullptr;
    mSolver = new Solver(this, aName, paramMap);
}

bool OptimProblem::createEnergyVector(const std::string& aName, const std::string& aType,
    const std::string& aTechnoType, const t_mapParamData& paramMap)
{
    EnergyVector* lptr_EV = nullptr;
    if (aType == "ElectricalCarrier" || aType == "Electrical" || aTechnoType == "Electricity")
        lptr_EV = new ElectricalCarrier(this, aName, aTechnoType, paramMap);
    else
        lptr_EV = new MaterialCarrier(this, aName, aTechnoType, paramMap);

    return (lptr_EV != nullptr);
}

MilpComponent* OptimProblem::createMilpComponent(const std::string& compoName, const std::string& compoType,
    const t_mapParamData& paramsMap, const std::map < std::string, t_mapParamData>& portsMap)
{
    MilpComponent* lptr = nullptr;
    try {
        if (compoType == "TecEcoAnalysis")
        {
            // only one TecEco
            TecEcoCompo *pTecEco = findChild<TecEcoCompo>();
            if (pTecEco) removeChild(pTecEco);
            lptr = dynamic_cast <MilpComponent*> (new TecEcoCompo(this, compoName, paramsMap, portsMap, mMilpData, mModelFactory));
            lptr->initMilpComponent();
            mTecEcoAnalysis = dynamic_cast<TecEcoAnalysis*> (lptr->compoModel());
            lptr->setTecEcoAnalysis(mTecEcoAnalysis); //In case of TecEcoCompo mTecEcoAnalysis == mCompoModel ! 
        }
        else if (compoType == "Converter"
            || compoType == "Storage"
            || compoType == "PhysicalEquation"
            || compoType == "OperationConstraint")
        {
            lptr = dynamic_cast <MilpComponent*> (
                new MilpComponent(this, compoName, mMilpData, mTecEcoAnalysis, paramsMap, portsMap, mModelFactory)
                );
            lptr->initMilpComponent();
        }    
        else if (compoType == "Grid")
        {
            lptr = dynamic_cast <MilpComponent*> (
                new GridCompo(this, compoName, paramsMap, portsMap, mMilpData, mTecEcoAnalysis, mModelFactory)
                );
            lptr->initMilpComponent();
        }
        else if (compoType == "SourceLoad") {
            lptr = dynamic_cast <MilpComponent*> (
                new SourceLoadCompo(this, compoName, paramsMap, portsMap, mMilpData, mTecEcoAnalysis, mModelFactory)
                );
            lptr->initMilpComponent();
        }
        else if (compoType == "BusFlowBalance" || compoType == "BusSameValue") {
            lptr = dynamic_cast <MilpComponent*> (
                new BusCompo(this, compoName, paramsMap, portsMap, mMilpData, mTecEcoAnalysis, mModelFactory)
                );
            lptr->initMilpComponent();
            /* set EnergyCarrier (needed for case GUI). When using API, carrier is set in OptimProblemAPI::create_Bus */
            configureBusCarrier(lptr, CairnUtils::getParamValue(paramsMap, "componentCarrier"));
        }
        else if (compoType == "MultiObjCompo") {
            lptr = dynamic_cast <MilpComponent*> (
                new MultiObjCompo(this, compoName, paramsMap, portsMap, mMilpData, mTecEcoAnalysis, mModelFactory)
                );
            lptr->initMilpComponent();
            configureBusCarrier(lptr, CairnUtils::getParamValue(paramsMap, "componentCarrier"));
        }        
        else if (compoType == "" || compoType == "undefined")
        {
            Cairn_Exception cairn_error("Unkown type " + compoType + " for component " + compoName, -1);
            throw cairn_error;
        }
        else
        {
            cInfo() << "Try loading special component type " << compoType;

            const std::string UserMilpFilePath = CairnUtils::getParamValue(paramsMap, "file");
            f_MilpComponent UserMilp = LoadDllMilpComponent(UserMilpFilePath, compoType);
            if (UserMilp == nullptr)
            {
                cCritical() << "Unable to load library or component " << compoName + " of type " + compoType;
                Cairn_Exception cairn_error("Please Check that file exists and is in the PATH: " + UserMilpFilePath, -1);
                throw cairn_error;
            }
            MilpComponent* lptr_temp = dynamic_cast <MilpComponent*> (UserMilp(this, compoName, mMilpData, mTecEcoAnalysis, paramsMap, portsMap));
            lptr_temp->initMilpComponent(); 
        }
    }
    catch (...) {
        Cairn_Exception cairn_error("Error creating component " + compoName + " of type " + compoType, -1);
        throw cairn_error;
    }
    return lptr;
}

void OptimProblem::configureBusCarrier(MilpComponent* lptrBus, const std::string& carrierName)
{
    /* set EnergyCarrier (needed for case GUI) */
    EnergyVector* vEnergyVector = findChild<EnergyVector>(carrierName);
    if (vEnergyVector) {
        lptrBus->setMainCarrier(vEnergyVector);
    }
    else {
        Cairn_Exception cairn_error("Error creating Bus " + lptrBus->Name() + "There is no EnergyCarrier with name " + carrierName, -1);
        throw cairn_error;
    }
}

void OptimProblem::deleteComponent(MilpComponent* lptrComponent)
{
    if (lptrComponent) {
        try {
            lptrComponent->deleteCompoModel();
            delete lptrComponent;
        }
        catch (...) {
            throw Cairn_Exception("Error Deleting Compoenet!");
        }
    }
}

void OptimProblem::createDynamicIndicators()
{
    std::vector<std::string> prevUDINames = {};
 
    for (auto& indicator : mDynamicIndicatorsData) {        
        prevUDINames.push_back(indicator["name"]);
        DynamicIndicator* dynamicIndicator = new DynamicIndicator(this, indicator["name"], indicator["formula"], prevUDINames);
        mDynamicIndicators.push_back(dynamicIndicator);
    } 
}

void OptimProblem::computeDynamicIndicators(const int& aNsol) //Assumes that the simulation has finished (should be called after readSolution)
{       
    const InputParam::t_Indicators& vIndicators = mTecEcoAnalysis->getInputIndicators()->getIndicators();

    const double* optSol = mSolver->getOptimalSolution(aNsol);
    for (int i = 0; i < mDynamicIndicators.size(); i++) {
        std::string warningMessage = std::string("While parsing the formula (" + mDynamicIndicators[i]->getFormula() + ") of dynamic indicator " + mDynamicIndicators[i]->getName());
        //Link variables (symbols) to their values
        std::map<std::string, std::string> renamingMap = mDynamicIndicators[i]->variableRenamingMap();
        std::map<std::string, double*> varValueMap = mDynamicIndicators[i]->variableValueMap();
       
        for (auto& [varName, value] : renamingMap) {       
            bool isUserDefinedIndicator = false;
            //The variable is on of user-defined indicators
            for (int j = 0; j < mDynamicIndicators.size(); j++) {//innerForLoop
                if (CairnUtils::simplified(mDynamicIndicators[j]->getName()) == CairnUtils::simplified(value))
                {
                    if (j >= i) {
                        cCritical() << "User-defined indicators are computed in order. Cannot use " + mDynamicIndicators[j]->getName() + " to define indicator " + mDynamicIndicators[i]->getName();
                        break; //innerForLoop
                    }
                    *varValueMap[varName] = mDynamicIndicators[j]->compute();
                    isUserDefinedIndicator = true;
                }
            }
            //
            //user-defined indicator found
            if (isUserDefinedIndicator) {
                continue; //While Loop
            }
            //Case of compoName.varName
            else if (CairnUtils::split(value, '.').size() == 2) {
                std::string compoName = CairnUtils::split(value, '.')[0];
                std::string mipExpName = CairnUtils::split(value, '.')[1];
                //std::string mipExpLongName;
                //if (mTecEcoAnalysis) {
                //    mipExpLongName = mTecEcoAnalysis->EnvImpactLongName(mipExpName); //used for env impact indicators in case of components
                //}
                //Look for componenet
                std::map <std::string, std::vector<double>*> compoIndicatorsMap;
                //TecEco case
                if (compoName == mTecEcoAnalysis->Name()) 
                {
                    bool isFound = false;
                    for (auto& vIndicator : vIndicators) {
                        const std::string &tecEcoIndicatorShortName = vIndicator->getShortName();
                        if (CairnUtils::simplified(tecEcoIndicatorShortName) == CairnUtils::simplified(mipExpName))
                        {
                            //tecEcoIndicator_Itr.key() is the long name of the indicator e.g. "Net Present Value (Levelized Profit)" for "NPV"
                            *varValueMap[varName] = vIndicator->getValue();
                            isFound = true;
                            break;
                        }
                    }                    
                    if (!isFound)
                    {
                        cWarning() << warningMessage 
                            << ", indicator" << mipExpName << "of componenet" << compoName << "not found!";
                    }
                }
                //Other components
                else 
                {
                    bool isFoundComponent = false;
                    for (auto& [key, lptrComponent] : MilpComponents())
                    {
                        if (lptrComponent->Name() == compoName) {
                            const InputParam::t_Indicators& vIndicators = lptrComponent->compoModel()->getInputIndicators()->getIndicators();
                            bool isFoundIndicator = false;
                            for (auto& vIndicator : vIndicators) {
                                const std::string& indicatorShortName = vIndicator->getShortName();
                                if (CairnUtils::simplified(indicatorShortName) == CairnUtils::simplified(mipExpName)
                                    //|| CairnUtils::simplified(indicatorName) == CairnUtils::simplified(mipExpLongName)
                                    )
                                {
                                    *varValueMap[varName] = vIndicator->getValue();
                                    isFoundIndicator = true;
                                    break;//indicator
                                }
                            }
                            if (!isFoundIndicator)
                            {
                                cWarning() << warningMessage 
                                           << ", indicator" << mipExpName << "of componenet" << compoName << "not found!";
                            }
                            isFoundComponent = true;
                            break;//component
                        }
                    }
                    if (!isFoundComponent) {
                        cWarning() << warningMessage << ", componenet" << compoName << "not found!";
                    }
                }
            }
            else {
                cWarning() << warningMessage
                    << ", an invalid variable format detected ('" << value
                    << "'). Expected format is: ComponentName.VarName";

                break; //while loop
            }
        }
    }
}

f_MilpComponent OptimProblem::LoadDllMilpComponent(std::string Filename, std::string ModuleName)
{
#if defined(WIN32) || defined(_WIN32)
//  HINSTANCE hGetProcIDDLL = LoadLibrary("C:\\Documents and Settings\\User\\Desktop\\test.dll");
  LPWSTR ws = GS::ConvertToLPWSTR( Filename );
  HINSTANCE hGetProcIDDLL = LoadLibrary(ws);
#else
  wchar_t* ws = GS::ConvertToLPWSTR( Filename );
  void *hGetProcIDDLL = dlopen((const char*)ws, RTLD_NOW);
#endif

  delete[] ws; // caller responsible for deletion

  f_MilpComponent funci ;

  if (!hGetProcIDDLL) {
    cCritical() << "could not load the dynamic library " << (Filename) ;
    return nullptr;
  }

  // resolve function address here

#if defined(WIN32) || defined(_WIN32)
  funci = (f_MilpComponent)GetProcAddress(hGetProcIDDLL, "UserMilp");
#else
  funci = (f_MilpComponent)dlsym(hGetProcIDDLL, "UserMilp");
#endif

  if (!funci) {
    cCritical() << "could not locate the function" << ModuleName;
    return funci;
  }

  cInfo() << " INFO : LoadDllMilpComponent succeeded for : " << ModuleName;

  return funci;
}

void OptimProblem::createLinksToBus()
{
    /* The order is not necessary here. It is only to preserve a certain solution for existing studies */

    // Component -> Bus links
    for (auto& component : NonBusMilpComponents()) {
        createLinksToBus(component);
    }

    //TecEcoAnalysis -> Bus links
    TecEcoCompo* pTecEco = dynamic_cast<TecEcoCompo*>(mTecEcoAnalysis->parent());
    if (pTecEco) {
        createLinksToBus(pTecEco);
    }

    // Bus -> Bus links
    for (auto& bus : BusComponents()) {
        createLinksToBus(bus);
    }
}

void OptimProblem::createLinksToBus(MilpComponent* lptrComponent) 
{
    std::vector<MilpPort*> portsToRemove;

    /** Get list of ports for connections onto a Bus  */    
    for (auto& lptrport : lptrComponent->PortList()) {
    
        /* 
        * Alternative solutions: 
        * 1- set port carrier and linked bus at the creation of the port. 
        *    This requires to create EnergyVectors and Buses before creating MilpComponents.
        * 2- store initial port carrier and initial linked bus (from input study file) 
        *    inside MilpPort class at the creation of the port
        */

        t_mapParamData inputPortParam = lptrComponent->portData(lptrport->ID(), lptrport->Name()); /* port data from input study file (.json) */
        std::string portCarrierName = CairnUtils::getParamValue(inputPortParam, "Carrier");
        std::string linkedBusName = CairnUtils::getParamValue(inputPortParam, "LinkedComponent");

        EnergyVector* lptrEnergyVector = nullptr;
        if (!portCarrierName.empty()) {
            lptrEnergyVector = this->findChild<EnergyVector>(portCarrierName);
        }

        BusCompo* lptrLinkedBus = nullptr;
        if (!linkedBusName.empty()) {
            lptrLinkedBus =  this->findChild<BusCompo>(linkedBusName);
        }

        if (lptrLinkedBus)
        {
            const std::string errorMsg = "Error creating link from " + lptrComponent->Name() + " (port " + lptrport->Name() + ") to Bus "
                + linkedBusName;

            //Verify that Bus and port have the same carrier
            if (lptrLinkedBus->CarrierName() != portCarrierName) {
                throw Cairn_Exception(errorMsg + ". Bus and port must have the same carrier (EnergyVector)!", -1);
            }

            //The EnergyVector of the Bus is set (same value) for every link!
            if (lptrEnergyVector) {
                lptrLinkedBus->setMainCarrier(lptrEnergyVector);
            }
            else {
                throw Cairn_Exception(errorMsg+ ". EnergyVector " + lptrLinkedBus->CarrierName() + " does not exist!", -1);
            }

            //Set port EnergyVector 
            const bool isTecEco = (lptrComponent->objectType() == "TecEcoCompo");

            if (isTecEco) {
                // TecEcoCompo: set port carrier 
                lptrport->setCarrier(lptrEnergyVector);
            }
            else if (lptrport->getCarrier() != lptrEnergyVector) {
                // Other components: carrier must be already; verify that it is the same carrier for safety
                throw Cairn_Exception("Carrier for componenet " + lptrComponent->Name() + ", port " + lptrport->Name() 
                    + " doesn't match the carrier name from the study input file!", -1);
            }

            //Add bus port and create the corresponding link
            lptrLinkedBus->addLink(lptrComponent, lptrport);

        } 
        else {
            //A non-connected port or link is not well-set
            lptrport->setLinkedBus(nullptr);

            MilpComponent* lptrLinkedCompo = nullptr;
            if (linkedBusName != "") {
                lptrLinkedCompo = this->findChild<MilpComponent>(linkedBusName);

                if (lptrLinkedCompo != nullptr) {
                    //A non-Bus to a non-Bus link
                    std::string  errorMsg = "Error: it is not possible to connect a componenet directly to another component: "
                        + lptrComponent->Name() + " - " + linkedBusName + ". An intermediate NodeEquality bus should be used.";
                    Cairn_Exception cairn_error(errorMsg, -1);
                    throw cairn_error;
                }
                else
                {
                    //Linked component is not defined !
                    cInfo() << "The linked component " + linkedBusName + " to the port " + lptrComponent->Name()
                        + "." + lptrport->Name() + " is not defined (a Bus is expected). Link skipped !!";
                }
            }

            if (lptrport->IsDefaultPort()) {
                //A default port that is not connected
                if (lptrEnergyVector) {
                    lptrport->setCarrier(lptrEnergyVector);
                }
                else {
                    std::string defaultPortNames = "";
                    for (const auto& pair : lptrComponent->compoModel()->DefaultPorts()) {
                        if (defaultPortNames != "") defaultPortNames += ", ";
                        defaultPortNames += pair.first;
                    }
                    std::string errorMsg = "Error: the default port " + lptrport->ID() + " (" + lptrport->Name() + ") of component " 
                        + lptrComponent->Name() + " doesn't have an assigned carrier (EnergyVector)!\n\n" 
                        + "Possible solutions: \n"
                        + "- Move back the default ports of " + lptrComponent->Name() + " to their original positions, or\n"
                        + "- Change the values of \"id\" of the default ports in json file to " + defaultPortNames;
                    Cairn_Exception cairn_error(errorMsg, -1);
                    throw cairn_error;
                }
            }
            else {
                //A non-default port that is not connected => remove it ?!
                portsToRemove.push_back(lptrport);
            }
        }
    }

    // Remove after iteration — no iterator invalidation
    for (MilpPort* port : portsToRemove) {
        lptrComponent->removePort(port);
    }
}

void OptimProblem::createImportZEVariablesList()
{
    mListSubscribedVars.clear();
    for (auto& [key, lptr] : MilpComponents()) {
        if (!lptr->allDefaultPortsHaveCarriers()) {
            continue;
        }
         //Read time series        
        lptr->readTSVariablesFromModel();
        //register subscribed lists at OptimProblem level
        lptr->createImportListVars(mListSubscribedVars);
    }
}

void OptimProblem::createExportZEVariablesList()
{
    mListPublishedVars.clear();
    for (auto& [key, lptr] : MilpComponents()) { 
        //register published lists at OptimProblem level
        lptr->createExportListVars(mListPublishedVars);
    }

    //No need to export TecEco output timeseries 
    //TecEcoCompo* lptrTecEco = dynamic_cast<TecEcoCompo*> (mTecEcoAnalysis->parent());
    //if (lptrTecEco) {
    //    lptrTecEco->createExportListVars(mListPublishedVars);
    //}
}

void OptimProblem::computeGUIBusLocations()
{
    int Xbus = 100;
    int Ybus = 100;
    int MaxBusLength = 0;

    for (auto& lptrBus : BusComponents()) 
    {
        if (Xbus > 100) {
            Xbus += int(MAX_X / 4);

            if (Xbus > MAX_X)
            {
                Xbus = 120;
                Ybus += MaxBusLength;
            }
        }
        else//First Bus componenet
        {
            Xbus += 20;
            Ybus += 20;
        }
        
        int maxInOut = max(lptrBus->NbPorts(KPROD()), lptrBus->NbPorts(KCONS()));
        int BusLength = 80 * max(lptrBus->NbPorts(KDATA()), maxInOut);
        MaxBusLength = max(MaxBusLength, BusLength);

        if (lptrBus->getXpos() == 0) {
            lptrBus->setXpos(Xbus);
        }
        else {
            Xbus = lptrBus->getXpos();
        }

        if (lptrBus->getYpos() == 0) {
            lptrBus->setYpos(Ybus);
        }
        else {
            Ybus = lptrBus->getYpos();
        }
    }
}

void OptimProblem::computeGUIComponentLocations()
{
    for (auto& lptrBus : BusComponents()) 
    {
        int Xcompo = lptrBus->getXpos();
        int Ycompo = lptrBus->getYpos();

        for (auto& lptrCompo : lptrBus->ListComponent()) {
            bool bothAreZero = false;
            if (lptrCompo->getXpos() == 0 && lptrCompo->getYpos() == 0) {
                bothAreZero = true;
            }

            if (lptrCompo->getXpos() == 0) {
                lptrCompo->setXpos(Xcompo + OFFSET_X);
            }


            if (lptrCompo->getYpos() == 0) {
                lptrCompo->setYpos(Ycompo + OFFSET_Y);
            }

            if (bothAreZero) {
                Xcompo += OFFSET_X;
                Ycompo += OFFSET_Y;
            }
            else {
                if (lptrCompo->getXpos() > Xcompo && lptrCompo->getXpos() < Xcompo + 2 * OFFSET_X) {
                    Xcompo = lptrCompo->getXpos();
                }

                if (lptrCompo->getYpos() > Ycompo && lptrCompo->getYpos() < Ycompo + 2 * OFFSET_Y) {
                    Ycompo = lptrCompo->getYpos();
                }
            }
        }
    }
}

int OptimProblem::SaveFullArchitecture(const std::string& filename, const std::string& posAlgorithm)
{
    std::string vFileName = filename;
    if (vFileName == "") {
        vFileName = mStudyPathManager->getScenarioFile("_self.json", 0, false);
    }

    fs::path outputPath(vFileName);
    if (outputPath.extension() != ".json") {
        outputPath.replace_extension(".json");
    }
    vFileName = outputPath.string();

    std::ofstream file(vFileName);
    if (file.is_open()) {        
        bool generatePositions = false;
        for (auto& component : findChildren<MilpComponent>()) {
            //if there is a component with undefined coordinates
            if (component->getXpos() == 0 || component->getYpos() == 0) {
                generatePositions = true;
                break;
            }
        }
        if (generatePositions) {
            if (CairnUtils::toUpper(posAlgorithm) != "GRADIENT") {
                computeGUIBusLocations();
                computeGUIComponentLocations();
            }
            else {
                computeGUICompoAndBusLocations();
            }
        }
        ojson jsonOutputFile;
        jsonSaveDocument(jsonOutputFile);
        file << jsonOutputFile.dump(2);
        file.close();
    }
    else {
        cCritical() << "Error when saving self generated json File " << vFileName;
        return -1;
    }
   
    return 0;
}

void OptimProblem::computeGUICompoAndBusLocations() {
    cDebug() << " Computing automatic locations ... ";    
    std::map<int, std::string> indiceCompo;
    std::map<std::string, int> compoIndice;
    int nbCompo = MilpComponents().size();
    int i = 0;

    // création d'une matrice d'adjacence
    // index des composants pour pouvoir écrire la matrice
    cDebug() << "- Ecriture de la matrice d'adjacence";
    for (auto& [key, lptr] : MilpComponents()) {            
        indiceCompo[i] = key;
        compoIndice[key] = i;
        i++;
    }

    MatrixXf matAdj;
    matAdj=MatrixXf::Zero(nbCompo,nbCompo);
    for (auto& lptrBus : BusComponents()) {
        int comp = compoIndice[lptrBus->Name()];
        matAdj(comp,comp)=1;
        for (auto& compo : lptrBus->ListComponent()) {
            int comp2 = compoIndice[compo->Name()];
            matAdj(comp,comp2)=1;
            matAdj(comp2,comp)=1;
        }
    }

    cDebug()<<"Calcul des puissances de la matrice d'adjacence";

    // calcul des puissances de la matrice d'adjacence
    std::vector<MatrixXf> powMatAdj;
    MatrixXf matPow = MatrixXf::Identity(nbCompo,nbCompo);
    for(int i=0; i<nbCompo; i++){
        matPow = matPow*matAdj;
        powMatAdj.push_back (matPow.replicate(1,1));
    }
    cDebug()<<"Calcul des distances";
    // calcul des distances entre les noeuds
    MatrixXf distances = MatrixXf::Constant(nbCompo, nbCompo, nbCompo+1);
    for (int k=0; k<nbCompo; k++){
        for (int i=0; i<nbCompo; i++){
            distances(i,i)=0;
            for (int j=i; j<nbCompo; j++){
                if(powMatAdj[k](i,j)>0){
                    distances(i,j)=min(k+1,int(distances(i,j)));
                    distances(j,i)=min(k+1,int(distances(j,i)));
                }
            }
        }
    }

    int maxDist = distances.maxCoeff();

    // calcul de la fonction Energie
    cDebug()<<"Calcul de la fonction Energie";
    Eigen::MatrixXf positions = MatrixXf::Zero(nbCompo, 2);
    positions.setConstant(0); 
    int nIteration = 1000;  
    double gab = 0.001;
    for (int i = 0; i < nbCompo; i++) {
        positions(i, 0) = cos((double(i) / double(nbCompo)) * 2 * 3.14) * maxDist;
        positions(i, 1) = sin((double(i) / double(nbCompo)) * 2 * 3.14) * maxDist;
    }

    GradDescResult calcPos;
    cDebug()<<"Debut de la descente de gradient";
    calcPos = CairnUtils::GradientDescent(CairnUtils::Energy, &positions, &distances, nIteration, gab, 0.001, 0.001);
   
    MatrixXf newPos;
    if (calcPos.condition)
        newPos = calcPos.X[calcPos.iteration -1];
    else
        newPos = calcPos.X[calcPos.iteration -2];
    cDebug()<<"Fin de la descente de gradient";

    double shift = - min(0.,double(newPos.minCoeff()));
    for (auto& [key, lptrCompo] : MilpComponents()) {        
        if (lptrCompo->getXpos() == 0 && lptrCompo->getYpos() == 0)
        {
            double xPos = lptrCompo->getXpos();
            double yPos = lptrCompo->getYpos();

            if (xPos == 0) {
                lptrCompo->setXpos((newPos(compoIndice[key], 0) + shift) * 200 + 20);
            }
            if (yPos == 0) {
                lptrCompo->setYpos((newPos(compoIndice[key], 1) + shift) * 200 + 20);
            }
        }
    }
}

void OptimProblem::jsonSaveDocument (ojson &jsonOutputFile)
{    
    jsonOutputFile = ojson{      
        {"Components", ojson::array()},
        {"Links", ojson::array()},
        {"Groups", ojson::array()}
    };
    //Links should be before Components to set the name of Bus ports 
    jsonSaveGuiLinks(jsonOutputFile["Links"]);
    jsonSaveGuiComponents(jsonOutputFile["Components"]);  
    jsonSaveGuiGroups(jsonOutputFile["Groups"]);
}

void OptimProblem::jsonSaveGuiComponents(ojson &componentsArray)
{
    //TecEcoAnalysis
    if (mTecEcoAnalysis) mTecEcoAnalysis->jsonSaveGuiComponent(componentsArray);

    //EnergyVectors
    std::vector<EnergyVector*> vEnergyVectors = findChildren<EnergyVector>();
    for (auto& lptr : vEnergyVectors) {
        lptr->jsonSaveGuiComponent(componentsArray);
    }
    
    //Solver
    if(mSolver) mSolver->jsonSaveGuiComponent(componentsArray);

    //SimulationControl
    if (mSimulationControl) mSimulationControl->jsonSaveGuiComponent(componentsArray);

    //Other components
    for (auto& [key, lptrCompo] : MilpComponents()) {    
        std::string componentCarrier = "";
        if (lptrCompo->getMainCarrier()) componentCarrier = lptrCompo->getMainCarrier()->Name();
        lptrCompo->jsonSaveGuiComponent(componentsArray, componentCarrier, mTecEcoAnalysis->getLabelList()); //ideally, mTecEcoAnalysis should not be null
    }
}

void OptimProblem::jsonSaveGuiLinkNodes(ojson& linksArray, const std::string& compoName, 
    const std::string& compoPortName, const std::string& busName, const std::string& busPortName, 
    const int& compoX, const int& compoY, const int& busX, const int& busY)
{
    // Ensure linksArray is an array (important for push_back at the end)
    if (!linksArray.is_array()) {
        // turn null into array 
        if (linksArray.is_null()) {
            linksArray = ojson::array();
        }
        else {
            cDebug() << "linksArray is not an array; type=" << linksArray.type_name()
                << ". Re-initializing it to an array.";
            linksArray = ojson::array();
        }
    }

    ojson linkObject = ojson{
        {"tailNodeName", compoName},
        {"tailSocketName", compoPortName },
        {"headNodeName", busName},
        {"headSocketName", busPortName},
        {"listPoint", ojson::array()} };

    ojson &listPoints = linkObject["listPoint"];

    ojson pointObject1 = ojson{
        {"x", (compoX + busX) / 2},
        {"y", compoY}
    };    
    listPoints.push_back(pointObject1);

    ojson pointObject2 = ojson{
        {"x", (compoX + busX) / 2},
        {"y", busY}
    };    
    listPoints.push_back(pointObject2);
   
    linksArray.push_back(linkObject);
}

void OptimProblem::jsonSaveGuiLinks(ojson& linksArray)
{
    // Ensure linksArray is an array.
    if (!linksArray.is_array())
    {
        if (linksArray.is_null()) {
            linksArray = ojson::array();
        }
        else {
            cDebug() << "linksArray is not an array; type="
                << linksArray.type_name()
                << ". Re-initializing it to an array.";

            linksArray = ojson::array();
        }
    }

    // Loop on Bus components.
    const std::vector<BusCompo*> pBuses = findChildren<BusCompo>();

    for (const auto& pBus : pBuses)
    {
        if (!pBus) {
            cWarning() << "Encountered null BusCompo*; skipping.";
            continue;
        }

        const std::string busName = pBus->Name();

        int busX = 0;
        int busY = 0;

        try
        {
            busX = pBus->getXpos();
            busY = pBus->getYpos();
        }
        catch (...)
        {
            cWarning() << "Failed to get position for bus " << busName << "; skipping bus.";
            continue;
        }

        /*
         * Each bus must have unique port names.
         *
         * We keep track of all names already used by this bus.
         *
         * If an existing BusPortName() is duplicated, empty, or otherwise
         * unavailable, a new unique name is generated.
         */
        std::unordered_set<std::string> usedBusPortNames;

        int iNum = 0; // Inputs to the bus -> PortL*
        int oNum = 0; // Outputs from the bus -> PortR*
        int dNum = 0; // Data ports -> PortB*

        /* Loop on (Bus) ports:
        *  those are pointers to the ports of the componenets connected to the Bus
        *  Technically, the Bus doesn't have own ports.
        * 
        *  A Bus port means a link to the componenet that owns this port.
        */

        for (MilpPort* pPort : pBus->LinkedPorts())
        {
            if (!pPort) {
                cWarning() << "Null MilpPort* found in bus "  << busName << "; skipping port.";
                continue;
            }

            // -------------------------------------------------------------
            // Determine the expected port prefix according to direction.
            // -------------------------------------------------------------

            const auto direction = pPort->Direction();

            std::string portPrefix;
            int* portCounter = nullptr;
            std::string busPortPosition;

            if (direction == KPROD())
            {
                // Input to the Bus.
                portPrefix = "PortL";
                portCounter = &iNum;
                busPortPosition = Left();
            }
            else if (direction == KCONS())
            {
                // Output from the Bus.
                portPrefix = "PortR";
                portCounter = &oNum;
                busPortPosition = Right();
            }
            else
            {
                // Data port.
                portPrefix = "PortB";
                portCounter = &dNum;
                busPortPosition = Bottom();
            }

            // -------------------------------------------------------------
            // Get existing bus port name.
            // -------------------------------------------------------------

            std::string busPortName = pPort->BusPortName();

            /*
             * Keep the existing name if it is:
             *
             *   1. not empty
             *   2. not already used by another port of this bus
             *
             * Otherwise generate a new unique name.
             */
            const bool hasValidExistingName =
                !busPortName.empty() &&
                usedBusPortNames.find(busPortName) ==
                usedBusPortNames.end();

            if (!hasValidExistingName)
            {
                /*
                 * Generate a unique name.
                 *
                 * The do/while is important because the counter might
                 * generate a name that already exists.
                 */
                do
                {
                    busPortName = portPrefix + std::to_string((*portCounter)++);
                } while (usedBusPortNames.find(busPortName) !=
                    usedBusPortNames.end());

                // Store the generated name back into the port.
                pPort->setBusPortName(busPortName);

                cDebug() << "Assigned unique bus port name:" << busName << "->" << busPortName;
            }

            // Mark the name as used by this bus.
            usedBusPortNames.insert(busPortName);

            // -------------------------------------------------------------
            // Bus port position.
            // -------------------------------------------------------------

            pPort->setBusPortPosition(busPortPosition);

            // -------------------------------------------------------------
            // Component-side information.
            // -------------------------------------------------------------

            const std::string compoName = pPort->CompoName();
            const std::string compoPortName = pPort->Name();

            if (compoName.empty()) 
            {
                cWarning() << "A link to bus " << busName << " has empty component name; skipping link.";
                continue;
            }

            // -------------------------------------------------------------
            // Find the component.
            // -------------------------------------------------------------

            MilpComponent* pComponent = nullptr;

            if (mTecEcoAnalysis && compoName == mTecEcoAnalysis->Name())
            {
                pComponent = findChild<TecEcoCompo>(compoName);
            }
            else
            {
                pComponent = findChild<MilpComponent>(compoName);

                if (!pComponent) {
                    // The linked object may itself be a Bus.
                    pComponent =
                        findChild<BusCompo>(compoName);
                }
            }

            if (!pComponent) {
                cWarning()  << "Could not find component " << compoName
                    << " linked to bus " << busName << "; skipping link.";
                continue;
            }

            // -------------------------------------------------------------
            // Get component position.
            // -------------------------------------------------------------

            int compoX = 0;
            int compoY = 0;

            try  {
                compoX = pComponent->getXpos();
                compoY = pComponent->getYpos();
            }
            catch (...) {
                cWarning()
                    << "Failed to get position for component " << compoName
                    << "; skipping link to bus " << busName  << ".";
                continue;
            }

            // -------------------------------------------------------------
            // Consistency checks.
            // -------------------------------------------------------------

            const auto& linkedComponents = pBus->ListComponent();
            const bool busNamesMatch = (pPort->LinkedBusName() == busName);
            const bool compoLinkedToBus =
                (std::find(
                    linkedComponents.begin(),
                    linkedComponents.end(),
                    pComponent) != linkedComponents.end());

            if (!busNamesMatch || !compoLinkedToBus)
            {
                cWarning() << "Mismatch: the bus and the linked component "
                    "must be identical! Skip link between " << compoName
                    << " and " << busName << ".";
                continue;
            }

            // -------------------------------------------------------------
            // Write the link.
            // -------------------------------------------------------------

            try
            {
                jsonSaveGuiLinkNodes(linksArray, compoName, compoPortName,
                    busName, busPortName, compoX, compoY, busX, busY);
            }
            catch (...)
            {
                cWarning()
                    << "Failed to serialize link between component "
                    << compoName << " and bus " << busName << "; skipping link.";
                continue;
            }
        }
    }
}

void OptimProblem::jsonSaveGuiGroups(ojson& groupsArray) const
{
    groupsArray = ojson::array();

    for (const auto& group : mGroups)
    {
        ojson groupObject;

        // Basic fields
        groupObject["groupId"] = group.at("groupId");
        groupObject["groupName"] = group.at("groupName");
        groupObject["mainNodeName"] = group.at("mainNodeName");
        groupObject["minimized"] = (group.at("minimized") == "true");
        groupObject["borderColor"] = group.at("borderColor");

        // listNodeName: comma-separated -> JSON array
        ojson listNodeArray = ojson::array();

        if (auto it = group.find("listNodeName"); it != group.end())
        {
            const std::string& str = it->second;

            std::stringstream ss(str);
            std::string token;

            while (std::getline(ss, token, ','))
            {
                if (!token.empty())
                    listNodeArray.push_back(token);
            }
        }

        groupObject["listNodeName"] = std::move(listNodeArray);

        groupsArray.push_back(std::move(groupObject));
    }
}

std::string OptimProblem::getOptimDirection()
{
    /* Always Minimize ??!! */

    //if (mComponent["OptimDirection"] == "Maximize")
    //    return mComponent["OptimDirection"] ;

    return std::string("Minimize");
}

void OptimProblem::setMIPModel(MIPModeler::MIPModel* aModel)
{   
    // set global MIP model pointer
    mModel = aModel;
    mModel->setExternalModeler(mSolver->getExternalModeler());
    if (mTecEcoAnalysis) {
        mTecEcoAnalysis->setMIPModel(aModel);
    }

    for (auto& [key, lptr] : MilpComponents()) {    
        lptr->setMIPModel(aModel) ;
    }
    return ;
}

void OptimProblem::setObjective(MIPModeler::MIPExpression* aExpObjective)
{   // set global MIP objective pointer
    mExpObjective = aExpObjective ;
    return ;
}

void OptimProblem::setStopSignal(int *stopSignal){
    mSolver->setStopSignal(stopSignal);

}

int OptimProblem::initProblem()
{
    cInfo() << "Initializing problem: configuring ports and declaring parameters/timeseries...";

    if (mMilpData->iHMFuturSize() < mMilpData->timeshift())
    {
        const std::string msg = "Error in doInit of Cairn! timeShift " +
            std::to_string(mMilpData->timeshift()) +
            " should be < futursize " +
            std::to_string(mMilpData->iHMFuturSize()) + " !!";
        throw Cairn_Exception(msg, -1);
    }

    // Is order important ?!

    //Loop on EnergyVectors
    for (auto* pCarrier : EnergyVectors()) {
        if (pCarrier->initProblem() < 0) {
            throw Cairn_Exception("ERROR in initialization of carrier: " + pCarrier->Name(), -1);
        }
    }

    //Loop on Non-Bus components
    for (auto& pComponent : NonBusMilpComponents())
    {
        // read parameters then create and initialize MIP variables
        if (pComponent->initProblem() < 0) {
            throw Cairn_Exception("ERROR in initialization of component: " + pComponent->Name(), -1);
        }
    }

    // Loop on Bus components
    for (auto& pBus : BusComponents()) 
    {
        // read parameters then create and initialize MIP variables
        if (pBus->initProblem() < 0) {
            throw Cairn_Exception("ERROR in initialization of Bus component: " + pBus->Name(), -1);
        }
    }

    // init TecEcoAnalysis
    TecEcoCompo* pTecEco = dynamic_cast<TecEcoCompo*>(mTecEcoAnalysis->parent());
    if (!pTecEco
        || pTecEco->initPorts() < 0
        || pTecEco->initSubModelConfiguration() < 0)
    {
        throw Cairn_Exception("Error while configuring optim problem: TecEcoAnalysis is not well defined!", -1);
    }

    return 0;
}

void OptimProblem::redeclareEnvImpactParameters()
{
    for (auto& [key, lptrCompo] : MilpComponents())
    {
        lptrCompo->redeclareEnvImpactParameters();
    }
    // create input ZEvariable (associated to input time series) list by component, and register them at Problem level.
    createImportZEVariablesList(); //TODO: move to MilpComponent::initProblem so a component-related vars are published at the component creation
    createExportZEVariablesList();
}

int OptimProblem::initSubModelInput()
{
    cInfo() << "Setting/Reading parameters and timeseries values...";

    int ierr = 0;

    for (auto& [key, lptr] : MilpComponents()) {
        ierr = lptr->initSubModelInput(); // read parameters then create and initialize MIP variables
        if (ierr <0) {
            cCritical() << "ERROR in initialization of component ";
            return ierr ;
        }
    }

    //After checking ports, in particular defining the value of mVarType, now it is possible to publish port variables 
    for (auto& [key, lptr] : MilpComponents()) {
        lptr->createPortsExportListVars(mListPublishedVars);
    }

    TecEcoCompo* lptrTecEco = dynamic_cast<TecEcoCompo*> (mTecEcoAnalysis->parent());
    if (lptrTecEco) {
        ierr = lptrTecEco->initSubModelInput();
        if (ierr < 0) return ierr;
        lptrTecEco->createPortsExportListVars(mListPublishedVars); 
    }
    else {
        throw Cairn_Exception("Error while initializing optim problem: TecEcoAnalysis is not well defined!", -1);
    }

    return ierr ;
}

void OptimProblem::exportRHVariableInModel()
{
    // Export for all MILP components
    for (auto& [key, comp] : MilpComponents()) {
        comp->exportRHVariableInModel();
    }

    // Export for TecEcoCompo (parent of TecEcoAnalysis) -- TecEcoAnalysis is not expected to have Control IOs
    auto* tecEco = dynamic_cast<TecEcoCompo*>(mTecEcoAnalysis->parent());
    if (!tecEco) {
        //throw Cairn_Exception(
        //    "Error while initializing OptimProblem: TecEcoAnalysis has no valid TecEcoCompo parent.",
        //    -1
        //);
        cWarning() << "Error while initializing OptimProblem: TecEcoAnalysis has no valid TecEcoCompo parent.";
        return;
    }

    tecEco->exportRHVariableInModel();
}


//------------------------------------------------------------------------------
//  Build Problem
//------------------------------------------------------------------------------
void OptimProblem::buildComponentConstraints()
{
    cInfo() << "Constructing MILP component models and related-adding optimization constraints...";

    for (auto& [key, lptr] : MilpComponents()) { // for (auto& lptr : NonBusMilpComponents()) use ?

        BusCompo* lptrBus = dynamic_cast<BusCompo*> (lptr);
        if (lptrBus) continue;

        TecEcoCompo* lptrTecEco = dynamic_cast<TecEcoCompo*> (lptr);
        if (lptrTecEco) continue;

        try {
            lptr->buildProblem();  
        }
        catch (const Cairn_Exception& cairn_error) {
            throw cairn_error;
        }
    }
}

void OptimProblem::buildBusConstraints()
{
    // Build problem for the Buses that have own ports first because 
    // their expressions will be used by the other Buses

    // Collect buses
    // const std::vector<BusCompo*>& buses = BusComponents();

    //std::list<std::string> busNames;
    //for (const BusCompo* bus : buses) {
    //    if (bus) {
    //        busNames.push_back(bus->Name());
    //    }
    //}

    cInfo() << "Constructing Bus models and adding related-optimization constraints...";

    std::vector<BusCompo*> buses;
    std::list<std::string> busNames;
    for (auto& [key, compo] : MilpComponents()) {

        BusCompo* bus = dynamic_cast<BusCompo*> (compo);
        if (bus) {
            buses.push_back(bus);
            busNames.push_back(bus->Name());
        }
    }

    // Iterate and build problem for buses starting from those that don't depend on other buses
    std::unordered_set<std::string> processedBuses;

    const int maxIterations = busNames.size();
    int iteration = 0;
    while (!busNames.empty())
    {
        if (iteration++ >= maxIterations) {
            throw Cairn_Exception("Circular dependency detected between buses: unable to resolve build order!", -1);
        }

        for (BusCompo* bus : buses)
        {
            if (!bus) continue;

            bool shouldWait = false;
            for (const MilpPort* port : bus->LinkedPorts())
            {
                const BusCompo* portParent = dynamic_cast<const BusCompo*>(port->parent());
                if (!portParent) continue;
                if (!processedBuses.count(portParent->Name()))
                {
                    shouldWait = true;
                    break;
                }
            }

            if (!shouldWait)
            {
                bus->buildProblem();
                const std::string name = bus->Name();
                busNames.remove(name);
                processedBuses.insert(name);  // insert for unordered_set, not push_back
            }
        }
    }
}

void OptimProblem::computeObjectiveFunction(MIPModeler::MIPExpression& aObjective)
{
    if (mTecEcoAnalysis != nullptr) {
        aObjective = mTecEcoAnalysis->objectiveExpression();
    }
}

void OptimProblem::resetFlags() 
{
    //QMapIterator<std::string, MilpComponent*> icomponent(MilpComponents());
    //while (icomponent.hasNext())
    //{
    //    icomponent.next();
    //    MilpComponent* lptr = icomponent.value();
    //    lptr->resetFlags();
    //}
    if (mTecEcoAnalysis) {
        mTecEcoAnalysis->resetFlags();
    }
}

void OptimProblem::buildProblem()
{
    cInfo() << "  ";
    cInfo() << "Building problem...";

    // Component constraints
    buildComponentConstraints();

    // TecEco pre-simulation
    if (mTecEcoAnalysis)
    {
        try {
            mTecEcoAnalysis->computeTecEcoContribution();
            MilpComponent* tecEcoCompo = static_cast<MilpComponent*>(mTecEcoAnalysis->parent());
            if (tecEcoCompo)
            {
                tecEcoCompo->setBusFluxPortExpression();      // FlowBalanceBus
                tecEcoCompo->setBusSameValuePortExpression(); // SameValueBus
            }
        }
        catch (...) {
            throw Cairn_Exception("Error while computing pre-simulation expressions of TecEcoAnalysis!", -1);
        }
    }

    // Bus constraints
    buildBusConstraints();

    // TecEco model + objective
    if (mTecEcoAnalysis)
    {
        mTecEcoAnalysis->buildTecEcoModel();     // define behaviour model + variables
        computeObjectiveFunction(*mExpObjective); // set mObjective from TecEcoAnalysis
    }
}


void OptimProblem::solveProblem(std::string& optimLogFileName,  const int cycle, const std::map<std::string, bool> paramMap, const bool aExportResultsEveryCycle)
{
    std::string location = std::string(mStudyPathManager->getScenarioFile("", 0, false).c_str());
    
    int iCycle = cycle;
    if (iCycle == 0) iCycle = 1;

    //Add a speration line
    std::ofstream optimLogFile;
    if (iCycle == 1) {
        //write
        optimLogFile.open(optimLogFileName.c_str(), std::ofstream::out | std::ofstream::trunc);
    }
    else {
        //append
        optimLogFile.open(optimLogFileName.c_str(), std::ofstream::out | std::ofstream::app);
    }

    optimLogFile << "---------------------------------------" << (" Cycle " + std::to_string(iCycle)).c_str() << " ---------------------------------------\n";
    optimLogFile.close();

    if(aExportResultsEveryCycle)
        mSolver->SolveProblem(mModel, location, cycle, paramMap);
    else 
        mSolver->SolveProblem(mModel, location, 0, paramMap); //to avoid writing .lp every cycle 
}

std::string OptimProblem::getOptimisationStatus()
{
    return mSolver->getOptimisationStatus();
}

int OptimProblem::getInterpretedOptimStatus()
{
    std::string status = getOptimisationStatus();

    if (status == "Optimal" || status == "OptimalGlobal") {
        mOptimStatus = 0;
    }
    else if (status == "Best Feasible (TimeLimit Reached)"
        || status == "Best Feasible"
        || status == "Best Feasible (TreeMemoryLimit Reached)"
        || status == "OptimalLocal"
        || status == "Feasible"
        || status == "IntegerSolution") {
        mOptimStatus = 1;
    }
    else {
        mOptimStatus = 2; //No Solution
    }
    return mOptimStatus;
}

bool OptimProblem::getIsCheckConflicts()
{
    return mSolver->getIsCheckConflicts();
}

void OptimProblem::readSolution(int aNsol)
{
    /*
     NoteL computeAllIndicators assumes mSolver->getModelType() == GS::MIPMODELER()
    */

    Solver* solver = mSolver;

    // Get the optimal solution pointer once
    const double* vOptimalSolution = solver->getOptimalSolution(aNsol);

    // Iterate components; cache MilpComponents() result to avoid repeated calls
    auto compos = MilpComponents();
    for (auto& kv : compos) {
        auto* compo = kv.second;
        compo->compoModel()->computeAllIndicators(vOptimalSolution);
        compo->exportSubmodelIO(solver, aNsol);
    }
}

void OptimProblem::closeExpressions()
{
    //QMapIterator<std::string, MilpComponent*> icomponent(MilpComponents());
    //while (icomponent.hasNext())
    //{
    //    icomponent.next();
    //    MilpComponent* lptr = icomponent.value();
    //    lptr->closeExpressions();
    //}
    if (mTecEcoAnalysis) {
        mTecEcoAnalysis->closeExpressions();
    }
}

void OptimProblem::writeSolution(int n, std::map<std::string, std::vector<double>>& resultats)
{
    resultats.clear();

    Solver* solver = mSolver;

    // Cache optimal solution once
    const double* optimalSolution = solver ? solver->getOptimalSolution(n) : nullptr;

    // Cache MilpComponents() result to avoid repeated calls
    auto compos = MilpComponents();

    for (auto it = compos.begin(); it != compos.end(); ++it) {
        auto* compo = it->second;
        if (compo) 
            compo->compoModel()->writeSolution(optimalSolution, resultats);
    }
}

int OptimProblem::getNumberOfSolutions()
{
    if (mSolver)
        return mSolver->getNumberOfSolutions();
    else
        return 0;
}

void OptimProblem::prepareOptim()
{
    cInfo() << "  ";
    cInfo() << "Prepare problem...";

    initSubModelInput();

    exportRHVariableInModel();

    // create output ZEvariable (associated to add IO variables which are published to outside e.g. to Pegase) list by component, and register them at Problem level.
    //createExportZEVariablesList(); // This causes a problem for Pegase because the variables are exported in ModuleCairn::doInit()


    // update current absolute timestep and input variables due to TimeShifting
   // mMilpData->prepareOptim();

    for (auto& [key, lptr] : MilpComponents()) {
    
        lptr->prepareOptim();
    }
}

int OptimProblem::setParameters()
{
    int ierr = 0 ;
    for (auto& [key, lptr] : MilpComponents()) {
    
        //ierr = lptr->setParameters(); enlevé: il est présent deux fois !
        if (ierr <0) return ierr ;
    }
    return ierr ;
}

void OptimProblem::computeTecEcoEnvAnalysis(const int& aNsol)
{

    //-------------- Compute TecEco Indicators -----------------------------//
    //computeAllIndicators assumes mSolver->getModelType() == GS::MIPMODELER()
    mTecEcoAnalysis->computeAllIndicators(mSolver->getOptimalSolution(aNsol));

    //Case of GAMS
    /*
        ModelerInterface* pExternalModeler = aSolver->getExternalModeler();
        if (pExternalModeler != nullptr) {
            std::string gamsVarName = Name() + "_v_ObjContribution";
            const double* objective = aSolver->getOptimalSolution(aNsol, gamsVarName);
            if (objective != nullptr) {
                objectiveContribution = objective[0];
            }
            else {
                cDebug() << aSolver->getModelType() << "::Variable key: " << gamsVarName << " not defined in " << aSolver->getModelType() << " model";
            }
            delete objective;
        }
    */

    //-------------- Compute Dynamic Indicators -----------------------------//
    computeDynamicIndicators(aNsol); 
}

void OptimProblem::exportEnvImpactMassIndicators(const std::string& aFileName, const std::string& encoding)
{
    if (!mTecEcoAnalysis) return;

    // Determine output filename
    std::string fileName = aFileName.empty()
        ? "study_results_EnvImpactMass.csv"
        : aFileName;

    // Obtain possible env impact names 
    const t_list impactNames = mTecEcoAnalysis->getPossibleImpactNames();

    // -------------------------
    // Build headers + values
    // -------------------------
    std::string header1;     // Impact Name 
    std::string header2;     // Cumulative / Mass / EnvGrey
    std::string headerUnits; // Impact Unit 
    std::map<std::string, std::string> valuesMap;

    bool firstComponent = true;

    for (auto& [key, comp] : MilpComponents()) {
        if (!comp->EnvironmentModel())
            continue;

        if (firstComponent) {
            header1 = "Impact Name";
            header2 = "";
            headerUnits = "Unit";
        }

        std::string& row = valuesMap[comp->Name()];
        const auto& indicators = comp->compoModel()->getInputIndicators()->getIndicators();

        for (const auto& impact : impactNames) {
            double mass = 0.0;
            double grey = 0.0;
            bool foundMass = false; 
            bool foundGrey = false; 
            std::string unit;

            // Scan indicators for this impact
            for (const auto* ind : indicators) {
                const std::string& name = ind->getName();

                if (!CairnUtils::contains(name, impact))
                    continue;

                if (CairnUtils::contains(name, "Env impact mass")) {
                    mass = ind->getValue();
                    unit = ind->getUnit();
                    foundMass = true;
                }
                else if (CairnUtils::contains(name, "EnvGrey impact mass")) {
                    grey = ind->getValue();
                    unit = ind->getUnit();
                    foundGrey = true;
                }

                if (foundMass && foundGrey)
                    break;
            }

            if (foundMass || foundGrey) {
                if (firstComponent) {
                    header1 += ";" + impact + ";" + impact + ";" + impact;
                    header2 += ";Cumulative impact mass"
                        ";Env impact mass"
                        ";Embodied impact mass";
                    headerUnits += ";" + unit + ";" + unit + ";" + unit;
                }

                row += ";" + std::to_string(mass + grey)
                    + ";" + std::to_string(mass)
                    + ";" + std::to_string(grey);
            }

        }

        firstComponent = false;
    }

    // Nothing to export
    if (header1.empty()) {
        return;
    }

    // -------------------------
    // Open file
    // -------------------------
    std::ios_base::openmode mode = std::ios::out | std::ios::binary;
    std::fstream out;

    if (!CairnUtils::openFileForWriting(out, fileName, mode)) {
        return; //error ?!
    }

    // -------------------------
    // Write BOM if encoding is UTF-8
    // -------------------------
    if (encoding == "UTF-8") {
        writeUTF8BOM(out);
    }

    // -------------------------
    // Write table
    // -------------------------
    out << header1 << "\n";
    out << header2 << "\n";
    out << headerUnits << "\n";

    for (auto& [name, values] : valuesMap) {
        out << name << values << "\n";
    }

    out.close();
}

void OptimProblem::exportEnvImpactParameters(const std::string& aFileName,
    const std::string& encoding)
{
    if (!mTecEcoAnalysis)
        return;

    // Determine output filename
    const std::string fileName =
        aFileName.empty() ? "study_EnvImpactParameters.csv" : aFileName;

    // Obtain possible impact names
    const t_list impactNames = mTecEcoAnalysis->getPossibleImpactNames();

    // -------------------------
    // Build header + values
    // -------------------------
    std::string header;
    std::map<std::string, std::string> valuesMap;
    bool firstComponent = true;

    // Reserve space for header to avoid repeated reallocations
    header.reserve(256);

    for (auto& [key, comp] : MilpComponents())
    {
        if (!comp->EnvironmentModel())
            continue;

        const auto& model = comp->compoModel();
        if (!model)
            continue;

        if (firstComponent)
            header = "Component Name";

        std::string& row = valuesMap[comp->Name()];
        row.reserve(256); // avoid repeated reallocations

        const auto& envMap = model->getInputEnvImpactsParam()->getMapParams();
        const auto& cfgMap = model->getInputConfigEnvImpactsParam()->getMapParams();

        // Unified lambda to process both maps
        auto processMap = [&](const auto& paramMap)
            {
                for (const auto& [pkey, param] : paramMap)
                {
                    if (!param)
                        continue;

                    const std::string& paramName = param->getName();

                    bool matchesImpact = false;
                    for (const auto& impact : impactNames)
                    {
                        if (CairnUtils::contains(paramName, impact))
                        {
                            matchesImpact = true;
                            break;
                        }
                    }

                    if (!matchesImpact)
                        continue;

                    // First component => build header columns
                    if (firstComponent)
                    {
                        const std::string shortName =
                            mTecEcoAnalysis->EnvImpactShortName(paramName);
                        header += ";" + shortName;
                    }

                    // Append parameter value
                    row += ";" + param->toString();
                }
            };

        processMap(envMap);
        processMap(cfgMap);

        firstComponent = false;
    }

    // Nothing to export
    if (header.empty())
        return;

    // -------------------------
    // Open file
    // -------------------------
    std::ios_base::openmode mode = std::ios::out | std::ios::binary;
    std::fstream out;

    if (!CairnUtils::openFileForWriting(out, fileName, mode))
        return;

    // -------------------------
    // Write BOM if encoding is UTF-8
    // -------------------------
    if (encoding == "UTF-8")
        writeUTF8BOM(out);

    // -------------------------
    // Write table
    // -------------------------
    out << header << "\n";

    for (const auto& [name, values] : valuesMap)
        out << name << values << "\n";

    out.close();
}

void OptimProblem::exportPortEnvImpactParameters(const std::string& aFileName,
    const std::string& encoding)
{
    if (!mTecEcoAnalysis)
        return;

    // Determine output filename
    const std::string fileName =
        aFileName.empty() ? "study_PortEnvImpactParameters.csv" : aFileName;

    // Obtain possible impact names
    const t_list impactNames = mTecEcoAnalysis->getPossibleImpactNames();

    // -------------------------
    // Build header + values
    // -------------------------
    std::string header;
    std::map<std::string, std::string> valuesMap;

    header.reserve(256);
    bool firstPort = true;

    for (auto& [compKey, comp] : MilpComponents())
    {
        if (!comp->EnvironmentModel())
            continue;

        const auto& model = comp->compoModel();
        if (!model)
            continue;

        const auto& ports = comp->PortList();

        const auto& portMap = model->getInputPortImpactsParam()->getMapParams();
        const auto& cfgPortMap = model->getInputConfigPortImpactsParam()->getMapParams();

        // Unified lambda for processing both maps
        auto processMap = [&](const auto& paramMap,
            const std::string& portName,
            const std::string& fullName)
            {
                for (const auto& [pkey, param] : paramMap)
                {
                    if (!param)
                        continue;

                    const std::string& paramName = param->getName();

                    // Must match port name
                    if (!CairnUtils::contains(paramName, portName))
                        continue;

                    // Must match at least one impact
                    bool matchesImpact = false;
                    for (const auto& impact : impactNames)
                    {
                        if (CairnUtils::contains(paramName, impact))
                        {
                            matchesImpact = true;
                            break;
                        }
                    }

                    if (!matchesImpact)
                        continue;

                    // First port => build header columns
                    if (firstPort)
                    {
                        std::string shortName = mTecEcoAnalysis->EnvImpactShortName(paramName);
                        const auto parts = CairnUtils::split(shortName, '.');
                        header += ";" + (parts.size() > 1 ? parts[1] : shortName);
                    }

                    // Append parameter value
                    valuesMap[fullName] += ";" + param->toString();
                }
            };

        for (MilpPort* port : ports)
        {
            const std::string portName = port->Name();
            const std::string varName = port->Variable();
            const std::string fullName = comp->Name() + "." + portName;

            // First port -> build header base
            if (firstPort)
                header = "Port Name;Variable";

            // Initialize row with variable name
            std::string& row = valuesMap[fullName];
            row.reserve(128);
            row = ";" + varName;

            // Apply on all port impact parameters
            processMap(portMap, portName, fullName);
            processMap(cfgPortMap, portName, fullName);

            firstPort = false;
        }
    }

    // Nothing to export
    if (header.empty())
        return;

    // -------------------------
    // Open file
    // -------------------------
    std::ios_base::openmode mode = std::ios::out | std::ios::binary;
    std::fstream out;

    if (!CairnUtils::openFileForWriting(out, fileName, mode))
        return;

    // -------------------------
    // Write BOM if encoding is UTF-8
    // -------------------------
    if (encoding == "UTF-8")
        writeUTF8BOM(out);

    // -------------------------
    // Write table
    // -------------------------
    out << header << "\n";

    for (const auto& [name, values] : valuesMap)
        out << name << values << "\n";

    out.close();
}



ExportParameterRows OptimProblem::collectParameterData(const std::map<std::string, bool>& optionsMap)
{
    ExportParameterRows data;

    // Get options
    auto getOption = [&](const std::string& key, bool defaultValue) {
        auto it = optionsMap.find(key);
        return (it != optionsMap.end()) ? it->second : defaultValue;
    };

    const bool exportPortParameters = getOption("ExportPortParams", true);
    const bool showLabels = getOption("ShowLabels", true);

    // Collect label headers
    std::vector<std::string> labelList;
    if (showLabels && mTecEcoAnalysis) {
        labelList = mTecEcoAnalysis->getLabelList();
        data.labelHeaders = labelList;
    }

    // Collect Solver parameters
    if (mSolver) {
        CairnUtils::collectParameters(data.rows, mSolver->Name(),
            mSolver->getParameters(), optionsMap);
    }

    // Collect SimulationControl parameters
    if (mSimulationControl) {
        CairnUtils::collectParameters(data.rows, mSimulationControl->Name(),
            mSimulationControl->getParameters(), optionsMap);
    }

    // Collect TecEcoAnalysis parameters
    if (mTecEcoAnalysis) {
        CairnUtils::collectParameters(data.rows, mTecEcoAnalysis->Name(),
            mTecEcoAnalysis->getParameters(), optionsMap);
    }

    // Collect MILP component parameters
    for (auto& [key, comp] : MilpComponents()) {
        if (!comp) continue;

        CairnUtils::collectParameters(data.rows, comp->Name(),
            comp->getParameters(exportPortParameters),
            optionsMap,
            comp->getTimeSeriesNames(),
            labelList,
            comp->compoModel()->getLabelMap());
    }

    return data;
}

void OptimProblem::exportParameters(const std::string& aFileName, const std::string& encoding,
    const std::map<std::string, bool>& optionsMap, const std::map<std::string, 
    std::vector<ExtraParameterData>>& extraData)
{
    // Determine output filename 
    const std::string fileName = aFileName.empty()
        ? "study_parameters.csv"
        : aFileName;

    // Open file 
    std::fstream out;
    if (!CairnUtils::openFileForWriting(out, fileName,
        std::ios::out | std::ios::binary))
    {
        return; // silent failure behavior
    }

    // Write BOM only when needed
    if (encoding == "UTF-8") {
        writeUTF8BOM(out);
    }

    // Collect base parameter rows
    ExportParameterRows data = collectParameterData(optionsMap);

    // ------------------------------------------------------------
    // Build lookup map using reserve() to avoid rehashing
    // ------------------------------------------------------------
    std::unordered_map<std::pair<std::string, std::string>, size_t, PairHash> rowIndex;
    rowIndex.reserve(data.rows.size());

    for (size_t i = 0; i < data.rows.size(); ++i) {
        rowIndex.emplace(std::make_pair(data.rows[i].component, data.rows[i].parameter), i);
    }

    // ------------------------------------------------------------
    // Process extraData with minimal allocations
    // ------------------------------------------------------------
    data.extraHeaders.reserve(data.extraHeaders.size() + extraData.size());

    for (const auto& kv : extraData)
    {
        const std::string& columnName = kv.first;
        const auto& paramDataList = kv.second;

        // Add header once
        data.extraHeaders.push_back(columnName);

        // Process each extra parameter
        for (const ExtraParameterData& paramData : paramDataList)
        {
            auto it = rowIndex.find(std::make_pair(paramData.component, paramData.parameter));

            if (it != rowIndex.end()) {
                // Insert extra data without reallocating the row map
                data.rows[it->second].extraData.emplace(columnName, paramData.value);
            }
        }
    }

    // ------------------------------------------------------------
    // Write final CSV
    // ------------------------------------------------------------
    CairnUtils::writeParameterDataToCSV(out, data, optionsMap);

    out.close();
}

void OptimProblem::exportParameters_all_files(std::string aFileName, const std::string& encoding,
    const std::map<std::string, bool>& optionsMap, const std::map<std::string, 
    std::vector<ExtraParameterData>>& extraData)
{
    try
    {
        if (aFileName.empty()) {
            aFileName = mStudyPathManager->getScenarioFile("_Parameters.csv", 0, false);
        }

        // Export main parameters
        exportParameters(aFileName, encoding, optionsMap, extraData);

        // Precompute suffixes 
        constexpr const char* ENV_SUFFIX = "_EnvImpact.csv";
        constexpr const char* PORT_SUFFIX = "_PortEnvImpact.csv";

        // Build filenames once using reserved strings to avoid reallocations
        std::string envFile;
        envFile.reserve(aFileName.size() + 16);
        envFile = CairnUtils::replace(aFileName, ".csv", ENV_SUFFIX);

        std::string portEnvFile;
        portEnvFile.reserve(aFileName.size() + 20);
        portEnvFile = CairnUtils::replace(aFileName, ENV_SUFFIX, PORT_SUFFIX);

        // Export special files
        exportEnvImpactParameters(envFile, encoding);
        exportPortEnvImpactParameters(portEnvFile, encoding);
    }
    catch (...)
    {
        throw Cairn_Exception("Error while exporting parameters! ", -1);
    }
}


void OptimProblem::exportMultiObjFile(std::fstream& out, int aNsol, const bool showDescription)
{
    if (mTecEcoAnalysis) {
        for (auto& lptrBus : BusComponents()) {
            if (lptrBus->ModelClassName() == "ManualObjective") {
                MIPModeler::MIPExpression* expSubObjective = lptrBus->getMIPExpression("SubObjectiveExpression");
                double value = expSubObjective->evaluate(mSolver->getOptimalSolution(aNsol));
                if (showDescription)
                    CairnUtils::outputIndicator(out, lptrBus->Name(), "Subobjective", value, mTecEcoAnalysis->ObjectiveUnit(), "Subobjective", "Sub objective");
                else
                    CairnUtils::outputIndicator(out, lptrBus->Name(), "Subobjective", value, mTecEcoAnalysis->ObjectiveUnit(), "Subobjective");
            }
        }
    }
}

void OptimProblem::exportAllTecEcoEnvAnalysis(const std::string& resultFile, const std::string& range,
    bool showDescription, const std::string& encoding, bool isRollingHorizon, int aNsol)
{
    std::ios_base::openmode mode = std::ios::out | std::ios::binary;

    std::fstream out;
    if (!CairnUtils::openFileForWriting(out, resultFile, mode)) {
        throw Cairn_Exception("OptimProblem: couldn't open result file for writing: " + resultFile, -1);
    }

    // Write UTF‑8 BOM only when encoding is UTF‑8
    if (encoding == "UTF-8") {
        writeUTF8BOM(out);
    }

    // -----------------------------------------
    // Write header
    // -----------------------------------------
    out << "Model;Indicator;Value;Unit;Alias";

    const auto& labels = mTecEcoAnalysis->getLabelList();
    for (const auto& label : labels) {
        out << ";" << label;
    }

    if (showDescription) {
        out << ";Description";
    }

    out << "\n";

    const bool exportAll = mTecEcoAnalysis->ForceExportAllIndicators();

    // -----------------------------------------
    // Export TecEcoAnalysis indicators (Total)
    // -----------------------------------------
    InputParam* modelIndicators = mTecEcoAnalysis->getInputIndicators();
    const auto& indicators = modelIndicators->getIndicators();

    for (const auto* ind : indicators) {
        ind->Export(out, mTecEcoAnalysis->Name(), range, exportAll, showDescription);
    }

    // -----------------------------------------
    // Export component-level indicators
    // -----------------------------------------
    for (auto& [key, comp]: MilpComponents()) {
        comp->compoModel()->exportIndicators(out, comp->Name(), range, labels,
            showDescription, exportAll, isRollingHorizon);
    }

    // -----------------------------------------
    // Multi-objective results (PLAN only)
    // -----------------------------------------
    if (range == "PLAN") {
        exportMultiObjFile(out, aNsol, showDescription);
    }

    // -----------------------------------------
    // User-defined indicators (PLAN only)
    // -----------------------------------------
    if (range == "PLAN") {
        for (auto* dyn : mDynamicIndicators) {
            const std::string& name = dyn->getName();
            const double value = dyn->compute();

            if (showDescription) {
                CairnUtils::outputIndicator(out, "User-Defined", name,
                    value, "UNIT", name, "User-Defined Indicator");
            }
            else {
                CairnUtils::outputIndicator(out, "User-Defined", name,
                    value, "UNIT", name);
            }
        }
    }

    out.close();
}

void OptimProblem::exportResultsPLAN(std::string aResultFile, const int& aNsol)
{
    if (aResultFile == "") {
        aResultFile = mStudyPathManager->getScenarioFile("_PLAN.csv", aNsol);
    }

    bool isShowIndicatorDescription = false;
    if (mMilpData) {
        isShowIndicatorDescription = mMilpData->ShowIndicatorDescription(); 
    }

    exportAllTecEcoEnvAnalysis(aResultFile, "PLAN", isShowIndicatorDescription, "UTF-8", false, aNsol);
}

void OptimProblem::computeHistState()
{
    for (auto& [key, lptr] : MilpComponents()) {
        lptr->computeHistNbHours();
    }
    TecEcoCompo* pTecEco = dynamic_cast<TecEcoCompo*>(mTecEcoAnalysis->parent());
    pTecEco->computeHistNbHours();
}

void OptimProblem::populatePublishedVars()
{
    for (auto& [key, lptr] : MilpComponents()) {
        lptr->populatePublishedVars(mListPublishedVars);
    }
}

void OptimProblem::setDefaultsResults()
{
    for (auto& [key, lptr] : MilpComponents()) {
        lptr->setDefaultsResults();
    }
}

void OptimProblem::exportOptimaSizeAllCycles(const std::string& fileName, int cycle, const std::string& encoding)
{
    std::ios_base::openmode mode = std::ios::out | std::ios::binary;

    std::fstream out;
    if (!CairnUtils::openFileForWriting(out, fileName, mode)) {
        throw Cairn_Exception("OptimProblem: couldn't open file optimalSize.csv for writing.", -1);
    }

    // Write BOM only if encoding is UTF‑8
    if (encoding == "UTF-8") {
        writeUTF8BOM(out);
    }

    // -------------------------
    // Build header row
    // -------------------------
    std::string header = "";
    for (auto& comp : NonBusMilpComponents()) {
        const auto& sizes = comp->compoModel()->getOptimalSizeAllCycles();
        if (!sizes.empty()) {
            header += ";" + comp->Name();
        }
    }

    out << header << "\n";

    // -------------------------
    // Write each cycle row
    // -------------------------
    for (int i = 0; i < cycle; ++i) {
        std::string row = "Cycle " + std::to_string(i + 1);

        for (auto& comp : NonBusMilpComponents()) {
            const auto& sizes = comp->compoModel()->getOptimalSizeAllCycles();

            if (i < static_cast<int>(sizes.size())) {
                row += ";" + std::to_string(sizes[i]);
            }
            else if (CairnUtils::contains(header, comp->Name())) {
                // Component has some sizes but fewer than expected -> write empty cell
                // Usually, should not be the case!
                row += ";";
            }
        }

        out << row << "\n";
    }

    out.close();
}

std::string OptimProblem::getAbsoluteFileName(const std::string& filename)
{
    if (!fs::exists(filename)) {
        return (projectDir() + filename);
    }
    return filename;
}

std::vector<std::string> OptimProblem::GroupNames() const {
    std::vector<std::string> names{};
    names.reserve(mGroups.size());
    for (const auto& group : mGroups) {
        names.push_back(group.at("groupName"));
    }
    return names;
};

void OptimProblem::addGroup(const std::vector<std::string>& compoNames, 
    const std::string& mainCompo, const std::string& groupName)
{
    t_mapGroups groupData{};

    // buildGroupData() returns std::map<std::string, std::string>
    groupData = CairnUtils::buildGroupData(compoNames, GroupNames(), 
        mainCompo, groupName);

    mGroups.push_back(groupData);
}


int OptimProblem::checkVersion() const
{
    // --- Validate JSON study version -------------------------------------------------------
    if (mStudyVersion.empty()) 
    {
        cWarning() << "checkVersion() called with no study version loaded; skipping compatibility check.";
        return 0;
    }

    // --- Parse JSON study version -------------------------------------------------------
    const auto jsonParts = parseVersion(mStudyVersion);
    const int jsonMajor = (jsonParts.size() > 0 ? jsonParts[0] : 0);
    const int jsonMinor = (jsonParts.size() > 1 ? jsonParts[1] : 0);

    const std::string jsonBaseVersion =
        std::to_string(jsonMajor) + "." + std::to_string(jsonMinor);

    // --- Parse current Cairn version ----------------------------------------------
    const std::string currentRaw = CairnUtils::extractVersion(GS::Cairn_Release);
    const auto currentParts = CairnUtils::parseVersion(currentRaw);

    const int curMajor = (currentParts.size() > 0 ? currentParts[0] : 0);
    const int curMinor = (currentParts.size() > 1 ? currentParts[1] : 0);

    const std::string currentBaseVersion =
        std::to_string(curMajor) + "." + std::to_string(curMinor);

    // --- Compare ------------------------------------------------------------
    const int cmp = compareVersion(jsonParts, currentParts);

    if (cmp < 0)
    {
        cWarning() << "Compatibility script may be required: "
            << "study version " << jsonBaseVersion
            << " is older than current Cairn version " << currentBaseVersion;
    }
    else if (cmp > 0)
    {
        cWarning() << "Backward compatibility is not guaranteed: "
            << "study version " << jsonBaseVersion
            << " is more recent than current Cairn version " << currentBaseVersion;
    }

    return cmp;
}
