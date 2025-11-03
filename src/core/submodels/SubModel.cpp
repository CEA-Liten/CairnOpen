#include "SubModel.h"
#include "MilpComponent.h"
#include "MilpPort.h"
#include "CairnUtils.h"
using namespace CairnUtils;

SubModel::SubModel(CairnObject* aParent) :   
    CairnObject(aParent),
    mException(Cairn_Exception()),
    mModel(nullptr),
    mParentCompo(nullptr),
    mMainCarrier(nullptr),
    mAllocate(true),
    mComputeSizeMax(false),
    mAddStateVariable(false),
    mAddStartUpShutDownVariable(false),
    mWeight(1.), //needed for models that doesn't have parameter "Weight" such as Ramp
    mUseWeightOptimization(false),
    mLPWeightOptimization(false),
    mLPModelOnly(false),
    mMaxValue(MIP_INFINITY),
    mMinValue(-MIP_INFINITY),
    mVariablePortNumber(false),
    mNbInputPorts(1),
    mNbOutputPorts(1),
    mNbInputFlux(1),
    mNbOutputFlux(1),
    mActivateConstraintsBetweenTP(false),
    mCondenseVariablesOnTP(false),
    mCondenseBinariesOnly(false),
    mTypicalPeriods(false),
    mAbsInitialState(0),
    mHistVariableCostsDiscounted(0.),
    mCurrency("EUR"),
    m_OptimalSizeUnit("OptimalSizeUnit"),
    p_OptimalSizeUnit(nullptr),
    mSubObjectiveExpression("N/A"),
    mPenaltyConstraintExpression("N/A"),
    mOpexExpression("N/A"),
    mOptimalSizeExpression(""),
    mHorizon(0),
    mInputParam(nullptr),
    mInputPerfParam(nullptr),
    mInputData(nullptr),
    mInputDataTS(nullptr),
    mInputIndicators(nullptr),
    mInputEnvImpacts(nullptr),
    mInputPortImpacts(nullptr),
    mTSInputPortImpacts(nullptr),
    mLabelMap({})
{
    std:string name = "_SubModel"; //case of AgeingRunningHours
    if (aParent) name = aParent->objectName();
    declareInputParams(name);
}


SubModel::~SubModel()
{
    if (mInputParam) delete(mInputParam);
    if (mInputPerfParam) delete(mInputPerfParam);
    if (mInputData) delete(mInputData);
    if (mInputDataTS) delete(mInputDataTS);
    
    if (mInputIndicators) delete(mInputIndicators);
    if (mInputEnvImpacts) delete (mInputEnvImpacts);
    if (mInputPortImpacts) delete (mInputPortImpacts);
    if (mTSInputPortImpacts) delete (mTSInputPortImpacts);
    
    removeIOs();

    mListPort.clear();

    deleteEnvImpacts();
}

void SubModel::finalizeModelData()
{
    // nothing here - Can be overridden in individual SubModels
}

void SubModel::deleteEnvImpacts()
{
    for (EnvImpact* impact : mEnvImpacts) {
        if (impact) {
            removeEnvImpactIOs(impact->Name()); /* remove related IOs from mIOExpressions */
            /* remove related Exps from mExpressions0D and mExpressions1D? (currently there is no any!) */
            delete(impact);
        }
    }
    mEnvImpacts.clear();
}

void SubModel::declareInputParams(const std::string& name)
{
    if (mInputParam) delete(mInputParam);
    if (mInputPerfParam) delete(mInputPerfParam);
    if (mInputData) delete(mInputData);
    if (mInputDataTS) delete(mInputDataTS);
    if (mInputEnvImpacts) delete (mInputEnvImpacts);
    if (mInputPortImpacts) delete (mInputPortImpacts);
    if (mTSInputPortImpacts) delete (mTSInputPortImpacts);

    // Param
    mInputParam = new InputParam(this, "SubModelbaseInputParam" + name);
    // Carto
    mInputPerfParam = new InputParam(this, "SubModelbaseInputPerfParam" + name);
    // Config
    mInputData = new InputParam(this, "SubModelbaseInputData" + name);
    // TimeSeries
    mInputDataTS = new InputParam(this, "SubModelbaseInputDataTS" + name);

    // Impacts
    deleteEnvImpacts(); 
    mInputEnvImpacts = new InputParam(this, "SubModelbaseInputEnvImpactsParam" + name);
    mInputPortImpacts = new InputParam(this, "SubModelbaseInputPortImpactsParam" + name);
    mTSInputPortImpacts = new InputParam(this, "SubModelbaseTSInputPortImpactsParam" + name);

    // Indicateurs
    resetIndicators();
    mInputIndicators = new InputParam(this, "SubModelbaseInputIndicators" + name);
}

void SubModel::resetIndicators()
{
    if (mInputIndicators) {
        //reset contribution values
        const InputParam::t_Indicators& vIndicators = mInputIndicators->getIndicators();
        for (auto& vIndicator : vIndicators) {
            vIndicator->resetValue();
        }
        delete(mInputIndicators);
    }

    resetHistStoredVaues();
}


MilpPort* SubModel::getPort(const std::string& aPortId) 
{
    for(MilpPort * lptrport: mListPort)
    {
        if (lptrport->ID() == aPortId) {
            return lptrport;
        }
    }
    return nullptr;
}

MilpPort* SubModel::getPortByType(const std::string& aType, const std::string& aDirection)
{
    for(MilpPort * lptrport: mListPort)
    {
        if (CairnUtils::contains(lptrport->getCarrier()->Type(), aType) && (lptrport->Direction() == aDirection || aDirection == "ANY"))
        {
            return lptrport;
        }
    }
    return nullptr;
}

void SubModel::removePort(MilpPort* lptrport)
{
    if (lptrport != nullptr) {
        std::vector<MilpPort*>::iterator vIter = find(mListPort.begin(), mListPort.end(), lptrport);
        if (vIter != mListPort.end()) {
            mListPort.erase(vIter);       
            delete lptrport;
        }
        else {
            Cairn_Exception erreur((std::string)"Error deleting port", -1);
            this->setException(erreur);
        }
    }
}

void SubModel::removeBusPort(MilpPort* lptrport) 
{
    if (lptrport != nullptr) {
        std::vector<MilpPort*>::iterator vIter = find(mListPort.begin(), mListPort.end(), lptrport);
        if (vIter != mListPort.end()) {
            mListPort.erase(vIter);
        }
    }
}

void SubModel::addParameter(const std::string& aParamName, const t_pvalue &aPtr, t_value aDefaultValue, t_flag aIsBlocking, t_flag aIsUsed, 
    const std::string& aDescription, const t_unit& aUnit, const std::string& aShowConfig)
{
    mInputParam->addParameter(aParamName, aPtr, aDefaultValue, aIsBlocking, aIsUsed, aDescription, aUnit, aShowConfig);
}

void SubModel::addPerfParam(const std::string& aParamName, std::vector<double>* aPtr, t_flag aIsBlocking, t_flag aIsUsed, 
    const std::string& aDescription, const t_unit& aUnit)
{
    mInputPerfParam->addPerfParam(aParamName, aPtr, aIsBlocking, aIsUsed, aDescription, aUnit);
}

