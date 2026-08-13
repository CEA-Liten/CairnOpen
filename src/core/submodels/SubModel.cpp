#include "SubModel.h"
#include "MilpPort.h"
#include "MaterialCarrier.h"
#include "CairnUtils.h"
#include "SubModelLoopEngine.h"

using namespace SubModelComputation;
using namespace CairnUtils;

// --- Construction & identity ------------------------------------------------

SubModel::SubModel(CairnObject* aParent)
   : CairnObject(aParent, aParent ? aParent->objectName() : "_SubModel"), 
     mModel(nullptr),
     mMainCarrier(nullptr),
     mAllocate(true),
     mComputeSizeMax(false),
     mAddStateVariable(false),
     mAddStartUpShutDownVariable(false),
     mWeight(1.),
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
     mTypicalPeriods(0),
     mAbsInitialState(0),
     mHistVariableCostsDiscounted(0.),
     m_OptimalSizeUnit("OptimalSizeUnit"),
     p_OptimalSizeUnit(nullptr),
     mSubObjectiveExpression("N/A"),
     mPenaltyConstraintExpression("N/A"),
     mOpexExpression("N/A"),
     mOptimalSizeExpression(""),
     mHorizon(0),
     mInputConfigParam(nullptr),
     mInputParam(nullptr),
     mInputPerfParam(nullptr),
     mInputTimeSeries(nullptr),
     mInputIndicators(nullptr),
     mInputConfigEnvImpacts(nullptr),
     mInputEnvImpacts(nullptr),
     mInputConfigPortImpacts(nullptr),
     mInputPortImpacts(nullptr),
     mTSInputPortImpacts(nullptr)
{
    declareInputParams(this->objectName());
}

SubModel::~SubModel()
{
    deleteInputParams();
    removeIOs();
    mListPort.clear();
    deleteEnvImpacts();
}

std::string SubModel::ModelClassName() const
{
    const auto* compo = parentComponent();
    return compo ? compo->ModelClassName() : "";
}

// --- Core virtual interface -------------------------------------------------

void SubModel::setTimeData()
{
    mMilpNpdt = (mTimeStepBeginLP > 0) ? mTimeStepBeginLP : mHorizon;

    if (mTimeStepBeginLP > 0 && mUseTypicalPeriods)
        cError() << "Typical periods models are not compatible with variable time step models";

    // Reset historical state vectors
    const std::size_t histSize = mHorizon + mNpdtPast;
    for (auto* hist : { &mHistState, &mHistStartUp, &mHistShutDown })
    {
        hist->clear();
        hist->resize(histSize);
    }

    mOptimalSizeAllCycles.clear();
}

void SubModel::setTypicalPeriods(
    const bool& useTypicalPeriods,
    const uint& aTypicalPeriods,
    const uint& aNDtTypicalPeriods,
    const std::vector<int>& aVectTypicalPeriods)
{
    mUseTypicalPeriods   = useTypicalPeriods;
    mTypicalPeriods      = aTypicalPeriods;
    mNDtTypicalPeriods   = aNDtTypicalPeriods;
    mVectTypicalPeriods  = aVectTypicalPeriods;

    if (mUseTypicalPeriods && mCondenseVariablesOnTP)
    {
        mCondensedNpdt = mTypicalPeriods * mNDtTypicalPeriods;
    }
    else
    {
        for (uint i = 0; i < mTimeSteps.size(); ++i)
            mVectTypicalPeriods[i] = i;
        mCondensedNpdt = static_cast<uint>(mTimeSteps.size());
    }
}

bool SubModel::isSizeOptimized()
{
    if (mOptimalSizeExpression.empty())
        return false;

    const ModelParam* pParam = getInputParam()->getParameter(mOptimalSizeExpression);
    if (!pParam)
        return false;

    double vValue = 0.;
    return pParam->getNumValue(vValue) && vValue <= 0.; 
}

bool SubModel::isPriceOptimized()
{
    return false;
}

uint64_t SubModel::exprMilpHorizon()
{
    /** Horizon for expressions: never condensed for typical periods */
    return mUseTypicalPeriods ? static_cast<uint64_t>(mHorizon) : mMilpNpdt;
}

uint64_t SubModel::varMilpHorizon()
{
    /** Horizon for variables: may be condensed for typical periods */
    return (mUseTypicalPeriods || mUseVariableTimeSteps) ? mCondensedNpdt : mMilpNpdt;
}

void SubModel::buildControlVariables()
{
    for (auto& [key, var] : mListControlIO) 
        var->ComputeValue(mNpdtPast);
}

// --- Component identity, parent, ports & topology ---------------------------

MilpComponent* SubModel::parentComponent() const
{
    auto* component = dynamic_cast<MilpComponent*>(this->parent());
    if (!component)
        throw Cairn_Exception("Model " + Name() + " must have a defined MilpComponent parent", -1);
    return component;
}

int SubModel::checkPortCount()
{
    if (!parentComponent()->isBus() && PortList().empty())
    {
        cError() << Name() << ": model must have at least one port";
        return -1;
    }
    return 0;
}

int SubModel::checkPorts()
{
    if (checkPortCount() < 0)
        return -1;

    for (const MilpPort* port : PortList())
    {
        if (!port) 
        {
            cError() << Name() << ": encountered a null port in PortList()";
            return -1;
        }

        const std::string pID         = port->ID();
        const std::string pName       = port->Name();
        const std::string varName     = port->Variable();
        const std::string direction   = port->Direction();
        const std::string fluxUnit    = port->FluxUnit();
        const std::string storageUnit = port->StorageUnit();
        const std::string exprUnit    = ExpUnit(varName);
        const bool        checkUnit   = CairnUtils::toUpper(port->VarCheckUnit()) == "YES";

        if (!port->getCarrier()) 
        {
            cError() << Name() << ": port [" << pID << "] '" << pName
                     << "' has no carrier defined (variable = '" << varName << "')";
            return -1;
        }

        if (direction != KCONS() && direction != KPROD() && direction != KDATA()) 
        {
            cError() << Name() << ": invalid direction for port [" << pID << "] '" << pName
                     << "' — value = '" << direction << "'";
            cInfo() << "Expected directions: " << KCONS() << ", " << KPROD() << ", " << KDATA();
            return -1;
        }

        if (checkVariable(varName) < 0)
        {
            cError() << Name() << ": port [" << pID << "] '" << pName
                     << "' references unknown variable '" << varName << "'";
            dumpIOExpressions();
            return -1;
        }

        if (checkUnit)
        {
            const bool unitOk = CairnUtils::contains(exprUnit, fluxUnit)
                             || CairnUtils::contains(exprUnit, storageUnit);
            if (!unitOk)
            {
                cError() << Name() << ": unit mismatch for port [" << pID << "] '" << pName << "'"
                    << "Variable: '" << varName << "'"
                    << "Variable unit (model): '" << exprUnit << "'"
                    << "Expected unit (carrier): '" << fluxUnit << "' or '" << storageUnit << "'";
                dumpIOExpressions();
                return -1;
            }
        }
    }

    return 0;
}

