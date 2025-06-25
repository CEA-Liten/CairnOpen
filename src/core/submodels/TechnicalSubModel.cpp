#include "TechnicalSubModel.h"


TechnicalSubModel::TechnicalSubModel(QObject* aParent) :
SubModel(aParent),
mHistPureOpexContributionDiscounted(0.),
mHistReplacementPartDiscounted(0.),
mOptimalSize(2, 0.),
mTotalCostFunction(2, 0.),
mCapexContribution(2, 0.),
mExistence(2,0.),
mOpexContribution(2, 0.),
mPureOpexContribution(2, 0.),
mReplacementPart(2, 0.),
mEnvImpactPart(2, 0.),
mEnvGreyImpactCost(2, 0.),
mSumUp(2, 0.),
mRunningTime(2, 0.),
mMaxRunningTime(2, 0.),
mChargingTime(2, 0.),
mDischargingTime(2, 0),
mRunningTimeAvlblt(2, 0.),
mEfficiency_Ageing(2, 0.),
mAreaContribution(2, 0.),
mVolumeContribution(2, 0.),
mMassContribution(2, 0.),
mVariableCosts(2, 0.)
{
}


TechnicalSubModel::~TechnicalSubModel()
{
}

void TechnicalSubModel::addMinimumCapacity(double& aMaxSize)
{
    if (mAllocate)
    {
        mInvest = MIPModeler::MIPVariable0D(0, 1, MIPModeler::MIP_INT);
    }
    addVariable(mInvest, "Invest");
    addConstraint(mVarSizeMax <= mInvest * aMaxSize, "maximumCapacity");
    addConstraint(mInvest * mMinSize <= mExpSizeMax, "minimumCapacity");
}

int TechnicalSubModel::checkConsistency()
{
    // note that this function is not used for SourceLoad where the only dimensionning variable is the weight
    if (mPiecewiseCapex && mUseWeightOptimization)
    {
        qCritical() << "ERROR : options PiecewiseCapex and UseWeightOptimization cannot be used together  " << mPiecewiseCapex << mUseWeightOptimization;
        return -1;
    }

    for (int i = 0; i < mEnvImpacts.size(); i++)
    {
        if (mEnvImpacts[i]->PiecewiseEnvGreyContentCoeff() && mUseWeightOptimization) {
            qCritical() << "ERROR : options PiecewiseEnvGreyContentCoeff and UseWeightOptimization cannot be used together  " << mEnvImpacts[i]->PiecewiseEnvGreyContentCoeff() << mUseWeightOptimization;
            return -1;
        }
    }

    return 0;
}

void TechnicalSubModel::computeAllContribution()
{
    /** Compute possible geometric expressions and add corresponding constraints, if GeometryModel=true */
    computeGeometricContribution();
    /** Compute possible environment expressions and add corresponding constraints, if EnvironmentModel=true */
    computeEnvContribution();
    /** Compute economical expressions and add corresponding constraints, if EncoInvestModel=true */
    computeEconomicalContribution();
    //Next Opex (PureOpex + Replacement + VarCost + EnvImpactCost) should be computet at the end because it is a sum of other expressions
    computeNetOpexContribution();
}

void TechnicalSubModel::computeGeometricContribution() {
    if (mGeometryModel) {
        closeExpression(mExpArea);
        closeExpression(mExpVolume);
        closeExpression(mExpMass);
    }

    if (mGeometryModel) {
        if (mPiecewiseArea) {
            qInfo() << "Add Piecewise Area. Try Relaxation : " << mTryRelaxationArea;
            computePiecewiseContribution(mAreaCapacitySetPoint, mAreaSetPoint, mTryRelaxationArea, 0, mExpArea);
        }
        else
        {
            mExpArea = mArea * mExpSizeMax;
        }

        if (mPiecewiseVolume) {
            qInfo() << "Add Piecewise Volume. Try Relaxation : " << mTryRelaxationVolume;
            computePiecewiseContribution(mVolumeCapacitySetPoint, mVolumeSetPoint, mTryRelaxationVolume, 0, mExpVolume);
        }
        else
        {
            mExpVolume = mVolume * mExpSizeMax;
        }

        if (mPiecewiseMass) {
            qInfo() << "Add Piecewise Mass. Try Relaxation : " << mTryRelaxationMass;
            computePiecewiseContribution(mMassCapacitySetPoint, mMassSetPoint, mTryRelaxationMass, 0, mExpMass);
        }
        else
        {
            mExpMass = mMass * mExpSizeMax;
        }
    }
}

