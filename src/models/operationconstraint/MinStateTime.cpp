/* --------------------------------------------------------------------------
 * File: MinStateTime.cpp
 * version 1.0
 * Author: Pimprenelle Parmentier
 * Date 09/03/2022
 *---------------------------------------------------------------------------
 * Description: Model imposing a minimal time to stay on or off .
 * --------------------------------------------------------------------------
 */

#include "MinStateTime.h"
extern "C" MODELS_DECLSPEC QObject * createModel(QObject * aParent)
{
    return new MinStateTime(aParent);
}

MinStateTime::MinStateTime(QObject* aParent) : OperationSubModel(aParent),
mNbStartUps(2, 0.),
mNbShutDowns(2, 0.),
mLevStartUpsCost(2, 0.),
mLevShutDownsCost(2, 0.)
{
}

MinStateTime::~MinStateTime()
{
}

void MinStateTime::addMinProductionTime() {
    bool insideTypicalPeriod = true;
    double npdtMinProdTime = ceil(mMinProductionTime / TimeStep(0));
    for (uint64_t t = 0; t < mMilpNpdt; t++) {
        MIPModeler::MIPExpression sumExp;
        sumExp = 0;
        insideTypicalPeriod = true;
        for (uint64_t t2 = t + 1; t2 <= t + npdtMinProdTime; t2++) {
            if (int(t2 - npdtMinProdTime + *mptrTimeshift - 1) % mNDtTypicalPeriods == 0 || t2 <= (uint64_t)npdtMinProdTime) {
                insideTypicalPeriod = false;
            }
            if (t2 >= (uint64_t)npdtMinProdTime) {
                sumExp += mExpStartUp[t2 - npdtMinProdTime];
            }
            else if (!mAllocate) {
                sumExp += mHistStartUp[mNpdtPast + t2 - npdtMinProdTime];
            }
        }
        if (!mUseTypicalPeriods || insideTypicalPeriod || mActivateConstraintsBetweenTP) 
            addConstraint((sumExp - mExpState[t] <= 0), "MinProdTime", t);
        sumExp.close();
    }
}

void MinStateTime::addMinStandByTime() {
    bool insideTypicalPeriod = true;
    double npdtMinStandbyTime = ceil(mMinStandbyTime / TimeStep(0));
    for (uint64_t t = 0; t < mMilpNpdt; t++) {
        MIPModeler::MIPExpression sumExp;
        sumExp = 0;
        insideTypicalPeriod = true;
        for (uint64_t t2 = t + 1; t2 <= t + npdtMinStandbyTime; t2++) {
            if (int(t2 - mMinProductionTime + *mptrTimeshift - 1) % mNDtTypicalPeriods == 0 || t2 <= (uint64_t)npdtMinStandbyTime)
                insideTypicalPeriod = false;
            if (t2 >= (uint64_t)npdtMinStandbyTime) {
                sumExp += mExpShutDown[t2 - npdtMinStandbyTime];
            }
            else if (!mAllocate) {
                sumExp += mHistShutDown[mNpdtPast + t2 - npdtMinStandbyTime];
            }
        }
        if (!mUseTypicalPeriods || insideTypicalPeriod || mActivateConstraintsBetweenTP)
            addConstraint((sumExp + mExpState[t] <= 1 ), "MinStandByTime", t);
        sumExp.close();
    }
}

void MinStateTime::setParameters(double aMinConstraintBusValue, double aMaxConstraintBusValue, double aStrictConstraintBusValue)
{
}

void MinStateTime::setTimeData()
{
    SubModel::setTimeData();
    mConverterUse.resize(mHorizon, 1.0);
}

