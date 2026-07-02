#if  (!defined(WIN32) && !defined(_WIN32))
#include <dlfcn.h>
#endif

#include "MilpComponent.h"
#include "TechnicalSubModel.h"
#include "TecEcoAnalysis.h"
#include "OptimProblem.h"
#include "ModelTS.h"
#include "CairnAPIUtils.h"
#include "CairnUtils.h"
#include "ModelFactory.h"
#include <map>

#include <filesystem>
namespace fs = std::filesystem;

using namespace CairnUtils;
using namespace CairnAPIUtils;
using namespace GS ;

using Eigen::Map;

MilpComponent::MilpComponent(CairnObject *aParent,
                             const std::string& aName,
                             MilpData *aMilpData, TecEcoAnalysis* aTecEcoAnalysis,
                             const t_mapParamData& aComponent,
                             const std::map < std::string, t_mapParamData > &aPorts,
                             ModelFactory* aModelFactory) 
:  
  CairnObject(aParent, aName),
  mTecEcoAnalysis(aTecEcoAnalysis),
  mException(Cairn_Exception()),
  mComponent(aComponent),
  mPorts(aPorts),
  mMilpData(aMilpData),
  mType(CairnUtils::getParamValue(aComponent,"type")),
  mCompoModelName(CairnUtils::getParamValue(aComponent,"ModelType")),        // node type used in the GUI, e.g. Electrolyzer
  mCompoTechnoType(CairnUtils::getParamValue(aComponent,"ModelTechnoType")), // node image used in the GUI
  mCompoModelClassName(CairnUtils::getParamValue(aComponent,"ModelClass")), // e.g. ElectrolyzerDetailed
   mHistNbHours(0),
   mOptimalSize(0)
  {
    //this->setObjectName(aName);
    setObjectType("MilpComponent");
    mModelFactory = aModelFactory;
    mCompoModel = nullptr;
    
    mCompoInputParam = new InputParam (this,"CompoInputParam"+aName) ;
    mInputParam = new InputParam (this, "InputParam"+aName) ;                   /** List of COMPONENT Input parameters (for link with PEGASE or OUTSIDE) */
    mPlugSubmodelIO = new InputParam (this, "PlugSubmodelIO"+aName) ;           /** List of COMPONENT Output data (for link with PEGASE or OUTSIDE) Used for timeShifting, IMPORT and EXPORT wrt PEGASE exchange Zone */
    mTimeSeriesSubmodel = new InputParam (this, "TimeSeriesSubmodel"+aName) ;   /** List of COMPONENT TimeSeries input data (for link with PEGASE) Used for timeShifting, IMPORT and EXPORT wrt PEGASE exchange Zone */

  /** mComponent is used for architecture and topological or constant data */
  /** mSettings is used for "constant" parameters that can be made variable and optimized from outside */

    mFirstInit = 0 ; //TODO: one mFirst.. flag is enough! 
    mFirstInitTS = 0;

  } // MilpCompoData()

void MilpComponent::initMilpComponent()
{
    createCompoModel();

    declareCompoInputParam();
    setCompoInputParam(mComponent);

    //auto getOr = [&](const std::string& key, const std::string& defaultValue)-> std::string {
    //    if (auto it = mComponent.find(key); it != mComponent.end())
    //        return it->second;
    //    return defaultValue;
    //};

    // should not be called before setCompoInputParam
    t_mapParamData extractedParams = CairnUtils::extractGuiParams(mComponent);
    initGuiData(extractedParams); //To be generalized for GUI data param
}

MilpComponent::~MilpComponent()
{
    delete mInputParam;
    delete mCompoInputParam;
    delete mTimeSeriesSubmodel; 
    delete mPlugSubmodelIO;   

    mInputParam = nullptr;
    mCompoInputParam = nullptr;
    mTimeSeriesSubmodel = nullptr;
    mPlugSubmodelIO = nullptr;

    delete mCompoModel;
    delete mGUIData;

    mCompoModel = nullptr;
    mGUIData = nullptr;
}

void MilpComponent::setMIPModel (MIPModeler::MIPModel* aModel)
{
    mModel = aModel;
    if (mCompoModel != nullptr)
        mCompoModel->setMIPModel(aModel);
    else
        cCritical() << "Coding error : setMIPModel called with non MIPModeler model !! " ;
}

void MilpComponent::setTecEcoAnalysis(TecEcoAnalysis* aTecEcoAnalysis)
{
    mTecEcoAnalysis = aTecEcoAnalysis;
}

const std::string* MilpComponent::pCurrency() const  {
    return mTecEcoAnalysis->pCurrency();
}

double& MilpComponent::ExtrapolationFactor() {
    return mTecEcoAnalysis->ExtrapolationFactor();
}

std::vector<double>& MilpComponent::LevelizationTable() {
    return mTecEcoAnalysis->LevelizationTable(); 
}

std::vector<double>& MilpComponent::ImpactLevelizationTable() {
    return mTecEcoAnalysis->ImpactLevelizationTable(); 
}

std::vector<double>& MilpComponent::TableYearsHours() {
    return mTecEcoAnalysis->TableYearsHours(); 
}

MIPModeler::MIPExpression* MilpComponent::getMIPExpression(std::string aExpressionName) {
    return mCompoModel->getMIPExpression(aExpressionName);
}
MIPModeler::MIPExpression1D* MilpComponent::getMIPExpression1D(std::string aExpressionName) {
    return mCompoModel->getMIPExpression1D(aExpressionName);
}
MIPModeler::MIPExpression& MilpComponent::getMIPExpression1D(uint i, std::string aExpressionName) {
    return mCompoModel->getMIPExpression1D(i, aExpressionName);
}

std::string MilpComponent::getUniquePortID() 
{
    std::string portId;
    bool isUnique = false;
    int n = PortList().size() + 1;
    while (!isUnique) {
        isUnique = true;
        portId = "port" + std::to_string(n);
        for(MilpPort * lptrport : PortList())
        {
            if (lptrport->ID() == portId) {
                isUnique = false;
                n = n + 1;
                break;
            }
        }
    }
    return portId;
}

const std::vector<MilpPort*> &MilpComponent::PortList() {
    return mCompoModel->PortList();
}

void MilpComponent::addPort(MilpPort* lptrport) {
    mCompoModel->addPort(lptrport);
}

void MilpComponent::removePort(MilpPort* lptrport) {
    mCompoModel->removePort(lptrport);
}

MilpPort* MilpComponent::getPort(const std::string& portId)
{
    return mCompoModel->getPort(portId);
}

MilpPort* MilpComponent::getPortByName(const std::string& aPortName)
{
    MilpPort* vRet = nullptr;    
    for(MilpPort * lptrport: PortList())
    {
        if (lptrport->Name() == aPortName) {
            vRet = lptrport;
            break;
        }         
    }
    return vRet;    
}

void MilpComponent::defineMainCarrier() {
    /**
    * Point the main carrier of non-Bus component to the EnergyVector of the first default Input port.
    * Note, BusCompo has only one EnergyVector, and it is set in OptimProblem::createLinksToBus
    */

    if(!mCompoModel) {
        cDebug() << Name() << ": the component model is not defined";
        return;
    }

    if (!allDefaultPortsHaveCarriers()) {
        cDebug() << Name() << ": not all default ports have carriers defined";
        return;
    }

    // Case 1: main carrier is defined inside the model 
    mCompoModel->defineMainCarrier();

    if (mCompoModel->getMainCarrier())
        return;

    // Case 2: First default consumption port or single default port
    const int numDefaultPorts = mCompoModel->DefaultPorts().size();
    const bool hasSingleDefaultPort = (numDefaultPorts == 1);

    for (MilpPort* port : PortList()) {
        if (!port || !port->IsDefaultPort() || !port->getCarrier()) {
            continue;
        }

        if (hasSingleDefaultPort || port->Direction() == KCONS()) {
            setMainCarrier(port->getCarrier());
            cDebug() << "Main carrier set from"
                << (hasSingleDefaultPort ? " single default port" : " consumption port");
            return;
        }
    }

    // Fallback: First available carrier (for components that don't have default ports)
    for (MilpPort* port : PortList()) {
        if (port && port->getCarrier()) {
            setMainCarrier(port->getCarrier());
            cDebug() << "Main carrier set from first available port as fallback";
            return;
        }
    }

    if (numDefaultPorts != 0) {
        cWarning() << "No suitable carrier found for component: " << Name();
    }
}

