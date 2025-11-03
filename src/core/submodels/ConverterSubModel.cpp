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
    mAgeingModel = new AgeingRunningHours(mInputParam, mInputData);
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

    addVariable(mZ, "Pmin");

    if (!mLPModelOnly)
    {
        for (uint64_t t = 0; t < mHorizon; t++)
        {
            addConstraint(aPower[t] - fabs(aNomPower) * mExpState[t] <= 0, "PowMax", t);
            addConstraint(mZ(t) == aMinPowList[t] * mExpSizeMax, "DefPmin", t);
            addConstraint(aPower[t] >= mZ(t) - (1 - mExpState[t]) * aMinPowList[t] * fabs(aNomPower), "Pmin", t);


        }
    }
    else {
        for (uint64_t t = 0; t < mHorizon; t++)
        {
            addConstraint(aPower[t] >= aMinPowList[t] * mExpSizeMax, "MinPowerLPModOnly", t);
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
        mEfficiency_Ageing.at(0) = mEfficiency_Ageing.at(1) = Efficiency();
    }

    //Save optimal size from the current cycle
    if (mOptimalSize.size() > 0)
        mOptimalSizeAllCycles.push_back(mOptimalSize.at(0));

        
    bool firstPort = true;
    double memRunningTime = 0.;
    // Calcul running time

    for (auto& port : mListPort) {
        std::string varName = port->Variable();
        

        if (port->VarType() == "vector")
        {
            MIPModeler::MIPExpression1D variable = *getMIPExpression1D(varName);
            if (firstPort && port->Direction() == GS::KPROD()) {
                memRunningTime = mRunningTime.at(0);
                computeTime(true, mHorizon, mNpdtPast, variable, optSol, mRunningTime.at(0));
                computeTime(false, *mptrTimeshift, mNpdtPast, variable, optSol, mRunningTime.at(1));
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

    for (auto &port : mListPort) {    
        std::string portName = port->Name();
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
            if (port->Direction() == GS::KPROD()) {
                computeProduction(true, mHorizon, mNpdtPast, variable, optSol, aPort, bPort, mProductionMap[portName].at(0));
                computeProduction(false, *mptrTimeshift, mNpdtPast, variable, optSol, aPort, bPort, mProductionMap[portName].at(1));
                computeLvlProduction(true, mHorizon, mNpdtPast, variable, optSol, aPort, bPort, mProdLvlTotMap[portName].at(0));
                computeLvlProduction(false, *mptrTimeshift, mNpdtPast, variable, optSol, aPort, bPort, mProdLvlTotMap[portName].at(1));

                for (int i = 0; i < 2; ++i) mRunningTimeAvlblt.at(i) = mRunningTime.at(i) / mMaxRunningTime.at(i); // non
                for (int i = 0; i < 2; ++i) if (mRunningTime.at(i) > 1.e-20) mProdMeanMap[portName].at(i) = mProductionMap[portName].at(i) / mRunningTime.at(i);
            }
            else if (port->Direction() == GS::KCONS()) {
                computeConsumption(true, mHorizon, mNpdtPast, variable, optSol, aPort, bPort, mConsumptionMap[portName].at(0)); // plan
                computeConsumption(false, *mptrTimeshift, mNpdtPast, variable, optSol, aPort, bPort, mConsumptionMap[portName].at(1));
                computeLvlConsumption(true, mHorizon, mNpdtPast, variable, optSol, aPort, bPort, mConsLvlTotMap[portName].at(0));
                computeLvlConsumption(false, *mptrTimeshift, mNpdtPast, variable, optSol, aPort, bPort, mConsLvlTotMap[portName].at(1));

                for (int i = 0; i < 2; ++i) if (mRunningTime.at(i) > 1.e-20) mConsMeanMap[portName].at(i) = mConsumptionMap[portName].at(i) / mRunningTime.at(i);
                for (int i = 0; i < 2; ++i) if (mOptimalSize.at(i) > 1.e-20) mConsPFMap[portName].at(i) = -mConsMeanMap[portName].at(i) / mOptimalSize.at(i);
                for (int i = 0; i < 2; ++i) if (mOptimalSize.at(i) > 1.e-20) mRateOfUse[portName].at(i) = -mConsumptionMap[portName].at(i) / (mOptimalSize.at(i)*mMaxRunningTime.at(i));
            }
            else if (port->Direction() == GS::KDATA()) {
                computeConsumption(true, mHorizon, mNpdtPast, variable, optSol, aPort, bPort, mExpEchData[portName].at(0));
                computeConsumption(false, *mptrTimeshift, mNpdtPast, variable, optSol, aPort, bPort, mExpEchData[portName].at(1));
            }
        }
    } 
}

void ConverterSubModel::cleanFluxIOs(std::string name) 
{
    if (name != "INPUTFlux" && name != "OUTPUTFlux")
        return;
    for (auto& [key, vIO] : mIOExpressions) {
        if (CairnUtils::contains( key, name) && key != name + "1")
        {
            bool vOK = false;
            for (int i = 1; i < mNbInputFlux; i++)
            {
                if (key == name + std::to_string(i + 1)) {
                    vOK = true;
                    break;
                }
            }
            if (!vOK) {//=> key == name + "j", where j > mNbInputFlux/mNbOutputFlux
                for(MilpPort * lptrport: mListPort)
                {
                    if (lptrport->Variable() == key) {
                        std::string paramName;
                        std::string paramValue;
                        if (name == "INPUTFlux") {
                            paramName = "NbInputFlux";
                            paramValue = std::to_string(mNbInputFlux);
                        }
                        else {
                            paramName = "NbOutputFlux";
                            paramValue = std::to_string(mNbOutputFlux);
                        }
                        Cairn_Exception error("ERROR at " + Name()  + ": " + paramName + " cannot be set to " + paramValue + " because " + key + " is used at port " + lptrport->ID() + "(" + lptrport->Name()+")", -1);
                        throw error;
                    }
                }
                //delete
                removeIO(key);
            }
        }
    }
}

void ConverterSubModel::declareInputFluxIOs(MilpPort* defaultPort)
{
    if (defaultPort == nullptr)
        defaultPort = getPort("PortINPUTFlux1");

    //Delete IOs with index > mNbInputFlux !!
    ConverterSubModel::cleanFluxIOs("INPUTFlux");

    //Add INPUTFlux IOs
    mExpInput.resize(mNbInputFlux);
    for (int i = 1; i < mNbInputFlux; i++)
    {
        if (getIOExpression("INPUTFlux" + std::to_string(i + 1)) == nullptr) {
            bool found = false;
            //Look if there is a port whose Variable = "INPUTFlux" + std::to_string(i + 1)
            for(MilpPort * lptrport: mListPort)//InnerLoop
            {
                if (lptrport->Variable() == "INPUTFlux" + std::to_string(i + 1))
                {
                    addIO("INPUTFlux" + std::to_string(i + 1), &mExpInput[i], true, lptrport->pFluxUnit()); /** Computed input flow at port N_i */
                    found = true;
                    break; //InnerLoop
                }
            }
            if (!found) {//Use default port mPortINPUTFlux1. Don't use a dynamic unit!
                std::string unit = "FluxUnit";
                if (defaultPort) unit = defaultPort->getCarrier()->FluxUnit();
                addIO("INPUTFlux" + std::to_string(i + 1), &mExpInput[i], true, unit); /** Computed input flow at port N_i */
            }
        }
    }
}

void ConverterSubModel::declareOutputFluxIOs(MilpPort* defaultPort) 
{
    if (defaultPort == nullptr)
        defaultPort = getPort("PortOUTPUTFlux1");

    //Delete IOs with index > mNbOutputFlux !!
    ConverterSubModel::cleanFluxIOs("OUTPUTFlux");

    //Add OUTPUTFlux IOs
    mExpOutput.resize(mNbOutputFlux);
    for (int i = 1; i < mNbOutputFlux; i++)
    {
        if (getIOExpression("OUTPUTFlux" + std::to_string(i + 1)) == nullptr) {
            bool found = false;
            //Look if there is a port whose Variable = "OUTPUTFlux" + std::to_string(i + 1)
            for(MilpPort * lptrport: mListPort)//InnerLoop
            {
                if (lptrport->Variable() == "OUTPUTFlux" + std::to_string(i + 1))
                {
                    addIO("OUTPUTFlux" + std::to_string(i + 1), &mExpOutput[i], true, lptrport->pFluxUnit()); /** Computed output flow at port N_i */
                    found = true;
                    break; //InnerLoop
                }
            }
            if (!found) {//Use default port PortOUTPUTFlux1 ! Don't use a dynamic unit!
                std::string unit = "FluxUnit";
                if (defaultPort) unit = defaultPort->getCarrier()->FluxUnit();
                addIO("OUTPUTFlux" + std::to_string(i + 1), &mExpOutput[i], true, unit); /** Computed output flow at port N_i */
            }
        }
    }
}