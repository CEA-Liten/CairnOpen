
#include "TechnicalSubModel.h"
#include "Cairn_Exception.h"

EnvImpact::EnvImpact(TechnicalSubModel* aParent, std::string aName, std::string aUnit, std::string aShortName) :
mParentModel(aParent),
mName(aName),
mImpactUnit(aUnit),
mShortName(aShortName),
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
mEnvImpactPart(2,0.),
mEnvImpactMass(2,0.),
mEnvImpactPartDiscounted(2, 0.),
mEnvImpactMassDiscounted(2, 0.),
mEmbodiedEnvImpactCost(2,0.),
mEmbodiedEnvImpact(2, 0.),
mReplacementEnvImpact(2, 0.)
{  
    if (!mParentModel) {
        Cairn_Exception error("Error: parent SubModel of EnvImpact " + mName + " is null!", -1);
        throw error;
    }
    if (mShortName == "") mShortName = mName;

    resizeCoeffs(mParentModel->PortList().size()); // reserve a larger capacity to avoid reallocation when a new port is added?!
}

EnvImpact::~EnvImpact() { }

InputParam* EnvImpact::InputEnvImpacts() { return mParentModel->getInputEnvImpactsParam(); }
InputParam* EnvImpact::InputPortImpacts() { return mParentModel->getInputPortImpactsParam(); }
InputParam* EnvImpact::InputPortImpactsTS() { return mParentModel->getInputPortImpactsParamTS(); }
InputParam* EnvImpact::InputPerfParam() { return mParentModel->getInputPerfParam(); }
InputParam* EnvImpact::InputIndicators() { return mParentModel->getInputIndicators(); }

const std::vector<double>& EnvImpact::getTimeSteps() const {
    assert(mParentModel && "Parent model must not be null");
    return mParentModel->timesteps();
}

const double EnvImpact::getLifeTime() const { 
    assert(mParentModel && "Parent model must not be null");
    return mParentModel->LifeTime(); 
}

void EnvImpact::addConfigParameters(std::string aPortName, int j)
{
    // componenet parameters
    if (j == 0) {
        InputEnvImpacts()->addParameter(mName + " PiecewiseEmbodiedCoeff_A", &mPiecewiseEmbodiedCoeff, false, false, mParentModel->pEnvironmentModel(), "If true use piecewise linear functionality for Grey environmental impacts", "", "EnvironmentModel");
    }
    // port parameters
    InputPortImpacts()->addParameter(aPortName + "." + mName + " UseEnvContentTS_A", &mUseTSEnvContentCoeff[j], false, false, mParentModel->pEnvironmentModel(), "Comment", "", "EnvironmentModel");
}

void EnvImpact::addGreyParameters()
{
    //bool
    InputEnvImpacts()->addParameter(mName + " TryRelaxationEmbodiedCoeff_A", &mTryRelaxationEmbodiedCoeff, true, &mPiecewiseEmbodiedCoeff, &mPiecewiseEmbodiedCoeff, "If the EnvGreyContentCoefficient is a convex function of size the linearization variables will be continuous", "", "EnvironmentModel");
    //double
    InputEnvImpacts()->addParameter(mName + " EmbodiedCoefficient_A", &mEmbodiedCoefficient, 0., false, mParentModel->pEnvironmentModel(), "Environmental impact linked to manufacturing - Coefficient A. Grey impact calculation: A*X+B with X the size and A = Multiplying coefficient - Default value is 1 - B = Offset - Default value is 0", SFunctionUnit({ eFTypeDivision, {&mImpactUnit}, "Unit" }), "EnvironmentModel");  /** Always optional : Grey Environmental Emission Mass in kg per unit of input flux (power, flowrate) - Default value is 0 */
    InputEnvImpacts()->addParameter(mName + " EmbodiedOffset_B", &mEmbodiedOffset, 0., false, mParentModel->pEnvironmentModel(), "Environmental impact linked to manufacturing - Offset B. Grey impact calculation: A*X+B with X the size and A = Multiplying coefficient - Default value is 1 - B = Offset - Default value is 0", &mImpactUnit, "EnvironmentModel");  /** Always optional : Grey Environmental Emission Mass in kg per unit of input flux (power, flowrate) - Default value is 0 */
    InputEnvImpacts()->addParameter(mName + " EmbodiedReplacement", &mEmbodiedReplacement, 0., false, mParentModel->pEnvironmentModel(), "Environmental impact linked to future manufacturing for replacement - Replacement env impact contribution ", SFunctionUnit({ eFTypeDivision, { &mImpactUnit, mParentModel->pOptimalSizeUnit()}, "Replacement" }), "EnvironmentModel");
    InputEnvImpacts()->addParameter(mName + " EmbodiedReplacementOffset", &mEmbodiedReplacementOffset, 0., false, mParentModel->pEnvironmentModel(), "The constant part of replacement env impact contribution ", SFunctionUnit({ eFTypeDivision, {&mImpactUnit}, "Replacement" }), "EnvironmentModel");
}