void MilpComponent::setMainCarrier(EnergyVector* aptrEnergyVector) {
    mCompoModel->setMainCarrier(aptrEnergyVector);
}  

EnergyVector* MilpComponent::getMainCarrier() const {
    return mCompoModel->getMainCarrier();
}   

int MilpComponent::defineDefaultVarNames() {
    return mCompoModel->defineDefaultVarNames();
}

void MilpComponent::createOnePort(const std::string& portId, 
    const t_mapParamData & portParams, EnergyVector* carrier)
{
    const std::string portName = CairnUtils::getParamValue(portParams, "Name");

    for(MilpPort* lptrport : PortList())
    {
        const std::string compoName = CairnUtils::getParamValue(portParams, "CompoName");

        if (lptrport->ID() == portId) {
            Cairn_Exception error("Error: componenet " + compoName + " already has a port with same ID " + portId + " (names " + portName + " and " + lptrport->Name() + ")", -1);
            throw error;
        }
        if (lptrport->Name() == portName) {
            Cairn_Exception error("Error: componenet " + compoName + " already has a port with same name " + portName + " (IDs " + portId + " and " + lptrport->ID() + ")", -1);
            throw error;
        }
    }

    MilpPort* lptrport = new MilpPort(this, portId, portName, portParams, carrier);
    addPort(lptrport);

    if (CairnUtils::getParamValue(portParams, "IsDefaultPort") == Yes()) {
        const std::string linkedPort = CairnUtils::getParamValue(portParams, portName);
        cDebug() << (linkedPort.empty() ? "Created default port " : "Created port ")
            << Name() << "." << portName
            << (linkedPort.empty() ? "" : " linked to " + linkedPort);
    }

    std::string vVar = lptrport->Variable();
    if (CairnUtils::contains(vVar, "INPUTFlux") || CairnUtils::contains(vVar, "OUTPUTFlux"))
    {
        if (lptrport->Variable() != "INPUTFlux1" && lptrport->Variable() != "OUTPUTFlux1" 
            && (mCompoModelClassName == "MultiConverter" || mCompoModelClassName == "Cogeneration"))
        {
            ModelIO* vIO = mCompoModel->getIOExpression(lptrport->Variable());
            if (vIO) {
                vIO->setUnit(lptrport->pFluxUnit()); 
            }
        }
    }
}

MilpPort* MilpComponent::mapDefaultPort(const std::string& portId,
    const std::map<std::string, std::string>& portParams)
{
    // --- Lookup by ID ------------------------------------------------
    if (MilpPort* port = getPort(portId))
        return port;

    // --- Lookup by Name ----------------------------
    auto itName = portParams.find("Name");
    if (itName != portParams.end()) {
        if (MilpPort* port = getPortByName(itName->second))
            return port;
    }

    // --- Fallback: single-port component with single default port -----------
    if (mPorts.size() == 1 && mCompoModel->DefaultPorts().size() == 1)
        return PortList()[0];

    // --- No match -----------------------------------------------------------
    return nullptr;
}


t_mapParamData MilpComponent::portData(const std::string& portId, const std::string& portName) const
{
    // Lookup by portId
    const auto it = mPorts.find(portId);
    if (it != mPorts.end()) {
        return it->second;
    }

    // Lookup by portName (backward compatibility) 
    for (const auto& [key, value] : mPorts)
    {
        if (CairnUtils::getParamValue(value, "Name") == portName) {
            return value;
        }
    }

    return {};
}


void MilpComponent::createPorts()
{
    //Create default ports first
    for (auto [portId, portParams] : mCompoModel->DefaultPorts()) {
        portParams["CompoName"] = Name();
        portParams["IsDefaultPort"] = Yes();
        createOnePort(portId, CairnUtils::convertToParamDataMap(portParams));
    }

    //Create other ports
    for (auto [portId, portParams] : mPorts) {  

        /**
         * JSON loading case:
         * The definition of TecEcoAnalysis port carriers must be postponed until
         * OptimProblem::createLinksToBus, because TecEcoAnalysis is created before
         * the EnergyVectors.
         *
         * Does TecEcoAnalysis really need to be created before the EnergyVectors?
         * Do EnergyVectors rely on TecEco information (e.g., Currency)?
         *
         * API creation case:
         * When creating ports through the API, an EnergyVector is provided at
         * creation time, so there is no issue.
         */

        // Get carrier
        EnergyVector* carrier = nullptr;
        const std::string carrierName = CairnUtils::getParamValue(portParams, "Carrier");
        if (!carrierName.empty()) {
            carrier = getCarrier(carrierName);
        }

        // Map or create port
        MilpPort* defaultPort = mapDefaultPort(portId, CairnUtils::convertFromParamDataMap(portParams));
        if (defaultPort) {
            defaultPort->completePortInfo(portParams, carrier);

            const std::string linkedBus = defaultPort->LinkedBusName();
            cDebug() << "Created default port " << Name() << "." << defaultPort->ID()
                << (linkedBus.empty() ? "" : " linked to " + linkedBus);
        }
        else {
            CairnUtils::setParamValue(portParams, "CompoName", Name());
            CairnUtils::setParamValue(portParams, "IsDefaultPort", No());
            createOnePort(portId, portParams, carrier);
        }
    }
}

EnergyVector* MilpComponent::getCarrier(const std::string& carrierName) 
{
    OptimProblem* optimProblem = dynamic_cast<OptimProblem*>(parent());
    if (!optimProblem) {
        cError() << "Parent is not a defined OptimProblem: " + Name();
        return nullptr;
    }

    EnergyVector* carrier = dynamic_cast<EnergyVector*>(optimProblem->findChild(carrierName));
    return carrier;
}

void MilpComponent::declareCompoInputParam()
{    
    mCompoInputParam->addParameter("ModelClass", &mCompoModelClassName, "", true, true, "ModelClass used", "");
    mCompoInputParam->addParameter("Control", &mControl, "", false, true, "Type of time control rolling horizon or MPC");
    mCompoInputParam->addParameter("DataFile", &mDataFile, "", false, true, "Path to .csv data file for 1D or 2D map definitions. Use semicolon to provide several files","file");
    mCompoInputParam->addParameter("PublishUserVariable", &mPublishUserVariable, "", false, true, "Full path to define text file for additionnal variables publication to output","file");
    mCompoInputParam->addParameter("submodelfile", &mSubmodelFile, "", false, true, "If model uses user dll path to this dll");
}

void MilpComponent::setCompoInputParam(const t_mapParamData& aComponent)
{
    int ierr = mCompoInputParam->readParameters(aComponent);
    if (ierr < 0) {
        throw Cairn_Exception("ERROR: while reading parameters of component " + Name(), -1);
    }
    if (Name().empty()) {
        throw Cairn_Exception("ERROR: a MilpComponent has an empty name: " + mCompoModelName, -1);
    }

    if (mCompoModelClassName.empty()) {
        mCompoModelClassName = mCompoModelName; /** backward compatibility */
    }

    if (mCompoModelName.empty()) {
        mCompoModelName = mCompoModelClassName;  
    }

    if (mCompoTechnoType.empty()) {
        mCompoTechnoType = mCompoModelName; /** image used in the GUI */
    }

    if (mCompoModel) {
        mCompoModel->setControlType(mControl);
    }
}

void MilpComponent::initGuiData(const t_mapParamData& paramMap)
{
    if (mGUIData) delete mGUIData;
    mGUIData = new GUIData(this);
    mGUIData->doInit(mCompoModelName, mCompoTechnoType, mType, paramMap);
}

bool MilpComponent::allDefaultPortsHaveVariables() 
{
    /* It doesn't ensure that the variables are valid */
    bool hasVariable = false;
    if (mCompoModel) {
        hasVariable = true;
        if (mType != "Bus") {
            /*
            * A Bus doesn't have any default port
            * Its default ports are a copy of the ports of linked components
            */
            for (MilpPort* lptrport : PortList())
            {
                if (lptrport->IsDefaultPort() && lptrport->Variable().empty())
                {
                    hasVariable = false;
                    break;
                }
            }
        }
    }
    return hasVariable;
}