void SubModel::addTimeSeries(const std::string& aParamName, std::vector<double>* aDblePtr, t_flag IsBlocking, t_flag aIsUsed, 
    const std::string& aDescription, const t_unit& aUnit, const std::string& aShowConfig, double a_default, double a_min, double a_max)
{
    mInputDataTS->addTimeSeries(aParamName, aDblePtr, a_default, IsBlocking, aIsUsed, aDescription, aUnit, aShowConfig, a_min, a_max);
}

//Model IO Interface
void SubModel::addIO(const std::string& aIOName, MIPModeler::MIPExpression* aExprPtr, t_flag aIsUsed, const t_unit& aUnit)
{
    /* 0D Exp and dynamic unit */
    if (assertIONonExistence(aIOName, aExprPtr)) {
        removeIO(aIOName);
    }
    assertIsNotSizeMaxExp(aExprPtr);
    mIOExpressions[aIOName] = new ModelIO(aIOName, aExprPtr, aIsUsed, aUnit);
}

void SubModel::addIO(const std::string& aIOName, MIPModeler::MIPExpression1D* aExprPtr1D, t_flag aIsUsed, const t_unit& aUnit)
{
    /* 1D Exp and dynamic unit */
    if (assertIONonExistence(aIOName, aExprPtr1D)) {
        removeIO(aIOName);
    }
    mIOExpressions[aIOName] = new ModelIO(aIOName, aExprPtr1D, aIsUsed, aUnit);
}

void SubModel::addSizeMaxIO(const std::string& aIOName, MIPModeler::MIPExpression* aExprPtr, t_flag aIsUsed, const std::string aUnit)
{
    /* used only for mExpSizeMax(0D, scalar unit) */
    if (assertIONonExistence(aIOName, aExprPtr)) {
        removeIO(aIOName);
    }
    assertIsSizeMaxExp(aExprPtr);
    mComputeSizeMax = true;
    mOptimalSizeExpression = aIOName;
    m_OptimalSizeUnit = aUnit;
    mIOExpressions[aIOName] = new ModelIO(aIOName, aExprPtr, aIsUsed, aUnit);
}

void SubModel::addSizeMaxIO(const std::string& aIOName, MIPModeler::MIPExpression* aExprPtr, t_flag aIsUsed, const std::string* pUnit)
{
    /* used only for mExpSizeMax(0D, scalar unit) */
    if (assertIONonExistence(aIOName, aExprPtr)) {
        removeIO(aIOName);
    }
    assertIsSizeMaxExp(aExprPtr);
    mComputeSizeMax = true;
    mOptimalSizeExpression = aIOName;
    p_OptimalSizeUnit = pUnit;
    mIOExpressions[aIOName] = new ModelIO(aIOName, aExprPtr, aIsUsed, pUnit);
}

bool SubModel::assertIONonExistence(const std::string& name, const t_pExpr expression)
{
//#ifdef DEBUG 
    for (auto& [vName, vIO] : mIOExpressions) {
        /* throw an error if an IO with the same name but different expression already exist */
        if (vName == name && vIO && vIO->getPtr() != expression) {
            Cairn_Exception error("An IO Expression with the same name but different expression already exist: " + vName, -1);
            throw error;
        }
        /* throw an error if an IO with the same expression but different name already exist */
        else if (vName != name && vIO && vIO->getPtr() == expression) {
            Cairn_Exception error("An IO associated to the same expression but different name already exists: " + vName + " and " + name, -1);
            throw error;
        }
        else if (vName == name && vIO && vIO->getPtr() == expression) {
            /* An IO with the same name and same expression already exist. return true to delete the IO and create a new one
            with possibly difefrent comment, unit, or isUsed values */
            return true;
        }
    }
    return false;
//#endif
}

void SubModel::assertIsSizeMaxExp(MIPModeler::MIPExpression* aExprPtr) {
    /* throw an error if a given pointer doesn't point to the expression mExpSizeMax */
    if (aExprPtr != &mExpSizeMax) {
        Cairn_Exception error("Method addSizeMaxIO can only be used for mExpSizeMax. Please, use method addIO for other expressions!", -1);
        throw error;
    }
}

void SubModel::assertIsNotSizeMaxExp(MIPModeler::MIPExpression* aExprPtr) {
    /* throw an error if a given pointer points to the expression mExpSizeMax */
    if (aExprPtr == &mExpSizeMax) {
        Cairn_Exception error("Method addIO cannot be used for mExpSizeMax. Please, use method addSizeMaxIO!", -1);
        throw error;
    }
}


// Model Rolling Horizon variables
void SubModel::addControlIO(const std::string& aIOName, MIPModeler::MIPExpression1D* aExprPtr1D, t_flag aIsUsed, 
    const t_unit& aUnit, double* aValuePtr, double* aDefaultValue, bool a_isMPC)
{
    addIO(aIOName, aExprPtr1D, aIsUsed, aUnit);
    mListControlIO[aIOName] = new ControlVar(aIOName, aValuePtr, aDefaultValue, a_isMPC);
}

void SubModel::addControlIO(const std::string& aIOName, MIPModeler::MIPExpression1D* aExprPtr1D, t_flag aIsUsed, 
    const t_unit& aUnit, std::vector<double>* aHistPtr, double* aDefaultValue, bool a_isMPC)
{
    addIO(aIOName, aExprPtr1D, aIsUsed, aUnit);
    mListControlIO[aIOName] = new ControlVar(aIOName, aHistPtr, aDefaultValue, a_isMPC);
}

void SubModel::removeIO(const std::string& aName)
{
    if (mIOExpressions.find(aName) != mIOExpressions.end()) {
        delete mIOExpressions[aName];
        mIOExpressions.erase(aName);
    }

    if (mListControlIO.find(aName) != mListControlIO.end()) {
        delete mListControlIO[aName];
        mListControlIO.erase(aName);
    }
}

void SubModel::removeEnvImpactIOs(const std::string& aImpactName)
{
    std:vector<std::string> keysToRemove = {};
    //Delete related ModelIO
    for (auto& [vKey, vIO] : mIOExpressions) {
        if (vIO && CairnUtils::contains(vKey, aImpactName)) {
            keysToRemove.push_back(vKey);
            delete vIO;
        }
    }

    //Remove from the map 
    for (auto& vKey: keysToRemove) {
        mIOExpressions.erase(vKey);
    }

    keysToRemove.clear();
    //Delete related Control ModelIO
    for (auto& [vKey, vIO] : mListControlIO) {
        if (vIO && CairnUtils::contains(vKey, aImpactName)) 
        {
            keysToRemove.push_back(vKey);
            delete vIO;
        }
    }

    //Remove from the map 
    for (auto& vKey : keysToRemove) {
        mIOExpressions.erase(vKey);
    }
}

void SubModel::removeIOs()
{
    for (auto& [vKey, value] : mIOExpressions) {
        if (value) delete value;
    }
    mIOExpressions.clear();

    for (auto& [vKey, value] : mListControlIO) {
        if (value) delete value;
    }
    mListControlIO.clear();
}

