/*
 * \file    SourceLoadSubModel.cpp
 * \brief   SourceLoadSubModel is a specialized TechnicalSubModel representing a SourceLoad.
 * \version 1.0
 * \author	Ali KASSEM
 * \date    13/09/2024
 */

#include "SourceLoadSubModel.h"

// --- Construction -----------------------------------------------------------

SourceLoadSubModel::SourceLoadSubModel(CairnObject* aParent)
    : TechnicalSubModel(aParent)
{
}

// --- SubModel interface -----------------------------------------------------

void SourceLoadSubModel::declareModelInterface()
{
    if (!mSourceLoadDefaultPort)
        throw Cairn_Exception(Name() + ": default port of SourceLoad is not defined", -1);

    if (mSourceLoadDefaultPort->getCarrier() != mMainCarrier)
        throw Cairn_Exception(Name() + ": default port carrier does not match main carrier", -1);

    TechnicalSubModel::declareModelInterface();
}

void SourceLoadSubModel::declareModelIndicators()
{
    TechnicalSubModel::declareModelIndicators();

    mInputIndicators->addIndicator(
        "Component Weight",
        &mOptimalSize, &mExportIndicators,
        "Component size", pOptimalSizeUnit(), "Weight");

    if (isPriceOptimized())
    {
        mInputIndicators->addIndicator(
            "Component Optimal Price",
            &mOptimalSize, &mExportIndicators,
            "Component Optimal Price", pOptimalSizeUnit(), "OptPrice");
    }

    mInputIndicators->addIndicator(
        "ImposedProfile " + Direction() + " time",
        &mRunningTime, &mExportIndicators,
        "Running time", "h", "ImposedProfileTime");

    for (const auto& port : mListPort)
    {
        if (!port->getCarrier())
            continue;

        const std::string portId = port->ID();
        const std::string varName = port->Variable();
        const std::string storageName = port->getCarrier()->StorageName();
        const std::string fluxName = port->getCarrier()->FluxName();

        const MIPModeler::MIPExpression1D* ptrExp1D = getMIPExpression1D(varName);
        if (!ptrExp1D)
            continue;

        /** Sens() is that of the default port, not of the current port */
        if (port->Direction() == GS::KPROD() && Sens() > 0)
        {
            mProductionMap.try_emplace(portId, kResultCount, 0.0);
            mProdLvlTotMap.try_emplace(portId, kResultCount, 0.0);
            mProdMeanMap.try_emplace(portId, kResultCount, 0.0);

            mInputIndicators->addIndicator(
                SExtFunctionName({ this, port, &indicatorName, { "ImposedProfile", STORAGE_NAME, VARIABLE } }),
                &mProductionMap[portId], &mExportIndicators,
                "ImposedProfile " + storageName + " " + varName, port->pStorageUnit(),
                SExtFunctionName({ this, port, &indicatorName, { "TotImposedProfile", VARIABLE } }));

            mInputIndicators->addIndicator(
                SExtFunctionName({ this, port, &indicatorName, { "Levelized ImposedProfile", STORAGE_NAME, VARIABLE } }),
                &mProdLvlTotMap[portId], &mExportIndicators,
                "Levelized ImposedProfile " + storageName + " " + varName, port->pStorageUnit(),
                SExtFunctionName({ this, port, &indicatorName, { "LvlzdTotImposedProfile", VARIABLE } }));

            mInputIndicators->addIndicator(
                SExtFunctionName({ this, port, &indicatorName, { "Mean", FLUX_NAME, VARIABLE } }),
                &mProdMeanMap[portId], &mExportIndicators,
                "Mean " + fluxName + " " + varName, port->pFluxUnit(),
                SExtFunctionName({ this, port, &indicatorName, { "MeanImposedProfile", VARIABLE } }));
        }
        else if (port->Direction() == GS::KCONS() && Sens() <= 0)
        {
            mConsumptionMap.try_emplace(portId, kResultCount, 0.0);
            mConsLvlTotMap.try_emplace(portId, kResultCount, 0.0);
            mConsMeanMap.try_emplace(portId, kResultCount, 0.0);

            mInputIndicators->addIndicator(
                SExtFunctionName({ this, port, &indicatorName, { "ImposedProfile", STORAGE_NAME, VARIABLE } }),
                &mConsumptionMap[portId], &mExportIndicators,
                "ImposedProfile " + storageName + " " + varName, port->pStorageUnit(),
                SExtFunctionName({ this, port, &indicatorName, { "TotImposedProfile", VARIABLE } }));

            mInputIndicators->addIndicator(
                SExtFunctionName({ this, port, &indicatorName, { "Levelized ImposedProfile", STORAGE_NAME, VARIABLE } }),
                &mConsLvlTotMap[portId], &mExportIndicators,
                "Levelized ImposedProfile " + storageName + " " + varName, port->pStorageUnit(),
                SExtFunctionName({ this, port, &indicatorName, { "LvlzdTotImposedProfile", VARIABLE } }));

            mInputIndicators->addIndicator(
                SExtFunctionName({ this, port, &indicatorName, { "Mean", FLUX_NAME, VARIABLE } }),
                &mConsMeanMap[portId], &mExportIndicators,
                "Mean " + fluxName + " " + varName, port->pFluxUnit(),
                SExtFunctionName({ this, port, &indicatorName, { "MeanImposedProfile", VARIABLE } }));
        }
        else if (port->Direction() == GS::KDATA())
        {
            mExpEchData.try_emplace(portId, kResultCount, 0.0);

            mInputIndicators->addIndicator(
                SExtFunctionName({ this, port, &indicatorName, { "Data Port published", VARIABLE, "- data computed" } }),
                &mExpEchData[portId], &mExportIndicators,
                "Data port", port->pStorageUnit(),
                SExtFunctionName({ this, port, &indicatorName, { "DataPort", VARIABLE } }));
        }
    }
}

