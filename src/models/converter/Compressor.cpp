/**
* \file		compressor.cpp
* \brief	Economic model of compressor
* \version	1.0
* \author	Stephanie Crevon modified Pimprenelle Parmentier
* \date		06/12/2019
*/
#include "Compressor.h"
#include "CarrierTypes.h"
#include "OrUnitsConverter.h"
#include "MaterialCarrier.h"

extern "C" MODELS_DECLSPEC CairnObject * createModel(CairnObject * aParent)
{
    return new Compressor(aParent);
}

Compressor::Compressor(CairnObject* aParent) 
    : ConverterSubModel(aParent),
    mPortInMassFlowRate(nullptr),
    mPortOutMassFlowRate(nullptr),
    mPortUsedPower(nullptr),
    mMinPower(0.),
    mMaxPower(0.),
    mSpecificHeatRatio(0.),
    mCp_Gas(0.),
    mK(0.),
    mEta(0.),
    mPInlet(0.),
    mPOutlet(0.),
    mMaxMFR(2, 0.),
    mPowerUnit("MW"),
    mMassUnit("kg")
{
    std::string rsc = std::getenv("CAIRN_BIN") + (std::string)"/../resources/DefUnits.json";
    UnitsConverter::Load(rsc);
    CarrierTypes::Load(rsc);
}

Compressor::~Compressor() {}

int Compressor::checkConsistency()
{
    if (TechnicalSubModel::checkConsistency() < 0)
        return -1;

    // TIn and POut cannot both be optimized
    if (mUseVariableTIn && mUseVariablePOut) {
        cCritical() << "ERROR (Compressor " << Name()
            << "): TIn and POut cannot be optimized simultaneously. "
            << "Set either UseVariableTIn or UseVariablePOut to false.";
        return -1;
    }

    // NbStages must be strictly positive
    if (mNbStages <= 0.0) {
        cCritical() << "ERROR (Compressor " << Name()
            << "): NbStages must be strictly positive.";
        return -1;
    }

    // MotorEfficiency must be strictly positive
    if (mMotorEfficiency <= 0.0) {
        cCritical() << "ERROR (Compressor " << Name()
            << "): MotorEfficiency must be strictly positive.";
        return -1;
    }

    // Manual power consumption requires a non-zero value
    if (mSetManualPowerConsumption && mPowerConsumption <= 0.0) {
        cCritical() << "ERROR (Compressor " << Name()
            << "): PowerConsumption must be strictly positive "
            << "when SetManualPowerConsumption is true.";
        return -1;
    }

    return 0;
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
                * 1.e-6 / UnitsConverter::Convert(1, mPowerUnit, "MW") //MW default unit !
                * 1. / 3600 // kg/s
                * aCp_Gas
                * UnitsConverter::Convert(mTInlet, "DegC", "K")
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
                * 1.e-6 / UnitsConverter::Convert(1, mPowerUnit, "MW") //MW default unit !
                * 1. / 3600 // kg/s
                * aCp_Gas
                * UnitsConverter::Convert(PointsTIn[p], "DegC", "K")
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
    bool vOk = false;
    //Check default port first
    double vRet = mPortInMassFlowRate->getCarrierPressure(&vOk);
    if (!vOk) {
        //Look for any Input port with PotentialName == "Pressure" ! (should not be the case if the Compressor ports are correctly used).
        for(MilpPort * lptrport: mListPort)
        {
            if (lptrport->Direction() == KCONS()) {
                vRet = lptrport->getCarrierPressure(&vOk);
                if (vOk) break;
            }            
        }
    }        
    return vRet;
}

double Compressor::getOutletPressure()
{
    bool vOk = false;
    //Check default port first
    double vRet = mPortOutMassFlowRate->getCarrierPressure(&vOk);
    if (!vOk) {
        //Look for any Input port with PotentialName == "Pressure" ! (should not be the case if the Compressor ports are correctly used).
        for (MilpPort* lptrport : mListPort)
        {
            if (lptrport->Direction() == KPROD()) {
                vRet = lptrport->getCarrierPressure(&vOk);
                if (vOk) break;
            }
        }
    }
    return vRet;
}


