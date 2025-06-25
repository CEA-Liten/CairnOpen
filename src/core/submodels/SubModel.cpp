#include "SubModel.h"
#include "MilpComponent.h"
#include "MilpPort.h"
#include "CairnUtils.h"
using namespace CairnUtils;

SubModel::SubModel(QObject* aParent) :   
    QObject(aParent),
    mException(Cairn_Exception()),
    mModel(nullptr),
    mParentCompo(nullptr),
    mEnergyVector(nullptr),
    mAllocate(true),
    mUseAgeing(false),
    mAddStateVariable(false),
    mWeight(1.), //needed for models that doesn't have parameter "Weight" such as Ramp
    mUseWeightOptimization(false),
    mLPWeightOptimization(false),
    mLPModelOnly(false),
    mMaxBound(-1),
    mMinBound(-1),
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
    m_OptimalSizeUnit("N/A"),
    p_OptimalSizeUnit(nullptr),
    mSubObjectiveExpression("N/A"),
    mPenaltyConstraintExpression("N/A"),
    mOpexExpression("N/A"),
    mOptimalSizeExpression("")
{
    QString aName = "";
    if(aParent)  aName = aParent->objectName();

    // Param
    mInputParam = new InputParam (this,"SubModelbaseInputParam" + aName) ;
    // Carto
    mInputPerfParam = new InputParam(this, "SubModelbaseInputPerfParam" + aName);
    // Config
    mInputData = new InputParam (this,"SubModelbaseInputData" + aName) ;
    // TimeSeries
    mInputDataTS = new InputParam(this, "SubModelbaseInputDataTS" + aName);
       
    // Indicateurs, Impact
    mInputIndicators = new InputParam(this, "SubModelbaseInputIndicators" + aName);
    mInputEnvImpacts = new InputParam(this, "SubModelbaseInputEnvImpactsParam" + aName);
    mInputPortImpacts = new InputParam(this, "SubModelbaseInputPortImpactsParam" + aName);
    mTSInputPortImpacts = new InputParam(this, "SubModelbaseTSInputPortImpactsParam" + aName); 
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
    
    for (auto &[vKey, value] : mIOExpressions) {
        if (value) delete value;
    }
    for (auto &[vKey, value] : mListControlIO) {
        if (value) delete value;
    }

    mListPort.clear();

    for (EnvImpact* impact : mEnvImpacts) {
        if (impact) delete(impact);
    }
}

void SubModel::finalizeModelData()
{
 // nothing here - Can be overridden in individual SubModels
}

MilpPort* SubModel::getPort(const QString& aPortId) 
{
    foreach(MilpPort * lptrport, mListPort)
    {
        if (lptrport->ID() == aPortId) {
            return lptrport;
        }
    }
    return nullptr;
}

MilpPort* SubModel::getPortByType(const QString& aType, const QString& aDirection)
{
    foreach(MilpPort * lptrport, mListPort)
    {
        if (lptrport->ptrEnergyVector()->Type().contains(aType) && (lptrport->Direction() == aDirection || aDirection == "ANY"))
        {
            return lptrport;
        }
    }
    return nullptr;
}

void SubModel::removePort(MilpPort* lptrport)
{
    //TODO: if the port is connected the link should be deleted (delet the port form the Bus::mListPort)
    if (lptrport != nullptr) {
        if (mListPort.removeOne(lptrport)) {
            delete lptrport;
        }
        else {
            Cairn_Exception erreur("Error deleting port", -1);
            this->setException(erreur);
        }
    }
}

void SubModel::removeBusPort(MilpPort* lptrport) {
    if (lptrport != nullptr) {
        mListPort.removeOne(lptrport);
    }
}

void SubModel::addConfig(const QString& aParamName, const t_pvalue &aPtr, t_value aDefaultValue, t_flag aIsBlocking, t_flag aIsUsed, const QString& aDescription, const QString& aUnit, const QList<QString>& aconfigList)
{
    mInputData->addParameter(aParamName, aPtr, aDefaultValue, aIsBlocking, aIsUsed, aDescription, aUnit, aconfigList);
}