void SourceLoadSubModel::computeAllIndicators(const double* optSol)
{
    TechnicalSubModel::computeAllIndicators(optSol);
    computeMaxRunningTime(optSol);
    computeRunningTime(optSol);

    for (const auto& port : mListPort)
    {
        const MIPModeler::MIPExpression1D* ptrExp1D = getMIPExpression1D(port->Variable());
        if (!ptrExp1D)
            continue;

        if (port->Direction() == GS::KPROD() && Sens() > 0)
            computeProductionIndicators(port, *ptrExp1D, optSol);
        else if (port->Direction() == GS::KCONS() && Sens() <= 0)
            computeConsumptionIndicators(port, *ptrExp1D, optSol);
        else if (port->Direction() == GS::KDATA())
            computeDataExchangeIndicators(port, *ptrExp1D, optSol);
    }
}

// --- SourceENR-specific Indicators -------------------------------------- 

void SourceLoadSubModel::declareSourceENRModelIndicators()
{
    TechnicalSubModel::declareModelIndicators();

    mInputIndicators->addIndicator(
        "Component Weight",
        &mOptimalSize, &mExportIndicators,
        "Component size", pOptimalSizeUnit(), "Weight");

    for (const auto& port : mListPort)
    {
        if (!port->getCarrier())
            continue;

        const std::string portId = port->ID();
        const MIPModeler::MIPExpression1D* ptrExp1D = getMIPExpression1D(port->Variable());
        if (!ptrExp1D)
            continue;

        if (port->Direction() == GS::KPROD())
        {
            mProductionMap.try_emplace(portId, kResultCount, 0.0);
            mProdLvlTotMap.try_emplace(portId, kResultCount, 0.0);
            mProdMeanMap.try_emplace(portId, kResultCount, 0.0);

            mInputIndicators->addIndicator(
                SExtFunctionName({ this, port, &indicatorName, { "ENR injection time" } }),
                &mRunningTime, &mExportIndicators,
                "Running time", "h",
                SExtFunctionName({ this, port, &indicatorName, { "ENRInjectionTime" } }));

            mInputIndicators->addIndicator(
                SExtFunctionName({ this, port, &indicatorName, { "ENR injection", STORAGE_NAME, VARIABLE } }),
                &mProductionMap[portId], &mExportIndicators,
                "", port->pStorageUnit(),
                SExtFunctionName({ this, port, &indicatorName, { "Tot", VARIABLE } }));

            mInputIndicators->addIndicator(
                SExtFunctionName({ this, port, &indicatorName, { "Levelized ENR injection", STORAGE_NAME, VARIABLE } }),
                &mProdLvlTotMap[portId], &mExportIndicators,
                "", port->pStorageUnit(),
                SExtFunctionName({ this, port, &indicatorName, { "LvlzdTot", VARIABLE } }));

            mInputIndicators->addIndicator(
                SExtFunctionName({ this, port, &indicatorName, { "Mean", FLUX_NAME, VARIABLE } }),
                &mProdMeanMap[portId], &mExportIndicators,
                "Mean", port->pFluxUnit(),
                SExtFunctionName({ this, port, &indicatorName, { "Mean", VARIABLE } }));
        }
        else if (port->Direction() == GS::KDATA())
        {
            mExpEchData.try_emplace(portId, kResultCount, 0.0);

            mInputIndicators->addIndicator(
                SExtFunctionName({ this, port, &indicatorName, { "Data Port published", VARIABLE, "- data computed" } }),
                &mExpEchData[portId], &mExportIndicators,
                "Data port", port->pStorageUnit(),
                SExtFunctionName({ this, port, &indicatorName, { "DataPort", VARIABLE } }));
        }
    }
}

