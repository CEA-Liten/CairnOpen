#include "Electrolyzer.h"
extern "C" MODELS_DECLSPEC CairnObject * createModel(CairnObject * aParent)
{
    return new Electrolyzer(aParent);
}

Electrolyzer::Electrolyzer(CairnObject* aParent) :
    ConverterSubModel(aParent),
    mPortUsedPower(nullptr),
    mPortH2MassFlowRate(nullptr),
    mPci_H2(1.)
{
    mPossibleModelClasses = { "Electrolyzer", "ElectrolyzerDetailed" };
}

Electrolyzer::~Electrolyzer()
{    
}

int Electrolyzer::checkConsistency()  
{
    int ier = TechnicalSubModel::checkConsistency();
    if (mPortH2MassFlowRate->useProfileLHV()) {
        cCritical() << "ERROR: TS for LHV is not allowed for hydrogen ";
        return -1;
    }
    return ier;
}

//-----------------------------------------------------------------------------
void Electrolyzer::computeInitialData() 
{
    setMaxValue(mMaxPower_H2);
    setMinValue(mMinSize);

    mAddStateVariable = true; /* always add state constraints */
}

void Electrolyzer::computeModelContribution()
{
    /**
    * If use ageing is activated, a model is used to simulate a degradation of efficiency and/or capacity.
    * See :ref:`AgeingRunningHours`
    */

    /** Assumes that the default port "PortH2MassFlowRate" is mandatory to be used and its variable cannot be changed from FluidH2.
    * Otherwise, should loop over all ports and look for a connected output port whose variable is FluidH2. */

    if (!mPortH2MassFlowRate->useProfileLHV()) {
        mPci_H2 = mPortH2MassFlowRate->LHV();
    }

    /**Computation of energy consumption for H2 production :
    * `convertPowToMfr` is a factor of conversion built with H2 LHV, ageing coefficient and Efficiency.
    * If the hydrogen is recorded in kg, the conversion is done with LHV. */
    const double efficiencyAgeing = EfficiencyAgeing();

    double convertPowToMfr = 0.;
    if (mEfficiencyLHVbased) {
        convertPowToMfr = mEfficiency / mPci_H2 * efficiencyAgeing; // conversion MWh -> kg/h
    }
    else {
        convertPowToMfr = mEfficiency_Global;
    }

    mMinFlow_H2 = mMinPower_H2 * getMaxBound() * convertPowToMfr;
    mMaxFlow_H2 = getMaxBound() * convertPowToMfr;

    // add variables to model
    // =========
    addVariable(mVarPower_H2, "Pow", 0., getMaxBound());
    addVariable(mVarFlow_H2, "Flow", 0., mMaxFlow_H2);
    addVariable(mVarUsablePower, "UsablePow", 0.f, getMaxBound());
    if (mAddAuxConso) addVariable(mVarAuxConso, "AuxConso", 0.f, mAuxConso * getMaxBound());
    if (mAddStdByConso) {
        addVariable(mVarStdByConso, "StdByConso", 0.f, mStdByConso * getMaxBound());
        addVariable(mVarZ2, "StdByConsoZ2", 0., mStdByConso * getMaxBound());
    }

    // fill expressions
    // ===========
    mExpUsablePower += mVarUsablePower;

    for (uint64_t t = 0; t < mHorizon; t++) {
        mExpFlow_H2[t] += mVarFlow_H2(t);
    }
    for (uint64_t t = 0; t < mHorizon; t++) {
        mExpPower_H2[t] += mVarPower_H2(t);
        mExpTotalPower[t] += mVarPower_H2(t);
        if (mAddAuxConso) mExpAuxConso[t] += mVarAuxConso(t);
        if (mAddStdByConso) mExpStdByConso[t] += mVarStdByConso(t);
    }

    //
    // variables, expressions and optional constraints
    // -----------------------------------------------

    // constraints
    // ===========

    /**
    * The factor of conversion is then used to compute the energy consumption of the electrolyzer.
    */
    for (uint64_t t = 0; t < mHorizon; t++) {
        addConstraint(mExpPower_H2[t] - mExpFlow_H2[t] / convertPowToMfr == 0, "PowFlow", t);
    }

    /**
    * The maximum power is limited by usable power. Usable power is decreasing during the time by capacityAgeing.
    */
    const double capacityAgeing = CapacityAgeing();
    addConstraint(mExpUsablePower - mExpSizeMax * capacityAgeing == 0, "UsePow");
    for (uint64_t t = 0; t < mHorizon; t++) {
        addConstraint(mExpTotalPower[t] <= mExpUsablePower, "MaxTotalPow", t);
    }

    /**
    *  Minimum power: if integer variables are allowed,
    *  linearization of the function 
    *  :math:`Z(t) = varMinPowerH2 * Yonoff(t) (linearization)`.
    */

    for (uint64_t t = 0; t < mHorizon; t++) {
        if (mAddAuxConso) {
            addConstraint(mExpAuxConso[t] == mComponentAvailabilityTS[t] * mExpSizeMax * mAuxConso, "DefVarAuxConso"); // warning, it is not impacted by ageing
            mExpTotalPower[t] += mExpAuxConso[t];
        }
        if (mAddStdByConso) {
            addConstraint(mExpStdByConso[t] == mComponentAvailabilityTS[t] * mVarZ2(t), "DefVarStdByConso");
            mExpTotalPower[t] += mExpStdByConso[t];
        }
    }

    if (! mLPModelOnly) {
        for (uint64_t t = 0; t < mHorizon; t++) {
        if (mAddStdByConso) {
            mModel->add((mVarZ2(t) <= mStdByConso * getMaxBound() * (1 - mExpState[t])), CName("cZ2-1", t));
            mModel->add((mVarZ2(t) <= mStdByConso * mVarSizeMax), CName("cZ2-2", t));
            mModel->add((mVarZ2(t) >= mStdByConso * mVarSizeMax - mStdByConso * getMaxBound() * mExpState[t]), CName("cZ2-3", t));
            }
        }
        double maxBound = getMaxBound();
        setMinPower(mExpPower_H2, mMinPower_H2, maxBound);
    }
    else {
        for (uint64_t t = 0; t < mHorizon; t++) {
            addConstraint(mExpPower_H2[t] >= mMinPower_H2 * mExpSizeMax, "MinPowerLPModOnly", t);
        }
    }

    for (uint64_t t = 0; t < mHorizon; t++) {
        addConstraint(mExpPower_H2[t] - mExpSizeMax * mComponentAvailabilityTS[t] <= 0, "ConverterUse", t);
    }
}

//-----------------------------------------------------------------------------
void Electrolyzer::computeEconomicalContribution() { 
    TechnicalSubModel::computeEconomicalContribution();

    //Variable OPEX
    for (uint64_t t = 0; t < mHorizon; t++) {
        mExpVariableCosts[t] += mExpPower_H2[t] * mCost * TimeStep(t);
        mExpVariableOpex[t] += mExpFlow_H2[t] * mVariableOpex * TimeStep(t);
    }
}
//-----------------------------------------------------------------------------
void Electrolyzer::computeAllIndicators(const double* optSol) // DO NOT SHOW
{
    ConverterSubModel::computeDefaultIndicators(optSol);
}