void SubModel::addParameter(const QString& aParamName, const t_pvalue &aPtr, t_value aDefaultValue, t_flag aIsBlocking, t_flag aIsUsed, const QString& aDescription, const QString& aUnit, const QList<QString>& aconfigList)
{
    mInputParam->addParameter(aParamName, aPtr, aDefaultValue, aIsBlocking, aIsUsed, aDescription, aUnit, aconfigList);
}

void SubModel::addParameter(const QString& aParamName, std::vector<double>* aPtr, t_flag aIsBlocking, t_flag aIsUsed, const QString& aDescription, const QString& aUnit, const QList<QString>& aconfigList)
{
    mInputParam->addParameter(aParamName, aPtr, std::vector<double>{}, aIsBlocking, aIsUsed, aDescription, aUnit, aconfigList);
}

void SubModel::addPerfParam(const QString& aParamName, const t_pvalue& aPtr, t_value aDefaultValue, t_flag aIsBlocking, t_flag aIsUsed, const QString& aDescription, const QString& aUnit, const QList<QString>& aconfigList)
{
    mInputPerfParam->addParameter(aParamName, aPtr, aDefaultValue, aIsBlocking, aIsUsed, aDescription, aUnit, aconfigList);
}

void SubModel::addPerfParam(const QString& aParamName, std::vector<double>* aPtr, t_flag aIsBlocking, t_flag aIsUsed, const QString& aDescription, const QString& aUnit, const QList<QString>& aconfigList)
{
    mInputPerfParam->addParameter(aParamName, aPtr, std::vector<double>{}, aIsBlocking, aIsUsed, aDescription, aUnit, aconfigList);
}

void SubModel::addTimeSeries(const QString& aParamName, std::vector<double>* aDblePtr, t_flag IsBlocking, t_flag aIsUsed, const QString& aDescription, const QString& aUnit, const QList<QString>& aconfigList, double a_default, double a_min, double a_max)
{
    mInputDataTS->addTimeSeries(aParamName, aDblePtr, a_default, IsBlocking, aIsUsed, aDescription, aUnit, aconfigList, a_min, a_max);
}


//Model IO Interface
void SubModel::addIO(const QString& aParamName, MIPModeler::MIPExpression* aExprPtr, const QString aUnit, const QString& aCurrency)
{
    mIOExpressions[aParamName] = new ModelIO(aParamName, aExprPtr, aUnit, aCurrency);
}

void SubModel::addIO(const QString& aParamName, MIPModeler::MIPExpression1D* aExprPtr1D, const QString aUnit, const QString& aCurrency)
{
    mIOExpressions[aParamName] = new ModelIO(aParamName, aExprPtr1D, aUnit, aCurrency);
}

void SubModel::addIO(const QString& aParamName, MIPModeler::MIPExpression* aExprPtr, const QString* aUnit, const QString& aCurrency)
{
    mIOExpressions[aParamName] = new ModelIO(aParamName, aExprPtr, aUnit, aCurrency);
}

void SubModel::addIO(const QString& aParamName, MIPModeler::MIPExpression1D* aExprPtr1D, const QString* aUnit, const QString& aCurrency)
{
    mIOExpressions[aParamName] = new ModelIO(aParamName, aExprPtr1D, aUnit, aCurrency);
}

void SubModel::removeIO(const QString& aName)
{
    if (mIOExpressions.find(aName) != mIOExpressions.end()) {
        delete mIOExpressions[aName];
        mIOExpressions.erase(aName);
    }
}

MIPModeler::MIPExpression* SubModel::getMIPExpression(QString aExpressionName)
{
    ModelIO* vIO = getIOExpression(aExpressionName);
    if (vIO) {
        if (vIO->getType() == EIOModelType::eMIPExpression) {
            return (MIPModeler::MIPExpression*)(std::get<EIOModelType::eMIPExpression>(vIO->getPtr()));
        }
    }
    return nullptr;
}

// Model Rolling Horizon variables
// 
void SubModel::addControlIO(const QString& aParamName, MIPModeler::MIPExpression1D* aExprPtr1D, const QString aUnit, double* aValuePtr, double* aDefaultValue, bool a_isMPC, const QString& aCurrency)
{
    addIO(aParamName, aExprPtr1D, aUnit, aCurrency);
    mListControlIO[aParamName] = new ControlVar(aParamName, aValuePtr, aDefaultValue, a_isMPC);
}