void SourceLoadSubModel::computeSourceENRModelIndicators(const double* optSol)
{
    TechnicalSubModel::computeAllIndicators(optSol);
    computeMaxRunningTime(optSol);
    computeRunningTime(optSol);

    for (const auto& port : mListPort)
    {
        const MIPModeler::MIPExpression1D* ptrExp1D = getMIPExpression1D(port->Variable());
        if (!ptrExp1D)
            continue;

        if (port->Direction() == GS::KPROD())
            computeENRProductionIndicators(port, *ptrExp1D, optSol);
        else if (port->Direction() == GS::KDATA())
            computeDataExchangeIndicators(port, *ptrExp1D, optSol);
    }
}

// --- SourceLoad-specific ----------------------------------------------------

double SourceLoadSubModel::Sens() const
{
    if (!mSourceLoadDefaultPort)
    {
        cError() << "Sens() called but mSourceLoadDefaultPort is null for" << Name();
        return +1.0; // safe default
    }
    return CairnUtils::toUpper(mSourceLoadDefaultPort->Direction()) == KCONS()
        ? -1.0  // load / sink
        : +1.0; // source (includes DATAEXCHANGE)
}

std::string SourceLoadSubModel::Direction() const
{
    return Sens() < 0 ? "load" : "source";
}

// --- Indicator computation helpers -----------------------------------------------

void SourceLoadSubModel::computeMaxRunningTime(const double* /*optSol*/)
{
    auto* compo = parentComponent();
    const double extrapolationFactor = compo ? compo->ExtrapolationFactor() : 1.0;

    mMaxRunningTime.at(0) = 0.;
    for (uint64_t t = 0; t < mHorizon; ++t)
        mMaxRunningTime.at(0) += TimeStep(t) * extrapolationFactor;  // PLAN - extrapolated
    mMaxRunningTime.at(1) += mNpdtPast * TimeStep(0);                // HIST - cumulated

    if (!mOptimalSize.empty())
        mOptimalSizeAllCycles.push_back(mOptimalSize.at(0));
}

void SourceLoadSubModel::computeRunningTime(const double* optSol)
{
    for (const auto& port : mListPort)
    {
        const MIPModeler::MIPExpression1D* ptrExp1D = getMIPExpression1D(port->Variable());
        if (!ptrExp1D)
            continue;

        const bool isRelevantProd = (port->Direction() == GS::KPROD() && Sens() > 0);
        const bool isRelevantCons = (port->Direction() == GS::KCONS() && Sens() <= 0);
        if (!isRelevantProd && !isRelevantCons)
            continue;

        const double prevRunningTime = mRunningTime.at(0);
        computeTime(true, mHorizon, *ptrExp1D, optSol, mRunningTime.at(0));        // PLAN
        computeTime(false, *mptrTimeshift, *ptrExp1D, optSol, mRunningTime.at(1)); // HIST

        if (mRunningTime.at(0) > prevRunningTime)
            break; // found first relevant port
    }
}

