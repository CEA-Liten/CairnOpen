#include "Electrolyzer.h"
extern "C" MODELS_DECLSPEC QObject * createModel(QObject * aParent)
{
    return new Electrolyzer(aParent);
}

Electrolyzer::Electrolyzer(QObject* aParent) :
    ConverterSubModel(aParent),
    mPortUsedPower(nullptr),
    mPortH2MassFlowRate(nullptr),
    mPci_H2(1.)
{
    mAgeingModel = new AgeingRunningHours(mInputParam, mInputData);
}

Electrolyzer::~Electrolyzer()
{    
}

int Electrolyzer::checkConsistency() // DO NOT SHOW 
{
    int ier = TechnicalSubModel::checkConsistency();
    return ier;
}

//-----------------------------------------------------------------------------
void Electrolyzer::buildModel() 
{
    ConverterSubModel::buildModel();
    /**
    * Assumes that the default port "PortH2MassFlowRate" is mandatory to be used and its variable cannot be changed from FluidH2.
    * Otherwise, should loop over all ports and look for a connected output port whose variable is FluidH2.
    */
    if (mPortH2MassFlowRate->ptrEnergyVector() != nullptr) {
        mPci_H2 = mPortH2MassFlowRate->ptrEnergyVector()->LHV();
    }

    // parameters:
    // ===========   
    /**
    * If use ageing is activated, a model is used to simulate a degradation of efficiency and/or capacity.
    * See :ref:`AgeingRunningHours`
    */
    double efficiencyAgeing = Efficiency();
    double capacityAgeing = CapacityAgeing();
    if (mUseAgeing) {        
        qInfo() << "Electrolyzer efficiency " << mEfficiency * efficiencyAgeing;
        qInfo() << "Electrolyzer capcity ageing" << capacityAgeing;
    }
    
    /**
    * Computation of energy consumption for H2 production :
    * `aConvertPowToMfr` is a factor of conversion built with H2 LHV, ageing coefficient and Efficiency.
    * If the hydrogen is recorded in kg, the conversion is done with LHV.
    */
    double aConvertPowToMfr = 0.;
    if (mEfficiencyLHVbased)
        aConvertPowToMfr = mEfficiency / mPci_H2 * efficiencyAgeing; // conversion MWh -> kg/h
    else {
        aConvertPowToMfr = mEfficiency_Global;
    }
    
    /**
    * Max size of electrolyzer is defined by MaxPower_H2
    */
    setExpSizeMax(mMaxPower_H2, "MaxPower");

    mMinFlow_H2 = mMinPower_H2 * getMaxBound() * aConvertPowToMfr;
    mMaxFlow_H2 = getMaxBound() * aConvertPowToMfr;

    // allocate variables and expressions
    // =========
    if (mAllocate) {
        mVarUsablePower = MIPModeler::MIPVariable0D(0.f, getMaxBound());
        mVarPower_H2 = MIPModeler::MIPVariable1D(mHorizon, 0., getMaxBound());
        mVarFlow_H2 = MIPModeler::MIPVariable1D(mHorizon, 0., mMaxFlow_H2);
        
        mExpUsablePower = MIPModeler::MIPExpression();
        mExpPower_H2 = MIPModeler::MIPExpression1D(mHorizon);
        mExpTotalPower = MIPModeler::MIPExpression1D(mHorizon);
        mExpFlow_H2 = MIPModeler::MIPExpression1D(mHorizon);
        mExpCost = MIPModeler::MIPExpression1D(mHorizon);

        if (mAddAuxConso) {
            mVarAuxConso = MIPModeler::MIPVariable1D(mHorizon, 0.f, mAuxConso*getMaxBound());
            mExpAuxConso = MIPModeler::MIPExpression1D(mHorizon);
        }

        if (mAddStdByConso) {
            mVarZ2 = MIPModeler::MIPVariable1D(mHorizon, 0., mStdByConso * getMaxBound());
            mVarStdByConso = MIPModeler::MIPVariable1D(mHorizon, 0.f, mStdByConso * getMaxBound());
            mExpStdByConso = MIPModeler::MIPExpression1D(mHorizon);
        }
    }
    else {
        closeExpression(mExpUsablePower);
        closeExpression1D(mExpPower_H2);
        closeExpression1D(mExpTotalPower);
        closeExpression1D(mExpFlow_H2);
        closeExpression1D(mExpCost);
        closeExpression1D(mExpAuxConso);
        closeExpression1D(mExpStdByConso);
    }
    // add variables to model
    // =========
    addVariable(mVarPower_H2, "Pow");
    addVariable(mVarFlow_H2, "Flow");
    addVariable(mVarUsablePower, "UsablePow");
    if (mAddAuxConso) addVariable(mVarAuxConso, "AuxConso");
    if (mAddStdByConso) {
        addVariable(mVarStdByConso, "StdByConso");
        addVariable(mVarZ2, "StdByConsoZ2");
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

        //Costs
        mExpCost[t] += mExpPower_H2[t] * mCost * TimeStep(t);
    }

    //
    // variables, expressions and optional constraints
    // -----------------------------------------------

    // Integer variables: On/Off
    // -----------------------
    addStateConstraints(mHorizon, mCondensedNpdt);

    // constraints
    // ===========

    /**
    * The factor of conversion is then used to compute the energy consumption of the electrolyzer.
    */
    for (uint64_t t = 0; t < mHorizon; t++) {
        addConstraint(mExpPower_H2[t] - mExpFlow_H2[t] / aConvertPowToMfr == 0, "PowFlow", t);
    }

    /**
    * The maximum power is limited by usable power. Usable power is decreasing during the time by capacityAgeing.
    */

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
            addConstraint(mExpAuxConso[t] == mConverterUse[t] * mExpSizeMax * mAuxConso, "DefVarAuxConso"); // warning, it is not impacted by ageing
            mExpTotalPower[t] += mExpAuxConso[t];
        }
        if (mAddStdByConso) {
            addConstraint(mExpStdByConso[t] == mConverterUse[t] * mVarZ2(t), "DefVarStdByConso");
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
        addConstraint(mExpPower_H2[t] - mExpSizeMax * mConverterUse[t] <= 0, "ConverterUse", t);
    }

    /** Compute all expressions */
    computeAllContribution();

    mAllocate = false;
}

//-----------------------------------------------------------------------------
void Electrolyzer::computeEconomicalContribution() { 
    TechnicalSubModel::computeEconomicalContribution();
    for (uint64_t t = 0; t < mHorizon; t++)
        mExpVariableCosts[t] += mExpCost[t];
}
//-----------------------------------------------------------------------------
void Electrolyzer::computeAllIndicators(const double* optSol) // DO NOT SHOW
{
    ConverterSubModel::computeDefaultIndicators(optSol);
}