int MinStateTime::checkConsistency()
{
    if (mControl == "MPC" || mControl == "RollingHorizon") {
        if ((mAddMinProductionTime && mMinProductionTime > mNpdtPast * TimeStep(0)) || (mAddMinStandbyTime && mMinStandbyTime > mNpdtPast * TimeStep(0))) {
            qCritical() << parent()->objectName() << ":Npdt past  size is equal to " << mNpdtPast <<
                " but should be greater than MinProductionTime and MinStandbyTime, eg " << max(ceil(mMinProductionTime / TimeStep(0)), ceil(mMinStandbyTime / TimeStep(0)));
            return -1;
        }
    }
    
    return 0; // ier;
}

void MinStateTime::buildModel()
{
    // allocate variables and expressions
    if (mAllocate) {
        mExpInstalled = MIPModeler::MIPExpression();
        if (mAddStartUpCost) mExpStartUpCosts = MIPModeler::MIPExpression1D(mTimeSteps.size());
        if (mAddShutDownCost) mExpShutDownCosts = MIPModeler::MIPExpression1D(mTimeSteps.size());
    }
    else {
        if (mAddStartUpCost) closeExpression1D(mExpStartUpCosts);
        if (mAddShutDownCost) closeExpression1D(mExpShutDownCosts);
    }
    addVariable(mVarInstalled, "Installed");
    mExpInstalled = mVarInstalled;

    uint64_t nPdtCond = varMilpHorizon();
    addStateConstraints(mHorizon, mCondensedNpdt);
    addStartUpShutDown(nPdtCond, mHorizon);

    // constraints
    // ===========
    addConstraint(mVarInstalled == 1, "sInstalled");

    if (mAddMinProductionTime) {
        addMinProductionTime();
    }
    if (mAddMinStandbyTime) {
        addMinStandByTime();
    }

    computeAllContribution() ;

    mAllocate = false ;
}

//----------------Parts of buildProblem -------------------------------------- 
void MinStateTime::computeAllContribution()
{
    OperationSubModel::computeAllContribution() ;

    if (mAddStartUpCost) {
        for (uint64_t t = 0; t < mHorizon; t++) {
            mExpStartUpCosts[t] += mExpStartUp[t] * mStartUpCost;
            mExpVariableCosts[t] += mExpStartUpCosts[t];
        }
            
    }
    if (mAddShutDownCost) {
        for (uint64_t t = 0; t < mHorizon; t++) {
            mExpShutDownCosts[t] += mExpShutDown[t] * mShutDownCost;
            mExpVariableCosts[t] += mExpShutDownCosts[t];
        }
    }
}

void MinStateTime::computeAllIndicators(const double* optSol)
{
    OperationSubModel::computeDefaultIndicators(optSol);

    mNbStartUps.at(0) = 0.;
    mNbShutDowns.at(0) = 0.;
    mLevStartUpsCost.at(0) = 0.;
    mLevShutDownsCost.at(0) = 0.;
    for (uint64_t t = 0; t < mHorizon; ++t) { // PLAN
        mNbStartUps.at(0) += mExpStartUp.at(t).evaluate(optSol);
        mNbShutDowns.at(0) += mExpShutDown.at(t).evaluate(optSol);
        if (mAddStartUpCost) mLevStartUpsCost.at(0) += mExpStartUp.at(t).evaluate(optSol) * mStartUpCost * mParentCompo->ExtrapolationFactor();
        if (mAddShutDownCost) mLevShutDownsCost.at(0) += mExpShutDown.at(t).evaluate(optSol) * mShutDownCost * mParentCompo->ExtrapolationFactor();
    }
    for (uint64_t t = 0; t < *mptrTimeshift; ++t) { // HIST
        mNbStartUps.at(1) += mExpStartUp.at(t).evaluate(optSol);
        mNbShutDowns.at(1) += mExpShutDown.at(t).evaluate(optSol);
        if (mAddStartUpCost) mLevStartUpsCost.at(1) += mExpStartUp.at(t).evaluate(optSol) * mStartUpCost;
        if (mAddShutDownCost) mLevShutDownsCost.at(1) += mExpShutDown.at(t).evaluate(optSol) * mShutDownCost;
    }
}







