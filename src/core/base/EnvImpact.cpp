
#include "SubModel.h"
#include "TechnicalSubModel.h"
#include "Cairn_Exception.h"

#include "Constants.h"
using namespace CairnConstants;

EnvImpact::EnvImpact(TechnicalSubModel* parent, const std::string& name,
    const std::string& unit, const std::string& shortName) 
    :
mParentModel(parent),
mName(name),
mImpactUnit(unit),
mShortName(shortName.empty() ? name : shortName),
mNewlySelected(true),
mEnvImpactCost(0.),
mPiecewiseEmbodiedCoeff(false),
mTryRelaxationEmbodiedCoeff(true),
mEmbodiedCoefficient(0.),
mEmbodiedOffset(0.),
mEmbodiedReplacement(0.),
mEmbodiedReplacementOffset(0.),
mEnvContentCoefficients(0),
mEnvContentOffsets(0),
mEnvImpactPart(kResultCount, 0.),
mEnvImpactMass(kResultCount, 0.),
mEnvImpactPartDiscounted(kResultCount, 0.),
mEnvImpactMassDiscounted(kResultCount, 0.),
mEmbodiedEnvImpactCost(kResultCount, 0.),
mEmbodiedEnvImpact(kResultCount, 0.),
mReplacementEnvImpact(kResultCount, 0.)
{  
    if (!mParentModel)
        throw Cairn_Exception("EnvImpact '" + mName + "': parent TechnicalSubModel must not be null", -1);

    mInputConfigEnvImpacts  = mParentModel->getInputConfigEnvImpactsParam();
    mInputEnvImpacts        = mParentModel->getInputEnvImpactsParam();
    mInputConfigPortImpacts = mParentModel->getInputConfigPortImpactsParam();
    mInputPortImpacts       = mParentModel->getInputPortImpactsParam();
    mInputPortImpactsTS     = mParentModel->getInputPortImpactsParamTS();
    mInputPerfParam         = mParentModel->getInputPerfParam();
    mInputIndicators        = mParentModel->getInputIndicators();

    if (!mInputConfigEnvImpacts  || !mInputEnvImpacts  || 
        !mInputConfigPortImpacts || !mInputPortImpacts || 
        !mInputPortImpactsTS     || !mInputPerfParam   ||
        !mInputIndicators)
        throw Cairn_Exception("EnvImpact '" + mName + "': injected InputParam pointers must not be null", -1);

    initPortCoefficients(mParentModel->PortList().size());
}

void EnvImpact::addConfigParameters(const std::string& portName, int j)
{
    /** Component-level config parameters - registered only once (j == 0) */
    if (j == 0) {
        mInputConfigEnvImpacts->addParameter(mName + " PiecewiseEmbodiedCoeff_A", &mPiecewiseEmbodiedCoeff, false, false, mParentModel->pEnvironmentModel(), "If true use piecewise linear functionality for Grey environmental impacts", "", "EnvironmentModel");
    }

    /** Port-level config parameters - registered for each port j */
    mInputConfigPortImpacts->addParameter(portName + "." + mName + " UseEnvContentTS_A", &mUseTSEnvContentCoeff[j], false, false, mParentModel->pEnvironmentModel(), "Comment", "", "EnvironmentModel");
}

