#include "GlobalSettings.h"
#include "base/TecEcoEnv.h"
#include <QFile>
#include <QDebug>

using namespace GS ;

TecEcoEnv::TecEcoEnv(QObject* aParent, QString aName,
    const double aDiscountRate,
    const double aImpactDiscountRate,
    const unsigned int aNbYear,
    const unsigned int aNbYearInput,
    const unsigned int aLeapYearPos,
    const double aExtrapolationFactor,
    QString aRange) : QObject(aParent),
    mCurrency("EUR"),
    mObjectiveUnit("EUR"),
    mNbYear(aNbYear),
    mNbYearOffset(0),
    mNbYearInput(aNbYearInput),
    mLeapYearPos(aLeapYearPos),
    mDiscountRate(aDiscountRate),
    mImpactDiscountRate(aImpactDiscountRate),
    mExtrapolationFactor(aExtrapolationFactor),
    mInternalRateOfReturn(-1.),
    mRateOfReturnDiscountFactor(1.),
    mEnvImpactsList()
{
    this->setObjectName(aName);

    /* TecEco Analysis */
    mRange=aRange ;
    mBusEnergyBalance = 0. ;
    mOptimalSize = 0. ;
    mChargingTime = 0. ;
    mDischargingTime = 0. ;
    mChargedEnergy = 0. ;
    mDischargedEnergy = 0. ;
    mNLevChargedEnergy = 0. ;
    mNLevDischargedEnergy = 0. ;
    //Converter analysis over time horizon (no projection)
    mRunnningTime = 0. ;
    mContractualEnergy = 0. ;
    mNLevContractualEnergy = 0. ;
    //Contractual Load/Grid analysis over time horizon (no projection)
    mFreeEnergy = 0. ;
    mNLevFreeEnergy = 0. ;

    // History cumulated data
    mHistBusEnergyBalance = 0. ;
    mHistChargingTime = 0. ;
    mHistDischargingTime = 0. ;
    mHistChargedEnergy = 0. ;
    mHistDischargedEnergy = 0. ;
    mHistNLevChargedEnergy = 0. ;
    mHistNLevDischargedEnergy = 0. ;
    //Converter analysis over time horizon (no projection)
    mHistRunningTime = 0. ;
    mHistProduction = 0.;
    mHistConsumption = 0. ;
    mHistNLevProduction = 0.;
    mHistNLevConsumption = 0. ;
    //Contractual Load/Grid analysis over time horizon (no projection)
    mHistContractualEnergy = 0. ;
    mHistNLevContractualEnergy = 0. ;
    mHistContractualEnergyOptimal = 0. ;
    //Contractual Load/Grid analysis over time horizon (no projection)
    mHistFreeEnergy = 0. ;
    mHistNLevFreeEnergy = 0. ;

    mProductionContribution = 0. ;
    mHistProductionContribution = 0. ;

    mHistNbHours = 0.;
}
TecEcoEnv::~TecEcoEnv()
{
    if (mGUIData) delete mGUIData;
} // ~TecEcoEnv()

void TecEcoEnv::setCurrency (QString aCurrency)
{
    if (aCurrency != "") {
       mCurrency = aCurrency ;
    }
}
void TecEcoEnv::setRange (const QString aRange)
{
    if (aRange != "") {
       mRange = aRange ;
    }
}
void TecEcoEnv::setNbYear (const int aNbYear)
{
     mNbYear = aNbYear;
}
void TecEcoEnv::setNbYearOffset (const int aNbYearOffset)
{
    mNbYearOffset = aNbYearOffset;
}

void TecEcoEnv::setNbYearInput (const int aNbYearInput)
{
    mNbYearInput = aNbYearInput;
}

void TecEcoEnv::setLeapYearPos (const int aLeapYearPos)
{
    mLeapYearPos = aLeapYearPos;
}

void TecEcoEnv::setDiscountRate (const double aDiscountRate)
{
    mDiscountRate = aDiscountRate;
}

void TecEcoEnv::setImpactDiscountRate(const double aImpactDiscountRate)
{
    mImpactDiscountRate = aImpactDiscountRate;
}

void TecEcoEnv::setEnvImpactsList(const QStringList aEnvImpactsList)
{
    mEnvImpactsList = aEnvImpactsList;
}
void TecEcoEnv::setEnvImpactsShortNamesList(const QStringList aEnvImpactsShortNamesList)
{
    mEnvImpactsShortNamesList = aEnvImpactsShortNamesList;
}
void TecEcoEnv::setEnvImpactUnitsList(const QStringList aEnvImpactUnitsList)
{
    mEnvImpactUnitsList = aEnvImpactUnitsList;
}
void TecEcoEnv::setEnvImpactCosts(const std::vector<double> aEnvImpactCosts)
{
    mEnvImpactCosts = aEnvImpactCosts;
}


void TecEcoEnv::setInternalRateOfReturn (const QString aInternalRateOfReturn)
{
    if (aInternalRateOfReturn != "") {
        mInternalRateOfReturn = aInternalRateOfReturn.toDouble() ;
    }
}
void TecEcoEnv::setInternalRateOfReturn (const double aInternalRateOfReturn)
{
        mInternalRateOfReturn = aInternalRateOfReturn ;
}

void TecEcoEnv::setExtrapolationFactor(const double aExtrapolationFactor)
{
    mExtrapolationFactor = aExtrapolationFactor;
}

void TecEcoEnv::computeTecEcoEnvAnalysis()
{
}

void TecEcoEnv::setLevelizationTable(){
    mLevelizationTable = levelizationTable(mDiscountRate, mNbYear, mNbYearInput, mLeapYearPos, mNbYearOffset, mExtrapolationFactor);
}

void TecEcoEnv::setImpactLevelizationTable() {
    mImpactLevelizationTable = levelizationTable(mImpactDiscountRate, mNbYear, mNbYearInput, mLeapYearPos, mNbYearOffset, mExtrapolationFactor);
}

void TecEcoEnv::setTableYearsHours(){
    mTableYearsHours = yearHourTable(mNbYearInput,mLeapYearPos);
}

