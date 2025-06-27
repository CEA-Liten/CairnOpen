/**
* \file		compressor.cpp
* \brief	Economic model of compressor
* \version	1.0
* \author	Stephanie Crevon modified Pimprenelle Parmentier
* \date		06/12/2019
*/
#include "Compressor.h"
extern "C" MODELS_DECLSPEC QObject * createModel(QObject * aParent)
{
    return new Compressor(aParent);
}

Compressor::Compressor(QObject* aParent) 
    : ConverterSubModel(aParent),
    mPortInMassFlowRate(nullptr),
    mPortOutMassFlowRate(nullptr),
    mPortUsedPower(nullptr),
    mMinPower(0.),
    mMaxPower(0.),
    mSpecificHeatRatio(0.),
    mPInlet(0.),
    mPOutlet(0.),
    mMaxMFR(2, 0.),
    mPowerUnit("MW"),
    mMassUnit("kg")
{
}

Compressor::~Compressor() {}

int Compressor::checkConsistency ()
{
    int ier = TechnicalSubModel::checkConsistency();
    if (mUseVariablePOut && mUseVariableTIn){
        qCritical() << "ERROR (compressor): it is not possible to optimize TIn and POut as the same time. Please put UseVariableTIn or UseVariablePOut on false" ;
        return -1 ;
    }
    return ier ;
}

void Compressor::ComputeElecPowerMapPOut(double aCp_Gas, double ak, double aEta, const bool aRelaxedFormSOE, const MIPModeler::MIPLinearType& methode){
    uint64_t nRows = mPrecisionPressure + 1;
    uint64_t  nCols = mPrecisionMassFlow + 1;
    //uint64_t vSize = nRows * nCols;

    std::vector<double> PointsPowerConsumption(nRows);
    MIPModeler::MIPData2D PointsElecPower(nRows, MIPModeler::MIPData1D(nCols));
    std::vector<double>  PointsMassFlow(nCols);
    std::vector<double>  PointsPOut(nRows);
    for (int p = 0; p < mPrecisionPressure + 1; p++){
        //PointsPOut[p] = mPInlet + p * (mPOutlet - mPInlet)/mPrecisionPressure;
        PointsPOut[p] = p * (mPOutletFixe)/mPrecisionPressure;
        PointsPowerConsumption[p] = mNbStages
                * 1.e-6 / EnergyVector::PowerToMW(mPowerUnit) //MW default unit !
                * 1. / 3600 // kg/s
                * aCp_Gas
                * EnergyVector::Deg2Kel(mTInlet)
                / aEta
                * (pow(PointsPOut[p] / mPInlet, (ak - 1)/(mNbStages*ak)) -1)
                / mMotorEfficiency;
    }
    for (int pmf = 0; pmf < mPrecisionMassFlow + 1; pmf++){
        //PointsMassFlow[pmf] = mMinFlow + pmf * (mMaxFlow - mMinFlow)/mPrecisionMassFlow;
        PointsMassFlow[pmf] = pmf * (mMaxFlow)/mPrecisionMassFlow;
    }
    for (int p = 0; p < mPrecisionPressure + 1; p++){
        for (int pmf = 0; pmf < mPrecisionMassFlow + 1; pmf++){
            PointsElecPower[p][pmf] = PointsMassFlow[pmf] * PointsPowerConsumption[p];
        }
    }

    mExpUsedPower = MIPModeler::MIPTriMeshLinearisation(*mModel, mExpPOut, mExpInMassFlow, PointsPOut,
                                                               PointsMassFlow, PointsElecPower,
                                                               methode, aRelaxedFormSOE);
    for(uint64_t t = 0; t<mHorizon; t++){
        addConstraint(mExpUsedPower[t] == mUsedPower(t),"PressionVariableCompr",t);
    }
}