MIPModeler::MIPExpression* SubModel::getMIPExpression(std::string aExpressionName)
{
    ModelIO* vIO = getIOExpression(aExpressionName);
    if (vIO) {
        if (vIO->getType() == EIOModelType::eMIPExpression) {
            return (MIPModeler::MIPExpression*)(std::get<EIOModelType::eMIPExpression>(vIO->getPtr()));
        }
    }
    return nullptr;
}

void SubModel::buildControlVariables()
{
    for (auto [key, var] : mListControlIO) {
        var->ComputeValue(mNpdtPast);
    }
}

MIPModeler::MIPExpression1D* SubModel::getMIPExpression1D(std::string aExpressionName)
{
    ModelIO* vIO = getIOExpression(aExpressionName);
    if (vIO) {
        if (vIO->getType() == EIOModelType::eMIPExpression1D) {
            return (MIPModeler::MIPExpression1D*)(std::get<EIOModelType::eMIPExpression1D>(vIO->getPtr()));
        }
    }
    return nullptr;
}

MIPModeler::MIPExpression& SubModel::getMIPExpression1D(uint i, std::string aExpressionName)
{
    ModelIO* vIO = getIOExpression(aExpressionName);
    if (vIO) {
        if (vIO->getType() == EIOModelType::eMIPExpression1D) {
            if (i < vIO->size()) {
                MIPModeler::MIPExpression1D* vExpr = (MIPModeler::MIPExpression1D*)(std::get<EIOModelType::eMIPExpression1D>(vIO->getPtr()));
                return (*vExpr)[i];
            }
        }
    }
    Cairn_Exception error("ERROR: MilpExpression " + aExpressionName + " does not exist in the component model or its size is less than " + std::to_string(i), -1);
    throw error;
}

void SubModel::dumpIOExpression1DList()
{
    // Loop on expected input parameters
    cInfo() << "\n\t Vector Expression ;" << " \t\t\t " << " Unit ;";
    for (auto& [key, vIO] : mIOExpressions) {
        if (vIO != nullptr) {
            if (vIO->getType() == EIOModelType::eMIPExpression1D) {
                cInfo() << key << " \t\t\t " << vIO->getUnit();
            }
        }
    }
}

void SubModel::dumpIOExpressionList()
{
    // Loop on expected input parameters
    cInfo() << "\n\t Scalar Expression " << " \t\t\t " << " Unit ";
    for (auto& [key, vIO] : mIOExpressions) {
        if (vIO != nullptr) {
            if (vIO->getType() == EIOModelType::eMIPExpression) {
                cInfo() << key << " \t\t\t " << vIO->getUnit();
            }
        }
    }
}

ModelIO* SubModel::getIOExpression(const std::string& aName)
{
    t_mapIOs::iterator vIter = mIOExpressions.find(aName);
    if (vIter != mIOExpressions.end()) {
        return vIter->second;
    }
    return nullptr;
}


std::vector<ModelIO*> SubModel::getIOExpressions(const EIOModelType& aIOType)
{
    std::vector<ModelIO*> vRet;
    for (auto& [key, vIO] : mIOExpressions) {
        if (vIO != nullptr) {
            if (vIO->getType() == aIOType) {
                vRet.push_back(vIO);
            }
        }
    }
    return vRet;
}

void SubModel::fillExpression(MIPModeler::MIPExpression1D& aExpress1D, MIPModeler::MIPVariable1D& aVariable) 
{
    int dimVar = aVariable.getDims();
    int dimExpr = aExpress1D.size();
    if (dimVar != dimExpr) {
        Cairn_Exception cairn_error(Name() + ": expression and variable sizes don't match");
        cCritical() << Name() + ": expression and variable sizes don't match";
        throw cairn_error;
    }
    for (uint64_t t = 0; t < mHorizon; ++t) {
        aExpress1D[t] += aVariable(t); 
    }
}
void SubModel::closeExpression(MIPModeler::MIPExpression& aExpress)
{
    aExpress.close() ;
}

void SubModel::closeExpression1D(MIPModeler::MIPExpression1D& aExpress1D)
{
    for (int i = 0; i < (int)aExpress1D.size();i++) {
        aExpress1D.at(i).close();
    }
}

void SubModel::allocateExpressions()
{
    /* Allocate IO expressions*/
    for (auto& [key, vIO] : mIOExpressions) {
        if (vIO && vIO->isPExpr()) {
            //0D
            if (vIO->getType() == EIOModelType::eMIPExpression) {
                MIPModeler::MIPExpression* ptrIOExp0D = (MIPModeler::MIPExpression*)(std::get<EIOModelType::eMIPExpression>(vIO->getPtr()));
                *ptrIOExp0D = MIPModeler::MIPExpression(); /* not really needed */
            }
            //1D
            else if (vIO->getType() == EIOModelType::eMIPExpression1D) {
                MIPModeler::MIPExpression1D* ptrIOExp1D = (MIPModeler::MIPExpression1D*)(std::get<EIOModelType::eMIPExpression1D>(vIO->getPtr()));
                *ptrIOExp1D = MIPModeler::MIPExpression1D(mHorizon); /* always size mHorizon for IO expressions */
            }
        }
    }

    /* Allocate 0D expressions*/
    for (auto& ptrExp0D : mExpressions0D) {
        if (ptrExp0D) {
            *ptrExp0D = MIPModeler::MIPExpression(); /* not really needed */
        }
    }

    /* Allocate 1D expressions*/
    for (auto& sExp1D : mExpressions1D) {
        if (sExp1D.pExp1D) {
            *(sExp1D.pExp1D) = MIPModeler::MIPExpression1D(*(sExp1D.pSize)); /* given size */
        }
    }

    /* Allocate particular expressions
     *
     * These are the expressions declared in SubModel. But, not used in all models.
     * Usually, they are used in TechnicalSubModel. But, not in OperationSubModel and/or BusSubModel.
     * When they are used in a certain non-Technical model, e.g. Ramp, they should be registered
     * using "add IO" or addExp inside that model.
     * This close here is for safety in case they are used in a model but, have not been registered.
     * 
     * Allocate size mHorizon for 1D expressions
    */
    mExpSizeMax = MIPModeler::MIPExpression();
    mExpState = MIPModeler::MIPExpression1D(mHorizon);
    mExpStartUp = MIPModeler::MIPExpression1D(mHorizon);
    mExpShutDown = MIPModeler::MIPExpression1D(mHorizon);
}