MilpPort* SubModel::getPort(const std::string& aPortId)
{
    const auto it = std::find_if(mListPort.cbegin(), mListPort.cend(),
        [&aPortId](const MilpPort* p) { return p->ID() == aPortId; });
    return (it != mListPort.cend()) ? *it : nullptr;
}

MilpPort* SubModel::getPortByType(const std::string& aType, const std::string& aDirection)
{
    for (MilpPort* port : mListPort)
    {
        if (port->getCarrier()->Type() == aType
            && (port->Direction() == aDirection || aDirection == "ANY"))
            return port;
    }
    return nullptr;
}

void SubModel::removePort(MilpPort* port)
{
    if (!port)
        return;

    const auto it = std::find(mListPort.begin(), mListPort.end(), port);
    if (it == mListPort.end())
        throw Cairn_Exception("Cannot delete port '" + port->Name() + "' — not found in " + Name(), -1);

    mListPort.erase(it);
    delete port;
}

void SubModel::removeBusPort(MilpPort* port)
{
    if (!port)
        return;

    const auto it = std::find(mListPort.begin(), mListPort.end(), port);
    if (it != mListPort.end())
        mListPort.erase(it);
}

bool SubModel::isPortIndicatorNameUnique(const MilpPort* targetPort)
{
    if (!targetPort)
        return true;

    const auto& tdir = targetPort->Direction();
    const auto& tvar = targetPort->Variable();

    return std::none_of(mListPort.cbegin(), mListPort.cend(),
        [&](const MilpPort* port)
        {
            return port != targetPort
                && port->Direction() == tdir
                && port->Variable()  == tvar;
        });
}

std::map<std::string, std::string> SubModel::getDefaultPortData(const std::string& portId) const
{
    const auto it = mDefaultPorts.find(portId);
    if (it != mDefaultPorts.end())
        return it->second;

    cWarning() << "Default port not found:" << portId;
    return {};
}

// --- Indicators -------------------------------------------------------------

void SubModel::resetIndicators()
{
    if (mInputIndicators)
    {
        for (auto& indicator : mInputIndicators->getIndicators())
            indicator->resetValue();
        delete mInputIndicators;
        mInputIndicators = nullptr;
    }
    resetHistStoredValues();
}

void SubModel::exportIndicators(std::fstream& out,
    std::string name,
    const std::string& range,
    const std::vector<std::string>& refLabelList,
    const bool showDescription,
    bool forced,
    const bool isRollingHorizon)
{
    // Filter and order labels from refLabelList (defined by TecEcoAnalysis)
    std::vector<std::string> vLabelList;
    vLabelList.reserve(refLabelList.size());
    for (const auto& label : refLabelList)
        vLabelList.push_back(getLabelValue(label));

    const bool vIsSizeOptimized  = isSizeOptimized();
    const bool vIsPriceOptimized = isPriceOptimized();

    for (auto& indicator : mInputIndicators->getIndicators())
    {
        indicator->Export(out, name, range, forced,
            vIsSizeOptimized, vIsPriceOptimized,
            isRollingHorizon, mOptimalSizeAllCycles,
            showDescription, vLabelList);
    }
}

// --- Parameters & time series registration ----------------------------------

void SubModel::declareInputParams(const std::string& name)
{
    deleteInputParams();

    mInputConfigParam   = new InputParam(this, "SubModelInputConfigParam" + name);
    mInputParam         = new InputParam(this, "SubModelInputParam" + name);
    mInputPerfParam     = new InputParam(this, "SubModelInputPerfParam" + name);
    mInputTimeSeries    = new InputParam(this, "SubModelInputDataTS" + name);

    mInputConfigEnvImpacts  = new InputParam(this, "SubModelInputConfigEnvImpactsParam" + name);
    mInputEnvImpacts        = new InputParam(this, "SubModelInputEnvImpactsParam" + name);
    mInputConfigPortImpacts = new InputParam(this, "SubModelInputConfigPortImpactsParam" + name);
    mInputPortImpacts       = new InputParam(this, "SubModelInputPortImpactsParam" + name);
    mTSInputPortImpacts     = new InputParam(this, "SubModelTSInputPortImpactsParam" + name);

    deleteEnvImpacts();

    resetIndicators();
    mInputIndicators = new InputParam(this, "SubModelInputIndicators" + name);
}

void SubModel::deleteInputParams()
{
    auto clear = [](auto*& ptr) {
        delete ptr;
        ptr = nullptr;
    };

    clear(mInputConfigParam);
    clear(mInputParam);
    clear(mInputTimeSeries);
    clear(mInputConfigEnvImpacts);
    clear(mInputEnvImpacts);
    clear(mInputConfigPortImpacts);
    clear(mInputPortImpacts);
    clear(mTSInputPortImpacts);
    clear(mInputPerfParam);
    clear(mInputIndicators);
}

void SubModel::addConfigParameter(
    const std::string& aParamName,
    const t_pvalue& aPtr,
    t_value aDefaultValue,
    t_flag aIsBlocking,
    t_flag aIsUsed,
    const std::string& aDescription,
    const t_unit& aUnit,
    const std::string& aShowConfig)
{
    mInputConfigParam->addParameter(aParamName, aPtr, aDefaultValue,
        aIsBlocking, aIsUsed, aDescription, aUnit, aShowConfig);
}

void SubModel::addParameter(
    const std::string& aParamName,
    const t_pvalue& aPtr,
    t_value aDefaultValue,
    t_flag aIsBlocking,
    t_flag aIsUsed,
    const std::string& aDescription,
    const t_unit& aUnit,
    const std::string& aShowConfig)
{
    mInputParam->addParameter(aParamName, aPtr, aDefaultValue,
        aIsBlocking, aIsUsed, aDescription, aUnit, aShowConfig);
}