void SourceLoadSubModel::computeProductionIndicators(
    const MilpPort* port,
    const MIPModeler::MIPExpression1D& exp1D,
    const double* optSol)
{
    const std::string portId = port->ID();
    const double aPort = port->VarCoeff();
    const double bPort = port->VarOffset();

    auto& prod = mProductionMap[portId];
    auto& prodLvl = mProdLvlTotMap[portId];
    auto& prodMean = mProdMeanMap[portId];

    prod.at(0) = prodLvl.at(0) = prodMean.at(0) = 0.;

    computeProduction(true, mHorizon, exp1D, optSol, aPort, bPort, prod.at(0));
    computeProduction(false, *mptrTimeshift, exp1D, optSol, aPort, bPort, prod.at(1));
    computeLvlProduction(true, mHorizon, exp1D, optSol, aPort, bPort, prodLvl.at(0));
    computeLvlProduction(false, *mptrTimeshift, exp1D, optSol, aPort, bPort, prodLvl.at(1));

    for (int i = 0; i < kResultCount; ++i)
        if (mRunningTime.at(i) > kRunningEpsilon)
            prodMean.at(i) = prod.at(i) / mRunningTime.at(i);
}

void SourceLoadSubModel::computeENRProductionIndicators(
    const MilpPort* port,
    const MIPModeler::MIPExpression1D& exp1D,
    const double* optSol)
{
    const std::string portId = port->ID();
    const double aPort = port->VarCoeff();
    const double bPort = port->VarOffset();

    auto& prod = mProductionMap[portId];
    auto& prodLvl = mProdLvlTotMap[portId];
    auto& prodMean = mProdMeanMap[portId];

    prod.at(0) = prodLvl.at(0) = prodMean.at(0) = 0.;

    computeProduction(true, mHorizon, exp1D, optSol, aPort, bPort, prod.at(0));
    computeProduction(false, *mptrTimeshift, exp1D, optSol, aPort, bPort, prod.at(1));

    // Scale by optimal size - specific to ENR model
    if (mOptimalSize.size() > kResultCount - 1)
    {
        prod.at(0) *= mOptimalSize.at(0);
        prod.at(1) *= mOptimalSize.at(1);
    }

    computeLvlProduction(true, mHorizon, exp1D, optSol, aPort, bPort, prodLvl.at(0));
    computeLvlProduction(false, *mptrTimeshift, exp1D, optSol, aPort, bPort, prodLvl.at(1));

    if (mOptimalSize.size() > kResultCount - 1)
    {
        prodLvl.at(0) *= mOptimalSize.at(0);
        prodLvl.at(1) *= mOptimalSize.at(1);
    }

    for (int i = 0; i < kResultCount; ++i)
        if (mRunningTime.at(i) > kRunningEpsilon)
            prodMean.at(i) = prod.at(i) / mRunningTime.at(i);
}

void SourceLoadSubModel::computeConsumptionIndicators(
    const MilpPort* port,
    const MIPModeler::MIPExpression1D& exp1D,
    const double* optSol)
{
    const std::string portId = port->ID();
    const double aPort = port->VarCoeff();
    const double bPort = port->VarOffset();

    auto& cons = mConsumptionMap[portId];
    auto& consLvl = mConsLvlTotMap[portId];
    auto& consMean = mConsMeanMap[portId];

    cons.at(0) = consLvl.at(0) = consMean.at(0) = 0.; 

    computeConsumption(true, mHorizon, exp1D, optSol, aPort, bPort, cons.at(0));
    computeConsumption(false, *mptrTimeshift, exp1D, optSol, aPort, bPort, cons.at(1));
    computeLvlConsumption(true, mHorizon, exp1D, optSol, aPort, bPort, consLvl.at(0));
    computeLvlConsumption(false, *mptrTimeshift, exp1D, optSol, aPort, bPort, consLvl.at(1));

    for (int i = 0; i < kResultCount; ++i)
        if (mRunningTime.at(i) > kRunningEpsilon)
            consMean.at(i) = cons.at(i) / mRunningTime.at(i);
}

void SourceLoadSubModel::computeDataExchangeIndicators(
    const MilpPort* port,
    const MIPModeler::MIPExpression1D& exp1D,
    const double* optSol)
{
    const std::string portId = port->ID();
    const double aPort = port->VarCoeff();
    const double bPort = port->VarOffset();

    computeProduction(true, mHorizon, exp1D, optSol, aPort, bPort, mExpEchData[portId].at(0));
    computeProduction(false, *mptrTimeshift, exp1D, optSol, aPort, bPort, mExpEchData[portId].at(1));
}

