#include "TechnicalSubModel.h"

TechnicalSubModel::TechnicalSubModel(CairnObject* aParent) :
SubModel(aParent),
mHistFixedOpexContributionDiscounted(0.),
mHistReplacementPartDiscounted(0.),
mOptimalSize(2, 0.),
mWeightResult(2, 0.),
mTotalCostFunction(2, 0.),
mCapexContribution(2, 0.),
mExistence(2,0.),
mOpexContribution(2, 0.),
mFixedOpexContribution(2, 0.),
mVariableOpexContribution(2, 0.),
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

void TechnicalSubModel::removeImpactSettings(const std::string& impactName)
{
    /* remove all related parameters, IOs, and indicators */

    mInputEnvImpacts->removeImpactParameters(impactName);
    mInputPortImpacts->removeImpactParameters(impactName);
    mTSInputPortImpacts->removeImpactParameters(impactName);
    mInputPerfParam->removeImpactParameters(impactName);

    removeEnvImpactIOs(impactName);

    mInputIndicators->removeImpactIndicators(impactName);
}

void TechnicalSubModel::cleanNonSelectedEnvImpacts() {
    /*
    * This method might be dynamically called while using the api for example when a port is add/removed.
    * mEnvImpactsList : is a list of the selected Env Impact names
    * mEnvImpacts : is a list of existing Env Impacts (some impacts might be got unselected and new impacts might be got selected)
    */

    for (auto it = mEnvImpacts.begin(); it != mEnvImpacts.end(); ) {
        EnvImpact* impact = *it;
        bool selected = std::any_of(mEnvImpactsList.begin(), mEnvImpactsList.end(),
            [&](std::string impactName) {
                return (impact->Name() == impactName);
            });

        if (!selected) {
            removeImpactExpressionName(impact->Name());   // Remove related expressions from Exp Name Lists
            removeImpactSettings(impact->Name()); // Remove parameters, IOs and indicators of the impacts that are no longer selected
            delete* it;                     // Delete impact
            it = mEnvImpacts.erase(it);    // Remove impcat entry from mEnvImpacts and get next iterator
        }
        else {
            impact->markAsOld();
            ++it;
        }
    }
}

void TechnicalSubModel::createEnvImpacts() {
    // Create missing Env Impacts
    for (size_t i = 0; i < mEnvImpactsList.size(); i++)
    {
        bool exist = std::any_of(mEnvImpacts.begin(), mEnvImpacts.end(),
            [&](EnvImpact* impact) {
                return impact->Name() == mEnvImpactsList[i];
            });

        if (exist) continue; // Already exists

        // Create new EnvImpact 
        auto newImpact = new EnvImpact(this, mEnvImpactsList[i], mEnvImpactUnitsList[i], mEnvImpactsShortNamesList[i]);
        newImpact->setEnvImpactCost(mEnvImpactCosts[i]);
        mEnvImpacts.push_back(newImpact);
    }
}


void TechnicalSubModel::setTimeData()
{
    SubModel::setTimeData();
    mComponentAvailabilityTS.clear();
    mComponentAvailabilityTS.resize(mHorizon);
}

void TechnicalSubModel::resetHistStoredValues()
{
    mHistFixedOpexContributionDiscounted = 0.;
    mHistReplacementPartDiscounted = 0.;
    mHistVariableCostsDiscounted = 0.;

    mMaxRunningTime = std::vector<double>(2, 0.);

    mExpEchData = {};
    mConsumptionMap = {};
    mConsLvlTotMap = {};
    mConsPFMap = {};
    mRateOfUse = {};
    mConsMeanMap = {};
    mProductionMap = {};
    mProdLvlTotMap = {};
    mProdMeanMap = {};
    mProdContributionMap = {};
    mChargedEnergyMap = {};
    mDischargedEnergyMap = {};
    mNLevChargedEnergyMap = {};
    mNLevDischargedEnergyMap = {};
    mChargedMeanMap = {};
    mDischargedMeanMap = {};
    mNbCylesMap = {};
}

