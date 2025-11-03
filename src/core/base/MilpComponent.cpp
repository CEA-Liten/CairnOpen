#if  (!defined(WIN32) && !defined(_WIN32))
#include <dlfcn.h>
#endif

#include "MilpComponent.h"
#include "SubModel.h"
#include "TechnicalSubModel.h"
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
                             std::string aName,
                             MilpData *aMilpData, TecEcoEnv &aTecEcoEnv,
                             const std::map<std::string, std::string> &aComponent,
                             const std::map < std::string, std::map<std::string, std::string> > &aPorts,
                             ModelFactory* aModelFactory) 
    :  TecEcoEnv(aParent, 
            aName,
            aTecEcoEnv.DiscountRate(),
            aTecEcoEnv.ImpactDiscountRate(),
            aTecEcoEnv.NbYear(),
            aTecEcoEnv.NbYearInput(),
            aTecEcoEnv.LeapYearPos(),
            aTecEcoEnv.ExtrapolationFactor(),
            aTecEcoEnv.Range()),
  mException(Cairn_Exception()),
  mComponent(aComponent),
  mPorts(aPorts),
  mMilpData(aMilpData),
  mType(CairnUtils::getParam(aComponent,"type")),
  mCompoModelName(CairnUtils::getParam(aComponent,"Model")), //for GUI
  mCompoTechnoType(CairnUtils::getParam(aComponent,"ModelTechnoType")), //for GUI
  mCompoModelClassName(CairnUtils::getParam(aComponent,"ModelClass")),
  mNports(0)
  {
    setObjectType("MilpComponent");
    mModelFactory = aModelFactory;
    mCompoModel = nullptr;
    
    mCompoInputParam = new InputParam (this,"CompoInputParam"+aName) ;
    mInputParam = new InputParam (this, "InputParam"+aName) ;                   /** List of COMPONENT Input parameters (for link with PEGASE or OUTSIDE) */
    mPlugSubmodelIO = new InputParam (this, "PlugSubmodelIO"+aName) ;           /** List of COMPONENT Output data (for link with PEGASE or OUTSIDE) Used for timeShifting, IMPORT and EXPORT wrt PEGASE exchange Zone */
    mTimeSeriesSubmodel = new InputParam (this, "TimeSeriesSubmodel"+aName) ;   /** List of COMPONENT TimeSeries input data (for link with PEGASE) Used for timeShifting, IMPORT and EXPORT wrt PEGASE exchange Zone */

    setTecEcoEnvSettings(aTecEcoEnv);

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
    initGuiData({ {"Xpos", mComponent["Xpos"]}, {"Ypos", mComponent["Ypos"]} }); //To be generalized for GUI data param
}

MilpComponent::~MilpComponent()
{
    if (mTimeSeriesSubmodel) delete mTimeSeriesSubmodel ;   /** List of COMPONENT TimeSeries input data (for link with PEGASE) Used for timeShifting, IMPORT and EXPORT wrt PEGASE exchange Zone */
    if (mPlugSubmodelIO) delete mPlugSubmodelIO ;           /** List of COMPONENT Output data (for link with PEGASE or OUTSIDE) Used for timeShifting, IMPORT and EXPORT wrt PEGASE exchange Zone */
    if (mInputParam) delete mInputParam ;                   /** List of COMPONENT Input parameters (for link with PEGASE or OUTSIDE) */
    if (mCompoInputParam) delete mCompoInputParam ;

    if (mCompoModel) delete mCompoModel;

    if (mGUIData) delete mGUIData;
}

void MilpComponent::setTecEcoEnvSettings(TecEcoEnv& aTecEcoEnv) 
{
    setRange(aTecEcoEnv.Range());
    setCurrency(aTecEcoEnv.Currency());
    if (mCompoModel) {
        mCompoModel->setCurrency(aTecEcoEnv.Currency());
    }
    setExtrapolationFactor(aTecEcoEnv.ExtrapolationFactor());
    setDiscountRate(aTecEcoEnv.DiscountRate());
    setImpactDiscountRate(aTecEcoEnv.ImpactDiscountRate());
    setInternalRateOfReturn(aTecEcoEnv.InternalRateOfReturn());
    setNbYear(aTecEcoEnv.NbYear());
    setNbYearOffset(aTecEcoEnv.NbYearOffset());
    setNbYearInput(aTecEcoEnv.NbYearInput());
    setLeapYearPos(aTecEcoEnv.LeapYearPos());
    setEnvImpactsList(aTecEcoEnv.EnvImpactsList());
    setEnvImpactsShortNamesList(aTecEcoEnv.EnvImpactsShortNamesList());
    setEnvImpactUnitsList(aTecEcoEnv.EnvImpactUnitsList());
    setEnvImpactCosts(aTecEcoEnv.EnvImpactCosts());
    setExtrapolationFactor(aTecEcoEnv.ExtrapolationFactor());
}

