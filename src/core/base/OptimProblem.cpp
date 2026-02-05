#if (!defined(WIN32) && !defined(_WIN32))
#include <dlfcn.h>
#endif

#include "OptimProblem.h"
#include "MIPModeler.h"
#include "CairnUtils.h"
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

    mJsonDescription  = new JsonDescription (this, "JsonDescription");

    //Retrieve the list of private submodels
    mModelFactory = new ModelFactory(spdlog::default_logger());
    mModelFactory->findModels();

    //Default TecEcoAnalysis
    if (!createTecEcoAnalysis()) {
        Cairn_Exception cairn_error((std::string)"Error creating the default TecEcoAnalysis!", -1);
        throw cairn_error;
    }

    //Default Solver 
    if (!createSolver()) {
        Cairn_Exception cairn_error((std::string)"Error creating the default Solver!", -1);
        throw cairn_error;
    }

    //Default SimulationControl 
    if (!createSimulationControl()) {
        Cairn_Exception cairn_error((std::string)"Error creating the default SimulationControl!", -1);
        throw cairn_error;
    }

} // OptimProblem()

OptimProblem::~OptimProblem()
{
    //delete mTecEcoEnv; //Attention mTecEcoEnv == this so deleting it creates an infinite loop!
    //delete mTecEcoAnalysis; mTecEcoAnalysis == mCompoModel which is deleted in ~TecEcoComponent()
    delete mSolver;
    delete mSimulationControl;
    delete mJsonDescription;
    delete mModelFactory;

    // Avoid dangling pointer
    mTecEcoEnv = nullptr;
    mTecEcoAnalysis = nullptr;
    mSolver = nullptr;
    mSimulationControl = nullptr;
    mJsonDescription = nullptr;
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

void OptimProblem::setExtrapolationFactor() {
    if (!mTecEcoAnalysis || !mMilpData) return;

    TecEcoCompo* pTecEcoCompo = dynamic_cast<TecEcoCompo*>(mTecEcoAnalysis->parent());
    if (!pTecEcoCompo) return;

    double factor = 1.;
    if (mMilpData->UseExtrapolationFactor()) {
        if (mTecEcoAnalysis->NbYearInput() > 1) {
            factor = 1.;
        }
        else {
            if (mTecEcoAnalysis->LeapYearPos() == 1) {
                factor = 8784. / (mMilpData->npdt() * mMilpData->pdtHeure());
            }
            else {
                factor = 8760. / (mMilpData->npdt() * mMilpData->pdtHeure());
            }
        }
    }
    else {
        factor = 1.;
    }

    pTecEcoCompo->setExtrapolationFactor(factor);
    pTecEcoCompo->setLevelizationTable();
    pTecEcoCompo->setImpactLevelizationTable();
    pTecEcoCompo->setTableYearsHours();

    for (auto& [key, lptrCompo] : MilpComponents()) {        
        lptrCompo->setExtrapolationFactor(factor);
        lptrCompo->setLevelizationTable();
        lptrCompo->setImpactLevelizationTable();
        lptrCompo->setTableYearsHours();
    }
}

void OptimProblem::doInit(const StudyPathManager& aStudy, bool aLoad)
{
    mStudyFile = &aStudy;

    if (aLoad) {     
        /* Case of GUI */

        /* Initialization of Optimization problem from input Values */
        std::string vStudyFile = mStudyFile->archFile();
        try {
            cInfo() << " Use JSON input file ";
            mMilpComponents = mJsonDescription->parseJsonDescription(mStudyFile->archFile());
            mDynamicIndicatorsData = mJsonDescription->dynamicIndicators();

            if (mJsonDescription->getException().error() != 0)//It is never set. A Cairn_Exception is thrown directly inside JsonDescription.cpp
            {
                Cairn_Exception cairn_error("Fatal error parsing file " + vStudyFile, -1);
                throw cairn_error;
            }
        }
        catch (Cairn_Exception& cairn_err) {
            Cairn_Exception cairn_error("Fatal error parsing file " + vStudyFile, -1);
            cairn_error.setMessage(cairn_err.message());
            throw cairn_error;
        }
        catch (...) {
            Cairn_Exception cairn_error("Fatal error parsing file " + vStudyFile, -1);
            throw cairn_error;
        }

        // create components from Milp description map mMilpComponents
        try {
            createTecEcoAnalysisFromParamMap(); 
            createSimulationControlFromParamMap();
            createMilpComponentsFromParamMap();
            createLinksToBus();
            createDynamicIndicators();
        }
        catch (Cairn_Exception& cairn_error) {
            cCritical() << "Error while creating in components in OptimProblem!";
            throw cairn_error;
        }
    }

    setExtrapolationFactor();

    // init components and their models
    int ierr = 0;
    try {
        ierr = initProblem();
    }
    catch (Cairn_Exception& cairn_error) {
        cCritical() << "Error in initialization of OptimProblem !";
        throw cairn_error;
    }

    if (ierr < 0) {
        Cairn_Exception cairn_error((std::string)"Error in initialization of OptimProblem !", -1);
        throw cairn_error;
    }

    // create input ZEvariable (associated to input time series) list by component, and register them at Problem level.
    createImportZEVariablesList(); //TODO: move to MilpComponent::initProblem so a component-related vars are published at the component creation
    createExportZEVariablesList();
}

void OptimProblem::initOptimProblemFromTecEcoAnalysis()
{
    if (!mTecEcoAnalysis) return;

    mForceExportAllIndicators = mTecEcoAnalysis->ForceExportAllIndicators();

    TecEcoCompo* pTecEcoCompo = dynamic_cast<TecEcoCompo*>(mTecEcoAnalysis->parent());
    if (!pTecEcoCompo) return;

    pTecEcoCompo->setObjectiveUnit(mTecEcoAnalysis->ObjectiveUnit());
    pTecEcoCompo->setCurrency(mTecEcoAnalysis->Currency());
    pTecEcoCompo->setRange(mTecEcoAnalysis->Range());

    pTecEcoCompo->setNbYear(mTecEcoAnalysis->NbYear());
    pTecEcoCompo->setNbYearOffset(mTecEcoAnalysis->NbYearOffset());
    pTecEcoCompo->setNbYearInput(mTecEcoAnalysis->NbYearInput());
    pTecEcoCompo->setLeapYearPos(mTecEcoAnalysis->LeapYearPos());

    pTecEcoCompo->setDiscountRate(mTecEcoAnalysis->DiscountRate());
    pTecEcoCompo->setImpactDiscountRate(mTecEcoAnalysis->ImpactDiscountRate());
    pTecEcoCompo->setInternalRateOfReturn(mTecEcoAnalysis->InternalRateOfReturn());

    pTecEcoCompo->setEnvImpactsList(mTecEcoAnalysis->EnvImpactsList());
    pTecEcoCompo->setEnvImpactUnitsList(mTecEcoAnalysis->EnvImpactUnitsList());
    pTecEcoCompo->setEnvImpactsShortNamesList(mTecEcoAnalysis->EnvImpactShortNamesList());
    pTecEcoCompo->setEnvImpactCosts(mTecEcoAnalysis->EnvImpactCosts());

    setExtrapolationFactor();

    TecEcoEnv* tecEcoEnv = dynamic_cast<TecEcoEnv*>(pTecEcoCompo);
    if (tecEcoEnv) {
        mTecEcoEnv = tecEcoEnv;
        setCompoTecEcoEnvSettings(*mTecEcoEnv);
    }
}

void OptimProblem::setCompoTecEcoEnvSettings(TecEcoEnv& aTecEcoEnv) {
    for (auto& [key, lptrCompo] : MilpComponents()) {        
        lptrCompo->setTecEcoEnvSettings(aTecEcoEnv);
    }
}

bool OptimProblem::createTecEcoAnalysis()
{
    createComponent("TecEcoAnalysis");
    if (mTecEcoAnalysis) initOptimProblemFromTecEcoAnalysis();
    return (mTecEcoAnalysis != nullptr);
}

void OptimProblem::createTecEcoAnalysisFromParamMap()
{        
    mTecEcoAnalysis = nullptr;
    for (auto & component : mMilpComponents) {        
        if (component["type"] == "TecEcoAnalysis") {
            std::map < std::string, std::map<std::string, std::string> > ports = mJsonDescription->getCompoPortData(component["id"]);
            createComponent(component["type"], component, ports);
            if (mTecEcoAnalysis) {
                mTecEcoAnalysis->setLabelList(mJsonDescription->LabelList());
                initOptimProblemFromTecEcoAnalysis();
            }
            break;
        }
    }
}

void OptimProblem::createMilpComponentsFromParamMap()
{    
    for (auto& component : mMilpComponents)
    {        
        if (component["type"] == "TecEcoAnalysis" || component["type"] == "SimulationControl")
            continue; //already created
        if (component["type"] == "Solver") {
            //component["id"] is componenet name, and component["Solver"] is the name of the Solver: Cplex, Cbc, Highs, ...
            bool vOK = createSolver(component["id"], component);
            if (!vOK) {
                Cairn_Exception cairn_error("Error creating Solver " + component["id"] + ": cannot found solver asked " + component["Solver"], -1);
                throw cairn_error;
            }
        }
        else if (component["type"] == "EnergyVector") {
            //component["Type"] is the carrier type : Fluid, Electrical, Thermal
            bool vOK = createEnergyVector(component["id"], component["Type"], component); 
            if (!vOK) {
                Cairn_Exception cairn_error("Error creating EnergyVector " + component["id"], -1);
                throw cairn_error;
            }
        }
        else {
            std::map < std::string, std::map<std::string, std::string> > ports = mJsonDescription->getCompoPortData(component["id"]);
            createComponent(component["type"], component, ports);
        }
    }
}

bool OptimProblem::createComponent(const std::string& componentType, const std::map<std::string, std::string>& paramsMap,
    const std::map < std::string, std::map<std::string, std::string> >& portsMap)
{
    MilpComponent* lptr = nullptr;
    try {
        if (componentType == "TecEcoAnalysis")
        {
            // only one TecEco
            TecEcoCompo *pTecEco = findChild<TecEcoCompo>();
            if (pTecEco) removeChild(pTecEco);
            TecEcoEnv vTecoEnv = TecEcoEnv();
            lptr = dynamic_cast <MilpComponent*> (new TecEcoCompo(this, paramsMap, portsMap, mMilpData, vTecoEnv, mModelFactory));
            lptr->initMilpComponent();
            mTecEcoAnalysis = dynamic_cast<TecEcoAnalysis*> (lptr->compoModel());
        }
        else if (componentType == "Converter"
            || componentType == "Storage"
            || componentType == "PhysicalEquation"
            || componentType == "OperationConstraint") {
            lptr = dynamic_cast <MilpComponent*> (new MilpComponent(this, paramsMap.at("id"), mMilpData, *mTecEcoEnv, paramsMap, portsMap, mModelFactory));
            lptr->initMilpComponent(); 
        }    
        else if (componentType == "Grid")
        {
            lptr = dynamic_cast <MilpComponent*> (new GridCompo(this, paramsMap, portsMap, mMilpData, *mTecEcoEnv, mModelFactory));
            lptr->initMilpComponent();
        }
        else if (componentType == "SourceLoad") {
            lptr = dynamic_cast <MilpComponent*> (new SourceLoadCompo(this, paramsMap, portsMap, mMilpData, *mTecEcoEnv, mModelFactory));
            lptr->initMilpComponent();
        }
        else if (componentType == "BusFlowBalance" || componentType == "BusSameValue") {
            lptr = dynamic_cast <MilpComponent*> (new BusCompo(this, paramsMap, portsMap, mMilpData, *mTecEcoEnv, mModelFactory));
            lptr->initMilpComponent();
            /* set EnergyCarrier (needed for case GUI). When using API, carrier is set in OptimProblemAPI::create_Bus */
            configureBusCarrier(lptr, CairnUtils::getParam(paramsMap, "componentCarrier"));
        }
        else if (componentType == "MultiObjCompo") {
            lptr = dynamic_cast <MilpComponent*> (new MultiObjCompo(this, paramsMap, portsMap, mMilpData, *mTecEcoEnv, mModelFactory));
            lptr->initMilpComponent();
            configureBusCarrier(lptr, CairnUtils::getParam(paramsMap, "componentCarrier"));
        }        
        else if (componentType == "" || componentType == "undefined")
        {
            Cairn_Exception cairn_error("Unkown type for component " + paramsMap.at("id"), -1);
            throw cairn_error;
        }
        else
        {
            cInfo() << "Try loading special component type " << componentType;
            f_MilpComponent UserMilp = LoadDllMilpComponent(paramsMap.at("file"), componentType);
            if (UserMilp == nullptr)
            {
                cCritical() << "Unable to load library or component " << paramsMap.at("id") + " of type " + componentType;
                Cairn_Exception cairn_error("Please Check that file exists and is in the PATH: " + (paramsMap.at("file")), -1);
                throw cairn_error;
            }
            MilpComponent* lptr_temp = dynamic_cast <MilpComponent*> (UserMilp(this, paramsMap.at("id"), mMilpData, *mTecEcoEnv, paramsMap, portsMap));
            lptr_temp->initMilpComponent(); 
        }
    }
    catch (...) {
        Cairn_Exception cairn_error("Error creating component " + paramsMap.at("id") + " of type " + componentType, -1);
        throw cairn_error;
    }
    if (lptr && lptr->compoModel()) {
        // mJsonDescription->LabelMap() is [compoName, [label: value]]
        auto vItr = mJsonDescription->LabelMap().find(lptr->Name());
        if (vItr != mJsonDescription->LabelMap().end()) {
            lptr->compoModel()->setLabelMap(vItr->second); 
        }
    }
    return true;
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
                        cWarning() << warningMessage;
                        cWarning() << "Indicator" << mipExpName << "of componenet" << compoName << "not found!";
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
                                cWarning() << warningMessage;
                                cWarning() << "Indicator" << mipExpName << "of componenet" << compoName << "not found!";
                            }
                            isFoundComponent = true;
                            break;//component
                        }
                    }
                    if (!isFoundComponent) {
                        cWarning() << warningMessage;
                        cWarning() << "Componenet" << compoName << "not found!";
                    }
                }
            }
            else {
                cWarning() << warningMessage;
                cWarning() << value << " is not a valid variable format. It should be in the form ComponentName.VarName";
                break; //while loop
            }
        }
    }
}