void SubModel::closeExpressions()
{
    /* Close IO expressions*/
    for (auto& [key, vIO] : mIOExpressions) {
        if (vIO) {
            vIO->close();
        }
    }

    /* Close 0D expressions*/
    for (auto& ptrExp0D : mExpressions0D) {
        if (ptrExp0D) {
            ptrExp0D->close();
        }
    }

    /* Close 1D expressions*/
    for (auto& sExp1D : mExpressions1D) {
        if (sExp1D.pExp1D) {
            closeExpression1D(*(sExp1D.pExp1D));
        }
    }

    /* Close particular expressions 
     *
     * These are the expressions declared in SubModel. But, not used in all models.
     * Usually, they are used in TechnicalSubModel. But, not in OperationSubModel and/or BusSubModel.
     * When they are used in a certain non-Technical model, e.g. Ramp, they should be registered 
     * using "add IO" or addExp inside that model. 
     * This close here is for safety in case they are used in a model but, have not been registered. 
    */
    closeExpression(mExpSizeMax);
    closeExpression1D(mExpState);
    closeExpression1D(mExpStartUp);
    closeExpression1D(mExpShutDown);
}

int SubModel::defineDefaultVarNames()
{
    int ierr = 0;
    // check BusVarName,    
    int inumberchange = 0;
    std::string varUseCheck = "none";
    for (auto &port : PortList()) {    
        std::string portType = port->PortType();
        if (portType == "BusSameValue") {
            ierr = checkBusSameValueVarName(port);
        }
        else if (portType == "BusFlowBalance") {
            ierr = checkBusFlowBalanceVarName(port, inumberchange, varUseCheck);
        }      
        if (ierr < 0) return -1;
    }
    return ierr;
}

int SubModel::checkBusSameValueVarName(MilpPort *port)
{
    if (port->Variable() == "")
    {
        cCritical() << " ERROR at port " << (port->Name())
            << " You should set Variable= property in <port> field to be able to impose same value through BusSameValue bus ";
        return -1;
    }
    if (port->Direction() == "")
    {
        cCritical() << " ERROR at port " << (port->Name())
            << " You should set Use= DATAEXCHANGE in <port> field to be able to impose same value through BusSameValue bus ";
        return -1;
    }
    return 0;
}

int SubModel::checkBusFlowBalanceVarName(MilpPort* port, int& inumberchange, std::string& varUseCheck)
{
    std::string varName = port->Variable();
    std::string varUse = port->Direction();
    if (varUseCheck != varUse) {
        varUseCheck = varUse;
        inumberchange++;
    }

    // Uncomplete description -> use default definitions functions of models
    if (varName == "") {
        if (!defineDefaultVarNames(port)) {
            cCritical() << " ERROR at port " << (port->Name())
                << " No default variable exist for that port type " << (port->getCarrier()->Type())
                << " - You should set Variable field to send from Converter to BusFlowBalance bus ";
            return -1;
        }        
    }
    if (varUse == "") {
        cCritical() << " ERROR at port " << (port->Name())
            << " No default use type Producer / Consumer exist for that port type " << (port->getCarrier()->Type())
            << " - You should set Variable & Use fields to exchanger from Converter to BusFlowBalance bus ";
        return -1;
    }
    if (inumberchange == 0) {
        cCritical() << " ERROR on component " << (Name())
            << " Found only one type of port : " << (varUseCheck)
            << " You should have at least one consumer and one producer ! ";
        return -1;
    }
    return 0;
}

bool SubModel::defineDefaultVarNames(MilpPort* port)
{
    // pas de définition par défaut
    return false;
}

int SubModel::checkUnit(MilpPort* port)
{
    int ierr = 0 ;
    if (port->getCarrier() == nullptr) return 0;
    std::string varName = port->Variable() ;
    std::string varUse = port->Direction() ;
    std::string varFluxUnit = port->FluxUnit();
    std::string varStorageUnit = port->StorageUnit();
    std::string varPotentialUnit = port->PotentialUnit();
    std::string ExprUnit = ExpUnit(varName) ;

    if (varUse == "") {
        cWarning() << ("Variable "+varName +" neither defined as INPUT nor OUTPUT for the component ! ") ;
        ierr = 1 ;
    }

    MIPModeler::MIPExpression* ptrExp = getMIPExpression(varName);
    MIPModeler::MIPExpression1D* ptrExp1D = getMIPExpression1D(varName);

    if (ptrExp != nullptr)
    {
        port->setVarType("scalar");   // Give access to Scalar value to Bus
    }
    else if (ptrExp1D != nullptr)
    {
        port->setVarType("vector");   // Give access to Scalar value to Bus
    }
    else
    {
        cCritical() << ("ERROR no Milp Expression "+varName +" exist in submodel ! ") ;
        cInfo() << "Available Milp Expressions are";
        dumpIOExpressionList();
        dumpIOExpression1DList();
        return -1 ;
    }

    if (port->PortType()=="BusSameValue")
    {
         if (!CairnUtils::contains(ExprUnit, varFluxUnit) && !CairnUtils::contains(ExprUnit, varStorageUnit) 
             && !CairnUtils::contains(ExprUnit, varPotentialUnit) && CairnUtils::toUpper(port->VarCheckUnit()) == "YES")
         {
             cCritical() << (" ERROR MilpExpression "+varName +" unit found is "+ExprUnit) ;
             cCritical() << (" Unit should be either "+varFluxUnit +" or "+varStorageUnit +" or "+varPotentialUnit) ;
             dumpIOExpressionList();
             dumpIOExpression1DList();
             return -1 ;
         }
    }
    if (port->PortType()=="BusFlowBalance")
    {
         if (!CairnUtils::contains(ExprUnit, varFluxUnit) && !CairnUtils::contains(ExprUnit, varStorageUnit) 
             && !CairnUtils::contains(ExprUnit, varPotentialUnit)  
             && CairnUtils::toUpper(port->VarCheckUnit()) == "YES")
         {
             cCritical() << (" ERROR MilpExpression "+varName +" flux unit (from SubModel) is "+ExprUnit) ;
             cCritical() << (" But unit expected by energy vector is "+varFluxUnit) ;
             dumpIOExpressionList();
             dumpIOExpression1DList();
             return -1 ;
         }
    }
    return ierr ;
}

void SubModel::computeAllIndicators(const double* optSol) {
    //computeDefaultIndicators(optSol);
}

void SubModel::computeIndicator(const MIPModeler::MIPExpression1D& exp, const double* optSol, double& aUnDiscounted, double& aDiscounted, double& aHistUnDiscounted, double& aHistDiscounted, bool isEnvImpact)
{
    //PLAN values are initialized to 0 as they are calculated at each cycle whereas HIST values are cumulated 
    aUnDiscounted = 0.0;
    aDiscounted = 0.0;

    double ExtrapolationFactor = mParentCompo->ExtrapolationFactor();

    int year = 0;
    double value_t = 0.0;
    for (unsigned int t = 0; t < mTimeSteps.size(); ++t)
    {
        uint t_hour = std::ceil(t * TimeStep(t)) + mParentCompo->HistNbHours();
        while (t_hour > mParentCompo->TableYearsHours().at(year) && year < mParentCompo->TableYearsHours().size() - 1) {
            year += 1;
        }
        //Note that, LevelizationFactor is already multiplied by ExtrapolationFactor
        double LevelizationFactor = 1.0;
        if(isEnvImpact)
            LevelizationFactor = mParentCompo->ImpactLevelizationTable().at(year);
        else 
            LevelizationFactor = mParentCompo->LevelizationTable().at(year);
        value_t = exp[t].evaluate(optSol);
        aUnDiscounted += value_t * ExtrapolationFactor;
        aDiscounted += value_t * LevelizationFactor; 
        if (t < *mptrTimeshift) {  
            aHistUnDiscounted += value_t;
            aHistDiscounted += (value_t * LevelizationFactor) / ExtrapolationFactor;  
        }
    }
}