void MilpComponent::setMIPModel (MIPModeler::MIPModel* aModel)
{
    this->setLevelizationTable();
    this->setImpactLevelizationTable();
    this->setTableYearsHours();
    mModel = aModel;
    if (mCompoModel != nullptr)
        mCompoModel->setMIPModel(aModel);
    else
        cCritical() << "Coding error : setMIPModel called with non MIPModeler model !! " ;
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
std::string MilpComponent::ObjectiveType() { return mCompoModel->ObjectiveType(); }


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

    if (!allDefaultPortsHaveCarriers()) return;

    bool found = false;
    int numDefaultPorts = mCompoModel->DefaultPorts().size();
    for(MilpPort * lptrport: PortList())
    {
        if (lptrport->IsDefaultPort() && lptrport->getCarrier() != nullptr)
        {
            if (numDefaultPorts == 1 || lptrport->Direction() == KCONS()) {
                setMainCarrier(lptrport->getCarrier());
                found = true;
                break;
            }
        }
    }

    if (!found) {
        //Take the first port! (at least case of TecEcoAnalysis!)
        for(MilpPort * lptrport: PortList())
        {
            if (lptrport->getCarrier() != nullptr)
            {
                setMainCarrier(lptrport->getCarrier());
                break;
            }
        }
    }
}

void MilpComponent::setMainCarrier(EnergyVector* aptrEnergyVector) {
    mCompoModel->setMainCarrier(aptrEnergyVector);
}  

EnergyVector* MilpComponent::getMainCarrier() {
    return mCompoModel->getMainCarrier();
}   

int MilpComponent::defineDefaultVarNames() {
    return mCompoModel->defineDefaultVarNames();
}