bool MilpComponent::allDefaultPortsHaveCarriers() 
{
    bool hasEnergyVector = false;
    if (mCompoModel) {
        hasEnergyVector = true;
        if (mType != "Bus") {
            /*
            * A Bus doesn't have any default port
            * Its default ports are a copy of the ports of linked components
            */
            for (MilpPort* lptrport : PortList())
            {
                if (lptrport->IsDefaultPort() && !lptrport->getCarrier())
                {
                    hasEnergyVector = false;
                    break;
                }
            }
        }
    }
    return hasEnergyVector;
}

void MilpComponent::declareIndicators()
{
    if (mCompoModel && allDefaultPortsHaveVariables()) {
        mCompoModel->getInputIndicators()->removeIndicators();
        mCompoModel->declareModelIndicators();
    }
}

void MilpComponent::declareIOVariables()
{
    // Declare IO variables
    if (mCompoModel && allDefaultPortsHaveCarriers()) {
        removeIOs();
        mCompoModel->declareModelInterface();
    }
}

std::vector<std::string> MilpComponent::get_IOVarNames() const
{
    std::vector<std::string> vRet = {};

    if(mCompoModel) {
	    const SubModel::t_mapIOs& vIOMap = mCompoModel->getMapIOExpression();
	    for (auto& [vName, vIO] : vIOMap) {
		    if (vIO && vIO->IsUsed()) {
			    vRet.push_back(vName);
		    }
	    }
    }

    return vRet;
}

std::string MilpComponent::get_IOVarDescription(const std::string& varName) const
{
    if (mCompoModel) {
        const SubModel::t_mapIOs& vIOMap = mCompoModel->getMapIOExpression();
        for (auto& [vName, vIO] : vIOMap) {
            if (vName == varName && vIO) {
                return vIO->getDescription();
            }
        }
    }

    return {};
}

void MilpComponent::setXpos(const int& aXpos)
{
    if (mGUIData) mGUIData->setXpos(aXpos);
}

void MilpComponent::setYpos(const int& aYpos)
{
    if (mGUIData) mGUIData->setYpos(aYpos);
}

void MilpComponent::createImportListVars(t_mapExchange& a_Import)
{
    // import TS
    for (auto &[varName, var] : m_timeSeries) {
        if (var.getName() != "") {
            cDebug() << " -- Auto Adding to Subscribed Variables by createImportListVars " << varName << objectName() + "." + varName + "." + var.getName();
            std::string exName = Name() + "." + varName + "." + var.getName();
            var.subscribeTS(exName, a_Import, npdtTot());
        }       
    }
    
    // import MPC   
    if (mControl == "MPC") {
        for (auto& [varName, ivar1D] : mCompoModel->getListControlIO()) {            
            ivar1D->subscribeMPC(Name(), a_Import, npdtTot());
        }
    }
}

void MilpComponent::createExportListVars(t_mapExchange& a_Exchange)
{    
    if (mCompoModel) {
        for (auto& ivar1D : mCompoModel->getIOExpressions(EIOModelType::eMIPExpression1D)) {
            /* 
            * Note that, if the isUsed of an IO variables is modified after this point, then it will not be published 
            * The ideal place to publish the IO vars is in OptimProblem::buildProblem() after initSubModelInput(..) -- after computeInitialData(). 
            * For example, mAddStateVariable and mAddStartUpShutDownVariable are being updated in computeInitialData()
            * 
            * However, this will cause a problem for Pegase because the variables are exported in ModuleCairn::doInit()
            * 
            * Currently, createExportListVars(...) is called in OptimProblem::doInit(...)
            * This doesn't cause a problem for the API thanks to the re-initialization before run() !
            */
            if (ivar1D->isPExpr() && ivar1D->IsUsed()) {
                std::string varName = ivar1D->getName();
                std::string exName = Name() + "." + varName;
                a_Exchange[exName] = new ZEVariables(
                    exName,
                    mCompoModel->pExpUnitParam(varName),
                    varName);                
            }
        }
    }

    /*
    * Port variables (expressions) are published later on in MilpComponent::initSubModelInput().
    * A port variable can be published only after defining the value of its mVarType.
    * This can only be done on execution as user may change the port variable after component initialization
    * createPortsExportListVars(a_Exchange);
    */

     /** Define PublishUserVariable file for additionnal variables publication to output */
    if (!mPublishUserVariable.empty()) {
        createZEUserVariablesList(mPublishUserVariable, a_Exchange);
    }
}

void MilpComponent::createPortsExportListVars(t_mapExchange& a_Exchange) 
{
    for (MilpPort* port : PortList()) { 
        const MIPModeler::MIPExpression1D* ptrExp1D = getMIPExpression1D(port->Variable());
        if (ptrExp1D) {
            std::string varName = port->Variable();
            double aPort = port->Direction() == KCONS() ? -1.0 * port->VarCoeff() : port->VarCoeff();
            double bPort = port->Direction() == KCONS() ? -1.0 * port->VarOffset() : port->VarOffset(); 
            std::string exName = Name() + "." + port->Name() + "." + varName;

            a_Exchange[exName] =
                new ZEVariables(
                    exName,
                    mCompoModel->pExpUnitParam(varName),
                    varName,
                    std::to_string(aPort),
                    std::to_string(bPort));
        }
    }
}

void MilpComponent::readTSVariablesFromModel() {
    //Read Time Series variables from Model Data 
    mModelDataTS = mCompoModel->getInputTimeSeries();
    MilpComponent::readTSVariables(mModelDataTS); 
    mModelPortImpactParamTS = mCompoModel->getInputPortImpactsParamTS();
    MilpComponent::readTSVariables(mModelPortImpactParamTS);

    // Read Time Series from related EnergyVectors
    /*
    * Should be updated when changing the port carrier (postpone TS creation until a buildProblem).
    * Currently, it works properly thanks to initialize() before run()
    */
    for (MilpPort* port : PortList()) {
        if (!port) continue;
        EnergyVector* carrier = port->getCarrier();
        if (!carrier) continue;
        const InputParam* carrierTSParams = carrier->getTimeSeriesParam();
        readEnergyVectorTS(carrier, carrierTSParams);
    }
}

std::string MilpComponent::getTimeSeriesName(const std::string& ts_paramName)
{
    if (m_timeSeries.size() != 0) {
        //ModelTS have been created for all time series (after API::run)
        for (auto& [key, value] : m_timeSeries) {
            if (key == ts_paramName) {
                return value.getName();
            }
        }
    }
    else {
        //This is needed for the API to get the name before run! Before run, m_timeSeries is empty! 
        if (mComponent.find(ts_paramName)!=mComponent.end()) { 
            return CairnUtils::getParamValue(mComponent, ts_paramName); 
        }
    }

    return ""; 
}

std::map<std::string, std::string> MilpComponent::getTimeSeriesNames()
{
    std::vector<std::string> tsParamNameList;
    mCompoModel->getInputTimeSeries()->getParameters(tsParamNameList);
    mCompoModel->getInputPortImpactsParamTS()->getParameters(tsParamNameList);

    std::map<std::string, std::string> vRet;
    for (auto& paramName : tsParamNameList) {
        vRet[paramName] = getTimeSeriesName(paramName);
    }
    return vRet;
}

void MilpComponent::setTimeSeriesName(const std::string& ts_paramName, const std::string& ts_name) 
{
    /** @brief
    @ts_paramName: name of timeseries parameter e.g. "UseProfileLoadFlux" from SourceLoad
    @ts_name: name of the timeseries as appears in the input timeseries.csv
            That's, the string value of the parameter which is associated to a vector representing the real value
    */

    //update map! This is needed for the API before run!
    CairnUtils::setParamValue(mComponent, ts_paramName, ts_name);

    //update the name of the corresponding ModelTS (if already created)
    for (auto& [varName, var] : m_timeSeries) {
        if (varName == ts_paramName) {
            var.setName(ts_name);
        }
    }
}

