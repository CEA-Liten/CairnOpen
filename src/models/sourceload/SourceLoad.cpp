/**
* \file		SourceLoad.cpp
* \brief	SourceLoad model
* \version	1.0
* \author	Yacine Gaoua
* \date		08/02/2019
*/

#include "SourceLoad.h"
extern "C" MODELS_DECLSPEC QObject * createModel(QObject * aParent)
{
    return new SourceLoad(aParent);
}

SourceLoad::SourceLoad(QObject* aParent) : 
    SourceLoadSubModel(aParent),
    mTemperature_in1(0.),
    mTemperature_out1(0.)
{

}

SourceLoad::~SourceLoad()
{}

void SourceLoad::setTimeData()
{
    SubModel::setTimeData();
    mImposedFlux.resize(mHorizon);
    mStartStopProfile.resize(mHorizon);
    mImposedFluxSeasonal.resize(mHorizon);
    mEnergyPrice.resize(mHorizon);
    mMaxSheddingTS.resize(mHorizon);
    mCostSheddingTS.resize(mHorizon);  
}

int SourceLoad::checkBusFlowBalanceVarName(MilpPort* port, int& inumberchange, QString& varUseCheck)
{
    int ierr = SourceLoadSubModel::checkBusFlowBalanceVarName(port, inumberchange, varUseCheck);  
    QString varUse = port->Direction();
    MilpComponent* pParent = (MilpComponent*)parent();
    mSens = pParent->Sens();
    // Uncomplete description -> use default definitions functions of models
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
    return ierr;
}

int SourceLoad::checkConsistency()
{
    //int ier = TechnicalSubModel::checkConsistency();
    if (mWeight < 0 && (mUseControlledFlux == true || mUseWeightedFlux == true))
    {
        qCritical() << " For linearity purpose, optimization of SourceLoad Weight " << mWeight << " requires mUseControlledFlux = false and mUseWeightedFlux = false ! " << mUseControlledFlux << mUseWeightedFlux;
        return -1;
    }
    if (mWeight < 0 && mComputeOptimalPrice)
    {
        qCritical() << " For linearity purpose, optimization of SourceLoad Weight " << mWeight << " requires mComputeOptimalPrice = false ! " << mComputeOptimalPrice;
        return -1;
    }

    if (mAddSheddingDetailed && mSens > 0.) {
        qCritical() << "Load shedding cannot be used for sources (only loads)";
        return -1;
    }
    if (mAddSheddingDetailed && mLPModelOnly) {
        qCritical() << "Load shedding is not compatible with LPModelOnly option";
        return -1;
    }
    if (mAddSheddingDetailed && mControl != "" && mNpdtPast < mMaxTimeShedding) {
        qCritical() << "Load shedding max activation time (" << mMaxTimeShedding << ") must be less or equal to past size (" << mNpdtPast << ")";
        return -1;
    }
    if (mAddSheddingDetailed && mControl != "" && mNpdtPast < mMinSheddingStandBy) {
        qCritical() << "Load shedding min deactivation time (" << mMinSheddingStandBy << ") must be less or equal to past size (" << mNpdtPast << ")";
        return -1;
    }

    return 0;
}

double SourceLoad::getTemperature(const QString& direction)
{
    foreach(MilpPort* lptrport, mListPort)
    {
        if (lptrport->Direction() == direction && lptrport->PotentialName() == "Temperature" && lptrport->ptrEnergyVector() != nullptr)
        {
            return lptrport->ptrEnergyVector()->Potential();
        }
    }
    return 0.;
}

