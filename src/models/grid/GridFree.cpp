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
        to over-write the default value provided in declareDefaultModelConfigurationParameters() 

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
    addVariable(mVarFluxGrid,"GFx", fabs(mMinFlux), fabs(mMaxFlux));
    
    for (uint64_t t = 0; t < mHorizon; t++) {
        mExpFlux[t] = mVarFluxGrid(t) * mComponentAvailabilityTS[t];
    }

    for (uint64_t t = 0; t < mHorizon; t++) {
        addConstraint(mExpFlux[t] <= mMaxFlux * mExpState[t], "StateGrid", t);
    }

    // 2 constraints: 1 for maximum value, the other one if Electrical power Imposed (constant or profile)
    for (uint64_t t = 0; t < mHorizon; t++)
        addConstraint(mExpFlux[t] <= mExpSizeMax,"MaxGFx",t) ;

    if (mAddVariableMaxFlow) {
        for (uint64_t t = 0; t < mHorizon; t++) {
            addConstraint(mExpFlux[t] <= mGridVariableMaxFlow[t], "MPow", t);
        }
    }

    // Muting last time steps if pre-computed seasonalCosts model
    if (mSeasonalCosts) {
        for (uint64_t t = mMilpNpdt; t < mHorizon; t++) {
            addConstraint(mExpFlux[t] == 0, "seasonalMute", t);
        }
    }

    if (Sens() < 0) {
        mEnergyPrice = mSellPrice;
    }
    else {
        mEnergyPrice = mBuyPrice;
    }

    // fill mTemporalPrice here because used in GAMS and computeEconomicalContribution()
    mTemporalPrice.assign(mEnergyPrice.begin(), mEnergyPrice.end());
    if (mUseConstantPrice) {
        cInfo() << "Using grid constant price instead of the one defined in the energy vector.";
        for (uint64_t t = 0; t < mHorizon; t++) {
            if (Sens() > 0) {
                mTemporalPrice[t] = mConstantBuyPrice * mPriceMultiplier;
            } else {
                mTemporalPrice[t] = mConstantSellPrice * mPriceMultiplier;
            }
        }
    }
    else {
        for (uint64_t t = 0; t < mHorizon; t++) {
            mTemporalPrice[t] = mEnergyPrice[t] * mPriceMultiplier;
        }
    }
    if (mSeasonalPrevisions) {
        cInfo() << "Overwriting grid price by seasonal price on the forecasting part of the time horizon.";
        
        for (uint64_t t = mTimeStepBeginForecast; t < mHorizon; t++) {
            mTemporalPrice[t] = mBuyPriceSeasonal[t] * mPriceMultiplier;
        }
    }
    if (mSeasonalCostsFree) {
        cInfo() << "Overwriting grid price by zero on the LP part of the time horizon.";

        for (uint64_t t = mMilpNpdt; t < mHorizon; t++) {
            mTemporalPrice[t] = 0;
        }
    }

    // define grid price expression for IO
    for (uint64_t t = 0; t < mHorizon; t++) {
        mExpGridPrice[t] += mTemporalPrice[t];
    }

    ModelerInterface* pExternalModeler = mModel->getExternalModeler();
    if (pExternalModeler != nullptr) {
        std::string compoName = SubModel::parent()->objectName();
        pExternalModeler->addText("");
        pExternalModeler->addComment("+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++");
        pExternalModeler->addComment(" add new GridFree component");
        pExternalModeler->addComment("+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++");

        ModelerParams vParams;
        vParams.addParam(compoName + "_p_EnergyPrice", "k", mTemporalPrice);
        vParams.addParam(compoName + "_p_MaxFlow", fabs(mMaxFlux));
        vParams.addParam(compoName + "_p_MinFlow", fabs(mMinFlux));
        vParams.addParam(compoName + "_p_Direction", Sens());
        pExternalModeler->setModelData(vParams);


        pExternalModeler->addText("$\t setLocal CompoName " + compoName);
        pExternalModeler->addText("");
        ModelerParams vOptions;
        pExternalModeler->addModelFromFile("%gamslib%/Grid/Grid.gms", "%CompoName%", vOptions);
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
