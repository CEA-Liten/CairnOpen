#include "base/MilpData.h"
#include "GlobalSettings.h"
#include <QDebug>

using namespace GS ;
// For mapping a vector subscribed in ZE to a VectorXf
#define ZE_IN(x) Map<VectorXf>(x.data(), x.size())

#define ZE_OUT(x,y) Map<VectorXf> y(x.data(), x.size());
//
// Pegase-type constructor with full args - time data set right now
//
MilpData::MilpData(QObject* aParent, const QString& aName, const double& aPdt,
    const uint& aNpdtPast,
    const uint& aNpdtFuture,
    const uint& aTimeshift, const uint& aIHMFuturSize, const QString& aGlobalTimeStepFile, const QString& aGlobalTypicalPeriodFile) : QObject(aParent),
    mPdt(aPdt),
    mPdtHeure(mPdt / 3600.),
    mNpdtPast(aNpdtPast),
    mNpdt(aNpdtFuture),
    mNpdtTot(aNpdtPast + aNpdtFuture),
    mTimeshift(aTimeshift),
    mIHMFuturSize(aIHMFuturSize),
    mNbCycle(1),
    mRollingMode("Periodic"),
    mReadingMode("Average"),
    mRunUntilSimulationEnd(false),
    mExportResultsEveryCycle(false),
    mUseExtrapolationFactor(true),
    mGlobalTimeStepFile(aGlobalTimeStepFile),
    mGlobalTypicalPeriodFile(aGlobalTypicalPeriodFile),
    mUseVariableTimeSteps(false),
    mTypicalPeriods(365),
    mNDtTypicalPeriods(24),
    mUseTypicalPeriods(false)
{
    this->setObjectName(aName);

    mStartingAbsoluteTimeStep = 0 ;

    setTimeSteps(mGlobalTimeStepFile) ;
    setTypicalPeriods(mGlobalTypicalPeriodFile) ;

    if (mPlanHorizon != aNpdtFuture) {
        qCritical () << "Fatal ERROR : number of timesteps mismatch between ";
        qCritical () << "- number of timestep values in studyName_ListeOfTimeSteps.csv file " << mPlanHorizon  ;
        qCritical () << "- number of timestep values declared by <ComputationFuturSize> field in Pegase .xml file " << aNpdtFuture;
        Q_ASSERT(mPlanHorizon == aNpdtFuture) ;
    }

} // MilpData()

//
// default constructor with few args - time data set later on at doinit step from Cairn Settings file
//
MilpData::MilpData(QObject *aParent, const QString& aName, const QString &aGlobalTimeStepFile, const QString &aGlobalTypicalPeriodFile) : QObject(aParent),
    mPdt(3600.),
    mPdtHeure(mPdt/3600.),
    mNpdtPast(1),
    mNpdt(8760),
    mNpdtTot(mNpdtPast+mNpdt),
    mTimeshift(1),
    mIHMFuturSize(mNpdt),
    mNbCycle(1),
    mRollingMode("Periodic"),
    mReadingMode("Average"),
    mRunUntilSimulationEnd(false),
    mExportResultsEveryCycle(false),
    mUseExtrapolationFactor(true),
    mGlobalTimeStepFile(aGlobalTimeStepFile),
    mGlobalTypicalPeriodFile(aGlobalTypicalPeriodFile),
    mUseVariableTimeSteps(false),
    mTypicalPeriods(365),
    mNDtTypicalPeriods(24),
    mUseTypicalPeriods(false)
{
    this->setObjectName(aName);

    mStartingAbsoluteTimeStep = 0 ;
    mTimeStepBeginLP = 0 ; // by default use LP models only ? or never ?
    mTimeStepBeginForecast = mNpdt ;
    mDecreaseOptimizationHorizon = 0. ;
    mPlanHorizon = mNpdt; 
} 

MilpData::~MilpData()
{
}  