void SourceLoad::buildModel() {

    if (mAllocate)
    {
        mExpFlux = MIPModeler::MIPExpression1D(mHorizon);
        mExpPowerOut = MIPModeler::MIPExpression1D(mHorizon);
        mExpPowerIn = MIPModeler::MIPExpression1D(mHorizon);
        mExpFluxWeight = MIPModeler::MIPExpression1D(mHorizon);

        if (mUseControlledFlux)
        {
            mVarControlledFlux = MIPModeler::MIPVariable1D(mHorizon, 0., fabs(mMaxFlux));
        }
        else if (mUseWeightedFlux)
        {
            mVarFluxWeight = MIPModeler::MIPVariable1D(mHorizon, 0., 1.e6); // ponderation;
        }

        mExpCost = MIPModeler::MIPExpression1D(mHorizon);
    }
    else
    {
        closeExpression1D(mExpFlux);

        closeExpression1D(mExpPowerOut);
        closeExpression1D(mExpPowerIn);
        closeExpression1D(mExpFluxWeight);
        closeExpression1D(mExpCost);
    }

    if (mComputeOptimalPrice)
    {
        setExpSizeMax(mMaxOptimalPrice, "MaxPrice");
    }
    else
        setExpSizeMax(1, "MWeight");

    if (mUseControlledFlux)
    {
        // Imposed flux is set by external expression via node equality constraint

        addVariable(mVarControlledFlux, "SoCtrlFlux");

        for (uint64_t t = 0; t < mHorizon; ++t)
        {
            mExpFlux[t] += mVarControlledFlux(t);
            // Muting last time steps if pre-computed seasonalCosts model
            if (mSeasonalCosts && t >= (uint64_t)mMilpNpdt) {
                addConstraint(mExpFlux[t] == 0, "seasonalMute", t);
            }
        }
    }
    else if (mUseWeightedFlux)
    {
        // Imposed flux is weighted by external expression via node equality constraint

        addVariable(mVarFluxWeight, "SoWghtFlux");

        for (uint64_t t = 0; t < mHorizon; ++t)
        {
            mExpFluxWeight[t] += mVarFluxWeight(t);

            mExpFlux[t] += (mExpFluxWeight[t] + mStartStopProfile[t]) * mImposedFlux[t];
            // Muting last time steps if pre-computed seasonalCosts model
            if (mSeasonalCosts && t >= (uint64_t)mMilpNpdt) {
                addConstraint(mExpFlux[t] == 0, "seasonalMute", t);
            }
        }
    }
    else
    {
        // Imposed flux is imposed by time series, possibly weighted by optimal Size

        for (uint64_t t = 0; t < mHorizon; ++t)
        {
            // Muting last time steps if pre-computed seasonalCosts model
            if (mSeasonalCosts && t >= (uint64_t)mMilpNpdt)
            {
                // Imposed flux is set to zero for seasonnal cost model.
                mExpFlux[t] += 0. * mImposedFlux[t];
            }
            else
            {
                if (t >= mTimeStepBeginForecast && mSeasonalPrevisions)
                {
                    qInfo() << "SourceLoad, mTimeStepBeginForecast =" << mTimeStepBeginForecast;
                    // Imposed flux is imposed by Seasonnal flux time series, possibly weighted by optimal Size
                    mExpFlux[t] += mExpSizeMax * mImposedFluxSeasonal[t];
                }
                else
                {
                    mExpFlux[t] += mExpSizeMax * mImposedFlux[t];
                }
            }
        }
    }

    double coeffin = 1.;
    double coeffout = 1.; // to ensure upward compatibility with previous studies based on OUTPUTFlux1
    if (mAddHeatConsumerModel)
    {
        mTemperature_in1 = getTemperature(KCONS());
        mTemperature_out1 = getTemperature(KPROD());

        if (abs(mTemperature_in1 - mTemperature_out1) > 1.e-3)
            coeffin = mTemperature_in1 / (mTemperature_in1 - mTemperature_out1); // Pin = qCpTin = coeffin * ImposedFlux = coeffin * q * Cp (Tin - Tout)
        else
            coeffin = 1.;

        coeffout = coeffin - 1.;   // Pout = Pin - ImposedFlux
    }
    for (uint64_t t = 0; t < mHorizon; ++t)
    {
        mExpPowerOut[t] += coeffout * mExpFlux[t];
        mExpPowerIn[t] += coeffin * mExpFlux[t];
    }

    for (uint64_t t = 0; t < mHorizon; ++t)
    {
        if (mComputeOptimalPrice) {
            mExpCost[t] += mSens * mExpSizeMax * mImposedFlux[t];
        }
        else {
            mExpCost[t] += mEnergyPrice[t] * mExpFlux[t];
        }
    }
    // 1 constraints: 1 for maximum value, the other one if power Imposed (constant or profile)

    for (uint64_t t = 0; t < mHorizon; ++t)
    {
        addConstraint(mExpFlux[t] <= fabs(mMaxFlux), "MSFx", t);
    }

    if (mAddStaticCompensation) {
        computeReactivePower();
    }

    if (mAddSheddingDetailed) {
        addLoadShedding();
    }
    if (mAddPeakShavingDetailed) {
        addPeakShaving();
    }
    /** Compute all expressions*/
    computeAllContribution();

    mAllocate = false;

    ModelerInterface* pExternalModeler = mModel->getExternalModeler();
    if (pExternalModeler != nullptr) {
        std::string compoName = SubModel::parent()->objectName().toStdString();
        pExternalModeler->addText("");
        pExternalModeler->addComment("+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++");
        pExternalModeler->addComment(" add new SourceLoad component");
        pExternalModeler->addComment("+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++");

        ModelerParams vParams;
        vParams.addParam(compoName + "_p_LoadProfile", "k", mImposedFlux);
        pExternalModeler->setModelData(vParams);


        pExternalModeler->addText("$\t setLocal CompoName " + compoName);
        pExternalModeler->addText("");
        ModelerParams vOptions;
        pExternalModeler->addModelFromFile("%gamslib%/SourceLoad/SourceLoad.gms", "%CompoName%", vOptions);
    }

}
void SourceLoad::computeEconomicalContribution()
{
    TechnicalSubModel::computeEconomicalContribution();

    if (mAddVariableCostModel)
        // whether CAPEX + OPEX or not (EcoInvestModel), production cost can be accounted for
        // To be put on for the shedding model
    {
        for (uint64_t t = 0; t < mHorizon; ++t) {
            mExpVariableCosts[t] += mSens * TimeStep(t) * mExpCost[t];
        }
    }

    if (mAddSheddingDetailed) {
        for (uint64_t t = 0; t < mHorizon; t++) {
            mExpVariableCosts[t] += mExpCostShedding[t];
        }
    }
    if (mAddPeakShavingDetailed) {
        mExpCapex += mMaxEffectCapex * mVarMaxEffect;
        for (uint64_t t = 0; t < mTimeSteps.size(); ++t) {
            mExpPureOpex[t] += mMaxEffectCapex * mMaxEffectOpex * mVarMaxEffect;
        }
    }
}

