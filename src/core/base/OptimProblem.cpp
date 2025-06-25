#if (!defined(WIN32) && !defined(_WIN32))
#include <dlfcn.h>
#endif

#include "OptimProblem.h"
#include "CairnUtils.h"
using namespace CairnUtils;

using Eigen::Map;
using namespace GS ;

static const QSettings::Format JsonFormat = QSettings::registerFormat("json", &readSettingsJson, &writeSettingsJson);

OptimProblem::OptimProblem(QObject* aParent, QString aName, MilpData* aMilpData, TecEcoEnv &aTecEcoEnv, const QMap<QString, QString> &aComponent) : 
    MilpComponent(aParent, aName, aMilpData, aTecEcoEnv, aComponent, {}),
    mSolver(nullptr),
    mSimulationControl(nullptr),
    mStdAloneMode(true),
    mExportIndicators(true), //used for TecEco indicators
    mOptimStatus(2)
{
    this->setObjectName(aName);
    mName = aName ;

    mJsonDescription  = new JsonDescription (this, "JsonDescription");

    mComponent["type"] = "SYSTEM";
    setCompoInputParam(mComponent);

    mTecEcoEnv = this ;

    //Retrieve the list of private submodels
    mModelFactory = new ModelFactory();
    mModelFactory->findModels();

    mGUIData = new GUIData (aParent, aName) ;
    mGUIData->doInit("OptimProblem", "OptimProblem", "OptimProblem", 100, 10);
    if (mComponent["Xpos"] == "")
        mGUIData->setXpos(mGUIData->GetId()) ;

    if (mComponent["Ypos"] == "")
        mGUIData->setYpos(10) ; 

    //Default TecEcoAnalysis
    createCompoModel();

    //Default Solver 
    if (!createSolver()) {
        Cairn_Exception cairn_error("Error creating the default Solver!", -1);
        throw cairn_error;
    }

    //Default SimulationControl 
    if (!createSimulationControl()) {
        Cairn_Exception cairn_error("Error creating the default createSimulationControl!", -1);
        throw cairn_error;
    }

} // OptimProblem()

OptimProblem::~OptimProblem()
{
    if (mSolver) delete mSolver;
    if (mSimulationControl) delete mSimulationControl;
    if (mJsonDescription) delete mJsonDescription ;
    if (mModelFactory) delete mModelFactory;

    QMapIterator<QString, ZEVariables*> iPublishedVariable(mListPublishedVars);
    while (iPublishedVariable.hasNext())
    {
        iPublishedVariable.next();
        ZEVariables* var = iPublishedVariable.value();
        if (var) delete var;
        var = nullptr;
    }
    mListPublishedVars.clear();    
    mListSubscribedVars.clear();

    QMapIterator<QString, MilpComponent*> icomponent(mMapMilpComponents);
    while (icomponent.hasNext())
    {
        icomponent.next();
        MilpComponent* lptr = icomponent.value();
        if (lptr) delete lptr;
    }
    mMapMilpComponents.clear();

    QMapIterator<QString, EnergyVector*> iNRJ(mMapEnergyVectors);
    while (iNRJ.hasNext())
    {
        iNRJ.next();
        EnergyVector* lptr = iNRJ.value();
        if (lptr) delete lptr;
    }
    mMapEnergyVectors.clear();

    for (size_t i = 0;i < mDynamicIndicators.size();i++) {
        if (mDynamicIndicators[i]) delete mDynamicIndicators[i];
    }
    mDynamicIndicators.clear();
}

//------------------------------------------------------------------------------
//  init Problem
//------------------------------------------------------------------------------

void OptimProblem::setExtrapolationFactor() {
    if (mMilpData->UseExtrapolationFactor()) {
        if (mNbYearInput > 1) {
            mExtrapolationFactor = 1.;
        }
        else {
            if (mLeapYearPos == 1) {
                mExtrapolationFactor = 8784. / (mMilpData->npdt() * mMilpData->pdtHeure());
            }
            else {
                mExtrapolationFactor = 8760. / (mMilpData->npdt() * mMilpData->pdtHeure());
            }
        }
    }
    else {
        mExtrapolationFactor = 1.;
    }

    this->setLevelizationTable();
    this->setImpactLevelizationTable();
    this->setTableYearsHours();

    QMapIterator<QString, MilpComponent*> icomponent(mMapMilpComponents);
    while (icomponent.hasNext())
    {
        icomponent.next();
        MilpComponent* lptrCompo = icomponent.value();
        lptrCompo->setExtrapolationFactor(mExtrapolationFactor);
        lptrCompo->setLevelizationTable();
        lptrCompo->setImpactLevelizationTable();
        lptrCompo->setTableYearsHours();
    }
}

