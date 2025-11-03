/* --------------------------------------------------------------------------
 * File: Ramp.cpp
 * version 1.0
 * Author: Pimprenelle Parmentier
 * Date 09/03/2022
 *---------------------------------------------------------------------------
 * Description: Model imposing ramps or adding ramp costs for variables linked to a bus.
 * --------------------------------------------------------------------------
 */

#include "Ramp.h"
extern "C" MODELS_DECLSPEC CairnObject * createModel(CairnObject * aParent)
{
    return new Ramp(aParent);
}

Ramp::Ramp(CairnObject* aParent)
    :OperationSubModel(aParent),
    mInitialValue(0.)
{

}

Ramp::~Ramp()
{
}

void Ramp::setParameters(double aMinConstraintBusValue, double aMaxConstraintBusValue, double aStrictConstraintBusValue)
{
}

void Ramp::setTimeData()
{
    SubModel::setTimeData();
    mRampUpLimit.resize(mHorizon);
    mRampDownLimit.resize(mHorizon);
}

void Ramp::computeInitialData()
{
    setMaxValue(mMaxWeight);
    setMinValue(mMinSize);

    mAddStateVariable = mAddStartUpShutDownVariable;
}

void Ramp::computeModelContribution()
{
    if (mConstantRamp) {
        mRampUpLimit = std::vector<double>(mHorizon);
        mRampDownLimit = std::vector<double>(mHorizon);
        for (unsigned int t = 0; t < mHorizon; ++t) {
            mRampUpLimit[t] = mConstantRampUpLimit; //* TimeStep(t) 
            mRampDownLimit[t] = mConstantRampDownLimit; // *TimeStep(t);
        }
    }

    // add variables to model
    // =========
    addVariable(mVarInput,"RampInput");

    // fill expressions
    // ===========
    for (unsigned int t = 0; t < mHorizon; ++t) {
        mExpInput[t] += mVarInput(t);
    }

    // constraints
    // ===========
    if (mAddStartUpShutDownVariable) {
        addRelativeRampWithMinPower();
    }
    else {
        addRelativeRamp();
    }

    if (mAddRampCost) {
        addRampCost();
    }

    if (mAddRampCost) {
        for (uint64_t t = 0; t < mHorizon; t++)
            mExpVariableCosts[t] += mExpRampCostVar[t];
    }
}

void Ramp::addRampCost() 
{
    addVariable(mRampCostVar, "RampCost", 0, getMaxBound() * mRampCost, MIPModeler::MIP_FLOAT, varMilpHorizon());

    fillExpression(mExpRampCostVar, mRampCostVar);
    for (uint64_t t = 0; t < mHorizon; t++) {
        if (t > 0) {
            addConstraint((mExpRampCostVar[t] >= mRampCost * (mExpInput[t] - mExpInput[t - 1])), "RampCostVar", t);
            addConstraint((mExpRampCostVar[t] >= mRampCost * (mExpInput[t - 1] - mExpInput[t])), "RampCostVar", t);
        }
        else if (!mAllocate) {
            addConstraint((mExpRampCostVar[t] >= mRampCost * (mExpInput[t] - mHistInput)), "RampCostVar", t);
            addConstraint((mExpRampCostVar[t] >= mRampCost * (mHistInput - mExpInput[t])), "RampCostVar", t);
        }
    }
}

void Ramp::addRelativeRamp()
{
    for (uint64_t t = 0; t < mHorizon; t++) {
        if (t > 0) {
            if (mAddRampUp) addConstraint((mExpInput)[t] - (mExpInput)[t-1] <= mRampUpLimit[t] * TimeStep(t) * (mVarSizeMax), "RelRampUp");
            if (mAddRampDown) addConstraint((mExpInput)[t-1] - (mExpInput)[t] <= mRampDownLimit[t] * TimeStep(t) * (mVarSizeMax), "RelRampDown");
        }
        else if (!mAllocate) {
            if (mAddRampUp) addConstraint((mExpInput)[t] - mHistInput <= mRampUpLimit[t] * TimeStep(t) * (mVarSizeMax), "RelRampUpCycle");
            if (mAddRampDown) addConstraint(mHistInput - (mExpInput)[t] <= mRampDownLimit[t] * TimeStep(t) * (mVarSizeMax), "RelRampDownCycle");
        }
    }
}
//-------------------------------------------------------------------------------------------
void Ramp::addRelativeRampWithMinPower(){
    for (int i=0; i < (mHorizon-1); i++){
        addConstraint(mExpInput[i] <= mState(i) * fabs(mMaxWeight), "MaxInput");
        addConstraint(mExpInput[i] >= mMinInput * (mExpSizeMax- getMaxBound() * (1-mState(i))), "MinInput");
        addConstraint((mExpInput)[i + 1] - (mExpInput)[i] <= mRampUpLimit[i] * mExpSizeMax * TimeStep(i) + mMinInput * fabs(mMaxWeight) * mStartUp(i+1), "RampUpMinInp1");
        addConstraint((mExpInput)[i + 1] - (mExpInput)[i] <= mMinInput * mExpSizeMax + mRampUpLimit[i] * fabs(mMaxWeight) * TimeStep(i) * (1-mStartUp(i+1)), "RampUpMinInp2");
        addConstraint((mExpInput)[i] - (mExpInput)[i + 1] <= mRampDownLimit[i] * TimeStep(i) * mExpSizeMax + fabs(mMaxWeight) * mMinInput * mShutDown(i + 1), "RampDownMinInp1");
        addConstraint((mExpInput)[i] - (mExpInput)[i + 1] <= mMinInput * mExpSizeMax + mRampDownLimit[i] * fabs(mMaxWeight) * TimeStep(i) * (1 - mShutDown(i + 1)), "RampDownMinInp1");
    }
}
//-------------------------------------------------------------------------------------------

void Ramp::computeAllIndicators(const double* optSol)
{
    OperationSubModel::computeDefaultIndicators(optSol);
}







