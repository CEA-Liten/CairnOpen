#include "SourceLoadSubModel.h"

SourceLoadSubModel::SourceLoadSubModel(QObject* aParent) :
    TechnicalSubModel(aParent)//,
    //mPortFlow(nullptr)
{
}

SourceLoadSubModel::~SourceLoadSubModel()
{
}

void SourceLoadSubModel::computeDefaultIndicators(const double* optSol)
{
    TechnicalSubModel::computeDefaultIndicators(optSol);
    std::vector<double> meanValue = std::vector<double>(2, 0.);
    mMaxRunningTime.at(0) = 0;
    for (uint64_t t = 0; t < mHorizon; ++t) mMaxRunningTime.at(0) += TimeStep(t) * mParentCompo->ExtrapolationFactor(); // fichier plan: extrapolé
    mMaxRunningTime.at(1) += mNpdtPast * TimeStep(0); // fichier hist, cumulé 

    mOptimalSize.at(0) = mExpSizeMax.evaluate(optSol);
    double sauv = mOptimalSize.at(1);
    mOptimalSize.at(1) = max(sauv, mOptimalSize.at(0));

    //Save optimal size from the current cycle
    mOptimalSizeAllCycles.push_back(mOptimalSize.at(0));

    MilpPort* port;
    QListIterator<MilpPort*> iport(mListPort);
    bool firstPort = true;
    double memRunningTime = 0.;
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

        double aPort = port->VarCoeff();
        double bPort = port->VarOffset();

        QString sens;
        if (mParentCompo->Sens() > 0) sens = "source";
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

    MilpPort* port;
    QListIterator<MilpPort*> iport(mListPort);
    bool firstPort = true;
    double memRunningTime = 0.;
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

    iport = mListPort;
    while (iport.hasNext())
    {
        port = iport.next();
        QString varName = port->Variable();
        QString storageName = port->ptrEnergyVector()->StorageName();
        QString storageUnit = port->ptrEnergyVector()->StorageUnit();
        QString fluxUnit = port->ptrEnergyVector()->FluxUnit();
        QString fluxName = port->ptrEnergyVector()->FluxName();
        bool isHeatCarrier = port->ptrEnergyVector()->isHeatCarrier();

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

int SourceLoadSubModel::checkBusFlowBalanceVarName(MilpPort* port, int& inumberchange, QString& varUseCheck)
{    
    QString varName = port->Variable(); 
    // Uncomplete description -> use default definitions functions of models
    if (varName == "") {
        port->setVariable("SourceLoadFlow");
        qInfo() << "Use default Variable name to send from Component to BusFlowBalance bus " << (port->Variable());
    }    
    return 0;
}