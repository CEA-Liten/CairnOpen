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

using namespace CairnUtils;
using namespace CairnAPIUtils;
using namespace GS ;

using Eigen::Map;

MilpComponent::MilpComponent(QObject *aParent,
                             QString aName,
                             MilpData *aMilpData, TecEcoEnv &aTecEcoEnv,
                             const QMap<QString, QString> &aComponent,
                             const QMap < QString, QMap<QString, QString> > &aPorts,
                             ModelFactory* aModelFactory) 
    :  TecEcoEnv(aParent, aName,
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
  mName(aComponent["id"]),
  mType(aComponent["type"]),
  mCompoModelName(aComponent["Model"]),
  mCompoTechnoType(aComponent["ModelTechnoType"]),
  mCompoModelClassName(aComponent["ModelClass"]),
  mNports(0),
  mSens(1.)
  {
    mModelFactory = aModelFactory;

    mCompoModel = nullptr;

    mCompoInputParam = new InputParam (this,"CompoInputParam"+aName) ;
    mInputParam = new InputParam (this, "InputParam"+aName) ;                   /** List of COMPONENT Input parameters (for link with PEGASE or OUTSIDE) */
    mCompoToModel = new InputParam (this, "CompoToModel"+aName) ;               /** List of COMPONENT data to be sent to Model (derived secundary parameters...) */
    mPlugSubmodelIO = new InputParam (this, "PlugSubmodelIO"+aName) ;           /** List of COMPONENT Output data (for link with PEGASE or OUTSIDE) Used for timeShifting, IMPORT and EXPORT wrt PEGASE exchange Zone */
    mTimeSeriesSubmodel = new InputParam (this, "TimeSeriesSubmodel"+aName) ;   /** List of COMPONENT TimeSeries input data (for link with PEGASE) Used for timeShifting, IMPORT and EXPORT wrt PEGASE exchange Zone */

    setTecEcoEnvSettings(aTecEcoEnv);

  /** mComponent is used for architecture and topological or constant data */
  /** mSettings is used for "constant" parameters that can be made variable and optimized from outside */

    mFirstInit = 0 ;
    mFirstInitTS = 0;

  } // MilpCompoData()

void MilpComponent::initMilpComponent()
{
    createCompoModel();
    declareCompoInputParam();
    setCompoInputParam(mComponent);
    initGuiData();
}

MilpComponent::~MilpComponent()
{
    if (mTimeSeriesSubmodel) delete mTimeSeriesSubmodel ;   /** List of COMPONENT TimeSeries input data (for link with PEGASE) Used for timeShifting, IMPORT and EXPORT wrt PEGASE exchange Zone */
    if (mPlugSubmodelIO) delete mPlugSubmodelIO ;           /** List of COMPONENT Output data (for link with PEGASE or OUTSIDE) Used for timeShifting, IMPORT and EXPORT wrt PEGASE exchange Zone */
    if (mCompoToModel) delete mCompoToModel ;               /** List of COMPONENT data to be sent to Model (derived secundary parameters...) */
    if (mInputParam) delete mInputParam ;                   /** List of COMPONENT Input parameters (for link with PEGASE or OUTSIDE) */
    if (mCompoInputParam) delete mCompoInputParam ;

    if (mCompoModel) delete mCompoModel;
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
        qCritical() << "Coding error : setMIPModel called with non MIPModeler model !! " ;
}

MIPModeler::MIPExpression* MilpComponent::getMIPExpression(QString aExpressionName) {
    return mCompoModel->getMIPExpression(aExpressionName);
}
MIPModeler::MIPExpression1D* MilpComponent::getMIPExpression1D(QString aExpressionName) {
    return mCompoModel->getMIPExpression1D(aExpressionName);
}
MIPModeler::MIPExpression& MilpComponent::getMIPExpression1D(uint i, QString aExpressionName) {
    return mCompoModel->getMIPExpression1D(i, aExpressionName);
}
QString MilpComponent::ObjectiveType() { return mCompoModel->ObjectiveType(); }


QString MilpComponent::getUniquePortID() 
{
    QString portId;
    bool isUnique = false;
    int n = PortList().size() + 1;
    while (!isUnique) {
        isUnique = true;
        portId = "port" + QString::number(n);
        foreach(MilpPort * lptrport, PortList())
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

QList<MilpPort*> MilpComponent::PortList() {
    return mCompoModel->PortList();
}

void MilpComponent::addPort(MilpPort* lptrport) {
    mCompoModel->addPort(lptrport);
}

void MilpComponent::removePort(MilpPort* lptrport) {
    mCompoModel->removePort(lptrport);
}

MilpPort* MilpComponent::getPort(const QString& portId)
{
    return mCompoModel->getPort(portId);
}

MilpPort* MilpComponent::getPortByName(const QString& aPortName)
{
    MilpPort* vRet = nullptr;    
    foreach(MilpPort * lptrport, PortList())
    {
        if (lptrport->Name() == aPortName) {
            vRet = lptrport;
            break;
        }         
    }
    return vRet;    
}

void MilpComponent::defineMainEnergyVector() {
    /**
    * Point the main carrier of non-Bus component to the EnergyVector of the first default Input port. 
    * Note, BusCompo has only one EnergyVector, and it is set in OptimProblem::createPortsAndLinksToBus from mVectorName
    */

    bool found = false;
    int numDefaultPorts = mCompoModel->DefaultPorts().count();
    foreach(MilpPort * lptrport, PortList())
    {
        if (lptrport->IsDefaultPort() && lptrport->ptrEnergyVector() != nullptr)
        {
            if (numDefaultPorts == 1 || lptrport->Direction() == KCONS()) {
                setEnergyVector(lptrport->ptrEnergyVector());
                found = true;
                break;
            }
        }
    }

    if (!found) {
        //Take the first port! (at least case of TecEcoAnalysis!)
        foreach(MilpPort * lptrport, PortList())
        {
            if (lptrport->ptrEnergyVector() != nullptr)
            {
                setEnergyVector(lptrport->ptrEnergyVector());
                break;
            }
        }
    }
}

void MilpComponent::setEnergyVector(EnergyVector* aptrEnergyVector) {
    mCompoModel->setEnergyVector(aptrEnergyVector); 
}  

EnergyVector* MilpComponent::getEnergyVector() {
    return mCompoModel->getEnergyVector(); 
}   

int MilpComponent::defineDefaultVarNames() {
    return mCompoModel->defineDefaultVarNames();
}

void MilpComponent::createOnePort(const QString& portId, const QMap<QString, QString>& portParams)
{
    QString portName = portParams["Name"];
    foreach(MilpPort* lptrport, PortList())
    {
        if (lptrport->ID() == portId) {
            Cairn_Exception error("Error: componenet " + portParams["CompoName"] + " already has a port with same ID " + portId + " (names " + portName + " and " + lptrport->Name() + ")", -1);
            throw error;
        }
        if (lptrport->Name() == portName) {
            Cairn_Exception error("Error: componenet " + portParams["CompoName"] + " already has a port with same name " + portName + " (IDs " + portId + " and " + lptrport->ID() + ")", -1);
            throw error;
        }
    }

    MilpPort* lptrport = new MilpPort(this, portId, portName, portParams);
    addPort(lptrport);
    mNports++;
    if (portParams["IsDefaultPort"] == Yes()) {//For a default port, the linked component is not known at this point.
        qDebug() << "Created port " << Name() << portName << " linked to " << portParams[portName];
    }
    if (lptrport->Variable().contains("INPUTFlux") || lptrport->Variable().contains("OUTPUTFlux")) 
    {
        if (lptrport->Variable() != "INPUTFlux1" && lptrport->Variable() != "OUTPUTFlux1" 
            && (mCompoModelClassName == "MultiConverter" || mCompoModelClassName == "Cogeneration"))
        {
            ModelIO* vIO = mCompoModel->getIOExpression(lptrport->Variable());
            if (vIO && vIO->getPtrUnit() == nullptr) {
                vIO->setPtrUnit(lptrport->pFluxUnit());
            }
        }
    }
}

MilpPort* MilpComponent::mapDefaultPort(const QString& portId, const QMap<QString, QString>& portParams)
{
    QString portName = portParams["Name"];
    MilpPort* defaultPort = getPort(portId);
    if (defaultPort != nullptr) {
        return defaultPort;
    }
    else {
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

void MilpComponent::createPorts()
{
    //clear port list if it is not empty!
    mNports = 0;
   // mCompoModel->clearPortList();

    //Create default ports first
    for (auto& portId : mCompoModel->DefaultPorts().keys()) {
        QMap<QString, QString> portParams = mCompoModel->DefaultPorts()[portId];
        portParams["IsDefaultPort"] = Yes();
        createOnePort(portId, portParams);
    }

    //Create other ports
    for (auto& portId : mPorts.keys()) {
        QMap<QString, QString> portParams = mPorts[portId];
        MilpPort* defaultPort = mapDefaultPort(portId, portParams);
        if (defaultPort != nullptr) {
            //At this point there is no problem if the carrier (EnergyVector) is not set yet (in particular for the default ports)
            defaultPort->completePortInfo(portParams);
            qDebug() << "Created default port " << Name() << defaultPort->ID() << " linked to " << defaultPort->LinkedComponent();
        }
        else {
            portParams["IsDefaultPort"] = No();
            createOnePort(portId, portParams);
        }
    }
}

void MilpComponent::declareCompoInputParam()
{    
    //QString
    // this is not an Option of the Component !! mCompoInputParam->addParameter("Model",&mComponent["Model"],true,"Model used","",{"DONOTSHOW"});
    mCompoInputParam->addParameter("ModelClass", &mCompoModelClassName, "", true, true, "ModelClass used", "", {});
    mCompoInputParam->addParameter("Xpos", &mXpos, 0, false, true,"X position on planteditor","",{"DONOTSHOW"});
    mCompoInputParam->addParameter("Ypos", &mYpos, 0, false, true,"Y position on planteditor","",{"DONOTSHOW"});
    mCompoInputParam->addParameter("Control", &mControl, "", false, true, "Type of time control rolling horizon or MPC");
    mCompoInputParam->addParameter("DataFile", &mDataFile, "", false, true, "Path to .csv data file for 1D or 2D map definitions. Use ; to provide several files","file",{""});
    mCompoInputParam->addParameter("PublishUserVariable", &mPublishUserVariable, "", false, true, "Full path to define text file for additionnal variables publication to output","file",{""});
    mCompoInputParam->addParameter("submodelfile", &mSubmodelFile, "", false, true, "If model uses user dll path to this dll", "");
}

void MilpComponent::setCompoInputParam(const QMap<QString, QString> aComponent) 
{
    int ierr = mCompoInputParam->readParameters(aComponent);
    if (ierr < 0) {
        Cairn_Exception error("ERROR readParameters: missing value for parameter " + mName, -1);
        throw& error;
    }
    if (mName == "") {
        qCritical("MilpComponent ERROR : No void name should be given by field id= ");
        Q_ASSERT(mName != "");
    }

    if (aComponent.contains("Model")) {
        mCompoModelName = aComponent["Model"];
    }
    else {
        mCompoModelName = mCompoModelClassName;
    }

    if (mCompoModelClassName == "")  mCompoModelClassName = mCompoModelName;
    
    if (aComponent.contains("ModelTechnoType")) {
        mCompoTechnoType = aComponent["ModelTechnoType"];
    }
    else {
        mCompoTechnoType = mCompoModelClassName;
    }
    
    mCompoToModel->publishData("ControlType", &mControl);
}

void MilpComponent::initGuiData() 
{
    mGUIData = new GUIData(this, mName);
    mGUIData->doInit(mCompoModelName, mCompoTechnoType, mType, mXpos, mYpos);
}


void MilpComponent::setXpos(const int& aXpos)
{
    mXpos = fmax(0., fmin(aXpos, MAX_X));
    if (mGUIData) mGUIData->setXpos(aXpos);
}

void MilpComponent::setYpos(const int& aYpos)
{
    mYpos = fmax(0., fmin(aYpos, MAX_Y));
    if (mGUIData) mGUIData->setYpos(aYpos);
}

void MilpComponent::createExchangeListVars(t_mapExchange& a_Import, t_mapExchange& a_Export)
{
    createImportListVars(a_Import);
    createExportListVars(a_Export);
}

void MilpComponent::createImportListVars(t_mapExchange& a_Import)
{
    // import TS
    for (auto &[varName, var] : m_timeSeries) {
        if (var.getName() != "") {
            qDebug() << " -- Auto Adding to Subscribed Variables by createImportListVars " << varName << mName + "." + varName + "." + var.getName();
            QString exName = mName + "." + varName + "." + var.getName();
            var.subscribeTS(exName, a_Import, npdtTot());
        }       
    }
    
    // import MPC   
    if (mControl == "MPC") {
        for (auto& [varName, ivar1D] : mCompoModel->getListControlIO()) {            
            ivar1D->subscribeMPC(mName, a_Import);
        }
    }
}

void MilpComponent::createExportListVars(t_mapExchange& a_Exchange)
{    
    if (mCompoModel != nullptr)
    {
        for (auto& ivar1D : mCompoModel->getIOExpressions(EIOModelType::eMIPExpression1D)) {
            if (ivar1D->isPExpr()) {
                QString varName = ivar1D->getName();
                QString exName = mName + "." + varName;
                a_Exchange[exName] = new ZEVariables(                        
                        exName,
                        mCompoModel->getUnit(varName),
                        varName);                
            }
        }
    }
    // automatically publish variables at ports !    
    createPortsExportListVars(a_Exchange);

    if (mCompoInputParam->getParamQSValue("PublishUserVariable") != "")  /** define file for additionnal variables publication to output */
    {
        createZEUserVariablesList(mCompoInputParam->getParamQSValue("PublishUserVariable"), a_Exchange) ;
    }
}

void MilpComponent::createPortsExportListVars(t_mapExchange& a_Exchange) 
{
    QListIterator<MilpPort*> iport(PortList());
    while (iport.hasNext()) {
        MilpPort* port = iport.next();
        if (port->VarType() == "vector") {
            QString varName = port->Variable();
            double aPort = port->VarCoeff();
            double bPort = port->VarOffset();
            QString exName = mName + "." + port->Name() + "." + varName;

            a_Exchange[exName] =
                new ZEVariables(
                    exName,
                    mCompoModel->getUnit(varName),
                    varName,
                    QString::number(aPort),
                    QString::number(bPort));

        }
    }
}

void MilpComponent::createZEVariablesList()
{     

}

void MilpComponent::readTSVariablesFromModel() {
    //Read Time Series variables from Model Data 
    MilpComponent::readTSVariables(mModelDataTS);    
    MilpComponent::readTSVariables(mModelPortImpactParamTS);
}

QString MilpComponent::getTimeSeriesName(const QString& ts_paramName)
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
        //TODO: Shall we always use mComponent or create ModelTS earlier : call createZEVariablesList() in MilpComponent::initProblem() ?!
        if (mComponent.contains(ts_paramName)) { 
            return mComponent[ts_paramName];
        }
    }

    return ""; 
}

QMap<QString, QString> MilpComponent::getTimeSeriesNames()
{
    QList<QString> tsParamNameList;
    mCompoModel->getInputDataTS()->getParameters(tsParamNameList);
    mCompoModel->getInputPortImpactsParamTS()->getParameters(tsParamNameList);

    QMap<QString, QString> vRet;
    for (auto& paramName : tsParamNameList) {
        vRet[paramName] = getTimeSeriesName(paramName);
    }
    return vRet;
}

void MilpComponent::setTimeSeriesName(const QString& ts_paramName, const QString& ts_name) 
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

void MilpComponent::readTSVariables(InputParam* aMapParamTS)
{
    //Read time series variables
    const InputParam::t_mapParams& vSrcParams = aMapParamTS->getMapParams();
    for (auto const& [varName, val] : vSrcParams) {
        if (val) {
            if (val->getType() == eVectorDouble) {
                if (m_timeSeries.find(varName) != m_timeSeries.end()) {
                    qDebug() << " -- " << varName << " already added.";
                }
                else {
                    qDebug() << " -- Adding " << varName << " to the time series list";

                    m_timeSeries[varName] = ModelTS(mComponent[varName],
                        mCompoModel->convertUnits(PortList()[0]->ptrEnergyVector(), val->getQuantities()), val);

                    //Exception for Converter SetPoints
                    if (mCompoModelName == "Converter" || mCompoModelClassName.contains(".Converter#")) {
                        if (varName.contains(".InputSetPoint#") || varName.contains(".OutputSetPoint#")) {
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
            qDebug() << " -- fill in vector FX " << mCompoModelName << varName << var.getName();
            var.set_Values(npdtPast());
        }
        else {
            qInfo() << "INFO : no " << varName << " series specified, use of default: " << var.getDefault();
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

void MilpComponent::createZEUserVariablesList(QString Full_File_Name, t_mapExchange& a_Exchange)
{
//    QString Full_File_Name="PublishUserVariable.csv" ;
    QString Separator=";" ;

    QStringList fields ;
    QFile File (Full_File_Name) ;
    QTextStream FileStream_In (&File);

    if (!File.open(QIODevice::ReadOnly))
    {
         qInfo() << "no user configurationfile for published variable - keep default list" ;
         return ;
    }

    while ( ! FileStream_In.atEnd())
    {
// read header line
        QString line = FileStream_In.readLine();
        fields = line.split(Separator);
        if ( fields.contains(QString("Error\n")) )
        {
          qInfo() << "Error reading line " << line ;
        }
        QString compoName = fields[0] ;
        QString varName  = fields[1] ;
        QString acomment  = fields[2] ;
        QString aunit = fields[3] ;
        QString exName = mName + "." + varName;
        a_Exchange[exName] = new ZEVariables(
            exName,
            aunit,
            acomment);
    }
}
void MilpComponent::initSubModelTopology()
{
    //mCompoModel->setTopo(mListPort) ;
    mCompoModel->setParentCompo(this) ;
}

int MilpComponent::initSubModelConfiguration()
{
    /** initSubModelInput :
     * init SubModel timestep and horizon data
     * init list of SubModel parameters (scalar, double) and data (time series, secundary parameters...)
     * */

    mCompoModel->setParent(this);

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
    mCompoModel->setTimeSteps(mMilpData->TimeSteps(), mMilpData->TimeStepBeginLP(), mMilpData->TimeStepBeginForecast(), mMilpData->DecreaseOptimizationHorizon()) ;
    mCompoModel->setNpdtPast(mMilpData->npdtPast()) ;

    mCompoModel->setTimeData() ;

    mModelParam = mCompoModel->getInputParam() ;
    mModelData = mCompoModel->getInputData() ;
    mModelDataTS = mCompoModel->getInputDataTS();
    
    mModelPortImpactParam = mCompoModel->getInputPortImpactsParam();
    mModelPortImpactParamTS = mCompoModel->getInputPortImpactsParamTS();
    mModelPerfParam = mCompoModel->getInputPerfParam();
    mModelEnvImpactParam = mCompoModel->getInputEnvImpactsParam();

    //first delcare then read configuration parameters for other parameter settings.

    mCompoModel->declareModelConfigurationParameters();

    int ierr = 0;
    //read configuration parameters
    ierr = mModelParam->readParameters(mComponent);
    if (ierr < 0) { qCritical() << " Error reading Parameters of SubModel " << (mName); return -1; }
        
    ierr = mModelPortImpactParam->readParameters(mComponent);
    if (ierr < 0) { qCritical() << " Error reading PortImpact of SubModel " << (mName); return -1; }

    ierr = mModelEnvImpactParam->readParameters(mComponent);
    if (ierr < 0) { qCritical() << " Error reading EnvImpact of SubModel " << (mName); return -1; }

    //fill data transmitted from MilpComponent
    ierr = mModelData->fillData(mName, *mCompoToModel, eBool); // transmit
    if (ierr < 0) { qCritical() << " Error filling DBLE scalar data of SubModel from Component data " << (mName); return -1; }

    ierr = mModelData->fillData(mName, *mCompoToModel, eInt); // transmit
    if (ierr < 0) { qCritical() << " Error filling DBLE scalar data of SubModel from Component data " << (mName); return -1; }

    ierr = mModelData->fillData(mName, *mCompoToModel, eDouble); // transmit
    if (ierr < 0) { qCritical() << " Error filling DBLE scalar data of SubModel from Component data " << (mName); return -1; }

    ierr = mModelData->fillData(mName, *mCompoToModel, eString); // transmit
    if (ierr < 0) { qCritical() << " Error filling DBLE scalar data of SubModel from Component data " << (mName); return -1; }


    // now build list of SubModel parameters (int, bool, scalar, double, QString) and data (time series, secundary parameters...)

    mCompoModel->declareModelParameters();

    mCompoModel->declareModelInterface();

    // compute initial data for m input parameters to properly fill in Hist* vectors used by submodels to get their initial states.
    mCompoModel->computeInitialData() ;
    mCompoModel->setTypicalPeriods(mMilpData->useTypicalPeriods(), mMilpData->TypicalPeriods(), mMilpData->NDtTypicalPeriods(), mMilpData->VectTypicalPeriods()) ;

    ierr = checkPorts() ;
    if (ierr <0 )
    {
       Cairn_Exception error (" ERROR in component "+Name()+" for model "+mCompoModelName ,-1) ;
       this->setException(error) ;
       return ierr;
    }

    //---------------------------------------------------------------------------------------------------------------------
    // read dynamic input parameters at Component level    
    int ierr00 = mInputParam->readParameters(mComponent);
    if (ierr00 < 0) { qCritical() << " Error reading Parameters of SubModel " << (mName); return -1; }

    //---------------------------------------------------------------------------------------------------------------------

    //read non-configuration parameters
    ierr = mModelParam->readParameters(mComponent);
    if (ierr < 0) { qCritical() << " Error reading Parameters of SubModel " << (mName); return -1; }

    ierr = mModelEnvImpactParam->readParameters(mComponent);
    if (ierr < 0) { qCritical() << " Error reading EnvImpact of SubModel " << (mName); return -1; }

    ierr = mModelPortImpactParam->readParameters(mComponent);
    if (ierr < 0) { qCritical() << " Error reading PortImpact of SubModel " << (mName); return -1; }

    //Indicators
    mCompoModel->declareModelIndicators();

    if (mCompoModel != nullptr)
    {
        for (auto& ivar1D : mCompoModel->getIOExpressions(EIOModelType::eMIPExpression1D))
        {        
          QString varName = ivar1D->getName() ;
          qDebug() << " - AUTO_PlugSubmodelIO MilpComponent and SubModel vector : " <<  mName+"."+varName << npdtTot() << TimeSteps().size();
          mPlugSubmodelIO->publishData(varName, npdtTot(), NAN) ;
        }
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
    mModelParam = mCompoModel->getInputParam() ;
    mModelPerfParam = mCompoModel->getInputPerfParam();
    mModelData = mCompoModel->getInputData() ;
    mModelDataTS = mCompoModel->getInputDataTS();
    
    mModelEnvImpactParam = mCompoModel->getInputEnvImpactsParam();
    mModelPortImpactParam = mCompoModel->getInputPortImpactsParam();
    mModelPortImpactParamTS = mCompoModel->getInputPortImpactsParamTS();

    int ierr = 0;

    // if DataFile specified: direct reading of SubModel array parameters (perf maps : vector, double) from file
    QString dataFileValue = mCompoInputParam->getParamQSValue("DataFile");
    if (dataFileValue.simplified().replace(" ", "") != "")
    {
        QList<QString> perfParamNames;
        mModelPerfParam->getParameters(perfParamNames, EParamType::eVectorDouble);
        QStringList dataFiles = {};
        if (mCompoInputParam->getParamQSValue("DataFile").contains(";")) {
            dataFiles = mCompoInputParam->getParamQSValue("DataFile").split(";");
        }
        else {
            dataFiles = mCompoInputParam->getParamQSValue("DataFile").split(",");
        }
        //Read
        for (int i = 0; i < dataFiles.size(); i++) {
            QString dataFile_i = dataFiles[i].replace("./", "").replace(".\\", "").trimmed();
            QFile dataFileName(dataFile_i);
            if (!dataFileName.exists()) {
                OptimProblem* optimProblem = dynamic_cast<OptimProblem*> (this->parent());
                if (optimProblem) {
                    QString projectDir = QString(optimProblem->getStudyPathManager()->projectDir().c_str());
                    dataFile_i = projectDir + "/" + dataFile_i;
                }
            }
            mModelPerfParam->readVectorParameters(mName, dataFile_i, perfParamNames);
        }
        //Verification
        bool NotFound = false;
        const InputParam::t_mapParams vParams = mModelPerfParam->getMapParams();
        for (int i = 0; i < perfParamNames.size(); i++) {
            InputParam::t_mapParams::const_iterator vIter = vParams.find(perfParamNames[i]);
            if (vIter != vParams.end()) {
                if (vIter->second->IsBlocking()) {
                    qCritical() << "ERROR readVectorParameters: No data found in DataFile " << (mCompoInputParam->getParamQSValue("DataFile")) << " for variable name " << (mName + "." + perfParamNames[i]);
                    NotFound = true;
                }
                else if (GS::iVerbose > 0) {
                    qWarning() << "Initialization not performed in readVectorParameters: No data found in " << (mCompoInputParam->getParamQSValue("DataFile")) << " for expected variable name " << (mName + "." + perfParamNames[i]);
                }
            }            
        }
        //
        if (NotFound) {
            return -1;
        }
    }

    //---------------------------------------------------------------------------------------------------------------------

    // set dynamic secondary parameters at Component level
    ierr = setParameters() ;
    if (ierr <0) return ierr ;

    //---------------------------------------------------------------------------------------------------------------------

    // indirect offset reading of SubModel time series (vector, double) (length = futursize) from PEGASE subscribed vectors (length = pastsize+futursize)
    // fill SubModel vector Double timeseries from VectorXf values
    ierr = mModelData->fillVectorData(mName, *mCompoToModel, npdtPast()) ;
    if (ierr <0 ) { qCritical() << " Error filling Vector data of SubModel from Component Time Series "<< (mName)  ; return -1 ; }
   
    //---------------------------------------------------------------------------------------------------------------------

    // indirect reading of SubModel data (scalar, double) from Cairn ()
    // fill SubModel scalar double from Component scalar double values
    ierr = mModelData->fillData(mName, *mCompoToModel, eDouble) ; // transmit
    if (ierr <0 ) { qCritical() << " Error filling DBLE scalar data of SubModel from Component data "<< (mName)  ; return -1 ; }

    ierr = mModelData->fillData(mName, *mCompoToModel, eInt) ; // transmit
    if (ierr <0 ) { qCritical() << " Error filling INT scalar data of SubModel from Component data "<< (mName)  ; return -1 ; }

    ierr = mModelData->fillData(mName, *mCompoToModel, eString) ; // transmit
    if (ierr <0 ) { qCritical() << " Error filling QS scalar data of SubModel from Component data "<< (mName)  ; return -1 ; }

    //---------------------------------------------------------------------------------------------------------------------

    // possible finalization step
    mCompoModel->finalizeModelData() ;

    //Check Consistency
    ierr = mCompoModel->checkConsistency() ;
    if (ierr <0 ) { qCritical() << " Error Model Data are not consistent "<< (mName)  ; return -1 ; }

    //---------------------------------------------------------------------------------------------------------------------

    return 0 ;
}

int MilpComponent::setParameters()
{
    MilpComponent::exportRHVariableInModel();
    return MilpComponent::createHistFXLists();
}

int MilpComponent::initProblem()
{
    // Send component ports data to Submodel and allocate Ports own variables
    int ierr = initPorts();
    if (ierr < 0) return ierr;

    ierr = initSubModelConfiguration();
    if (ierr < 0) return ierr;

    return ierr;
}

int MilpComponent::initPorts()
{
    int ierr = 0;
    // define default variable names at ports
    ierr = defineDefaultVarNames();
    if (ierr < 0) {
        qCritical() << "ERROR in defining the port VarNames of component " + mName;
        return ierr;
    }

    //Set the main carrier of the component 
    defineMainEnergyVector();

    // Use port data to fill in Submodel data
    int iIn = 0 ;
    int iOut = 0;
    int iHeatCarrierIn = 0 ;
    int iHeatCarrierOut = 0;
    int iData = 0;
    int numport = 0;
    MilpPort* port ;
    QListIterator<MilpPort*> iport (PortList());
    while (iport.hasNext())
    {
       port = iport.next() ;

       ierr = port->initProblem(npdt()) ;
       if (ierr <0) return ierr ;      
       
        QString varUse = port->Direction() ;
        if (varUse != KCONS() && varUse != KPROD() && varUse != KDATA()) 
        {
            qCritical() << "Error : wrong Variable type  at " << (mName+"."+port->Name()) << " found : " << varUse ;
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
    MilpPort* port ;
    QListIterator<MilpPort*> iport(PortList());
    while (iport.hasNext())
    {
       port = iport.next() ;
       ierr = mCompoModel->checkUnit(port) ;
       if (ierr <0)
       {
           qCritical() << ("Error checkUnit at port "+Name()+"."+port->Name()) ;
           return ierr;
       }
       if (ierr >0)
       {
           qWarning() << ("Warning checkUnit at port "+Name()+"."+port->Name()) ;
           ierr = 0 ;
       }
    }
    return ierr ;
}

void MilpComponent::deleteCompoModel()
{
    if (mModelFactory) {
        mModelFactory->deleteModel(mCompoModelClassName, mName);
    }
    mCompoModel = nullptr;
}

void MilpComponent::createCompoModel()
{
    if (mCompoModel == nullptr) {
        if (!newCompoModel()) {
            if (mModelFactory) {
                if (mModelFactory->getModelList().contains(mCompoModelClassName)) {
                    try {
                        mCompoModel = dynamic_cast <SubModel*> (mModelFactory->createModel(this, mCompoModelClassName, mName));
                    }
                    catch (...) {
                        Cairn_Exception error("ERROR while loading model " + mCompoModelClassName, -1);
                        throw& error;
                    }
                    qInfo() << "model " + mCompoModelClassName + " has been successfully loaded!";
                }
            }
            
            if (mCompoModel==nullptr) {
                Cairn_Exception error("Error : unknown model name " + mCompoModelClassName + " on component " + Name(), -1);
                throw& error;
            }
        }
        mCompoModel->initDefaultPorts();
        createPorts();
        mCompoModel->setPortPointers();
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
        //Why declareModelInterface is called twice?! It is already called in initSubModelConfiguration
        //mCompoModel->declareModelInterface(); /**  make IO expression available to Component */
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

void MilpComponent::resetFlags() {
    mCompoModel->resetFlags();
}

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
    MilpPort* port ;
    QListIterator<MilpPort*> iport(PortList());
    while (iport.hasNext())
    {
       port = iport.next() ;

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
    QString varName = port->Variable() ;
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
        //qCritical() << " ERROR at port "<< (Name()+"."+port->Name()+" MilpExpression "+varName+" does not exist in component model "+mCompoModelName) ;
        Cairn_Exception error (" ERROR at port "+Name()+"."+port->Name()+" MilpExpression "+ varName +" does not exist in component model "+mCompoModelName ,-1) ;
        this->setException(error) ;
        return ;
//        throw &error ;
    }

}
void MilpComponent::setBusSameValuePortExpression()
{
    MilpPort* port ;
    QListIterator<MilpPort*> iport(PortList());
    while (iport.hasNext())
    {
       port = iport.next() ;
       QString varName = port->Variable() ;

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

bool MilpComponent::findFirstCoeff(QString aVarName, QMap<QString, ZEVariables*> aList , float &coeff, float &offset)
{
    QMapIterator<QString, ZEVariables*> ivar(aList) ;
    ZEVariables* lptrvar ;
    while (ivar.hasNext())
    {
       ivar.next() ;
       lptrvar=ivar.value() ;
       if (ivar.key() == aVarName && lptrvar!=nullptr)
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

    QList<InputParam::ModelParam*> vList;
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
    QList<InputParam::ModelParam*> vList;
    mPlugSubmodelIO->getParameters(vList, EParamType::eVectorEigen);
    for (auto& vParam : vList) {
        QString varName = vParam->getName();
        t_mapExchange::iterator vIter = a_Export.find(mName + "." + varName);
        if (vIter != a_Export.end()) {
            if (vIter.value()->set_Values(vParam, pdtHeure(), TimeSteps(), npdtPast())) {
                if (mFirstInitTS == 0) {
                    if (vIter.value()->update_PastValues(npdtPast(), timeshift()))
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
    MilpPort* port;
    QListIterator<MilpPort*> iport(PortList());
    while (iport.hasNext())
    {
        port = iport.next();
        if (port->VarType() == "vector") {
            QString varName = port->Variable();
            t_mapExchange::iterator vIter = a_Export.find(mName + "." + port->Name() + "." + varName);
            if (vIter != a_Export.end()) {
                if (vIter.value()->set_Values(mPlugSubmodelIO->getParameter(varName), pdtHeure(), TimeSteps(), npdtPast())) {
                    if (mFirstInitTS == 0) {
                        if (vIter.value()->update_PastValues(npdtPast(), timeshift()))
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
    QList<InputParam::ModelParam*> vList;
    mPlugSubmodelIO->getParameters(vList, EParamType::eVectorEigen);
    for (auto& vParam : vList) {
        VectorXf* lptr = std::get< Eigen::VectorXf*>(vParam->getPtr());            
        if (lptr->size() == 0)
        {
            qCritical () << "MilpComponen::setDefaultsResults " << vParam->getName() << " should have been allocated by component constructor ! "  ;
        }
        else
        {
            lptr->tail(npdt()).setConstant(0.);
        }
    }

    if (mCompoModel != nullptr)
    {
        mCompoModel->cleanExpression();
    }
}

void MilpComponent::computeHistNbHours()
{
    float histNbHours = 0;
    for (unsigned int t = 0; t < timeshift() ; ++t)
    {
        histNbHours += (TimeStep(t));
    }
    setHistNbHours(qCeil(histNbHours));
}

void MilpComponent::exportSubmodelIO(Solver* aSolver, int aNsol)
{
    /** Output Data (for link with PEGASE or OUTSIDE) */
    mFirstInit = 1;

    QString gamsVarName = "";
    ModelerInterface* pExternalModeler = nullptr;
    const double* vOptimalSolution = nullptr;

    if (aSolver->getModelType() == GS::MIPMODELER ()) {
        vOptimalSolution = aSolver->getOptimalSolution(aNsol);
    }
    else{//Case of GAMS
        pExternalModeler = aSolver->getExternalModeler();
        if (pExternalModeler == nullptr) {
            qCritical() << "External solver" << aSolver->getModelType() << "is not defined!";
            return;
        }
    }

    //automatically get every 1D variables declared in SubModel IO stack
    const double* externalOptValue = nullptr;
    double value = 0.;
    for (auto& ivar1D : mCompoModel->getIOExpressions(EIOModelType::eMIPExpression1D))
    {    
        MIPModeler::MIPExpression1D* ptrExp1D = (MIPModeler::MIPExpression1D*)(std::get<EIOModelType::eMIPExpression1D>(ivar1D->getPtr()));        
        InputParam::ModelParam *pParam = mPlugSubmodelIO->getParameter(ivar1D->getName());
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
                        gamsVarName = mName + "_v_" + ivar1D->getName();
                        externalOptValue = aSolver->getOptimalSolution(aNsol, gamsVarName.toStdString());
                        for (unsigned int t = 0; t < npdt(); ++t) {
                            if (externalOptValue != nullptr) {
                                value = externalOptValue[t];
                            }
                            else {
                                qDebug() << aSolver->getModelType() << "::Variable key: " << gamsVarName << " not defined in " << aSolver->getModelType() << " model";
                            }
                            (*ptrSubmodelIO)[t + npdtPast()] = value;
                        }
                        delete externalOptValue;                        
                    }
                } 
                else {
                    qWarning() << " - Solution1D for " << mName << "." << ivar1D->getName() << " of model " << mCompoModelName << " cannot be saved : missing corresponding VectorXf in MilpComponent ! ";
                }
            }
            else {
                qWarning() << " - Vector Expression1D " << mName << "." << ivar1D->getName() << " of model " << mCompoModelName << " has not been allocated in submodel ! ";
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

void MilpComponent::cleanExpressions()
{
    if (mCompoModel != nullptr)
    {
        mCompoModel->cleanExpression();
    }
}
QList<MilpPort*> MilpComponent::listSidePorts(const QString& aside)
{
    QList<MilpPort*> ptrlist;
    foreach(MilpPort * lptrport, PortList())
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

void MilpComponent::jsonSaveGuiComponent(QJsonArray &componentsArray, const QString& componentCarrier)
{
    QJsonArray paramArray;
    QJsonArray optionArray;

    mCompoModel->getInputParam()->jsonSaveGUIInputParam(paramArray);
    mCompoInputParam->jsonSaveGUIInputParam(optionArray);

    QJsonArray timeSeriesArray;
    QJsonArray envImpactArray;
    QJsonArray portImpactArray;
    if (!isBus()) {
        jsonSaveGUITimeSeries(timeSeriesArray, mCompoModel->getInputDataTS());
        mCompoModel->getInputEnvImpactsParam()->jsonSaveGUIInputParam(envImpactArray);
        mCompoModel->getInputPortImpactsParam()->jsonSaveGUIInputParam(portImpactArray);
        jsonSaveGUITimeSeries(portImpactArray, mCompoModel->getInputPortImpactsParamTS());
    }

    QJsonObject nodePorts;
    QJsonArray nodePortsArray;

    int portCount = listSidePorts(Left()).size();
    if (portCount) {
        nodePorts.insert(Left(), QJsonValue::fromVariant(portCount));
        jsonSaveGUINodePortsData(nodePortsArray, Left());
    }
    portCount = listSidePorts(Right()).size();
    if (portCount) {
        nodePorts.insert(Right(), QJsonValue::fromVariant(portCount));
        jsonSaveGUINodePortsData(nodePortsArray, Right());
    }
    portCount = listSidePorts(Bottom()).size();
    if (portCount) {
        nodePorts.insert(Bottom(), QJsonValue::fromVariant(portCount));
        jsonSaveGUINodePortsData(nodePortsArray, Bottom());
    }
    portCount = listSidePorts(Top()).size();
    if (portCount) {
        nodePorts.insert(Top(), QJsonValue::fromVariant(portCount));
        jsonSaveGUINodePortsData(nodePortsArray, Top());
    }

    QJsonObject compoObject;
    mGUIData->jsonSaveGUILine(compoObject, componentCarrier);
    compoObject.insert("nodePorts", nodePorts);
    compoObject.insert("nodePortsData", nodePortsArray);
    compoObject.insert("paramListJson", paramArray);
    compoObject.insert("optionListJson", optionArray);
    if (!isBus()) {
        compoObject.insert("timeSeriesListJson", timeSeriesArray);
        compoObject.insert("envImpactsListJson", envImpactArray);
        compoObject.insert("portImpactsListJson", portImpactArray);
    }

    componentsArray.push_back(compoObject) ;
}

void MilpComponent::jsonSaveGUITimeSeries(QJsonArray& timeSeriesArray, const InputParam* const inputParam)
{
    QJsonObject paramObject;
    if (inputParam != nullptr) {
        const InputParam::t_mapParams& vMapParams = inputParam->getMapParams();
        for (auto& [key, param] : vMapParams) {
            paramObject.insert("key", QJsonValue::fromVariant(key));
            paramObject.insert("value", QJsonValue::fromVariant(getTimeSeriesName(key)));
            timeSeriesArray.push_back(paramObject);
        }
    }

}

void MilpComponent::jsonSaveGUIlistPortsData(QJsonArray &nodePortArray, const QString& aSide)
{
    MilpPort* port ;
    QListIterator<MilpPort*> iport (PortList());
    while (iport.hasNext())
    {
        port = iport.next();
        if (port->Position() == aSide) {
            port->jsonSaveGUIPortsData(nodePortArray);
        }
    }
}

void MilpComponent::jsonSaveGUINodePortsData(QJsonArray &nodePortsArray, const QString & aSide)
{
    QJsonObject nodePortObject ;
    QJsonArray nodePortArray ;

    jsonSaveGUIlistPortsData(nodePortArray, aSide);
    nodePortObject.insert("ports", nodePortArray) ;
    nodePortObject.insert("pos", aSide) ;

    nodePortsArray.push_back(nodePortObject);
}

std::map<QString, InputParam::ModelParam*> MilpComponent::getParameters()
{
    std::map<QString, InputParam::ModelParam*> paramMap;

    paramMap.insert(mCompoModel->getInputParam()->getMapParams().begin(), mCompoModel->getInputParam()->getMapParams().end());
    paramMap.insert(getCompoInputParam()->getMapParams().begin(), getCompoInputParam()->getMapParams().end());
    paramMap.insert(mCompoModel->getInputDataTS()->getMapParams().begin(), mCompoModel->getInputDataTS()->getMapParams().end());
    
    paramMap.insert(mCompoModel->getInputEnvImpactsParam()->getMapParams().begin(), mCompoModel->getInputEnvImpactsParam()->getMapParams().end());
    paramMap.insert(mCompoModel->getInputPortImpactsParam()->getMapParams().begin(), mCompoModel->getInputPortImpactsParam()->getMapParams().end());
    paramMap.insert(mCompoModel->getInputPortImpactsParamTS()->getMapParams().begin(), mCompoModel->getInputPortImpactsParamTS()->getMapParams().end());

    return paramMap;
}

void MilpComponent::updateCompoParamMap(const std::string& a_SettingName, const t_value& a_SettingValue) {
    mComponent[QString::fromStdString(a_SettingName)] = QString(CairnAPIUtils::getParamValue(a_SettingValue).c_str());
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