void SubModel::addTimeSeries(
    const std::string& aParamName,
    std::vector<double>* aDblePtr,
    t_flag aIsBlocking,
    t_flag aIsUsed,
    const std::string& aDescription,
    const t_unit& aUnit,
    const std::string& aShowConfig,
    double a_default,
    double a_min,
    double a_max)
{
    mInputTimeSeries->addTimeSeries(aParamName, aDblePtr, a_default,
        aIsBlocking, aIsUsed, aDescription, aUnit, aShowConfig, a_min, a_max);
}

void SubModel::addPerfParam(
    const std::string& aParamName,
    std::vector<double>* aPtr,
    t_flag aIsBlocking,
    t_flag aIsUsed,
    const std::string& aDescription,
    const t_unit& aUnit)
{
    mInputPerfParam->addPerfParam(aParamName, aPtr, aIsBlocking, aIsUsed, aDescription, aUnit);
}

// --- IO & expressions (interface) -------------------------------------------

void SubModel::addIO(
    const std::string& aIOName,
    MIPModeler::MIPExpression* aExprPtr,
    t_flag aIsUsed,
    const t_unit& aUnit,
    const std::string& aDescription)
{
    if (assertIONonExistence(aIOName, aExprPtr))
        removeIO(aIOName);
    assertIsNotSizeMaxExp(aExprPtr);
    mIOExpressions[aIOName] = new ModelIO(aIOName, aExprPtr, aIsUsed, aUnit, aDescription);
}

void SubModel::addIO(
    const std::string& aIOName,
    MIPModeler::MIPExpression1D* aExprPtr1D,
    t_flag aIsUsed,
    const t_unit& aUnit,
    const std::string& aDescription)
{
    if (assertIONonExistence(aIOName, aExprPtr1D))
        removeIO(aIOName);
    mIOExpressions[aIOName] = new ModelIO(aIOName, aExprPtr1D, aIsUsed, aUnit, aDescription);
}

void SubModel::addSizeMaxIO(
    const std::string& aIOName,
    MIPModeler::MIPExpression* aExprPtr,
    t_flag aIsUsed,
    const std::string& aUnit,
    const std::string& aDescription)
{
    if (assertIONonExistence(aIOName, aExprPtr))
        removeIO(aIOName);
    assertIsSizeMaxExp(aExprPtr);
    mComputeSizeMax        = true;
    mOptimalSizeExpression = aIOName;
    m_OptimalSizeUnit      = aUnit;
    mIOExpressions[aIOName] = new ModelIO(aIOName, aExprPtr, aIsUsed, aUnit, aDescription);
}

void SubModel::addSizeMaxIO(
    const std::string& aIOName,
    MIPModeler::MIPExpression* aExprPtr,
    t_flag aIsUsed,
    const std::string* pUnit,
    const std::string& aDescription)
{
    if (assertIONonExistence(aIOName, aExprPtr))
        removeIO(aIOName);
    assertIsSizeMaxExp(aExprPtr);
    mComputeSizeMax        = true;
    mOptimalSizeExpression = aIOName;
    p_OptimalSizeUnit      = pUnit;
    mIOExpressions[aIOName] = new ModelIO(aIOName, aExprPtr, aIsUsed, pUnit, aDescription);
}

bool SubModel::assertIONonExistence(const std::string& name, const t_pExpr expression)
{
    for (const auto& [vName, vIO] : mIOExpressions)
    {
        if (!vIO) continue;

        const bool sameName = (vName == name);
        const bool sameExpr = (vIO->getPtr() == expression);

        if (sameName && !sameExpr)
            throw Cairn_Exception("IO expression with same name but different pointer already exists: " + vName, -1);

        if (!sameName && sameExpr)
            throw Cairn_Exception("IO expression with same pointer but different name already exists: " + vName + " vs " + name, -1);

        if (sameName && sameExpr)
            return true; /** Same name and same expression — delete and recreate to update metadata */
    }
    return false;
}

void SubModel::assertIsSizeMaxExp(MIPModeler::MIPExpression* aExprPtr)
{
    if (aExprPtr != &mExpSizeMax)
        throw Cairn_Exception("addSizeMaxIO can only be used for mExpSizeMax — use addIO for other expressions", -1);
}

void SubModel::assertIsNotSizeMaxExp(MIPModeler::MIPExpression* aExprPtr)
{
    if (aExprPtr == &mExpSizeMax)
        throw Cairn_Exception("addIO cannot be used for mExpSizeMax — use addSizeMaxIO instead", -1);
}

void SubModel::addControlIO(
    const std::string& aIOName,
    MIPModeler::MIPExpression1D* aExprPtr1D,
    t_flag aIsUsed,
    const t_unit& aUnit,
    double* aValuePtr,
    double* aDefaultValue,
    bool a_isMPC,
    const std::string& aDescription)
{
    addIO(aIOName, aExprPtr1D, aIsUsed, aUnit);
    mListControlIO[aIOName] = new ControlVar(aIOName, aValuePtr, aDescription, aDefaultValue, a_isMPC);
}

void SubModel::addControlIO(
    const std::string& aIOName,
    MIPModeler::MIPExpression1D* aExprPtr1D,
    t_flag aIsUsed,
    const t_unit& aUnit,
    std::vector<double>* aHistPtr,
    double* aDefaultValue,
    bool a_isMPC,
    const std::string& aDescription)
{
    addIO(aIOName, aExprPtr1D, aIsUsed, aUnit);
    mListControlIO[aIOName] = new ControlVar(aIOName, aHistPtr, aDescription, aDefaultValue, a_isMPC);
}

void SubModel::removeIO(const std::string& name)
{
    const auto it = mIOExpressions.find(name);
    if (it != mIOExpressions.end())
    {
        delete it->second;
        mIOExpressions.erase(it);
    }

    const auto itCtrl = mListControlIO.find(name);
    if (itCtrl != mListControlIO.end())
    {
        delete itCtrl->second;
        mListControlIO.erase(itCtrl);
    }
}

SubModel::IOIterator SubModel::removeIO(SubModel::IOIterator it)
{
    if (it == mIOExpressions.end())
        return it;
    delete it->second;
    return mIOExpressions.erase(it);
}

