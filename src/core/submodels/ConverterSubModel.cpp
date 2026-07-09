#include "ConverterSubModel.h"

ConverterSubModel::ConverterSubModel(CairnObject* aParent) :
TechnicalSubModel(aParent),
mUseAgeing(false),
mAgeingModel(nullptr)
{
}

ConverterSubModel::~ConverterSubModel()
{
    if (mAgeingModel) delete mAgeingModel;
}

void ConverterSubModel::declareInputParams(const std::string& name)
{
    SubModel::declareInputParams(name);
    mAgeingModel = new AgeingRunningHours(mInputParam, &mUseAgeing);
}

void ConverterSubModel::setTimeData()
{
    TechnicalSubModel::setTimeData();
    if (mUseAgeing && mAgeingModel) {
        mAgeingModel->setTimeData();
    }
}

void ConverterSubModel::computeAgeingModelContribution()
{
    if (mUseAgeing && mAgeingModel) {
        mAgeingModel->computeModelContribution();
    }
}

void ConverterSubModel::setMinPower(MIPModeler::MIPExpression1D aPower, std::vector<double> aMinPowList, double aNomPower) {
    /** Minimum power:
    * Linearization of function :math:`Z(t) = varMinPowerH2 * Yonoff(t) (linearization)`,  :math:`mPowerH2(t) >= varMinPowerH2 * YonOff(t)` with mPowerH2 >= Z
    */

    

    MIPModeler::MIPExpression1D aMinExpr(mTimeSteps.size());

    if (mWeight < 0) {
        for (uint64_t t = 0; t < mHorizon; t++)
        {
            aMinExpr[t] = aMinPowList[t] * aNomPower;
        }
    }
    else if (mWeight > 1) {
        for (uint64_t t = 0; t < mHorizon; t++)
        {
            aMinExpr[t] = aMinPowList[t] * mExpSizeMax / mWeight;
        }
    }
    else {
        for (uint64_t t = 0; t < mHorizon; t++)
        {
            aMinExpr[t] = aMinPowList[t] * mExpSizeMax;
        }
    }
    if (!mLPModelOnly)
    {
        addVariable(mZ, "Pmin");
        for (uint64_t t = 0; t < mHorizon; t++)
        {
            addConstraint(aPower[t] - fabs(aNomPower*mWeight)*mExpState[t] <= 0, "PowMax", t);
            addConstraint(mZ(t) == aMinExpr[t], "DefPmin", t);
            addConstraint(aPower[t] >= mZ(t) - (1 - mExpState[t]) * aMinPowList[t] * fabs(aNomPower * mWeight), "Pmin", t);


        }
    }
    else {
        for (uint64_t t = 0; t < mHorizon; t++)
        {
            addConstraint(aPower[t] >= aMinExpr[t], "MinPowerLPModOnly", t);
        }
    }
}

void ConverterSubModel::setMinPower(MIPModeler::MIPExpression1D aPower, double aMinPow, double aNomPower) {

    std::vector<double> aMinPowerTS;
    aMinPowerTS.resize(mHorizon);
    for (uint64_t t = 0; t < mHorizon; ++t) {
        aMinPowerTS[t] = aMinPow;
    }
    setMinPower(aPower, aMinPowerTS, aNomPower);
}

