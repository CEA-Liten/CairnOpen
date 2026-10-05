/**
* \brief	elec converter model (simplified form)
* \version	1.1
* \author	Alain Ruby
* \date		17/01/2020
*/
#include "Converter.h"
extern "C" MODELS_DECLSPEC CairnObject * createModel(CairnObject * aParent)
{
    return new Converter(aParent);
}

Converter::Converter(CairnObject* aParent) 
    : ConverterSubModel(aParent),
    mPortPowerIn(nullptr),
    mPortPowerOut(nullptr),
    mMaxPowerOut(0.)
{
    mAddStateVariable = true; /* always add state constraints */
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

void Converter::timeSeriesMapEfficiency(const std::vector<std::vector<double>>& aPowerInSetPointVec,
    const std::vector<std::vector<double>>& aPowerOutSetPointVec, const bool aRelaxedFormSOE)
{
    // Verify vector sizes
    if (aPowerInSetPointVec.size() != aPowerOutSetPointVec.size()) {
        throw Cairn_Exception("ERROR: the number of InputSetPoints should be equal to the number of OutputSetPoints", -1);
    }

    const auto checkSetPointSize = [this](const std::vector<double>& aSetPoint, const std::string& aLabel, std::size_t aIndex) {
        if (aSetPoint.size() != mHorizon) {
            throw Cairn_Exception("ERROR: the size of " + aLabel + "#" + std::to_string(aIndex + 1) +
                " should be " + std::to_string(mHorizon) + ". It is " + std::to_string(aSetPoint.size()), -1);
        }
    };

    const std::size_t vecSize = aPowerInSetPointVec.size();
    for (std::size_t i = 0; i < vecSize; ++i) {
        checkSetPointSize(aPowerInSetPointVec[i], "InputSetPoint", i);
        checkSetPointSize(aPowerOutSetPointVec[i], "OutputSetPoint", i);
    }

    // Build the piecewise-linear mapping and constraint 
    std::vector<double> powerIn_t(vecSize);
    std::vector<double> powerOut_t(vecSize);

    for (uint64_t t = 0; t < mHorizon; ++t) 
    {
        for (std::size_t i = 0; i < vecSize; ++i) {
            powerIn_t[i] = aPowerInSetPointVec[i][t];
            powerOut_t[i] = aPowerOutSetPointVec[i][t];
        }
        mExpPower_Out[t] = MIPModeler::MIPPiecewiseLinearisation(*mModel, mExpPower_In[t], 
            powerIn_t, powerOut_t, "Power_In", MIPModeler::MIP_SOS, aRelaxedFormSOE);

        addConstraint(mExpPower_Out[t] == mPower_Out(t), "map1Delectro", t);
    }
}

void Converter::mapEfficiency(const std::vector<double>& aPowerInSetPoint, 
    const std::vector<double>& aPowerOutSetPoint, const bool aRelaxedFormSOE)
{
    for (uint64_t t = 0; t < mHorizon; ++t) 
    {
        mExpPower_Out[t] = MIPModeler::MIPPiecewiseLinearisation(*mModel, mExpPower_In[t], 
            aPowerInSetPoint, aPowerOutSetPoint, "Power_In", MIPModeler::MIP_SOS, aRelaxedFormSOE);

        addConstraint(mExpPower_Out[t]==mPower_Out(t), "map1Delectro",t);
    }
}

int Converter::checkConsistency()
{
    int ier = TechnicalSubModel::checkConsistency();

    if (mMinPower>1. || mMinPower<0.) {
        cCritical() << "ERROR (converter): "<<parent()->objectName() <<"Min power should be comprised between 0 and 1 but equal to " << mMinPower;
        return -1;
    }
    return ier;
}

void Converter::computeInitialData()
{
    setMinValue(mMinSize);
    setMaxValue(mMaxPower);
}

void Converter::computeModelContribution()
{
    mMaxPowerOut = mMaxPower * mEfficiency * mWeight;

    for (uint64_t t = 0; t < mHorizon; ++t) {
        mMaxPowerTS[t] = mConverterUpperBound[t];
        mMinPowerTS[t] = mMinPower * mConverterLowerBound[t];
    }

    addVariable(mPower_In,"PowIn", 0., fabs(mMaxPower*mWeight));
	addVariable(mPower_Out,"PowOut", 0., fabs(mMaxPowerOut));
   
    fillExpression(mExpPower_In, mPower_In);
    fillExpression(mExpPower_Out, mPower_Out);

    // constraints
    for (uint64_t t = 0; t < mHorizon; ++t) {
        addConstraint(mExpPower_In[t] <= fabs(mMaxPower*mWeight) * mMaxPowerTS[t], "PowerInMaxBound");
    }
    setMinPower(mExpPower_In, mMinPowerTS, mMaxPower);
    

    if (!mPiecewiseEfficiency && !mTimeSeriesPiecewiseEfficiency) {
        for (uint64_t t = 0; t < mHorizon; ++t) {
            addConstraint(mExpPower_In[t] * mEfficiency * mConverterCoeff[t] - mExpPower_Out[t] + mOffset * mEfficiency * mComponentAvailabilityTS[t] * mExpState[t] == 0,"PowEff", t);
        }
    }
    else if (mPiecewiseEfficiency) {
        mapEfficiency(mPowerInSetPoint, mPowerOutSetPoint, false);
    }
    else { //if(mTimeSeriesPiecewiseEfficiency)
        timeSeriesMapEfficiency(mPowerInSetPointVec, mPowerOutSetPointVec, false);
    }

    for (uint64_t t = 0; t < mHorizon; ++t) {
        addConstraint(mExpPower_In[t] - mExpSizeMax * mMaxPowerTS[t] * mComponentAvailabilityTS[t] <= 0, "PowVar", t);
    }
}


void Converter::computeEconomicalContribution()
{
    TechnicalSubModel::computeEconomicalContribution();
}

//-----------------------------------------------------------------------------
void Converter::computeAllIndicators(const double* optSol)
{
    ConverterSubModel::computeAllIndicators(optSol);
}