void SubModel::computeTime(bool bsetValue, uint aNpdt, uint aShift, MIPModeler::MIPExpression1D exp, const double* optSol, double& ret) {
    float factor = 1.;
    if (bsetValue) {
        ret = 0.; // réinitialisation de ret à 0 pour le fichier plan
        factor = mParentCompo->ExtrapolationFactor(); // pour le fichier plan, mutliplication par le facteur d'actualisation
    }
    for (uint64_t t = 0; t < aNpdt; ++t)
    {
        if (fabs(exp.at(t).evaluate(optSol)) > 1.e-6f) ret += TimeStep(t) * factor; 
    }
}

void SubModel::computeTime(bool bsetValue, uint aNpdt, uint aShift, MIPModeler::MIPExpression1D exp, const double* optSol, double& retCharged, double& retDischarged) {
    float factor = 1.;
    if (bsetValue) {
        retCharged = retDischarged = 0.; // réinitialisation de ret à 0 pour le fichier plan
        factor = mParentCompo->ExtrapolationFactor();
    }
    for (uint64_t t = 0; t < aNpdt; ++t)
    {
        if (exp.at(t).evaluate(optSol) > 1.e-6f) retDischarged += TimeStep(t) * factor;
        else if (exp.at(t).evaluate(optSol) < -1.e-6f) retCharged += TimeStep(t) * factor;
    }
}

void SubModel::computeProduction(bool bsetValue, uint aNpdt, uint aShift, MIPModeler::MIPExpression1D exp, const double* optSol, const double& aCoeff, const double& bCoeff, double& aProduction, const bool& aTimeIntegration)
{
    float factor = 1;
    if (bsetValue) {
        aProduction = 0.;
        factor = mParentCompo->ExtrapolationFactor();
    }
    //compute indicators on the planification part
    for (unsigned int t = 0; t < aNpdt; ++t)
    {
        if (fabs(exp.at(t).evaluate(optSol)) > 1.e-6)
        {
            if (aTimeIntegration) aProduction += (aCoeff * exp.at(t).evaluate(optSol) + bCoeff) * TimeStep(t) * factor;// *mParentCompo->ExtrapolationFactor();
            else aProduction += (aCoeff * exp.at(t).evaluate(optSol) + bCoeff);
        }
    }
}

void SubModel::computeProduction(bool bsetValue, uint aNpdt, uint aShift, MIPModeler::MIPExpression1D exp, const double* optSol, const double& aCoeff, const double& bCoeff, double& retCharged, double& retDischarged) {
    float factor = 1.;
    if (bsetValue) {
        retCharged = retDischarged = 0.;
        factor = mParentCompo->ExtrapolationFactor();
    }
    for (uint64_t t = 0; t < aNpdt; ++t)
    {
        if (exp.at(t).evaluate(optSol) > 1.e-6f) retDischarged += (aCoeff * exp.at(t).evaluate(optSol) + bCoeff) * TimeStep(t) * factor;// *mParentCompo->ExtrapolationFactor();
        else if (exp.at(t).evaluate(optSol) < -1.e-6f) retCharged += (aCoeff * exp.at(t).evaluate(optSol) + bCoeff) * TimeStep(t) * factor;// *mParentCompo->ExtrapolationFactor();
    }
}

void SubModel::computeConsumption(bool bsetValue, uint aNpdt, uint aShift, MIPModeler::MIPExpression1D exp, const double* optSol, const double& aCoeff, const double& bCoeff, double& aConsumption)
{
    float factor = 1.;
    if (bsetValue) { // fichier plan : on extrapole sans ajouter; sinon on ajoute sans extrapoler
        aConsumption = 0.;
        factor = mParentCompo->ExtrapolationFactor();
    }
    //compute indicators on the planification part
    for (unsigned int t = 0; t < aNpdt; ++t)
    {
        if (fabs(exp.at(t).evaluate(optSol)) > 1.e-6)
        {
            aConsumption -= (aCoeff * (exp.at(t).evaluate(optSol)) + bCoeff) * TimeStep(t) * factor;// *mParentCompo->ExtrapolationFactor();
        }
    }
}

void SubModel::computeLvlConsumption(bool bsetValue, uint aNpdt, uint aShift, MIPModeler::MIPExpression1D exp, const double* optSol, const double& aCoeff, const double& bCoeff, double& aConsumption)
{
    int year = 0;
    float factor = 1. / mParentCompo->ExtrapolationFactor();// si c'est le fichier, HIST, on doit enlever l'extrapolation de la LevelizationTable
    if (bsetValue) {
        aConsumption = 0.;
        factor = 1.;
    }
    for (unsigned int t = 0; t < aNpdt; ++t)
    {
        if (fabs((exp.at(t).evaluate(optSol))) > 1.e-6)
        {
            uint t_hour = std::ceil(t * TimeStep(t)) + mParentCompo->HistNbHours();
            while ((t_hour) > mParentCompo->TableYearsHours()[year] && year < (mParentCompo->TableYearsHours()).size() - 1) {
                year += 1;
                //cInfo() << "year:" << year << "hours" << t_hour << mParentCompo->TableYearsHours() << "mhistnbhour" << mParentCompo->HistNbHours() << "table" << mParentCompo->LevelizationTable();
            }
            aConsumption -= (aCoeff * (exp.at(t).evaluate(optSol)) + bCoeff) * TimeStep(t) * mParentCompo->LevelizationTable().at(year) * factor;
        }
    }
}


void SubModel::computeLvlProduction(bool bsetValue, uint aNpdt, uint aShift, MIPModeler::MIPExpression1D exp, const double* optSol, const double& aCoeff, const double& bCoeff, double& aProduction)
{
    int year = 0;
    float factor = 1./ mParentCompo->ExtrapolationFactor();// si c'est le fichier, HIST, on doit enlever l'extrapolation de la LevelizationTable
    if (bsetValue) {
        aProduction = 0.;
        factor = 1.; // si c'est le plan, on laisse tel quel
    }
    for (unsigned int t = 0; t < aNpdt; ++t)
    {
        if (fabs((exp.at(t).evaluate(optSol))) > 1.e-6)
        {
            uint t_hour = std::ceil(t * TimeStep(t)) + mParentCompo->HistNbHours();
            while ((t_hour) > mParentCompo->TableYearsHours()[year] && year < (mParentCompo->TableYearsHours()).size() - 1) {
                year += 1;
                //cInfo() << "year:" << year << "hours" << t_hour << mParentCompo->TableYearsHours() << "mhistnbhour" << mParentCompo->HistNbHours() << "table" << mParentCompo->LevelizationTable();
            }
            aProduction += (aCoeff * (exp.at(t).evaluate(optSol)) + bCoeff) * TimeStep(t) * mParentCompo->LevelizationTable().at(year) * factor;
        }
    }
}