bool MilpComponent::createModelTS(const std::string& varName, const std::string& tsName, ModelParam* aParamTS)
{
    /** 
    * varName : TS param name
    * tsName : TS name inside input file
    */

    if (!aParamTS) {
        return false;
    }

    // overwrite ?!
    if (m_timeSeries.count(varName))
    {
        cDebug() << " -- " << varName << " already added.";
        return false;
    }

    cDebug() << " -- Adding " << varName << " to the time series list";
    m_timeSeries[varName] = ModelTS(tsName, aParamTS->pUnitParam(), aParamTS);

    // Exception for Converter SetPoints
    const bool isConverter = mCompoModelName == "Converter"
        || CairnUtils::contains(mCompoModelClassName, ".Converter#");
    const bool isSetPoint = CairnUtils::contains(varName, ".InputSetPoint#")
        || CairnUtils::contains(varName, ".OutputSetPoint#");

    if (isConverter && isSetPoint) {
        ModelTS& ts = m_timeSeries[varName];
        ts.setName(varName);
    }

    return true;
}

void MilpComponent::readTSVariables(InputParam* aMapParamTS)
{
    for (auto const& [varName, param] : aMapParamTS->getMapParams()) {
        if (param->getType() != eVectorDouble) {
            continue;
        }
        createModelTS(varName, CairnUtils::getParamValue(mComponent, varName), param);
    }
}

void MilpComponent::readEnergyVectorTS(const EnergyVector* carrier, const InputParam* aMapParamTS)
{
    for (auto const& [varName, param] : aMapParamTS->getMapParams()) {
        if (!param->IsUsed()) {
            continue;
        }
        createModelTS(carrier->tsProfileID(varName), param->toString(), param);
    }
}


int MilpComponent::setTimeSeriesValues()
{
    int vRet = 0;
    for (auto& [varName, var] : m_timeSeries) {
        if (!var.getName().empty()) {
            cDebug() << " -- fill in vector FX " << mCompoModelName << varName << var.getName();
            var.set_Values(npdtPast());
        }
        else {
            cInfo() << "No timeseries name specified for " << varName << ", use default value: " << var.getDefault();
            var.set_Values(npdtTot(), var.getDefault());
        }
        if (!var.checkProfile()) {
            vRet = -1;
            break;
        }
    }        
    return vRet;   
}

void MilpComponent::exportRHVariableInModel()
{
    const InputParam::t_mapParams& vMapParams = mPlugSubmodelIO->getMapParams();
    for (auto& [varName, ivar1D] : mCompoModel->getListControlIO()) {
        
        ivar1D->set_Values(mControl, vMapParams, *mMilpData, (mFirstInit == 0));
    }

    if (mFirstInit == 0){        
        mFirstInit=1;
    }
}

void MilpComponent::createZEUserVariablesList(const std::string& Full_File_Name, t_mapExchange& a_Exchange)
{
    const char Separator = ';';

    if (!fs::exists(Full_File_Name)) {
        cError() << "User variable configuration file not found: " << Full_File_Name;
        return;
    }

    std::fstream File(Full_File_Name, std::ios_base::in);
    if (!File.is_open()) {
        cError() << "Could not open user variable configuration file: " << Full_File_Name;
        return;
    }

    std::string line;
    int lineNumber = 0;
    while (std::getline(File, line))
    {
        ++lineNumber;
        const std::vector<std::string> fields = split(line, Separator);

        // Check if stream went bad mid-read
        if (File.bad()) {
            cError() << "Read error in file:" << Full_File_Name
                << "at line:" << lineNumber;
            break;
        }

        if (fields.size() < 4) {
            cError() << "Invalid line " << lineNumber << " in " << Full_File_Name
                << " - expected at least 4 fields (component, variable, comment, unit)"
                << " - got " << fields.size() << " fields"
                << " - line: " << line;
            continue; 
        }

        const std::string varName = fields[1];
        const std::string exName = Name() + "." + varName;

        a_Exchange[exName] = new ZEVariables(
            exName,
            fields[3], // unit
            fields[2]  // comment
        );
    }

    // After loop - distinguish EOF from error
    if (File.fail() && !File.eof()) {
        cError() << "Unexpected read failure in:" << Full_File_Name;
    }
    else {
        cDebug() << "File read successfully:" << Full_File_Name;
    }
}

void MilpComponent::resetCompoModel()
{
    /*
    * Don't delete IOs (mIOExpressions) here
    */
    if (mCompoModel) {
        mCompoModel->closeExpressions(); /* It is better to stay before declareInputParams(Name()) */
        mCompoModel->declareInputParams(Name());
        mCompoModel->resetFlags();
    }

    cleanTimeSeries();
}

void MilpComponent::cleanTimeSeries()
{
    m_timeSeries.clear();
}


void MilpComponent::setSubModelEnvImpacts()
{
    if (!mTecEcoAnalysis) return;
    // Pass the info related to selected EnvImpacts from MilpComponent to SubModel
    mCompoModel->setEnvImpactsList(mTecEcoAnalysis->EnvImpactsList());
    mCompoModel->setEnvImpactsShortNamesList(mTecEcoAnalysis->EnvImpactShortNamesList());
    mCompoModel->setEnvImpactUnitsList(mTecEcoAnalysis->EnvImpactUnitsList());
    mCompoModel->setEnvImpactCosts(mTecEcoAnalysis->EnvImpactCosts());
}

void MilpComponent::redeclareEnvImpactParameters()
{
    if (!allDefaultPortsHaveCarriers()) {
        return;
    }

    // Init the list of considered environmental impacts
    setSubModelEnvImpacts();

    TechnicalSubModel* TechnicalCompoModel = dynamic_cast<TechnicalSubModel*> (mCompoModel);
    if (TechnicalCompoModel) {
        TechnicalCompoModel->cleanNonSelectedEnvImpacts();
        TechnicalCompoModel->createEnvImpacts(); //create the newly selected EnvImpacts
        TechnicalCompoModel->declareEnvImpactConfigurationParameters();
        declareIOVariables(); //TODO: filter only for Env Impact IOs?!
        TechnicalCompoModel->declareEnvImpactParameters();
        declareIndicators(); //TODO: filter only for Env Impact indicators?!
    }

    //Publish IO variables 
    initializeSubmodelIO();
}

int MilpComponent::initSubModelConfiguration(const bool& readParams)
{
    /** initSubModelConfiguration :
     * init SubModel timestep and horizon data
     * init list of SubModel parameters (scalar, double) and data (time series, performance parameters...)
     * */

    resetCompoModel();
    defineMainCarrier();

    // Init the list of considered environmental impacts
    setSubModelEnvImpacts();

    // init SubModel timesteps (constant and variable) and horizon data
    mCompoModel->setAbsoluteTimeStep(mMilpData->getAbsoluteTimeStep()) ;
    mCompoModel->setTimeshift(mMilpData->getTimeshift()) ;
    mCompoModel->setFuturesize(mMilpData->getIHMFuturSize()) ;
    mCompoModel->setTimeSteps(mMilpData->useVariableTimeSteps(), mMilpData->TimeSteps(), mMilpData->TimeStepBeginLP(), mMilpData->TimeStepBeginForecast(), mMilpData->DecreaseOptimizationHorizon());
    mCompoModel->setNpdtPast(mMilpData->npdtPast()) ;

    mCompoModel->setTimeData() ;

    mModelParam = mCompoModel->getInputParam();
    mModelPortImpactParam = mCompoModel->getInputPortImpactsParam();
    mModelEnvImpactParam = mCompoModel->getInputEnvImpactsParam();
    mModelPerfParam = mCompoModel->getInputPerfParam();

    //first delcare then read configuration parameters for other parameter settings.

    mCompoModel->declareModelConfigurationParameters();

    int ierr = 0;

    if (readParams) {
        //read configuration parameters
        ierr = mModelParam->readParameters(mComponent);
        if (ierr < 0) { cCritical() << " Error reading Parameters of SubModel " << (objectName()); return -1; }

        ierr = mModelPortImpactParam->readParameters(mComponent);
        if (ierr < 0) { cCritical() << " Error reading PortImpact of SubModel " << (objectName()); return -1; }

        ierr = mModelEnvImpactParam->readParameters(mComponent);
        if (ierr < 0) { cCritical() << " Error reading EnvImpact of SubModel " << (objectName()); return -1; }
    }

    /* 
    * It is recommended to declare IO variables before parameters in order
    * to set the value of OptimalSizeUnit which may get used in parameter units. 
    * But after the configuration parameters, because the number of IO variables, 
    * e.g. in MultiConverter and Cogeneration, depends on NbInputFlux and NbOutputFlux
    */
    declareIOVariables();

    /* After adding IO variables, now the indicators can be declared */
    declareIndicators();

    // now build list of SubModel parameters (int, bool, scalar, double, std::string) and data (time series, secundary parameters...)

    mCompoModel->declareModelParameters();

    mCompoModel->setTypicalPeriods(mMilpData->useTypicalPeriods(), mMilpData->TypicalPeriods(), mMilpData->NDtTypicalPeriods(), mMilpData->VectTypicalPeriods()) ; 

    //---------------------------------------------------------------------------------------------------------------------
    if (readParams) {
        // read dynamic input parameters at Component level    
        ierr = mInputParam->readParameters(mComponent);
        if (ierr < 0) { cCritical() << " Error reading Parameters of SubModel " << (objectName()); return -1; }
    }

    //---------------------------------------------------------------------------------------------------------------------

    if (readParams) {
        //read non-configuration parameters
        ierr = mModelParam->readParameters(mComponent);
        if (ierr < 0) { cCritical() << " Error reading Parameters of SubModel " << (objectName()); return -1; }

        ierr = mModelEnvImpactParam->readParameters(mComponent);
        if (ierr < 0) { cCritical() << " Error reading EnvImpact of SubModel " << (objectName()); return -1; }

        ierr = mModelPortImpactParam->readParameters(mComponent);
        if (ierr < 0) { cCritical() << " Error reading PortImpact of SubModel " << (objectName()); return -1; }
    }

    //Publish IO variables 
    initializeSubmodelIO();

    // Read performance-map files, if needed
    mModelPerfParam = mCompoModel->getInputPerfParam();
    if (readPerfMapFiles() < 0)
        return -1;


    mCompoModel->computeInitialData();

    return 0 ;
}