void Compressor::computeInitialData()
{
    EnergyVector* vCarrier = mPortInMassFlowRate->getCarrier();
    MaterialCarrier* pCarrier = (MaterialCarrier*)vCarrier;

    if (!vCarrier) {
        throw Cairn_Exception("Error in Compressor " + Name() + ": no port InMassFlowRate", -1);
    }

    if (!pCarrier) {
        throw Cairn_Exception("Error in Compressor " + Name() + ": the port InMassFlowRate must be a MaterialCarrier", -1);
    }

    mPInlet = getInletPressure();
    mPOutlet = getOutletPressure();

    if (fabs(mPInlet) < 1.e-6) { 
        throw Cairn_Exception("Error in Compressor " + Name() + ": the pressure of the carrier " + mPortInMassFlowRate->CarrierName()
            + " used by the default port " + mPortInMassFlowRate->Name() + " cannot be 0.", -1);
    }

    mPowerUnit = *pCarrier->pQuantity("PowerUnit");
    mMassUnit = *pCarrier->pQuantity("MassUnit");
    
    mSpecificHeatRatio = pCarrier->SpecificHeatRatio();
    mCp_Gas = pCarrier->computeCp(mTInlet);   // Cp in J/DegC/kg

    // mK cannot be 0 due to a division
    if (mUsePolytropicModel) {
        if (fabs(mPolytropicCoefficient) < 1.e-6) {
            throw Cairn_Exception("Error in Compressor " + Name() + ": the parameter PolytropicCoefficient cannot be 0 when UsePolytropicModel is true.", -1);
        }
        mK = mPolytropicCoefficient;
        mEta = mPolytropicEfficiency;
    }
    else {
        if (fabs(mSpecificHeatRatio) < 1.e-6) {
            throw Cairn_Exception("Error in Compressor " + Name() + ": the SpecificHeatRatio cannot be 0 (coming from the carrier " + mPortInMassFlowRate->CarrierName()
                + " used by the default port " + mPortInMassFlowRate->Name() + ")", -1);
        }
        mK = mSpecificHeatRatio;
        mEta = mIsentropicEfficiency;
    }

    if (!mSetManualPowerConsumption) {
        mPowerConsumption = mNbStages
            * 1.e-6 / UnitsConverter::Convert(1, mPowerUnit, "MW") //MW default unit !
            * 1. / 3600 // kg/s
            * mCp_Gas
            * UnitsConverter::Convert(mTInlet, "DegC", "K")
            / mEta
            * (pow(mPOutlet / mPInlet, (mK - 1) / (mNbStages * mK)) - 1)
            / mMotorEfficiency * UnitsConverter::Convert(1, mMassUnit, "kg");
    }

    cInfo() << Name() + ": power consumption = " << mPowerConsumption;

    if (mUseSteamMap) {
        mMaxPower = *max_element(mUsedElecPowerSetPoint.begin(), mUsedElecPowerSetPoint.end());
        mMinPower = *min_element(mUsedElecPowerSetPoint.begin(), mUsedElecPowerSetPoint.end());
    }
    else {
        mMaxPower = mPowerConsumption * (mMaxFlow);     // puissance de dimensionnement max du composant. negative means optimization, absolute value gives max range value
        mMinPower = mPowerConsumption * abs(mMinFlow);     // puissance de dimensionnement max du composant. negative means optimization, absolute value gives max range value
    }

    setMinValue(mMinSize);
    setMaxValue(mMaxPower);

    mAddStateVariable = true; /* always add state constraints */
}