bool MilpData::setMilpDataFromSettings(const std::map<QString, InputParam::ModelParam*>& paramMap, const bool& isStdAloneMode)
{
    bool ierr = true;
    //if (mSettings) {
    //    ierr = configureFromSettingsFile(isStdAloneMode);
    //}
    //else {//isStdAloneMode is always true in this case (API) ? 
    //    ierr = configureFromParam(paramMap, isStdAloneMode);
    //}

    for (auto const& [key, param] : paramMap) 
    {
        double value;
        if (isStdAloneMode) {
            if (key == "TimeStep")  param->getNumValue(mPdt);
            if (key == "TimeShift" && param->getNumValue(value))   mTimeshift = int(value);
            if (key == "PastSize" && param->getNumValue(value))    mNpdtPast = int(value);
            if (key == "FutureSize" && param->getNumValue(value))  mNpdt = int(value);
            if (key == "FutureVariableTimestep" && param->getNumValue(value))  mIHMFuturSize = int(value);
            if (key == "NbCycle" && param->getNumValue(value))   mNbCycle = int(value);
        }

        if (key == "RunUntilSimulationEnd" && param->getNumValue(value))    mRunUntilSimulationEnd = bool(value);
        if (key == "ExportResultsEveryCycle" && param->getNumValue(value))  mExportResultsEveryCycle = bool(value);
        if (key == "UseExtrapolationFactor" && param->getNumValue(value))   mUseExtrapolationFactor = bool(value);

        if (key == "RollingMode")    mRollingMode   = param->toString();
        if (key == "ReadingMode")    mReadingMode   = param->toString();
    }

    if (mPdt == 0 || mNpdt == 0 || mIHMFuturSize == 0)
    {
        ierr = false;
    }

    mPdtHeure = mPdt/3600. ;
    mNpdtTot = mNpdtPast+mNpdt ;

    setTimeSteps(mGlobalTimeStepFile) ;
    setTypicalPeriods(mGlobalTypicalPeriodFile) ;

    if (mPlanHorizon != mNpdt) {
        qCritical () << "Fatal ERROR : number of timesteps mismatch between ";
        qCritical () << "- number of timestep values in studyName_ListeOfTimeSteps.csv file : mPlanHorizon = " << mPlanHorizon  ;
        qCritical () << "- number of timestep values declared by <ComputationFutureVariableTimeStep> = " << mIHMFuturSize ;
        ierr = false ;
    }
    return ierr ;
}
void MilpData::setTimeSteps (QString aCsvTimeStepFileName)
{
    QList<QStringList> data_Inputs = GS::readFromCsvFile (aCsvTimeStepFileName, ";");

    if (data_Inputs.size() == 0) {
        mTimeSteps.resize(mNpdt);
        mTimeSteps.assign(mNpdt, mPdtHeure);
        mTimeStepBeginLP = 0 ; // by default use LP models only ? or never ?
        mTimeStepBeginForecast = mNpdt ;
        mDecreaseOptimizationHorizon = 0 ;
    }
    else {
        // timesteps are expected in column #0, in HOUR
        qInfo() << "Using variable timesteps from file " << aCsvTimeStepFileName ;
        mUseVariableTimeSteps = true;
        mTimeSteps = getDataArray(data_Inputs, 0, 0) ;
        mTimeStepBeginLP = getIntDataArray(data_Inputs, 1, 0).at(0) ; // first row of column #1 indicates value for mTimeStepBeginLP
        mTimeStepBeginForecast = getIntDataArray(data_Inputs, 2, 0).at(0) ; // first row of column #1 indicates value for mTimeStepBeginForecast
        mDecreaseOptimizationHorizon = getIntDataArray(data_Inputs, 1, 0).at(1) ;
    }
    mPlanHorizon = mTimeSteps.size() ;
}
void MilpData::setTypicalPeriods (QString aCsvTimeStepFileName)
{
    QList<QStringList> data_Inputs = GS::readFromCsvFile (aCsvTimeStepFileName, ";");

    if (data_Inputs.size() == 0) {
        mUseTypicalPeriods = false ;
        mTypicalPeriods = 365 ; // first row of column #1 indicates value for mTimeStepBeginLP
        mNDtTypicalPeriods = 24 ; // first row of column #1 indicates value for mTimeStepBeginForecast
        mVectTypicalPeriods.resize(mNpdt) ;
        for (uint i = 0; i < mNpdt; i++)
        {
                mVectTypicalPeriods[i] = i ;
        }
    }
    else
    {
        // timesteps are expected in column #0, in HOUR
        mUseTypicalPeriods = true ;
        mVectTypicalPeriods = getIntDataArray(data_Inputs, 0, 0) ;
        mTypicalPeriods = getIntDataArray(data_Inputs, 1, 0).at(0) ; // first row of column #1 indicates value for mTimeStepBeginLP
        mNDtTypicalPeriods = getIntDataArray(data_Inputs, 2, 0).at(0) ; // first row of column #1 indicates value for mTimeStepBeginForecast
        if (mVectTypicalPeriods.size() != mNpdt)
        {
            qCritical() << " Bad configuration in TypicalPeriodsFile : length " << mVectTypicalPeriods.size() << " while expected " << mNpdt ;
        }
        qInfo() << " We will use " << mTypicalPeriods << " Typical Periods of " << mNDtTypicalPeriods << " timesteps instead of " << mNpdt << " yielding reduction factor : " <<  float(mNpdt/(mTypicalPeriods * mNDtTypicalPeriods)) ;
    }
}
void MilpData::prepareOptim()
{
    mStartingAbsoluteTimeStep += mTimeshift ;    // update current absolute timestep due to TimeShifting
}