int MilpComponent::readPerfMapFiles()
{
    if (CairnUtils::simplified(mDataFile).empty())
        return 0; // nothing to do

    // Collect parameter names
    std::vector<std::string> perfParamNames;
    mModelPerfParam->getParameters(perfParamNames, EParamType::eVectorDouble);

    // Split file list
    std::vector<std::string> dataFiles =
        CairnUtils::contains(mDataFile, ";")
        ? CairnUtils::split(mDataFile, ';')
        : CairnUtils::split(mDataFile, ',');

    // Read each file
    for (const auto& file : dataFiles) {
        fs::path p(file);
        p = p.relative_path();
        mModelPerfParam->readVectorParameters(
            Name(),
            getAbsoluteFileName(p.string()),
            perfParamNames
        );
    }

    // Validate
    const auto& params = mModelPerfParam->getMapParams();
    bool missing = false;

    for (const auto& name : perfParamNames) {
        auto it = params.find(name);
        if (it == params.end())
            continue;

        if (it->second->IsBlocking()) {
            cError() << "ERROR readVectorParameters: No data found in DataFile "
                << mDataFile << " for variable " << Name() + "." + name;
            missing = true;
        }
        else if (GS::iVerbose > 0) {
            cWarning() << "Initialization not performed in readVectorParameters: No data found in "
                << mDataFile << " for expected variable " << Name() + "." + name;
        }
    }

    return missing ? -1 : 0;
}

int MilpComponent::initializeModel()
{
    if (setTimeSeriesValues() < 0)
        return -1;

    if (mCompoModel->checkConsistency() < 0) {
        cCritical() << "Error in component " << Name() << ": model data is not consistent";
        return -1;
    }

    if (checkPorts() < 0) {
        cCritical() << "Error in component " << Name() << ": ports are not well-defined";
        return -1;
    }

    return 0;
}

int MilpComponent::initSubModelInput()
{
    // Initialize model (initial data, RH variables, consistency, ports)
    if (initializeModel() < 0)
        return -1;

    return 0;
}

// TODO: move initProblem to CairnObject
int MilpComponent::initProblem(const bool& readParams)
{
    int ierr = initPorts();
    if (ierr < 0) return ierr;

    if (readParams || allDefaultPortsHaveCarriers()) {
        /*
        * GUI(works also for old studies where default ports are not marked) and reinitialize in api
        * A Bus component is configured at the creation because main carrier is directly set, 
        * but with readParams = false to avoid error due to mandatory parameters are not set yet.
        */
        ierr = initSubModelConfiguration(readParams);
        if (ierr < 0) return ierr;
    }

    return ierr;
}

int MilpComponent::initPorts()
{
    int ierr = 0;

    // define default variable names at ports
    ierr = defineDefaultVarNames();

    if (ierr < 0) {
        cCritical() << "ERROR in defining the port VarNames of component " + objectName();
        return ierr;
    }
    
    for (MilpPort* port : PortList()) 
    {
       ierr = port->initProblem(npdt()) ;
       if (ierr < 0) return ierr ;      
       
        const std::string portDirection = port->Direction() ;
        if (portDirection != KCONS() && portDirection != KPROD() && portDirection != KDATA()) {
            cCritical() << "Error: unknown direction at " << (Name() + "." + port->Name()) << " - " << portDirection;
            return -1 ;
        }
    }
    return ierr ;
}

int MilpComponent::checkPorts()
{
    if (!mCompoModel) {
        throw Cairn_Exception("The SubModel of " + Name() + " is not defined!", -1);
    }

    return mCompoModel->checkPorts();
}

void MilpComponent::deleteCompoModel()
{
    if (mModelFactory) {
        mModelFactory->deleteModel(mCompoModelClassName, Name());
    }
    mPorts.clear();
    mCompoModel = nullptr;
}

void MilpComponent::createCompoModel()
{
    if (mCompoModel == nullptr) {
        if (!newCompoModel()) {
            if (mModelFactory) {
                if (CairnUtils::contains(mModelFactory->getModelList(), mCompoModelClassName)) {
                    try {
                        mCompoModel =  (SubModel*) (mModelFactory->createModel(this, mCompoModelClassName, objectName()));
                    }
                    catch (...) {
                        Cairn_Exception error("ERROR while loading model " + mCompoModelClassName, -1);
                        throw error;
                    }
                    cDebug() << "model " + mCompoModelClassName + " has been successfully loaded!";
                }
            }
        }

        if (mCompoModel) {
            mCompoModel->setPortList({}); //clear port list
            mCompoModel->setParentCompo(this);
            mCompoModel->initDefaultPorts();
            createPorts();
            mCompoModel->setPortPointers();
        }
        else {
            Cairn_Exception error("Error : unknown model name " + mCompoModelClassName + " on component " + Name(), -1);
            throw error;
        }
    }
}

void MilpComponent::buildProblem()
{
    // Model component behaviour
    if (mCompoModel) {
        try {
            mCompoModel->buildControlVariables();
            mCompoModel->buildModel();     /**  define behaviour model and associated Variables */
            //Model Interface at ports
            setBusFluxPortExpression();       /**  send flux expressions to FlowBalanceBus */
            setBusSameValuePortExpression();  /**  publish expression to SameValueBus */
        }
        catch (Cairn_Exception cairn_error) {
            throw cairn_error;
        }
    }
}

void MilpComponent::setBusFluxPortExpression()
{
    for (MilpPort* port : PortList()) {

        const bool isFluxPort =
            port->PortType() == "BusFlowBalance" ||
            port->PortType() == "MultiObjCompo";

        if (!isFluxPort)
            continue;

        const double sign = (port->Direction() == KCONS()) ? -1.0 : 1.0;
        assignFluxToPort(port, sign);
    }
}

void MilpComponent::assignFluxToPort(MilpPort* port, double sign)
{
    const std::string var = port->Variable();

    // Case 1: 1D expression exists and matches time dimension
    MIPModeler::MIPExpression1D* exp1D = mCompoModel ? mCompoModel->getMIPExpression1D(var) : nullptr;

    if (exp1D && exp1D->size() == npdt()) {
        for (unsigned int t = 0; t < npdt(); ++t)
            port->setFlux(t, sign, (*exp1D)[t]); 
        return;
    }

    // Case 2: 0D expression
    MIPModeler::MIPExpression*  exp0D = mCompoModel ? mCompoModel->getMIPExpression(var) : nullptr;

    if (exp0D) {
        if (port->PortType() == "MultiObjCompo") {
            port->setFlux0D(sign, *exp0D);
        }
        else {
            for (unsigned int t = 0; t < npdt(); ++t)
                port->setFlux(t, sign, *exp0D);
        }
        return;
    }

    // Case 3: no expression found -> error
    throw Cairn_Exception("Component '" + Name() + "', port '" + port->Name() +
        "': expression '" + var + "' not found in model '" + mCompoModelName + "'", -1);
}