bool OptimProblem::createSolver(const std::string& aName, const std::map<std::string, std::string>& paramMap)
{
    delete mSolver;
    mSolver = new Solver(this, aName, paramMap);
    if (mSolver->getException().error() == -1) {
        return false;
    }
    return (mSolver != nullptr);
}

void OptimProblem::createSimulationControlFromParamMap()
{
    bool found = false;    
    for (auto& component : mMilpComponents)
    {      
        if (component["type"] == "SimulationControl") {
            bool vOK = createSimulationControl(component["id"], component);
            if (!vOK) {
                Cairn_Exception cairn_error("Error creating SimulationControl " + component["id"], -1);
                throw cairn_error;
            }
            found = true;
        }
    }
    if (!found) {
        cWarning() << "No SimulationControl found. The default SimulationControl will be used.";
    }
}

bool OptimProblem::createSimulationControl(const std::string& mSimulationControlName, const std::map<std::string, std::string>& paramMap)
{
    delete mSimulationControl;
    mSimulationControl = new SimulationControl(this, mSimulationControlName, paramMap);
    //TODO: use mException
    if (mSimulationControl) {
        //Set MilpData from SimulationControl params
        mMilpData->setMilpDataFromSettings(mSimulationControl->getParameters(), mStdAloneMode); 
        setExtrapolationFactor();
        return true;
    }
    return false;
}

