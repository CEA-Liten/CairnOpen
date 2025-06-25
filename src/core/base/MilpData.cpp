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
    mTypicalPeriods(365),
    mNDtTypicalPeriods(24),
    mUseTypicalPeriods(false),
    mModeObjective("Mono"),
    mSettings(nullptr)
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
    mTypicalPeriods(365),
    mNDtTypicalPeriods(24),
    mUseTypicalPeriods(false),
    mModeObjective("Mono"),
    mSettings(nullptr)
{
    this->setObjectName(aName);

    mStartingAbsoluteTimeStep = 0 ;
    mTimeStepBeginLP = 0 ; // by default use LP models only ? or never ?
    mTimeStepBeginForecast = mNpdt ;
    mDecreaseOptimizationHorizon = 0. ;
    mPlanHorizon = mNpdt; 
} // MilpData()

void MilpData::setSettings(const QSettings* aSettings)
{
    mSettings = aSettings; 
} 
MilpData::~MilpData()
{
} // ~MilpData()
bool MilpData::configureFromSettingsFile(const bool isStdAloneMode)
{
    QString simulationControlName = getVariantSetting("SimulationControlName", Settings()).toString();

    if(isStdAloneMode){
        if (getVariantSetting(simulationControlName + ".TimeStep", Settings()) != QVariant())      
            mPdt = getVariantSetting(simulationControlName + ".TimeStep", Settings()).toDouble();
        if (getVariantSetting(simulationControlName + ".PastSize", Settings()) != QVariant())     
            mNpdtPast = getVariantSetting(simulationControlName + ".PastSize", Settings()).toInt();
        if (getVariantSetting(simulationControlName + ".FutureSize", Settings()) != QVariant())    
            mNpdt = getVariantSetting(simulationControlName + ".FutureSize", Settings()).toInt();
        if (getVariantSetting(simulationControlName + ".TimeShift", Settings()) != QVariant())             
            mTimeshift = getVariantSetting(simulationControlName + ".TimeShift", Settings()).toInt();
        if (getVariantSetting(simulationControlName + ".FutureVariableTimestep", Settings()) != QVariant()) 
            mIHMFuturSize = getVariantSetting(simulationControlName + ".FutureVariableTimestep", Settings()).toInt();
    
        //Value is not used in case of non-StdAloneMode
        if (getVariantSetting(simulationControlName + ".NbCycle", Settings()) != QVariant())  
            mNbCycle = getVariantSetting(simulationControlName + ".NbCycle", Settings()).toInt();
    }

    if (getVariantSetting(simulationControlName + ".RollingMode", Settings()) != QVariant())            
        mRollingMode = getVariantSetting(simulationControlName + ".RollingMode", Settings()).toString();
    if (getVariantSetting(simulationControlName + ".ReadingMode", Settings()) != QVariant())            
        mReadingMode = getVariantSetting(simulationControlName + ".ReadingMode", Settings()).toString();
    if (getVariantSetting(simulationControlName + ".ModeObjective", Settings()) != QVariant())           
        mModeObjective = getVariantSetting(simulationControlName + ".ModeObjective", Settings()).toString();
    if (getVariantSetting(simulationControlName + ".RunUntilSimulationEnd", Settings()) != QVariant())  
        mRunUntilSimulationEnd = getVariantSetting(simulationControlName + ".RunUntilSimulationEnd", Settings()).toBool();
    if (getVariantSetting(simulationControlName + ".ExportResultsEveryCycle", Settings()) != QVariant()) 
        mExportResultsEveryCycle = getVariantSetting(simulationControlName + ".ExportResultsEveryCycle", Settings()).toBool();
    if (getVariantSetting(simulationControlName + ".UseExtrapolationFactor", Settings()) != QVariant())  
        mUseExtrapolationFactor = getVariantSetting(simulationControlName + ".UseExtrapolationFactor", Settings()).toBool();

    if (mPdt == 0 || mNpdt ==0  || mIHMFuturSize == 0)
    {
        return false ;
    }
    return true ;
}

bool MilpData::configureFromParam(QMap<QString, QString> paramMap, const bool isStdAloneMode)
{
    if (isStdAloneMode) {
        if (paramMap.contains("TimeStep"))    mPdt = (paramMap["TimeStep"]).toDouble();
        if (paramMap.contains("PastSize"))    mNpdtPast = (paramMap["PastSize"]).toInt();
        if (paramMap.contains("FutureSize"))  mNpdt = (paramMap["FutureSize"]).toInt();
        if (paramMap.contains("TimeShift"))   mTimeshift = (paramMap["TimeShift"]).toInt();
        if (paramMap.contains("FutureVariableTimestep"))  mIHMFuturSize = (paramMap["FutureVariableTimestep"]).toInt();

        //Value is not used in case of non-StdAloneMode
        if (paramMap.contains("NbCycle"))     mNbCycle = (paramMap["NbCycle"]).toInt();
    }

    if (paramMap.contains("RollingMode")) mRollingMode = paramMap["RollingMode"];
    if (paramMap.contains("ReadingMode")) mReadingMode = paramMap["ReadingMode"];
    if (paramMap.contains("RunUntilSimulationEnd"))   mRunUntilSimulationEnd = (bool)(paramMap["RunUntilSimulationEnd"]).toInt();
    if (paramMap.contains("ExportResultsEveryCycle")) mExportResultsEveryCycle = (bool)(paramMap["ExportResultsEveryCycle"]).toInt();
    if (paramMap.contains("UseExtrapolationFactor"))  mUseExtrapolationFactor = (bool)(paramMap["UseExtrapolationFactor"]).toInt();
    if (paramMap.contains("ModeObjective")) mModeObjective = paramMap["ModeObjective"];

    if (mPdt == 0 || mNpdt == 0 || mIHMFuturSize == 0)
    {
        return false;
    }

    //update mSettings from paramMap
    updateSettings(paramMap);

    return true;
}

void MilpData::updateSettings(QMap<QString, QString>& paramMap)
{
    QSettings* settings = new QSettings();

    //initialize from mSettings
    if (mSettings) {
        settings = const_cast<QSettings*>(mSettings);
    }

    //update from paramMap
    QMapIterator <QString, QString> iParam(paramMap);
    while (iParam.hasNext())
    {
        iParam.next();
        settings->setValue(iParam.key(), iParam.value());
    }

    //set mSettings
    setSettings(settings);
}


bool MilpData::setMilpDataFromSettings(const bool isStdAloneMode, const QMap<QString, QString> paramMap)
{
    bool ierr = true;
    if (mSettings) {
        ierr = configureFromSettingsFile(isStdAloneMode);
    }
    else {//isStdAloneMode is always true in this case (API) ? 
        ierr = configureFromParam(paramMap, isStdAloneMode);
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