void Compressor::computeModelContribution()
{
    //variable
    addVariable(mUsedPower,"UsedPower", 0.f, abs(mMaxPower));
    addVariable(mMassFlow,"MassFlow", 0.f, abs(mMaxFlow));
    addVariable(mPOut,"POutComp", 0.f, abs(10000));

    if(mUseSteamMap) {
        addVariable(mSteam, "SteamCompressor", 0.f, abs(10000));
        fillExpression(mExpSteam, mSteam);
    }

    addVariable(mVarTOutlet, "Toutlet", 0.f, 3000.);

    //variables exprimed as expressions on horizon (optional)
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
            ComputeElecPowerMapPOut(mCp_Gas, mK, mEta, false, methode);
            for (uint64_t t = 0; t < mHorizon; ++t) {
                addConstraint(mExpUsedPower[t] - fabs(mMaxPower) * mComponentAvailabilityTS[t] * mExpState[t] <= 0, "UseComp", t);
                addConstraint(mExpUsedPower[t] - fabs(mMinPower) * mComponentAvailabilityTS[t] * mExpState[t] >= 0, "PowMin", t);
                addConstraint(mExpUsedPower[t] - mExpSizeMax * mComponentAvailabilityTS[t] <= 0, "Max", t);
            }
        }
        else if (mUseVariableTIn) {
            addVariable(mTIn, "STInCompressor", 0.f, abs(10000));
            fillExpression(mExpTIn, mTIn);
            ComputeElecPowerMapTIn(mCp_Gas, mK, mEta, false, methode);
            for (uint64_t t = 0; t < mHorizon; ++t) {
                addConstraint(mExpUsedPower[t] - fabs(mMaxPower) * mComponentAvailabilityTS[t] * mExpState[t] <= 0, "UseComp", t);
                addConstraint(mExpUsedPower[t] - fabs(mMinPower) * mComponentAvailabilityTS[t] * mExpState[t] >= 0, "PowMin", t);
                addConstraint(mExpUsedPower[t] - mExpSizeMax * mComponentAvailabilityTS[t] <= 0, "Max", t);
            }
        }
    }
    else if (mUseSteamMap) {
        computeUsedPower_Steam_PressureOut(false);
        for (uint64_t t = 0; t < mHorizon; ++t) {
            addConstraint(mExpInMassFlow[t]>=mExpState[t]*mMinFlow,"MinFlow",t);
            addConstraint(mExpInMassFlow[t]<= mExpState[t] *mMaxFlow,"MaxFlow",t);
            addConstraint(mExpUsedPower[t] - fabs(mMinPower) * mComponentAvailabilityTS[t] * mExpState[t] >= 0,"PowMin",t);
            addConstraint(mExpUsedPower[t] - fabs(mMaxPower) * mComponentAvailabilityTS[t] * mExpState[t] <= 0,"UseComp",t) ;
        }
    }
    else {
        for (uint64_t t = 0; t < mHorizon; ++t) {
            mExpUsedPower[t] += mUsedPower(t);
            addConstraint(mExpUsedPower[t] - mExpInMassFlow[t] * mPowerConsumption == 0.f, "PowComp", t);
        }
        for (uint64_t t = 0; t < mHorizon; ++t) {
            addConstraint(mExpUsedPower[t] - fabs(mMaxPower) * mComponentAvailabilityTS[t] * mExpState[t] <= 0,"UseComp",t) ;
            addConstraint(mExpUsedPower[t] - fabs(mMinPower) * mComponentAvailabilityTS[t] * mExpState[t] >= 0,"PowMin",t);
            addConstraint(mExpUsedPower[t] - mExpSizeMax * mComponentAvailabilityTS[t] <= 0,"Max",t) ;
        }
    }

    if (!mAddLosses) {
        mExpOutMassFlow = mExpInMassFlow;
    }
    else {
        addVariable(mMassFlowOut, "MassFlowOutCompressor", 0.f, abs(mMaxFlow));
        for (uint64_t t = 0; t < mHorizon; ++t) {
            mExpOutMassFlow[t]+=mMassFlowOut(t);
            addConstraint(mExpOutMassFlow[t] == mExpInMassFlow[t] * (1 - mLosses), "LossesCompressor");
        }
    }

    // compute constant outlet temperature
    addConstraint(mVarTOutlet == mTInlet * pow(mPOutlet / mPInlet, (mK - 1)/(mNbStages*mK)),"TOut",0);
    mExpTOutlet = MIPModeler::MIPExpression();
    mExpTOutlet = mVarTOutlet ;
}

void Compressor::computeEconomicalContribution()
{
    TechnicalSubModel::computeEconomicalContribution();
}

void Compressor::computeAllIndicators(const double* optSol)
{
    ConverterSubModel::computeDefaultIndicators(optSol);
    mMaxMFR.at(0) = mOptimalSize.at(0) / std::get<double>(mInputParam->getParameter("PowerConsumption")->getValue());
    mMaxMFR.at(1) = mOptimalSize.at(1) / std::get<double>(mInputParam->getParameter("PowerConsumption")->getValue());
}
