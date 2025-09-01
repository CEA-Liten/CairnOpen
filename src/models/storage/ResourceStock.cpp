#include "ResourceStock.h"
extern "C" MODELS_DECLSPEC QObject * createModel(QObject * aParent)
{
    return new ResourceStock(aParent);
}

ResourceStock::ResourceStock(QObject* aParent) : StorageSubModel(aParent)
{

}

ResourceStock::~ResourceStock()
{}

void ResourceStock::setTimeData() {
    SubModel::setTimeData();
}

int ResourceStock::checkConsistency()
{
    return 0;
}

void ResourceStock::closeExpressions()
{
    SubModel::closeExpressions();
}

void ResourceStock::buildModel() 
{
    if (!mAllocate)
    {
        closeExpressions();
    }

    setExpSizeMax(mMinSize,mTotalMaxSize, "MaxResourceUsed");

    /** Compute all expressions */
    computeAllContribution();

    mAllocate = false;
}


void ResourceStock::computeEconomicalContribution()
{
    TechnicalSubModel::computeEconomicalContribution();
}

void ResourceStock::computeAllIndicators(const double* optSol)
{
    StorageSubModel::computeDefaultIndicators(optSol);
}

