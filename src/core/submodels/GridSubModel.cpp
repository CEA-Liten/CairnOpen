/*
 * \file    GridSubModel.cpp
 * \brief   GridSubModel is a specialized TechnicalSubModel representing a grid connection.
 * \version 1.0
 * \author	Ali KASSEM
 * \date    13/09/2024
 */

#include "GridSubModel.h"

// --- Construction -----------------------------------------------------------

GridSubModel::GridSubModel(CairnObject* aParent)
    : TechnicalSubModel(aParent)
{
    mAddStateVariable = true; /** Always add state constraints for Grid */
}

// --- SubModel interface -----------------------------------------------------

void GridSubModel::defineMainCarrier()
{
    mMainCarrier = mPortGridFlow ? mPortGridFlow->getCarrier() : nullptr;
}

void GridSubModel::setTimeData()
{
    TechnicalSubModel::setTimeData();
    mEnergyPrice.resize(mHorizon);
    mSellPrice.resize(mHorizon);
    mBuyPrice.resize(mHorizon);
    mBuyPriceSeasonal.resize(mHorizon);
    mGridVariableMaxFlow.resize(mHorizon);
}

void GridSubModel::computeInitialData()
{
    setMaxValue(mMaxFlux);
    setMinValue(mMinSize);
}

void GridSubModel::declareModelConfigurationParameters()
{
    TechnicalSubModel::declareModelConfigurationParameters();

    /** Re-declare EcoInvestModel with default = false for Grid */
    addConfigParameter("EcoInvestModel", &mEcoInvestModel, false, false, true, "Use EcoInvestModel - i.e. use Capex and Opex if true", "");
    addConfigParameter("AddVariableMaxFlow", &mAddVariableMaxFlow, false, false, true, "If true: use time-variable maximum flow limitation defined by UseGridVariableMaxFlow - default is false");
}

void GridSubModel::declareModelParameters()
{
    TechnicalSubModel::declareModelParameters();

    addParameter("MaxFlow", &mMaxFlux, 1.e4, true, true,  "Maximum allowed extraction or injection", mPortGridFlow->pFluxUnit());
    addParameter("MinFlow", &mMinFlux, 0.,   false, true, "Minimum allowed extraction or injection", mPortGridFlow->pFluxUnit());

    addTimeSeries("UseProfileSellPrice",
        &mSellPrice,
        SExtFunctionFlag({ &isInjection,  this }),
        SExtFunctionFlag({ &isInjection,  this }),
        "Grid-specific sell price profile - overrides EnergyVector default",
        SFunctionUnit({ eFTypeDivision, { pCurrency(), mPortGridFlow->pStorageUnit() } }));

    addTimeSeries("UseProfileBuyPrice",
        &mBuyPrice,
        SExtFunctionFlag({ &isExtraction, this }),
        SExtFunctionFlag({ &isExtraction, this }),
        "Grid-specific buy price profile - overrides EnergyVector default",
        SFunctionUnit({ eFTypeDivision, { pCurrency(), mPortGridFlow->pStorageUnit() } }));

    addTimeSeries("UseProfileBuyPriceSeasonal",
        &mBuyPriceSeasonal,
        SFunctionFlag({ eFTypeNotAnd, {}, { &mSeasonalPrevisions }, SExtFunctionFlag({ &isExtraction, this }) }),
        SExtFunctionFlag({ &isExtraction, this }),
        "Seasonal purchase price time series for extraction - see EnergyVector",
        SFunctionUnit({ eFTypeDivision, { pCurrency(), mPortGridFlow->pStorageUnit() } }),
        "TimeSeriesForecast");

    addTimeSeries("UseVariableMaximumGridFlow",
        &mGridVariableMaxFlow,
        false, true,
        "Time series of maximum grid flow - extraction or injection",
        mPortGridFlow->pFluxUnit());
}