void SubModel::addControlIO(const QString& aParamName, MIPModeler::MIPExpression1D* aExprPtr1D, const QString aUnit, std::vector<double>* aHistPtr, double* aDefaultValue, bool a_isMPC, const QString& aCurrency)
{
    addIO(aParamName, aExprPtr1D, aUnit, aCurrency);
    mListControlIO[aParamName] = new ControlVar(aParamName, aHistPtr, aDefaultValue, a_isMPC);
}

void SubModel::addControlIO(const QString& aParamName, MIPModeler::MIPExpression1D* aExprPtr1D, const QString* aUnit, double* aValuePtr, double* aDefaultValue, bool a_isMPC, const QString& aCurrency)
{
    addIO(aParamName, aExprPtr1D, aUnit, aCurrency);
    mListControlIO[aParamName] = new ControlVar(aParamName, aValuePtr, aDefaultValue, a_isMPC);
}

void SubModel::addControlIO(const QString& aParamName, MIPModeler::MIPExpression1D* aExprPtr1D, const QString* aUnit, std::vector<double>* aHistPtr, double* aDefaultValue, bool a_isMPC, const QString& aCurrency)
{
    addIO(aParamName, aExprPtr1D, aUnit, aCurrency);
    mListControlIO[aParamName] = new ControlVar(aParamName, aHistPtr, aDefaultValue, a_isMPC);
}

void SubModel::buildControlVariables()
{
    for (auto [key, var] : mListControlIO) {
        var->ComputeValue(mNpdtPast);
    }
}

MIPModeler::MIPExpression1D* SubModel::getMIPExpression1D(QString aExpressionName)
{
    ModelIO* vIO = getIOExpression(aExpressionName);
    if (vIO) {
        if (vIO->getType() == EIOModelType::eMIPExpression1D) {
            return (MIPModeler::MIPExpression1D*)(std::get<EIOModelType::eMIPExpression1D>(vIO->getPtr()));
        }
    }
    return nullptr;
}

MIPModeler::MIPExpression& SubModel::getMIPExpression1D(uint i, QString aExpressionName)
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
    Cairn_Exception error(" ERROR: MilpExpression " + aExpressionName + " does not exist in the component model or its size is less than " + QString::number(i), -1);
    throw error;

}

void SubModel::dumpIOExpression1DList()
{
    // Loop on expected input parameters
    qInfo() << "\n\t Vector Expression ;" << " \t\t\t " << " Unit ;";
    for (auto& [key, vExpr] : mIOExpressions) {
        if (vExpr != nullptr) {
            if (vExpr->getType() == EIOModelType::eMIPExpression1D) {
                qInfo() << key << " \t\t\t " << vExpr->getUnit();
            }
        }
    }
}

void SubModel::dumpIOExpressionList()
{
    // Loop on expected input parameters
    qInfo() << "\n\t Scalar Expression " << " \t\t\t " << " Unit ";
    for (auto& [key, vExpr] : mIOExpressions) {
        if (vExpr != nullptr) {
            if (vExpr->getType() == EIOModelType::eMIPExpression) {
                qInfo() << key << " \t\t\t " << vExpr->getUnit();
            }
        }
    }
}

ModelIO* SubModel::getIOExpression(const QString& aName)
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
    for (auto& [key, vExpr] : mIOExpressions) {
        if (vExpr != nullptr) {
            if (vExpr->getType() == aIOType) {
                vRet.push_back(vExpr);
            }
        }
    }
    return vRet;
}