void MilpComponent::setBusSameValuePortExpression()
{
    if (!mCompoModel)
        return;

    for (MilpPort* port : PortList()) {
        if (port->PortType() == "BusSameValue") {
            assignPotentialToPort(port);
        }
    }
}

void MilpComponent::assignPotentialToPort(MilpPort* port)
{
    const std::string var = port->Variable();

    // Case 1: 1D expression with correct time dimension
    MIPModeler::MIPExpression1D* exp1D = mCompoModel ? mCompoModel->getMIPExpression1D(var) : nullptr;

    if (exp1D && exp1D->size() == npdt()) {
        for (unsigned int t = 0; t < npdt(); ++t)
            port->setPotential(t, (*exp1D)[t]);
        return;
    }

    // Case 2: 0D expression
    MIPModeler::MIPExpression* exp0D = mCompoModel ? mCompoModel->getMIPExpression(var) : nullptr;

    if (exp0D) {
        for (unsigned int t = 0; t < npdt(); ++t)
            port->setPotential(t, *exp0D);
        return;
    }

    // Case 3: no expression found -> error
    throw Cairn_Exception("Component '" + Name() + "', port '" + port->Name() +
        "': expression '" + var + "' not found in model '" + mCompoModelName + "'", -1);
}

bool MilpComponent::findFirstCoeff(std::string aVarName, t_mapExchange aList , float &coeff, float &offset)
{       
    for (auto& ivar : aList) {
       ZEVariables* lptrvar=ivar.second ;
       if (ivar.first == aVarName && lptrvar!=nullptr)
       {
           coeff = lptrvar->CoeffExport() ;
           offset = lptrvar->CoeffOffset() ;
           return true ;
       }
    }
    return false ;
}

void MilpComponent::prepareOptim()
{
    // On decale tout de timeshift
    //1. au premier passage, il est necessaire de les initialiser sur toute la longueur
    //2. au debut de chaque DoStep, il est necessaire d'efectuer un timeShift
    //3. apres chaque probleme d'optim, il est necessaire de mettre a  jour la partie future avec les resultats d'optim ou bien les
    //valeurs par defaut si le probleme n'a pas tourne.
    //mCompoModel->getInputParam()->getMapParamVXf()

    mCompoModel->decreaseOptimizationHorizon();

    std::vector<ModelParam*> vList;
    mPlugSubmodelIO->getParameters(vList, EParamType::eVectorEigen);
    for (auto& vParam : vList) {
        VectorXf* lptr = std::get< Eigen::VectorXf*>(vParam->getPtr());
        lptr->head(npdtPast()) = lptr->segment(timeshift(), npdtPast());
        if (mFirstInit != 1) {
            lptr->tail(npdt()).setConstant(NAN);
        }
    }    
}

void MilpComponent::populatePublishedVars(t_mapExchange& a_Export)
{
    uint modinitTS = 0;
    std::vector<ModelParam*> vList;
    mPlugSubmodelIO->getParameters(vList, EParamType::eVectorEigen);
    for (auto& vParam : vList) {
        std::string varName = vParam->getName();
        t_mapExchange::iterator vIter = a_Export.find(Name() + "." + varName);
        if (vIter != a_Export.end()) {
            ZEVariables* pVar = vIter->second;
            if (pVar->set_Values(vParam, pdtHeure(), TimeSteps(), npdtPast())) {
                if (mFirstInitTS == 0) {
                    if (pVar->update_PastValues(npdtPast(), timeshift()))
                        modinitTS = 1;
                }
            }
        }      
    }

    // automatically publish variables at ports !
    populatePublishedPortVars(a_Export, modinitTS);

    if (modinitTS == 1) mFirstInitTS = 1;
}

void MilpComponent::populatePublishedPortVars(t_mapExchange& a_Export, uint modinitTS)
{
    for (MilpPort* port : PortList()) {
        const MIPModeler::MIPExpression1D* ptrExp1D = getMIPExpression1D(port->Variable());
        if (ptrExp1D) {
            std::string varName = port->Variable();
            t_mapExchange::iterator vIter = a_Export.find(Name() + "." + port->Name() + "." + varName);
            if (vIter != a_Export.end()) {
                ZEVariables* pVar = vIter->second;
                if (pVar->set_Values(mPlugSubmodelIO->getParameter(varName), pdtHeure(), TimeSteps(), npdtPast())) {
                    if (mFirstInitTS == 0) {
                        if (pVar->update_PastValues(npdtPast(), timeshift()))
                            modinitTS = 1;
                    }
                }
            }
        }
    }
    if (modinitTS == 1) mFirstInitTS = 1;
}

void MilpComponent::initializeSubmodelIO()
{
    /*
     * Publish all IO variables. Then, in exportSubmodelIO(..), export only 
     * the IO variables that have isUsed == true.
     * Note: the value of isUsed may change later on, e.g., in buildModel().
     */
    for (auto& ivar1D : mCompoModel->getIOExpressions(EIOModelType::eMIPExpression1D))
    {
        const std::string varName = ivar1D->getName();

        cDebug() << " - AUTO_PlugSubmodelIO vector : " << objectName() << "." << varName
            << " size=" << npdtTot() << " timeSteps=" << TimeSteps().size();

        mPlugSubmodelIO->publishData(varName, npdtTot(), NAN);
    }

    setDefaultsResults();
}

void MilpComponent::setDefaultsResults()
{
    /*
 * Initialize default values for all vector-type IO parameters.
 * These vectors must have been allocated by the ModelParam constructor.
 */
    std::vector<ModelParam*> paramList;
    mPlugSubmodelIO->getParameters(paramList, EParamType::eVectorEigen);

    for (auto* param : paramList)
    {
        auto* vec = std::get<Eigen::VectorXf*>(param->getPtr());

        if (!vec || vec->size() == 0)
        {
            cInfo() << "MilpComponent::initializeSubmodelIO: parameter '"
                << param->getName()
                << "' should have been allocated by the component constructor! Skiped!";
            continue;
        }

        // Initialize only the tail (npdt() time steps)
        vec->tail(npdt()).setConstant(0.f);
    }
}

void MilpComponent::computeHistNbHours()
{
    float histNbHours = 0;
    for (unsigned int t = 0; t < timeshift() ; ++t)
    {
        histNbHours += (TimeStep(t));
    }
    setHistNbHours(std::ceil(histNbHours));
}

void MilpComponent::removeIOs()
{
    mCompoModel->removeIOs();
    if (mPlugSubmodelIO) delete mPlugSubmodelIO;
    mPlugSubmodelIO = new InputParam(this, "PlugSubmodelIO" + Name());
}

