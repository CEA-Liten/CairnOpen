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
    if (mOptimalSize.size() > 0)
        mOptimalSizeAllCycles.push_back(mOptimalSize.at(0));

    bool firstPort = true;
    for (auto &port :mListPort) {    
        std::string portName = port->Name();
        std::string varName = port->Variable();
        std::string storageName = port->getCarrier()->StorageName();
        std::string storageUnit = port->getCarrier()->StorageUnit();
        std::string fluxUnit = port->getCarrier()->FluxUnit();
        std::string fluxName = port->getCarrier()->FluxName();
        bool isHeatCarrier = false;
        if (port->getCarrier()) {
            isHeatCarrier = port->getCarrier()->isHeatCarrier();
        }

        if (isHeatCarrier)
        {
            fluxName = fluxName + " at " + std::to_string(port->getCarrier()->Potential()) + " degC";
            storageName = storageName + " at " + std::to_string(port->getCarrier()->Potential()) + " degC";
        }

        double aPort = port->VarCoeff();
        double bPort = port->VarOffset();

        std::string sens;
        if (Sens() > 0) sens = "extraction";
        else sens = "injection";

        if (port->VarType() == "vector")
        {
            MIPModeler::MIPExpression1D variable = *getMIPExpression1D(varName);
            if (port->Direction() != GS::KDATA()) {
                if (firstPort) {
                    computeTime(true, mHorizon, mNpdtPast, variable, optSol, mRunningTime.at(0)); // plan
                    computeTime(false, *mptrTimeshift, mNpdtPast, variable, optSol, mRunningTime.at(1));
                    firstPort = false;
                }
            }
            if (port->Direction() == GS::KPROD() && sens == "extraction")
            {
                mProductionMap[portName].at(0) = mProdLvlTotMap[portName].at(0) = mProdMeanMap[portName].at(0) = 0.;
                computeProduction(true, mHorizon, mNpdtPast, variable, optSol, aPort, bPort, mProductionMap[portName].at(0)); // PLAN
                computeProduction(false, *mptrTimeshift, mNpdtPast, variable, optSol, aPort, bPort, mProductionMap[portName].at(1)); // HIST
                computeLvlProduction(true, mHorizon, mNpdtPast, variable, optSol, aPort, bPort, mProdLvlTotMap[portName].at(0));
                computeLvlProduction(false, *mptrTimeshift, mNpdtPast, variable, optSol, aPort, bPort, mProdLvlTotMap[portName].at(1));
                //computeLvlProduction(*mptrTimeshift, mNpdtPast, variable, optSol, aPort, bPort, mProdLvlTotMap[portName].at(1));
                for (int i = 0; i < 2; ++i) if (mRunningTime.at(i) > 1.e-20) mProdMeanMap[portName].at(i) = mProductionMap[portName].at(i) / mRunningTime.at(i);
            }
            else if (port->Direction() == GS::KCONS() && sens == "injection")
            {
                mConsumptionMap[portName].at(0) = mConsLvlTotMap[portName].at(0) = mConsMeanMap[portName].at(0) = 0.;
                computeConsumption(true, mHorizon, mNpdtPast, variable, optSol, aPort, bPort, mConsumptionMap[portName].at(0));
                computeConsumption(false, *mptrTimeshift, mNpdtPast, variable, optSol, aPort, bPort, mConsumptionMap[portName].at(1));
                computeLvlConsumption(true, mHorizon, mNpdtPast, variable, optSol, aPort, bPort, mConsLvlTotMap[portName].at(0));
                computeLvlConsumption(false, *mptrTimeshift, mNpdtPast, variable, optSol, aPort, bPort, mConsLvlTotMap[portName].at(1));
                for (int i = 0; i < 2; ++i) if (mRunningTime.at(i) > 1.e-20) mConsMeanMap[portName].at(i) = mConsumptionMap[portName].at(i) / mRunningTime.at(i);
            }
            else if (port->Direction() == GS::KDATA())
            {
                computeProduction(true, mHorizon, mNpdtPast, variable, optSol, aPort, bPort, mExpEchData[portName].at(0));
                computeProduction(false, *mptrTimeshift, mNpdtPast, variable, optSol, aPort, bPort, mExpEchData[portName].at(1));
            }
        }
    }
}

double GridSubModel::Sens()
{
    std::string defaultPortDirection = (mPortGridFlow->Direction());
    if (CairnUtils::upperCase(defaultPortDirection) == KCONS())
    {
        return -1.0; //"InjectToGrid" 
    }
    else //if (CairnUtils::upperCase(defaultPortDirection) == KPROD().toStdString())
    {
        return +1.0; //"ExtractFromGrid" (includes port "DATAEXCHANGE")
    }
    //else {
    //    //return 0.0;
    //    Cairn_Exception error("Invalid direction " + defaultPortDirection + " of the default port of component " + Name().toStdString() + ". INPUT (InjectToGrid) or OUTPUT (ExtractFromGrid) is expected.", -1);
    //    throw& error;
    //}
}