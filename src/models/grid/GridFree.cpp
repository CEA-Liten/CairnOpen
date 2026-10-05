//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
// file		GridFree.cpp
// brief	GridFree model
// version	1.0
// author	Alain Ruby
// date		10/05/2019
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

#include "GridFree.h"
extern "C" MODELS_DECLSPEC CairnObject * createModel(CairnObject * aParent)
{
    return new GridFree(aParent);
}

//---------------------------------------------------------------------------
GridFree::GridFree(CairnObject* aParent) : GridSubModel(aParent),
mSeasonalCosts(false),
mSeasonalCostsFree(false),
mUseConstantPrice(false),
mConstantBuyPrice(0.), 
mConstantSellPrice(0.)
{
    /* 
        Should be set to false in setModelConfigurationParameters()
        to over-write the default value provided in declareModelConfigurationParameters() 

        mEcoInvestModel = false;
    */
}
//---------------------------------------------------------------------------
GridFree::~GridFree() {

}

//---------------------------------------------------------------------------
void GridFree::setTimeData() {
    GridSubModel::setTimeData();
    
    mTemporalPrice.resize(mHorizon);
}
//---------------------------------------------------------------------------

void GridFree::computeModelContribution()
{
    const uint64_t horizon = mHorizon;
    const double maxFluxAbs = std::fabs(mMaxFlux);
    const double minFluxAbs = std::fabs(mMinFlux);
    const double sens = Sens();

    addVariable(mVarFluxGrid, "GFx", minFluxAbs, maxFluxAbs);

    for (uint64_t t = 0; t < horizon; ++t) {
        const auto flux = mVarFluxGrid(t) * mComponentAvailabilityTS[t];
        mExpFlux[t] = flux;

        addConstraint(flux <= maxFluxAbs * mExpState[t], "StateGrid", t);
        addConstraint(flux <= mExpSizeMax, "MaxGFx", t);

        if (mAddVariableMaxFlow)
            addConstraint(flux <= mGridVariableMaxFlow[t], "MPow", t);

        if (mSeasonalCosts && t >= mMilpNpdt)
            addConstraint(flux == 0, "seasonalMute", t);
    }

    mEnergyPrice = (sens < 0 ? mSellPrice : mBuyPrice);
    mTemporalPrice.resize(horizon);
    const double mult = mPriceMultiplier;

    if (mUseConstantPrice)
    {
        cInfo() << "Using grid constant price instead of the one defined in the energy vector.";
        const double constantPrice = (sens > 0 ? mConstantBuyPrice : mConstantSellPrice) * mult;

        for (uint64_t t = 0; t < horizon; ++t)
            mTemporalPrice[t] = constantPrice;
    }
    else
    {
        for (uint64_t t = 0; t < horizon; ++t)
            mTemporalPrice[t] = mEnergyPrice[t] * mult;
    }

    if (mSeasonalPrevisions)
    {
        cInfo() << "Overwriting grid price by seasonal price on the forecasting part of the time horizon.";

        for (uint64_t t = mTimeStepBeginForecast; t < horizon; ++t)
            mTemporalPrice[t] = mBuyPriceSeasonal[t] * mult;
    }

    if (mSeasonalCostsFree)
    {
        cInfo() << "Overwriting grid price by zero on the LP part of the time horizon.";

        for (uint64_t t = mMilpNpdt; t < horizon; ++t)
            mTemporalPrice[t] = 0.0;
    }

    for (uint64_t t = 0; t < horizon; ++t)
        mExpGridPrice[t] += mTemporalPrice[t];

    // ---------------------------------------------------------
    // External modeler
    // ---------------------------------------------------------
    if (auto* ext = mModel->getExternalModeler())
    {
        const std::string compoName = SubModel::parent()->objectName();

        ext->addText("");
        ext->addComment("+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++");
        ext->addComment(" add new GridFree component");
        ext->addComment("+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++");

        ModelerParams params;
        params.addParam(compoName + "_p_EnergyPrice", "k", mTemporalPrice);
        params.addParam(compoName + "_p_MaxFlow", maxFluxAbs);
        params.addParam(compoName + "_p_MinFlow", minFluxAbs);
        params.addParam(compoName + "_p_Direction", sens);
        ext->setModelData(params);

        ext->addText("$\t setLocal CompoName " + compoName);
        ext->addText("");

        ModelerParams options;
        ext->addModelFromFile("%gamslib%/Grid/Grid.gms", "%CompoName%", options);
    }
}

//---------------------------------------------------------------------------
void GridFree::computeEconomicalContribution() {
    TechnicalSubModel::computeEconomicalContribution()  ;

    for (uint64_t t = 0; t < mHorizon; t++) {
        if (!(mSeasonalCostsFree && t>=mMilpNpdt)) { // do not compute variable costs if mSeasonalCostsFree is True and t >= mMilpNpdt (LP part of the horizon)
            mExpVariableCosts[t] += Sens() * TimeStep(t) * mExpFlux[t] * mTemporalPrice[t];
        }
    }
}