void SubModel::removeEnvImpactIOs(const std::string& aImpactName)
{
    for (auto it = mIOExpressions.begin(); it != mIOExpressions.end(); )
    {
        if (it->second && CairnUtils::contains(it->first, aImpactName))
        {
            delete it->second;
            it = mIOExpressions.erase(it);
        }
        else
            ++it;
    }

    for (auto it = mListControlIO.begin(); it != mListControlIO.end(); )
    {
        if (it->second && CairnUtils::contains(it->first, aImpactName))
        {
            delete it->second;
            it = mListControlIO.erase(it);
        }
        else
            ++it;
    }
}

void SubModel::removeIOs()
{
    for (auto& [key, value] : mIOExpressions)
        delete value;
    mIOExpressions.clear();

    for (auto& [key, value] : mListControlIO)
        delete value;
    mListControlIO.clear();
}

ModelIO* SubModel::getIOExpression(const std::string& aName) const
{
    const auto it = mIOExpressions.find(aName);
    return (it != mIOExpressions.end()) ? it->second : nullptr;
}

std::vector<ModelIO*> SubModel::getIOExpressions(const EIOModelType& aIOType)
{
    std::vector<ModelIO*> result;
    for (auto& [key, vIO] : mIOExpressions)
    {
        if (vIO && vIO->getType() == aIOType)
            result.push_back(vIO);
    }
    return result;
}

MIPModeler::MIPExpression* SubModel::getMIPExpression(const std::string& aExpressionName) const
{
    const ModelIO* vIO = getIOExpression(aExpressionName);
    if (vIO && vIO->getType() == EIOModelType::eMIPExpression)
        return static_cast<MIPModeler::MIPExpression*>(
            std::get<EIOModelType::eMIPExpression>(vIO->getPtr()));
    return nullptr;
}

MIPModeler::MIPExpression1D* SubModel::getMIPExpression1D(const std::string& aExpressionName) const
{
    const ModelIO* vIO = getIOExpression(aExpressionName);
    if (vIO && vIO->getType() == EIOModelType::eMIPExpression1D)
        return static_cast<MIPModeler::MIPExpression1D*>(
            std::get<EIOModelType::eMIPExpression1D>(vIO->getPtr()));
    return nullptr;
}

MIPModeler::MIPExpression& SubModel::getMIPExpression1D(uint i, const std::string& aExpressionName)
{
    ModelIO* vIO = getIOExpression(aExpressionName);
    if (vIO && vIO->getType() == EIOModelType::eMIPExpression1D && i < vIO->size())
    {
        auto* vExpr = static_cast<MIPModeler::MIPExpression1D*>(
            std::get<EIOModelType::eMIPExpression1D>(vIO->getPtr()));
        return (*vExpr)[i];
    }
    throw Cairn_Exception("MIPExpression '" + aExpressionName
        + "' does not exist or size is less than " + std::to_string(i), -1);
}

// --- Model / solver integration ---------------------------------------------

void SubModel::addStateConstraints(const uint64_t& aCondensedNpdt, const MIPModeler::MIPExpression& aExpInstalled)
{
    addVariable(mState, "State", 0, 1, MIPModeler::MIP_INT, static_cast<int>(aCondensedNpdt));

    for (uint64_t t = 0; t < mHorizon; ++t)
        mExpState[t] += mState(mVectTypicalPeriods[t]);

    for (uint64_t t = 0; t < mHorizon; ++t)
        addConstraint(mExpState[t] <= aExpInstalled, "NotRunningIfNotInstalled", static_cast<uint>(t));

    if (mLPModelOnly)
    {
        for (uint64_t t = 0; t < aCondensedNpdt; ++t)
            mState(t).fix(1);
    }
}

void SubModel::addStartUpShutDown(const uint64_t& aCondensedNpdt, const MIPModeler::MIPExpression& aExpInstalled)
{
    addVariable(mStartUp,  "StartUp",  0, 1, MIPModeler::MIP_INT, static_cast<int>(aCondensedNpdt));
    addVariable(mShutDown, "ShutDown", 0, 1, MIPModeler::MIP_INT, static_cast<int>(aCondensedNpdt));

    const uint64_t nUsedNpdt = exprMilpHorizon();

    for (uint64_t t = 0; t < nUsedNpdt; ++t)
    {
        mExpStartUp[t]  += mStartUp(mVectTypicalPeriods[t]);
        mExpShutDown[t] += mShutDown(mVectTypicalPeriods[t]);
    }

    for (uint64_t t = 0; t < nUsedNpdt; ++t)
    {
        const uint ut = static_cast<uint>(t);
        addConstraint(mExpStartUp[t]  <= aExpInstalled, "NoStartUpIfNotInstalled",  ut);
        addConstraint(mExpShutDown[t] <= aExpInstalled, "NoShutDownIfNotInstalled", ut);
        addConstraint(mExpStartUp[t]  - mExpState[t] <= 0, "StateUp",   ut);
        addConstraint(mExpShutDown[t] + mExpState[t] <= 1, "StateDown", ut);

        if (t > 0)
        {
            if (mUseTypicalPeriods)
            {
                /** No constraint between two typical periods — use mAbsInitialState instead */
                const bool crossPeriod = (t + *mptrTimeshift - 1) % mNDtTypicalPeriods == 0
                                      && !mActivateConstraintsBetweenTP;
                if (crossPeriod)
                    addConstraint(mExpState[t] - mAbsInitialState - mExpStartUp[t] + mExpShutDown[t] == 0, "StatesUpDown", ut);
                else
                    addConstraint(mExpState[t] - mExpState[t - 1] - mExpStartUp[t] + mExpShutDown[t] == 0, "StatesUpDown", ut);
            }
            else
            {
                addConstraint(mExpState[t] - mExpState[t - 1] - mExpStartUp[t] + mExpShutDown[t] == 0, "StatesUpDown", ut);
            }
        }
        else if (*mptrAbsoluteTimeStep > *mptrTimeshift)
        {
            addConstraint(mExpState[t] - mHistState[mNpdtPast - 1] - mExpStartUp[t] + mExpShutDown[t] == 0, "StatesUpDown", ut);
        }
        else
        {
            addConstraint(mExpState[t] - mAbsInitialState - mExpStartUp[t] + mExpShutDown[t] == 0, "StatesUpDown", ut);
        }
    }
}