void Compressor::ComputeElecPowerMapTIn(double aCp_Gas, double ak, double aEta, const bool aRelaxedFormSOE, const MIPModeler::MIPLinearType& methode){

    uint64_t nRows = mPrecisionTemperature + 1;
    uint64_t  nCols = mPrecisionMassFlow + 1;
    //uint64_t vSize = nRows * nCols;

    std::vector<double> PointsPowerConsumption(nRows);
    MIPModeler::MIPData2D PointsElecPower(nRows, MIPModeler::MIPData1D(nCols));
    std::vector<double>  PointsMassFlow(nCols);
    std::vector<double>  PointsTIn(nRows);
    for (int p = 0; p < mPrecisionTemperature + 1; p++){
        //PointsPOut[p] = mPInlet + p * (mPOutlet - mPInlet)/mPrecisionPressure;
        PointsTIn[p] = -5 + p * (100)/mPrecisionTemperature;
        PointsPowerConsumption[p] = mNbStages
                * 1.e-6 / EnergyVector::PowerToMW(mPowerUnit) //MW default unit !
                * 1. / 3600 // kg/s
                * aCp_Gas
                * EnergyVector::Deg2Kel(PointsTIn[p])
                / aEta
                * (pow(mPOutletFixe / mPInlet, (ak - 1)/(mNbStages*ak)) -1)
                / mMotorEfficiency ;
    }
    for (int pmf = 0; pmf < mPrecisionMassFlow + 1; pmf++){
        //PointsMassFlow[pmf] = mMinFlow + pmf * (mMaxFlow - mMinFlow)/mPrecisionMassFlow;
        PointsMassFlow[pmf] = pmf * (mMaxFlow)/mPrecisionMassFlow;
    }
    for (int p = 0; p < mPrecisionTemperature + 1; p++){
        for (int pmf = 0; pmf < mPrecisionMassFlow + 1; pmf++){
            PointsElecPower[p][pmf] = PointsMassFlow[pmf] * PointsPowerConsumption[p];
        }
    }

    mExpUsedPower = MIPModeler::MIPTriMeshLinearisation(*mModel, mExpTIn, mExpInMassFlow, PointsTIn,
                                                               PointsMassFlow, PointsElecPower,
                                                               methode, aRelaxedFormSOE);
    for(uint64_t t = 0; t<mHorizon; t++){
        addConstraint(mExpUsedPower[t] == mUsedPower(t),"TemperatureVariableComp",t);
    }
}
void Compressor::computeUsedPower_Steam_PressureOut(const bool aRelaxedFormSOE){
    uint64_t nRows = mSteamSetPoint.size();
    uint64_t  nCols = mPressureOutSetPoint.size();
    //uint64_t vSize = nRows * nCols;

    MIPModeler::MIPData2D elecPowerTable( nRows, MIPModeler::MIPData1D(nCols) );

    for (uint64_t i = 0; i < nRows; i++) {
        for (uint64_t j = 0; j < nCols; j++) {
            elecPowerTable[i][j] =  mUsedElecPowerSetPoint[i * nCols + j];
        }
    }

    //for (uint64_t t = 0; t < mNpdt; t++) {
      //  m[t] += mFlow_H2(t);
    //}
    MIPModeler::MIPLinearType methode;
    if (mUseLOG){
        methode = MIPModeler::MIP_LOG;
    }
    else{
        methode = MIPModeler::MIP_SOS;
    }

    mExpUsedPower = MIPModeler::MIPTriMeshLinearisation(*mModel, mExpSteam, mExpPOut, mSteamSetPoint, mPressureOutSetPoint,
                                                        elecPowerTable, methode, aRelaxedFormSOE);

    for (uint64_t t = 0; t < mHorizon; t++) {
        addConstraint((mExpUsedPower[t] == mUsedPower(t)),"TemperatureVariableComp",t);
    }

}

double Compressor::getInletPressure() 
{
    //Check default port first
    if (mPortInMassFlowRate->PotentialName() == "Pressure" && mPortInMassFlowRate->ptrEnergyVector() != nullptr)
    {
        return mPortInMassFlowRate->ptrEnergyVector()->Potential();
    }
    //Look for any Input port with PotentialName == "Pressure" ! (should not be the case if the Compressor ports are correctly used).
    foreach(MilpPort * lptrport, mListPort)
    {
        if (lptrport->Direction() == KCONS() && lptrport->PotentialName() == "Pressure" && lptrport->ptrEnergyVector() != nullptr)
        {
            return lptrport->ptrEnergyVector()->Potential();
        }
    }
    return 0.;
}

double Compressor::getOutletPressure()
{
    //Check default port first
    if (mPortOutMassFlowRate->PotentialName() == "Pressure" && mPortOutMassFlowRate->ptrEnergyVector() != nullptr)
    {
        return mPortOutMassFlowRate->ptrEnergyVector()->Potential();
    }

    //Look for any Output port with PotentialName == "Pressure" ! (should not be the case if the Compressor ports are correctly used).
    foreach(MilpPort * lptrport, mListPort)
    {
        if (lptrport->Direction() == KPROD() && lptrport->PotentialName() == "Pressure" && lptrport->ptrEnergyVector() != nullptr)
        {
            return lptrport->ptrEnergyVector()->Potential();
        }
    }
    return 0.;
}