void SourceLoad::computeReactivePower()
{
    if (mAllocate)
    {
        mStaticCompensation = MIPModeler::MIPVariable0D(-1., 1.);
        mExpStaticCompensation = MIPModeler::MIPExpression();
        mReactivePower = MIPModeler::MIPVariable1D(mHorizon);
        mExpReactivePower = MIPModeler::MIPExpression1D(mHorizon);
    }
    else
    {
        closeExpression(mExpStaticCompensation);
        closeExpression1D(mExpReactivePower);
    }

    addVariable(mStaticCompensation, "StaticCompensation");
    addVariable(mReactivePower, "reactivePower");
    mExpStaticCompensation += mStaticCompensation;
    if (mFixedStaticCompensation) {
        for (uint64_t t = 0; t < mHorizon; ++t) {
            mExpReactivePower[t] += mReactivePower(t);
            addConstraint(mExpReactivePower[t] == mStaticCompensationValue * mImposedFlux[t], "ReactivePowerFixedComp", t);
        }
    }
    else {
        for (uint64_t t = 0; t < mHorizon; ++t) {
            mExpReactivePower[t] += mReactivePower(t);
            addConstraint(mExpReactivePower[t] == mExpStaticCompensation * mImposedFlux[t], "ReactivePower", t);
        }
    }
}

bool SourceLoad::isPriceOptimized()
{
    return mComputeOptimalPrice;
}

