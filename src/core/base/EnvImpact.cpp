
#include "TechnicalSubModel.h"
#include "Cairn_Exception.h"

EnvImpact::EnvImpact(TechnicalSubModel* aParent, std::string aName, std::string aUnit, std::string aShortName) :
mParentModel(aParent),
mName(aName),
mImpactUnit(aUnit),
mShortName(aShortName),
mNewlySelected(true),
mEnvImpactCost(0.),
mPiecewiseEnvGreyContentCoeff(false),
mTryRelaxationEnvGreyContentCoeff(true),
mEnvGreyContentCoefficient(0.),
mEnvGreyContentOffset(0.),
mEnvGreyReplacement(0.),
mEnvGreyReplacementConstant(0.),
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

std::vector<double>& EnvImpact::TimeSteps() { 
    return mParentModel->timesteps(); 
}

double EnvImpact::LifeTime() { 
    return mParentModel->LifeTime(); 
}

void EnvImpact::addConfigParameters(std::string aPortName, int j)
{
    // componenet parameters
    if (j == 0) {
        InputEnvImpacts()->addParameter(mName + " PiecewiseEnvGreyContentCoeff_A", &mPiecewiseEnvGreyContentCoeff, false, false, mParentModel->pEnvironmentModel(), "If true use piecewise linear functionality for Grey environmental impacts", "", "EnvironmentModel");
    }
    // port parameters
    InputPortImpacts()->addParameter(aPortName + "." + mName + " UseEnvContentTS_A", &mUseTSEnvContentCoeff[j], 0, false, mParentModel->pEnvironmentModel(), "Comment", "", "EnvironmentModel");
}

void EnvImpact::addGreyParameters()
{
    //bool
    InputEnvImpacts()->addParameter(mName + " TryRelaxationEnvGreyContentCoeff_A", &mTryRelaxationEnvGreyContentCoeff, true, &mPiecewiseEnvGreyContentCoeff, &mPiecewiseEnvGreyContentCoeff, "If the EnvGreyContentCoefficient is a convex function of size the linearization variables will be continuous", "", "EnvironmentModel");
    //double
    InputEnvImpacts()->addParameter(mName + " EnvGreyContentCoefficient_A", &mEnvGreyContentCoefficient, 0., false, mParentModel->pEnvironmentModel(), "Environmental impact linked to manufacturing - Coefficient A. Grey impact calculation: A*X+B with X the size and A = Multiplying coefficient - Default value is 1 - B = Offset - Default value is 0", SFunctionUnit({ eFTypeDivision, {&mImpactUnit}, "Unit" }), "EnvironmentModel");  /** Always optional : Grey Environmental Emission Mass in kg per unit of input flux (power, flowrate) - Default value is 0 */
    InputEnvImpacts()->addParameter(mName + " EnvGreyContentOffset_B", &mEnvGreyContentOffset, 0., false, mParentModel->pEnvironmentModel(), "Environmental impact linked to manufacturing - Offset B. Grey impact calculation: A*X+B with X the size and A = Multiplying coefficient - Default value is 1 - B = Offset - Default value is 0", &mImpactUnit, "EnvironmentModel");  /** Always optional : Grey Environmental Emission Mass in kg per unit of input flux (power, flowrate) - Default value is 0 */
    InputEnvImpacts()->addParameter(mName + " EnvGreyReplacement", &mEnvGreyReplacement, 0., false, mParentModel->pEnvironmentModel(), "Environmental impact linked to future manufacturing for replacement - Replacement env impact contribution ", SFunctionUnit({ eFTypeDivision, { &mImpactUnit, mParentModel->pOptimalSizeUnit()}, "Replacement" }), "EnvironmentModel");
    InputEnvImpacts()->addParameter(mName + " EnvGreyReplacementConstant", &mEnvGreyReplacementConstant, 0., false, mParentModel->pEnvironmentModel(), "The constant part of replacement env impact contribution ", SFunctionUnit({ eFTypeDivision, {&mImpactUnit}, "Replacement" }), "EnvironmentModel");
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
    InputPerfParam()->addPerfParam(mName + " CapacitySetPoint", &mImpactCapacitySetPoint, &mPiecewiseEnvGreyContentCoeff, &mPiecewiseEnvGreyContentCoeff, "name of vector impact capacity that will be defined from DataFile specification by the User");
    InputPerfParam()->addPerfParam(mName + " SetPoint", &mImpactSetPoint, &mPiecewiseEnvGreyContentCoeff, &mPiecewiseEnvGreyContentCoeff, "name of vector impact SetPoint that will be defined from DataFile specification by the User");
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
        addIO(mName + " Env grey impact mass", &mExpEmbodiedEnvImpact, mParentModel->pEnvironmentModel(), &mImpactUnit); /** "mName Env grey impact mass" */
    mParentModel->
        addIO(mName + " Env grey impact cost", &mExpEmbodiedEnvImpactCost, mParentModel->pEnvironmentModel(), mParentModel->pCurrency()); /** "mName Env grey impact cost" */
    mParentModel->
        addIO(mName + " Env impact replacement", &mExpReplacementEnvImpact, mParentModel->pEnvironmentModel(), &mImpactUnit);
}