void TechnicalSubModel::setExpInstalled()
{
    /*
    * Optimize size on the basis of weighting factor multiplying constant production or storage capacity
    * Variable to multiply by the capex offset to model the "construction" work
    */
    if (mLPModelOnly) {
        addVariable(mVarInstalled, "Installed", 0.f, 1, MIPModeler::MIP_FLOAT);
    }
    else {
        addVariable(mVarInstalled, "Installed", 0.f, 1, MIPModeler::MIP_INT);
    }

    if (mWeight<0)
    {
        mExpInstalled = mVarInstalled;
    }
    else
    {
        mExpInstalled = mVarInstalled;
        if (mMaxValue >= 10E-10) {
            addConstraint(mVarInstalled == 1, "sInstalled");
        }
    }
}

void TechnicalSubModel::buildModel()
{
    if (mAllocate) {
        allocateExpressions();
    }
    else {
        closeExpressions();
    }

    /* set SizeMax expression and add constraints on SizeMax and Installed variables */
    setExpInstalled();
    setExpSizeMax(mExpInstalled);

    /* add State and StartUpShutDown constraints : add On/Off variable */
    if (mAddStateVariable) {
        addStateConstraints(varMilpHorizon(), mExpInstalled);
    }

    if (mAddStartUpShutDownVariable) {
        addStartUpShutDown(varMilpHorizon(), mExpInstalled);
    }
    
    /** compute expressions, add variables and add constraints */
    computeAllContribution();

    mAllocate = false;
}

void TechnicalSubModel::addMinimumCapacity(double& aMaxSize)
{
    addVariable(mInvest, "Invest", 0, 1, MIPModeler::MIP_INT);
    addConstraint(mVarSizeMax <= mInvest * aMaxSize, "maximumCapacity");
    addConstraint(mInvest * mMinSize <= mExpSizeMax, "minimumCapacity");
}

int TechnicalSubModel::checkConsistency()
{
    int ierr = SubModel::checkConsistency();

    // note that this function is not used for SourceLoad where the only dimensionning variable is the weight
    if (mPiecewiseCapex && mWeight<0)
    {
        cCritical() << "ERROR : options PiecewiseCapex and Weight<0 cannot be used together  " << mPiecewiseCapex << (mWeight<0);
        return -1;
    }

    for (int i = 0; i < mEnvImpacts.size(); i++)
    {
        if (mEnvImpacts[i]->PiecewiseEnvGreyContentCoeff() && (mWeight<0)) {
            cCritical() << "ERROR : options PiecewiseEnvGreyContentCoeff and (mWeight<0) cannot be used together  " << mEnvImpacts[i]->PiecewiseEnvGreyContentCoeff() << (mWeight < 0);
            return -1;
        }
    }

    if (mMaxValue < 0. && mWeight < 0.)
    {
        cCritical() << "ERROR : size of one component and weight cannot be optimized together maxvalue : " << mMaxValue << ", Weight : " << (mWeight);
        return -1;
    }

    return ierr;
}

void TechnicalSubModel::computeAllContribution()
{
    /* Compute Ageing Contribution */
    computeAgeingModelContribution();

    /* Compute Model particular Contribution */
    computeModelContribution();

    /* Compute possible geometric expressions and add corresponding constraints, if GeometryModel=true */
    computeGeometricContribution();
    
    /* Compute possible environment expressions and add corresponding constraints, if EnvironmentModel=true */
    computeEnvContribution();
    
    /* Compute economical expressions and add corresponding constraints, if EncoInvestModel=true */
    computeEconomicalContribution();

    /* Next Opex should be computet at the end because it is a sum of other expressions */
    computeNetOpexContribution();
}

