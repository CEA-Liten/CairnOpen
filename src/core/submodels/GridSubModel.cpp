#include "GridSubModel.h"

GridSubModel::GridSubModel(CairnObject* aParent) :
TechnicalSubModel(aParent) ,
mAddVariableMaxFlow(false),
mMaxFlux(1.e4),
mMinFlux(0.)
{ 
}

GridSubModel::~GridSubModel() { }

void GridSubModel::setTimeData()
{
    TechnicalSubModel::setTimeData();
    mEnergyPrice.resize(mHorizon);
    mSellPrice.resize(mHorizon);
    mBuyPrice.resize(mHorizon);
    mBuyPriceSeasonal.resize(mHorizon);
    mGridVariableMaxFlow.resize(mHorizon);
}

void GridSubModel::computeInitialData() {
    setMaxValue(mMaxFlux);
    setMinValue(mMinSize); 

    mAddStateVariable = true; /* always add state constraints */
}

void GridSubModel::computeAllIndicators(const double* optSol)
{
    TechnicalSubModel::computeDefaultIndicators(optSol);

    std::vector<double> meanValue = std::vector<double>(2, 0.);
    mMaxRunningTime.at(0) = 0;
    for (uint64_t t = 0; t < mHorizon; ++t) mMaxRunningTime.at(0) += TimeStep(t) * mParentCompo->ExtrapolationFactor(); // fichier plan: extrapolé
    mMaxRunningTime.at(1) += mNpdtPast * TimeStep(0); // fichier hist, cumulé 

    //Save optimal size from the current cycle //Is it needed for grid?
    if (mOptimalSize.size() > 0) {
        mOptimalSizeAllCycles.push_back(mOptimalSize.at(0));
    }

    bool firstPort = true;
    for (const auto &port :mListPort) {    
        const std::string portId = port->ID();
        const double aPort = port->VarCoeff();
        const  double bPort = port->VarOffset();

        const MIPModeler::MIPExpression1D* ptrExp1D = getMIPExpression1D(port->Variable());
        if (ptrExp1D) {
            if (port->Direction() != GS::KDATA()) 
            {
                if (firstPort) {
                    computeTime(true, mHorizon, mNpdtPast, *ptrExp1D, optSol, mRunningTime.at(0)); // plan
                    computeTime(false, *mptrTimeshift, mNpdtPast, *ptrExp1D, optSol, mRunningTime.at(1));
                    firstPort = false;
                }
            }
            if (port->Direction() == GS::KPROD() && Sens() > 0)
            {
                mProductionMap[portId].at(0) = mProdLvlTotMap[portId].at(0) = mProdMeanMap[portId].at(0) = 0.;
                computeProduction(true, mHorizon, mNpdtPast, *ptrExp1D, optSol, aPort, bPort, mProductionMap[portId].at(0)); // PLAN
                computeProduction(false, *mptrTimeshift, mNpdtPast, *ptrExp1D, optSol, aPort, bPort, mProductionMap[portId].at(1)); // HIST
                computeLvlProduction(true, mHorizon, mNpdtPast, *ptrExp1D, optSol, aPort, bPort, mProdLvlTotMap[portId].at(0));
                computeLvlProduction(false, *mptrTimeshift, mNpdtPast, *ptrExp1D, optSol, aPort, bPort, mProdLvlTotMap[portId].at(1));
                //computeLvlProduction(*mptrTimeshift, mNpdtPast, *ptrExp1D, optSol, aPort, bPort, mProdLvlTotMap[portId].at(1));
                for (int i = 0; i < 2; ++i) if (mRunningTime.at(i) > 1.e-20) mProdMeanMap[portId].at(i) = mProductionMap[portId].at(i) / mRunningTime.at(i);
            }
            else if (port->Direction() == GS::KCONS() && Sens() <= 0)
            {
                mConsumptionMap[portId].at(0) = mConsLvlTotMap[portId].at(0) = mConsMeanMap[portId].at(0) = 0.;
                computeConsumption(true, mHorizon, mNpdtPast, *ptrExp1D, optSol, aPort, bPort, mConsumptionMap[portId].at(0));
                computeConsumption(false, *mptrTimeshift, mNpdtPast, *ptrExp1D, optSol, aPort, bPort, mConsumptionMap[portId].at(1));
                computeLvlConsumption(true, mHorizon, mNpdtPast, *ptrExp1D, optSol, aPort, bPort, mConsLvlTotMap[portId].at(0));
                computeLvlConsumption(false, *mptrTimeshift, mNpdtPast, *ptrExp1D, optSol, aPort, bPort, mConsLvlTotMap[portId].at(1));
                for (int i = 0; i < 2; ++i) if (mRunningTime.at(i) > 1.e-20) mConsMeanMap[portId].at(i) = mConsumptionMap[portId].at(i) / mRunningTime.at(i);
            }
            else if (port->Direction() == GS::KDATA())
            {
                computeProduction(true, mHorizon, mNpdtPast, *ptrExp1D, optSol, aPort, bPort, mExpEchData[portId].at(0));
                computeProduction(false, *mptrTimeshift, mNpdtPast, *ptrExp1D, optSol, aPort, bPort, mExpEchData[portId].at(1));
            }
        }
    }
}

double GridSubModel::Sens() 
{
    std::string defaultPortDirection = (mPortGridFlow->Direction());
    if (CairnUtils::toUpper(defaultPortDirection) == KCONS())
    {
        return -1.0; //"InjectToGrid" 
    }
    else //if (CairnUtils::toUpper(defaultPortDirection) == KPROD().toStdString())
    {
        return +1.0; //"ExtractFromGrid" (includes port "DATAEXCHANGE")
    }
    //else {
    //    //return 0.0;
    //    Cairn_Exception error("Invalid direction " + defaultPortDirection + " of the default port of component " + Name().toStdString() + ". INPUT (InjectToGrid) or OUTPUT (ExtractFromGrid) is expected.", -1);
    //    throw& error;
    //}
}

std::string GridSubModel::Direction()  
{
    if (Sens() < 0) {
        return "injection"; //default port is INPUT
    }
    else {
        return "extraction"; //default port is OUTPUT or DATAEXCHANGE
    }
}