void SubModel::addVariable(
    MIPModeler::MIPVariable0D& variable0D,
    const std::string& name,
    const double& lowerBound,
    const double& upperBound,
    const MIPModeler::MIPVarType& varType)
{
    if (mAllocate)
        variable0D = MIPModeler::MIPVariable0D(lowerBound, upperBound, varType);
    mModel->add(variable0D, VName(name));
}

void SubModel::addVariable(
    MIPModeler::MIPVariable1D& variable1D,
    const std::string& name,
    const double& lowerBound,
    const double& upperBound,
    const MIPModeler::MIPVarType& varType,
    const int& cols)
{
    if (mAllocate)
    {
        const int dimension = (cols < 0) ? mHorizon : cols;
        variable1D = MIPModeler::MIPVariable1D(dimension, lowerBound, upperBound, varType);
    }
    mModel->add(variable1D, VName(name));
}

void SubModel::addConstraint(
    MIPModeler::MIPConstraint constraint,
    const std::string& name,
    const uint& t)
{
    mModel->add(constraint, CName(name, t));
}

// --- Expression allocation & lifecycle --------------------------------------

void SubModel::fillExpression(
    MIPModeler::MIPExpression1D& aExpress1D,
    MIPModeler::MIPVariable1D& aVariable)
{
    if (aVariable.getDims() != aExpress1D.size())
        throw Cairn_Exception(Name() + ": expression and variable sizes do not match", -1);

    for (uint64_t t = 0; t < mHorizon; ++t)
        aExpress1D[t] += aVariable(t);
}

void SubModel::closeExpression(MIPModeler::MIPExpression& aExpress)
{
    aExpress.close();
}

void SubModel::closeExpression1D(MIPModeler::MIPExpression1D& aExpress1D)
{
    for (int i = 0; i < static_cast<int>(aExpress1D.size()); ++i)
        aExpress1D.at(i).close();
}

void SubModel::allocateExpressions()
{
    /** Allocate IO expressions */
    for (auto& [key, vIO] : mIOExpressions)
    {
        if (!vIO || !vIO->isPExpr()) continue;

        if (vIO->getType() == EIOModelType::eMIPExpression)
        {
            auto* ptrExp0D = static_cast<MIPModeler::MIPExpression*>(
                std::get<EIOModelType::eMIPExpression>(vIO->getPtr()));
            *ptrExp0D = MIPModeler::MIPExpression();
        }
        else if (vIO->getType() == EIOModelType::eMIPExpression1D)
        {
            auto* ptrExp1D = static_cast<MIPModeler::MIPExpression1D*>(
                std::get<EIOModelType::eMIPExpression1D>(vIO->getPtr()));
            *ptrExp1D = MIPModeler::MIPExpression1D(mHorizon); /** IO expressions always sized mHorizon */
        }
    }

    /** Allocate registered 0D expressions */
    for (auto* ptrExp0D : mExpressions0D)
        if (ptrExp0D)
            *ptrExp0D = MIPModeler::MIPExpression();

    /** Allocate registered 1D expressions */
    for (auto& sExp1D : mExpressions1D)
        if (sExp1D.pExp1D)
            *(sExp1D.pExp1D) = MIPModeler::MIPExpression1D(*(sExp1D.pSize));

    /**
     * Allocate base SubModel expressions.
     * These are declared here but not necessarily used in every model.
     * TechnicalSubModel typically uses them; Bus and Operation models may not.
     * This ensures a safe state even if they are referenced without being registered.
     */
    mExpSizeMax  = MIPModeler::MIPExpression();
    mExpState    = MIPModeler::MIPExpression1D(mHorizon);
    mExpStartUp  = MIPModeler::MIPExpression1D(mHorizon);
    mExpShutDown = MIPModeler::MIPExpression1D(mHorizon);
}

void SubModel::closeExpressions()
{
    /** Close IO expressions */
    for (auto& [key, vIO] : mIOExpressions)
        if (vIO) vIO->close();

    /** Close registered 0D expressions */
    for (auto* ptrExp0D : mExpressions0D)
        if (ptrExp0D) ptrExp0D->close();

    /** Close registered 1D expressions */
    for (auto& sExp1D : mExpressions1D)
        if (sExp1D.pExp1D) closeExpression1D(*(sExp1D.pExp1D));

    /** Close base SubModel expressions (safety — see allocateExpressions comment) */
    closeExpression(mExpSizeMax);
    closeExpression1D(mExpState);
    closeExpression1D(mExpStartUp);
    closeExpression1D(mExpShutDown);
}

// --- Computation helpers ----------------------------------------------------

/**
 * Internal helper: resolve the year index for a given time step t.
 */

void SubModel::computeTime(bool bsetValue, uint aNpdt,
    const MIPModeler::MIPExpression1D& exp,
    const double* optSol, 
    double& ret)
{
    double factor = 1.0;
    if (bsetValue) {
        ret = 0.0;
        auto* compo = parentComponent();
        factor = compo ? compo->ExtrapolationFactor() : 1.0;
    }

    runLoop(this, aNpdt, exp, optSol,
        makeTimeAccumulator(this, ret, factor));
}

void SubModel::computeTime(bool bsetValue, uint aNpdt,
    const MIPModeler::MIPExpression1D& exp,
    const double* optSol,
    double& retCharged,
    double& retDischarged)
{
    double factor = 1.0;

    if (bsetValue)
    {
        retCharged = retDischarged = 0.0;
        auto* compo = parentComponent();
        factor = compo ? compo->ExtrapolationFactor() : 1.0;
    }

    runLoop(this, aNpdt, exp, optSol,
        makeTimeCDAccumulator(this, retCharged, retDischarged, factor));
}

void SubModel::computeProduction(bool bsetValue, uint aNpdt,
    const MIPModeler::MIPExpression1D& exp,
    const double* optSol,
    const double& aCoeff,
    const double& bCoeff,
    double& ret,
    const bool& integrate)
{
    double factor = 1.0;
    if (bsetValue) {
        ret = 0.0;
        auto* compo = parentComponent();
        factor = compo ? compo->ExtrapolationFactor() : 1.0;
    }

    runLoop(this, aNpdt, exp, optSol,
        makeProdAccumulator(this, ret, factor, aCoeff, bCoeff, integrate));
}