void SubModel::computeLvlProduction(bool bsetValue, uint aNpdt, uint aShift, MIPModeler::MIPExpression1D exp, const double* optSol, const double& aCoeff, const double& bCoeff, double& retCharged, double& retDischarged) {
    int year = 0;
    float factor = 1. / mParentCompo->ExtrapolationFactor();// si c'est le fichier, HIST, on doit enlever l'extrapolation de la LevelizationTable
    if (bsetValue) {
        retCharged = retDischarged = 0.;
        factor = 1.;
    }
    for (uint64_t t = 0; t < aNpdt; ++t)
    {
        uint t_hour = std::ceil(t * TimeStep(t)) + mParentCompo->HistNbHours();
        while ((t_hour) > mParentCompo->TableYearsHours()[year] && year < (mParentCompo->TableYearsHours()).size() - 1) {
            year += 1;
            //cInfo() << "year:" << year << "hours" << t_hour << mParentCompo->TableYearsHours() << "mhistnbhour" << mParentCompo->HistNbHours() << "table" << mParentCompo->LevelizationTable();
        }
        if (exp.at(t).evaluate(optSol) > 1.e-6) retDischarged += (aCoeff * (exp.at(t).evaluate(optSol)) + bCoeff) * TimeStep(t) * mParentCompo->LevelizationTable().at(year) * factor;
        else if (exp.at(t).evaluate(optSol) < -1.e-6f) retCharged += (aCoeff * exp.at(t).evaluate(optSol) + bCoeff) * TimeStep(t) * mParentCompo->LevelizationTable().at(year) * factor;
    }
}

void SubModel::computeLvlImpact(bool bsetValue, uint aNpdt, uint aShift, MIPModeler::MIPExpression1D exp, const double* optSol, const double& aCoeff, const double& bCoeff, double& aProduction)
{
    int year = 0;
    if (bsetValue) aProduction = 0.;
    for (unsigned int t = 0; t < aNpdt; ++t)
    {
        if (fabs((exp.at(t).evaluate(optSol))) > 1.e-6)
        {
            uint t_hour = std::ceil(t * TimeStep(t)) + mParentCompo->HistNbHours();
            while ((t_hour) > mParentCompo->TableYearsHours()[year] && year < (mParentCompo->TableYearsHours()).size() - 1) {
                year += 1;
                //cInfo() << "year:" << year << "hours" << t_hour << mParentCompo->TableYearsHours() << "mhistnbhour" << mParentCompo->HistNbHours() << "table" << mParentCompo->ImpactLevelizationTable();
            }
            aProduction += (aCoeff * (exp.at(t).evaluate(optSol)) + bCoeff) * TimeStep(t) * mParentCompo->ImpactLevelizationTable().at(year);
        }
    }
}


void SubModel::computeDiscounted(uint aNpdt, uint aShift, MIPModeler::MIPExpression1D exp, const double* optSol, double& aDiscounted)
{
    int year = 0;
    for (unsigned int t = 0; t < aNpdt; ++t)
    {
        if (fabs(exp.at(t).evaluate(optSol)) > 1.e-6)
        {
            //uint t_hour=qCeil((t+startingAbsoluteTimeStep())*TimeStep(t)) + mHistNbHours;
            uint t_hour = std::ceil(t * TimeStep(t)) + mParentCompo->HistNbHours(); // + mHistNbHours
            while ((t_hour) > mParentCompo->TableYearsHours()[year] && year < mParentCompo->TableYearsHours().size() - 1) {
                year += 1;
            }
            aDiscounted += (exp.at(t).evaluate(optSol)) * mParentCompo->LevelizationTable().at(year);
        }
    }
}

bool SubModel::isSizeOptimized()
{
    if (getOptimalSizeExpression() == "") return false;
    InputParam::ModelParam *pParam = getInputParam()->getParameter(getOptimalSizeExpression());
    if (pParam == nullptr) return false;
    else {
        double vValue;
        if (pParam->getNumValue(vValue)) {
            return (vValue > 0) ? false : true;
        }
        else
            return false;
    }    
}
bool SubModel::isPriceOptimized()
{
    return false;
}
//-----------------------------------------------------------------------------

std::string SubModel::getLabelValue(const std::string& aLabel) const
{
    auto vIter = mLabelMap.find(aLabel);
    if (vIter != mLabelMap.end()) {
        return vIter->second;
    }
    else {
        return "";
    }
}

void SubModel::exportIndicators(std::fstream& out, std::string name, const std::string &range, const std::vector< std::string >& refLabelList, 
    const bool showDescription, bool forced, const bool isRollingHorizon)
{
    /* labels applies to all indicator(same redundant value in all lines) */

    //filter labels that are not defined in refLabelList (from TecEcoAnalysis) and order the labels 
    std::vector<std::string> vLabelList = {};
    for (auto const& label : refLabelList) {
        vLabelList.push_back(getLabelValue(label));
    }

    const InputParam::t_Indicators& vIndicators = mInputIndicators->getIndicators();
    bool vIsSizeOptimized = isSizeOptimized();
    bool vIsPriceOptimized = isPriceOptimized();
    for (auto& vIndicator : vIndicators) {
        vIndicator->Export(out, name, range, forced, vIsSizeOptimized, vIsPriceOptimized, 
            isRollingHorizon, mOptimalSizeAllCycles, showDescription, vLabelList);
    }   
}

void SubModel::writeSolution(const double* optimalSolution, std::map<std::string, std::vector<double>>& resultats)
{
    for (auto& [key, vExpr] : getMapIOExpression()) {
        if (vExpr) {
            if (vExpr->getType() == EIOModelType::eMIPExpression) {
                if (key == getOptimalSizeExpression()) {
                    const t_value& optimalSize = vExpr->evaluate(optimalSolution);
                    mParentCompo->setOptimalSize((double)std::get<eDouble>(optimalSize));
                }
            }
            else {
                const t_value& optimalValues = vExpr->evaluate(optimalSolution);

                if (std::holds_alternative<vector<double>>(optimalValues)) {
                    const vector<double>& vOptimalValues = (const vector<double> &)std::get<vector<double>>(optimalValues);                                        
                    std::string vName = Name() + "." + key;
                    resultats[vName].resize(mHorizon + mNpdtPast);
                    std::vector<double>& pResults = resultats[vName];
                    size_t vNb = vOptimalValues.size();
                    for (unsigned int t = 0; t < mHorizon; ++t) {
                        if (t < vNb) {
                            pResults[t + npdtPast()] = vOptimalValues[t];
                        }
                    }
                }

            }
        }
    }
}

void SubModel::setTimeData()
{
    if (mTimeStepBeginLP > 0) {
        mMilpNpdt = mTimeStepBeginLP;
        if (mUseTypicalPeriods) {
            cCritical() << "Typical periods models are not compatible with variable time step models";
        }
    }
    else {
        mMilpNpdt = mHorizon;
    }

    mHistState.clear();
    mHistState.resize(mHorizon + mNpdtPast);

    mHistStartUp.clear();
    mHistStartUp.resize(mHorizon + mNpdtPast);

    mHistShutDown.clear();
    mHistShutDown.resize(mHorizon + mNpdtPast);

    mOptimalSizeAllCycles.clear();
}

