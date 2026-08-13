/**
* \file		SourceLoad.cpp
* \brief	SourceLoad model
* \version	1.0
* \author	Yacine Gaoua
* \date		08/02/2019
*/

#include "SourceLoad.h"
extern "C" MODELS_DECLSPEC CairnObject * createModel(CairnObject * aParent)
{
    return new SourceLoad(aParent);
}

SourceLoad::SourceLoad(CairnObject* aParent) :
    SourceLoadSubModel(aParent),
    mTemperature_in1(0.),
    mTemperature_out1(0.),
    mHorizonTimeSpanRatio(1)
{
    mPossibleModelClasses = { "SourceLoad", "SourceLoadMinMax" };
}

SourceLoad::~SourceLoad()
{}

void SourceLoad::setTimeData()
{
    SubModel::setTimeData();
    mImposedFlux.resize(mHorizon);
    mStartStopProfile.resize(mHorizon);
    mImposedFluxSeasonal.resize(mHorizon);
    mEnergyPrice.resize(mHorizon);
    mMaxSheddingTS.resize(mHorizon);
    mCostSheddingTS.resize(mHorizon);
}

int SourceLoad::checkConsistency()
{
    //int ier = TechnicalSubModel::checkConsistency();

    int ier = SubModel::checkConsistency();

    if (mWeight < 0 && (mUseControlledFlux == true || mUseWeightedFlux == true))
    {
        cCritical() << " For linearity purpose, optimization of SourceLoad Weight " << mWeight << " requires mUseControlledFlux = false and mUseWeightedFlux = false ! " << mUseControlledFlux << mUseWeightedFlux;
        return -1;
    }
    if (mWeight < 0 && mComputeOptimalPrice)
    {
        cCritical() << " For linearity purpose, optimization of SourceLoad Weight " << mWeight << " requires mComputeOptimalPrice = false ! " << mComputeOptimalPrice;
        return -1;
    }

    if (mAddSheddingDetailed && Sens() > 0.) {
        cCritical() << "Load shedding cannot be used for sources (only loads)";
        return -1;
    }
    if (mAddSheddingDetailed && mLPModelOnly) {
        cCritical() << "Load shedding is not compatible with LPModelOnly option";
        return -1;
    }
    if (mAddSheddingDetailed && mControl != "" && mNpdtPast < mMaxTimeShedding) {
        cCritical() << "Load shedding max activation time (" << mMaxTimeShedding << ") must be less or equal to past size (" << mNpdtPast << ")";
        return -1;
    }
    if (mAddSheddingDetailed && mControl != "" && mNpdtPast < mMinSheddingStandBy) {
        cCritical() << "Load shedding min deactivation time (" << mMinSheddingStandBy << ") must be less or equal to past size (" << mNpdtPast << ")";
        return -1;
    }
    return ier;
}

double SourceLoad::getTemperature(const std::string& direction)
{
    bool vOk = false;
    double vRet = 0;
    for (MilpPort* lptrport : mListPort)
    {
        if (lptrport->Direction() == direction)        {
            vRet = lptrport->getCarrierTemperature(&vOk);
            if (vOk) break;            
        }
    }
    return vRet;
}

void SourceLoad::computeInitialData()
{
    mHorizonTimeSpanRatio = mHorizon / mTimeSpan + 1;

    setMinValue(mMinSize);

    if (mComputeOptimalPrice) {
        setMaxValue(mMaxOptimalPrice);
    }
    else {
        setMaxValue(1);
    }
}