bool OptimProblem::createEnergyVector(const std::string& aName, const std::string& aType, const std::map<std::string, std::string> paramMap)
{
    EnergyVector* lptr_EV = nullptr;
    lptr_EV = new EnergyVector(this, aName, aType, paramMap);
    return (lptr_EV != nullptr);
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
    for (auto& component : NonBusMilpComponents())
    {
        createLinksToBus(component);
    }
    //Create TecEcoAnalysis links
    TecEcoCompo* pTecEco = dynamic_cast<TecEcoCompo*>(mTecEcoAnalysis->parent());
    if (pTecEco) {
        createLinksToBus(pTecEco);
    }
}

void OptimProblem::createLinksToBus(MilpComponent* lptrComponent) 
{
    /** Get list of ports for connections onto a Bus  */    
    for (auto& lptrport : lptrComponent->PortList()) {
    
        /* 
        * Alternative solutions: 
        * 1- set port carrier and linked bus at the creation of the port. 
        *    This requires to create EnergyVectors and Buses before creating MilpComponents.
        * 2- store initial port carrier and initial linked bus (from input study file) 
        *    inside MilpPort class at the creation of the port
        */
        std::map<std::string, std::string> inputPortParam = lptrComponent->portDataFromInputFile(lptrport->ID(), lptrport->Name()); /* port data from input study file (.json) */
        std::string portCarrierName = inputPortParam["Carrier"];
        std::string linkedBusName = inputPortParam["LinkedComponent"];

        EnergyVector* lptrEnergyVector = nullptr;
        if (portCarrierName != "") {
            lptrEnergyVector = this->findChild<EnergyVector>(portCarrierName);
        }

        BusCompo* lptrLinkedBus = nullptr;
        if (linkedBusName != "") {            
            lptrLinkedBus =  this->findChild<BusCompo>(linkedBusName);
        }

        if (lptrLinkedBus)
        {
            //Verify that Bus and port have the same carrier
            if (lptrLinkedBus->CarrierName() != portCarrierName) {
                std::string errorMsg = "Error creating link from " + lptrComponent->Name() + " (port " + lptrport->Name() + ") to Bus " 
                    + linkedBusName + ". Bus and port must have the same carrier (EnergyVector)!";
                Cairn_Exception cairn_error(errorMsg, -1);
                throw cairn_error;
            }

            //The EnergyVector of the Bus is set (same value) for every link!
            if (lptrEnergyVector != nullptr) {
                lptrLinkedBus->setMainCarrier(lptrEnergyVector);
            }
            else {
                std::string errorMsg = "Error creating link from " + lptrComponent->Name() + " (port " + lptrport->Name() + ") to Bus " 
                    + linkedBusName + ". EnergyVector " + lptrLinkedBus->CarrierName() + " does not exist!";
                Cairn_Exception cairn_error(errorMsg, -1);
                throw cairn_error;
            }

            //Set LinkedBus and EnergyVector of the port
            lptrport->setLinkedBus(lptrLinkedBus);
            lptrport->setCarrier(lptrEnergyVector);

            //Add bus port and create the corresponding link
            lptrLinkedBus->addPort(lptrport);
            lptrLinkedBus->addComponent(lptrComponent);
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
                    cWarning() << "The linked component " + linkedBusName + " to the port " + lptrComponent->Name()
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
                //A non-default port that is not connected => not needed
                lptrComponent->removePort(lptrport);
            }
        }
    }
}