void SubModel::computeProduction(
    bool bsetValue,
    uint aNpdt,
    const MIPModeler::MIPExpression1D& exp,
    const double* optSol,
    const double& aCoeff,
    const double& bCoeff,
    double& retCharged,
    double& retDischarged)
{
    double factor = 1.0;

    if (bsetValue)
    {
        retCharged = retDischarged = 0.0;
        auto* compo = parentComponent();
        factor = compo ? compo->ExtrapolationFactor() : 1.0;
    }

    runLoop(this, aNpdt, exp, optSol,
        makeProdCDAccumulator(this, retCharged, retDischarged,
            factor, aCoeff, bCoeff));
}

void SubModel::computeLvlProduction(bool bsetValue, uint aNpdt,
    const MIPModeler::MIPExpression1D& exp,
    const double* optSol,
    const double& aCoeff,
    const double& bCoeff,
    double& ret)
{
    auto* compo = parentComponent();
    double factor = compo ? (1.0 / compo->ExtrapolationFactor()) : 1.0;

    if (bsetValue) {
        ret = 0.0;
        factor = 1.0;
    }

    runLoop(this, aNpdt, exp, optSol,
        makeLvlAccumulator(this, ret, aCoeff, bCoeff, factor, false));
}

void SubModel::computeLvlProduction(bool bsetValue, uint aNpdt,
    const MIPModeler::MIPExpression1D& exp,
    const double* optSol,
    const double& aCoeff,
    const double& bCoeff,
    double& retCharged,
    double& retDischarged)
{
    auto* compo = parentComponent();

    // HIST: extrapolation already embedded in LevelizationTable
    double factor = compo ? (1.0 / compo->ExtrapolationFactor()) : 1.0;

    if (bsetValue)
    {
        retCharged = retDischarged = 0.0;
        factor = 1.0;   // PLAN: no extrapolation needed
    }

    runLoop(this, aNpdt, exp, optSol,
        [&](uint t, double val)
        {
            static int year = 0;
            year = resolveYear(
                t,
                TimeStep(t),
                compo->HistNbHours(),
                year,
                compo->TableYearsHours()
            );

            // Levelization factor
            const double lvl = compo->LevelizationTable().at(year);

            // Contribution
            const double contrib =
                (aCoeff * val + bCoeff) *
                TimeStep(t) *
                lvl *
                factor;

            // Charged/discharged split
            if (val > kEpsilon) retDischarged += contrib;
            if (val < -kEpsilon) retCharged += contrib;
        }
    );
}

void SubModel::computeConsumption(
    bool bsetValue,
    uint aNpdt,
    const MIPModeler::MIPExpression1D& exp,
    const double* optSol,
    const double& aCoeff,
    const double& bCoeff,
    double& aConsumption)
{
    double factor = 1.0;

    if (bsetValue)   // PLAN: extrapolate; HIST: accumulate without extrapolation
    {
        aConsumption = 0.0;
        auto* compo = parentComponent();
        factor = compo ? compo->ExtrapolationFactor() : 1.0;
    }

    runLoop(this, aNpdt, exp, optSol,
        [&](uint t, double val)
        {
            const double ts = TimeStep(t);
            aConsumption -= (aCoeff * val + bCoeff) * ts * factor;
        }
    );
}

void SubModel::computeLvlConsumption(bool bsetValue, uint aNpdt,
    const MIPModeler::MIPExpression1D& exp,
    const double* optSol,
    const double& aCoeff,
    const double& bCoeff,
    double& aConsumption)
{
    auto* compo = parentComponent();

    // HIST: extrapolation already embedded in LevelizationTable
    double factor = compo ? (1.0 / compo->ExtrapolationFactor()) : 1.0;

    if (bsetValue)
    {
        aConsumption = 0.0;
        factor = 1.0;   // PLAN: no extrapolation needed
    }

    int year = 0;

    runLoop(this, aNpdt, exp, optSol,
        [&](uint t, double val)
        {
            year = resolveYear(
                t,
                TimeStep(t),
                compo->HistNbHours(),
                year,
                compo->TableYearsHours()
            );

            // Levelization factor
            const double lvl = compo->LevelizationTable().at(year);

            // Contribution (negative for consumption)
            aConsumption -= (aCoeff * val + bCoeff)
                * TimeStep(t)
                * lvl
                * factor;
        }
    );
}

void SubModel::computeLvlImpact(bool bsetValue, uint aNpdt,
    const MIPModeler::MIPExpression1D& exp,
    const double* optSol,
    const double& aCoeff,
    const double& bCoeff,
    double& aProduction)
{
    if (bsetValue)
        aProduction = 0.0;

    auto* compo = parentComponent();
    int year = 0;

    runLoop(this, aNpdt, exp, optSol,
        [&](uint t, double val)
        {
            year = resolveYear(
                t,
                TimeStep(t),
                compo->HistNbHours(),
                year,
                compo->TableYearsHours()
            );

            // Impact-levelization factor
            const double lvl = compo->ImpactLevelizationTable().at(year);

            // Contribution
            aProduction += (aCoeff * val + bCoeff)
                * TimeStep(t)
                * lvl;
        }
    );
}

void SubModel::computeDiscounted(uint aNpdt,
    const MIPModeler::MIPExpression1D& exp,
    const double* optSol,
    double& aDiscounted)
{
    auto* compo = parentComponent();
    int year = 0;

    runLoop(this, aNpdt, exp, optSol,
        [&](uint t, double val)
        {
            year = resolveYear(
                t,
                TimeStep(t),
                compo->HistNbHours(),
                year,
                compo->TableYearsHours()
            );

            // Levelization factor
            const double lvl = compo->LevelizationTable().at(year);

            // Contribution
            aDiscounted += val * lvl;
        }
    );
}

void SubModel::computeIndicator(const MIPModeler::MIPExpression1D& exp,
    const double* optSol,
    double& unDisc,
    double& disc,
    double& histUnDisc,
    double& histDisc,
    bool isEnv)
{
    unDisc = disc = 0.0;

    auto* compo = parentComponent();
    const double extrap = compo ? compo->ExtrapolationFactor() : 1.0;
    const uint histHours = compo ? compo->HistNbHours() : 0;

    int year = 0;

    for (uint t = 0; t < mTimeSteps.size(); ++t)
    {
        const double val = exp[t].evaluate(optSol);
        year = resolveYear(t, TimeStep(t), histHours, year, compo->TableYearsHours());

        const double lvl = levelFactor(this, year, isEnv);

        unDisc += val * extrap;
        disc += val * lvl;

        if (t < *mptrTimeshift) {
            histUnDisc += val;
            histDisc += (val * lvl) / extrap;
        }
    }
}


