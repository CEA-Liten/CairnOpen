#include "OperationSubModel.h"

OperationSubModel::OperationSubModel(CairnObject* aParent) :
SubModel(aParent),
mVariableCosts(2, 0.)
{
}

OperationSubModel::~OperationSubModel()
{
}

int OperationSubModel::checkConsistency() {
    return SubModel::checkConsistency();
}

void OperationSubModel::closeExpressions()
{
    SubModel::closeExpressions();
    closeExpression1D(mExpVariableCosts);
}

void OperationSubModel::buildModel()
{
    if (mAllocate) {
        allocateExpressions();
    }
    else {
        closeExpressions();
    }

    /* set SizeMax expression and add constraints on SizeMax */
    setExpSizeMax();

    /* add State and StartUpShutDown constraints : add On/Off variable */
    if (mAddStateVariable) {
        addStateConstraints(varMilpHorizon());
    }

    if (mAddStartUpShutDownVariable) {
        addStartUpShutDown(varMilpHorizon());
    }

    /** compute expressions, add variables and add constraints */
    computeModelContribution();

    mAllocate = false;
}

void OperationSubModel::computeDefaultIndicators(const double* optSol)
{
    mVariableCosts.at(0) = 0.;
    for (uint64_t t = 0; t < mHorizon; ++t) mVariableCosts.at(0) += mExpVariableCosts.at(t).evaluate(optSol) * mParentCompo->ExtrapolationFactor(); // PLAN
    for (uint64_t t = 0; t < *mptrTimeshift; ++t) mVariableCosts.at(1) += mExpVariableCosts.at(t).evaluate(optSol); // HIST
}