void TechnicalSubModel::computeEnvContribution()
{
    if (mEnvironmentModel)
    {
        if (mAllocate)
        {
            for (EnvImpact* impact : mEnvImpacts) impact->allocateExpressions(mTimeSteps.size());
        }
        else
        {
            for (EnvImpact* impact : mEnvImpacts) impact->closeExpressions();
        }
        //Environmental impacts
        //Direct emissions
        for (EnvImpact* impact : mEnvImpacts) {
            impact->setCapex(mCapex);
            impact->setLifeTime(mLifeTime);
            impact->setOptimalSizeUnit(getOptimalSizeUnit());
            MilpPort* port;
            QListIterator<MilpPort*> iport(mListPort);
            int j = 0;
            while (iport.hasNext())
            {
                port = iport.next();
                if (port->VarType() == "vector")
                {
                    if (port->Variable() != "") {
                        impact->computeEnvImpactContribution(j, getMIPExpression1D(port->Variable()));
                    }
                }
                j++;
            }
        }
        //Grey emissions
        for (EnvImpact* impact : mEnvImpacts) {
            if (impact->PiecewiseEnvGreyContentCoeff()) {
                qInfo() << "Add Piecewise EnvGreyContentCoeff. Try Relaxation : " << impact->TryRelaxationEnvGreyContentCoeff();
                computePiecewiseContribution(impact->CapacitySetPoint(), impact->SetPoint(), impact->TryRelaxationEnvGreyContentCoeff(),
                    impact->EnvGreyContentOffset(), *(impact->getExpEnvGreyMass()));
            }
            else {
                impact->computeEnvGreyImpactContribution(mExpSizeMax);
            }
            impact->computeEnvGreyImpactReplacement(mExpSizeMax);
        }
    }
    //
    if (mEnvironmentModel) {
        //Environmental impacts
        for (EnvImpact* impact : mEnvImpacts)
        {
            impact->computeEnvImpactContributionCost();
        }
    }
}

void TechnicalSubModel::computePiecewiseContribution(const MIPModeler::MIPData1D& aCapacitySetPoint, const MIPModeler::MIPData1D& aCostSetPoint,
    const bool& aTryRelaxation, const double& aOffset, MIPModeler::MIPExpression& aExp) 
{
    if (aCapacitySetPoint.size() == 0 || aCostSetPoint.size() == 0) {
        Cairn_Exception cairn_error(parent()->objectName() + ": setpoint is void ! (CapacitySetPoint or CostSetPoint in DataFile)", -1);
        throw cairn_error;
    }

    bool aRelaxedFormSOE = false;
    if (aTryRelaxation) {
        qInfo() << "Try convex ...";
        aRelaxedFormSOE = MIPModeler::isConvexSet(aCapacitySetPoint, aCostSetPoint);
    }
    if (aRelaxedFormSOE) {
        qInfo() << "Function is convex, linearization is continuous";
    }

    //compute ContentCoefficient
    aExp += MIPModeler::MIPPiecewiseLinearisation(*mModel, mVarSizeMax, aCapacitySetPoint, aCostSetPoint, "SizeMax",
        MIPModeler::MIP_SOS, aRelaxedFormSOE);

    //add Offset
    aExp += aOffset;
}


void TechnicalSubModel::computeEconomicalContribution()
{
    if (mAllocate)
    {
        if (mEcoInvestModel)
        {
            mExpOpex = MIPModeler::MIPExpression1D(mTimeSteps.size());
            mExpPureOpex = MIPModeler::MIPExpression1D(mTimeSteps.size());
            mExpReplacement = MIPModeler::MIPExpression1D(mTimeSteps.size());
        }
        mExpVariableCosts = MIPModeler::MIPExpression1D(mTimeSteps.size());
    }
    else
    {
        if (mEcoInvestModel)
        {
            closeExpression(mExpCapex);
            closeExpression1D(mExpOpex);
            closeExpression1D(mExpPureOpex);
            closeExpression1D(mExpReplacement);
        }
        closeExpression1D(mExpVariableCosts);
    }

    if (mEcoInvestModel)
    {
        if (mPiecewiseCapex) {
            qInfo() << "Add Piecewise Capex. Try Relaxation : " << mTryRelaxationCapex;
            computePiecewiseContribution(mCapexCapacitySetPoint, mCapexSetPoint, mTryRelaxationCapex, 0, mExpCapex);
        }
        else
        {
            if (mCapex < 1.e-10) // pourquoi cette distinction ??
                mExpCapex = mTotalCapexOffset * mExpInstalled;
            else
                mExpCapex += mTotalCapexCoefficient * mCapex * mExpSizeMax + mTotalCapexOffset*mExpInstalled;
        }
        if (mCapex < 1.e-10)
        {
            for (uint64_t t = 0; t < mTimeSteps.size(); ++t)
            {
                mExpPureOpex[t] = 0.;
            }

            for (uint64_t t = 0; t < mTimeSteps.size(); ++t)
            {
                mExpReplacement[t] = 0.;
            }
        }
        else
        {
            for (uint64_t t = 0; t < mTimeSteps.size(); ++t)
            {
                mExpPureOpex[t] += TimeStep(t) * (mCapex * mOpex + mOpexConstant) * mExpSizeMax / 8760.;
            }

            if (fabs(mLifeTime) > 1.e-6) {
                for (uint64_t t = 0; t < mTimeSteps.size(); ++t)
                {
                    mExpReplacement[t] += TimeStep(t) * (mCapex * mReplacement + mReplacementConstant) * mExpSizeMax / (mLifeTime * 8760.);
                }
            }
            else {
                Cairn_Exception cairn_error("An error occurred while computing the replacement cost of " + getCompoName() + ". The value of the parameter LifeTime cannot be 0.", -1);
                throw cairn_error;
            }
        }
    }
}

