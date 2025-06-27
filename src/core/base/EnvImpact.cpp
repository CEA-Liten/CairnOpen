
#include <QDebug>
#include "Cairn_Exception.h"
#include "SubModel.h"

EnvImpact::EnvImpact(QObject* aParent, QString aName, QString aShortName) :
mException(Cairn_Exception()),
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
mEnvGreyImpactCost(2,0.),
mEnvGreyImpactMass(2, 0.),
mEnvImpactReplacement(2, 0.)
{  
    if (mShortName == "") mShortName = mName;
}

EnvImpact::EnvImpact(const QString &aName, const QString &aShortName, const QString& aUnit) :
    
mException(Cairn_Exception()),
mName(aName),
mShortName(aShortName),
mImpactUnit(aUnit),
mCapex(0.),
mEnvImpactCost(0.),
mLifeTime(1.),
mEnvImpactPart(2, 0.),
mEnvImpactMass(2, 0.),
mEnvImpactPartDiscounted(2, 0.),
mEnvImpactMassDiscounted(2, 0.),
mEnvGreyImpactCost(2, 0.),
mEnvGreyImpactMass(2, 0.),
mEnvImpactReplacement(2, 0.)
{  
    if (mShortName == "") mShortName = mName;
}

EnvImpact::~EnvImpact()
{
}

void EnvImpact::computeEnvGreyImpactContribution(MIPModeler::MIPExpression aExpSizeMax)
{
    mExpEnvGreyImpactMass += mEnvGreyContentCoefficient * aExpSizeMax + mEnvGreyContentOffset;
}

void EnvImpact::computeEnvGreyImpactReplacement(MIPModeler::MIPExpression aExpSizeMax)
{
    for (uint64_t t = 0; t < mTimeSteps.size(); ++t)
    {
        mExpEnvGreyImpactReplacement[t] = (mEnvGreyReplacement * aExpSizeMax + mEnvGreyReplacementConstant) / (mLifeTime * 8760.);
    }
}

void EnvImpact::computeEnvImpactContribution(const int j, const MIPModeler::MIPExpression1D* aFlux)
{
    for (uint64_t t = 0; t < mTimeSteps.size(); ++t)
    {
        if (mUseTSEnvContentCoeff[j]) {
            
            mExpEnvImpactMass[t] += (mTSEnvContentCoeff[j][t] * aFlux->at(t) + mEnvContentOffsets[j]) * mTimeSteps[t];
            mExpEnvImpactFlow[t] += (mTSEnvContentCoeff[j][t] * aFlux->at(t) + mEnvContentOffsets[j]);
        }
        else {
            mExpEnvImpactMass[t] += (mEnvContentCoefficients[j] * aFlux->at(t) + mEnvContentOffsets[j]) * mTimeSteps[t];
            mExpEnvImpactFlow[t] += (mEnvContentCoefficients[j] * aFlux->at(t) + mEnvContentOffsets[j]);
        }
    }
}

void EnvImpact::computeEnvImpactContributionCost()
{
    mExpEnvGreyImpactCost += mEnvImpactCost * mExpEnvGreyImpactMass;
    for (uint64_t t = 0; t < mTimeSteps.size(); ++t)
    {
        mExpEnvImpactCost[t] += mEnvImpactCost * mExpEnvImpactMass[t];
    }
}

void EnvImpact::addIOs(SubModel* aSubModel)
{
    aSubModel->
        addIO(mName + " Env impact mass", &mExpEnvImpactMass, mImpactUnit); /** "mName Env impact mass" */
    aSubModel->
        addIO(mName + " Env impact flow", &mExpEnvImpactFlow, mImpactUnit + "/h"); /** "mName Env impact flow" */
    aSubModel->
        addIO(mName + " Env impact cost", &mExpEnvImpactCost, aSubModel->Currency()); /** "mName Env impact cost" */
    aSubModel->
        addIO(mName + " Env grey impact mass", &mExpEnvGreyImpactMass, mImpactUnit); /** "mName Env grey impact mass" */
    aSubModel->
        addIO(mName + " Env grey impact cost", &mExpEnvGreyImpactCost, aSubModel->Currency()); /** "mName Env grey impact cost" */
    aSubModel->
        addIO(mName + " Env grey impact replacement part", &mExpEnvGreyImpactReplacement, aSubModel->Currency()); /** "mName Env impact replacement part" */
}