void EnvImpact::addGreyParameters()
{
    //bool
    mInputEnvImpacts->addParameter(mName + " TryRelaxationEmbodiedCoeff_A", &mTryRelaxationEmbodiedCoeff, true, &mPiecewiseEmbodiedCoeff, &mPiecewiseEmbodiedCoeff, "If the EnvGreyContentCoefficient is a convex function of size the linearization variables will be continuous", "", "EnvironmentModel");
    //double
    mInputEnvImpacts->addParameter(mName + " EmbodiedCoefficient_A", &mEmbodiedCoefficient, 0., false, mParentModel->pEnvironmentModel(), "Environmental impact linked to manufacturing - Coefficient A. Grey impact calculation: A*X+B with X the size and A = Multiplying coefficient - Default value is 1 - B = Offset - Default value is 0", SFunctionUnit({ eFTypeDivision, {&mImpactUnit}, "Unit" }), "EnvironmentModel");  /** Always optional : Grey Environmental Emission Mass in kg per unit of input flux (power, flowrate) - Default value is 0 */
    mInputEnvImpacts->addParameter(mName + " EmbodiedOffset_B", &mEmbodiedOffset, 0., false, mParentModel->pEnvironmentModel(), "Environmental impact linked to manufacturing - Offset B. Grey impact calculation: A*X+B with X the size and A = Multiplying coefficient - Default value is 1 - B = Offset - Default value is 0", &mImpactUnit, "EnvironmentModel");  /** Always optional : Grey Environmental Emission Mass in kg per unit of input flux (power, flowrate) - Default value is 0 */
    mInputEnvImpacts->addParameter(mName + " EmbodiedReplacement", &mEmbodiedReplacement, 0., false, mParentModel->pEnvironmentModel(), "Environmental impact linked to future manufacturing for replacement - Replacement env impact contribution ", SFunctionUnit({ eFTypeDivision, { &mImpactUnit, mParentModel->pOptimalSizeUnit()}, "Replacement" }), "EnvironmentModel");
    mInputEnvImpacts->addParameter(mName + " EmbodiedReplacementOffset", &mEmbodiedReplacementOffset, 0., false, mParentModel->pEnvironmentModel(), "The constant part of replacement env impact contribution ", SFunctionUnit({ eFTypeDivision, {&mImpactUnit}, "Replacement" }), "EnvironmentModel");
}

void EnvImpact::addPortParameters(const std::string& portName, int j, EnergyVector* carrier)
{
    //vector (time series)
    mInputPortImpactsTS->addTimeSeries(portName + "." + mName + " EnvContentTS_A", &mTSEnvContentCoeff[j], 1.0, 
        SFunctionFlag({ eFTypeNotAnd, {}, {mParentModel->pEnvironmentModel() , (bool*)&mUseTSEnvContentCoeff[j]} }), 
        SFunctionFlag({ eFTypeNotAnd, {}, {mParentModel->pEnvironmentModel() ,  (bool*)&mUseTSEnvContentCoeff[j]} }), 
        "Environmental impact linked to usage - Time profile of coefficient A. Impact calculation: A*x+B with x the flow and A = Multiplying coefficient", 
        SFunctionUnit({ eFTypeDivision, {&mImpactUnit, carrier->pStorageUnit()} }), "EnvironmentModel");
    
    //double
    mInputPortImpacts->addParameter(portName + "." + mName + " EnvContentCoefficient_A", &mEnvContentCoefficients[j], 0., false, 
        SFunctionFlag({ eFTypeNotAnd, {(bool*)&mUseTSEnvContentCoeff[j]}, {mParentModel->pEnvironmentModel()} }), 
        "Environmental impact linked to usage - Coefficient A. Impact calculation: A*x+B with x the flow and A = Multiplying coefficient - Default value is 1 - B = Offset - Default value is 0", 
        SFunctionUnit({ eFTypeDivision, {&mImpactUnit, carrier->pStorageUnit()} }), "EnvironmentModel");
   
    mInputPortImpacts->addParameter(portName + "." + mName + " EnvContentOffset_B", &mEnvContentOffsets[j], 0., false, 
        mParentModel->pEnvironmentModel(), 
        "Environmental impact linked to usage - Offset B. Impact calculation: A*x+B with x the flow and A = Multiplying coefficient - Default value is 1 - B = Offset - Default value is 0", 
        &mImpactUnit, "EnvironmentModel");
}