void SourceLoad::addLoadShedding() {
    if (mAllocate) {
        mVarPowerShedding = MIPModeler::MIPVariable1D(mHorizon, 0., fabs(mMaxShedding));

        mExpPowerShedding = MIPModeler::MIPExpression1D(mHorizon); // shedded power
        mExpCostShedding = MIPModeler::MIPExpression1D(mHorizon); // shedding penalty cost      

        mShedState = MIPModeler::MIPVariable1D(mHorizon, 0, 1, MIPModeler::MIP_INT); // 1 iif shedding is activated
        mShedOn = MIPModeler::MIPVariable1D(mHorizon, 0, 1, MIPModeler::MIP_INT); // 1 iif shedding state changes from 0 to 1
        mShedOff = MIPModeler::MIPVariable1D(mHorizon, 0, 1, MIPModeler::MIP_INT); // 1 iif shedding state changes from 1 to 0

        mExpShedState = MIPModeler::MIPExpression1D(mHorizon);
        mExpShedOn = MIPModeler::MIPExpression1D(mHorizon);
        mExpShedOff = MIPModeler::MIPExpression1D(mHorizon);
    }
    else {
        closeExpression1D(mExpPowerShedding);
        closeExpression1D(mExpCostShedding);

        closeExpression1D(mExpShedState);
        closeExpression1D(mExpShedOn);
        closeExpression1D(mExpShedOff);
    }

    addVariable(mVarPowerShedding, "PowerShedding");

    addVariable(mShedState, "ShedState");
    addVariable(mShedOn, "ShedOn");
    addVariable(mShedOff, "ShedOff");

    // Expressions from variables
    for (uint64_t t = 0; t < mHorizon; t++) {
        mExpPowerShedding[t] += mVarPowerShedding(t);

        mExpShedState[t] += mShedState(t);
        mExpShedOn[t] += mShedOn(t);
        mExpShedOff[t] += mShedOff(t);
    }

    // Constraints to link binaries ShedOn and ShedOff with binary State    
    qInfo() << "Initial shedding state: " << mShedStateIni;

    for (uint64_t t = 0; t < mHorizon; t++) {
        if (t > 0) {
            addConstraint(mExpShedState[t] - mExpShedState[t - 1] - mExpShedOn[t] + mExpShedOff[t] == 0, "ShedState", t);
        }
        else {
            addConstraint(mExpShedState[t] - mShedStateIni - mExpShedOn[t] + mExpShedOff[t] == 0, "ShedState", t);
        }

        addConstraint(mExpShedOn[t] <= mExpShedState[t], "ShedOn", t);
        addConstraint(mExpShedOff[t] <= 1 - mExpShedState[t], "ShedOff", t);
    }

    // Load shedding
    for (uint64_t t = 0; t < mHorizon; ++t)
    {
        addConstraint(mExpPowerShedding[t] <= mExpFlux[t], "PositiveShed", t);

        double maxShedPower;
        if (mAddSheddingTS) {
            maxShedPower = fabs(mMaxSheddingTS[t]);
        }
        else {
            maxShedPower = mMaxShedding;
        }
        addConstraint(mExpPowerShedding[t] <= maxShedPower * mExpShedState[t], "MaxShedPower", t);

        mExpFlux[t] -= mExpPowerShedding[t];
    }

    // Max duration of activated shedding
    for (uint64_t t = mMaxTimeShedding - 1; t < mHorizon; t++) {
        MIPModeler::MIPExpression sumExp;
        sumExp = 0;

        for (uint64_t i = t - mMaxTimeShedding + 1; i <= t; i++) {
            sumExp += mExpShedOn[i];
        }

        addConstraint(sumExp >= mExpShedState[t], "MaxShedTime", t);

        sumExp.close(); 
    }

    // Enabling Rolling Horizon option
    if (mNpdtPast > 0 && mControl == "RollingHorizon")
    {
        for (uint64_t t = 0; t < mMaxTimeShedding - 2; t++)
        {
            MIPModeler::MIPExpression sumExp;

            for (uint64_t i = t - mMaxTimeShedding + 1; i <= t; i++)
            {
                if (i < 0)
                    sumExp += mExpHistOn[i + mNpdtPast];
                else
                {
                    sumExp += mExpShedOn[i];
                }
            }

            addConstraint(sumExp >= mExpShedState[t], "MaxShedTime", t);

            sumExp.close();
        }
    }

    // Min duration of deactivated shedding
    for (uint64_t t = mMinSheddingStandBy - 1; t < mHorizon; t++) {
        MIPModeler::MIPExpression sumExp;
        sumExp = 0;

        for (uint64_t i = t - mMinSheddingStandBy + 1; i <= t; i++) {
            sumExp += mExpShedOff[i];
        }
        addConstraint(sumExp <= 1 - mExpShedState[t], "MinShedStdBy", t);

        sumExp.close();
    }

    // Enabling Rolling Horizon option
    if (mNpdtPast > 0 && mControl == "RollingHorizon")
    {
      for (uint64_t t = 0; t < mMinSheddingStandBy - 2; t++)
      {
         MIPModeler::MIPExpression sumExp;

         for (uint64_t i = t - mMinSheddingStandBy + 1; i <= t; i++)
         {

            if (i < 0)
              sumExp += mExpShedOff[i + mNpdtPast];
            else
            {
              sumExp += mExpShedOff[i];
            }
         }

         addConstraint(sumExp <= 1 - mExpShedState[t], "MinShedStdBy", t);

         sumExp.close();
      }
    }

    // Shedding penalty
    if (mAddVariableCostModel) {
        double shedCost = 0;
        for (uint64_t t = 0; t < mHorizon; t++) {
            if (mAddSheddingTS) {
                shedCost = mCostSheddingTS[t];
            }
            else {
                shedCost = mCostShedding;
            }
            mExpCostShedding[t] += shedCost * mExpPowerShedding[t];

            //mExpVariableCosts[t] += mExpCostShedding[t]; //moved to computeEconomicalContribution
        }
    }
}