void MilpComponent::createOnePort(const std::string& portId, const std::map<std::string, std::string>& portParams)
{
    std::string portName = portParams.at("Name");
    for(MilpPort* lptrport : PortList())
    {
        if (lptrport->ID() == portId) {
            Cairn_Exception error("Error: componenet " + portParams.at("CompoName") + " already has a port with same ID " + portId + " (names " + portName + " and " + lptrport->Name() + ")", -1);
            throw error;
        }
        if (lptrport->Name() == portName) {
            Cairn_Exception error("Error: componenet " + portParams.at("CompoName") + " already has a port with same name " + portName + " (IDs " + portId + " and " + lptrport->ID() + ")", -1);
            throw error;
        }
    }

    MilpPort* lptrport = new MilpPort(this, portId, portName, portParams);
    addPort(lptrport);
    mNports++;
    if (CairnUtils::getParam(portParams,"IsDefaultPort") == Yes()) {//For a default port, the linked component is not known at this point.
        if(CairnUtils::getParam(portParams,portName) != "")
            cInfo() << "Created port" << Name() +"."+portName << " linked to " << portParams.at(portName);
        else
            cInfo() << "Created default port" << Name() + "." + portName;
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

MilpPort* MilpComponent::mapDefaultPort(const std::string& portId, const std::map<std::string, std::string>& portParams)
{
    std::string portName = portParams.at("Name");
    MilpPort* defaultPort = getPort(portId);
    if (defaultPort != nullptr) {
        return defaultPort;
    }
    else {
        /* A mapping error may happen when two default ports are switched in the GUI. Shall also verify the port variable?! */
        defaultPort = getPortByName(portName);
        if (defaultPort != nullptr) {
            return defaultPort;
        }
        else if (mPorts.size() == 1 && mCompoModel->DefaultPorts().size() == 1) {
            defaultPort = PortList()[0];
            return defaultPort;
        }
    }
    return nullptr;
}

std::map<std::string, std::string> MilpComponent::portDataFromInputFile(const std::string& portId, const std::string& portName)
{
    if (mPorts.find(portId) != mPorts.end()) {
        return mPorts[portId];
    }
    else{
        for (auto& [key, value] : mPorts) {                  
            if (portName == value["Name"]) {
                return value;
            }
        }
    }
    return {};
}

void MilpComponent::createPorts()
{
    //clear port list if it is not empty!
    mNports = 0;
   // mCompoModel->clearPortList();

    //Create default ports first
    for (auto [portId, portParams] : mCompoModel->DefaultPorts()) {
        portParams["CompoName"] = Name();
        portParams["IsDefaultPort"] = Yes();
        createOnePort(portId, portParams);
    }

    //Create other ports
    for (auto [portId, portParams] : mPorts) {            
        MilpPort* defaultPort = mapDefaultPort(portId, portParams);
        if (defaultPort != nullptr) {
            //At this point there is no problem if the carrier (EnergyVector) is not set yet (in particular for the default ports)
            defaultPort->completePortInfo(portParams);
            cDebug() << "Created default port " << Name() << defaultPort->ID() << " linked to " << defaultPort->LinkedBusName();
        }
        else {
            portParams["CompoName"] = Name();
            portParams["IsDefaultPort"] = No();
            createOnePort(portId, portParams);
        }
    }
}

void MilpComponent::declareCompoInputParam()
{    
    //std::string
    mCompoInputParam->addParameter("ModelClass", &mCompoModelClassName, "", true, true, "ModelClass used", "");
    mCompoInputParam->addParameter("Control", &mControl, "", false, true, "Type of time control rolling horizon or MPC");
    mCompoInputParam->addParameter("DataFile", &mDataFile, "", false, true, "Path to .csv data file for 1D or 2D map definitions. Use semicolon to provide several files","file");
    mCompoInputParam->addParameter("PublishUserVariable", &mPublishUserVariable, "", false, true, "Full path to define text file for additionnal variables publication to output","file");
    mCompoInputParam->addParameter("submodelfile", &mSubmodelFile, "", false, true, "If model uses user dll path to this dll");
}

void MilpComponent::setCompoInputParam(const std::map<std::string, std::string> aComponent) 
{
    int ierr = mCompoInputParam->readParameters(aComponent);
    if (ierr < 0) {
        Cairn_Exception error("ERROR readParameters: missing value for parameter " + Name(), -1);
        throw error;
    }
    if (Name() == "") {
        cCritical() << "MilpComponent ERROR : No void name should be given by field id = ";
        assert(Name() != "");
    }

    if (aComponent.find("Model")!=aComponent.end()) {
        mCompoModelName = aComponent.at("Model");
    }
    else {
        mCompoModelName = mCompoModelClassName;
    }

    if (mCompoModelClassName == "")  mCompoModelClassName = mCompoModelName;
    
    if (aComponent.find("ModelTechnoType") != aComponent.end()) {
        mCompoTechnoType = aComponent.at("ModelTechnoType");
    }
    else {
        mCompoTechnoType = mCompoModelClassName;
    }

    if (mCompoModel) {
        mCompoModel->setControlType(mControl);
    }
}

void MilpComponent::initGuiData(const std::map<std::string, std::string>& paramMap)
{
    if (mGUIData) delete mGUIData;
    mGUIData = new GUIData(this);
    mGUIData->doInit(mCompoModelName, mCompoTechnoType, mType, paramMap);
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

void MilpComponent::declareIOVariables()
{
    //Declare IO variables
    if (mCompoModel && allDefaultPortsHaveCarriers())
    {
        removeIOs();
        mCompoModel->declareModelInterface();
    }
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
            ivar1D->subscribeMPC(Name(), a_Import);
        }
    }
}

void MilpComponent::createExportListVars(t_mapExchange& a_Exchange)
{    
    if (mCompoModel != nullptr)
    {
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

    if (mPublishUserVariable != "")  /** define file for additionnal variables publication to output */
    {
        createZEUserVariablesList(mPublishUserVariable, a_Exchange) ;
    }
}

void MilpComponent::createPortsExportListVars(t_mapExchange& a_Exchange) 
{
    for (MilpPort* port : PortList()) {    
        if (port->VarType() == "vector") {
            std::string varName = port->Variable();
            double aPort = port->VarCoeff();
            double bPort = port->VarOffset();
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
    mModelDataTS = mCompoModel->getInputDataTS();
    MilpComponent::readTSVariables(mModelDataTS); 
    mModelPortImpactParamTS = mCompoModel->getInputPortImpactsParamTS();
    MilpComponent::readTSVariables(mModelPortImpactParamTS);
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
            return mComponent[ts_paramName];
        }
    }

    return ""; 
}

std::map<std::string, std::string> MilpComponent::getTimeSeriesNames()
{
    std::vector<std::string> tsParamNameList;
    mCompoModel->getInputDataTS()->getParameters(tsParamNameList);
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
    mComponent[ts_paramName] = ts_name;

    //update the name of the corresponding ModelTS (if already created)
    for (auto& [varName, var] : m_timeSeries) {
        if (varName == ts_paramName) {
            var.setName(ts_name);
        }
    }
}

void MilpComponent::cleanTimeSeries()
{
    m_timeSeries.clear();
}


void MilpComponent::readTSVariables(InputParam* aMapParamTS)
{
    //Read time series variables
    const InputParam::t_mapParams& vSrcParams = aMapParamTS->getMapParams();
    for (auto const& [varName, val] : vSrcParams) {
        if (val) {
            if (val->getType() == eVectorDouble) {
                if (m_timeSeries.find(varName) != m_timeSeries.end()) {
                    cDebug() << " -- " << varName << " already added.";
                }
                else {
                    cDebug() << " -- Adding " << varName << " to the time series list";

                    m_timeSeries[varName] = ModelTS(mComponent[varName], val->pUnitParam(), val);

                    //Exception for Converter SetPoints
                    if (mCompoModelName == "Converter" || CairnUtils::contains(mCompoModelClassName,".Converter#")) {
                        if (CairnUtils::contains(varName, ".InputSetPoint#") || CairnUtils::contains(varName,".OutputSetPoint#")) {
                            m_timeSeries[varName].setName(varName);
                        }
                    }
                }
            }
        }
	}
}

int MilpComponent::createHistFXLists()
{
    int vRet = 0;
    for (auto& [varName, var] : m_timeSeries) {
        if (var.getName() != "") {
            cDebug() << " -- fill in vector FX " << mCompoModelName << varName << var.getName();
            var.set_Values(npdtPast());
        }
        else {
            cInfo() << "INFO : no " << varName << " series specified, use of default: " << var.getDefault();
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

void MilpComponent::createZEUserVariablesList(std::string Full_File_Name, t_mapExchange& a_Exchange)
{
//    std::string Full_File_Name="PublishUserVariable.csv" ;
    char Separator=';' ;    
    fs::path vFile(Full_File_Name);
    if (!fs::exists(vFile))
    {
         cInfo() << "no user configurationfile for published variable - keep default list" ;
         return ;
    }
    std::fstream File(Full_File_Name, std::ios_base::in);
    std::string line;
    while (std::getline(File, line))
    {
// read header line        
        std::vector<std::string> fields = split(line, Separator);
        if (contains(fields, "Error\n") || fields.size()<4)
        {
          cInfo() << "Error reading line " << line;
        }
        std::string compoName = fields[0] ;
        std::string varName  = fields[1] ;
        std::string acomment  = fields[2] ;
        std::string aunit = fields[3] ;
        std::string exName = Name() + "." + varName;
        a_Exchange[std::string(exName.c_str())] = new ZEVariables(
            std::string(exName.c_str()),
            std::string(aunit.c_str()),
            std::string(acomment.c_str()));
    }
}
void MilpComponent::initSubModelTopology()
{
    //mCompoModel->setTopo(mListPort) ;
    mCompoModel->setParentCompo(this) ;
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

int MilpComponent::initSubModelConfiguration(const bool& readParams)
{
    /** initSubModelConfiguration :
     * init SubModel timestep and horizon data
     * init list of SubModel parameters (scalar, double) and data (time series, performance parameters...)
     * */
 
    resetCompoModel();

    defineMainCarrier();

    // init SubModel topology data : list of ports
    initSubModelTopology();
    // Init the list of considered environmental impacts
    mCompoModel->setEnvImpactsList(mEnvImpactsList);
    mCompoModel->setEnvImpactsShortNamesList(mEnvImpactsShortNamesList); 
    mCompoModel->setEnvImpactUnitsList(mEnvImpactUnitsList);
    mCompoModel->setEnvImpactCosts(mEnvImpactCosts);

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

    /*
    * Publish all IO variables. Then, in exportSubmodelIO(..), export only the IO variables that have isUsed == True
    * Note, the value of isUsed may change later on, e.g., in buildModel()
    */
    for (auto& ivar1D : mCompoModel->getIOExpressions(EIOModelType::eMIPExpression1D))
    {        
        std::string varName = ivar1D->getName() ;
        cDebug() << " - AUTO_PlugSubmodelIO vector : " <<  objectName() +"."+varName << npdtTot() << TimeSteps().size();
        mPlugSubmodelIO->publishData(varName, npdtTot(), NAN) ;
    }

    return 0 ;
}

int MilpComponent::initSubModelInput()
{
    /** initSubModelInput :
     * init SubModel timestep and horizon data
     * init list of SubModel parameters (scalar, double) and data (time series, secundary parameters...)
     * direct reading of SubModel parameters (scalar, double) from aSettings file
     * if DataFile specified in Description.xml:
     * direct reading of SubModel array parameters (perf maps : vector, double) from file
     * file must be csv formatted (one head line, one column per variable)
     * indirect reading of SubModel time series (vector, double) from PEGASE subscribed vectors ()
     * fill SubModel vector Double timeseries from VectorXf values
     * indirect reading of SubModel data (scalar, double) from Cairn ()
     * fill SubModel scalar double from Component scalar double values **/

    // read dynamic input parameters at Submodel level
    int ierr = 0;

    mModelPerfParam = mCompoModel->getInputPerfParam();

    // if DataFile specified: direct reading of SubModel array parameters (perf maps : vector, double) from file
    if (CairnUtils::simplified(mDataFile) != "")
    {
        std::vector<std::string> perfParamNames;
        mModelPerfParam->getParameters(perfParamNames, EParamType::eVectorDouble);
        std::vector<std::string> dataFiles = {};
        if (CairnUtils::contains(mDataFile, ";")) {
            dataFiles = CairnUtils::split(mDataFile, ';');
        }
        else {
            dataFiles = CairnUtils::split(mDataFile, ',');
        }
        //Read
        for (int i = 0; i < dataFiles.size(); i++) {
            fs::path dataFile_i(dataFiles[i]);
            dataFile_i = dataFile_i.relative_path();
            mModelPerfParam->readVectorParameters(Name(), getAbsoluteFileName(dataFile_i.string()), perfParamNames);
        }
        //Verification
        bool NotFound = false;
        const InputParam::t_mapParams vParams = mModelPerfParam->getMapParams();
        for (int i = 0; i < perfParamNames.size(); i++) {
            InputParam::t_mapParams::const_iterator vIter = vParams.find(perfParamNames[i]);
            if (vIter != vParams.end()) {
                if (vIter->second->IsBlocking()) {
                    cCritical() << "ERROR readVectorParameters: No data found in DataFile " << mDataFile << " for variable name " << (Name() + "." + perfParamNames[i]);
                    NotFound = true;
                }
                else if (GS::iVerbose > 0) {
                    cWarning() << "Initialization not performed in readVectorParameters: No data found in " << mDataFile << " for expected variable name " << (Name() + "." + perfParamNames[i]);
                }
            }            
        }
        //
        if (NotFound) {
            return -1;
        }
    }

    /*
    * computeInitialData should stay before setParameters
    * because the intial state data (which set in computeInitialData) for ControlVar 
    * is used in exportRHVariableInModel() and createHistFXLists()
    */
    mCompoModel->computeInitialData();

    // set dynamic secondary parameters at Component level
    ierr = setParameters() ;
    if (ierr <0) return ierr ;

    // possible finalization step
    mCompoModel->finalizeModelData() ;

    //Check Consistency
    ierr = mCompoModel->checkConsistency() ;
    if (ierr <0 ) { cCritical() << " Error Model Data are not consistent "<< (objectName())  ; return -1 ; }

    ierr = checkPorts();
    if (ierr < 0)
    {
        Cairn_Exception error(" ERROR in component " + Name() + " for model " + mCompoModelName, -1);
        this->setException(error);
        return ierr;
    }

    //After checking ports, in particular defining the value of mVarType, now it is possible to publish port variables 
    OptimProblem* optimProblem = (OptimProblem*)this->parent();
    createPortsExportListVars(optimProblem->ListPublishedVariables());

    //---------------------------------------------------------------------------------------------------------------------
    //Indicators : declare only once, not every cycle
    if (mCompoModel->getInputIndicators()->getIndicators().size() == 0) {
        /* !!! Attention: declareModelIndicators should be called after checkPorts !!! */
        mCompoModel->declareModelIndicators();
    }

    return 0 ;
}

int MilpComponent::setParameters()
{
    MilpComponent::exportRHVariableInModel();
    return MilpComponent::createHistFXLists();
}

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
    
    int iIn = 0 ;
    int iOut = 0;
    int iHeatCarrierIn = 0 ;
    int iHeatCarrierOut = 0;
    int iData = 0;
    int numport = 0;
    for (MilpPort* port : PortList()) {

       ierr = port->initProblem(npdt()) ;
       if (ierr <0) return ierr ;      
       
        std::string varUse = port->Direction() ;
        if (varUse != KCONS() && varUse != KPROD() && varUse != KDATA()) 
        {
            cCritical() << "Error : wrong Variable type  at " << (Name() + "." + port->Name()) << " found : " << varUse;
            return -1 ;
        }
        numport++ ;
    }
    return ierr ;
}

int MilpComponent::checkPorts()
{
    // 1- Check that Variable exists as SubModel expression (0D or 1D)
    // 2- Warning if Use type has not been defined
    // 3- Check that units are consistent between Port (inherited from Bus) and SubModel expression
    int ierr = 0 ;
    for (MilpPort* port : PortList()) {
       ierr = mCompoModel->checkUnit(port) ;
       if (ierr <0)
       {
           cCritical() << ("Error checkUnit at port "+Name()+"."+port->Name());
           return ierr;
       }
       if (ierr >0)
       {
           cWarning() << ("Warning checkUnit at port "+Name()+"."+port->Name());
           ierr = 0 ;
       }
    }
    return ierr ;
}

void MilpComponent::deleteCompoModel()
{
    if (mModelFactory) {
        mModelFactory->deleteModel(mCompoModelClassName, Name());
    }
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
                    cInfo() << "model " + mCompoModelClassName + " has been successfully loaded!";
                }
            }
            
            if (mCompoModel) {
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
}

void MilpComponent::buildProblem()
{
    // Model component behaviour
    if (mCompoModel != nullptr)
    {
        try {
            mCompoModel->buildControlVariables();
            mCompoModel->buildModel();     /**  define behaviour model and associated Variables */
        }
        catch (Cairn_Exception cairn_error) {
            throw cairn_error;
        }
    }

    //Model Interface at ports
    try
    {
            setBusFluxPortExpression() ;       /**  send flux expressions to FlowBalanceBus */
            setBusSameValuePortExpression() ;  /**  publish expression to SameValueBus */
    }
    catch (...)
    {
       Cairn_Exception error (" ERROR in component "+Name()+" for model "+mCompoModelName ,-1) ;
       this->setException(error) ;
       return ;
    }
}

//void MilpComponent::resetFlags() {
//    if (mCompoModel) {
//        mCompoModel->resetFlags();
//    }
//}

void MilpComponent::setBusFluxPortExpression()
{    
    // whatever the port, use the same coefficient applied to Flow for balancing
    setBusFluxPortExpression(1.) ;
}

void MilpComponent::setBusFluxPortExpression(const double &aSignedCoefficient)
{
    // Caution : port.VarName should always be non void from InitProblem.
    // whatever the port, use the same coefficient applied to Flow for balancing
    // coefficient differ from one model to the other, depending on whether Weight exists or not...
    for (MilpPort* port : PortList()) {

       // flux has to integrate the sign : Positive if generated by the component, negative else.
       if (port->PortType()=="BusFlowBalance" || port->PortType()=="MultiObjCompo")
       {
           if (port->Direction() == KCONS())
           {
               setBusFluxPortExpression(port, -aSignedCoefficient);
           }
           else
           {
               setBusFluxPortExpression(port, aSignedCoefficient);
           }
       }
    }
}
void MilpComponent::setBusFluxPortExpression(MilpPort* port, const double &aSignedCoefficient)
{
    // Caution : port.VarName should always be non void from InitProblem.
    MIPModeler::MIPExpression1D* ptrExp1D ;
    MIPModeler::MIPExpression* ptrExp0D = nullptr ;
    std::string varName = port->Variable() ;
    //check expression has been provided by component Model, if checking missed in initProblem !

    if (mCompoModel != nullptr)
    {
      ptrExp1D = mCompoModel->getMIPExpression1D(varName);
      if (ptrExp1D != nullptr) {
          if ((*ptrExp1D).size() == npdt()) {
              for (unsigned int t = 0; t < npdt(); ++t)
              {
                  port->setFlux(t, aSignedCoefficient, (*ptrExp1D)[t]);   // contribution to Electrical_Bus
              }
          }
          else {
              ptrExp1D = nullptr;
          }
      }
      else if (port->PortType()=="MultiObjCompo"){
          ptrExp0D = mCompoModel->getMIPExpression(varName);
          if (ptrExp0D != nullptr) {
              port->setFlux0D(aSignedCoefficient, *ptrExp0D);
          }
      }
      else{
          ptrExp0D = mCompoModel->getMIPExpression(varName);
          if (ptrExp0D != nullptr) {
              for (unsigned int t = 0; t < npdt() ; ++t)
              {
                 port->setFlux(t, aSignedCoefficient, *ptrExp0D);   // contribution to Electrical_Bus
              }
          }
      }
    }
    else{
        ptrExp1D = nullptr;
        ptrExp0D = nullptr;
    }

    if (ptrExp1D == nullptr && ptrExp0D == nullptr)
    {
        //cCritical() << " ERROR at port "<< (Name()+"."+port->Name()+" MilpExpression "+varName+" does not exist in component model "+mCompoModelName) ;
        Cairn_Exception error (" ERROR at port "+Name()+"."+port->Name()+" MilpExpression "+ varName +" does not exist in component model "+mCompoModelName ,-1) ;
        this->setException(error) ;
        return ;
//        throw &error ;
    }

}
void MilpComponent::setBusSameValuePortExpression()
{
    MilpPort* port ;
    for (MilpPort* port : PortList()) {
       std::string varName = port->Variable() ;

       if (mCompoModel != nullptr)
       {
           if (port->PortType()=="BusSameValue")
           {
                MIPModeler::MIPExpression* ptrExp = mCompoModel->getMIPExpression(varName);
                MIPModeler::MIPExpression1D* ptrExp1D = mCompoModel->getMIPExpression1D(varName);
                if (ptrExp != nullptr)
                {
                    for (unsigned int t = 0; t < npdt() ; ++t)
                    {
                         port->setPotential(t, *ptrExp );   // Give access to Scalar value to BusSameValue
                    }
                }
                else if (ptrExp1D != nullptr)
                {
                    if ((*ptrExp1D).size() == npdt()) {
                        for (unsigned int t = 0; t < npdt(); ++t)
                        {
                            port->setPotential(t, (*ptrExp1D)[t]);   // Give access to Vector of values to BusSameValue
                        }
                    }
                    else {
                        ptrExp1D = nullptr;
                    }
                }
            
                if (ptrExp1D == nullptr && ptrExp == nullptr)
                {
                    Cairn_Exception error (" ERROR at port "+Name()+"."+port->Name()+" MilpExpression "+ varName +" does not exist in component model "+mCompoModelName ,-1) ;
                    this->setException(error) ;
                    return ;
                }
           }
       }
    }
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

    std::vector<InputParam::ModelParam*> vList;
    mPlugSubmodelIO->getParameters(vList, EParamType::eVectorEigen);
    for (auto& vParam : vList) {
        VectorXf* lptr = std::get< Eigen::VectorXf*>(vParam->getPtr());
        lptr->head(npdtPast()) = lptr->segment(timeshift(), npdtPast());
        if (mFirstInit != 1) {
            lptr->tail(npdt()).setConstant(NAN);
        }
    }    
}

void MilpComponent::exportResults(t_mapExchange& a_Export)
{
    uint modinitTS = 0;
    std::vector<InputParam::ModelParam*> vList;
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
    exportPortResults(a_Export, modinitTS);

    if (modinitTS == 1) mFirstInitTS = 1;
}

void MilpComponent::exportPortResults(t_mapExchange& a_Export, uint modinitTS)
{
    for (MilpPort* port : PortList()) {
        if (port->VarType() == "vector") {
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

void MilpComponent::setDefaultsResults()
{
    // Write default value
    std::vector<InputParam::ModelParam*> vList;
    mPlugSubmodelIO->getParameters(vList, EParamType::eVectorEigen);
    for (auto& vParam : vList) {
        VectorXf* lptr = std::get< Eigen::VectorXf*>(vParam->getPtr());            
        if (lptr->size() == 0)
        {
            cCritical () << "MilpComponen::setDefaultsResults " << vParam->getName() << " should have been allocated by component constructor ! "  ;
        }
        else
        {
            lptr->tail(npdt()).setConstant(0.);
        }
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

            InputParam::ModelParam* pParam = mPlugSubmodelIO->getParameter(ivar1D->getName());
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
                        cWarning() << " - Solution1D for " << objectName() << "." << ivar1D->getName() << " of model " << mCompoModelName << " cannot be saved : missing corresponding VectorXf in MilpComponent ! ";
                    }
                }
                else {
                    cWarning() << " - Vector Expression1D " << objectName() << "." << ivar1D->getName() << " of model " << mCompoModelName << " has not been allocated in submodel ! ";
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

        jsonSaveGUITimeSeries(compoObject["timeSeriesListJson"], mCompoModel->getInputDataTS());
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
    
    nodePortsArray.push_back(nodePortObject);
}

std::map<std::string, InputParam::ModelParam*> MilpComponent::getParameters()
{
    std::map<std::string, InputParam::ModelParam*> paramMap;

    paramMap.insert(mCompoModel->getInputParam()->getMapParams().begin(), mCompoModel->getInputParam()->getMapParams().end());
    paramMap.insert(getCompoInputParam()->getMapParams().begin(), getCompoInputParam()->getMapParams().end());
    paramMap.insert(mCompoModel->getInputDataTS()->getMapParams().begin(), mCompoModel->getInputDataTS()->getMapParams().end());
    
    paramMap.insert(mCompoModel->getInputEnvImpactsParam()->getMapParams().begin(), mCompoModel->getInputEnvImpactsParam()->getMapParams().end());
    paramMap.insert(mCompoModel->getInputPortImpactsParam()->getMapParams().begin(), mCompoModel->getInputPortImpactsParam()->getMapParams().end());
    paramMap.insert(mCompoModel->getInputPortImpactsParamTS()->getMapParams().begin(), mCompoModel->getInputPortImpactsParamTS()->getMapParams().end());

    return paramMap;
}

void MilpComponent::updateCompoParamMap(const std::string& a_SettingName, const t_value& a_SettingValue) {
    mComponent[(a_SettingName)] = std::string(CairnAPIUtils::getParamValue(a_SettingValue).c_str());
}

void MilpComponent::updateCompoParamMap(const t_dict& a_SettingValues) {
    for (auto& vParam : a_SettingValues) {
        updateCompoParamMap(vParam.first, vParam.second);
    }
}

bool MilpComponent::EnvironmentModel() {
    TechnicalSubModel* TechnicalCompoModel = dynamic_cast<TechnicalSubModel*> (mCompoModel);
    if (TechnicalCompoModel != nullptr) {
        return TechnicalCompoModel->EnvironmentModel();
    }
    else {
        return false;
    }


}
bool MilpComponent::EcoInvestModel() {
    TechnicalSubModel* TechnicalCompoModel = dynamic_cast<TechnicalSubModel*> (mCompoModel);
    if (TechnicalCompoModel != nullptr) {
        return TechnicalCompoModel->EcoInvestModel();
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
    if (mType == "BusFlowBalance" || mType == "BusSameValue" || mType == "MultiObjCompo") 
    {
        return true;
    }
    return false;
}

std::string MilpComponent::getAbsoluteFileName(const std::string& filename)
{
    if (!fs::exists(filename)) {
        OptimProblem* optimProblem = (OptimProblem*) (this->parent());
        if (optimProblem) {
            return (optimProblem->projectDir() + filename);
        }
    }
    return filename;
}