void GridSubModel::declareModelInterface()
{
    if (!mPortGridFlow)
        throw Cairn_Exception(Name() + ": default Flow port of the Grid is not defined", -1);

    TechnicalSubModel::declareModelInterface();

    addSizeMaxIO("MaxFlow", &mExpSizeMax, true, mPortGridFlow->pFluxUnit(), "Sizing Grid flow - name must match mInputParam MaxFlow");
   
    addIO("GridFlow", &mExpFlux, true, mPortGridFlow->pFluxUnit(), "Grid flow injected or extracted - positive = extraction");

    addIO("GridPrice", 
        &mExpGridPrice, 
        true, 
        SFunctionUnit({ eFTypeDivision, { mPortGridFlow->pFluxUnit(), pCurrency() } }), 
        "Grid price - extraction = buy price"
    );
}

void GridSubModel::declareModelIndicators()
{
    TechnicalSubModel::declareModelIndicators();

    /** Running time indicator for the grid */
    mInputIndicators->addIndicator(
        SExtFunctionName({ this, nullptr, &indicatorName, { "Grid", DIRECTION, "time" } }),
        &mRunningTime, &mExportIndicators,
        "Number of hours of grid " + Direction(), "h",
        SExtFunctionName({ this, nullptr, &indicatorName, { DIRECTION, "Time" } }));

    for (const auto& port : mListPort)
    {
        if (!port->getCarrier())
            continue;

        const std::string portId = port->ID();
        const MIPModeler::MIPExpression1D* ptrExp1D = getMIPExpression1D(port->Variable());
        if (!ptrExp1D)
            continue;

        /** Sens() is that of the main default port (flow), not of the current port */
        if (port->Direction() == GS::KPROD() && Sens() > 0)
        {
            mProductionMap.try_emplace(portId, kResultCount, 0.0);
            mProdLvlTotMap.try_emplace(portId, kResultCount, 0.0);
            mProdMeanMap.try_emplace(portId, kResultCount, 0.0);

            mInputIndicators->addIndicator(
                SExtFunctionName({ this, port, &indicatorName, { "Grid", DIRECTION, STORAGE_NAME, VARIABLE } }),
                &mProductionMap[portId], &mExportIndicators,
                "Grid " + Direction(), port->pStorageUnit(),
                SExtFunctionName({ this, port, &indicatorName, { "Tot", VARIABLE } }));

            mInputIndicators->addIndicator(
                SExtFunctionName({ this, port, &indicatorName, { "Levelized Grid", DIRECTION, STORAGE_NAME, VARIABLE } }),
                &mProdLvlTotMap[portId], &mExportIndicators,
                "Levelized Grid " + Direction(), port->pStorageUnit(),
                SExtFunctionName({ this, port, &indicatorName, { "LvlzdTot", VARIABLE } }));

            mInputIndicators->addIndicator(
                SExtFunctionName({ this, port, &indicatorName, { "Mean", DIRECTION, FLUX_NAME, VARIABLE } }),
                &mProdMeanMap[portId], &mExportIndicators,
                "Mean " + Direction(), port->pFluxUnit(),
                SExtFunctionName({ this, port, &indicatorName, { "Mean", VARIABLE } }));
        }
        else if (port->Direction() == GS::KCONS() && Sens() <= 0)
        {
            mConsumptionMap.try_emplace(portId, kResultCount, 0.0);
            mConsLvlTotMap.try_emplace(portId, kResultCount, 0.0);
            mConsMeanMap.try_emplace(portId, kResultCount, 0.0);

            mInputIndicators->addIndicator(
                SExtFunctionName({ this, port, &indicatorName, { "Grid", DIRECTION, STORAGE_NAME, VARIABLE } }),
                &mConsumptionMap[portId], &mExportIndicators,
                "Grid " + Direction(), port->pStorageUnit(),
                SExtFunctionName({ this, port, &indicatorName, { "Tot", VARIABLE } }));

            mInputIndicators->addIndicator(
                SExtFunctionName({ this, port, &indicatorName, { "Levelized Grid", DIRECTION, STORAGE_NAME, VARIABLE } }),
                &mConsLvlTotMap[portId], &mExportIndicators,
                "Levelized Grid " + Direction(), port->pStorageUnit(),
                SExtFunctionName({ this, port, &indicatorName, { "LvlzdTot", VARIABLE } }));

            mInputIndicators->addIndicator(
                SExtFunctionName({ this, port, &indicatorName, { "Mean", DIRECTION, FLUX_NAME, VARIABLE } }),
                &mConsMeanMap[portId], &mExportIndicators,
                "Mean " + Direction(), port->pFluxUnit(),
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

void GridSubModel::computeAllIndicators(const double* optSol)
{
    TechnicalSubModel::computeAllIndicators(optSol);

    auto* compo = parentComponent();
    double extrapolationFactor = compo ? compo->ExtrapolationFactor() : 1.0;

    // Compute maximum running time
    mMaxRunningTime.at(0) = 0.;
    for (uint64_t t = 0; t < mHorizon; ++t)
        mMaxRunningTime.at(0) += TimeStep(t) * extrapolationFactor; // PLAN - extrapolated
    mMaxRunningTime.at(1) = mNpdtPast * TimeStep(0);                // HIST - cumulated : TODO: = or += ?!

    if (!mOptimalSize.empty())
        mOptimalSizeAllCycles.push_back(mOptimalSize.at(0));

    bool runningTimeComputed = false;
    for (const auto& port : mListPort)
    {
        const MIPModeler::MIPExpression1D* ptrExp1D = getMIPExpression1D(port->Variable());
        if (!ptrExp1D)
            continue;

        // Compute running time from first non-data port
        if (port->Direction() != GS::KDATA() && !runningTimeComputed)
        {
            computeRunningTime(*ptrExp1D, optSol);
            runningTimeComputed = true;
        }

        if (port->Direction() == GS::KPROD() && Sens() > 0)
            computeProductionIndicators(port, *ptrExp1D, optSol);
        else if (port->Direction() == GS::KCONS() && Sens() <= 0)
            computeConsumptionIndicators(port, *ptrExp1D, optSol);
        else if (port->Direction() == GS::KDATA())
            computeDataExchangeIndicators(port, *ptrExp1D, optSol);
    }
}

// --- Grid-specific ----------------------------------------------------------

double GridSubModel::Sens() const
{
    if (!mPortGridFlow)
    {
        cError() << "Sens() called but mPortGridFlow is null for" << Name();
        return +1.0; // safe default
    }
    return CairnUtils::toUpper(mPortGridFlow->Direction()) == KCONS()
        ? -1.0  // injection
        : +1.0; // extraction (includes DATAEXCHANGE)
}

std::string GridSubModel::Direction() const
{
    return Sens() < 0 ? "injection" : "extraction";
}

// --- Indicator computation helpers --------------------------------------------------

void GridSubModel::computeRunningTime(const MIPModeler::MIPExpression1D& exp1D, const double* optSol)
{
    computeTime(true, mHorizon, exp1D, optSol, mRunningTime.at(0)); // PLAN
    computeTime(false, *mptrTimeshift, exp1D, optSol, mRunningTime.at(1)); // HIST
}

void GridSubModel::computeProductionIndicators(
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

    computeProduction(true, mHorizon, exp1D, optSol, aPort, bPort, prod.at(0));    // PLAN
    computeProduction(false, *mptrTimeshift, exp1D, optSol, aPort, bPort, prod.at(1));    // HIST
    computeLvlProduction(true, mHorizon, exp1D, optSol, aPort, bPort, prodLvl.at(0));
    computeLvlProduction(false, *mptrTimeshift, exp1D, optSol, aPort, bPort, prodLvl.at(1));

    for (int i = 0; i < kResultCount; ++i)
        if (mRunningTime.at(i) > kRunningEpsilon)
            prodMean.at(i) = prod.at(i) / mRunningTime.at(i);
}

void GridSubModel::computeConsumptionIndicators(
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

void GridSubModel::computeDataExchangeIndicators(
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