void Compressor::buildModel() 
{
    mPInlet = getInletPressure();
    mPOutlet = getOutletPressure();

    if (mPortInMassFlowRate->ptrEnergyVector()) {
        QString vectorType = mPortInMassFlowRate->ptrEnergyVector()->Type();
        mSpecificHeatRatio = *(mPortInMassFlowRate->ptrEnergyVector()->pSpecificHeatRatio(vectorType));
        mPowerUnit = mPortInMassFlowRate->ptrEnergyVector()->PowerUnit();
        mMassUnit = mPortInMassFlowRate->ptrEnergyVector()->MassUnit();
    }

    EV::Fluid_Type Type = EnergyVector::getFluidTypeFromQString(mEnergyVector->Type()) ;

    double Cp_Gas = EnergyVector::Compute_Cp(mTInlet, EnergyVector::Get_Pointer_To_Fluid_Properties(Type));   // Cp in J/DegC/kg
    double k ;
    double Eta ;

    if (mUsePolytropicModel) {
        k = mPolytropicCoefficient ;
        Eta = mPolytropicEfficiency ;
    }
    else {
        k = mSpecificHeatRatio ;
        Eta = mIsentropicEfficiency ;
    }

    mPowerConsumption = mNbStages
        * 1.e-6 / EnergyVector::PowerToMW(mPowerUnit) //MW default unit !
        * 1. / 3600 // kg/s
        * Cp_Gas
        * EnergyVector::Deg2Kel(mTInlet)
        / Eta
        * (pow(mPOutlet / mPInlet, (k - 1) / (mNbStages * k)) - 1)
        / mMotorEfficiency * EnergyVector::MassToKg(mMassUnit);

    if (isnan(mPowerConsumption)) {
        QString error_message = "A division by 0 is detected while computing Power Consumption of " + getCompoName() +".";
        if (fabs(mPInlet) < 1.e-6) {
            error_message += " The value of the parameter Potential of the carrier used by the first input port is 0.";
        }
        Cairn_Exception persee_error(error_message, -1);
        throw persee_error;
    }

    //variable
    if(mUseSteamMap) {
        mMaxPower = *max_element(mUsedElecPowerSetPoint.begin(), mUsedElecPowerSetPoint.end());
        mMinPower = *min_element(mUsedElecPowerSetPoint.begin(), mUsedElecPowerSetPoint.end());
    }
    else {
        mMaxPower = mPowerConsumption * (mMaxFlow);     // puissance de dimensionnement max du composant. negative means optimization, absolute value gives max range value
        mMinPower = mPowerConsumption * abs(mMinFlow);     // puissance de dimensionnement max du composant. negative means optimization, absolute value gives max range value
    }

    setExpSizeMax(mMaxPower, "MaxPower");
    addStateConstraints(mHorizon, mCondensedNpdt);

    mUsedPower = MIPModeler::MIPVariable1D(mHorizon, 0.f, abs(mMaxPower));
    mMassFlow = MIPModeler::MIPVariable1D(mHorizon, 0.f, abs(mMaxFlow));
    mPOut = MIPModeler::MIPVariable1D(mHorizon, 0.f, abs(10000));

    addVariable(mUsedPower,"UsedPower");
    addVariable(mMassFlow,"MassFlow");
    addVariable(mPOut,"POutComp");

    if(mUseSteamMap) {
        mSteam = MIPModeler::MIPVariable1D(mHorizon, 0.f, abs(10000));
        addVariable(mSteam, "SteamCompressor");
        mExpSteam = MIPModeler::MIPExpression1D(mHorizon);
        fillExpression(mExpSteam, mSteam);
    }

    mVarTOutlet = MIPModeler::MIPVariable0D( 0.f, 3000.);
    addVariable(mVarTOutlet, "Toutlet");

    //variables exprimed as expressions on horizon (optional)
    mExpUsedPower = MIPModeler::MIPExpression1D(mHorizon);
    mExpInMassFlow = MIPModeler::MIPExpression1D(mHorizon);
    mExpOutMassFlow = MIPModeler::MIPExpression1D(mHorizon);
    mExpPOut = MIPModeler::MIPExpression1D(mHorizon);

    fillExpression(mExpPOut, mPOut);
    fillExpression(mExpInMassFlow, mMassFlow);

    if (mUseVariablePOut || mUseVariableTIn) {
        MIPModeler::MIPLinearType methode;
        if (mUseLOG) {
            methode = MIPModeler::MIP_LOG;
        }
        else {
            methode = MIPModeler::MIP_SOS;
        }
        if (mUseVariablePOut) {
            ComputeElecPowerMapPOut(Cp_Gas, k, Eta, false, methode);
            for (uint64_t t = 0; t < mHorizon; ++t) {
                addConstraint(mExpUsedPower[t] - fabs(mMaxPower) * mConverterUse[t] * mExpState[t] <= 0, "UseComp", t);
                addConstraint(mExpUsedPower[t] - fabs(mMinPower) * mConverterUse[t] * mExpState[t] >= 0, "PowMin", t);
                addConstraint(mExpUsedPower[t] - mExpSizeMax * mConverterUse[t] <= 0, "Max", t);
            }
        }
        else if (mUseVariableTIn) {
            mTIn = MIPModeler::MIPVariable1D(mHorizon, 0.f, abs(10000));
            addVariable(mTIn, "STInCompressor");
            mExpTIn = MIPModeler::MIPExpression1D(mHorizon);
            fillExpression(mExpTIn, mTIn);
            ComputeElecPowerMapTIn(Cp_Gas, k, Eta, false, methode);
            for (uint64_t t = 0; t < mHorizon; ++t) {
                addConstraint(mExpUsedPower[t] - fabs(mMaxPower) * mConverterUse[t] * mExpState[t] <= 0, "UseComp", t);
                addConstraint(mExpUsedPower[t] - fabs(mMinPower) * mConverterUse[t] * mExpState[t] >= 0, "PowMin", t);
                addConstraint(mExpUsedPower[t] - mExpSizeMax * mConverterUse[t] <= 0, "Max", t);
            }
        }
    }
    else if (mUseSteamMap) {
        computeUsedPower_Steam_PressureOut(false);
        for (uint64_t t = 0; t < mHorizon; ++t) {
            addConstraint(mExpInMassFlow[t]>=mExpState[t]*mMinFlow,"MinFlow",t);
            addConstraint(mExpInMassFlow[t]<= mExpState[t] *mMaxFlow,"MaxFlow",t);
            addConstraint(mExpUsedPower[t] - fabs(mMinPower) * mConverterUse[t] * mExpState[t] >= 0,"PowMin",t);
            addConstraint(mExpUsedPower[t] - fabs(mMaxPower) * mConverterUse[t] * mExpState[t] <= 0,"UseComp",t) ;
        }
    }
    else {
        for (uint64_t t = 0; t < mHorizon; ++t) {
            mExpUsedPower[t] += mUsedPower(t);
            addConstraint(mExpUsedPower[t] - mExpInMassFlow[t] * mPowerConsumption == 0.f, "PowComp", t);
        }
        for (uint64_t t = 0; t < mHorizon; ++t) {
            addConstraint(mExpUsedPower[t] - fabs(mMaxPower) * mConverterUse[t] * mExpState[t] <= 0,"UseComp",t) ;
            addConstraint(mExpUsedPower[t] - fabs(mMinPower) * mConverterUse[t] * mExpState[t] >= 0,"PowMin",t);
            addConstraint(mExpUsedPower[t] - mExpSizeMax * mConverterUse[t] <= 0,"Max",t) ;
        }
    }

    if (!mAddLosses) {
        mExpOutMassFlow = mExpInMassFlow;
    }
    else {
        mMassFlowOut = MIPModeler::MIPVariable1D(mHorizon, 0.f, abs(mMaxFlow));
        addVariable(mMassFlowOut, "MassFlowOutCompressor");
        for (uint64_t t = 0; t < mHorizon; ++t) {
            mExpOutMassFlow[t]+=mMassFlowOut(t);
            addConstraint(mExpOutMassFlow[t] == mExpInMassFlow[t] * (1 - mLosses), "LossesCompressor");
        }
    }

    // compute constant outlet temperature
    addConstraint(mVarTOutlet == mTInlet * pow(mPOutlet / mPInlet, (k - 1)/(mNbStages*k)),"TOut",0);
    mExpTOutlet = MIPModeler::MIPExpression();
    mExpTOutlet = mVarTOutlet ;

    /** Compute all expressions */
    computeAllContribution();

    mAllocate = false ;
}
void Compressor::computeEconomicalContribution()
{
    TechnicalSubModel::computeEconomicalContribution()  ;
}

void Compressor::computeAllIndicators(const double* optSol)
{
    ConverterSubModel::computeDefaultIndicators(optSol);
    mMaxMFR.at(0) = mOptimalSize.at(0) / std::get<double>(mInputParam->getParameter("PowerConsumption")->getValue());
    mMaxMFR.at(1) = mOptimalSize.at(1) / std::get<double>(mInputParam->getParameter("PowerConsumption")->getValue());
}