void SubModel::setTypicalPeriods(const bool& useTypicalPeriods, const uint& aTypicalPeriods, const uint& aNDtTypicalPeriods, const std::vector<int>& aVectTypicalPeriods)
{
    mUseTypicalPeriods = useTypicalPeriods;
    mTypicalPeriods = aTypicalPeriods;
    mNDtTypicalPeriods = aNDtTypicalPeriods;
    mVectTypicalPeriods = aVectTypicalPeriods;
    if (mUseTypicalPeriods && mCondenseVariablesOnTP)
        mCondensedNpdt = mTypicalPeriods * mNDtTypicalPeriods;
    else {
        for (uint i = 0; i < mTimeSteps.size(); i++)
        {
            mVectTypicalPeriods[i] = i;
        }
        mCondensedNpdt = mTimeSteps.size();
    }
}

void SubModel::setTimeSteps(const bool& useVariableTimeSteps, std::vector<double> aTimeSteps, uint64_t aTimeStepBeginLP, uint64_t aTimeStepBeginForecast, uint64_t aDecreaseOptimizationHorizon)    /** TimeStep settings */
{
    mUseVariableTimeSteps = useVariableTimeSteps;
    mHorizon = aTimeSteps.size();
    mTimeSteps = aTimeSteps;
    mTimeStepBeginLP = aTimeStepBeginLP;
    mTimeStepBeginForecast = aTimeStepBeginForecast;
    mDecreaseOptimizationHorizon = aDecreaseOptimizationHorizon;
}

void SubModel::decreaseOptimizationHorizon()                  /** Update mTimeSteps */
{
    if (mDecreaseOptimizationHorizon == 1) {
        cInfo() << "virtuSubModel, mDecreaseOptimizationHorizon = " << mDecreaseOptimizationHorizon;
        uint64_t sumTimeSteps = accumulate(mTimeSteps.begin(), mTimeSteps.end(), 0);

        while (sumTimeSteps + *mptrAbsoluteTimeStep - *mptrTimeshift > *mptrFuturesize)
        {
            int i = mTimeSteps.size() - 1;
            if (mTimeSteps[i] <= 0)
            {
                while (mTimeSteps[i] == 0) { i--; }
            }
            if (mTimeSteps[i] == 1)
                mTimeSteps[i] -= 1;
            else
                mTimeSteps[i] -= *mptrTimeshift;

            sumTimeSteps = accumulate(mTimeSteps.begin(), mTimeSteps.end(), 0);

            cInfo() << "decreaseOptimizationHorizon: *mptrAbsoluteTimeStep = " << *mptrAbsoluteTimeStep;
            cInfo() << "decreaseOptimizationHorizon: *mptrTimeshift = " << *mptrTimeshift;
            cInfo() << "decreaseOptimizationHorizon: *mptrFuturesize = " << *mptrFuturesize;
            cInfo() << "decreaseOptimizationHorizon: mTimeSteps = " << mTimeSteps;
            if (mTimeSteps[i] < 0)
            {
                cCritical() << "Time steps list found is aTimeSteps = " << mTimeSteps;
                Cairn_Exception erreur((std::string)"Error : decreaseOptimizationHorizon Time step sizes must be multiple of timeshift ", -1);
                this->setException(erreur);
                return;
            }
        }
    }
}

void SubModel::setExpSizeMax(const MIPModeler::MIPExpression& aExpInstalled)
{
    if (!mComputeSizeMax) return ;
    if (mMaxValue == MIP_INFINITY) {
        Cairn_Exception error("ERROR : submodel " + parent()->objectName() + " maxbound not defined", -1);
        throw error;
    }
    if (mMinValue == -MIP_INFINITY) {
        Cairn_Exception error("ERROR : submodel " + parent()->objectName() + " minbound not defined", -1);
        throw error;
    }
    if (mOptimalSizeExpression == "") {
        Cairn_Exception error("ERROR : submodel " + parent()->objectName() + " OptimalSizeExpression not defined", -1);
        throw error;
    }

    //Add SizeMax variable
    addVarSizeMax(mMaxValue, mOptimalSizeExpression);

    //Compute SizeMax expression and add constraints
    if (mUseWeightOptimization)
    {
        mExpSizeMax = fabs(mMaxValue) * mVarSizeMax;
        if (mWeight >= 0.) {
            addConstraint(mVarSizeMax == mWeight, "sWeight");
        }
    }
    else
    {
        mExpSizeMax = mVarSizeMax * fabs(mWeight);
        if (mMaxValue >= 0.) {
            addConstraint(mVarSizeMax == mMaxValue, "sMaxVal");
        }
    }
    addConstraint(mExpSizeMax <= aExpInstalled * fabs(mMaxValue) * fabs(mWeight), "sBigMInstalled");
    addConstraint(mExpSizeMax >= aExpInstalled * mMinValue, "sMinSizeInstalled");
}

void SubModel::addVarSizeMax(const double& aMaxVal, const std::string& aStrName)
{
    //Max sizing value of the component (Weighting value, or Absolute value). Negative means optimization, absolute value gives max range value
    if (mUseWeightOptimization)
    {
        if (mLPWeightOptimization) {
            addVariable(mVarSizeMax, aStrName, 0.f, fabs(mWeight));
        }
        else {
            addVariable(mVarSizeMax, aStrName, 0, fabs(mWeight), MIPModeler::MIP_INT);
        }
    }
    else {
        // optimize size on the basis of absolute capicity
        addVariable(mVarSizeMax, aStrName, 0.f, fabs(aMaxVal));
    }
}

double SubModel::getMaxBound() {
    /** Upper bound of size equal to weight * maxval */
    if (mMaxValue == MIP_INFINITY) {
        Cairn_Exception error("ERROR : submodel " + parent()->objectName() + " maxbound not defined", -1);
        throw error;
    }
    return fabs(mMaxValue) * fabs(mWeight);
}

double SubModel::getMinBound() {
    /** Lower bound of size equal to weight * minval */
    if (mMinValue == -MIP_INFINITY) {
        Cairn_Exception error("ERROR : submodel " + parent()->objectName() + " minbound not defined", -1);
        throw error;
    }
    return fabs(mMinValue) * fabs(mWeight);
}

void SubModel::setMaxValue(const double& aMaxVal)
{
    /* 
    *  getMaxBound() = fabs(mMaxValue) * fabs(mWeight) is an upper bound on mExpSizeMax
    */
    mMaxValue = aMaxVal;
}

void SubModel::setMinValue(const double& aMinVal)
{
    /*
    *  mMinValue is a lower bound on mExpSizeMax
    */
    mMinValue = aMinVal;
}

