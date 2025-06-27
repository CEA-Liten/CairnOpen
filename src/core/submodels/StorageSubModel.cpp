#include "StorageSubModel.h"

StorageSubModel::StorageSubModel(QObject* aParent) :
    TechnicalSubModel(aParent)//,
    //mPortFlow(nullptr)
{
}

StorageSubModel::~StorageSubModel()
{
}

void StorageSubModel::computeDefaultIndicators(const double* optSol)
{
    TechnicalSubModel::computeDefaultIndicators(optSol);

    mChargingTime.at(0) = 0.;
    mDischargingTime.at(0) = 0.;

    std::vector<double> meanValue = std::vector<double>(2, 0.);
    mMaxRunningTime.at(0) = 0;
    for (uint64_t t = 0; t < mHorizon; ++t) mMaxRunningTime.at(0) += TimeStep(t) * mParentCompo->ExtrapolationFactor(); // fichier plan: extrapolé
    mMaxRunningTime.at(1) += mNpdtPast * TimeStep(0); // fichier hist, cumulé 

    //Save optimal size from the current cycle
    if (mOptimalSize.size() > 0)
        mOptimalSizeAllCycles.push_back(mOptimalSize.at(0));

    MilpPort* port;
    QListIterator<MilpPort*> iport(mListPort);
    bool firstPort = true;
    double memChargingTime, memDischargingTime;
    memChargingTime = memDischargingTime = 0.;
    // Calcul running time
    while (iport.hasNext())
    {
        port = iport.next();
        QString varName = port->Variable();
        QString sens;
        if (mParentCompo->Sens() > 0) sens = "source";
        else sens = "load";

        if (port->VarType() == "vector")
        {
            MIPModeler::MIPExpression1D variable = *getMIPExpression1D(varName);
            if (port->Direction() == GS::KPROD() && varName == "Flow") {
                if (firstPort) {
                    memChargingTime = mChargingTime.at(0);
                    memDischargingTime = mDischargingTime.at(0);
                    computeTime(true, mHorizon, mNpdtPast, variable, optSol, mChargingTime.at(0), mDischargingTime.at(0));
                    computeTime(false, *mptrTimeshift, mNpdtPast, variable, optSol, mChargingTime.at(1), mDischargingTime.at(1));
                    if (mChargingTime.at(0) > memChargingTime && mDischargingTime.at(0) > memDischargingTime) {
                        firstPort = false;
                    }
                }
            }
            else if (port->Direction() == GS::KPROD()) {
                if (firstPort) {
                    memChargingTime = mChargingTime.at(0);
                    memDischargingTime = mDischargingTime.at(0);
                    computeTime(true, mHorizon, mNpdtPast, variable, optSol, mDischargingTime.at(0));
                    computeTime(false, *mptrTimeshift, mNpdtPast, variable, optSol, mDischargingTime.at(1));
                    if (mChargingTime.at(0) > memChargingTime && mDischargingTime.at(0) > memDischargingTime) {
                        firstPort = false;
                    }
                }
            }
            else if (port->Direction() == GS::KCONS()) {
                if (firstPort) {
                    memChargingTime = mChargingTime.at(0);
                    memDischargingTime = mDischargingTime.at(0);
                    computeTime(true, mHorizon, mNpdtPast, variable, optSol, mChargingTime.at(0));
                    computeTime(false, *mptrTimeshift, mNpdtPast, variable, optSol, mChargingTime.at(1));
                    if (mChargingTime.at(0) > memChargingTime && mDischargingTime.at(0) > memDischargingTime) {
                        firstPort = false;
                    }
                }
            }
        }
    }
    iport = mListPort;
    while (iport.hasNext())
    {
        port = iport.next();
        QString portName = port->Name();
        QString varName = port->Variable();
        QString storageName = port->ptrEnergyVector()->StorageName();
        QString storageUnit = port->ptrEnergyVector()->StorageUnit();
        QString fluxUnit = port->ptrEnergyVector()->FluxUnit();
        QString fluxName = port->ptrEnergyVector()->FluxName();
        bool isHeatCarrier = port->ptrEnergyVector()->isHeatCarrier();

        if (isHeatCarrier)
        {
            fluxName = fluxName + " at " + QString::number(port->ptrEnergyVector()->Potential()) + " degC";
            storageName = storageName + " at " + QString::number(port->ptrEnergyVector()->Potential()) + " degC";
        }

        double aPort = port->VarCoeff();
        double bPort = port->VarOffset();

        if (port->VarType() == "vector")
        {
            MIPModeler::MIPExpression1D variable = *getMIPExpression1D(varName);

            if (port->Direction() == GS::KPROD())
            {
                mChargedEnergyMap[portName].at(0) = mNLevChargedEnergyMap[portName].at(0) = mChargedMeanMap[portName].at(0) = 0.;
                mDischargedEnergyMap[portName].at(0) = mNLevDischargedEnergyMap[portName].at(0) = mDischargedMeanMap[portName].at(0) = 0.;
                computeProduction(true, mHorizon, mNpdtPast, variable, optSol, aPort, bPort, mChargedEnergyMap[portName].at(0), mDischargedEnergyMap[portName].at(0));
                computeProduction(false, *mptrTimeshift, mNpdtPast, variable, optSol, aPort, bPort, mChargedEnergyMap[portName].at(1), mDischargedEnergyMap[portName].at(1));
                computeLvlProduction(true, mHorizon, mNpdtPast, variable, optSol, aPort, bPort, mNLevChargedEnergyMap[portName].at(0), mNLevDischargedEnergyMap[portName].at(0));
                computeLvlProduction(false, *mptrTimeshift, mNpdtPast, variable, optSol, aPort, bPort, mNLevChargedEnergyMap[portName].at(1), mNLevDischargedEnergyMap[portName].at(1));
                for (int i = 0; i < 2; ++i) if (mChargingTime.at(i) > 1.e-20) mChargedMeanMap[portName].at(i) = mChargedEnergyMap[portName].at(i) / mChargingTime.at(i);
                for (int i = 0; i < 2; ++i) if (mDischargingTime.at(i) > 1.e-20) mDischargedMeanMap[portName].at(i) = mDischargedEnergyMap[portName].at(i) / mDischargingTime.at(i);
                for (int i = 0; i < 2; ++i) if (mOptimalSize.at(i) > 1.e-20) mNbCylesMap[portName].at(i) = std::ceil((mDischargedEnergyMap[portName].at(i) - mChargedEnergyMap[portName].at(i)) / 2. / mOptimalSize.at(i));
            }
            else if (port->Direction() == GS::KCONS())
            {
                computeConsumption(true, mHorizon, mNpdtPast, variable, optSol, aPort, bPort, mConsumptionMap[portName].at(0));
                computeConsumption(false, *mptrTimeshift, mNpdtPast, variable, optSol, aPort, bPort, mConsumptionMap[portName].at(1));
                computeLvlConsumption(true, mHorizon, mNpdtPast, variable, optSol, aPort, bPort, mConsLvlTotMap[portName].at(0));
                computeLvlConsumption(false, *mptrTimeshift, mNpdtPast, variable, optSol, aPort, bPort, mConsLvlTotMap[portName].at(1));
                for (int i = 0; i < 2; ++i) if (mChargingTime.at(i) > 1.e-20) mConsMeanMap[portName].at(i) = mConsumptionMap[portName].at(i) / mChargingTime.at(i);
            }
            else if (port->Direction() == GS::KDATA())
            {
                computeProduction(true, mHorizon, mNpdtPast, variable, optSol, aPort, bPort, mExpEchData[portName].at(0));
                computeProduction(false, *mptrTimeshift, mNpdtPast, variable, optSol, aPort, bPort, mExpEchData[portName].at(1));
            }
        }
    }
}

int StorageSubModel::checkBusFlowBalanceVarName(MilpPort* port, int& inumberchange, QString& varUseCheck)
{
    // do nothing
    return 0;
}