void ConverterSubModel::computeDefaultIndicators(const double* optSol)
{
    TechnicalSubModel::computeDefaultIndicators(optSol);

    mRunningTime.at(0) = 0.;
    mSumUp.at(0) = mSumUp.at(1) = mParentCompo->startingAbsoluteTimeStep();

    std::vector<double> meanValue = std::vector<double>(2, 0.);
    mMaxRunningTime.at(0) = 0;
    for (uint64_t t = 0; t < mHorizon; ++t) mMaxRunningTime.at(0) += TimeStep(t) * mParentCompo->ExtrapolationFactor(); // fichier plan: extrapolé
    mMaxRunningTime.at(1) += mNpdtPast * TimeStep(0); // fichier hist, cumulé 

    if (mUseAgeing) {
        mEfficiency_Ageing.at(0) = mEfficiency_Ageing.at(1) = EfficiencyAgeing();
    }

    //Save optimal size from the current cycle
    if (mOptimalSize.size() > 0) {
        mOptimalSizeAllCycles.push_back(mOptimalSize.at(0));
    }

    // Caluculate running time
    bool firstPort = true;
    double memRunningTime = 0.;
    for (const  auto& port : mListPort) {
        const MIPModeler::MIPExpression1D* ptrExp1D = getMIPExpression1D(port->Variable());
        if (ptrExp1D) {
            if (firstPort && port->Direction() == GS::KPROD()) 
            {
                memRunningTime = mRunningTime.at(0);
                computeTime(true, mHorizon, mNpdtPast, *ptrExp1D, optSol, mRunningTime.at(0));
                computeTime(false, *mptrTimeshift, mNpdtPast, *ptrExp1D, optSol, mRunningTime.at(1));
                if (mRunningTime.at(0) > memRunningTime) {
                    firstPort = false;
                }
            }
        }
    }

    // set running to Ageing
    if (mUseAgeing && mAgeingModel) {
        mAgeingModel->setHistRunningTime(mRunningTime.at(1));
    }

    for (const auto &port : mListPort) {
        const std::string portId = port->ID();
        const double aPort = port->VarCoeff();
        const double bPort = port->VarOffset();

        const MIPModeler::MIPExpression1D* ptrExp1D = getMIPExpression1D(port->Variable());
        if (ptrExp1D) {
            if (port->Direction() == GS::KPROD()) 
            {
                computeProduction(true, mHorizon, mNpdtPast, *ptrExp1D, optSol, aPort, bPort, mProductionMap[portId].at(0));
                computeProduction(false, *mptrTimeshift, mNpdtPast, *ptrExp1D, optSol, aPort, bPort, mProductionMap[portId].at(1));
                computeLvlProduction(true, mHorizon, mNpdtPast, *ptrExp1D, optSol, aPort, bPort, mProdLvlTotMap[portId].at(0));
                computeLvlProduction(false, *mptrTimeshift, mNpdtPast, *ptrExp1D, optSol, aPort, bPort, mProdLvlTotMap[portId].at(1));

                for (int i = 0; i < 2; ++i) mRunningTimeAvlblt.at(i) = mRunningTime.at(i) / mMaxRunningTime.at(i); // non
                for (int i = 0; i < 2; ++i) if (mRunningTime.at(i) > 1.e-20) mProdMeanMap[portId].at(i) = mProductionMap[portId].at(i) / mRunningTime.at(i);
            }
            else if (port->Direction() == GS::KCONS()) 
            {
                computeConsumption(true, mHorizon, mNpdtPast, *ptrExp1D, optSol, aPort, bPort, mConsumptionMap[portId].at(0)); // plan
                computeConsumption(false, *mptrTimeshift, mNpdtPast, *ptrExp1D, optSol, aPort, bPort, mConsumptionMap[portId].at(1));
                computeLvlConsumption(true, mHorizon, mNpdtPast, *ptrExp1D, optSol, aPort, bPort, mConsLvlTotMap[portId].at(0));
                computeLvlConsumption(false, *mptrTimeshift, mNpdtPast, *ptrExp1D, optSol, aPort, bPort, mConsLvlTotMap[portId].at(1));

                for (int i = 0; i < 2; ++i) if (mRunningTime.at(i) > 1.e-20) mConsMeanMap[portId].at(i) = mConsumptionMap[portId].at(i) / mRunningTime.at(i);
                for (int i = 0; i < 2; ++i) if (mOptimalSize.at(i) > 1.e-20) mConsPFMap[portId].at(i) = -mConsMeanMap[portId].at(i) / mOptimalSize.at(i);
                for (int i = 0; i < 2; ++i) if (mOptimalSize.at(i) > 1.e-20) mRateOfUse[portId].at(i) = -mConsumptionMap[portId].at(i) / (mOptimalSize.at(i)*mMaxRunningTime.at(i));
            }
            else if (port->Direction() == GS::KDATA()) 
            {
                computeConsumption(true, mHorizon, mNpdtPast, *ptrExp1D, optSol, aPort, bPort, mExpEchData[portId].at(0));
                computeConsumption(false, *mptrTimeshift, mNpdtPast, *ptrExp1D, optSol, aPort, bPort, mExpEchData[portId].at(1));
            }
        }
    } 
}