void EnvImpact::addPortParameters(std::string aPortName, int j, EnergyVector* aCarrier)
{
    //vector (time series)
    InputPortImpactsTS()->addTimeSeries(aPortName + "." + mName + " EnvContentTS_A", &mTSEnvContentCoeff[j], 1.0, SFunctionFlag({ eFTypeNotAnd, {}, {mParentModel->pEnvironmentModel() , (bool*)&mUseTSEnvContentCoeff[j]} }), SFunctionFlag({ eFTypeNotAnd, {}, {mParentModel->pEnvironmentModel() ,  (bool*)&mUseTSEnvContentCoeff[j]} }), "Environmental impact linked to usage - Time profile of coefficient A. Impact calculation: A*x+B with x the flow and A = Multiplying coefficient", SFunctionUnit({ eFTypeDivision, {&mImpactUnit, aCarrier->pStorageUnit()} }), "EnvironmentModel");
    //double
    InputPortImpacts()->addParameter(aPortName + "." + mName + " EnvContentCoefficient_A", &mEnvContentCoefficients[j], 0., false, SFunctionFlag({ eFTypeNotAnd, {(bool*)&mUseTSEnvContentCoeff[j]}, {mParentModel->pEnvironmentModel()} }), "Environmental impact linked to usage - Coefficient A. Impact calculation: A*x+B with x the flow and A = Multiplying coefficient - Default value is 1 - B = Offset - Default value is 0", SFunctionUnit({ eFTypeDivision, {&mImpactUnit, aCarrier->pStorageUnit()} }), "EnvironmentModel");
    InputPortImpacts()->addParameter(aPortName + "." + mName + " EnvContentOffset_B", &mEnvContentOffsets[j], 0., false, mParentModel->pEnvironmentModel(), "Environmental impact linked to usage - Offset B. Impact calculation: A*x+B with x the flow and A = Multiplying coefficient - Default value is 1 - B = Offset - Default value is 0", &mImpactUnit, "EnvironmentModel");
}

void EnvImpact::addPerfParameters() {
    //vector
    InputPerfParam()->addPerfParam(mName + " CapacitySetPoint", &mImpactCapacitySetPoint, &mPiecewiseEmbodiedCoeff, &mPiecewiseEmbodiedCoeff, "name of vector impact capacity that will be defined from DataFile specification by the User");
    InputPerfParam()->addPerfParam(mName + " SetPoint", &mImpactSetPoint, &mPiecewiseEmbodiedCoeff, &mPiecewiseEmbodiedCoeff, "name of vector impact SetPoint that will be defined from DataFile specification by the User");
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
    InputIndicators()->addIndicator(mName + " Operational impact mass", &mEnvImpactMass, mParentModel->pEnvironmentModel(), 
        "Mass of the direct environmental impact linked to usage ", &mImpactUnit, mShortName + "DirectMass"); 

    InputIndicators()->addIndicator(mName + " Embodied impact mass", &mEmbodiedEnvImpact, mParentModel->pEnvironmentModel(), 
        "Mass of the grey emissions linked to manufacturing", &mImpactUnit, mShortName + "GreyMass"); 

    InputIndicators()->addIndicator(mName + " Replacement impact mass", &mReplacementEnvImpact, mParentModel->pEnvironmentModel(), 
        "Env impact of replacement linked to future manufacturing for replacement", &mImpactUnit, mShortName + "Replacement"); 
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
    constexpr double EPSILON = 1.e-6;
    constexpr double HOURS_PER_YEAR = 8760.0;

    const double lifeTime = getLifeTime();

    // Validate lifetime
    if (lifeTime <= EPSILON) {
        const std::string compoName = mParentModel ? " for component " + mParentModel->Name() : "";
        throw Cairn_Exception(
            "Invalid LifeTime" + compoName +
            ": must be positive (got " + std::to_string(lifeTime) + ")");
    }

    // Pre-compute the constant part
    const MIPModeler::MIPExpression replacementImpact =
        (mEmbodiedReplacement * aExpSizeMax + mEmbodiedReplacementOffset * aExpInstalled)
        / (lifeTime * HOURS_PER_YEAR);

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