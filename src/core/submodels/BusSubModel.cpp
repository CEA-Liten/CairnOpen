#include "BusSubModel.h"

BusSubModel::BusSubModel(CairnObject* aParent) :
SubModel(aParent)
{
}

BusSubModel::~BusSubModel(){
}

void BusSubModel::computeDefaultIndicators(const double* optSol)
{
}

void BusSubModel::buildModel()
{
    if (mAllocate) {
        allocateExpressions();
    }
    else {
        closeExpressions();
    }

    /** compute expressions, add variables and add constraints */
    computeModelContribution();

    mAllocate = false;
}




