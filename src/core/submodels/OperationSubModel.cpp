#include "OperationSubModel.h"

OperationSubModel::OperationSubModel(QObject* aParent) :
SubModel(aParent),
mVariableCosts(2, 0.)
{
}

OperationSubModel::~OperationSubModel(){ }

void OperationSubModel::computeDefaultIndicators(const double* optSol)
{
    mVariableCosts.at(0) = 0.;
    for (uint64_t t = 0; t < mHorizon; ++t) mVariableCosts.at(0) += mExpVariableCosts.at(t).evaluate(optSol) * mParentCompo->ExtrapolationFactor(); // PLAN
    for (uint64_t t = 0; t < *mptrTimeshift; ++t) mVariableCosts.at(1) += mExpVariableCosts.at(t).evaluate(optSol); // HIST
}

void OperationSubModel::computeAllContribution()
{
    if (mAllocate){
        mExpVariableCosts = MIPModeler::MIPExpression1D(mTimeSteps.size());
    }
    else {
        closeExpression1D(mExpVariableCosts);
    }
}


bool OperationSubModel::defineDefaultVarNames(MilpPort* port)
{
    // seulement PhysicalEquationCompo
    if (port->ptrEnergyVector()->Type() == "FluidH2")
    {
        port->setVariable("Power");
        port->setDirection(KPROD());
        return true;
    }
    else if (port->ptrEnergyVector()->Type() == "Electrical")
    {
        port->setVariable("Power");
        port->setDirection(KCONS());
        return true;
    }
    return false;
}