void TechnicalSubModel::computeNetOpexContribution() {
    //Next Opex should be computet at the end because it is a sum of other expressions
    //NetOpex = PureOpex + Replacement + VarCost + EnvImpactCost 
    if (mEcoInvestModel) {
        for (uint64_t t = 0; t < mTimeSteps.size(); ++t) {
            mExpOpex[t] = mExpPureOpex[t] + mExpReplacement[t] + mExpVariableCosts[t];
            if (mEnvironmentModel) {
                for (int i = 0; i < mEnvImpacts.size(); i++) {
                    mExpOpex[t] += mEnvImpacts[i]->getExpEnvCost()->at(t);
                }
            }
        }
    }
}

void TechnicalSubModel::computeDefaultIndicators(const double* optSol)
{
    mOptimalSize.at(0) = 0.;
    mTotalCostFunction.at(0) = 0.;
    mCapexContribution.at(0) = 0.;
    mOpexContribution.at(0) = 0.;
    mPureOpexContribution.at(0) = 0.;
    mVariableCosts.at(0) = 0.;
    mReplacementPart.at(0) = 0.;
    mEnvImpactPart.at(0) = 0.;
    mEnvGreyImpactCost.at(0) = 0.;

    mOptimalSize.at(0) = mExpSizeMax.evaluate(optSol);
    double sauv = mOptimalSize.at(1);
    mOptimalSize.at(1) = max(sauv, mOptimalSize.at(0));

    if (mEcoInvestModel)
    {
        mCapexContribution.at(0) = mCapexContribution.at(1) = mExpCapex.evaluate(optSol);
        for (uint64_t t = 0; t < mHorizon; ++t) mPureOpexContribution.at(0) += mExpPureOpex.at(t).evaluate(optSol) * mParentCompo->ExtrapolationFactor(); // mise à jour du PLAN
        for (uint64_t t = 0; t < *mptrTimeshift; ++t) mPureOpexContribution.at(1) += mExpPureOpex.at(t).evaluate(optSol); // mise à jour du HIST
        for (uint64_t t = 0; t < mHorizon; ++t) mReplacementPart.at(0) += mExpReplacement.at(t).evaluate(optSol) * mParentCompo->ExtrapolationFactor(); // PLAN
        for (uint64_t t = 0; t < *mptrTimeshift; ++t) mReplacementPart.at(1) += mExpReplacement.at(t).evaluate(optSol); // HIST
    }
    mExistence.at(0) = mExistence.at(1) = mExpInstalled.evaluate(optSol);
    for (uint64_t t = 0; t < mHorizon; ++t) mVariableCosts.at(0) += mExpVariableCosts.at(t).evaluate(optSol) * mParentCompo->ExtrapolationFactor(); // PLAN
    for (uint64_t t = 0; t < *mptrTimeshift; ++t) mVariableCosts.at(1) += mExpVariableCosts.at(t).evaluate(optSol); // HIST

    if (mGeometryModel) {
        mAreaContribution.at(0) = mAreaContribution.at(1) = mExpArea.evaluate(optSol);
        mVolumeContribution.at(0) = mVolumeContribution.at(1) = mExpVolume.evaluate(optSol);
        mMassContribution.at(0) = mMassContribution.at(1) = mExpMass.evaluate(optSol);
    }

    if (mEnvironmentModel)
    {
        for (EnvImpact* impact : mEnvImpacts)
        {
            computeProduction(true, mHorizon, mNpdtPast, *impact->getExpEnvCost(), optSol, 1., 0., *impact->getEnvImpactPartPLAN());
            computeProduction(false, *mptrTimeshift, mNpdtPast, *impact->getExpEnvCost(), optSol, 1., 0., *impact->getEnvImpactPartHIST());
            mEnvImpactPart.at(0) += *impact->getEnvImpactPartPLAN();
            mEnvImpactPart.at(1) += *impact->getEnvImpactPartHIST();
            //Use getExpEnvFlow instead of getExpEnvMass to compute getEnvImpactMassPLAN and getEnvImpactMassHIST because computeProduction multiplies by TimeStep
            computeProduction(true, mHorizon, mNpdtPast, *impact->getExpEnvFlow(), optSol, 1., 0., *impact->getEnvImpactMassPLAN());
            computeProduction(false, *mptrTimeshift, mNpdtPast, *impact->getExpEnvFlow(), optSol, 1., 0., *impact->getEnvImpactMassHIST());
            //Grey impact
            impact->evaluateEnvGreyImpact(optSol);
            computeProduction(true, mHorizon, mNpdtPast, *impact->getExpEnvReplacement(), optSol, 1., 0., *impact->getEnvImpactReplacementPLAN());
            computeProduction(false, *mptrTimeshift, mNpdtPast, *impact->getExpEnvReplacement(), optSol, 1., 0., *impact->getEnvImpactReplacementHIST());
        }
    }

    mOpexContribution.at(0) = mPureOpexContribution.at(0) + mReplacementPart.at(0) + mEnvImpactPart.at(0) + mVariableCosts.at(0);
    mOpexContribution.at(1) = mPureOpexContribution.at(1) + mReplacementPart.at(1) + mEnvImpactPart.at(1) + mVariableCosts.at(1);
    double pureOpexContributionDiscounted = 0.;
    double replacementPartDiscounted = 0.;
    double envEmissionPartDiscounted = 0.;
    double envImpactPartDiscounted = 0.;
    double envHistImpactPartDiscounted = 0.;
    double variableCostsDiscounted = 0.;

    if (mEcoInvestModel) {
        computeDiscounted(mHorizon, mNpdtPast, mExpPureOpex, optSol, pureOpexContributionDiscounted);
        computeDiscounted(mHorizon, mNpdtPast, mExpReplacement, optSol, replacementPartDiscounted);
    }

    if (mEnvironmentModel) for (EnvImpact* impact : mEnvImpacts) {
        //Use getExpEnvFlow instead of getExpEnvMass to compute getEnvImpactMassDiscountedPLAN because computeLvlImpact multiplies by TimeStep
        computeLvlImpact(true, mHorizon, mNpdtPast, *impact->getExpEnvCost(), optSol, 1., 0., *impact->getEnvImpactPartDiscountedPLAN());
        computeLvlImpact(true, mHorizon, mNpdtPast, *impact->getExpEnvFlow(), optSol, 1., 0., *impact->getEnvImpactMassDiscountedPLAN());
        envImpactPartDiscounted += *impact->getEnvImpactPartDiscountedPLAN();
    }
    computeDiscounted(mHorizon, mNpdtPast, mExpVariableCosts, optSol, variableCostsDiscounted);
    mTotalCostFunction.at(0) = mCapexContribution.at(0) + pureOpexContributionDiscounted + replacementPartDiscounted
        + envEmissionPartDiscounted + envImpactPartDiscounted + variableCostsDiscounted; 

    if (mEcoInvestModel) {
        computeDiscounted(*mptrTimeshift, mNpdtPast, mExpPureOpex, optSol, mHistPureOpexContributionDiscounted);
        computeDiscounted(*mptrTimeshift, mNpdtPast, mExpReplacement, optSol, mHistReplacementPartDiscounted);
    }

    if (mEnvironmentModel) for (EnvImpact* impact : mEnvImpacts) {
        //Use getExpEnvFlow instead of getExpEnvMass to compute getEnvImpactMassDiscountedHIST because computeLvlImpact multiplies by TimeStep
        computeLvlImpact(false, *mptrTimeshift, mNpdtPast, *impact->getExpEnvCost(), optSol, 1., 0., *impact->getEnvImpactPartDiscountedHIST());
        computeLvlImpact(false, *mptrTimeshift, mNpdtPast, *impact->getExpEnvFlow(), optSol, 1., 0., *impact->getEnvImpactMassDiscountedHIST());
        envHistImpactPartDiscounted += *impact->getEnvImpactPartDiscountedHIST();
    }
    computeDiscounted(*mptrTimeshift, mNpdtPast, mExpVariableCosts, optSol, mHistVariableCostsDiscounted);
    mTotalCostFunction.at(1) = mCapexContribution.at(1) + (mHistPureOpexContributionDiscounted + mHistReplacementPartDiscounted + envHistImpactPartDiscounted + mHistVariableCostsDiscounted) / mParentCompo->ExtrapolationFactor();
}