void OptimProblem::createImportZEVariablesList()
{
    mListSubscribedVars.clear();
    for (auto& [key, lptr] : MilpComponents()) {
         //Read time series        
        lptr->readTSVariablesFromModel();
        //register subscribed lists at OptimProblem level
        lptr->createImportListVars(mListSubscribedVars);
    }

    //TecEco doesn't have timeseries
    //TecEcoCompo* lptrTecEco = dynamic_cast<TecEcoCompo*> (mTecEcoAnalysis->parent());
    //if (lptrTecEco) {
    //    lptrTecEco->readTSVariablesFromModel();
    //    lptrTecEco->createImportListVars(mListSubscribedVars);
    //}
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
        vFileName = mStudyFile->getScenarioFile("_self.json", 0, false);
    }

    fs::path vOutputFile(vFileName);
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
    calcPos = GradientDescent(Energy, &positions, &distances, nIteration, gab, 0.001, 0.001);
    MatrixXf newPos;
    if (calcPos.cond)
        newPos = calcPos.X[calcPos.iter-1];
    else
        newPos = calcPos.X[calcPos.iter-2];
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
        {"Links", ojson::array()}
    };
    //Links should be before Components to set the name of Bus ports 
    jsonSaveGuiLinks(jsonOutputFile["Links"]);
    jsonSaveGuiComponents(jsonOutputFile["Components"]);  
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
            cWarning() << "linksArray is not an array; type=" << linksArray.type_name()
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

