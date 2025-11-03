#include "SourceLoadSubModel.h"

SourceLoadSubModel::SourceLoadSubModel(CairnObject* aParent) :
    TechnicalSubModel(aParent)//,
    //mPortFlow(nullptr)
{
}

SourceLoadSubModel::~SourceLoadSubModel()
{
}

void SourceLoadSubModel::setTimeData()
{
    TechnicalSubModel::setTimeData();
}

void SourceLoadSubModel::computeDefaultIndicators(const double* optSol)
{
    TechnicalSubModel::computeDefaultIndicators(optSol);
    std::vector<double> meanValue = std::vector<double>(2, 0.);
    mMaxRunningTime.at(0) = 0;
    for (uint64_t t = 0; t < mHorizon; ++t) mMaxRunningTime.at(0) += TimeStep(t) * mParentCompo->ExtrapolationFactor(); // fichier plan: extrapolé
    mMaxRunningTime.at(1) += mNpdtPast * TimeStep(0); // fichier hist, cumulé 

    //Save optimal size from the current cycle
    mOptimalSizeAllCycles.push_back(mOptimalSize.at(0));
    
    bool firstPort = true;
    double memRunningTime = 0.;
    // Calcul running time
    for (auto& port : mListPort) {    
        std::string varName = port->Variable();
        std::string sens;
        if (Sens() > 0) sens = "source";
        else sens = "load";

        if (port->VarType() == "vector")
        {
            MIPModeler::MIPExpression1D variable = *getMIPExpression1D(varName);
            if (port->Direction() == GS::KPROD()) {
                if (firstPort && sens == "source") {
                    memRunningTime = mRunningTime.at(0);
                    computeTime(true, mHorizon, mNpdtPast, variable, optSol, mRunningTime.at(0));
                    computeTime(false, *mptrTimeshift, mNpdtPast, variable, optSol, mRunningTime.at(1));
                    if (mRunningTime.at(0) > memRunningTime) {
                        firstPort = false;
                    }
                }
            }
            else if (port->Direction() == GS::KCONS()) {
                if (firstPort && sens == "load") {
                    memRunningTime = mRunningTime.at(0);
                    computeTime(true, mHorizon, mNpdtPast, variable, optSol, mRunningTime.at(0));
                    computeTime(false, *mptrTimeshift, mNpdtPast, variable, optSol, mRunningTime.at(1));
                    if (mRunningTime.at(0) > memRunningTime) {
                        firstPort = false;
                    }
                }
            }
        }
    }

    for (auto& port : mListPort) {    
        std::string portName = port->Name();
        std::string varName = port->Variable();
        std::string storageName = port->getCarrier()->StorageName();
        std::string storageUnit = port->getCarrier()->StorageUnit();
        std::string fluxUnit = port->getCarrier()->FluxUnit();
        std::string fluxName = port->getCarrier()->FluxName();
        bool isHeatCarrier = port->getCarrier()->isHeatCarrier();

        double aPort = port->VarCoeff();
        double bPort = port->VarOffset();

        std::string sens;
        if (Sens() > 0) sens = "source";
        else sens = "load";

        if (port->VarType() == "vector")
        {
            MIPModeler::MIPExpression1D variable = *getMIPExpression1D(varName);
            if (port->Direction() == GS::KPROD() && sens == "source")
            {
                mProductionMap[portName].at(0) = mProdLvlTotMap[portName].at(0) = mProdMeanMap[portName].at(0) = 0.;
                computeProduction(true, mHorizon, mNpdtPast, variable, optSol, aPort, bPort, mProductionMap[portName].at(0));
                computeProduction(false, *mptrTimeshift, mNpdtPast, variable, optSol, aPort, bPort, mProductionMap[portName].at(1));
                computeLvlProduction(true, mHorizon, mNpdtPast, variable, optSol, aPort, bPort, mProdLvlTotMap[portName].at(0));
                computeLvlProduction(false, *mptrTimeshift, mNpdtPast, variable, optSol, aPort, bPort, mProdLvlTotMap[portName].at(1));
                for (int i = 0; i < 2; ++i) if (mRunningTime.at(i) > 1.e-20) mProdMeanMap[portName].at(i) = mProductionMap[portName].at(i) / mRunningTime.at(i);
            }
            else if (port->Direction() == GS::KCONS() && sens == "load")
            {
                mConsumptionMap[portName].at(0) = mConsLvlTotMap[portName].at(0) = mConsumptionMap[portName].at(0) = 0.;
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

void SourceLoadSubModel::computeSourceENRModelIndicators(const double* optSol)
{
    TechnicalSubModel::computeDefaultIndicators(optSol);

    std::vector<double> meanValue = std::vector<double>(2, 0.);
    mMaxRunningTime.at(0) = 0;
    for (uint64_t t = 0; t < mHorizon; ++t) mMaxRunningTime.at(0) += TimeStep(t) * mParentCompo->ExtrapolationFactor(); // fichier plan: extrapolé
    mMaxRunningTime.at(1) += mNpdtPast * TimeStep(0); // fichier hist, cumulé 

    //Save optimal size from the current cycle
    if (mOptimalSize.size() > 0)
        mOptimalSizeAllCycles.push_back(mOptimalSize.at(0));
    
    bool firstPort = true;
    double memRunningTime = 0.;
    // Calcul running time
    for (auto& port : mListPort) {    
        std::string varName = port->Variable();
        std::string sens;
        if (Sens() > 0) sens = "source";
        else sens = "load";

        if (port->VarType() == "vector")
        {
            MIPModeler::MIPExpression1D variable = *getMIPExpression1D(varName);
            if (port->Direction() == GS::KPROD()) {
                if (firstPort && sens == "source") {
                    memRunningTime = mRunningTime.at(0);
                    computeTime(true, mHorizon, mNpdtPast, variable, optSol, mRunningTime.at(0));
                    computeTime(false, *mptrTimeshift, mNpdtPast, variable, optSol, mRunningTime.at(1));
                    if (mRunningTime.at(0) > memRunningTime) {
                        firstPort = false;
                    }
                }
            }
            else if (port->Direction() == GS::KCONS()) {
                if (firstPort && sens == "load") {
                    memRunningTime = mRunningTime.at(0);
                    computeTime(true, mHorizon, mNpdtPast, variable, optSol, mRunningTime.at(0));
                    computeTime(false, *mptrTimeshift, mNpdtPast, variable, optSol, mRunningTime.at(1));
                    if (mRunningTime.at(0) > memRunningTime) {
                        firstPort = false;
                    }
                }
            }
        }
    }

    for (auto& port : mListPort) {    
        std::string varName = port->Variable();
        std::string storageName = port->getCarrier()->StorageName();
        std::string storageUnit = port->getCarrier()->StorageUnit();
        std::string fluxUnit = port->getCarrier()->FluxUnit();
        std::string fluxName = port->getCarrier()->FluxName();
        bool isHeatCarrier = port->getCarrier()->isHeatCarrier();

        double aPort = port->VarCoeff();
        double bPort = port->VarOffset();

        if (port->VarType() == "vector")
        {
            MIPModeler::MIPExpression1D variable = *getMIPExpression1D(varName);
            if (port->Direction() == GS::KPROD())
            {
                mProductionMap[varName].at(0) = mProdLvlTotMap[varName].at(0) = mProdMeanMap[varName].at(0) = 0.;
                computeProduction(true, mHorizon, mNpdtPast, variable, optSol, aPort, bPort, mProductionMap[varName].at(0));
                computeProduction(false, *mptrTimeshift, mNpdtPast, variable, optSol, aPort, bPort, mProductionMap[varName].at(1));
                mProductionMap[varName].at(0) *= mOptimalSize.at(0);
                mProductionMap[varName].at(1) *= mOptimalSize.at(1);
                computeLvlProduction(true, mHorizon, mNpdtPast, variable, optSol, aPort, bPort, mProdLvlTotMap[varName].at(0));
                computeLvlProduction(false, *mptrTimeshift, mNpdtPast, variable, optSol, aPort, bPort, mProdLvlTotMap[varName].at(1));
                mProdLvlTotMap[varName].at(0) *= mOptimalSize.at(0);
                mProdLvlTotMap[varName].at(1) *= mOptimalSize.at(1);
                for (int i = 0; i < 2; ++i) if (mRunningTime.at(i) > 1.e-20) mProdMeanMap[varName].at(i) = mProductionMap[varName].at(i) / mRunningTime.at(i);
            }
            else if (port->Direction() == GS::KDATA())
            {
                computeProduction(true, mHorizon, mNpdtPast, variable, optSol, aPort, bPort, mExpEchData[varName].at(0));
                computeProduction(false, *mptrTimeshift, mNpdtPast, variable, optSol, aPort, bPort, mExpEchData[varName].at(1));
            }
        }
    }
}

double SourceLoadSubModel::Sens()
{
    std::string defaultPortDirection = (mSourceLoadDefaultPort->Direction());
    if (CairnUtils::upperCase(defaultPortDirection) == KCONS())
    {
        return -1.0; //"Load/Sink" 
    }
    else //if (CairnUtils::upperCase(defaultPortDirection) == KPROD())
    {
        return +1.0; //"Source" (includes port "DATAEXCHANGE")
    }
    //else {
    //    //return 0.0;
    //    Cairn_Exception error("Invalid direction " + defaultPortDirection +  " of the default port of component " + Name().toStdString() + ". INPUT (Sink) or OUTPUT (Source) is expected.", -1);
    //    throw& error;
    //}
}
