#include "StorageSubModel.h"

StorageSubModel::StorageSubModel(CairnObject* aParent) :
    TechnicalSubModel(aParent)//,
    //mPortFlow(nullptr)
{
}

StorageSubModel::~StorageSubModel()
{
}

void StorageSubModel::setTimeData()
{
    TechnicalSubModel::setTimeData();
}

void StorageSubModel::computeAllIndicators(const double* optSol)
{
    TechnicalSubModel::computeAllIndicators(optSol);

    mChargingTime.at(0) = 0.;
    mDischargingTime.at(0) = 0.;

    auto* compo = parentComponent();
    const double ExtrapolationFactor = compo ? compo->ExtrapolationFactor() : 1.0;

    std::vector<double> meanValue = std::vector<double>(2, 0.);
    mMaxRunningTime.at(0) = 0;
    for (uint64_t t = 0; t < mHorizon; ++t) 
        mMaxRunningTime.at(0) += TimeStep(t) * ExtrapolationFactor; // fichier plan: extrapolé

    mMaxRunningTime.at(1) += mNpdtPast * TimeStep(0); // fichier hist, cumulé 

    //Save optimal size from the current cycle
    if (mOptimalSize.size() > 0) {
        mOptimalSizeAllCycles.push_back(mOptimalSize.at(0));
    }
    
    // Calculate running time
    bool firstPort = true;
    double memChargingTime, memDischargingTime;
    memChargingTime = memDischargingTime = 0.;
    for (const auto& port : mListPort) {    
        const std::string variable = port->Variable();
        const MIPModeler::MIPExpression1D* ptrExp1D = getMIPExpression1D(variable);
        if (ptrExp1D) {
            if (port->Direction() == GS::KPROD() && variable == "Flow")
            {
                if (firstPort) {
                    memChargingTime = mChargingTime.at(0);
                    memDischargingTime = mDischargingTime.at(0);
                    computeTime(true, mHorizon, *ptrExp1D, optSol, mChargingTime.at(0), mDischargingTime.at(0));
                    computeTime(false, *mptrTimeshift, *ptrExp1D, optSol, mChargingTime.at(1), mDischargingTime.at(1));
                    if (mChargingTime.at(0) > memChargingTime && mDischargingTime.at(0) > memDischargingTime) {
                        firstPort = false;
                    }
                }
            }
            else if (port->Direction() == GS::KPROD()) 
            {
                if (firstPort) {
                    memChargingTime = mChargingTime.at(0);
                    memDischargingTime = mDischargingTime.at(0);
                    computeTime(true, mHorizon, *ptrExp1D, optSol, mDischargingTime.at(0));
                    computeTime(false, *mptrTimeshift, *ptrExp1D, optSol, mDischargingTime.at(1));
                    if (mChargingTime.at(0) > memChargingTime && mDischargingTime.at(0) > memDischargingTime) {
                        firstPort = false;
                    }
                }
            }
            else if (port->Direction() == GS::KCONS()) 
            {
                if (firstPort) {
                    memChargingTime = mChargingTime.at(0);
                    memDischargingTime = mDischargingTime.at(0);
                    computeTime(true, mHorizon, *ptrExp1D, optSol, mChargingTime.at(0));
                    computeTime(false, *mptrTimeshift, *ptrExp1D, optSol, mChargingTime.at(1));
                    if (mChargingTime.at(0) > memChargingTime && mDischargingTime.at(0) > memDischargingTime) {
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
            if (port->Direction() == GS::KPROD())
            {
                mChargedEnergyMap[portId].at(0) = mNLevChargedEnergyMap[portId].at(0) = mChargedMeanMap[portId].at(0) = 0.;
                mDischargedEnergyMap[portId].at(0) = mNLevDischargedEnergyMap[portId].at(0) = mDischargedMeanMap[portId].at(0) = 0.;
                computeProduction(true, mHorizon, *ptrExp1D, optSol, aPort, bPort, mChargedEnergyMap[portId].at(0), mDischargedEnergyMap[portId].at(0));
                computeProduction(false, *mptrTimeshift, *ptrExp1D, optSol, aPort, bPort, mChargedEnergyMap[portId].at(1), mDischargedEnergyMap[portId].at(1));
                computeLvlProduction(true, mHorizon, *ptrExp1D, optSol, aPort, bPort, mNLevChargedEnergyMap[portId].at(0), mNLevDischargedEnergyMap[portId].at(0));
                computeLvlProduction(false, *mptrTimeshift, *ptrExp1D, optSol, aPort, bPort, mNLevChargedEnergyMap[portId].at(1), mNLevDischargedEnergyMap[portId].at(1));
                for (int i = 0; i < 2; ++i) if (mChargingTime.at(i) > 1.e-20) mChargedMeanMap[portId].at(i) = mChargedEnergyMap[portId].at(i) / mChargingTime.at(i);
                for (int i = 0; i < 2; ++i) if (mDischargingTime.at(i) > 1.e-20) mDischargedMeanMap[portId].at(i) = mDischargedEnergyMap[portId].at(i) / mDischargingTime.at(i);
                for (int i = 0; i < 2; ++i) if (mOptimalSize.at(i) > 1.e-20) mNbCylesMap[portId].at(i) = std::ceil((mDischargedEnergyMap[portId].at(i) - mChargedEnergyMap[portId].at(i)) / 2. / mOptimalSize.at(i));
            }
            else if (port->Direction() == GS::KCONS())
            {
                computeConsumption(true, mHorizon, *ptrExp1D, optSol, aPort, bPort, mConsumptionMap[portId].at(0));
                computeConsumption(false, *mptrTimeshift, *ptrExp1D, optSol, aPort, bPort, mConsumptionMap[portId].at(1));
                computeLvlConsumption(true, mHorizon, *ptrExp1D, optSol, aPort, bPort, mConsLvlTotMap[portId].at(0));
                computeLvlConsumption(false, *mptrTimeshift, *ptrExp1D, optSol, aPort, bPort, mConsLvlTotMap[portId].at(1));
                for (int i = 0; i < 2; ++i) if (mChargingTime.at(i) > 1.e-20) mConsMeanMap[portId].at(i) = mConsumptionMap[portId].at(i) / mChargingTime.at(i);
            }
            else if (port->Direction() == GS::KDATA())
            {
                computeProduction(true, mHorizon, *ptrExp1D, optSol, aPort, bPort, mExpEchData[portId].at(0));
                computeProduction(false, *mptrTimeshift, *ptrExp1D, optSol, aPort, bPort, mExpEchData[portId].at(1));
            }
        }
    }
}