void OptimProblem::doInit(const StudyPathManager& aStudy, bool aLoad)
{
    mStudyFile = &aStudy;
    if (!aLoad) {
        // RAZ config file (use current configuration in memory)        
        
        /* Initialization of Optimization problem from input Values */

        //mMilpData->setMilpDataFromSettings(mStdAloneMode);
        setExtrapolationFactor();
        int ierr3 = 0;
        try {
            ierr3 = initProblem();
        }
        catch (Cairn_Exception& cairn_error) {
            qCritical() << "Error in initialization of OptimProblem !";
            throw cairn_error;
        }

        if (ierr3 < 0) {
            Cairn_Exception cairn_error("Error in initialization of OptimProblem !", -1);
            throw cairn_error;
        }

        // Now EnergyVectors have been associated with Ports, create ZEvariable list by component, and register them at Problem level.
        createZEVariablesList();
    }
    else {
        /* Initialization of Optimization problem from input Values */
        QString vStudyFile = QString(mStudyFile->archFile().c_str());
        QSettings settingsJson(vStudyFile, JsonFormat);

        try {
            qInfo() << " Use JSON input file ";
            mMilpComponents = mJsonDescription->parseJsonDescription(vStudyFile);
            mDynamicIndicatorsData = mJsonDescription->dynamicIndicators();

            if (mJsonDescription->getException().error() != 0)//It is never set. A Cairn_Exception is thrown directly inside JsonDescription.cpp
            {
                Cairn_Exception cairn_error("Fatal error parsing file " + vStudyFile, -1);
                throw cairn_error;
            }

            QString dir = QFileInfo(vStudyFile).absoluteDir().absolutePath();
            settingsJson.setPath(JsonFormat, QSettings::UserScope, dir);
            mMilpData->setSettings(&settingsJson);
            qInfo() << " Use Json SettingsFile : " << vStudyFile;
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

        mMilpData->setMilpDataFromSettings(mStdAloneMode);

        setExtrapolationFactor();

        // create components from Milp description map mMilpComponents
        try {
            createTecEcoAnalysis(); //FromParamMap
            createMilpComponentsFromParamMap();
            createPortsAndLinksToBus();
            createDynamicIndicators();
        }
        catch (Cairn_Exception& cairn_error) {
            qCritical() << "Error while creating in components in OptimProblem!";
            throw cairn_error;
        }

        // init components and their models from Settings and timeSeries file
        int ierr3 = 0;
        try {
            ierr3 = initProblem();
        }
        catch (Cairn_Exception& cairn_error) {
            qCritical() << "Error in initialization of OptimProblem !";
            throw cairn_error;
        }

        if (ierr3 < 0) {
            Cairn_Exception cairn_error("Error in initialization of OptimProblem !", -1);
            throw cairn_error;
        }

        // Now EnergyVectors have been associated with Ports, create ZEvariable list by component, and register them at Problem level.
        createZEVariablesList();

        // make sure no attempt to read settings later on as local QSettings will have been destroyed 
        mMilpData->setSettings(nullptr);
    }
}

void OptimProblem::registerMilpComponent(QMap<QString,QString> component, MilpComponent* lptr)
{
    if (lptr != nullptr)
    {
       mMapMilpComponents.insert(component["id"],lptr);
       lptr->initMilpComponent();
    }
}

void OptimProblem::registerEnergyVector(const QString& componentId, EnergyVector* lptr)
{
    if (lptr != nullptr)
    {
       mMapEnergyVectors.insert(componentId,lptr);
    }
}

void OptimProblem::initOptimProblemFromTecEcoAnalysis()
{
    if (!mTecEcoAnalysis) return;
    mTecEcoAnalysis->updateEnvImpactUnitsList();
    mCurrency = mTecEcoAnalysis->Currency() ;
    mObjectiveUnit = mTecEcoAnalysis->ObjectiveUnit();
    mRange = mTecEcoAnalysis->Range();
    mForceExportAllIndicators = mTecEcoAnalysis->ForceExportAllIndicators();
    mNbYear = mTecEcoAnalysis->NbYear();
    mNbYearOffset = mTecEcoAnalysis->NbYearOffset();
    mNbYearInput = mTecEcoAnalysis->NbYearInput();
    mLeapYearPos = mTecEcoAnalysis->LeapYearPos();
    mDiscountRate = mTecEcoAnalysis->DiscountRate();
    mImpactDiscountRate = mTecEcoAnalysis->ImpactDiscountRate();
    mInternalRateOfReturn = mTecEcoAnalysis->InternalRateOfReturn();
    mEnvImpactsList = mTecEcoAnalysis->EnvImpactsList();
    for (int i = 0; i < mEnvImpactsList.size(); i++) {
        mEnvImpactsShortNamesList.append(mTecEcoAnalysis->getImpactShortName(mEnvImpactsList[i]));
    }

    mEnvImpactUnitsList = mTecEcoAnalysis->EnvImpactUnitsList();
    mEnvImpactCosts = mTecEcoAnalysis->EnvImpactCosts();

    setExtrapolationFactor();

    mCompoModelName = mTecEcoAnalysis->Model();
    mName = mTecEcoAnalysis->Name();
    setCompoTecEcoEnvSettings(*mTecEcoEnv);
}

void OptimProblem::setCompoTecEcoEnvSettings(TecEcoEnv& aTecEcoEnv) {
    QMapIterator<QString, MilpComponent*> icomponent(mMapMilpComponents);
    while (icomponent.hasNext())
    {
        icomponent.next();
        MilpComponent* lptrCompo = icomponent.value();
        lptrCompo->setTecEcoEnvSettings(aTecEcoEnv);

    }
}

void OptimProblem::createTecEcoAnalysis()
{
    QVectorIterator< QMap<QString,QString> > icomponent(mMilpComponents) ;
    while (icomponent.hasNext())
    {
        QMap<QString,QString> component = icomponent.next();
        if (component["type"] == "TecEcoAnalysis")
        {
            if (mCompoModel) delete mCompoModel;
            mCompoModel = new TecEcoAnalysis(this, component);
            mTecEcoAnalysis = dynamic_cast<TecEcoAnalysis*> (mCompoModel);
            if (mCompoModel == nullptr || mTecEcoAnalysis == nullptr) {
                Cairn_Exception cairn_error("Error creating TecEcoAnalysis " + component["id"], -1);
                throw cairn_error;
            }
            mPorts = mJsonDescription->getCompoPortData(component["id"]); //Ports of TecEcoAnalysis
            mCompoModel->initDefaultPorts();
            MilpComponent::createPorts();
            mCompoModel->setPortPointers();
            initOptimProblemFromTecEcoAnalysis();
            break;
        }
    }
    mGUIData->setGuiNodeName(mName) ;
    mGUIData->setObjectName(mName) ;
}

void OptimProblem::createMilpComponentsFromParamMap()
{
    QVectorIterator<QMap<QString,QString>> icomponent(mMilpComponents) ;
    while (icomponent.hasNext())
    {
        QMap<QString, QString> comp = icomponent.next();
        if (comp["type"] == "TecEcoAnalysis")
            continue; //already created
        if (comp["type"] == "Solver") {
            bool vOK = createSolver(comp["Solver"], comp);
            if (!vOK) {
                Cairn_Exception cairn_error("Error creating Solver " + comp["Solver"] + ": " + comp["id"], -1);
                throw cairn_error;
            }
        }
        else if (comp["type"] == "SimulationControl") {
            bool vOK = createSimulationControl(comp["id"], comp);
            if (!vOK) {
                Cairn_Exception cairn_error("Error creating SimulationControl " + comp["id"], -1);
                throw cairn_error;
            }
        }
        else if (comp["type"] == "EnergyVector") {
            bool vOK = createEnergyVector(comp["id"], comp["type"], comp, mMilpData->Settings());
            if (!vOK) {
                Cairn_Exception cairn_error("Error creating EnergyVector " + comp["id"], -1);
                throw cairn_error;
            }
        }
        else {
            QMap < QString, QMap<QString, QString> > ports = mJsonDescription->getCompoPortData(comp["id"]);
            createComponent(comp, ports);
        }
    }
}

bool OptimProblem::createComponent(const QMap<QString, QString>& component, const QMap < QString, QMap<QString, QString> >& ports)
{
    MilpComponent* lptr = nullptr;
    try {
        if (component["type"] == "Converter" 
            || component["type"] == "Storage" 
            || component["type"] == "PhysicalEquation" 
            || component["type"] == "OperationConstraint") {
            lptr = dynamic_cast <MilpComponent*> (new MilpComponent(this, component["id"], mMilpData, *mTecEcoEnv, component, ports, mModelFactory));
            registerMilpComponent(component, lptr);
        }              
        else if (component["type"] == "Grid")
        {
            lptr = dynamic_cast <MilpComponent*> (new GridCompo(this, component, ports, mMilpData, *mTecEcoEnv, mModelFactory));
            registerMilpComponent(component, lptr);
        }
        else if (component["type"] == "SourceLoad") {
            lptr = dynamic_cast <MilpComponent*> (new SourceLoadCompo(this, component, ports, mMilpData, *mTecEcoEnv, mModelFactory));
            registerMilpComponent(component, lptr);
        }
        else if (component["type"] == "BusFlowBalance" || component["type"] == "BusSameValue") {
            lptr = dynamic_cast <MilpComponent*> (new BusCompo(this, component, ports, mMilpData, *mTecEcoEnv, mModelFactory));
            registerMilpComponent(component, lptr);
        }
        else if (component["type"] == "MultiObjCompo") {
            lptr = dynamic_cast <MilpComponent*> (new MultiObjCompo(this, component, ports, mMilpData, *mTecEcoEnv, mModelFactory));
            registerMilpComponent(component, lptr);
        }        
        else if (component["type"] == "" || component["type"] == "undefined")
        {
            Cairn_Exception cairn_error("Unkown type for component " + component["id"], -1);
            throw cairn_error;
        }
        else
        {
            qInfo() << "Try loading special component type " << component["type"];
            f_MilpComponent UserMilp = LoadDllMilpComponent(component["file"], component["type"]);
            if (UserMilp == nullptr)
            {
                qCritical() << "Unable to load library or component " << component["id"] + " of type " + component["type"];
                Cairn_Exception cairn_error("Please Check that file exists and is in the PATH: " + (component["file"]), -1);
                throw cairn_error;
            }
            MilpComponent* lptr_temp = dynamic_cast <MilpComponent*> (UserMilp(this, component["id"], mMilpData, *mTecEcoEnv, component, ports));
            registerMilpComponent(component, lptr_temp);
        }
    }
    catch (...) {
        Cairn_Exception cairn_error("Error creating component " + component["id"] + " of type " + component["type"], -1);
        throw cairn_error;
    }
    return true;
}


void OptimProblem::deleteComponent(MilpComponent* lptrComponent)
{
    try
    {
        if (lptrComponent != nullptr) {
            lptrComponent->deleteCompoModel();
            if (mMapMilpComponents.contains(lptrComponent->Name())) {
                mMapMilpComponents.remove(lptrComponent->Name());                
            }
            delete lptrComponent;
        }
    }
    catch (...)
    {
        Cairn_Exception erreur("Error Deleting Compoenet ");
        this->setException(erreur);
        return;
    }
}

void OptimProblem::createDynamicIndicators()
{
    QStringList prevUDINames = {};
    QVectorIterator<QMap<QString, QString>> itrIndicators(mDynamicIndicatorsData);
    while (itrIndicators.hasNext())
    {
        QMap<QString, QString> indicator = itrIndicators.next();
        prevUDINames.push_back(indicator["name"]);
        DynamicIndicator* dynamicIndicator = new DynamicIndicator(this, indicator["name"], indicator["formula"], prevUDINames);
        mDynamicIndicators.push_back(dynamicIndicator);
    }
}

void OptimProblem::computeDynamicIndicators(const int& aNsol) //Assumes that the simulation has finished (should be called after readSolution)
{       
    const InputParam::t_Indicators& vIndicators = mCompoModel->getInputIndicators()->getIndicators();

    const double* optSol = mSolver->getOptimalSolution(aNsol);
    for (int i = 0; i < mDynamicIndicators.size(); i++) {
        QString warningMessage = QString("While parsing the formula (" + mDynamicIndicators[i]->getFormula() + ") of dynamic indicator " + mDynamicIndicators[i]->getName());
        //Link variables (symbols) to their values
        QMap<std::string, QString> renamingMap = mDynamicIndicators[i]->variableRenamingMap();
        QMap<std::string, double*> varValueMap = mDynamicIndicators[i]->variableValueMap();
        QMapIterator<std::string, QString> var_Itr(renamingMap);
        while (var_Itr.hasNext())
        {
            var_Itr.next();
            std::string varName = var_Itr.key();
            bool isUserDefinedIndicator = false;
            //The variable is on of user-defined indicators
            for (int j = 0; j < mDynamicIndicators.size(); j++) {//innerForLoop
                if (mDynamicIndicators[j]->getName().simplified() == var_Itr.value().simplified())
                {
                    if (j >= i) {
                        qCritical() << "User-defined indicators are computed in order. Cannot use " + mDynamicIndicators[j]->getName() + " to define indicator " + mDynamicIndicators[i]->getName();
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
            else if (var_Itr.value().split(".").size() == 2) {
                QString compoName = var_Itr.value().split(".")[0];
                QString mipExpName = var_Itr.value().split(".")[1];
                QString mipExpLongName;
                if (mTecEcoAnalysis) {
                    mipExpLongName = mTecEcoAnalysis->getImpactLongName(mipExpName); //used for env impact indicators in case of components
                }
                //Look for componenet
                QMap <QString, std::vector<double>*> compoIndicatorsMap;
                //TecEco case
                if (compoName == mName) 
                {
                    bool isFound = false;
                    for (auto& vIndicator : vIndicators) {
                        const QString &indicatorShortName = vIndicator->getShortName();
                        if (indicatorShortName.simplified() == mipExpName.simplified())
                        {
                            //tecEcoIndicator_Itr.key() is the long name of the indicator e.g. "Net Present Value (Levelized Profit)" for "NPV"
                            *varValueMap[varName] = vIndicator->getValue();
                            isFound = true;
                            break;
                        }
                    }                    
                    if (!isFound)
                    {
                        qWarning() << warningMessage;
                        qWarning() << "Indicator" << mipExpName << "of componenet" << compoName << "not found!";
                    }
                }
                //Other components
                else if (mMapMilpComponents.contains(compoName)) 
                {
                    MilpComponent* lptrComponent = mMapMilpComponents[compoName];
                    const InputParam::t_Indicators& vIndicators = lptrComponent->compoModel()->getInputIndicators()->getIndicators();
                    bool isFound = false;
                    for (auto& vIndicator : vIndicators) {
                        QString indicatorName = vIndicator->getName();
                        if (indicatorName.simplified() == mipExpName.simplified() || indicatorName.simplified() == mipExpLongName.simplified())
                        {
                            *varValueMap[varName] = vIndicator->getValue();
                            isFound = true;
                            break;
                        }
                    }                    
                    if(!isFound)
                    {
                        qWarning() << warningMessage;
                        qWarning() << "Indicator" << mipExpName << "of componenet" << compoName << "not found!";
                    }
                }
                else {
                    qWarning() << warningMessage;
                    qWarning() << "Componenet" << compoName << "not found!";
                }
            }
            else {
                qWarning() << warningMessage;
                qWarning() << var_Itr.value() << " is not a valid variable format. It should be in the form ComponentName.VarName";
                break; //while loop
            }
        }
    }
}

bool OptimProblem::createSolver(const QString& aSolverName, const QMap<QString, QString>& paramMap)
{
    if (mSolver) delete mSolver;
    mSolver = new Solver(aSolverName, paramMap);
    if (mSolver->getException().error() == -1) {
        return false;
    }
    return (mSolver != nullptr);
}

bool OptimProblem::createSimulationControl(const QString& mSimulationControlName, const QMap<QString, QString>& paramMap)
{
    if (mSimulationControl) delete mSimulationControl;
    mSimulationControl = new SimulationControl(mSimulationControlName, paramMap);
    //TODO: use mException
    return (mSimulationControl != nullptr);
}

bool OptimProblem::createEnergyVector(const QString& aName, const QString& aType, const QMap<QString, QString> paramMap, const QSettings& aSettings)
{
    EnergyVector* lptr_EV = nullptr;
    if (paramMap.size() != 0 || aSettings.allKeys().size() !=0) {
        lptr_EV = new EnergyVector(this, aName, paramMap, aSettings);
    }
    else {
        lptr_EV = new EnergyVector(this, aName, aType);
    }
    registerEnergyVector(aName, lptr_EV);
    return (lptr_EV != nullptr);
}

f_MilpComponent OptimProblem::LoadDllMilpComponent(QString Filename, QString ModuleName)
{
#if defined(WIN32) || defined(_WIN32)
//  HINSTANCE hGetProcIDDLL = LoadLibrary("C:\\Documents and Settings\\User\\Desktop\\test.dll");
  LPWSTR ws = GS::ConvertToLPWSTR( Filename.toStdString() );
  HINSTANCE hGetProcIDDLL = LoadLibrary(ws);
#else
  wchar_t* ws = GS::ConvertToLPWSTR( Filename.toStdString() );
  void *hGetProcIDDLL = dlopen((const char*)ws, RTLD_NOW);
#endif

  delete[] ws; // caller responsible for deletion

  f_MilpComponent funci ;

  if (!hGetProcIDDLL) {
    qCritical() << "could not load the dynamic library " << (Filename) ;
    return nullptr;
  }

  // resolve function address here

#if defined(WIN32) || defined(_WIN32)
  funci = (f_MilpComponent)GetProcAddress(hGetProcIDDLL, "UserMilp");
#else
  funci = (f_MilpComponent)dlsym(hGetProcIDDLL, "UserMilp");
#endif

  if (!funci) {
    qCritical() << "could not locate the function" << (ModuleName);
    return funci;
  }

  qInfo() << " INFO : LoadDllMilpComponent succeeded for : " << (ModuleName) ;

  return funci;
}

void OptimProblem::createPortsAndLinksToBus()
{
    QMapIterator<QString, MilpComponent*> icomponent(mMapMilpComponents) ;
    while (icomponent.hasNext())
    {
        icomponent.next() ;
        createPortsAndLinksToBus(icomponent.value());                
    }
    //Create TecEcoAnalysis links
    createPortsAndLinksToBus(this);
}

void OptimProblem::createPortsAndLinksToBus(MilpComponent* aComponent)
{
    BusCompo* lptrIsBus = dynamic_cast<BusCompo*> (aComponent);
    if (lptrIsBus != nullptr) return;

    /** Get list of ports for connections onto a Bus  */
    QListIterator<MilpPort*> iport(aComponent->PortList());
    MilpPort* lptrport;
    while (iport.hasNext())
    {
        lptrport = iport.next();
        QString ObjectLinkedName = lptrport->LinkedComponent();

        //Non-connected port or link is not well-set
        if (!mMapMilpComponents.contains(ObjectLinkedName))
        {
            if (ObjectLinkedName != "") {
                qWarning() << "Linked Bus " + ObjectLinkedName + " is unknown. Link skipped !!";
                lptrport->setLinkedComponent(QString(""));
            }


            if (lptrport->IsDefaultPort()) {
                //A default port that is not connected
                if (mMapEnergyVectors.contains(lptrport->Carrier())) {
                    lptrport->setEnergyVector(mMapEnergyVectors[lptrport->Carrier()]);
                }
                else {
                    QString errorMsg = "Error: the default port " + lptrport->ID() + " (" + lptrport->Name() + ") of component " + aComponent->Name()
                        + " doesn't have an assigned carrier (EnergyVector)!";
                    Cairn_Exception cairn_error(errorMsg, -1);
                    throw cairn_error;
                }
            }
            else {
                //A non-default port that is connected
                aComponent->removePort(lptrport);
            }

             continue;
        }
        
        BusCompo* lptrLinkedBus = dynamic_cast<BusCompo*> (mMapMilpComponents[ObjectLinkedName]);
        MilpComponent* lptrLinkedCompo = dynamic_cast<MilpComponent*> (mMapMilpComponents[ObjectLinkedName]);

        if (lptrLinkedBus != nullptr)
        {
            lptrLinkedBus->addPort(lptrport);
            lptrLinkedBus->addComponent(aComponent);

            //Verify that Bus and port have the same carrier
            if (lptrLinkedBus->VectorName() != lptrport->Carrier()) {
                QString errorMsg = "Error creating link from " + aComponent->Name() + " (port " + lptrport->Name() + ") to Bus " + ObjectLinkedName
                    + ". Bus and port must have the same carrier (EnergyVector)!";
                Cairn_Exception cairn_error(errorMsg, -1);
                throw cairn_error;
            }

            EnergyVector* lptrEnergyVector = mMapEnergyVectors[lptrLinkedBus->VectorName()];
            if (lptrEnergyVector != nullptr){//The EnergyVector of the Bus is set (same value) for every link!
                lptrLinkedBus->setEnergyVector(lptrEnergyVector);
            }
            else{
                QString errorMsg = "Error creating link from " + aComponent->Name() + " (port " + lptrport->Name() + ") to Bus " + ObjectLinkedName
                    + ". EnergyVector " + lptrLinkedBus->VectorName() + " does not exist!";
                Cairn_Exception cairn_error(errorMsg, -1);
                throw cairn_error;
            }
        }
        else if (lptrLinkedCompo != nullptr)
        {
            //Non-Bus - Non-Bus link
            QString errorMsg = "Error: it is not possible to connect a componenet directly to another component: " + aComponent->Name() + " - " + ObjectLinkedName
                + ". An intermediate NodeEquality bus should be used.";
            Cairn_Exception cairn_error(errorMsg, -1);
            throw cairn_error;
        }
        else
        {
            //Linked component is not defined !
            Cairn_Exception cairn_error("Error: the linked component " + ObjectLinkedName + " to the port " + aComponent->Name() + "." + lptrport->Name() + " is not defined (a Bus is expected)!", -1);
            throw cairn_error;
        }

        //Everything is ok => Set LinkedCompo and EnergyVector of the port
        lptrport->setptrLinkedComponent(lptrLinkedCompo);
        lptrport->setEnergyVector(mMapEnergyVectors[lptrport->Carrier()]);
    }
}

void OptimProblem::createZEVariablesList()
{
    mListPublishedVars.clear();
    mListSubscribedVars.clear();
    QMapIterator<QString, MilpComponent*> icomponent(mMapMilpComponents) ;
    while (icomponent.hasNext())
    {
        icomponent.next() ;
        MilpComponent* lptr = icomponent.value() ;
        
         //Read time series        
        lptr->readTSVariablesFromModel();

        //register created lists at OptimProblem level
        lptr->createExchangeListVars(mListSubscribedVars, mListPublishedVars);
    }
    return ;
}

void OptimProblem::computeGUIBusLocations()
{
    int Xbus = 100;
    int Ybus = 100;
    int MaxBusLength = 0;

    QMapIterator<QString, MilpComponent*> icomponent(mMapMilpComponents);
    while (icomponent.hasNext())
    {
        icomponent.next();
        BusCompo* lptrIsBus = dynamic_cast<BusCompo*> (icomponent.value());

        if (lptrIsBus == nullptr) continue;

        if (Xbus > 100) {
            Xbus += int(MAX_X / 20);

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
        
        int maxInOut = max(lptrIsBus->NbPorts(KPROD()), lptrIsBus->NbPorts(KCONS()));
        int BusLength = 80 * max(lptrIsBus->NbPorts(KDATA()), maxInOut);
        MaxBusLength = max(MaxBusLength, BusLength);

        if (lptrIsBus->getXpos() == 0) {
            lptrIsBus->setXpos(Xbus);  
        }
        else {
            Xbus = lptrIsBus->getXpos();
        }

        if (lptrIsBus->getYpos() == 0) {
            lptrIsBus->setYpos(Ybus);  
        }
        else {
            Ybus = lptrIsBus->getYpos();
        }
    }
}

void OptimProblem::computeGUIComponentLocations()
{
    QMapIterator<QString, MilpComponent*> icomponent(mMapMilpComponents);
    while (icomponent.hasNext())
    {
        icomponent.next();
        BusCompo* lptrIsBus = dynamic_cast<BusCompo*> (icomponent.value());
        if (lptrIsBus == nullptr) continue;
        int Xcompo = lptrIsBus->getXpos();
        int Ycompo = lptrIsBus->getYpos();

        QListIterator<MilpComponent*> icompo(lptrIsBus->ListComponent());
        MilpComponent* lptrCompo;
        while (icompo.hasNext())
        {
            lptrCompo = icompo.next();

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

int OptimProblem::SaveFullArchitecture(QString& filename, QString& posAlgorithm)
{
    if (filename == "") {        
        filename = QString(mStudyFile->getScenarioFile("_self.json", 0, false).c_str());
    }

    QFile jsonOutputFile(filename);
    if (!CairnUtils::openFileForWriting(jsonOutputFile))
    {
        qCritical() << "Error when saving self generated json File " << filename;
        return -1;
    }

    bool generatePositions = false;
    QMapIterator<QString, MilpComponent*> icomponent(mMapMilpComponents);
    while (icomponent.hasNext())
    {
        icomponent.next();
        //if there is a component with undefined coordinates
        if ((icomponent.value())->getXpos() == 0 || (icomponent.value())->getYpos() == 0) {
            generatePositions = true;
            break;
        }
    }

    if (generatePositions) {
        if (posAlgorithm.toUpper() != "GRADIENT") {
            computeGUIBusLocations();
            computeGUIComponentLocations();
        }
        else {
            computeGUICompoAndBusLocations();
        }
    }

    jsonSaveDocument(&jsonOutputFile);
    jsonOutputFile.close();
   
    return 0;
}

void OptimProblem::computeGUICompoAndBusLocations() {
    qDebug() << " Computing automatic locations ... ";
    QMapIterator<QString, MilpComponent*> icomponent1(mMapMilpComponents);
    QMap<int, QString> indiceCompo;
    QMap<QString, int> compoIndice;
    int nbCompo = mMapMilpComponents.size();
    int i = 0;

    Eigen::MatrixXf positions = MatrixXf::Zero(nbCompo, 2);
    positions.setConstant(0); //positions.setConstant(100);

    // création d'une matrice d'adjacence
    // index des composants pour pouvoir écrire la matrice
    qDebug() << "- Ecriture de la matrice d'adjacence";
    while (icomponent1.hasNext())
    {
        icomponent1.next();
        indiceCompo[i] = icomponent1.key();
        compoIndice[icomponent1.key()] = i;

        //Initial position
        if ((icomponent1.value())->getXpos() != 0){
            positions(i, 0) = (icomponent1.value())->getXpos();
        }
        if ((icomponent1.value())->getYpos() != 0) {
            positions(i, 1) = (icomponent1.value())->getYpos();
        }
        i++;
    }

    MatrixXf matAdj;
    matAdj=MatrixXf::Zero(nbCompo,nbCompo);
    QMapIterator<QString, MilpComponent*> icomponent2(mMapMilpComponents) ;
    while (icomponent2.hasNext())
    {
        icomponent2.next() ;
        int comp = compoIndice[icomponent2.key()];
        matAdj(comp,comp)=1;
        BusCompo* lptrIsBus = dynamic_cast<BusCompo*> (icomponent2.value()) ;
        if (lptrIsBus == nullptr) continue;
        QListIterator<MilpComponent*> icompoconnection(lptrIsBus->ListComponent());
        while(icompoconnection.hasNext()){
            MilpComponent* compo = icompoconnection.next();
            int comp2 = compoIndice[compo->Name()];
            matAdj(comp,comp2)=1;
            matAdj(comp2,comp)=1;
        }
    }

    qDebug()<<"Calcul des puissances de la matrice d'adjacence";

    // calcul des puissances de la matrice d'adjacence
    QList<MatrixXf> powMatAdj;
    MatrixXf matPow = MatrixXf::Identity(nbCompo,nbCompo);
    for(int i=0; i<nbCompo; i++){
        matPow = matPow*matAdj;
        powMatAdj.append(matPow.replicate(1,1));
    }
    qDebug()<<"Calcul des distances";
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
    qDebug()<<"Calcul de la fonction Energie";
    int nIteration = 1000;  
    double gab = 0.001;
    for (int i = 0; i < nbCompo; i++) {
        if (positions(i, 0) == 0) {
            positions(i, 0) = cos((double(i) / double(nbCompo)) * 2 * 3.14) * maxDist;
        }
        if (positions(i, 1) == 0) {
            positions(i, 1) = sin((double(i) / double(nbCompo)) * 2 * 3.14) * maxDist;
        }
    }
    GradDescResult calcPos;
    qDebug()<<"Debut de la descente de gradient";
    calcPos = GradientDescent(Energy, &positions, &distances, nIteration, gab, 0.001, 0.001);
    MatrixXf newPos;
    if (calcPos.cond)
        newPos = calcPos.X[calcPos.iter-1];
    else
        newPos = calcPos.X[calcPos.iter-2];
    qDebug()<<"Fin de la descente de gradient";

    double shift = - min(0.,double(newPos.minCoeff()));
    QMapIterator<QString, MilpComponent*> icomponent3(mMapMilpComponents) ;
    while (icomponent3.hasNext())
    {
        icomponent3.next();
        MilpComponent* lptrCompo = icomponent3.value();

        if (lptrCompo->getXpos() == 0 && lptrCompo->getYpos() == 0)
        {
            double xPos = lptrCompo->getXpos();
            double yPos = lptrCompo->getYpos();

            if (xPos == 0) {
                lptrCompo->setXpos((newPos(compoIndice[icomponent3.key()], 0) + shift) * 200 + 20);
            }
            if (yPos == 0) {
                lptrCompo->setYpos((newPos(compoIndice[icomponent3.key()], 1) + shift) * 200 + 20);
            }
        }
    }
}

void OptimProblem::jsonSaveDocument (QFile* jsonOutputFile, QString encoding)
{
    QTextStream jsonFile(jsonOutputFile);
    jsonFile.setCodec(QTextCodec::codecForName(encoding.toUtf8()));
    QJsonObject recordObject;

    //Links should be before Components to set the name of Bus ports
    QJsonArray linksArray;
    jsonSaveGuiLinks(linksArray);
    recordObject.insert("Links", linksArray);

    QJsonArray componentsArray;
    jsonSaveGuiComponents(componentsArray);
    recordObject.insert("Components", componentsArray);

    QJsonDocument doc(recordObject);
    jsonFile << doc.toJson();
}

void OptimProblem::jsonSaveGuiComponents(QJsonArray &componentsArray)
{
    //TecEcoAnalysis
    if (mTecEcoAnalysis) mTecEcoAnalysis->jsonSaveGuiComponent(componentsArray);

    //EnergyVectors
    QMapIterator<QString, EnergyVector*> icomponent0(mMapEnergyVectors) ;
    while (icomponent0.hasNext())
    {
        icomponent0.next() ;
        EnergyVector* lptr = icomponent0.value() ;
        lptr->jsonSaveGuiComponent(componentsArray) ;
    }

    //Solver
    if(mSolver) mSolver->jsonSaveGuiComponent(componentsArray);

    //SimulationControl
    if (mSimulationControl) mSimulationControl->jsonSaveGuiComponent(componentsArray);

    //Other components
    QMapIterator<QString, MilpComponent*> icomponent(mMapMilpComponents) ;
    while (icomponent.hasNext())
    {
        icomponent.next() ;
        MilpComponent* lptrCompo = icomponent.value() ;
        lptrCompo->jsonSaveGuiComponent(componentsArray, lptrCompo->getEnergyVector()->Name());
    }
}

void OptimProblem::jsonSaveGuiLinkNodes(QJsonArray& linksArray, const QString& compoName, const QString& compoPortName, const QString& busName, const QString& busPortName, 
    const int& compoX, const int& compoY, const int& busX, const int& busY)
{
    QJsonObject linkObject;
    linkObject.insert("tailNodeName", QJsonValue::fromVariant(compoName)) ;
    linkObject.insert("tailSocketName", QJsonValue::fromVariant(compoPortName)) ;
    linkObject.insert("headNodeName", QJsonValue::fromVariant(busName)) ;
    linkObject.insert("headSocketName", QJsonValue::fromVariant(busPortName)) ;

    QJsonArray listPoints;

    QJsonObject pointObject1;
    pointObject1.insert("x", QJsonValue::fromVariant((compoX+busX)/2));
    pointObject1.insert("y", QJsonValue::fromVariant(compoY));
    listPoints.push_back(pointObject1);

    QJsonObject pointObject2;
    pointObject2.insert("x", QJsonValue::fromVariant((compoX+busX)/2));
    pointObject2.insert("y", QJsonValue::fromVariant(busY));
    listPoints.push_back(pointObject2);

    linkObject.insert("listPoint", listPoints) ;
    linksArray.push_back(linkObject);
}

void OptimProblem::jsonSaveGuiLinks(QJsonArray &linksArray)
{
    //Loop on Bus components
    QMapIterator<QString, MilpComponent*> icomponent(mMapMilpComponents) ;
    while (icomponent.hasNext())
    {
        icomponent.next() ;
        BusCompo* lptrBus = dynamic_cast<BusCompo*> (icomponent.value()) ;
        if (lptrBus == nullptr) continue; //if not Bus, then skip!

        QString busName = lptrBus->Name();
        int busX = lptrBus->getXpos() ;
        int busY = lptrBus->getYpos();

        /* Loop on (Bus) ports: 
        *  those are pointers to the ports of the componenets connected to the Bus
        *  Technically, the Bus doesn't have own ports. 
        *  A Bus port means a link to the componenet that owns this port. 
        */
        
        int iNum = 0;
        int oNum = 0;
        int dNum = 0;
        QListIterator<MilpPort*> iport(lptrBus->PortList());
        MilpPort* lptrport = nullptr;
        while (iport.hasNext())
        {
            lptrport = iport.next();

            QString busPortName = lptrport->BusPortName();

            QString bPortName;
            QString bPortPosition;
            if (lptrport->Direction() == KPROD()) {//Input to Bus
                bPortPosition = Left();
                bPortName = "PortL" + QString::number(iNum);
                iNum++;
            }
            else if (lptrport->Direction() == KCONS()) {//Output from BUS
                bPortPosition = Right();
                bPortName = "PortR" + QString::number(oNum);
                oNum++;
            }
            else {//KDATA()
                bPortPosition = Bottom();
                bPortName = "PortB" + QString::number(dNum);
                dNum++;
            }
            
            if (busPortName == "") {//Case API
                busPortName = bPortName;
                lptrport->setBusPortName(bPortName);
            }

            if (lptrport->BusPortPosition() == "") {//Expected
                lptrport->setBusPortPosition(bPortPosition);
            }

            QString compoPortName = lptrport->Name();

            QString compoName = lptrport->CompoName();
            MilpComponent* lptrCompo = mMapMilpComponents[compoName];
            int compoX = lptrCompo->getXpos();
            int compoY = lptrCompo->getYpos();

            if (lptrport->LinkedComponent() != busName || !lptrBus->ListComponent().contains(lptrCompo)) {
                qWarning() << "Something is wrong! The Bus and the linked component must be identical!";
                qInfo() << "Skip link between " << compoName << " and " << busName;
                continue;
            }

            jsonSaveGuiLinkNodes(linksArray, compoName, compoPortName, busName, busPortName, compoX, compoY, busX, busY);
        }
    }
}

QString OptimProblem::getOptimDirection()
{
    if (mComponent["OptimDirection"] == "Maximize")
        return mComponent["OptimDirection"] ;

    return QString("Minimize");
}

void OptimProblem::setMIPModel(MIPModeler::MIPModel* aModel)
{   
    // set global MIP model pointer
    mModel = aModel ;
    mModel->setExternalModeler(mSolver->getExternalModeler());
    if (mCompoModel != nullptr)
    {
        mCompoModel->setMIPModel(aModel);
    }

    QMapIterator<QString, MilpComponent*> icomponent(mMapMilpComponents) ;
    while (icomponent.hasNext())
    {
        icomponent.next() ;
        MilpComponent* lptr = icomponent.value() ;
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
        qCritical() << "DoInit timeShift " << mMilpData->timeshift() << " should be < futursize " << mMilpData->iHMFuturSize() << " !! ";
        Cairn_Exception cairn_error("Error in doInit of Cairn!", -1);
        //throw cairn_error;
        return -1 ;
    }

    int ierr = 0 ;    
    QMapIterator<QString, MilpComponent*> icomponent(mMapMilpComponents) ;
    while (icomponent.hasNext())
    {
        icomponent.next() ;

        BusCompo* lptrIsBus = dynamic_cast<BusCompo*> (icomponent.value()) ;
        if (lptrIsBus != nullptr) continue;

        MilpComponent* lptr = icomponent.value() ;

        ierr = lptr->initProblem() ; // read parameters then create and initialize MIP variables

        if (ierr <0) {
            qCritical() << "ERROR in initialization of component : " << (lptr->Name());
            return ierr ;
        }
    }
    // Loop on Bus components
    QMapIterator<QString, MilpComponent*> ibuscomponent(mMapMilpComponents) ;
    while (ibuscomponent.hasNext())
    {
        ibuscomponent.next() ;
        BusCompo* lptrIsBus = dynamic_cast<BusCompo*> (ibuscomponent.value()) ;
        if (lptrIsBus == nullptr) continue;

        ierr = lptrIsBus->initProblem() ; // read parameters then create and initialize MIP variables

        if (ierr <0) {
            qCritical() << "ERROR in initialization of Bus component : " << (lptrIsBus->Name());
            return ierr ;
        }
    }

    // Send component ports data to Submodel and allocate Ports own variables

    ierr = MilpComponent::initPorts() ;
    if (ierr <0) return ierr ;

    ierr = initSubModelConfiguration() ;

    return ierr ;
}

void OptimProblem::initSubModelTopology()
{
    //mCompoModel->setTopo(mListPort);
    mCompoModel->setTopoCompo(mMapMilpComponents) ;
    mCompoModel->setParentCompo(this) ;
}

int OptimProblem::initSubModelInput()
{
    int ierr = 0 ;

    QMapIterator<QString, MilpComponent*> icomponent(mMapMilpComponents) ;
    while (icomponent.hasNext())
    {
        icomponent.next() ;
        MilpComponent* lptr = icomponent.value() ;

        ierr = lptr->initSubModelInput() ; // read parameters then create and initialize MIP variables

        if (ierr <0) {
            qCritical() << "ERROR in initialization of component " ;
            return ierr ;
        }
    }

    ierr = MilpComponent::initSubModelInput() ;

    return ierr ;
}

EnergyVector* OptimProblem::getEnergyVector(const QString& aName)
{
    EnergyVector* vRet = nullptr;
    if (mMapEnergyVectors.contains(aName)) {
        vRet = mMapEnergyVectors[aName];
    }
    return vRet;
}

QList<EnergyVector*> OptimProblem::getEnergyVectorList()
{
    QList<EnergyVector*> vRet;
    QMapIterator<QString, EnergyVector*> icomponent(mMapEnergyVectors);
    while (icomponent.hasNext())
    {
        icomponent.next();
        vRet.push_back(icomponent.value());
    }
    return vRet;
}

void OptimProblem::deleteEnergyVector(const QString& aName)
{
    // Attention, il faut vérifier si cet élément n'est pas utilisé dans le problème
    // Cette étape de vérificaiton est efectuée dans l'API
    QMap<QString, EnergyVector*>::iterator vIter = mMapEnergyVectors.find(aName);
    if (vIter != mMapEnergyVectors.end()) {
        EnergyVector* vEV = vIter.value();
        if (vEV) delete vEV;
        mMapEnergyVectors.remove(aName);
    }        
}

//------------------------------------------------------------------------------
//  Build Problem
//------------------------------------------------------------------------------
void OptimProblem::buildComponentConstraints()
{
    QMapIterator<QString, MilpComponent*> icomponent(mMapMilpComponents) ;
    while (icomponent.hasNext())
    {
        icomponent.next() ;
        MilpComponent* lptr = icomponent.value() ;

        //Exclude Bus componenets
        BusCompo* lptrBus = dynamic_cast<BusCompo*> (icomponent.value()) ;
        if (lptrBus == nullptr)
        {
            try {
                lptr->buildProblem(); // les bus doivent attendre que toutes les expressions soient ecrites !
            }
            catch (Cairn_Exception cairn_error) {
                this->setException(cairn_error);
                return;
            }
        }
    }
}

void OptimProblem::buildManualObjectiveConstraints()
{
    QMapIterator<QString, MilpComponent*> icomponent(mMapMilpComponents);
    while (icomponent.hasNext())
    {
        icomponent.next();
        BusCompo* lptrBus = dynamic_cast<BusCompo*> (icomponent.value());
        if (lptrBus != nullptr)
        {
            //Only ManualObjective
            TechnicalSubModel* tecModel = dynamic_cast<TechnicalSubModel*> (lptrBus->compoModel());
            if (tecModel != nullptr) {
                lptrBus->buildProblem(); // les bus doivent attendre que toutes les expressions soient ecrites !
            }
        }
    }
}

void OptimProblem::buildBusConstraints()
{
    QMapIterator<QString, MilpComponent*> icomponent(mMapMilpComponents) ;
    while (icomponent.hasNext())
    {
        icomponent.next() ;
        BusCompo* lptrBus = dynamic_cast<BusCompo*> (icomponent.value()) ;
        if (lptrBus != nullptr)
        {
            //Exclude ManualObjective componenets
            TechnicalSubModel* tecModel = dynamic_cast<TechnicalSubModel*> (lptrBus->compoModel());
            if (tecModel == nullptr) {
                lptrBus->buildProblem(); // les bus doivent attendre que toutes les expressions soient ecrites !
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

void OptimProblem::resetFlags() {
    //Needed at the end of a problem run to avoid issues when having two consecutive runs! 
    QMapIterator<QString, MilpComponent*> icomponent(mMapMilpComponents);
    while (icomponent.hasNext())
    {
        icomponent.next();
        MilpComponent* lptr = icomponent.value();
        lptr->resetFlags();
    }
}

bool OptimProblem::newCompoModel()
{
    //default TecEcoAnalysis
    mCompoModel = new TecEcoAnalysis(this);
    mTecEcoAnalysis = dynamic_cast<TecEcoAnalysis*> (mCompoModel);
    initOptimProblemFromTecEcoAnalysis();
    return (mCompoModel != nullptr && mTecEcoAnalysis != nullptr);
}

void OptimProblem::buildProblem()
{
    QString vStudyFile = QString(mStudyFile->archFile().c_str());
    QSettings settingsJson(vStudyFile, JsonFormat);

    mRateOfReturnDiscountFactor = 0; // Why it is always 0 ?!!

    int ierr = initSubModelInput();

    if (ierr <0) {
        Cairn_Exception erreur ("Error in OptimProblem init ",ierr);
        //        throw erreur ; not functionnal because of QWidget in FBSF ??
        this->setException(erreur) ;
        return ;
    }

    // Create Constraints
    qInfo() << "OptimProblem::buildComponentConstraints";
    buildComponentConstraints();

    qInfo() << "OptimProblem::buildManualObjectiveConstraints";
    buildManualObjectiveConstraints();

    // Compute PreSimulation TecEco expressions 
    if (mCompoModel != nullptr)
    {
        qInfo() << "optimProblem::computeAllContribution";
        mCompoModel->computeAllContribution();
    }

    //TecEcoAnalysis Model Interface at ports
    try
    {
        setBusFluxPortExpression();       /**  send flux expressions to FlowBalanceBus */
        setBusSameValuePortExpression();  /**  publish expression to SameValueBus */
    }
    catch (...)
    {
        Cairn_Exception error(" ERROR in component setting the ports of TecEcoAnalysis", -1);
        this->setException(error);
        return;
    }

    // dans la version actuelle, on ne prevoit pas de connexion directe d'un composant a un autre.
    // la connexion n'est possible que par un convertisseur ou une relation d'agregation.

    // dans cette solution, les bus sont des agregateurs a potentiel identique, et assurent une somme de flux
    // les contributions sont des expressions calculees par tous les ports contributeurs.
    // la contrainte que les potentiels sont identiques est implicite, c'est porte par le vecteur energetique.
    // on passe d'un vecteur a un autre (== potentiel different) par un convertisseur.
    qInfo() << "OptimProblem::buildBusConstraints";
    buildBusConstraints();

    // Model component behaviour
    if (mCompoModel != nullptr)
    {
        qInfo() << "buildModel: " << mCompoModel->getCompoName();
        mCompoModel->buildModel();     /**  define behaviour model and associated Variables */
        computeObjectiveFunction(*mExpObjective);  /** set the value of mObjective to the ObjectiveExpression from TecEcoAnalysis */
    }

    return ;
}

void OptimProblem::solveProblem(QString& optimLogFileName,  const int cycle, const QMap<QString, bool> paramMap, const bool aExportResultsEveryCycle)
{
    QString location = QString(mStudyFile->getScenarioFile("", 0, false).c_str());
    
    int iCycle = cycle;
    if (iCycle == 0) iCycle = 1;

    //Add a speration line
    std::ofstream optimLogFile;
    if (iCycle == 1) {
        //write
        optimLogFile.open(optimLogFileName.toStdString().c_str(), std::ofstream::out | std::ofstream::trunc);
    }
    else {
        //append
        optimLogFile.open(optimLogFileName.toStdString().c_str(), std::ofstream::out | std::ofstream::app);
    }

    optimLogFile << "---------------------------------------" << (" Cycle " + QString::number(iCycle)).toStdString().c_str() << " ---------------------------------------\n";
    optimLogFile.close();

    if(aExportResultsEveryCycle)
        mSolver->SolveProblem(mModel, location, cycle, paramMap);
    else 
        mSolver->SolveProblem(mModel, location, 0, paramMap); //to avoid writing .lp every cycle 
}

QString OptimProblem::getOptimisationStatus()
{
    return mSolver->getOptimisationStatus();
}

int OptimProblem::getInterpretedOptimStatus()
{
    QString status = getOptimisationStatus();

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
        mOptimStatus = 2;
    }
    return mOptimStatus;
}

bool OptimProblem::getIsCheckConflicts()
{
    return mSolver->getIsCheckConflicts();
}

void OptimProblem::readSolution(int aNsol)
{
    QMapIterator<QString, MilpComponent*> icomponent(mMapMilpComponents) ;
    while (icomponent.hasNext())
    {
        icomponent.next() ;
        MilpComponent* lptr = icomponent.value() ;

        //computeAllIndicators assumes mSolver->getModelType() == GS::MIPMODELER()
        lptr->compoModel()->computeAllIndicators(mSolver->getOptimalSolution(aNsol));
        //
        lptr->exportSubmodelIO(mSolver, aNsol);
        //lptr->cleanExpressions(aNsol);
    }
}

void OptimProblem::cleanExpressions()
{
    QMapIterator<QString, MilpComponent*> icomponent(mMapMilpComponents);
    while (icomponent.hasNext())
    {
        icomponent.next();
        MilpComponent* lptr = icomponent.value();
        lptr->cleanExpressions();
    }
}

void OptimProblem::writeSolution(int n, std::map<std::string, std::vector<double>>& resultats)
{
    resultats.clear();
    const double* optimalSolution = mSolver->getOptimalSolution(n);    
    QMapIterator<QString, MilpComponent*> icomponent(mMapMilpComponents);
    while (icomponent.hasNext())
    {
        icomponent.next();
        MilpComponent* lptr = icomponent.value();
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

    QMapIterator<QString, MilpComponent*> icomponent(mMapMilpComponents) ;
    while (icomponent.hasNext())
    {
        icomponent.next() ;
        MilpComponent* lptr = icomponent.value() ;

        lptr->prepareOptim();
    }
}

int OptimProblem::setParameters()
{
    int ierr = 0 ;
    QMapIterator<QString, MilpComponent*> icomponent(mMapMilpComponents) ;
    while (icomponent.hasNext())
    {
        icomponent.next() ;
        MilpComponent* lptr = icomponent.value() ;

        //ierr = lptr->setParameters(); enlevé: il est présent deux fois !
        if (ierr <0) return ierr ;
    }
    return ierr ;
}

void OptimProblem::computeTecEcoEnvAnalysis(const int& aNsol, const int& istat)
{
    QMapIterator<QString, MilpComponent*> icomponent(mMapMilpComponents) ;
    while (icomponent.hasNext())
    {
        icomponent.next() ;
        MilpComponent* lptr = icomponent.value() ;
        lptr->computeTecEcoEnvAnalysis();
    }

    //-------------- Compute TecEco Indicators -----------------------------//
    //computeAllIndicators assumes mSolver->getModelType() == GS::MIPMODELER()
    if (istat < 2) mCompoModel->computeAllIndicators(mSolver->getOptimalSolution(aNsol)); 

    //Case of GAMS
    /*
        ModelerInterface* pExternalModeler = aSolver->getExternalModeler();
        if (pExternalModeler != nullptr) {
            QString gamsVarName = mName + "_v_ObjContribution";
            const double* objective = aSolver->getOptimalSolution(aNsol, gamsVarName.toStdString());
            if (objective != nullptr) {
                objectiveContribution = objective[0];
            }
            else {
                qDebug() << aSolver->getModelType() << "::Variable key: " << gamsVarName << " not defined in " << aSolver->getModelType() << " model";
            }
            delete objective;
        }
    */

    //-------------- Compute Dynamic Indicators -----------------------------//
    computeDynamicIndicators(aNsol); 
}

void OptimProblem::exportEnvImpactMassIndicators(QString& aFileName, QString& encoding) {
    //FileName
    if (aFileName == "") {
        aFileName = "study_results_EnvImpactMass.csv";
    }
    //----------------- Generate the Table -----------------
    QString headerNames1 = QString("");
    QString headerNames2 = QString("");
    QString headerUnits = QString("");
    QMap<QString, QString> valuesMap;
    QList<QString> impactNames;
    if(mTecEcoAnalysis) impactNames  = mTecEcoAnalysis->getPossibleImpactNames();
    bool first = true; //first componenet
    //loop over all componenets
    QMapIterator<QString, MilpComponent*> icomponent(mMapMilpComponents);
    while (icomponent.hasNext())
    {
        icomponent.next();
        MilpComponent* lptr = icomponent.value();
        //consider only componenets that have an Environment Model
        if (lptr->EnvironmentModel()) {
            if (first) {
                headerNames1 = "Impact Name";
                headerNames2 = "";
                headerUnits = "Unit";
            }
            valuesMap[lptr->compoModel()->getCompoName()] = QString("");
            //loop over the considered impacts: used only to have a good order (less efficent)
            for (int i = 0; i < impactNames.size(); i++) {
                double mass = 0.0;
                double grey = 0.0;
                //list of all indicators 
                const InputParam::t_Indicators& vIndicators = lptr->compoModel()->getInputIndicators()->getIndicators();
                for (auto& vIndicator : vIndicators) {
                    const QString& vIndicatorName = vIndicator->getName();
                    if (!vIndicatorName.contains(impactNames[i])) {
                        continue;
                    }
                    else if (vIndicatorName.contains("Env impact mass")) {
                        mass = vIndicator->getValue();
                    }
                    else if (vIndicatorName.contains("EnvGrey impact mass")) {
                        grey = vIndicator->getValue();
                        if (first) {
                            headerNames1 += " ; " + impactNames[i] + " ; " + impactNames[i] + " ; " + impactNames[i];
                            headerNames2 += " ; Cumulative impact mass  ; Env impact mass ; EnvGrey impact mass";
                            QString unit = vIndicator->getUnit();
                            headerUnits += " ; " + unit + " ; " + unit + " ; " + unit;
                        }
                        valuesMap[lptr->compoModel()->getCompoName()] += " ; " + QString::number(mass + grey) + " ; " + QString::number(mass) + " ; " + QString::number(grey);
                    }
                }               
            }
            first = false;
        }
    }

    //No Env Impact is selected or Environment Model is off for all componenets => Don't export the file !
    if (headerNames1 == QString("")) {
        return;
    }

    //----------------- Open the File ----------------- 
    QFile FileOut(aFileName);
    if (!CairnUtils::openFileForWriting(FileOut)) {
        return; //error!
    }

    QTextStream out(&FileOut);
    out.setCodec(QTextCodec::codecForName(encoding.toUtf8()));

    //-----------------  Write the Table ----------------- 
    out << headerNames1 << "\n";
    out << headerNames2 << "\n";
    out << headerUnits << "\n";
    
    QMapIterator<QString, QString> iLine(valuesMap);
    while (iLine.hasNext())
    {
        iLine.next();
        out << iLine.key() << iLine.value() << "\n";
    }

    FileOut.close();
}

void OptimProblem::exportEnvImpactParameters(QString& aFileName, QString& encoding) {
    //FileName
    if (aFileName == "") {
        aFileName = "study_EnvImpactParameters.csv";
    }
    //----------------- Generate the Table -----------------
    QString header = QString("");
    QMap<QString, QString> valuesMap;
    QList<QString> impactNames;
    if (mTecEcoAnalysis) impactNames = mTecEcoAnalysis->getPossibleImpactNames();
    bool first = true; //first componenet
    //loop over all componenets
    QMapIterator<QString, MilpComponent*> icomponent(mMapMilpComponents);
    while (icomponent.hasNext())
    {
        icomponent.next();
        MilpComponent* lptr = icomponent.value();
        //consider only componenets that have an Environment Model
        if (lptr->EnvironmentModel()) {
            if (first) {
                header = "Component Name";
            }
            valuesMap[lptr->compoModel()->getCompoName()] = QString("");

            std::map<QString, InputParam::ModelParam*> paramMap = lptr->compoModel()->getInputEnvImpactsParam()->getMapParams();

            //loop over the considered impacts: used only to have a good order (less efficent)
            for (int i = 0; i < impactNames.size(); i++) {                            
                for (auto const& [key, param] : paramMap) {
                    if (!param->getName().contains(impactNames[i]))
                        continue;
                    if (first) {
                        QString impactShortNames = mTecEcoAnalysis->getImpactShortName(param->getName());
                        header += " ; " + impactShortNames;
                    }
                    valuesMap[lptr->compoModel()->getCompoName()] += QString(" ; ") + param->toString();
                }                
            }
            first = false;
        }
    }

    //No Env Impact is selected or Environment Model is off for all componenets => Don't export the file !
    if (header == QString("")) {
        return;
    }

    //----------------- Open the File ----------------- 
    QFile FileOut(aFileName);
    if(!CairnUtils::openFileForWriting(FileOut)) {
        return; //error!
    }

    QTextStream out(&FileOut);
    out.setCodec(QTextCodec::codecForName(encoding.toUtf8()));

    //-----------------  Write the Table ----------------- 
    out << header << "\n";
    QMapIterator<QString, QString> iLine(valuesMap);
    while (iLine.hasNext())
    {
        iLine.next();
        out << iLine.key() << iLine.value() << "\n";
    }

    FileOut.close();
}

void OptimProblem::exportPortEnvImpactParameters(QString& aFileName, QString& encoding) {
    //FileName
    if (aFileName == "") {
        aFileName = "study_PortEnvImpactParameters.csv";
    }
    //----------------- Generate the Table -----------------
    QString header = QString("");
    QMap<QString, QString> valuesMap;
    QList<QString> impactNames;
    if (mTecEcoAnalysis) impactNames = mTecEcoAnalysis->getPossibleImpactNames();
    bool first = true; //first port
    //loop over all componenets
    QMapIterator<QString, MilpComponent*> icomponent(mMapMilpComponents);
    while (icomponent.hasNext())
    {
        icomponent.next();
        MilpComponent* lptr = icomponent.value();
        //consider only componenets that have an Environment Model
        if (lptr->EnvironmentModel()) {
            QList<MilpPort*> portList = lptr->PortList();
            //loop over all ports
            for (int p = 0; p < portList.size(); p++) {
                QString portName = portList[p]->Name();
                QString varName = portList[p]->Variable();
                QString fullName = lptr->compoModel()->getCompoName() + "." + portName;
                //
                if (first) {
                    header = "Port Name ; Variable";
                }
                valuesMap[fullName] = QString(" ; ") + varName; 

                std::map<QString, InputParam::ModelParam*> paramMap = lptr->compoModel()->getInputPortImpactsParam()->getMapParams();

                //loop over the impacts: used only to have a good order (less efficent)
                for (int i = 0; i < impactNames.size(); i++) {
                    for (auto const& [key, param] : paramMap) {
                        if (!param->getName().contains(portName))
                            continue;
                        if (!param->getName().contains(impactNames[i]))
                            continue;
                        if (first) {
                            QString impactShortNames = mTecEcoAnalysis->getImpactShortName(param->getName());
                            if (impactShortNames.split(".").size() > 1)
                                header += " ; " + impactShortNames.split(".")[1];
                            else
                                header += " ; " + impactShortNames;
                        }
                        valuesMap[fullName] += QString(" ; ") + param->toString();
                    }                   
                }
                first = false;
            }
        }
    }

    //No Env Impact is selected or Environment Model is off for all componenets => Don't export the file !
    if (header == QString("")) {
        return;
    }

    //----------------- Open the File ----------------- 
    QFile FileOut(aFileName);
    if (!CairnUtils::openFileForWriting(FileOut)) {
        return; //error!
    }

    QTextStream out(&FileOut);
    out.setCodec(QTextCodec::codecForName(encoding.toUtf8()));

    //-----------------  Write the Table ----------------- 
    out << header << "\n";
    QMapIterator<QString, QString> iLine(valuesMap);
    while (iLine.hasNext())
    {
        iLine.next();
        out << iLine.key() << iLine.value() << "\n";
    }

    FileOut.close();
}

void OptimProblem::exportParameters(QString& aFileName, QString& encoding) {
    //FileName
    if (aFileName == "") {
        aFileName = "study_parameters.csv";
    }

    //Open the File
    QFile FileOut(aFileName);
    if (!CairnUtils::openFileForWriting(FileOut)) {
        return; //error!
    }

    QTextStream out(&FileOut);
    out.setCodec(QTextCodec::codecForName(encoding.toUtf8()));

    //header
    out << "Component;Parameter;Value;Unit;Description\n";
    
    //Solver 
    std::map<QString, InputParam::ModelParam*> paramMap = mSolver->getParameters();
    exportParameters(out, mSolver->Name(), paramMap);

    //SimulationControl
    paramMap = mSimulationControl->getParameters();
    exportParameters(out, mSimulationControl->Name(), paramMap);

    //TecEcoAnalysis 
    paramMap = {};
    if (mTecEcoAnalysis) paramMap = mTecEcoAnalysis->getParameters();
    exportParameters(out, mName, paramMap);

    //MilpComponents
    QMapIterator<QString, MilpComponent*> icomponent(mMapMilpComponents);
    while (icomponent.hasNext())
    {
        icomponent.next();
        MilpComponent* lptr = icomponent.value();
        paramMap = lptr->getParameters();
        exportParameters(out, lptr->Name(), paramMap, lptr->PortList()[0]->ptrEnergyVector(), lptr->getTimeSeriesNames());
    }
    FileOut.close();
}

void OptimProblem::exportParameters(QTextStream& out, QString& name, std::map<QString, InputParam::ModelParam*>& paramMap, EnergyVector* aEnergyVector, QMap<QString, QString> aTimeSeriesNames)
{
    for (auto const& [key, param] : paramMap) {
        //Only mandatory and modified parameters (and optional timeseries if value is not empty)
        if ( param->IsBlocking() || param->isModified() == TriState::True
             || (aTimeSeriesNames.find(key) != aTimeSeriesNames.end() && aTimeSeriesNames[key] != "") )
        {
            QString unit = "-";
            if (param->getQuantities().size() > 0) {
                if (aEnergyVector == nullptr) {
                    unit = param->getQuantities()[0];
                }
                else {
                    unit = mCompoModel->convertUnits(aEnergyVector, param->getQuantities());
                }
            }

            QString  value = "";
            if (aTimeSeriesNames.find(key) != aTimeSeriesNames.end()) {
                value = aTimeSeriesNames[key];
        }
            else {
                value = param->toString();
    }

            out << name + ";" + param->getName() + ";" + value + ";" + unit + ";" + param->getDescription() + "\n";
        }
    }
}

void OptimProblem::exportMultiObjFile(QTextStream& out, int aNsol)
{
    if (mTecEcoAnalysis != nullptr) {
        QMap<QString, MIPModeler::MIPExpression*> mapSubObjective = mTecEcoAnalysis->mapSubObjective();
        QMapIterator<QString, MIPModeler::MIPExpression*> iExp(mapSubObjective);
        while (iExp.hasNext()) {
            iExp.next();
            CairnUtils::outputIndicator(out, iExp.key(), "Subobjective", (iExp.value())->evaluate(mSolver->getOptimalSolution(aNsol)), mObjectiveUnit, "Subobjective");
        }
    }
}

void OptimProblem::exportAllTecEcoEnvAnalysis(QString aResultFile, QString range, QString encoding, const bool isRollingHorizon, const int aNsol)
{
    QFile FileOut((aResultFile));
    if (!CairnUtils::openFileForWriting(FileOut)) {
            Cairn_Exception cairn_error("OptimProblem: couldn't open result file for writing : "+ aResultFile, -1);
            throw cairn_error;
     }

    QTextStream out(&FileOut) ;
    out.setCodec(QTextCodec::codecForName(encoding.toUtf8()));
    QString Qrange = (range) ;

    out << "Model" << ";Indicator" << ";Value" << ";Unit" << ";Alias" << "\n";

    //TecEco
    InputParam* modelIndicators = mCompoModel->getInputIndicators();
    const InputParam::t_Indicators& vIndicators = modelIndicators->getIndicators();
    for (size_t i = 0; i < vIndicators.size(); i++) {
        vIndicators[i]->Export(out, mName, range, mForceExportAllIndicators);
    }

    // DETAILED DATA; BY COMPONENTS 
    QMapIterator<QString, MilpComponent*> icomponent(mMapMilpComponents);
    while (icomponent.hasNext())
    {
        icomponent.next();
        MilpComponent* lptr = icomponent.value();

        lptr->setRange(range);
        lptr->compoModel()->exportIndicators(out, lptr->Name(), range, mForceExportAllIndicators, isRollingHorizon);
    }

    //Add multiObj results
    if (range == "PLAN") {
        exportMultiObjFile(out, aNsol);
    }

    //User-defined indicators
    if (range == "PLAN") {
        for (int i = 0; i < mDynamicIndicators.size(); i++) {
            CairnUtils::outputIndicator(out, "User-Defined", mDynamicIndicators[i]->getName(), mDynamicIndicators[i]->compute(), "UNIT", mDynamicIndicators[i]->getName());
        }
    }

    FileOut.close();
}

void OptimProblem::computeHistState()
{
    QMapIterator<QString, MilpComponent*> icomponent(mMapMilpComponents) ;
    while (icomponent.hasNext())
    {
        icomponent.next() ;
        MilpComponent* lptr = icomponent.value() ;
        lptr->computeHistNbHours();
    }
    MilpComponent::computeHistNbHours();
}

void OptimProblem::exportResults()
{
    QMapIterator<QString, MilpComponent*> icomponent(mMapMilpComponents) ;
    while (icomponent.hasNext())
    {
        icomponent.next() ;
        MilpComponent* lptr = icomponent.value() ;        
        lptr->exportResults(mListPublishedVars);
       // lptr->exportResults();
    }
}

void OptimProblem::setDefaultsResults()
{
    QMapIterator<QString, MilpComponent*> icomponent(mMapMilpComponents) ;
    while (icomponent.hasNext())
    {
        icomponent.next() ;
        MilpComponent* lptr = icomponent.value() ;

        lptr->setDefaultsResults();
    }
}

void OptimProblem::exportOptimaSizeAllCycles(const QString& aFileName, const int cycle, QString encoding)
{
    QFile FileOut(aFileName);
    if (!CairnUtils::openFileForWriting(FileOut)) {
        Cairn_Exception cairn_error("OptimProblem: couldn't open file optimalSize.csv for writing.", -1);
        throw cairn_error;
    }

    QTextStream out(&FileOut);
    out.setCodec(QTextCodec::codecForName(encoding.toUtf8()));

    out << "sep=;\n";

    QString header = "";
    QMapIterator<QString, MilpComponent*> icomponent(mMapMilpComponents);
    while (icomponent.hasNext())
    {
        icomponent.next();
        MilpComponent* lptr = icomponent.value();
        if (lptr->compoModel()->getOptimalSizeAllCycles().size() > 0) { 
            header += ";" + lptr->compoModel()->getCompoName();
        }
        else {
            //Must be a Bus component. In case of Bus, mOptimalSizeAllCycles is empty. 
        }
    }
    out << header << "\n";
    
    for (int i = 0; i < cycle; i++) {
        QString optimalSizeValues = "Cycle " + QString::number(i + 1);
        QMapIterator<QString, MilpComponent*> icomponent(mMapMilpComponents);
        while (icomponent.hasNext())
        {
            icomponent.next();
            MilpComponent* lptr = icomponent.value();
            if (i < lptr->compoModel()->getOptimalSizeAllCycles().size()) {
                optimalSizeValues += ";" + QString::number((lptr->compoModel()->getOptimalSizeAllCycles())[i]);
            }
            else if (header.contains(lptr->compoModel()->getCompoName())) { //lptr->compoModel()->getOptimalSizeAllCycles().size() > 0
                //This means that there is a non-Bus component with mOptimalSizeAllCycles.size > 0 but < nCycles. 
                //If happened then there must be something wrong!
                optimalSizeValues += ";"; 
            }
        }
        out << optimalSizeValues << "\n";
    }
}