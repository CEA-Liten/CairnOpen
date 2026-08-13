#include "ResourceStock.h"
extern "C" MODELS_DECLSPEC CairnObject * createModel(CairnObject * aParent)
{
    return new ResourceStock(aParent);
}

ResourceStock::ResourceStock(CairnObject* aParent) : StorageSubModel(aParent)
{
}

ResourceStock::~ResourceStock()
{}

void ResourceStock::setTimeData() {
    SubModel::setTimeData();
}

void ResourceStock::computeInitialData()
{
    setMinValue(mMinSize);
    setMaxValue(mTotalMaxSize);

}

void ResourceStock::computeModelContribution()
{

}

void ResourceStock::computeEconomicalContribution()
{
    TechnicalSubModel::computeEconomicalContribution();
}

void ResourceStock::computeAllIndicators(const double* optSol)
{
    StorageSubModel::computeAllIndicators(optSol);
}