void SubModel::fillExpression(MIPModeler::MIPExpression1D& aExpress1D, MIPModeler::MIPVariable1D& aVariable) {
    int dimVar = aVariable.getDims();
    int dimExpr = aExpress1D.size();
    if (dimVar != dimExpr) {
        Cairn_Exception cairn_error(getCompoName() + ": expression and variable sizes don't match");
        qCritical() << getCompoName() + ": expression and variable sizes don't match";
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
    for (int i=0; i<(int)aExpress1D.size();i++)
        aExpress1D.at(i).close() ;
}
void SubModel::cleanExpression()
{
    // Loop on expected input parameters
    for (auto& [key, vExpr] : mIOExpressions) {
        if (vExpr != nullptr) {
            vExpr->close();
        }
    }
}

int SubModel::defineDefaultVarNames()
{
    int ierr = 0;
    // check BusVarName,    
    int inumberchange = 0;
    QString varUseCheck = "none";
    QListIterator<MilpPort*> iport(PortList());
    while (iport.hasNext())
    {
        MilpPort* port = iport.next();
        QString portType = port->PortType();
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
        qCritical() << " ERROR at port " << (port->Name())
            << " You should set Variable= property in <port> field to be able to impose same value through BusSameValue bus ";
        return -1;
    }
    if (port->Direction() == "")
    {
        qCritical() << " ERROR at port " << (port->Name())
            << " You should set Use= DATAEXCHANGE in <port> field to be able to impose same value through BusSameValue bus ";
        return -1;
    }
    return 0;
}

int SubModel::checkBusFlowBalanceVarName(MilpPort* port, int& inumberchange, QString& varUseCheck)
{
    QString varName = port->Variable();
    QString varUse = port->Direction();
    if (varUseCheck != varUse) {
        varUseCheck = varUse;
        inumberchange++;
    }

    // Uncomplete description -> use default definitions functions of models
    if (varName == "") {
        if (!defineDefaultVarNames(port)) {
            qCritical() << " ERROR at port " << (port->Name())
                << " No default variable exist for that port type " << (port->ptrEnergyVector()->Type())
                << " - You should set Variable field to send from Converter to BusFlowBalance bus ";
            return -1;
        }        
    }
    if (varUse == "") {
        qCritical() << " ERROR at port " << (port->Name())
            << " No default use type Producer / Consumer exist for that port type " << (port->ptrEnergyVector()->Type())
            << " - You should set Variable & Use fields to exchanger from Converter to BusFlowBalance bus ";
        return -1;
    }
    if (inumberchange == 0) {
        qCritical() << " ERROR on component " << (getCompoName())
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
    if (port->ptrEnergyVector() == nullptr) return 0;
    QString varName = port->Variable() ;
    QString varUse = port->Direction() ;
    QString varFluxUnit = port->getFluxUnit();
    QString varStorageUnit = port->getStorageUnit();
    QString varPotentialUnit = port->getPotentialUnit();
    QString ExprUnit = getUnit(varName) ;

    if (varUse == "") {
        qWarning() << ("Variable "+varName+" neither defined as INPUT nor OUTPUT for the component ! ") ;
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
        qCritical() << ("ERROR no Milp Expression "+varName+" exist in submodel ! ") ;
        qInfo() << "Available Milp Expressions are";
        dumpIOExpressionList();
        dumpIOExpression1DList();
        return -1 ;
    }

    if (port->PortType()=="BusSameValue")
    {
         if (!ExprUnit.contains(varFluxUnit) && !ExprUnit.contains(varStorageUnit) && !ExprUnit.contains(varPotentialUnit)  && port->VarCheckUnit() == "Yes")
         {
             qCritical() << (" ERROR MilpExpression "+varName+" unit found is "+ExprUnit) ;
             qCritical() << (" Unit should be either "+varFluxUnit+" or "+varStorageUnit+" or "+varPotentialUnit) ;
             dumpIOExpressionList();
             dumpIOExpression1DList();
             return -1 ;
         }
    }
    if (port->PortType()=="BusFlowBalance")
    {
         if (! ExprUnit.contains(varFluxUnit) && ! ExprUnit.contains(varStorageUnit) && ! ExprUnit.contains(varPotentialUnit)  && QString::compare(port->VarCheckUnit(),"Yes", Qt::CaseInsensitive) == 0)
         {
             qCritical() << (" ERROR MilpExpression "+varName+" flux unit (from SubModel) is "+ExprUnit) ;
             qCritical() << (" But unit expected by energy vector is "+varFluxUnit) ;
             dumpIOExpressionList();
             dumpIOExpression1DList();
             return -1 ;
         }
    }
    return ierr ;
}


void SubModel::addStateConstraints(const uint64_t& aNpdt, const uint64_t& aCondensedNpdt)
{
    if (mAllocate) {
        mState = MIPModeler::MIPVariable1D(aCondensedNpdt, 0, 1, MIPModeler::MIP_INT);
        mExpState = MIPModeler::MIPExpression1D(aNpdt);
    }
    else {
        closeExpression1D(mExpState);
    }
    addVariable(mState, "State");

    for (uint64_t t = 0; t < aNpdt; t++) {
        mExpState[t] += mState(mVectTypicalPeriods[t]);
    }
    for (uint64_t t = 0; t < aNpdt; t++) {
        addConstraint(mExpState[t]<=mVarInstalled,"NotRunningIfNotInstalled",t);
    }

    // constrain that force relaxed variables mState to float variable = 1. (ON)
    if (mLPModelOnly) {
        for (uint64_t t = 0; t < aCondensedNpdt; t++) {
            mState(t).fix(1);
        }
    }
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
        uint t_hour = qCeil(t * TimeStep(t)) + mParentCompo->HistNbHours();
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
            uint t_hour = qCeil(t * TimeStep(t)) + mParentCompo->HistNbHours();
            while ((t_hour) > mParentCompo->TableYearsHours()[year] && year < (mParentCompo->TableYearsHours()).size() - 1) {
                year += 1;
                //qInfo() << "year:" << year << "hours" << t_hour << mParentCompo->TableYearsHours() << "mhistnbhour" << mParentCompo->HistNbHours() << "table" << mParentCompo->LevelizationTable();
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
            uint t_hour = qCeil(t * TimeStep(t)) + mParentCompo->HistNbHours();
            while ((t_hour) > mParentCompo->TableYearsHours()[year] && year < (mParentCompo->TableYearsHours()).size() - 1) {
                year += 1;
                //qInfo() << "year:" << year << "hours" << t_hour << mParentCompo->TableYearsHours() << "mhistnbhour" << mParentCompo->HistNbHours() << "table" << mParentCompo->LevelizationTable();
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
        uint t_hour = qCeil(t * TimeStep(t)) + mParentCompo->HistNbHours();
        while ((t_hour) > mParentCompo->TableYearsHours()[year] && year < (mParentCompo->TableYearsHours()).size() - 1) {
            year += 1;
            //qInfo() << "year:" << year << "hours" << t_hour << mParentCompo->TableYearsHours() << "mhistnbhour" << mParentCompo->HistNbHours() << "table" << mParentCompo->LevelizationTable();
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
            uint t_hour = qCeil(t * TimeStep(t)) + mParentCompo->HistNbHours();
            while ((t_hour) > mParentCompo->TableYearsHours()[year] && year < (mParentCompo->TableYearsHours()).size() - 1) {
                year += 1;
                //qInfo() << "year:" << year << "hours" << t_hour << mParentCompo->TableYearsHours() << "mhistnbhour" << mParentCompo->HistNbHours() << "table" << mParentCompo->ImpactLevelizationTable();
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
            uint t_hour = qCeil(t * TimeStep(t)) + mParentCompo->HistNbHours(); // + mHistNbHours
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
void SubModel::exportIndicators(QTextStream& out, QString name, QString range, bool forced, const bool isRollingHorizon) {
    const InputParam::t_Indicators& vIndicators = mInputIndicators->getIndicators();
    bool vIsSizeOptimized = isSizeOptimized();
    bool vIsPriceOptimized = isPriceOptimized();
    for (auto& vIndicator : vIndicators) {
        vIndicator->Export(out, name, range, forced,
            vIsSizeOptimized, vIsPriceOptimized, isRollingHorizon, mOptimalSizeAllCycles);
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
                    QString vName = getCompoName() + "." + key;
                    resultats[vName.toStdString()].resize(mHorizon + mNpdtPast);
                    std::vector<double>& pResults = resultats[vName.toStdString()];
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
            qCritical() << "Typical periods models are not compatible with variable time step models";
        }
    }
    else {
        mMilpNpdt = mHorizon;
    }
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
void SubModel::setTimeSteps(std::vector<double> aTimeSteps, uint64_t aTimeStepBeginLP, uint64_t aTimeStepBeginForecast, uint64_t aDecreaseOptimizationHorizon)    /** TimeStep settings */
{
    mHorizon = aTimeSteps.size();
    mTimeSteps = aTimeSteps;
    mTimeStepBeginLP = aTimeStepBeginLP;
    mTimeStepBeginForecast = aTimeStepBeginForecast;
    mDecreaseOptimizationHorizon = aDecreaseOptimizationHorizon;
}
void SubModel::decreaseOptimizationHorizon()                  /** Update mTimeSteps */
{
    if (mDecreaseOptimizationHorizon == 1) {
        qInfo() << "virtuSubModel, mDecreaseOptimizationHorizon = " << mDecreaseOptimizationHorizon;
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

            qInfo() << "decreaseOptimizationHorizon: *mptrAbsoluteTimeStep = " << *mptrAbsoluteTimeStep;
            qInfo() << "decreaseOptimizationHorizon: *mptrTimeshift = " << *mptrTimeshift;
            qInfo() << "decreaseOptimizationHorizon: *mptrFuturesize = " << *mptrFuturesize;
            qInfo() << "decreaseOptimizationHorizon: mTimeSteps = " << mTimeSteps;
            if (mTimeSteps[i] < 0)
            {
                qCritical() << "Time steps list found is aTimeSteps = " << mTimeSteps;
                Cairn_Exception erreur("Error : decreaseOptimizationHorizon Time step sizes must be multiple of timeshift ", -1);
                this->setException(erreur);
                return;
            }
        }
    }
}

void SubModel::setExpSizeMax(const double& aMaxVal, const std::string& aStrName)
{
    if (mAllocate)
    {
        setVarSizeMax(aMaxVal);
        mExpSizeMax = MIPModeler::MIPExpression();
        mExpInstalled = MIPModeler::MIPExpression();
    }
    else {
        closeExpression(mExpSizeMax);
        closeExpression(mExpInstalled);
    }

    addVariable(mVarSizeMax, aStrName);
    addVariable(mVarInstalled, "Installed");

    if (mUseWeightOptimization)
    {
        mExpSizeMax = fabs(aMaxVal) * mVarSizeMax;
        mExpInstalled = mVarInstalled;
        // constraints
        if (mWeight >= 0.) {
            addConstraint(mVarSizeMax == mWeight, "sWeight");
            addConstraint(mVarInstalled == 1, "sInstalled");
        }
    }
    else
    {
        mExpSizeMax = mVarSizeMax * fabs(mWeight);
        mExpInstalled = mVarInstalled;
        // constraints
        if (aMaxVal >= 0.) {
            addConstraint(mVarSizeMax == aMaxVal, "sMaxVal");
            addConstraint(mVarInstalled == 1, "sInstalled");
        }
    }
    addConstraint(mExpSizeMax <= mExpInstalled * fabs(aMaxVal) * fabs(mWeight), "sBigMInstalled");
    setMaxBound(aMaxVal);
}

void SubModel::setVarSizeMax(const double& aMaxVal)
{

    //Max sizing value of the component (Weighting value, or Absolute value). Negative means optimization, absolute value gives max range value

    // optimize size on the basis of weighting factor multiplying constant production or storage capacity
    mVarInstalled = MIPModeler::MIPVariable0D(0.f, 1, MIPModeler::MIP_INT);  // variable to multiply by the capex offset to model the "construction" work
    
    if (mUseWeightOptimization)
    {
        if (mLPWeightOptimization)
            mVarSizeMax = MIPModeler::MIPVariable0D(0.f, fabs(mWeight));
        else
            mVarSizeMax = MIPModeler::MIPVariable0D(0.f, fabs(mWeight), MIPModeler::MIP_INT);
    }
    // optimize size on the basis of absolute capicity
    else
        mVarSizeMax = MIPModeler::MIPVariable0D(0.f, fabs(aMaxVal));
}
void SubModel::setMaxBound(const double& aMaxVal)
{
    mMaxBound = fabs(aMaxVal) * fabs(mWeight);
}
void SubModel::setMinBound(const double& aMinVal)
{
    mMinBound = fabs(aMinVal) * fabs(mWeight);
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
    if (mUseTypicalPeriods)
    {
        return mCondensedNpdt;
    }
    else
    {
        return mMilpNpdt;
    }
}
void SubModel::addStartUpShutDown(const uint64_t& nPdtCond, const uint64_t& aNpdt) {
    if (mAllocate)
    {
        mStartUp = MIPModeler::MIPVariable1D(nPdtCond, 0, 1, MIPModeler::MIP_INT);
        mShutDown = MIPModeler::MIPVariable1D(nPdtCond, 0, 1, MIPModeler::MIP_INT);
        mExpStartUp = MIPModeler::MIPExpression1D(aNpdt);
        mExpShutDown = MIPModeler::MIPExpression1D(aNpdt);
    }
    else
    {
        SubModel::closeExpression1D(mExpStartUp);
        SubModel::closeExpression1D(mExpShutDown);
    }
    addVariable(mStartUp, "StartUp");
    addVariable(mShutDown, "ShutDown");

    mHistState.clear();
    mHistState.resize(aNpdt + mNpdtPast);

    uint64_t nPdtUsed = exprMilpHorizon();

    for (uint64_t t = 0; t < nPdtUsed; t++)
    {
        mExpStartUp[t] += mStartUp(mVectTypicalPeriods[t]);
        mExpShutDown[t] += mShutDown(mVectTypicalPeriods[t]);
    }
    
    for (uint64_t t = 0; t < nPdtUsed; t++)
    {
        addConstraint(mExpStartUp[t] <= mExpInstalled, "NoStartUpIfNotInstalled", t);
        addConstraint(mExpShutDown[t] <= mExpInstalled, "NoShutDownIfNotInstalled", t);
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
            else
                addConstraint(mExpState[t] - mExpState[t - 1] - mExpStartUp[t] + mExpShutDown[t] == 0, "StatesUpDown", t);
        }
        else if (*mptrAbsoluteTimeStep > *mptrTimeshift)
        {
            addConstraint(mExpState[t] - mHistState[mNpdtPast - 1] - mExpStartUp[t] + mExpShutDown[t] == 0, "StatesUpDown", t);
        }
        else
        {
            addConstraint(mExpState[t] - mAbsInitialState - mExpStartUp[t] + mExpShutDown[t] == 0, "StatesUpDown", t);
        }
    }
}

QString SubModel::convertUnit(EnergyVector* ptrEnergyVector, QString aUnit) {
    if (aUnit.toUpper().contains("CURRENCY"))
    {
        if (aUnit.toUpper().contains("FLUX")) { return mCurrency + QString("/") + ptrEnergyVector->FluxUnit(); }
        else if (aUnit.toUpper().contains("STORAGE")) { return mCurrency + QString("/") + ptrEnergyVector->StorageUnit(); }
        else if (aUnit.toUpper().contains("ENERGY")) { return mCurrency + QString("/") + ptrEnergyVector->EnergyUnit(); }
        else if (aUnit.toUpper().contains("POWER")) { return mCurrency + QString("/") + ptrEnergyVector->PowerUnit(); }
        else if (aUnit.toUpper().contains("POTENTIAL")) { return mCurrency + QString("/") + ptrEnergyVector->PotentialUnit(); }
        else { return aUnit.replace("Currency", mCurrency); }
    }
    else {
        QStringList fluxUnitList = { QString("FluxUnit"), QString("fluxUnit"), QString("fluxunit") };
        for (QString fluxUnit : fluxUnitList){
            if (aUnit.contains(fluxUnit)) { aUnit = aUnit.replace(fluxUnit, ptrEnergyVector->FluxUnit()); }
        }
        QStringList storageUnitList = { QString("StorageUnit"), QString("storageUnit"), QString("storageunit"), QString("STORAGEUNIT") };
        for (QString storageUnit : storageUnitList) {
            if (aUnit.contains(storageUnit)) { aUnit = aUnit.replace(storageUnit, ptrEnergyVector->StorageUnit()); }
        }
        QStringList energyUnitList = { QString("EnergyUnit"), QString("energyUnit"), QString("energyunit") };
        for (QString energyUnit : energyUnitList) {
            if (aUnit.contains(energyUnit)) { aUnit = aUnit.replace(energyUnit, ptrEnergyVector->EnergyUnit()); }
        }
        QStringList powerUnitList = { QString("PowerUnit"), QString("powerUnit"), QString("powerunit") };
        for (QString powerUnit : powerUnitList) {
            if (aUnit.contains(powerUnit)) { aUnit = aUnit.replace(powerUnit, ptrEnergyVector->PowerUnit()); }
        }
        QStringList potentialUnitList = { QString("PotentialUnit"), QString("potentialUnit"), QString("potentialunit") };
        for (QString potentialUnit : potentialUnitList) {
            if (aUnit.contains(potentialUnit)) { aUnit = aUnit.replace(potentialUnit, ptrEnergyVector->PotentialUnit()); }
        }

        QStringList surfaceUnitList = { QString("SurfaceUnit"), QString("surfaceUnit"), QString("surfaceunit") };
        for (QString surfaceUnit : surfaceUnitList) {
            if (aUnit.contains(surfaceUnit)) {  aUnit = aUnit.replace(surfaceUnit, ptrEnergyVector->SurfaceUnit());  }
        }

        QStringList peakUnitList = { QString("PeakUnit"), QString("peakUnit"), QString("peakunit") };
        for (QString peakUnit : peakUnitList) {
            if (aUnit.contains(peakUnit)) { aUnit = aUnit.replace(peakUnit, ptrEnergyVector->PeakUnit()); }
        }

        QStringList weightUnitList = { QString("WeightUnit"), QString("weightUnit"), QString("weightunit") };
        for (QString weightUnit : weightUnitList) {
            if (aUnit.contains(weightUnit)) { 
                aUnit = aUnit.replace(weightUnit, mWeightUnit); 
                aUnit = aUnit.replace("/-", "");
                aUnit = aUnit.replace("-/", "");
                aUnit = aUnit.replace("-", "");
            }
        }

        return aUnit;
    }
}

QString SubModel::convertUnits(EnergyVector* ptrEnergyVector, const QList<QString>& aUnits)
{
    QString vRet = "";
    for (auto& vUnit : aUnits) {
        if (vRet != "") vRet += ";";
        //Specific treatment for env impacts
        if (vUnit.toUpper().contains("IMPACTUNIT")) {
            for (QString vImpactUnit : mEnvImpactUnitsList) {
                if (vRet != "") vRet += ";";
                vRet += convertUnit(ptrEnergyVector, vUnit.toUpper().replace("IMPACTUNIT", vImpactUnit));
            }
        }
        else {
            vRet += convertUnit(ptrEnergyVector, vUnit);
        }
    }
    return vRet;
}

QString SubModel::getUnit(const QString &aExpressionName)
{
    QString vRet = "N/A";
    t_mapIOs::iterator vIter = mIOExpressions.find(aExpressionName);
    if (vIter != mIOExpressions.end()) {
        vRet = vIter->second->getUnit();
    }
    return vRet;
}

QString SubModel::getOptimalSizeUnit() const {
    if (p_OptimalSizeUnit) {
        return *p_OptimalSizeUnit;
    }
    else {
        return m_OptimalSizeUnit;
    }
}

void  SubModel::setOptimalSizeUnit(const QString aSizeUnit)
{
    m_OptimalSizeUnit = aSizeUnit;
}

void  SubModel::setOptimalSizeUnit(const QString* paSizeUnit)
{
    p_OptimalSizeUnit = paSizeUnit;
}

void SubModel::addVariable(MIPModeler::MIPVariable0D& variable0D, const std::string& name)
{
    mModel->add(variable0D, CName(name));
}
void SubModel::addVariable(MIPModeler::MIPVariable1D& variable1D, const std::string& name)
{
    mModel->add(variable1D, CName(name));
}
void SubModel::addVariable(MIPModeler::MIPVariable2D& variable2D, const std::string& name)
{
    mModel->add(variable2D, CName(name));
}
void SubModel::addConstraint(MIPModeler::MIPConstraint constraint, const std::string& name, const uint& t)
{
    mModel->add(constraint, CName(name,t));
}

bool SubModel::isIndicatorNameUnique(MilpPort* targetPort, QString quantityName) {
    MilpPort* port;
    QListIterator<MilpPort*> iport(mListPort);
    while (iport.hasNext())
    {
        port = iport.next();
        if(port == targetPort)
            continue; //Exclude the targetPort!

        if (port->Direction().toUpper() != targetPort->Direction().toUpper())
            continue; //If the two ports don't have the same direction, then no problem (related to indicator names)!

        if (quantityName.toUpper() == "STORAGENAME") {
            if(port->getStorageName() != targetPort->getStorageName())
                continue; //Don't have the same StorageName => no problem
        }
        else if (quantityName.toUpper() == "FLUXNAME") {
            if (port->getFluxName() != targetPort->getFluxName())
                continue; //Don't have the same FluxName => no problem
        }

        if (port->Variable().toUpper() == targetPort->Variable().toUpper())
            return false;
    }
    return true;
}