void SourceLoad::computeModelContribution()
{
    const uint64_t horizon = mHorizon;
    const uint64_t milpNpdtU = static_cast<uint64_t>(mMilpNpdt);
    const double   maxFluxAbs = fabs(mMaxFlux);

    if (mUseControlledFlux)
    {
        // Imposed flux is set by external expression via node equality constraint
        addVariable(mVarControlledFlux, "SoCtrlFlux", 0., maxFluxAbs);

        for (uint64_t t = 0; t < horizon; ++t)
        {
            mExpImposedFlux[t] += mVarControlledFlux(t);
            // Muting last time steps if pre-computed seasonalCosts model
            if (mSeasonalCosts && t >= milpNpdtU) {
                addConstraint(mExpImposedFlux[t] == 0, "seasonalMute", t);
            }
        }
    }
    else if (mUseWeightedFlux)
    {
        // Imposed flux is weighted by external expression via node equality constraint

        addVariable(mVarFluxWeight, "SoWghtFlux", 0., 1.e6);

        for (uint64_t t = 0; t < horizon; ++t)
        {
            mExpFluxWeight[t] += mVarFluxWeight(t);

            mExpImposedFlux[t] += (mExpFluxWeight[t] + mStartStopProfile[t]) * mImposedFlux[t];
            // Muting last time steps if pre-computed seasonalCosts model
            if (mSeasonalCosts && t >= milpNpdtU) {
                addConstraint(mExpImposedFlux[t] == 0, "seasonalMute", t);
            }
        }
    }
    else
    {
        // Imposed flux is imposed by time series, possibly weighted by optimal Size
        const uint64_t forecastStartT = static_cast<uint64_t>(mTimeStepBeginForecast);

        for (uint64_t t = 0; t < horizon; ++t)
        {
            // Muting last time steps if pre-computed seasonalCosts model
            if (mSeasonalCosts && t >= milpNpdtU)
            {
                // Imposed flux is set to zero for seasonnal cost model.
                mExpImposedFlux[t] += 0. * mImposedFlux[t];
            }
            else
            {
                if (t >= forecastStartT && mSeasonalPrevisions)
                {
                    cInfo() << "SourceLoad, mTimeStepBeginForecast =" << mTimeStepBeginForecast;
                    // Imposed flux is imposed by Seasonnal flux time series, possibly weighted by optimal Size
                    mExpImposedFlux[t] += mExpSizeMax * mImposedFluxSeasonal[t];
                }
                else
                {
                    mExpImposedFlux[t] += mExpSizeMax * mImposedFlux[t];
                }
            }
        }
    }

    double coeffin = 1.;
    double coeffout = 1.; // to ensure upward compatibility with previous studies based on OUTPUTFlux1
    if (mAddHeatConsumerModel)
    {
        mTemperature_in1 = getTemperature(KCONS());
        mTemperature_out1 = getTemperature(KPROD());

        if (abs(mTemperature_in1 - mTemperature_out1) > 1.e-3)
            coeffin = mTemperature_in1 / (mTemperature_in1 - mTemperature_out1); // Pin = qCpTin = coeffin * ImposedFlux = coeffin * q * Cp (Tin - Tout)
        else
            coeffin = 1.;

        coeffout = coeffin - 1.;   // Pout = Pin - ImposedFlux
    }

    for (uint64_t t = 0; t < horizon; ++t)
    {
        mExpPowerOut[t] += coeffout * mExpImposedFlux[t];
        mExpPowerIn[t] += coeffin * mExpImposedFlux[t];

        if (mComputeOptimalPrice) {
            mExpCost[t] += Sens() * mExpImposedFlux[t];
        }
        else {
            mExpCost[t] += mEnergyPrice[t] * mExpImposedFlux[t];
        }
    }

    if (mAddStaticCompensation) {
        computeReactivePower();
    }

    if (mAddSheddingDetailed) {
        addLoadShedding();
    }

    if (mAddPeakShavingDetailed) {
        addPeakShaving();
    }

    for (uint64_t t = 0; t < horizon; ++t)
    {
        // 1 constraints: 1 for maximum value, the other one if power Imposed (constant or profile)
        addConstraint(mExpFlux[t] <= maxFluxAbs, "MSFx", t);

        mExpFlux[t] = mExpImposedFlux[t];
        if (mAddSheddingDetailed) {
            mExpFlux[t] -= mExpPowerShedding[t];
        }
        if (mAddPeakShavingDetailed) {
            mExpFlux[t] += mExpPowerPeakShaving[t];
        }
    }
}