void SourceLoad::addPeakShaving() {
    //PRE PROCESSING
    setExpSizeMax(mMaxFlux + fabs(mMaxEffect), "MaxFlux");

    std::vector<double>::iterator itmax = std::max_element(mImposedFlux.begin(), mImposedFlux.end());      //find the maximum value of given flux
    int index = std::distance(mImposedFlux.begin(), itmax);

    double mMaxImposedFlux = mImposedFlux[index];
    for (uint64_t t = 0; t < mHorizon; ++t) { 										  // the given flux is scaled so that its maximum is mMaxFlux
        mImposedFlux[t] = (mImposedFlux[t] / mMaxImposedFlux) * mMaxFlux;
    }

    //VARIABLES
    mVarFluxGrid = MIPModeler::MIPVariable1D(mHorizon, 0., mMaxFlux + fabs(mMaxEffect));
    addVariable(mVarFluxGrid, "VarFluxGrid");

    mVarMaxEffect = MIPModeler::MIPVariable0D(0., fabs(mMaxEffect));
    addVariable(mVarMaxEffect, "VarMaxEffect");

    //TEMPORARY MIP EXPRESSIONS
    mExpSums = MIPModeler::MIPExpression1D(mHorizon / mTimeSpan + 1); 				//intermediate step to store sums of variables

    //CONSTRAINTS
    if (mMaxEffect > 0.)
        addConstraint(mVarMaxEffect == mMaxEffect, "DefMaxEffect");
    for (uint64_t t = 0; t < mHorizon; ++t) {
        //        addConstraint(mVarFluxGrid(t) - mImposedFlux[t] <= mVarMaxEffect) ;
        addConstraint(mVarFluxGrid(t) - mVarMaxEffect <= mImposedFlux[t], "FluxGridBound");
        //        addConstraint(mVarFluxGrid(t) - mImposedFlux[t] >= - mVarMaxEffect) ;
        addConstraint(mVarFluxGrid(t) + mVarMaxEffect >= mImposedFlux[t], "FluxGridBound2");
        addConstraint(mVarFluxGrid(t) - mExpSizeMax <= 0, "FluxGridMaxBound");
    }

    double mSum;
    for (uint64_t p = 0; p < mHorizon / mTimeSpan + 1; ++p) {
        mSum = 0.;
        for (unsigned int t = 0; t < std::min<unsigned int>(mTimeSpan, mHorizon - p * mTimeSpan); ++t) {
            mExpSums[p] += mVarFluxGrid(mTimeSpan * p + t);
            mSum += mImposedFlux[mTimeSpan * p + t];
        }
        addConstraint(mExpSums[p] == mSum, "Sums");
    }

    //SHARE AS MIP EXPRESSIONS

    mExpFlux = MIPModeler::MIPExpression1D(mHorizon);
    mExpPowerOut = MIPModeler::MIPExpression1D(mHorizon);
    for (uint64_t t = 0; t < mHorizon; ++t) {
        mExpFlux[t] += mVarFluxGrid(t);
    }
    for (uint64_t t = 0; t < mHorizon; ++t) {
        mExpPowerOut[t] += mExpFlux[t];
    }


    /** Compute all expressions */
    computeAllContribution();

    mAllocate = false;
}

void SourceLoad::computeAllIndicators(const double* optSol)
{
    SourceLoadSubModel::computeDefaultIndicators(optSol);
}

