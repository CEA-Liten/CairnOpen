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
extern "C" MODELS_DECLSPEC QObject * createModel(QObject * aParent)
{
    return new GridFree(aParent);
}

//---------------------------------------------------------------------------
GridFree::GridFree(QObject* aParent) : GridSubModel(aParent),
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

void GridFree::buildModel()
{
    setExpSizeMax(mMaxFlux, "MaxFlux");

    if (mAllocate) {
        mVarFluxGrid = MIPModeler::MIPVariable1D( mHorizon, fabs(mMinFlux), fabs(mMaxFlux)); // puissance max de soutirage, mis a jour dynamiquement
        //mVarSizeMax = MIPModeler::MIPVariable0D( 0., fabs(mMaxFlux)); // puissance de dimensionnement max du composant. negative means optimization, absolute value gives max range value
        mExpFlux = MIPModeler::MIPExpression1D(mHorizon);
        mExpGridPrice = MIPModeler::MIPExpression1D(mHorizon);
    }
    else {
        closeExpression1D(mExpFlux);
        closeExpression1D(mExpGridPrice);
    }
    addVariable(mVarFluxGrid,"GFx");
    
    SubModel::addStateConstraints(mHorizon, mCondensedNpdt);

    for (uint64_t t = 0; t < mHorizon; t++) {
        mExpFlux[t] = mVarFluxGrid(t) * mGridUse[t];
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
        //qDebug() << "GridFree, mSeasonalCosts = " << mSeasonalCosts ;
        for (uint64_t t = mMilpNpdt; t < mHorizon; t++) {
            addConstraint(mExpFlux[t] == 0, "seasonalMute", t);
        }
    }

    /** Objective contribution expressed as the sum of Capex and Opex so as
     * to be able to account for actualization rate on Opex part only*/
    
    if (mSens < 0) {
        mEnergyPrice = mSellPrice;
    }
    else {
        mEnergyPrice = mBuyPrice;
    }

    // fill mTemporalPrice here because used in GAMS and computeEconomicalContribution()
    mTemporalPrice.assign(mEnergyPrice.begin(), mEnergyPrice.end());
    if (mUseConstantPrice) {
        qInfo() << "Using grid constant price instead of the one defined in the energy vector.";
        
        for (uint64_t t = 0; t < mHorizon; t++) {
            if (mSens > 0) {
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
        qInfo() << "Overwriting grid price by seasonal price on the forecasting part of the time horizon.";
        
        for (uint64_t t = mTimeStepBeginForecast; t < mHorizon; t++) {
            mTemporalPrice[t] = mBuyPriceSeasonal[t] * mPriceMultiplier;
        }
    }
    if (mSeasonalCostsFree) {
        qInfo() << "Overwriting grid price by zero on the LP part of the time horizon.";

        for (uint64_t t = mMilpNpdt; t < mHorizon; t++) {
            mTemporalPrice[t] = 0;
        }
    }

    // define grid price expression for IO
    for (uint64_t t = 0; t < mHorizon; t++) {
        mExpGridPrice[t] += mTemporalPrice[t];
    }

    /** Compute all expressions */
    computeAllContribution();

    mAllocate = false ;

    ModelerInterface* pExternalModeler = mModel->getExternalModeler();
    if (pExternalModeler != nullptr) {
        std::string compoName = SubModel::parent()->objectName().toStdString();
        pExternalModeler->addText("");
        pExternalModeler->addComment("+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++");
        pExternalModeler->addComment(" add new GridFree component");
        pExternalModeler->addComment("+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++");

        ModelerParams vParams;
        vParams.addParam(compoName + "_p_EnergyPrice", "k", mTemporalPrice);
        vParams.addParam(compoName + "_p_MaxFlow", fabs(mMaxFlux));
        vParams.addParam(compoName + "_p_MinFlow", fabs(mMinFlux));
        vParams.addParam(compoName + "_p_Direction", mSens);
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
            mExpVariableCosts[t] += mSens * TimeStep(t) * mExpFlux[t] * mTemporalPrice[t];
        }
    }

}