void SourceLoad::computeEconomicalContribution()
{
    TechnicalSubModel::computeEconomicalContribution();

    if (mAddVariableCostModel)
        // whether CAPEX + OPEX or not (EcoInvestModel), production cost can be accounted for
        // To be put on for the shedding model
    {
        for (uint64_t t = 0; t < mHorizon; ++t) {
            mExpVariableCosts[t] += Sens() * TimeStep(t) * mExpCost[t];
        }
    }

    if (mAddSheddingDetailed && mAddVariableCostModel) {
        // Shedding penalty
        double shedCost = 0;
        for (uint64_t t = 0; t < mHorizon; t++) {
            if (mAddSheddingTS) {
                shedCost = mCostSheddingTS[t];
            }
            else {
                shedCost = mCostShedding;
            }
            mExpCostShedding[t] += shedCost * mExpPowerShedding[t];
            mExpVariableCosts[t] += mExpCostShedding[t];
        }
    }
    if (mAddPeakShavingDetailed) {
        mExpCapex += mMaxEffectCapex * mVarMaxEffect;
        for (uint64_t t = 0; t < mTimeSteps.size(); ++t) {
            mExpVariableCosts[t] += mMaxEffectCapex * mMaxEffectOpex * mVarMaxEffect / 8760;
        }
    }
}

void SourceLoad::computeReactivePower()
{
    addVariable(mStaticCompensation, "StaticCompensation", -1., 1.);
    addVariable(mReactivePower, "reactivePower");
    mExpStaticCompensation += mStaticCompensation;
    if (mFixedStaticCompensation) {
        for (uint64_t t = 0; t < mHorizon; ++t) {
            mExpReactivePower[t] += mReactivePower(t);
            addConstraint(mExpReactivePower[t] == mStaticCompensationValue * mImposedFlux[t], "ReactivePowerFixedComp", t);
        }
    }
    else {
        for (uint64_t t = 0; t < mHorizon; ++t) {
            mExpReactivePower[t] += mReactivePower(t);
            addConstraint(mExpReactivePower[t] == mExpStaticCompensation * mImposedFlux[t], "ReactivePower", t);
        }
    }
}

bool SourceLoad::isPriceOptimized()
{
    return mComputeOptimalPrice;
}

void SourceLoad::addLoadShedding()
{
    addVariable(mVarPowerShedding, "PowerShedding", 0., fabs(mMaxShedding));

    addVariable(mShedState, "ShedState", 0, 1, MIPModeler::MIP_INT);
    addVariable(mShedOn, "ShedOn", 0, 1, MIPModeler::MIP_INT);
    addVariable(mShedOff, "ShedOff", 0, 1, MIPModeler::MIP_INT);

    // Expressions from variables
    for (uint64_t t = 0; t < mHorizon; t++) {
        mExpPowerShedding[t] += mVarPowerShedding(t);

        mExpShedState[t] += mShedState(t);
        mExpShedOn[t] += mShedOn(t);
        mExpShedOff[t] += mShedOff(t);
    }

    // Constraints to link binaries ShedOn and ShedOff with binary State    
    cInfo() << "Initial shedding state: " << mShedStateIni;

    for (uint64_t t = 0; t < mHorizon; t++) {
        if (t > 0) {
            addConstraint(mExpShedState[t] - mExpShedState[t - 1] - mExpShedOn[t] + mExpShedOff[t] == 0, "ShedState", t);
        }
        else {
            addConstraint(mExpShedState[t] - mShedStateIni - mExpShedOn[t] + mExpShedOff[t] == 0, "ShedState", t);
        }

        addConstraint(mExpShedOn[t] <= mExpShedState[t], "ShedOn", t);
        addConstraint(mExpShedOff[t] <= 1 - mExpShedState[t], "ShedOff", t);
    }

    // Load shedding
    for (uint64_t t = 0; t < mHorizon; ++t)
    {
        addConstraint(mExpPowerShedding[t] <= mExpImposedFlux[t], "PositiveShed", t);

        double maxShedPower;
        if (mAddSheddingTS) {
            maxShedPower = fabs(mMaxSheddingTS[t]);
        }
        else {
            maxShedPower = mMaxShedding;
        }
        addConstraint(mExpPowerShedding[t] <= maxShedPower * mExpShedState[t], "MaxShedPower", t);
    }

    // Max duration of activated shedding
    for (uint64_t t = mMaxTimeShedding - 1; t < mHorizon; t++) {
        MIPModeler::MIPExpression sumExp;
        sumExp = 0;

        for (uint64_t i = t - mMaxTimeShedding + 1; i <= t; i++) {
            sumExp += mExpShedOn[i];
        }

        addConstraint(sumExp >= mExpShedState[t], "MaxShedTime", t);

        sumExp.close();
    }

    // Enabling Rolling Horizon option
    if (mNpdtPast > 0 && mControl == "RollingHorizon")
    {
        for (uint64_t t = 0; t < mMaxTimeShedding - 2; t++)
        {
            MIPModeler::MIPExpression sumExp;

            for (uint64_t i = t - mMaxTimeShedding + 1; i <= t; i++)
            {
                if (i < 0)
                    sumExp += mExpHistOn[i + mNpdtPast];
                else
                {
                    sumExp += mExpShedOn[i];
                }
            }

            addConstraint(sumExp >= mExpShedState[t], "MaxShedTime", t);

            sumExp.close();
        }
    }

    // Min duration of deactivated shedding
    for (uint64_t t = mMinSheddingStandBy - 1; t < mHorizon; t++) {
        MIPModeler::MIPExpression sumExp;
        sumExp = 0;

        for (uint64_t i = t - mMinSheddingStandBy + 1; i <= t; i++) {
            sumExp += mExpShedOff[i];
        }
        addConstraint(sumExp <= 1 - mExpShedState[t], "MinShedStdBy", t);

        sumExp.close();
    }

    // Enabling Rolling Horizon option
    if (mNpdtPast > 0 && mControl == "RollingHorizon")
    {
        for (uint64_t t = 0; t < mMinSheddingStandBy - 2; t++)
        {
            MIPModeler::MIPExpression sumExp;

            for (uint64_t i = t - mMinSheddingStandBy + 1; i <= t; i++)
            {

                if (i < 0) sumExp += mExpShedOff[i + mNpdtPast];
                else {
                    sumExp += mExpShedOff[i];
                }
            }

            addConstraint(sumExp <= 1 - mExpShedState[t], "MinShedStdBy", t);

            sumExp.close();
        }
    }
}