uint64_t SubModel::exprMilpHorizon()
//compute horizon to be considered for expression (never condensed for typical periods)
//Extending a part of the MILP formulation to all time steps if start up is considered on the long-term or for typical periods
{
    if (mUseTypicalPeriods)
    {
        return mHorizon;
    }
    else
    {
        return mMilpNpdt;
    }
}

uint64_t SubModel::varMilpHorizon()
//compute horizon to be considered for variables (may be condensed for typical periods)
//Extending a part of the MILP formulation to all time steps if start up is considered on the long-term or for typical periods
{
    if (mUseTypicalPeriods || mUseVariableTimeSteps)
    {
        return mCondensedNpdt;
    }
    else
    {
        return mMilpNpdt;
    }
}

void SubModel::addStateConstraints(const uint64_t& aCondensedNpdt, const MIPModeler::MIPExpression& aExpInstalled)
{
    addVariable(mState, "State", 0, 1, MIPModeler::MIP_INT, aCondensedNpdt);

    for (uint64_t t = 0; t < mHorizon; t++) {
        mExpState[t] += mState(mVectTypicalPeriods[t]);
    }

    for (uint64_t t = 0; t < mHorizon; t++) {
        addConstraint(mExpState[t] <= aExpInstalled, "NotRunningIfNotInstalled", t);
    }

    // constrain that force relaxed variables mState to float variable = 1. (ON)
    if (mLPModelOnly) {
        for (uint64_t t = 0; t < aCondensedNpdt; t++) {
            mState(t).fix(1);
        }
    }
}

void SubModel::addStartUpShutDown(const uint64_t& aCondensedNpdt, const MIPModeler::MIPExpression& aExpInstalled)
{
    addVariable(mStartUp, "StartUp", 0, 1, MIPModeler::MIP_INT, aCondensedNpdt);
    addVariable(mShutDown, "ShutDown", 0, 1, MIPModeler::MIP_INT, aCondensedNpdt);

    uint64_t nUsedNpdt = exprMilpHorizon();

    for (uint64_t t = 0; t < nUsedNpdt; t++)
    {
        mExpStartUp[t] += mStartUp(mVectTypicalPeriods[t]);
        mExpShutDown[t] += mShutDown(mVectTypicalPeriods[t]);
    }
    
    for (uint64_t t = 0; t < nUsedNpdt; t++)
    {
        addConstraint(mExpStartUp[t] <= aExpInstalled, "NoStartUpIfNotInstalled", t);
        addConstraint(mExpShutDown[t] <= aExpInstalled, "NoShutDownIfNotInstalled", t);
        addConstraint(mExpStartUp[t] - mExpState[t] <= 0, "StateUp", t);
        addConstraint(mExpShutDown[t] + mExpState[t] <= 1, "StateDown", t);

        if (t > 0)
        {
            if (mUseTypicalPeriods)
            {
                //On n'applique pas de contraintes entre 2 periodes types, le parametre mAbsInitialState est utilise
                if ((t + *mptrTimeshift - 1) % mNDtTypicalPeriods != 0 || mActivateConstraintsBetweenTP)
                    addConstraint(mExpState[t] - mExpState[t - 1] - mExpStartUp[t] + mExpShutDown[t] == 0, "StatesUpDown", t);
                else
                    addConstraint(mExpState[t] - mAbsInitialState - mExpStartUp[t] + mExpShutDown[t] == 0, "StatesUpDown", t);
            }
            else {
                addConstraint(mExpState[t] - mExpState[t - 1] - mExpStartUp[t] + mExpShutDown[t] == 0, "StatesUpDown", t);
            }
        }
        else if (*mptrAbsoluteTimeStep > *mptrTimeshift) {
            addConstraint(mExpState[t] - mHistState[mNpdtPast - 1] - mExpStartUp[t] + mExpShutDown[t] == 0, "StatesUpDown", t);
        }
        else {
            addConstraint(mExpState[t] - mAbsInitialState - mExpStartUp[t] + mExpShutDown[t] == 0, "StatesUpDown", t);
        }
    }
}

std::string SubModel::ExpUnit(const std::string &aExpressionName)
{
    /* get the unit value of a given expression */
    std::string vRet = "N/A";
    t_mapIOs::iterator vIter = mIOExpressions.find(aExpressionName);
    if (vIter != mIOExpressions.end()) {
        vRet = vIter->second->getUnit();
    }
    return vRet;
}

const UnitParam* SubModel::pExpUnitParam(const std::string& aExpressionName) {
    /* get a pointer to the UnitParam of a given expression (IO) in order to dynamically pass it to e.g. ZEVariables */
    t_mapIOs::iterator vIter = mIOExpressions.find(aExpressionName);
    if (vIter != mIOExpressions.end()) {
        return vIter->second->pUnitParam();
    }
    return nullptr;
}

void SubModel::addVariable(MIPModeler::MIPVariable0D& variable0D, const std::string& name, const double& lowerBound,
    const double& upperBound, const MIPModeler::MIPVarType& varType)
{
    if (mAllocate) {
        variable0D = MIPModeler::MIPVariable0D(lowerBound, upperBound, varType);
    }
    mModel->add(variable0D, CName(name));
}

void SubModel::addVariable(MIPModeler::MIPVariable1D& variable1D, const std::string& name, const double& lowerBound, 
    const double& upperBound, const MIPModeler::MIPVarType& varType, const int& cols)
{
    if (mAllocate) {
        int dimension = cols;
        if (dimension < 0) dimension = mHorizon;
        variable1D = MIPModeler::MIPVariable1D(dimension, lowerBound, upperBound, varType);
    }
    mModel->add(variable1D, CName(name)); 
}

void SubModel::addConstraint(MIPModeler::MIPConstraint constraint, const std::string& name, const uint& t)
{
    mModel->add(constraint, CName(name,t));
}

bool SubModel::isIndicatorNameUnique(MilpPort* targetPort, std::string quantityName) {
    for (auto &port : mListPort) {    
        if(port == targetPort)
            continue; //Exclude the targetPort!

        if (CairnUtils::toUpper(port->Direction()) != CairnUtils::toUpper(targetPort->Direction()))
            continue; //If the two ports don't have the same direction, then no problem (related to indicator names)!

        if (CairnUtils::toUpper(quantityName) == "STORAGENAME") {
            if(port->getStorageName() != targetPort->getStorageName())
                continue; //Don't have the same StorageName => no problem
        }
        else if (CairnUtils::toUpper(quantityName) == "FLUXNAME") {
            if (port->getFluxName() != targetPort->getFluxName())
                continue; //Don't have the same FluxName => no problem
        }

        if (CairnUtils::toUpper(port->Variable()) == CairnUtils::toUpper(targetPort->Variable()))
            return false;
    }
    return true;
}

std::string SubModel::OptimalSizeUnit() const
{
    if (p_OptimalSizeUnit) {
        return *p_OptimalSizeUnit;
    }
    else {
        return m_OptimalSizeUnit;
    }
}

const std::string* SubModel::pOptimalSizeUnit() const
{
    if (p_OptimalSizeUnit) {
        return p_OptimalSizeUnit;
    }
    else {
        return &m_OptimalSizeUnit;
    }
}