void EnvImpact::addPerfParameters() {
    //vector
    mInputPerfParam->addPerfParam(mName + " CapacitySetPoint", &mImpactCapacitySetPoint, &mPiecewiseEmbodiedCoeff, &mPiecewiseEmbodiedCoeff, "name of vector impact capacity that will be defined from DataFile specification by the User");
    mInputPerfParam->addPerfParam(mName + " SetPoint", &mImpactSetPoint, &mPiecewiseEmbodiedCoeff, &mPiecewiseEmbodiedCoeff, "name of vector impact SetPoint that will be defined from DataFile specification by the User");
}

void EnvImpact::addIOExpressions()
{
    mParentModel->
        addIO(mName + " Env impact mass", &mExpOpEnvImpact, mParentModel->pEnvironmentModel(), &mImpactUnit); /** "mName Env impact mass" */
    mParentModel->
        addIO(mName + " Env impact flow", &mExpFlowEnvImpact, mParentModel->pEnvironmentModel(), SFunctionUnit({ eFTypeDivision, {&mImpactUnit}, "h" })); /** "mName Env impact flow" */
    mParentModel->
        addIO(mName + " Env impact cost", &mExpOpEnvImpactCost, mParentModel->pEnvironmentModel(), mParentModel->pCurrency()); /** "mName Env impact cost" */
    mParentModel->
        addIO(mName + " Env embodied impact mass", &mExpEmbodiedEnvImpact, mParentModel->pEnvironmentModel(), &mImpactUnit); /** "mName Env embodied impact mass" */
    mParentModel->
        addIO(mName + " Env embodied impact cost", &mExpEmbodiedEnvImpactCost, mParentModel->pEnvironmentModel(), mParentModel->pCurrency()); /** "mName Env embodied impact cost" */
    mParentModel->
        addIO(mName + " Env impact replacement", &mExpReplacementEnvImpact, mParentModel->pEnvironmentModel(), &mImpactUnit);
}

void EnvImpact::addIndicators()
{
    mInputIndicators->addIndicator(mName + " Operational impact mass", &mEnvImpactMass, mParentModel->pEnvironmentModel(),
        "Mass of the direct environmental impact linked to usage ", &mImpactUnit, mShortName + "DirectMass");

    mInputIndicators->addIndicator(mName + " Embodied impact mass", &mEmbodiedEnvImpact, mParentModel->pEnvironmentModel(),
        "Mass of the grey emissions linked to manufacturing", &mImpactUnit, mShortName + "GreyMass");

    mInputIndicators->addIndicator(mName + " Replacement impact mass", &mReplacementEnvImpact, mParentModel->pEnvironmentModel(),
        "Env impact of replacement linked to future manufacturing for replacement", &mImpactUnit, mShortName + "Replacement");
}

void EnvImpact::initPortCoefficients(size_t portCount)
{
    mEnvContentCoefficients.resize(portCount, 0.);
    mEnvContentOffsets.resize(portCount, 0.);
    mUseTSEnvContentCoeff.resize(portCount, false);
    mTSEnvContentCoeff.resize(portCount);

    const size_t horizon = getTimeSteps().size();
    for (auto& coeffVec : mTSEnvContentCoeff)
        coeffVec.resize(horizon, 0.);
}

void EnvImpact::allocateExpressions(int horizon)
{
    mExpOpEnvImpactCost = MIPModeler::MIPExpression1D(horizon);
    mExpOpEnvImpact = MIPModeler::MIPExpression1D(horizon);
    mExpFlowEnvImpact = MIPModeler::MIPExpression1D(horizon);
    mExpReplacementEnvImpact = MIPModeler::MIPExpression1D(horizon);
}

void EnvImpact::closeExpressions()
{
    for (int i = 0; i < static_cast<int>(mExpOpEnvImpact.size()); ++i)
    {
        mExpOpEnvImpact.at(i).close();
        mExpOpEnvImpactCost.at(i).close();
    }

    for (int i = 0; i < static_cast<int>(mExpFlowEnvImpact.size()); ++i)
        mExpFlowEnvImpact.at(i).close();

    for (int i = 0; i < static_cast<int>(mExpReplacementEnvImpact.size()); ++i)
        mExpReplacementEnvImpact.at(i).close();

    mExpEmbodiedEnvImpactCost.close();
    mExpEmbodiedEnvImpact.close();
}