void SourceLoad::addPeakShaving()
{
    std::vector<double>::iterator itmax = std::max_element(mImposedFlux.begin(), mImposedFlux.end());      //find the maximum value of given flux
    int index = std::distance(mImposedFlux.begin(), itmax);
    double mMaxImposedFlux = mImposedFlux[index];

    //VARIABLES
    addVariable(mVarPowerPeakShaving, "VarPowerPeakShaving", -fabs(mMaxEffect), fabs(mMaxEffect));
    addVariable(mVarMaxEffect, "VarMaxEffect", 0, fabs(mMaxEffect));

    // Expressions from variables
    for (uint64_t t = 0; t < mHorizon; t++) {
        mExpPowerPeakShaving[t] += mVarPowerPeakShaving(t);
    }

    //CONSTRAINTS
    if (mMaxEffect > 0.)
        addConstraint(mVarMaxEffect == mMaxEffect, "DefMaxEffect");
    for (uint64_t t = 0; t < mHorizon; ++t) {
        addConstraint(mExpPowerPeakShaving[t] - mVarMaxEffect <= 0, "FluxGridBound");
        addConstraint(mExpPowerPeakShaving[t] + mVarMaxEffect >= 0, "FluxGridBound2");
        addConstraint(mExpImposedFlux[t] + mExpPowerPeakShaving[t] - (mMaxImposedFlux + fabs(mMaxEffect)) <= 0, "FluxGridMaxBound");
    }

    for (uint64_t p = 0; p < mHorizon / mTimeSpan + 1; ++p) {
        for (unsigned int t = 0; t < std::min<unsigned int>(mTimeSpan, mHorizon - p * mTimeSpan); ++t) {
            mExpSums[p] += mExpPowerPeakShaving[mTimeSpan * p + t];
        }
        addConstraint(mExpSums[p] == 0., "Sums", p);
    }
}

void SourceLoad::computeAllIndicators(const double* optSol)
{
    SourceLoadSubModel::computeAllIndicators(optSol);
}