void SubModel::writeSolution(const double* optimalSolution,
    std::map<std::string, std::vector<double>>& resultats)
{
    auto* compo = parentComponent();

    // Cache npdtPast, horizon and name
    const std::size_t past = static_cast<std::size_t>(npdtPast());
    const std::size_t totalSize = static_cast<std::size_t>(mHorizon + mNpdtPast);
    const std::string baseName = Name();

    // Iterate IO expressions once
    for (auto& kv : getMapIOExpression())
    {
        const std::string& key = kv.first;
        auto* vExpr = kv.second;
        if (!vExpr)
            continue;

        const EIOModelType type = vExpr->getType();

        // --- Scalar expression (0D) ---
        if (type == EIOModelType::eMIPExpression)
        {
            if (key == mOptimalSizeExpression)
            {
                const t_value& optimalSize = vExpr->evaluate(optimalSolution);
                compo->setOptimalSize(static_cast<double>(std::get<eDouble>(optimalSize)));
            }
            continue;
        }

        // --- Vector expression (1D) ---
        const t_value& optimalValues = vExpr->evaluate(optimalSolution);

        // Skip if not a vector<double>
        if (!std::holds_alternative<std::vector<double>>(optimalValues))
            continue;

        // Get reference to vector inside variant (avoid copying)
        const auto& vOptimalValues = std::get<std::vector<double>>(optimalValues);
        const std::size_t vNb = vOptimalValues.size();

        // Build result key once (avoid repeated string concatenation)
        std::string fullName;
        fullName.reserve(baseName.size() + key.size() + 1);
        fullName = baseName;
        fullName += '.';
        fullName += key;

        // Access or create result vector
        auto& pResults = resultats[fullName];

        // Resize result vector
        pResults.resize(totalSize);

        // Copy values
        double* dest = pResults.data();  // faster than operator[]
        const double* src = vOptimalValues.data();

        const std::size_t limit = std::min<std::size_t>(mHorizon, vNb);

        // Copy contiguous block
        for (std::size_t t = 0; t < limit; ++t)
            dest[t + past] = src[t];
    }
}

// --- Economic & size expressions --------------------------------------------

std::string SubModel::ExpUnit(const std::string& aExpressionName)
{
    const auto it = mIOExpressions.find(aExpressionName);
    return (it != mIOExpressions.end()) ? it->second->getUnit() : "N/A";
}

const UnitParam* SubModel::pExpUnitParam(const std::string& aExpressionName)
{
    const auto it = mIOExpressions.find(aExpressionName);
    return (it != mIOExpressions.end()) ? it->second->pUnitParam() : nullptr;
}

const FlagParam* SubModel::pExpIsUsed(const std::string& aExpressionName)
{
    const auto it = mIOExpressions.find(aExpressionName);
    return (it != mIOExpressions.end()) ? it->second->pIsUsed() : nullptr;
}

// --- Environmental impacts --------------------------------------------------

void SubModel::removeImpactExpressionName(const std::string& impactName)
{
    CairnUtils::removeMatchingSubstring(mEnvImpactCostExpression,    impactName);
    CairnUtils::removeMatchingSubstring(mEnvImpactMassExpression,    impactName);
    CairnUtils::removeMatchingSubstring(mEnvGreyImpactCostExpression,impactName);
    CairnUtils::removeMatchingSubstring(mEmbodiedMassExpression,     impactName);
}

void SubModel::deleteEnvImpacts()
{
    for (EnvImpact* impact : mEnvImpacts)
    {
        if (impact)
        {
            removeImpactExpressionName(impact->Name());
            removeEnvImpactIOs(impact->Name());
            delete impact;
        }
    }
    mEnvImpacts.clear();
}

double* SubModel::envEmbodiedMassContribution(int aIdxEnvImpact)
{
    return mEnvImpacts.at(aIdxEnvImpact)->getEnvGreyImpactMass();
}

double* SubModel::envGreyImpactCostContribution(int aIdxEnvImpact)
{
    return mEnvImpacts.at(aIdxEnvImpact)->getEnvGreyImpactPart();
}

double* SubModel::envImpactCostContribution(int aIdxEnvImpact)
{
    return mEnvImpacts.at(aIdxEnvImpact)->getEnvImpactPartPLAN();
}

double* SubModel::envHistImpactCostContribution(int aIdxEnvImpact)
{
    return mEnvImpacts.at(aIdxEnvImpact)->getEnvImpactPartHIST();
}

double* SubModel::envImpactCostContributionDiscounted(int aIdxEnvImpact)
{
    return mEnvImpacts.at(aIdxEnvImpact)->getEnvImpactPartDiscountedPLAN();
}

double* SubModel::envHistImpactCostContributionDiscounted(int aIdxEnvImpact)
{
    return mEnvImpacts.at(aIdxEnvImpact)->getEnvImpactPartDiscountedHIST();
}

double* SubModel::envImpactMassContribution(int aIdxEnvImpact)
{
    return mEnvImpacts.at(aIdxEnvImpact)->getEnvImpactMassPLAN();
}

double* SubModel::envHistImpactMassContribution(int aIdxEnvImpact)
{
    return mEnvImpacts.at(aIdxEnvImpact)->getEnvImpactMassHIST();
}

double* SubModel::envImpactMassContributionDiscounted(int aIdxEnvImpact)
{
    return mEnvImpacts.at(aIdxEnvImpact)->getEnvImpactMassDiscountedPLAN();
}

double* SubModel::envHistImpactMassContributionDiscounted(int aIdxEnvImpact)
{
    return mEnvImpacts.at(aIdxEnvImpact)->getEnvImpactMassDiscountedHIST();
}

// --- Time step & horizon management -----------------------------------------