void EnvImpact::computeEmbodiedEnvImpactContribution(
    const MIPModeler::MIPExpression& aExpSizeMax,
    const MIPModeler::MIPExpression& aExpInstalled)
{
    mExpEmbodiedEnvImpact = mEmbodiedCoefficient * aExpSizeMax + mEmbodiedOffset * aExpInstalled;
}

void EnvImpact::computeReplacementEnvImpactContribution(
    const MIPModeler::MIPExpression& aExpSizeMax,
    const MIPModeler::MIPExpression& aExpInstalled)
{
    const double lifeTime = getLifeTime();

    // Validate lifetime
    if (lifeTime <= kEpsilon) {
        const std::string compoName = mParentModel ? " for component " + mParentModel->Name() : "";
        throw Cairn_Exception(
            "Invalid LifeTime" + compoName +
            ": must be positive (got " + std::to_string(lifeTime) + ")");
    }

    // Pre-compute the constant part
    const MIPModeler::MIPExpression replacementImpact =
        (mEmbodiedReplacement * aExpSizeMax + mEmbodiedReplacementOffset * aExpInstalled)
        / (lifeTime * kHoursPerYear);

    // Assign to all time steps
    const auto& timeSteps = getTimeSteps();
    const size_t horizon = timeSteps.size();
    for (size_t t = 0; t < horizon; ++t) {
        mExpReplacementEnvImpact[t] = timeSteps[t] * replacementImpact;
    }
}

void EnvImpact::computeEnvImpactContribution(
    const size_t j,
    const MIPModeler::MIPExpression1D* aFlux,
    const MIPModeler::MIPExpression& aExpInstalled)
{
    const auto& timeSteps = getTimeSteps();
    const size_t horizon = timeSteps.size();
    const bool useTimeSeries = mUseTSEnvContentCoeff[j];
    const double offset = mEnvContentOffsets[j];

    for (size_t t = 0; t < horizon; ++t) {
        // Compute coefficient (timeseries or constant)
        const double coefficient = useTimeSeries
            ? mTSEnvContentCoeff[j][t]
            : mEnvContentCoefficients[j];

        // Compute impact contribution
        const MIPModeler::MIPExpression impactContribution =
            coefficient * aFlux->at(t) + offset * aExpInstalled;

        // Add to mass and flow
        mExpOpEnvImpact[t] += impactContribution * timeSteps[t];
        mExpFlowEnvImpact[t] += impactContribution;
    }
}

void EnvImpact::computeEnvImpactContributionCost()
{
    mExpEmbodiedEnvImpactCost += mEnvImpactCost * mExpEmbodiedEnvImpact;

    const size_t horizon = getTimeSteps().size();
    for (size_t t = 0; t < horizon; ++t) {
        mExpOpEnvImpactCost[t] += mEnvImpactCost * mExpOpEnvImpact[t];
    }
}

void EnvImpact::evaluateEmbodiedImpact(const double* optSol)
{
    mEmbodiedEnvImpact.at(0) = mEmbodiedEnvImpact.at(1) = mExpEmbodiedEnvImpact.evaluate(optSol);
    mEmbodiedEnvImpactCost.at(0) = mEmbodiedEnvImpactCost.at(1) = mExpEmbodiedEnvImpactCost.evaluate(optSol);
}

double EnvImpact::getLifeTime() const
{
    if (!mParentModel)
        throw Cairn_Exception("EnvImpact '" + mName + "': parent model is null", -1);
    return mParentModel->LifeTime();
}

const std::vector<double>& EnvImpact::getTimeSteps() const
{
    if (!mParentModel)
        throw Cairn_Exception("EnvImpact '" + mName + "': parent model is null", -1);
    return mParentModel->timesteps();
}








