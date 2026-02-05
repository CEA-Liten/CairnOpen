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
    if (mOptimalSize.size() > 0) {
        mOptimalSizeAllCycles.push_back(mOptimalSize.at(0));
    }
    
    // Calculate running time
    bool firstPort = true;
    double memRunningTime = 0.;
    for (const auto& port : mListPort) {    
        const MIPModeler::MIPExpression1D* ptrExp1D = getMIPExpression1D(port->Variable());
        if (ptrExp1D) {
            if (port->Direction() == GS::KPROD()) 
            {
                if (firstPort && Sens() > 0) {
                    memRunningTime = mRunningTime.at(0);
                    computeTime(true, mHorizon, mNpdtPast, *ptrExp1D, optSol, mRunningTime.at(0));
                    computeTime(false, *mptrTimeshift, mNpdtPast, *ptrExp1D, optSol, mRunningTime.at(1));
                    if (mRunningTime.at(0) > memRunningTime) {
                        firstPort = false;
                    }
                }
            }
            else if (port->Direction() == GS::KCONS()) 
            {
                if (firstPort && Sens() <= 0) {
                    memRunningTime = mRunningTime.at(0);
                    computeTime(true, mHorizon, mNpdtPast, *ptrExp1D, optSol, mRunningTime.at(0));
                    computeTime(false, *mptrTimeshift, mNpdtPast, *ptrExp1D, optSol, mRunningTime.at(1));
                    if (mRunningTime.at(0) > memRunningTime) {
                        firstPort = false;
                    }
                }
            }
        }
    }

    for (const auto& port : mListPort) {
        const std::string portId = port->ID();
        const double aPort = port->VarCoeff();
        const double bPort = port->VarOffset();

        const MIPModeler::MIPExpression1D* ptrExp1D = getMIPExpression1D(port->Variable());
        if (ptrExp1D) {
            if (port->Direction() == GS::KPROD() && Sens() > 0)  
            {
                mProductionMap[portId].at(0) = mProdLvlTotMap[portId].at(0) = mProdMeanMap[portId].at(0) = 0.;
                computeProduction(true, mHorizon, mNpdtPast, *ptrExp1D, optSol, aPort, bPort, mProductionMap[portId].at(0));
                computeProduction(false, *mptrTimeshift, mNpdtPast, *ptrExp1D, optSol, aPort, bPort, mProductionMap[portId].at(1));
                computeLvlProduction(true, mHorizon, mNpdtPast, *ptrExp1D, optSol, aPort, bPort, mProdLvlTotMap[portId].at(0));
                computeLvlProduction(false, *mptrTimeshift, mNpdtPast, *ptrExp1D, optSol, aPort, bPort, mProdLvlTotMap[portId].at(1));
                for (int i = 0; i < 2; ++i) if (mRunningTime.at(i) > 1.e-20) mProdMeanMap[portId].at(i) = mProductionMap[portId].at(i) / mRunningTime.at(i);
            }
            else if (port->Direction() == GS::KCONS() && Sens() <= 0) 
            {
                mConsumptionMap[portId].at(0) = mConsLvlTotMap[portId].at(0) = mConsumptionMap[portId].at(0) = 0.;
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
        const MIPModeler::MIPExpression1D* ptrExp1D = getMIPExpression1D(port->Variable());
        if (ptrExp1D) {
            if (port->Direction() == GS::KPROD()) {
                if (firstPort && Sens() > 0) {
                    memRunningTime = mRunningTime.at(0);
                    computeTime(true, mHorizon, mNpdtPast, *ptrExp1D, optSol, mRunningTime.at(0));
                    computeTime(false, *mptrTimeshift, mNpdtPast, *ptrExp1D, optSol, mRunningTime.at(1));
                    if (mRunningTime.at(0) > memRunningTime) {
                        firstPort = false;
                    }
                }
            }
            else if (port->Direction() == GS::KCONS()) {
                if (firstPort && Sens() <= 0) {
                    memRunningTime = mRunningTime.at(0);
                    computeTime(true, mHorizon, mNpdtPast, *ptrExp1D, optSol, mRunningTime.at(0));
                    computeTime(false, *mptrTimeshift, mNpdtPast, *ptrExp1D, optSol, mRunningTime.at(1));
                    if (mRunningTime.at(0) > memRunningTime) {
                        firstPort = false;
                    }
                }
            }
        }
    }

    for (auto& port : mListPort) {   
        const std::string portId = port->ID();
        const double aPort = port->VarCoeff();
        const double bPort = port->VarOffset();
        const MIPModeler::MIPExpression1D* ptrExp1D = getMIPExpression1D(port->Variable());
        if (ptrExp1D) {
            if (port->Direction() == GS::KPROD()) {
                mProductionMap[portId].at(0) = mProdLvlTotMap[portId].at(0) = mProdMeanMap[portId].at(0) = 0.;
                computeProduction(true, mHorizon, mNpdtPast, *ptrExp1D, optSol, aPort, bPort, mProductionMap[portId].at(0));
                computeProduction(false, *mptrTimeshift, mNpdtPast, *ptrExp1D, optSol, aPort, bPort, mProductionMap[portId].at(1));
                mProductionMap[portId].at(0) *= mOptimalSize.at(0);
                mProductionMap[portId].at(1) *= mOptimalSize.at(1);
                computeLvlProduction(true, mHorizon, mNpdtPast, *ptrExp1D, optSol, aPort, bPort, mProdLvlTotMap[portId].at(0));
                computeLvlProduction(false, *mptrTimeshift, mNpdtPast, *ptrExp1D, optSol, aPort, bPort, mProdLvlTotMap[portId].at(1));
                mProdLvlTotMap[portId].at(0) *= mOptimalSize.at(0);
                mProdLvlTotMap[portId].at(1) *= mOptimalSize.at(1);
                for (int i = 0; i < 2; ++i) if (mRunningTime.at(i) > 1.e-20) mProdMeanMap[portId].at(i) = mProductionMap[portId].at(i) / mRunningTime.at(i);
            }
            else if (port->Direction() == GS::KDATA()) {
                computeProduction(true, mHorizon, mNpdtPast, *ptrExp1D, optSol, aPort, bPort, mExpEchData[portId].at(0));
                computeProduction(false, *mptrTimeshift, mNpdtPast, *ptrExp1D, optSol, aPort, bPort, mExpEchData[portId].at(1));
            }
        }
    }
}

double SourceLoadSubModel::Sens() 
{
    std::string defaultPortDirection = (mSourceLoadDefaultPort->Direction());
    if (CairnUtils::toUpper(defaultPortDirection) == KCONS())
    {
        return -1.0; //"Load/Sink" 
    }
    else //if (CairnUtils::toUpper(defaultPortDirection) == KPROD())
    {
        return +1.0; //"Source" (includes port "DATAEXCHANGE")
    }
    //else {
    //    //return 0.0;
    //    Cairn_Exception error("Invalid direction " + defaultPortDirection +  " of the default port of component " + Name().toStdString() + ". INPUT (Sink) or OUTPUT (Source) is expected.", -1);
    //    throw& error;
    //}
}

std::string SourceLoadSubModel::Direction() 
{
    if (Sens() < 0) {
        return "load"; //default port is INPUT
    }
    else {
        return "source"; //default port is OUTPUT or DATAEXCHANGE
    }
}