void ConverterSubModel::cleanFluxIOs(const std::string& base)
{
    if (base != "INPUTFlux" && base != "OUTPUTFlux")
        return;

    const int maxCount = (base == "INPUTFlux" ? mNbInputFlux : mNbOutputFlux);

    for (auto it = mIOExpressions.begin(); it != mIOExpressions.end(); )
    {
        const std::string& key = it->first;

        // Skip non-matching keys and skip base + "1"
        if (!CairnUtils::contains(key, base) || key == fluxName(base, 0)) {
            ++it;
            continue;
        }

        // Check if key is valid (base + index)
        bool valid = false;
        for (int i = 0; i < maxCount; ++i) {
            if (key == fluxName(base, i)) {
                valid = true;
                break;
            }
        }

        if (valid) {
            ++it;
            continue;
        }

        // Invalid key: base + j where j > maxCount
        // Check if a port still uses this variable
        for (MilpPort* port : mListPort)
        {
            if (port && port->Variable() == key)
            {
                const std::string paramName = (base == "INPUTFlux" ? "NbInputFlux" : "NbOutputFlux");
                const std::string paramValue = std::to_string(maxCount);

                throw Cairn_Exception("ERROR at " + Name() + ": " + paramName + " cannot be set to " +
                    paramValue + " because " + key + " is used at port " + port->ID() + "(" + port->Name() + ")", -1);
            }
        }

        // Remove IO
        it = removeIO(it);   // deletes + erases + returns next iterator
    }
}

void ConverterSubModel::declareInputFluxIOs(MilpPort* defaultPort)
{
    if (!defaultPort)
        defaultPort = getPort("PortINPUTFlux1");

    cleanFluxIOs("INPUTFlux"); 

    mExpInput.resize(mNbInputFlux);

    for (int i = 0; i < mNbInputFlux; ++i)
    {
        const std::string name = fluxName("INPUTFlux", i);

        if (getIOExpression(name) != nullptr)
            continue;

        MilpPort* matchedPort = nullptr;

        for (MilpPort* port : mListPort)
        {
            if (port && port->Variable() == name) {
                matchedPort = port;
                break;
            }
        }

        if (matchedPort) {
            // Use the port's unit (dynamic)
            addIO(name, &mExpInput[i], true, matchedPort->pFluxUnit());
        }
        else {
            // Use OUTPUTFlux1's port unit
            std::string unit = "FluxUnit";
            if (defaultPort)
                unit = defaultPort->FluxUnit();

            addIO(name, &mExpInput[i], true, unit);
        }
    }
}

void ConverterSubModel::declareOutputFluxIOs(MilpPort* defaultPort)
{
    if (!defaultPort)
        defaultPort = getPort("PortOUTPUTFlux1");

    cleanFluxIOs("OUTPUTFlux");

    mExpOutput.resize(mNbOutputFlux);

    for (int i = 0; i < mNbOutputFlux; ++i)
    {
        const std::string name = fluxName("OUTPUTFlux", i);

        if (getIOExpression(name) != nullptr)
            continue;

        MilpPort* matchedPort = nullptr;

        for (MilpPort* port : mListPort)
        {
            if (port && port->Variable() == name) {
                matchedPort = port;
                break;
            }
        }

        if (matchedPort) {
            // Use the port's unit (dynamic)
            addIO(name, &mExpOutput[i], true, matchedPort->pFluxUnit());
        }
        else {
            // Use OUTPUTFlux1's port unit
            std::string unit = "FluxUnit";
            if (defaultPort && defaultPort->getCarrier())
                unit = defaultPort->getCarrier()->FluxUnit();

            addIO(name, &mExpOutput[i], true, unit);
        }
    }
}