void OptimProblem::jsonSaveGuiLinks(ojson &linksArray)
{
    //Loop on Bus components
    const std::vector<BusCompo*> pBuses = findChildren<BusCompo>();
    for (const auto& pBus : pBuses) 
    {
        if (!pBus) {
            cWarning() << "Encountered null BusCompo*; skipping.";
            continue;
        }

        const std::string busName = pBus->Name();
        int busX = 0, busY = 0;
        try {
            busX = pBus->getXpos();
            busY = pBus->getYpos();
        }
        catch (...) {
            cWarning() << "Failed to get position for bus " << busName << "; skipping bus.";
        }

        /* Loop on (Bus) ports: 
        *  those are pointers to the ports of the componenets connected to the Bus
        *  Technically, the Bus doesn't have own ports. 
        *  A Bus port means a link to the componenet that owns this port. 
        */
        
        int iNum = 0; // inputs ports (=> outputs of the Bus)
        int oNum = 0; // outputs      (=> inputs of the Bus)
        int dNum = 0; // data ports

        for (MilpPort* pPort : pBus->PortList()) 
        {
            if (!pPort) {
                cWarning() << "Null MilpPort* found in bus " << busName << "; skipping port.";
                continue;
            }
            // Bus side data
            std::string busPortName = pPort->BusPortName();
            std::string bPortName;
            std::string bPortPosition;

            const auto direction = pPort->Direction();
            if (direction == KPROD()) {// => Input to the Bus
                bPortPosition = Left();
                bPortName = "PortL" + std::to_string(iNum++);
            }
            else if (direction == KCONS()) {// => Output from the Bus
                bPortPosition = Right();
                bPortName = "PortR" + std::to_string(oNum++);
            }
            else {//KDATA()
                bPortPosition = Bottom();
                bPortName = "PortB" + std::to_string(dNum++);
            }
            
            if (busPortName.empty()) {//Case API
                busPortName = bPortName;
                pPort->setBusPortName(bPortName);
            }

            if (pPort->BusPortPosition().empty()) {//Expected
                pPort->setBusPortPosition(bPortPosition);
            }

            // Component side data
            const std::string compoName = pPort->CompoName();
            const std::string compoPortName = pPort->Name();

            if (compoName.empty()) {
                cWarning() << "A link to bus " << busName << " has empty component name; skipping link.";
                continue;
            }

            MilpComponent* pComponent = nullptr;
            if (compoName == mTecEcoAnalysis->Name()) {
                pComponent = dynamic_cast<MilpComponent*> (mTecEcoAnalysis);
            }
            else {
                pComponent = findChild<MilpComponent>(compoName);
            }

            if (!pComponent) {
                cWarning() << "Encountered null MilpComponent*; skipping.";
                continue;
            }

            int compoX = 0, compoY = 0;
            try {
                compoX = pComponent->getXpos();
                compoY = pComponent->getYpos();
            }
            catch (...) {
                cWarning() << "Failed to get position for component " << compoName << "; skipping link to bus " << busName << ".";
                continue;
            }

            // Consistency checks: port must actually be linked to this bus, and component must belong to this bus
            const auto& linkedComponents = pBus->ListComponent(); // component linked to pBus
            const bool busNamesMatch = (pPort->LinkedBusName() == busName);
            const bool compoLinkedToBus = (std::find(linkedComponents.begin(), linkedComponents.end(), pComponent) != linkedComponents.end());
            if (!busNamesMatch || !compoLinkedToBus) {
                cWarning() << "Mismatch: the bus and the linked component must be identical! "
                << "Skip link between " << compoName << " and " << busName << ".";
                continue;
            }


            // Write the link 
            try {
                jsonSaveGuiLinkNodes(linksArray, compoName, compoPortName, busName, busPortName, compoX, compoY, busX, busY);
            }
            catch (...) {
                cWarning() << "Failed to serialize link between component " << compoName << " and bus " << busName << "; skipping link.";
                continue;
            }
        }
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
    if (mMilpData->iHMFuturSize() < mMilpData->timeshift())
    {
        cCritical() << "DoInit timeShift " << mMilpData->timeshift() << " should be < futursize " << mMilpData->iHMFuturSize() << " !! ";
        Cairn_Exception cairn_error((std::string)"Error in doInit of Cairn!", -1);
        //throw cairn_error;
        return -1 ;
    }

    int ierr = 0 ;    

    //Loop on Non-Bus components
    for (auto& lptrCompo : NonBusMilpComponents()) 
    {
        ierr = lptrCompo->initProblem() ; // read parameters then create and initialize MIP variables
        if (ierr <0) {
            cCritical() << "ERROR in initialization of component : " << (lptrCompo->Name());
            return ierr ;
        }
    }

    // Loop on Bus components
    for (auto& lptrBus : BusComponents()) 
    {
        ierr = lptrBus->initProblem() ; // read parameters then create and initialize MIP variables
        if (ierr <0) {
            cCritical() << "ERROR in initialization of Bus component : " << (lptrBus->Name());
            return ierr ;
        }
    }

    // init TecEcoAnalysis
    TecEcoCompo* lptrTecEco = dynamic_cast<TecEcoCompo*> (mTecEcoAnalysis->parent());
    if (lptrTecEco) {
        ierr = lptrTecEco->initPorts();
        if (ierr < 0) return ierr;
        ierr = lptrTecEco->initSubModelConfiguration();
    }
    else {
        throw Cairn_Exception("Error while configuring optim problem: TecEcoAnalysis is not well defined!", -1);
    }

    return ierr ;
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
    int ierr = 0;

    for (auto& [key, lptr] : MilpComponents()) {
        ierr = lptr->initSubModelInput() ; // read parameters then create and initialize MIP variables
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

//------------------------------------------------------------------------------
//  Build Problem
//------------------------------------------------------------------------------
void OptimProblem::buildComponentConstraints()
{
    for (auto& [key, lptr] : MilpComponents()) {

        //Exclude Bus componenets
        BusCompo* lptrBus = dynamic_cast<BusCompo*> (lptr);
        TecEcoCompo* lptrTecEco = dynamic_cast<TecEcoCompo*> (lptr);
        if (lptrBus == nullptr && lptrTecEco == nullptr) {
            try {
                lptr->buildProblem(); // les bus doivent attendre que toutes les expressions soient ecrites !
            }
            catch (const Cairn_Exception& cairn_error) {
                throw cairn_error;
            }
        }
    }
}

void OptimProblem::buildBusConstraints()
{
    for (auto& [key, lptr] : MilpComponents()) {
        BusCompo* lptrBus = dynamic_cast<BusCompo*> (lptr) ;
        if (lptrBus) {
            lptrBus->buildProblem(); // les bus doivent attendre que toutes les expressions soient ecrites !
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
    std::string vStudyFile = std::string(mStudyFile->archFile().c_str());

    int ierr = initSubModelInput();

    // create output ZEvariable (associated to add IO variables which are published to outside e.g. to Pegase) list by component, and register them at Problem level.
    //createExportZEVariablesList(); // This causes a problem for Pegase because the variables are exported in ModuleCairn::doInit()

    if (ierr < 0) {
        throw Cairn_Exception("Error in OptimProblem init!", -1);
    }

    // Create Constraints
    cInfo() << "OptimProblem::buildComponentConstraints";
    buildComponentConstraints();

    // Compute PreSimulation TecEco expressions 
    if (mTecEcoAnalysis) {
        try {
            cInfo() << "optimProblem::computeAllContribution";
            mTecEcoAnalysis->computeAllContribution();

            //TecEcoAnalysis Model Interface at ports
            MilpComponent* pTecEcoCompo = dynamic_cast<MilpComponent*> (mTecEcoAnalysis->parent());
            if (pTecEcoCompo) {
                pTecEcoCompo->setBusFluxPortExpression();       /**  send flux expressions to FlowBalanceBus */
                pTecEcoCompo->setBusSameValuePortExpression();  /**  publish expression to SameValueBus */
            }
        }
        catch (...) {
            throw Cairn_Exception("ERROR in component setting the ports of TecEcoAnalysis", -1);
        }
    }

    // dans la version actuelle, on ne prevoit pas de connexion directe d'un composant a un autre.
    // la connexion n'est possible que par un convertisseur ou une relation d'agregation.

    // dans cette solution, les bus sont des agregateurs a potentiel identique, et assurent une somme de flux
    // les contributions sont des expressions calculees par tous les ports contributeurs.
    // la contrainte que les potentiels sont identiques est implicite, c'est porte par le vecteur energetique.
    // on passe d'un vecteur a un autre (== potentiel different) par un convertisseur.
    cInfo() << "OptimProblem::buildBusConstraints";
    buildBusConstraints();

    // Model component behaviour
    if (mTecEcoAnalysis)  {
        cInfo() << "buildModel: " << mTecEcoAnalysis->objectName();
        mTecEcoAnalysis->buildModel();     /**  define behaviour model and associated Variables */
        computeObjectiveFunction(*mExpObjective);  /** set the value of mObjective to the ObjectiveExpression from TecEcoAnalysis */
    }

    return ;
}

void OptimProblem::solveProblem(std::string& optimLogFileName,  const int cycle, const std::map<std::string, bool> paramMap, const bool aExportResultsEveryCycle)
{
    std::string location = std::string(mStudyFile->getScenarioFile("", 0, false).c_str());
    
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
    for (auto& [key, lptr] : MilpComponents()) {
    
        //computeAllIndicators assumes mSolver->getModelType() == GS::MIPMODELER()
        lptr->compoModel()->computeAllIndicators(mSolver->getOptimalSolution(aNsol));
        lptr->exportSubmodelIO(mSolver, aNsol);
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
    const double* optimalSolution = mSolver->getOptimalSolution(n);    

    for (auto& [key, lptr] : MilpComponents()) {    
        lptr->compoModel()->writeSolution(optimalSolution, resultats);
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

void OptimProblem::exportEnvImpactMassIndicators(const std::string& aFileName, const std::string& encoding) {
    //FileName
    std::string vFileName = aFileName;
    if (vFileName == "") {
        vFileName = "study_results_EnvImpactMass.csv";
    }
    //----------------- Generate the Table -----------------
    std::string headerNames1 = std::string("");
    std::string headerNames2 = std::string("");
    std::string headerUnits = std::string("");
    std::map<std::string, std::string> valuesMap;
    t_list impactNames;
    if(mTecEcoAnalysis) impactNames  = mTecEcoAnalysis->getPossibleImpactNames();
    bool first = true; //first componenet
    //loop over all componenets
    for (auto& [key, lptr] : MilpComponents()) {    
        //consider only componenets that have an Environment Model
        if (lptr->EnvironmentModel()) {
            if (first) {
                headerNames1 = "Impact Name";
                headerNames2 = "";
                headerUnits = "Unit";
            }
            valuesMap[lptr->Name()] = std::string("");
            //loop over the considered impacts: used only to have a good order (less efficent)
            for (int i = 0; i < impactNames.size(); i++) {
                double mass = 0.0;
                double grey = 0.0;
                //list of all indicators 
                const InputParam::t_Indicators& vIndicators = lptr->compoModel()->getInputIndicators()->getIndicators();
                for (auto& vIndicator : vIndicators) {
                    const std::string vIndicatorName = vIndicator->getName();
                    if (!CairnUtils::contains(vIndicatorName, impactNames[i])) {
                        continue;
                    }
                    else if (CairnUtils::contains(vIndicatorName, "Env impact mass")) {
                        mass = vIndicator->getValue();
                    }
                    else if (CairnUtils::contains(vIndicatorName, "EnvGrey impact mass")) {
                        grey = vIndicator->getValue();
                        if (first) {
                            headerNames1 += " ; " + impactNames[i] + " ; " + impactNames[i] + " ; " + impactNames[i];
                            headerNames2 += " ; Cumulative impact mass  ; Env impact mass ; EnvGrey impact mass";
                            std::string unit = vIndicator->getUnit();
                            headerUnits += " ; " + unit + " ; " + unit + " ; " + unit;
                        }
                        valuesMap[lptr->Name()] += " ; " + std::to_string(mass + grey) + " ; " + std::to_string(mass) + " ; " + std::to_string(grey);
                    }
                }               
            }
            first = false;
        }
    }

    //No Env Impact is selected or Environment Model is off for all componenets => Don't export the file !
    if (headerNames1 == std::string("")) {
        return;
    }

    //----------------- Open the File ----------------- 
    std::fstream FileOut;
    if (!CairnUtils::openFileForWriting(FileOut, vFileName)) {
        return; //error!
    }

    //-----------------  Write the Table ----------------- 
    FileOut << headerNames1 << std::endl;
    FileOut << headerNames2 << std::endl;
    FileOut << headerUnits << std::endl;
    
    for (auto &[key, value] : valuesMap) {
        FileOut << key << value << "\n";
    }

    FileOut.close();
}

void OptimProblem::exportEnvImpactParameters(const std::string& aFileName, const std::string& encoding) {
    //FileName
    std::string vFileName = aFileName;
    if (vFileName == "") {
        vFileName = "study_EnvImpactParameters.csv";
    }
    //----------------- Generate the Table -----------------
    std::string header = std::string("");
    std::map<std::string, std::string> valuesMap;
    std::vector<std::string> impactNames;
    if (mTecEcoAnalysis) impactNames = mTecEcoAnalysis->getPossibleImpactNames();
    bool first = true; //first componenet
    //loop over all componenets
    for (auto& [key, lptr] : MilpComponents()) {    
        //consider only componenets that have an Environment Model
        if (lptr->EnvironmentModel()) {
            if (first) {
                header = "Component Name";
            }
            valuesMap[lptr->Name()] = std::string("");

            std::map<std::string, InputParam::ModelParam*> paramMap = lptr->compoModel()->getInputEnvImpactsParam()->getMapParams();

            //loop over the considered impacts: used only to have a good order (less efficent)
            for (int i = 0; i < impactNames.size(); i++) {                            
                for (auto const& [key, param] : paramMap) {
                    if (!CairnUtils::contains(param->getName(), impactNames[i]))
                        continue;
                    if (first) {
                        std::string impactShortNames = mTecEcoAnalysis->EnvImpactShortName(param->getName());
                        header += " ; " + impactShortNames;
                    }
                    valuesMap[lptr->Name()] += std::string(" ; ") + param->toString();
                }                
            }
            first = false;
        }
    }

    //No Env Impact is selected or Environment Model is off for all componenets => Don't export the file !
    if (header == std::string("")) {
        return;
    }

    //----------------- Open the File ----------------- 
    std::fstream FileOut;
    if(!CairnUtils::openFileForWriting(FileOut, vFileName)) {
        return; //error!
    }

    
    //-----------------  Write the Table ----------------- 
    FileOut << header << std::endl;
    for (auto& [key, value] : valuesMap) {
        FileOut << key << value << "\n";
    }
    FileOut.close();
}

void OptimProblem::exportPortEnvImpactParameters(const std::string& aFileName, const std::string& encoding) {
    //FileName
    std::string vFileName = aFileName;
    if (vFileName == "") {
        vFileName = "study_PortEnvImpactParameters.csv";
    }
    //----------------- Generate the Table -----------------
    std::string header = std::string("");
    std::map<std::string, std::string> valuesMap;
    std::vector<std::string> impactNames;
    if (mTecEcoAnalysis) impactNames = mTecEcoAnalysis->getPossibleImpactNames();
    bool first = true; //first port
    //loop over all componenets
    for (auto& [key, lptr] : MilpComponents()) {    
        //consider only componenets that have an Environment Model
        if (lptr->EnvironmentModel()) {
            std::vector<MilpPort*> portList = lptr->PortList();
            //loop over all ports
            for (int p = 0; p < portList.size(); p++) {
                std::string portName = portList[p]->Name();
                std::string varName = portList[p]->Variable();
                std::string fullName = lptr->Name() + "." + portName;
                //
                if (first) {
                    header = "Port Name ; Variable";
                }
                valuesMap[fullName] = " ; " + varName; 

                std::map<std::string, InputParam::ModelParam*> paramMap = lptr->compoModel()->getInputPortImpactsParam()->getMapParams();

                //loop over the impacts: used only to have a good order (less efficent)
                for (int i = 0; i < impactNames.size(); i++) {
                    for (auto const& [key, param] : paramMap) {
                        if (!CairnUtils::contains(param->getName(), portName))
                            continue;
                        if (!CairnUtils::contains(param->getName(), impactNames[i]))
                            continue;
                        if (first) {
                            std::string impactShortNames = mTecEcoAnalysis->EnvImpactShortName(param->getName());
                            if (CairnUtils::split(impactShortNames, '.').size() > 1)
                                header += " ; " + CairnUtils::split(impactShortNames, '.')[1];
                            else
                                header += " ; " + impactShortNames;
                        }
                        valuesMap[fullName] += std::string(" ; ") + param->toString();
                    }                   
                }
                first = false;
            }
        }
    }

    //No Env Impact is selected or Environment Model is off for all componenets => Don't export the file !
    if (header == std::string("")) {
        return;
    }

    //----------------- Open the File ----------------- 
    std::fstream FileOut;
    if (!CairnUtils::openFileForWriting(FileOut, vFileName)) {
        return; //error!
    }
    
    //-----------------  Write the Table ----------------- 
    FileOut << header << std::endl;
    for (auto& [key, value] : valuesMap) {
        FileOut << key << value << "\n";
    }

    FileOut.close();
}

void OptimProblem::exportParameters(const std::string& aFileName, const std::map<std::string, bool>& optionsMap, const std::string& encoding) 
{
    //FileName
    std::string vFileName = aFileName;
    if (vFileName == "") {
        vFileName = "study_parameters.csv";
    }

    //Open the File
    std::fstream FileOut;
    if (!CairnUtils::openFileForWriting(FileOut, vFileName)) {
        return; //error!
    }

    //header
    auto getOption = [&](const std::string& key, bool defaultValue) {
        auto it = optionsMap.find(key);
        return (it != optionsMap.end()) ? it->second : defaultValue;
    };

    bool exportPortParameters = getOption("ExportPortParams", true);
    bool showMandatory = getOption("ShowMandatory", true);
    bool showLabels = getOption("ShowLabels", true);

    FileOut << "Component;Parameter;Value;Unit;Description";
    if (showMandatory) {
        FileOut << ";Mandatory"; 
    }
    if (showLabels) {
        for (auto const& label : mTecEcoAnalysis->getLabelList()) {
            FileOut << ";" + label;
        }
    }
    FileOut << std::endl;

    //Solver 
    if (mSolver) {
        exportParameters(FileOut, mSolver->Name(), mSolver->getParameters(), optionsMap);
    }

    //SimulationControl
    if (mSimulationControl) {
        exportParameters(FileOut, mSimulationControl->Name(), mSimulationControl->getParameters(), optionsMap);
    }

    //TecEcoAnalysis 
    if (mTecEcoAnalysis) {
        exportParameters(FileOut, mTecEcoAnalysis->Name(), mTecEcoAnalysis->getParameters(), optionsMap);
    }

    //MilpComponents
    for (auto& [key, lptr] : MilpComponents()) {
        if (lptr) {
            exportParameters(FileOut, lptr->Name(), lptr->getParameters(exportPortParameters), optionsMap, lptr->getTimeSeriesNames(), lptr->compoModel()->getLabelMap());
        }
    }
    FileOut.close();
}

void OptimProblem::exportParameters_all_files(std::string aFileName, const std::map<std::string, bool>& optionsMap, const std::string& encoding)
{
    try {
        if (aFileName == "") {
            aFileName = mStudyFile->getScenarioFile("_Parameters.csv", 0, false);
        }
        exportParameters(aFileName, optionsMap, encoding);
        //Env Impacts coeff and results - special files
        const std::string suffix = "_EnvImpact.csv";
        exportEnvImpactParameters(CairnUtils::replace(aFileName, ".csv", suffix), encoding);
        exportPortEnvImpactParameters(CairnUtils::replace(aFileName, suffix, "_PortEnvImpact.csv"), encoding);
    }
    catch (...) {
        Cairn_Exception error("Error while exporting parameters! ", -1);
        throw error;
    }
}

void OptimProblem::exportParameters(std::fstream& out, const std::string& name, const std::map<std::string, InputParam::ModelParam*>& paramMap,
    const std::map<std::string, bool>& optionsMap, const std::map<std::string, std::string>& aTimeSeriesNames, const std::map<std::string, std::string>& labelMap)
{
    auto getOption = [&](const std::string& key, bool defaultValue) {
        auto it = optionsMap.find(key);
        return (it != optionsMap.end()) ? it->second : defaultValue;
    };

    bool onlyIsUsed = getOption("OnlyUsedParams", false);
    bool showMandatory = getOption("ShowMandatory", true);
    bool showLabels = getOption("ShowLabels", true);

    for (auto const& [key, param] : paramMap) {
        //Only used parameters (and optional timeseries if value is not empty)
        if (!onlyIsUsed || param->IsUsed() || (aTimeSeriesNames.find(key) != aTimeSeriesNames.end() && aTimeSeriesNames.at(key) != ""))
        {
            std::string  value = "";
            if (aTimeSeriesNames.find(key) != aTimeSeriesNames.end()) {
                value = aTimeSeriesNames.at(key);
            }
            else {
                value = param->toString();
            }
            out << name << ";" << key << ";" << value << ";" << param->getUnit() << ";" << param->getDescription();
            if (showMandatory) {
                std::string mandatory = param->IsBlocking() ? "true" : "false";
                out << ";" + mandatory;
            }
            if (showLabels) {
                for (auto const& label : mTecEcoAnalysis->getLabelList()) {
                    std::string val = "";
                    auto vIter = labelMap.find(label);
                    if (vIter != labelMap.end()) {
                        val = vIter->second;
                    }
                    out << ";" + val;
                }
            }
            out << std::endl;
        }
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

void OptimProblem::exportAllTecEcoEnvAnalysis(const std::string& aResultFile, const std::string& range, const bool showDescription, const std::string& encoding, const bool isRollingHorizon, const int aNsol)
{
    std::fstream FileOut;
    if (!CairnUtils::openFileForWriting(FileOut, aResultFile)) {
            Cairn_Exception cairn_error("OptimProblem: couldn't open result file for writing : "+ aResultFile, -1);
            throw cairn_error;
     }

    FileOut << "Model" << ";Indicator" << ";Value" << ";Unit" << ";Alias"; 
    for (auto const& label : mTecEcoAnalysis->getLabelList()) {
        FileOut << ";"+label;
    }
    if (showDescription) FileOut << ";Description";
    FileOut << std::endl;

    //TecEco
    InputParam* modelIndicators = mTecEcoAnalysis->getInputIndicators();
    const InputParam::t_Indicators& vIndicators = modelIndicators->getIndicators();
    for (size_t i = 0; i < vIndicators.size(); i++) {
        vIndicators[i]->Export(FileOut, mTecEcoAnalysis->Name(), range, mForceExportAllIndicators, showDescription);
    }

    // DETAILED DATA; BY COMPONENTS 
    for (auto& [key, lptr] : MilpComponents()) {
    
        lptr->setRange(std::string(range.c_str()));
        lptr->compoModel()->exportIndicators(FileOut, lptr->Name(), range, mTecEcoAnalysis->getLabelList(), 
            showDescription, mForceExportAllIndicators, isRollingHorizon);
    }

    //Add multiObj results
    if (range == "PLAN") {
        exportMultiObjFile(FileOut, aNsol, showDescription);
    }

    //User-defined indicators
    if (range == "PLAN") {
        for (int i = 0; i < mDynamicIndicators.size(); i++) {
            if(showDescription)
                CairnUtils::outputIndicator(FileOut, "User-Defined", mDynamicIndicators[i]->getName(), mDynamicIndicators[i]->compute(), "UNIT", mDynamicIndicators[i]->getName(), "User-Defined Indicator");
            else 
                CairnUtils::outputIndicator(FileOut, "User-Defined", mDynamicIndicators[i]->getName(), mDynamicIndicators[i]->compute(), "UNIT", mDynamicIndicators[i]->getName());
        }
    }

    FileOut.close();
}

void OptimProblem::exportResultsPLAN(std::string aResultFile, const int& aNsol)
{
    if (aResultFile == "") {
        aResultFile = mStudyFile->getScenarioFile("_PLAN.csv", aNsol);
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

void OptimProblem::exportResults()
{
    for (auto& [key, lptr] : MilpComponents()) {
        lptr->exportResults(mListPublishedVars);
       // lptr->exportResults();
    }
}

void OptimProblem::setDefaultsResults()
{
    for (auto& [key, lptr] : MilpComponents()) {
        lptr->setDefaultsResults();
    }
}

void OptimProblem::exportOptimaSizeAllCycles(const std::string& aFileName, const int cycle, const std::string &encoding)
{
    std::fstream FileOut;
    if (!CairnUtils::openFileForWriting(FileOut, aFileName)) {
        Cairn_Exception cairn_error((std::string)"OptimProblem: couldn't open file optimalSize.csv for writing.", -1);
        throw cairn_error;
    }
   
    FileOut << "sep=;\n";

    std::string header = "";
    for (auto& [key, lptr] : MilpComponents()) {
        if (lptr->compoModel()->getOptimalSizeAllCycles().size() > 0) { 
            header += ";" + lptr->Name();
        }
        else {
            //Must be a Bus component. In case of Bus, mOptimalSizeAllCycles is empty. 
        }
    }
    FileOut << header << std::endl;
    
    for (int i = 0; i < cycle; i++) {
        std::string optimalSizeValues = "Cycle " + std::to_string(i + 1);
        for (auto& [key, lptr] : MilpComponents()) {
            if (i < lptr->compoModel()->getOptimalSizeAllCycles().size()) {
                optimalSizeValues += ";" + std::to_string((lptr->compoModel()->getOptimalSizeAllCycles())[i]);
            }
            else if (CairnUtils::contains(header, lptr->Name())) {            
                //This means that there is a non-Bus component with mOptimalSizeAllCycles.size > 0 but < nCycles. 
                //If happened then there must be something wrong!
                optimalSizeValues += ";"; 
            }
        }
        FileOut << optimalSizeValues << "\n";
    }
}

std::string OptimProblem::getAbsoluteFileName(const std::string& filename)
{
    if (!fs::exists(filename)) {
        return (projectDir() + filename);
    }
    return filename;
}