void EnvImpact::addIndicators()
{
    InputIndicators()->addIndicator(mName + " Env impact penalty part", &mEnvImpactPart, mParentModel->pEnvironmentModel(), 
        "Penalty contribution due to direct environmental impact linked to usage ", mParentModel->pCurrency(), mShortName + "DirectCost"); 

    InputIndicators()->addIndicator(mName + " Env impact mass", &mEnvImpactMass, mParentModel->pEnvironmentModel(), 
        "Mass of the direct environmental impact linked to usage ", &mImpactUnit, mShortName + "DirectMass"); 

    InputIndicators()->addIndicator(mName + " EnvGrey impact cost", &mEmbodiedEnvImpactCost, mParentModel->pEnvironmentModel(), 
        "Contribution due to the grey emissions linked to manufacturing ", mParentModel->pCurrency(), mShortName + "GreyCost"); 

    InputIndicators()->addIndicator(mName + " EnvGrey impact mass", &mEmbodiedEnvImpact, mParentModel->pEnvironmentModel(), 
        "Mass of the grey emissions linked to manufacturing", &mImpactUnit, mShortName + "GreyMass"); 

    InputIndicators()->addIndicator(mName + " Env impact replacement", &mReplacementEnvImpact, mParentModel->pEnvironmentModel(), 
        "Env impact of replacement linked to future manufacturing for replacement", &mImpactUnit, mShortName + "Replacement"); 
}

void EnvImpact::computeEmbodiedEnvImpactContribution(MIPModeler::MIPExpression aExpSizeMax)
{
    mExpEmbodiedEnvImpact += mEnvGreyContentCoefficient * aExpSizeMax + mEnvGreyContentOffset;
}

void EnvImpact::computeReplacementEnvImpactContribution(MIPModeler::MIPExpression aExpSizeMax)
{
    for (uint64_t t = 0; t < Horizon(); ++t){
        if (LifeTime() > 0) {
            mExpReplacementEnvImpact[t] = (mEnvGreyReplacement * aExpSizeMax + mEnvGreyReplacementConstant) / (LifeTime() * 8760.);
        }
    }
}

void EnvImpact::computeEnvImpactContribution(const int j, const MIPModeler::MIPExpression1D* aFlux)
{
    for (uint64_t t = 0; t < Horizon(); ++t) {
        if (mUseTSEnvContentCoeff[j]) {
            mExpOpEnvImpact[t] += (mTSEnvContentCoeff[j][t] * aFlux->at(t) + mEnvContentOffsets[j]) * TimeSteps().at(t);
            mExpFlowEnvImpact[t] += (mTSEnvContentCoeff[j][t] * aFlux->at(t) + mEnvContentOffsets[j]);
        }
        else {
            mExpOpEnvImpact[t] += (mEnvContentCoefficients[j] * aFlux->at(t) + mEnvContentOffsets[j]) * TimeSteps().at(t);
            mExpFlowEnvImpact[t] += (mEnvContentCoefficients[j] * aFlux->at(t) + mEnvContentOffsets[j]);
        }
    }
}

void EnvImpact::computeEnvImpactContributionCost()
{
    mExpEmbodiedEnvImpactCost += mEnvImpactCost * mExpEmbodiedEnvImpact;
    
    for (uint64_t t = 0; t < Horizon(); ++t) {
        mExpOpEnvImpactCost[t] += mEnvImpactCost * mExpOpEnvImpact[t];
    }
}

void EnvImpact::evaluateEnvGreyImpact(const double* optSol)
{
    mEmbodiedEnvImpact.at(0) = mEmbodiedEnvImpact.at(1) = mExpEmbodiedEnvImpact.evaluate(optSol);
    mEmbodiedEnvImpactCost.at(0) = mEmbodiedEnvImpactCost.at(1) = mExpEmbodiedEnvImpactCost.evaluate(optSol);
}