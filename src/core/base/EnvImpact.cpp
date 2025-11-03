
#include "Cairn_Exception.h"
#include "SubModel.h"

/* TODO: pass SubModel by pointer */
EnvImpact::EnvImpact(CairnObject* aParent, std::string aName, std::string aShortName) :
mName(aName),
mShortName(aShortName),
mCapex(0.),
mEnvImpactCost(0.),
mLifeTime(1.),
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
    if (mShortName == "") mShortName = mName;
}

EnvImpact::~EnvImpact()
{
}

void EnvImpact::computeEmbodiedEnvImpactContribution(MIPModeler::MIPExpression aExpSizeMax)
{
    mExpEmbodiedEnvImpact += mEnvGreyContentCoefficient * aExpSizeMax + mEnvGreyContentOffset;
}

void EnvImpact::computeReplacementEnvImpactContribution(MIPModeler::MIPExpression aExpSizeMax)
{
    for (uint64_t t = 0; t < mTimeSteps.size(); ++t)
    {
        if (mLifeTime>0)
            mExpReplacementEnvImpact[t] = (mEnvGreyReplacement * aExpSizeMax + mEnvGreyReplacementConstant) / (mLifeTime * 8760.);
    }
}

void EnvImpact::computeEnvImpactContribution(const int j, const MIPModeler::MIPExpression1D* aFlux)
{
    for (uint64_t t = 0; t < mTimeSteps.size(); ++t) {
        if (mUseTSEnvContentCoeff[j]) {
            mExpOpEnvImpact[t] += (mTSEnvContentCoeff[j][t] * aFlux->at(t) + mEnvContentOffsets[j]) * mTimeSteps[t];
            mExpFlowEnvImpact[t] += (mTSEnvContentCoeff[j][t] * aFlux->at(t) + mEnvContentOffsets[j]);
        }
        else {
            mExpOpEnvImpact[t] += (mEnvContentCoefficients[j] * aFlux->at(t) + mEnvContentOffsets[j]) * mTimeSteps[t];
            mExpFlowEnvImpact[t] += (mEnvContentCoefficients[j] * aFlux->at(t) + mEnvContentOffsets[j]);
        }
    }
}

void EnvImpact::computeEnvImpactContributionCost()
{
    mExpEmbodiedEnvImpactCost += mEnvImpactCost * mExpEmbodiedEnvImpact;
    
    for (uint64_t t = 0; t < mTimeSteps.size(); ++t) {
        mExpOpEnvImpactCost[t] += mEnvImpactCost * mExpOpEnvImpact[t];
    }
}

void EnvImpact::addIOs(SubModel* aSubModel, t_flag aIsUsed)
{
    aSubModel->
        addIO(mName + " Env impact mass", &mExpOpEnvImpact, aIsUsed, &mImpactUnit); /** "mName Env impact mass" */
    aSubModel->
        addIO(mName + " Env impact flow", &mExpFlowEnvImpact, aIsUsed, SFunctionUnit({ eFTypeDivision, {&mImpactUnit}, "h" })); /** "mName Env impact flow" */
    aSubModel->
        addIO(mName + " Env impact cost", &mExpOpEnvImpactCost, aIsUsed, aSubModel->pCurrency()); /** "mName Env impact cost" */
    aSubModel->
        addIO(mName + " Env grey impact mass", &mExpEmbodiedEnvImpact, aIsUsed, &mImpactUnit); /** "mName Env grey impact mass" */
    aSubModel->
        addIO(mName + " Env grey impact cost", &mExpEmbodiedEnvImpactCost, aIsUsed, aSubModel->pCurrency()); /** "mName Env grey impact cost" */
    aSubModel->
        addIO(mName + " Env impact replacement", &mExpReplacementEnvImpact, aIsUsed, &mImpactUnit);
}
