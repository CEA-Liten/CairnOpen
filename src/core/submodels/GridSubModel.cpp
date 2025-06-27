#include "GridSubModel.h"

GridSubModel::GridSubModel(QObject* aParent) :
TechnicalSubModel(aParent) ,
mAddVariableMaxFlow(false),
mMaxFlux(1.e4),
mMinFlux(0.)
{ }

GridSubModel::~GridSubModel() { }

void GridSubModel::setTimeData()
{
    SubModel::setTimeData();
    mEnergyPrice.resize(mHorizon);
    mSellPrice.resize(mHorizon);
    mBuyPrice.resize(mHorizon);
    mBuyPriceSeasonal.resize(mHorizon);
    mGridUse.resize(mHorizon);
    mGridVariableMaxFlow.resize(mHorizon);
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

    MilpPort* port;
    QListIterator<MilpPort*> iport(mListPort);
    bool firstPort = true;
    while (iport.hasNext())
    {
        port = iport.next();
        QString portName = port->Name();
        QString varName = port->Variable();
        QString storageName = port->ptrEnergyVector()->StorageName();
        QString storageUnit = port->ptrEnergyVector()->StorageUnit();
        QString fluxUnit = port->ptrEnergyVector()->FluxUnit();
        QString fluxName = port->ptrEnergyVector()->FluxName();
        bool isHeatCarrier = false;
        if (port->ptrEnergyVector()) {
            isHeatCarrier = port->ptrEnergyVector()->isHeatCarrier();
        }

        if (isHeatCarrier)
        {
            fluxName = fluxName + " at " + QString::number(port->ptrEnergyVector()->Potential()) + " degC";
            storageName = storageName + " at " + QString::number(port->ptrEnergyVector()->Potential()) + " degC";
        }

        double aPort = port->VarCoeff();
        double bPort = port->VarOffset();

        QString sens;
        if (mParentCompo->Sens() > 0) sens = "extraction";
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

int GridSubModel::checkBusFlowBalanceVarName(MilpPort* port, int& inumberchange, QString& varUseCheck)
{
    QString varName = port->Variable();
    QString varUse = port->Direction();
    MilpComponent* pParent = (MilpComponent*)parent();
    mSens = pParent->Sens();
    // Uncomplete description -> use default definitions functions of models
    if (varName == "") {
        if (mSens > 0) port->setDirection(KPROD()); //producer for balance bus - no sign change
        else if (mSens < 0) port->setDirection(KCONS()); //consumer for balance bus - negative sign change required
        port->setVariable("GridFlow");
        qInfo() << "Use default Variable name to send from Component to BusFlowBalance bus " << (port->Variable());
    }
    if (varUse == "") {
        if (mSens > 0) port->setDirection(KPROD()); //producer for balance bus - no sign change
        else if (mSens < 0) port->setDirection(KCONS()); //consumer for balance bus - negative sign change required
        qInfo() << "Use default Use to send from Component to BusFlowBalance bus " << (port->Direction());
    }
    // user defined varname and varuse, check consistency against sink/source attribute !
    if (mSens > 0 && varUse != KPROD() && PortList().size() == 1) { port->setDirection(KPROD()); qWarning() << "Variable Use reset to PRODUCER for consistency with Source attribute "; }
    if (mSens < 0 && varUse != KCONS() && PortList().size() == 1) { port->setDirection(KCONS()); qWarning() << "Variable Use reset to CONSUMER for consistency with Source attribute "; }
    if (varUse == "") {
        if (mSens > 0) port->setDirection(KPROD()); //producer for balance bus - no sign change
        else if (mSens < 0) port->setDirection(KCONS()); //consumer for balance bus - negative sign change required
        qInfo() << "Use default Use to send from Component to BusFlowBalance bus " << (port->Direction());    
    }    
    return 0;
}