void MilpComponent::exportSubmodelIO(Solver* aSolver, int aNsol)
{
    /** Output Data (for link with PEGASE or OUTSIDE) */
    mFirstInit = 1;

    std::string gamsVarName = "";
    ModelerInterface* pExternalModeler = nullptr;
    const double* vOptimalSolution = nullptr;

    if (aSolver->getModelType() == GS::MIPMODELER ()) {
        vOptimalSolution = aSolver->getOptimalSolution(aNsol);
    }
    else{//Case of GAMS
        pExternalModeler = aSolver->getExternalModeler();
        if (pExternalModeler == nullptr) {
            cCritical() << "External solver" << aSolver->getModelType() << "is not defined!";
            return;
        }
    }

    //automatically get every 1D variables declared in SubModel IO stack
    const double* externalOptValue = nullptr;
    double value = 0.;
    for (auto& ivar1D : mCompoModel->getIOExpressions(EIOModelType::eMIPExpression1D))
    {
        //Only export used IO variables
        if (ivar1D->IsUsed())
        {
            MIPModeler::MIPExpression1D* ptrExp1D = (MIPModeler::MIPExpression1D*)(std::get<EIOModelType::eMIPExpression1D>(ivar1D->getPtr()));

            if (ptrExp1D->size() == 0) {
                cWarning() << "IO variable " + ivar1D->getName() + " has flag isUsed == true. But, the corresponding expression is not allocated.";
                continue; //skip IO variables whose expressions are not allocated
            }

            ModelParam* pParam = mPlugSubmodelIO->getParameter(ivar1D->getName());
            if (pParam) {
                Eigen::VectorXf* ptrSubmodelIO = std::get< Eigen::VectorXf*>(pParam->getPtr());
                if (ptrExp1D != nullptr) {
                    if (ptrSubmodelIO != nullptr) {
                        if (aSolver->getModelType() == GS::MIPMODELER()) {
                            std::vector<double> vValues = std::get<vector<double>>(ivar1D->evaluate(vOptimalSolution));

                            for (unsigned int t = 0; t < npdt(); ++t) {
                                (*ptrSubmodelIO)[t + npdtPast()] = vValues[t];
                            }
                        }
                        else if (pExternalModeler != nullptr) {
                            gamsVarName = Name() + "_v_" + ivar1D->getName();
                            externalOptValue = aSolver->getOptimalSolution(aNsol, gamsVarName);
                            for (unsigned int t = 0; t < npdt(); ++t) {
                                if (externalOptValue != nullptr) {
                                    value = externalOptValue[t];
                                }
                                else {
                                    cDebug() << aSolver->getModelType() << "::Variable key: " << gamsVarName << " not defined in " << aSolver->getModelType() << " model";
                                }
                                (*ptrSubmodelIO)[t + npdtPast()] = value;
                            }
                            delete externalOptValue;
                        }
                    }
                    else {
                        cWarning() << " - Solution1D for " << Name() << "." << ivar1D->getName() << " of model " << mCompoModelName << " cannot be saved : missing corresponding VectorXf in MilpComponent!";
                    }
                }
                else {
                    cWarning() << " - Vector Expression1D " << Name() << "." << ivar1D->getName() << " of model " << mCompoModelName << " has not been allocated in submodel!";
                }
            }
        }
    }

    //Evaluate 0D variables to store their values before clearing the expressions!!
    for (auto& ivar0D : mCompoModel->getIOExpressions(EIOModelType::eMIPExpression))
    {
        //evaluate the expression to store the value in m_evaluateExpr
        ivar0D->evaluate(vOptimalSolution);
    }
}

std::vector<MilpPort*> MilpComponent::listSidePorts(const std::string& aside)
{
    std::vector<MilpPort*> ptrlist;
    std::vector<MilpPort*> portList = PortList();
    for (auto &lptrport : portList)
    {
        if (lptrport->Position() == aside) {
            ptrlist.push_back(lptrport);
        }
        if (lptrport->Position() == "" && aside == Bottom()) {
            //add to bottom-side if position is not defined
            ptrlist.push_back(lptrport);
        }
    }
    return ptrlist;
}

void MilpComponent::jsonSaveGuiComponent(ojson &componentsArray, const std::string& componentCarrier, 
    const std::vector<std::string>& refLabelList)
{
    ojson compoObject;    
    mGUIData->jsonSaveGUILine(compoObject, componentCarrier);
  
    compoObject["nodePortsData"] = ojson::array();
    compoObject["paramListJson"] = ojson::array();
    compoObject["optionListJson"] = ojson::array();
    if (!isBus()) {
        compoObject["timeSeriesListJson"] = ojson::array();
        compoObject["envImpactsListJson"] = ojson::array();
        compoObject["portImpactsListJson"] = ojson::array();

        jsonSaveGUITimeSeries(compoObject["timeSeriesListJson"], mCompoModel->getInputTimeSeries());
        mCompoModel->getInputEnvImpactsParam()->jsonSaveGUIInputParam(compoObject["envImpactsListJson"]);
        mCompoModel->getInputPortImpactsParam()->jsonSaveGUIInputParam(compoObject["portImpactsListJson"]);
        jsonSaveGUITimeSeries(compoObject["portImpactsListJson"], mCompoModel->getInputPortImpactsParamTS());
    }

    mCompoModel->getInputParam()->jsonSaveGUIInputParam(compoObject["paramListJson"]);
    mCompoInputParam->jsonSaveGUIInputParam(compoObject["optionListJson"]);

    jsonSaveGUICompoNodePortsData(compoObject["nodePortsData"], compoObject["nodePorts"]);

    compoObject["labelListJson"] = ojson::array();
    for (auto const& [key, val] : mCompoModel->getLabelMap()) {
        if (std::find(refLabelList.begin(), refLabelList.end(), key) == refLabelList.end())
            continue; //if the label is not defined in TecEcoAnalysis then ignore it!
            ojson labelObject;
            labelObject["key"] = key;
            labelObject["value"] = val;
            compoObject["labelListJson"].push_back(labelObject);
    }

    componentsArray.push_back(compoObject);
}

void MilpComponent::jsonSaveGUICompoNodePortsData(ojson& nodePortsArray, ojson& nodePortsData)
{
    int portCount = listSidePorts(Left()).size();
    if (portCount) {
        nodePortsData[Left()] = portCount;
        jsonSaveGUINodePortsData(nodePortsArray, Left());
    }
    portCount = listSidePorts(Right()).size();
    if (portCount) {
        nodePortsData[Right()] = portCount;
        jsonSaveGUINodePortsData(nodePortsArray, Right());
    }
    portCount = listSidePorts(Bottom()).size();
    if (portCount) {
        nodePortsData[Bottom()] = portCount;
        jsonSaveGUINodePortsData(nodePortsArray, Bottom());
    }
    portCount = listSidePorts(Top()).size();
    if (portCount) {
        nodePortsData[Top()] = portCount;
        jsonSaveGUINodePortsData(nodePortsArray, Top());
    }
}

void MilpComponent::jsonSaveGUITimeSeries(ojson& timeSeriesArray, const InputParam* const inputParam)
{
    ojson paramObject;
    if (inputParam != nullptr) {
        const InputParam::t_mapParams& vMapParams = inputParam->getMapParams();
        for (auto& [key, param] : vMapParams) {
            paramObject["key"] = key;
            paramObject["value"] = getTimeSeriesName(key);
            timeSeriesArray.push_back(paramObject);
        }
    }
}

void MilpComponent::jsonSaveGUIlistPortsData(ojson &nodePortArray, const std::string& aSide)
{
    for (MilpPort* port : PortList()) {
        if (port->Position() == aSide) {
            port->jsonSaveGUIPortsData(nodePortArray);
        }
    }
}

void MilpComponent::jsonSaveGUINodePortsData(ojson &nodePortsArray, const std::string & aSide)
{
    ojson nodePortObject = ojson{
        {"ports", ojson::array()},
        {"pos", aSide}
    };    

    jsonSaveGUIlistPortsData(nodePortObject["ports"], aSide);

    if (!nodePortsArray.is_array()) {
        if (nodePortsArray.is_null()) {
            nodePortsArray = ojson::array();
        }
        else {
            cDebug() << "nodePortsArray must be an array; got type=" << nodePortsArray.type_name()
                << ". Re-initializing to array.";
            nodePortsArray = ojson::array();
        }
    }

    nodePortsArray.push_back(nodePortObject);
}

std::map<std::string, ModelParam*> MilpComponent::getParameters(bool includePortParams)
{
    std::map<std::string, ModelParam*> paramMap;

    auto mergeParams = [&] (std::map<std::string, ModelParam*> sourceMap) {
        paramMap.insert(sourceMap.begin(), sourceMap.end());
    };

    mergeParams(mCompoModel->getInputParam()->getMapParams());
    mergeParams(getCompoInputParam()->getMapParams());
    mergeParams(mCompoModel->getInputTimeSeries()->getMapParams());
    mergeParams(mCompoModel->getInputEnvImpactsParam()->getMapParams());
    mergeParams(mCompoModel->getInputPortImpactsParam()->getMapParams());
    mergeParams(mCompoModel->getInputPortImpactsParamTS()->getMapParams());

    if (includePortParams) {
        for (MilpPort* port : PortList()) {
            const std::string prefix = port->Name();
            for (const auto& [key, value] : port->getInputParam()->getMapParams()) {
                paramMap.emplace(prefix + "." + key, value);
            }
        }
    }

    return paramMap;
}

