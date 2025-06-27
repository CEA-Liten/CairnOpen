/**
* \brief	elec converter model (simplified form)
* \version	1.1
* \author	Alain Ruby
* \date		17/01/2020
*/
#include "Converter.h"
extern "C" MODELS_DECLSPEC QObject * createModel(QObject * aParent)
{
    return new Converter(aParent);
}

Converter::Converter(QObject* aParent) 
    : ConverterSubModel(aParent),
    mPortPowerIn(nullptr),
    mPortPowerOut(nullptr),
    mMaxPowerOut(0.)
{
    mAddStateVariable = true;
}

Converter::~Converter() {}

void Converter::setTimeData() {
    ConverterSubModel::setTimeData();
    mConverterCoeff.resize(mHorizon);
	mConverterLowerBound.resize(mHorizon);
    mConverterUpperBound.resize(mHorizon);									  
    mMaxPowerTS.resize(mHorizon);
    mMinPowerTS.resize(mHorizon);
}

void Converter::timeSeriesMapEfficiency(std::vector<std::vector<double>> aPowerInSetPointVec, std::vector<std::vector<double>> aPowerOutSetPointVec, const bool aRelaxedFormSOE)
{
    //Verify vector sizes
    if (aPowerInSetPointVec.size() != aPowerOutSetPointVec.size()) {
        setException(Cairn_Exception("ERROR: the number of InputSetPoints should be equal to the number of OutputSetPoints", -1));
        return;
    }

    int aSize = aPowerInSetPointVec.size();
    for (int i = 0; i < aSize; i++) {
        if (aPowerInSetPointVec[i].size() != mHorizon) {
            setException(Cairn_Exception("ERROR: the size of InputSetPoint#" + QString::number(i + 1) + " Should be " + QString::number(mHorizon) + ". It is " + QString::number(aPowerInSetPointVec[i].size()), -1));
            return;
        }
        if (aPowerOutSetPointVec[i].size() != mHorizon) {
            setException(Cairn_Exception("ERROR: the size of OutputSetPoint#" + QString::number(i + 1) + " Should be " + QString::number(mHorizon) + ". It is " + QString::number(aPowerOutSetPointVec[i].size()), -1));
            return;
        }
    }

    //Use line by line
    std::vector<double> powerIn_t;
    std::vector<double> powerOut_t;
    for (uint64_t t = 0; t < mHorizon; t++) {
        powerIn_t.clear();
        powerOut_t.clear();

        for (int i = 0; i < aSize; i++) {
            powerIn_t.push_back(aPowerInSetPointVec[i][t]);
            powerOut_t.push_back(aPowerOutSetPointVec[i][t]);
        }

        mExpPower_Out[t] = MIPModeler::MIPPiecewiseLinearisation(*mModel, mExpPower_In[t], powerIn_t, powerOut_t, "Power_In",
            MIPModeler::MIP_SOS, aRelaxedFormSOE);
    }
    for (uint64_t t = 0; t < mHorizon; t++) {
        addConstraint(mExpPower_Out[t] == mPower_Out(t), "map1Delectro",t);
    }
}

void Converter::mapEfficiency(std::vector<double> aPowerInSetPoint , std::vector<double> aPowerOutSetPoint, const bool aRelaxedFormSOE)
{
    for (uint64_t t = 0; t < mHorizon; t++) {
        mExpPower_Out[t] = MIPModeler::MIPPiecewiseLinearisation(*mModel, mExpPower_In[t], aPowerInSetPoint, aPowerOutSetPoint, "Power_In", 
                                                                     MIPModeler::MIP_SOS, aRelaxedFormSOE);
    }
    for (uint64_t t = 0; t < mHorizon; t++){
        addConstraint(mExpPower_Out[t]==mPower_Out(t), "map1Delectro",t);
    }
}

int Converter::checkConsistency()
{
    int ier = TechnicalSubModel::checkConsistency();

    if (mMinPower>1. || mMinPower<0.) {
        qCritical() << "ERROR (converter): "<<parent()->objectName()<<"Min power should be comprised between 0 and 1 but equal to " << mMinPower;
        return -1;
    }
    return ier;
}
void Converter::buildModel() 
{
    setExpSizeMax(mMaxPower, "MaxPower");
    mMaxPowerOut = mMaxPower * mEfficiency;

    for (uint64_t t = 0; t < mHorizon; ++t) {
        mMaxPowerTS[t] = mConverterUpperBound[t];
        mMinPowerTS[t] = mMinPower * mConverterLowerBound[t];
    }

    if (mAllocate)
    {
        mPower_Out = MIPModeler::MIPVariable1D(mHorizon,0.,fabs(mMaxPowerOut));
        mPower_In = MIPModeler::MIPVariable1D(mHorizon, 0., fabs(mMaxPower));
        mExpPower_In = MIPModeler::MIPExpression1D(mHorizon);
        mExpPower_Out = MIPModeler::MIPExpression1D(mHorizon);


    }
    else
    {
        closeExpression1D(mExpPower_In);
        closeExpression1D(mExpPower_Out);
    }

    addVariable(mPower_In,"PowIn");
	addVariable(mPower_Out,"PowOut");
   

    fillExpression(mExpPower_In, mPower_In);
    fillExpression(mExpPower_Out, mPower_Out);

    // constraints
    for (uint64_t t = 0; t < mHorizon; ++t) {
        addConstraint(mExpPower_In[t] <= fabs(mMaxPower) * mMaxPowerTS[t], "PowerInMaxBound");
    }

    addStateConstraints(mHorizon, mCondensedNpdt);

    setMinPower(mExpPower_In, mMinPowerTS, mMaxPower);

    if (!mPiecewiseEfficiency && !mTimeSeriesPiecewiseEfficiency) {
        for (uint64_t t = 0; t < mHorizon; ++t) {
            addConstraint(mExpPower_In[t] * mEfficiency * mConverterCoeff[t] - mExpPower_Out[t] + mOffset * mEfficiency * mConverterUse[t] * mExpState[t] == 0,"PowEff", t);
        }
    }
    else if (mPiecewiseEfficiency) {
        mapEfficiency(mPowerInSetPoint, mPowerOutSetPoint, false);
    }
    else { //if(mTimeSeriesPiecewiseEfficiency)
        timeSeriesMapEfficiency(mPowerInSetPointVec, mPowerOutSetPointVec, false);
    }

    for (uint64_t t = 0; t < mHorizon; ++t) {
        addConstraint(mExpPower_In[t] - mExpSizeMax * mMaxPowerTS[t] * mConverterUse[t] <= 0, "PowVar", t);
    }
    
    /** Compute all expressions */
    computeAllContribution();

    mAllocate = false ;
}


void Converter::computeEconomicalContribution()
{
    TechnicalSubModel::computeEconomicalContribution()  ;
}
//-----------------------------------------------------------------------------
void Converter::computeAllIndicators(const double* optSol)
{
    ConverterSubModel::computeDefaultIndicators(optSol);
}