void SubModel::setTimeSteps(
    const bool& useVariableTimeSteps,
    std::vector<double> aTimeSteps,
    uint64_t aTimeStepBeginLP,
    uint64_t aTimeStepBeginForecast,
    uint64_t aDecreaseOptimizationHorizon)
{
    mUseVariableTimeSteps        = useVariableTimeSteps;
    mHorizon                     = static_cast<int>(aTimeSteps.size());
    mTimeSteps                   = std::move(aTimeSteps);  
    mTimeStepBeginLP             = aTimeStepBeginLP;
    mTimeStepBeginForecast       = aTimeStepBeginForecast;
    mDecreaseOptimizationHorizon = aDecreaseOptimizationHorizon;
}

void SubModel::decreaseOptimizationHorizon()
{
    if (mDecreaseOptimizationHorizon != 1)
        return;

    cInfo() << "decreaseOptimizationHorizon activated for" << Name();

    double sumTimeSteps = std::accumulate(mTimeSteps.begin(), mTimeSteps.end(), 0.);

    while (sumTimeSteps + *mptrAbsoluteTimeStep - *mptrTimeshift > *mptrFuturesize)
    {
        int i = static_cast<int>(mTimeSteps.size()) - 1;
        while (i > 0 && mTimeSteps[i] == 0) --i;

        if (mTimeSteps[i] == 1)
            mTimeSteps[i] -= 1;
        else
            mTimeSteps[i] -= *mptrTimeshift;

        sumTimeSteps = std::accumulate(mTimeSteps.begin(), mTimeSteps.end(), 0.);

        if (mTimeSteps[i] < 0)
        {
            cCritical() << "decreaseOptimizationHorizon: negative time step at index" << i;
            throw Cairn_Exception("decreaseOptimizationHorizon: time step sizes must be multiples of timeshift", -1);
        }
    }
}

// --- Size max & bounds ------------------------------------------------------

void SubModel::setMaxValue(const double& aMaxVal) { mMaxValue = aMaxVal; }
void SubModel::setMinValue(const double& aMinVal) { mMinValue = aMinVal; }

double SubModel::getMaxBound()
{
    if (mMaxValue == MIP_INFINITY)
        throw Cairn_Exception("SubModel " + parent()->objectName() + ": maxbound not defined", -1);
    return std::fabs(mMaxValue) * std::fabs(mWeight);
}

double SubModel::getMinBound()
{
    if (mMinValue == -MIP_INFINITY)
        throw Cairn_Exception("SubModel " + parent()->objectName() + ": minbound not defined", -1);
    return std::fabs(mMinValue) * std::fabs(mWeight);
}

void SubModel::addVarSizeMax(const double& aMaxVal, const std::string& aStrName)
{
    /** Maximum sizing value: negative weight = size optimization, positive = fixed absolute value */
    if (mWeight < 0.)
    {
        if (mLPWeightOptimization)
            addVariable(mVarSizeMax, "Weight", 0.0, std::fabs(mWeight));
        else
            addVariable(mVarSizeMax, "Weight", 0.0, std::fabs(mWeight), MIPModeler::MIP_INT);
    }
    else
    {
        addVariable(mVarSizeMax, aStrName, 0.0, std::fabs(aMaxVal));  
    }
}

void SubModel::setExpSizeMax(const MIPModeler::MIPExpression& aExpInstalled)
{
    if (!mComputeSizeMax)
        return;

    if (mMaxValue == MIP_INFINITY)
        throw Cairn_Exception("SubModel " + parent()->objectName() + ": maxbound not defined", -1);
    if (mMinValue == -MIP_INFINITY)
        throw Cairn_Exception("SubModel " + parent()->objectName() + ": minbound not defined", -1);
    if (mOptimalSizeExpression.empty())
        throw Cairn_Exception("SubModel " + parent()->objectName() + ": OptimalSizeExpression not defined", -1);

    addVarSizeMax(mMaxValue, mOptimalSizeExpression);

    if (mWeight < 0.)
        mExpSizeMax = std::fabs(mMaxValue) * mVarSizeMax;
    else
    {
        mExpSizeMax = mVarSizeMax * std::fabs(mWeight);
        if (mMaxValue >= 0.)
            addConstraint(mVarSizeMax == mMaxValue, "sMaxVal");
        if (std::fabs(mMaxValue) <= 1.e-7)
            addConstraint(aExpInstalled == 0, "nullInstalledSize");
    }

    addConstraint(mExpSizeMax <= aExpInstalled * std::fabs(mMaxValue) * std::fabs(mWeight), "sBigMInstalled");
    addConstraint(mExpSizeMax >= aExpInstalled * mMinValue, "sMinSizeInstalled");
}

// --- Units, labels, metadata ------------------------------------------------

const std::string* SubModel::pCurrency() const
{
    const auto* compo = parentComponent();
    return compo ? compo->pCurrency() : nullptr;
}

const std::string* SubModel::pQuantity(const std::string& a_Quantity) const
{
    return mMainCarrier ? mMainCarrier->pQuantity(a_Quantity) : nullptr;
}

const std::string* SubModel::pOptimalSizeUnit() const
{
    return p_OptimalSizeUnit ? p_OptimalSizeUnit : &m_OptimalSizeUnit;
}

std::string SubModel::getAbsoluteFileName(const std::string& filename) const
{
    const auto* compo = parentComponent();
    return compo ? compo->getAbsoluteFileName(filename) : filename;
}

std::string SubModel::getLabelValue(const std::string& aLabel) const
{
    const auto it = mLabelMap.find(aLabel);
    return (it != mLabelMap.end()) ? it->second : ""; 
}

// --- Private helpers --------------------------------------------------------

int SubModel::checkVariable(const std::string& variable) const
{
    return (getMIPExpression(variable) || getMIPExpression1D(variable)) ? 0 : -1;
}

void SubModel::dumpIOExpressions() const
{
    std::ostringstream oss;
    oss << "Available variables:\nVariable\t\tType\t\tUnit\n";
    for (const auto& [key, vIO] : mIOExpressions)
    {
        if (!vIO) continue;
        const char* typeStr = nullptr;
        switch (vIO->getType())
        {
        case EIOModelType::eMIPExpression:   typeStr = "(Scalar)"; break;
        case EIOModelType::eMIPExpression1D: typeStr = "(Vector)"; break;
        default: continue;
        }
        oss << key << "\t\t" << typeStr << "\t\t" << vIO->getUnit() << "\n";
    }
    cInfo() << oss.str();
}