void MilpComponent::updateCompoParamMap(const std::string& a_SettingName, 
    const std::string& a_AttributeName, const std::string& a_AttributeValue)
{
    if (a_AttributeName == "value")
        CairnUtils::setParamValue(mComponent, a_SettingName, a_AttributeValue);
    else if (a_AttributeName == "comment")
        CairnUtils::setParamComment(mComponent, a_SettingName, a_AttributeValue);
}

void MilpComponent::updatePortParamMap(const std::string& portId, const std::string& portName,
    const std::string& a_SettingName, const std::string& a_AttributeName, const std::string& a_AttributeValue)
{
    // Try to find by portId first
    auto it = mPorts.find(portId);

    // If not found, try by portName (backward compatibility) 
    if (it == mPorts.end()) {
        it = std::find_if(mPorts.begin(), mPorts.end(),
            [&portName](const auto& pair) {
                const std::string name  = CairnUtils::getParamValue(pair.second, "Name");
                return name == portName;
            });
    }

    if (it != mPorts.end()) {
        // Update existing entry
        if (a_AttributeName == "value")
            CairnUtils::setParamValue(it->second, a_SettingName, a_AttributeValue);
        else if (a_AttributeName == "comment")
            CairnUtils::setParamComment(it->second, a_SettingName, a_AttributeValue);
    }
    else  {
        // Port not found, inserting new entry 
        if (a_AttributeName == "value")
            CairnUtils::setParamValue(mPorts[portId], a_SettingName, a_AttributeValue);
        else if (a_AttributeName == "comment")
            CairnUtils::setParamComment(mPorts[portId], a_SettingName, a_AttributeValue);
    }
}

bool MilpComponent::EnvironmentModel() {
    TechnicalSubModel* TechnicalCompoModel = dynamic_cast<TechnicalSubModel*> (mCompoModel);
    if (TechnicalCompoModel != nullptr) {
        return *(TechnicalCompoModel->pEnvironmentModel());
    }
    else {
        return false;
    }


}

bool MilpComponent::EcoInvestModel() {
    TechnicalSubModel* TechnicalCompoModel = dynamic_cast<TechnicalSubModel*> (mCompoModel);
    if (TechnicalCompoModel != nullptr) {
        return *(TechnicalCompoModel->pEcoInvestModel());
    }
    else {
        return false;
    }
}

std::vector<std::string> MilpComponent::get_ModelClassList()
{
    // TO DO: Dynamics with Private/dll model
    if (mType == "Converter") {
        return { "Electrolyzer",
        "PipelineBasic",
        "ThermalGroup",
        "ProductionUC",
        "Cogeneration",
        "FuelCell",
        "Dam",
        "Mixer",
        "Transportation",
        "NeuralNetwork",
        "HeatPump",
        "SMReformer",
        "Methaner",
        "Methanizer",
        "Compressor",
        "Converter",
        "MultiConverter",
        "HeatExchange",
        "PowerToFluidT",
        "PowerToFluidH",
        "Cluster"
        };
    }
    else
        return std::vector<std::string>();
}

bool MilpComponent::newCompoModel()
{
    // create MIPModel
   
    return (mCompoModel != nullptr);
}

bool MilpComponent::isBus() 
{
    return CairnUtils::isBus(mType);
}

std::string MilpComponent::getAbsoluteFileName(const std::string& filename)
{
    //Attention: OptimProblem is a MilpComponent
    //TODO: find a better way
    if (!fs::exists(filename)) {
        OptimProblem* optimProblem = (OptimProblem*) (this->parent()); 
        if (optimProblem) {
            return (optimProblem->projectDir() + filename);
        }
    }
    return filename;
}

std::vector<std::string> MilpComponent::possibleModelClasses() const
{
    if (mCompoModel) {
        return mCompoModel->possibleModelClasses();
    }
    return {};
}


std::vector<InputParam*> MilpComponent::get_InputParams()
{
    // Check default port carriers
    if (!allDefaultPortsHaveCarriers()) {
        throw Cairn_Exception("Please, configure the carriers of all default ports of component "
            + objectName() + " first.", -1);
    }

    std::vector<InputParam*> result;
    result.reserve(7);   // avoid reallocations

    // Add component-specific input param (always available)
    result.push_back(getCompoInputParam());

    // Add component model parameters if available
    if (auto* model = compoModel()) {
        result.push_back(model->getInputParam());
        result.push_back(model->getInputTimeSeries());
        result.push_back(model->getInputEnvImpactsParam());
        result.push_back(model->getInputPortImpactsParam());
        result.push_back(model->getInputPortImpactsParamTS());
    }

    // Add GUI parameters if available
    if (auto* gui = getGUIData()) {
        result.push_back(gui->getGuiInputParam());
    }

    return result;
}


std::vector<InputParam*> MilpComponent::get_ParamInputParams()
{
    if (!allDefaultPortsHaveCarriers()) {
        throw Cairn_Exception("Please, configure the carriers of all default ports of component "
            + objectName() + " first.", -1);
    }

    std::vector<InputParam*> result;
    if (auto* model = compoModel()) {
        result.push_back(model->getInputParam());
    }
    return result;
}

std::vector<InputParam*> MilpComponent::get_OptionInputParams()
{
    if (!allDefaultPortsHaveCarriers()) {
        throw Cairn_Exception("Please, configure the carriers of all default ports of component "
            + objectName() + " first.", -1);
    }

    std::vector<InputParam*> result;
    result.push_back(getCompoInputParam());
    return result;
}

std::vector<InputParam*> MilpComponent::get_TimeSeriesInputParams()
{
    if (!allDefaultPortsHaveCarriers()) {
        throw Cairn_Exception("Please, configure the carriers of all default ports of component "
            + objectName() + " first.", -1);
    }

    std::vector<InputParam*> result;
    if (auto* model = compoModel()) {
        result.push_back(model->getInputTimeSeries());
    }
    return result;
}

std::vector<InputParam*> MilpComponent::get_EnvImpactInputParams()
{
    if (!allDefaultPortsHaveCarriers()) {
        throw Cairn_Exception("Please, configure the carriers of all default ports of component "
            + objectName() + " first.", -1);
    }

    std::vector<InputParam*> result;
    if (auto* model = compoModel()) {
        result.push_back(model->getInputEnvImpactsParam());
    }
    return result;
}

std::vector<InputParam*> MilpComponent::get_PortEnvImpactInputParams()
{
    if (!allDefaultPortsHaveCarriers()) {
        throw Cairn_Exception("Please, configure the carriers of all default ports of component "
            + objectName() + " first.", -1);
    }

    std::vector<InputParam*> result;
    if (auto* model = compoModel()) {
        result.push_back(model->getInputPortImpactsParam());
        result.push_back(model->getInputPortImpactsParamTS());
    }
    return result;
}

InputParam* MilpComponent::get_PerfParam()
{
    if (!allDefaultPortsHaveCarriers()) {
        throw Cairn_Exception("Please, configure the carriers of all default ports of component "
            + objectName() + " first.", -1);
    }

    InputParam* result = nullptr;
    if (auto* model = compoModel()) {
        result = model->getInputPerfParam();
    }
    return result;
}

std::optional<double> MilpComponent::getIndicatorValue(const std::string& indicatorName, 
    const std::string& range) const 
{
    if (!compoModel() || !compoModel()->getInputIndicators()) {
        return std::nullopt;  
    }

    const auto& indicators = compoModel()->getInputIndicators()->getIndicators();

    for (const auto* indicator : indicators) {
        if (indicator && indicator->getName() == indicatorName) {
            if (range == "HIST") {
                return indicator->getValue(1);
            }
            else {
                return indicator->getValue(0); // PLAN
            }
        }
    }

    return std::nullopt;  // Not found
}

bool MilpComponent::isInstalled() const {
    return getIndicatorValue(IS_INSTALLED, "PLAN") != 0.0;
}

const std::vector<double>* MilpComponent::getTimeSeries(const std::string& varName) const
{
    const auto it = m_timeSeries.find(varName);
    if (it == m_timeSeries.end())
    {
        cWarning() << "Time series " << varName << " of component " << Name() << " not found!";
        return nullptr;
    }
    return it->second.get_Values(npdtPast());
}