void TechnicalSubModel::computeGeometricContribution() 
{
    if (mGeometryModel) {
        if (mPiecewiseArea) {
            cInfo() << "Add Piecewise Area. Try Relaxation : " << mTryRelaxationArea;
            computePiecewiseContribution(mAreaCapacitySetPoint, mAreaSetPoint, mTryRelaxationArea, 0, mExpArea);
        }
        else
        {
            mExpArea = mArea * mExpSizeMax;
        }

        if (mPiecewiseVolume) {
            cInfo() << "Add Piecewise Volume. Try Relaxation : " << mTryRelaxationVolume;
            computePiecewiseContribution(mVolumeCapacitySetPoint, mVolumeSetPoint, mTryRelaxationVolume, 0, mExpVolume);
        }
        else
        {
            mExpVolume = mVolume * mExpSizeMax;
        }

        if (mPiecewiseMass) {
            cInfo() << "Add Piecewise Mass. Try Relaxation : " << mTryRelaxationMass;
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

        //Environmental impacts
        //Direct emissions
        for (EnvImpact* impact : mEnvImpacts) {
            size_t j = 0;
            for (auto& port : mListPort) {            
                const MIPModeler::MIPExpression1D* ptrExp1D = getMIPExpression1D(port->Variable());
                if (ptrExp1D) {
                    impact->computeEnvImpactContribution(j, ptrExp1D, mExpInstalled);
                }
                j++;
            }
        }
        //Embodied emissions
        for (EnvImpact* impact : mEnvImpacts) {
            if (impact->PiecewiseEnvGreyContentCoeff()) {
                cInfo() << "Add Piecewise EnvGreyContentCoeff. Try Relaxation : " << impact->TryRelaxationEnvGreyContentCoeff();
                computePiecewiseContribution(impact->CapacitySetPoint(), impact->SetPoint(), impact->TryRelaxationEnvGreyContentCoeff(),
                    impact->EnvGreyContentOffset(), *(impact->getExpEnvEmbodied()));
            }
            else {
                impact->computeEmbodiedEnvImpactContribution(mExpSizeMax, mExpInstalled);
            }
            impact->computeReplacementEnvImpactContribution(mExpSizeMax, mExpInstalled);
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
        cInfo() << "Try convex ...";
        aRelaxedFormSOE = MIPModeler::isConvexSet(aCapacitySetPoint, aCostSetPoint);
    }
    if (aRelaxedFormSOE) {
        cInfo() << "Function is convex, linearization is continuous";
    }

    //compute ContentCoefficient
    aExp += MIPModeler::MIPPiecewiseLinearisation(*mModel, mVarSizeMax, aCapacitySetPoint, aCostSetPoint, "SizeMax",
        MIPModeler::MIP_SOS, aRelaxedFormSOE);

    //add Offset
    aExp += aOffset;
}

void TechnicalSubModel::computeEconomicalContribution()
{
    // -----------------------------------------
    // Allocation: initialize expressions
    // -----------------------------------------
    if (mAllocate)
    {
        const std::size_t n = mTimeSteps.size();

        if (mEcoInvestModel)
        {
            mExpOpex = MIPModeler::MIPExpression1D(n);
            mExpFixedOpex = MIPModeler::MIPExpression1D(n);
            mExpReplacement = MIPModeler::MIPExpression1D(n);
        }

        mExpVariableOpex = MIPModeler::MIPExpression1D(n);
        mExpVariableCosts = MIPModeler::MIPExpression1D(n);
    }

    // -----------------------------------------
    // Validation & Configuration
    // -----------------------------------------
    if (!mEcoInvestModel)
        return;

    constexpr double EPSILON = 1e-6;
    constexpr double HOURS_PER_YEAR = 8760.0;

    // Validate LifeTime
    if (std::fabs(mLifeTime) < EPSILON) {
        throw Cairn_Exception(
            "An error occurred while computing the replacement cost of " + Name() +
            ". The value of the parameter LifeTime cannot be 0.",
            -1
        );
    }

    // Normalize near-zero Capex
    if (std::fabs(mCapex) < EPSILON)
        mCapex = 0.0;

    // -----------------------------------------
    // Capex contribution
    // -----------------------------------------
    if (mPiecewiseCapex)
    {
        cInfo() << "Add Piecewise Capex. Try Relaxation: " << mTryRelaxationCapex;
        computePiecewiseContribution(
            mCapexCapacitySetPoint,
            mCapexSetPoint,
            mTryRelaxationCapex,
            0,
            mExpCapex
        );
    }
    else
    {
        mExpCapex = mTotalCapexCoefficient * mCapex * mExpSizeMax +
            mTotalCapexOffset * mExpInstalled;
    }

    // -----------------------------------------
    // Fixed Opex and Replacement contributions
    // -----------------------------------------
    const std::size_t T = mTimeSteps.size();

    for (std::size_t t = 0; t < T; ++t)
    {
        const double dt = TimeStep(t);

        // Fixed Opex
        mExpFixedOpex[t] += dt * (mCapex * mFixedOpex * mExpSizeMax +
            mFixedOpexConstant * mExpInstalled) / HOURS_PER_YEAR; 

        // Replacement
        mExpReplacement[t] += dt * (mCapex * mReplacement * mExpSizeMax +
            mReplacementConstant * mExpInstalled) / (mLifeTime * HOURS_PER_YEAR);
    }

    // -----------------------------------------
    // Variable Opex contribution
    // -----------------------------------------
    computeVariableOpexContribution();
}

void TechnicalSubModel::computeVariableOpexContribution()
{
    const std::size_t T = mTimeSteps.size();

    for (MilpPort* port : mListPort)
    {
        if (!port) {
            throw Cairn_Exception("Null port encountered while computing variable Opex in " + Name(), -1);
        }

        const double  opex = port->VariableOpex();
        const std::string variable = port->Variable();

        MIPModeler::MIPExpression*   exp0D = getMIPExpression(variable);
        MIPModeler::MIPExpression1D* exp1D = getMIPExpression1D(variable);

        if (exp0D) {
            for (std::size_t t = 0; t < T; ++t) {
                const double dt = TimeStep(t);
                mExpVariableOpex[t] += dt * opex * (*exp0D);
            }
        }
        else if (exp1D) {
            for (std::size_t t = 0; t < T; ++t) {
                const double dt = TimeStep(t);
                mExpVariableOpex[t] += dt * opex * (*exp1D)[t];
            }
        }
        else {
            throw Cairn_Exception("Missing expression for port '" + variable +
                "' in component '" + Name() + "'", -1);
        }
    }
}

void TechnicalSubModel::computeNetOpexContribution() 
{
    //Next Opex should be computet at the end because it is a sum of other expressions
    if (mEcoInvestModel) {
        for (uint64_t t = 0; t < mTimeSteps.size(); ++t) {
            mExpOpex[t] = mExpFixedOpex[t] + mExpVariableOpex[t] + mExpReplacement[t] + mExpVariableCosts[t];
            if (mEnvironmentModel) {
                for (int i = 0; i < mEnvImpacts.size(); i++) {
                    mExpOpex[t] += mEnvImpacts[i]->getExpEnvOpCost()->at(t);
                }
            }
        }
    }
}

void TechnicalSubModel::computeDefaultIndicators(const double* optSol)
{
    mOptimalSize.at(0) = 0.;
    mWeightResult.at(0) = 1;
    mWeightResult.at(1) = 1;
    mTotalCostFunction.at(0) = 0.;
    mCapexContribution.at(0) = 0.;
    mOpexContribution.at(0) = 0.;
    mFixedOpexContribution.at(0) = 0.;
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
        for (uint64_t t = 0; t < mHorizon; ++t) mFixedOpexContribution.at(0) += mExpFixedOpex.at(t).evaluate(optSol) * mParentCompo->ExtrapolationFactor(); // mise à jour du PLAN
        for (uint64_t t = 0; t < *mptrTimeshift; ++t) mFixedOpexContribution.at(1) += mExpFixedOpex.at(t).evaluate(optSol); // mise à jour du HIST
        for (uint64_t t = 0; t < mHorizon; ++t) mVariableOpexContribution.at(0) += mExpVariableOpex.at(t).evaluate(optSol) * mParentCompo->ExtrapolationFactor(); // mise à jour du PLAN
        for (uint64_t t = 0; t < *mptrTimeshift; ++t) mVariableOpexContribution.at(1) += mExpVariableOpex.at(t).evaluate(optSol); // mise à jour du HIST
        for (uint64_t t = 0; t < mHorizon; ++t) mReplacementPart.at(0) += mExpReplacement.at(t).evaluate(optSol) * mParentCompo->ExtrapolationFactor(); // PLAN
        for (uint64_t t = 0; t < *mptrTimeshift; ++t) mReplacementPart.at(1) += mExpReplacement.at(t).evaluate(optSol); // HIST
    }
    mExistence.at(0) = mExistence.at(1) = mExpInstalled.evaluate(optSol);
    if (mOptimalSize.at(0) == 0.) {
        mExistence.at(0) = mExistence.at(1) = 0.;
    }
    if (mWeight > 1 || mWeight < 0) {
        mWeightResult.at(0) = mOptimalSize.at(0) / mMaxValue;
        mWeightResult.at(1) = mOptimalSize.at(1) / mMaxValue;
    }
    double xxx = mExpSizeMax.evaluate(optSol);
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
            computeProduction(true, mHorizon, mNpdtPast, *impact->getExpEnvOpCost(), optSol, 1., 0., *impact->getEnvImpactPartPLAN());
            computeProduction(false, *mptrTimeshift, mNpdtPast, *impact->getExpEnvOpCost(), optSol, 1., 0., *impact->getEnvImpactPartHIST());
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

    mOpexContribution.at(0) = mFixedOpexContribution.at(0) + mVariableOpexContribution.at(0) + mReplacementPart.at(0) + mEnvImpactPart.at(0) + mVariableCosts.at(0);
    mOpexContribution.at(1) = mFixedOpexContribution.at(1) + mVariableOpexContribution.at(1) + mReplacementPart.at(1) + mEnvImpactPart.at(1) + mVariableCosts.at(1);
    double pureOpexContributionDiscounted = 0.;
    double replacementPartDiscounted = 0.;
    double envEmissionPartDiscounted = 0.;
    double envImpactPartDiscounted = 0.;
    double envHistImpactPartDiscounted = 0.;
    double variableCostsDiscounted = 0.;

    if (mEcoInvestModel) {
        computeDiscounted(mHorizon, mNpdtPast, mExpFixedOpex, optSol, pureOpexContributionDiscounted);
        computeDiscounted(mHorizon, mNpdtPast, mExpReplacement, optSol, replacementPartDiscounted);
    }

    if (mEnvironmentModel) for (EnvImpact* impact : mEnvImpacts) {
        //Use getExpEnvFlow instead of getExpEnvMass to compute getEnvImpactMassDiscountedPLAN because computeLvlImpact multiplies by TimeStep
        computeLvlImpact(true, mHorizon, mNpdtPast, *impact->getExpEnvOpCost(), optSol, 1., 0., *impact->getEnvImpactPartDiscountedPLAN());
        computeLvlImpact(true, mHorizon, mNpdtPast, *impact->getExpEnvFlow(), optSol, 1., 0., *impact->getEnvImpactMassDiscountedPLAN());
        envImpactPartDiscounted += *impact->getEnvImpactPartDiscountedPLAN();
    }
    computeDiscounted(mHorizon, mNpdtPast, mExpVariableCosts, optSol, variableCostsDiscounted);
    mTotalCostFunction.at(0) = mCapexContribution.at(0) + pureOpexContributionDiscounted + replacementPartDiscounted
        + envEmissionPartDiscounted + envImpactPartDiscounted + variableCostsDiscounted; 

    if (mEcoInvestModel) {
        computeDiscounted(*mptrTimeshift, mNpdtPast, mExpFixedOpex, optSol, mHistFixedOpexContributionDiscounted);
        computeDiscounted(*mptrTimeshift, mNpdtPast, mExpReplacement, optSol, mHistReplacementPartDiscounted);
    }

    if (mEnvironmentModel) for (EnvImpact* impact : mEnvImpacts) {
        //Use getExpEnvFlow instead of getExpEnvMass to compute getEnvImpactMassDiscountedHIST because computeLvlImpact multiplies by TimeStep
        computeLvlImpact(false, *mptrTimeshift, mNpdtPast, *impact->getExpEnvOpCost(), optSol, 1., 0., *impact->getEnvImpactPartDiscountedHIST());
        computeLvlImpact(false, *mptrTimeshift, mNpdtPast, *impact->getExpEnvFlow(), optSol, 1., 0., *impact->getEnvImpactMassDiscountedHIST());
        envHistImpactPartDiscounted += *impact->getEnvImpactPartDiscountedHIST();
    }
    computeDiscounted(*mptrTimeshift, mNpdtPast, mExpVariableCosts, optSol, mHistVariableCostsDiscounted);
    mTotalCostFunction.at(1) = mCapexContribution.at(1) + (mHistFixedOpexContributionDiscounted + mHistReplacementPartDiscounted + envHistImpactPartDiscounted + mHistVariableCostsDiscounted) / mParentCompo->ExtrapolationFactor();
}

