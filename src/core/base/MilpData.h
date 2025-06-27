#ifndef MILPDATA_H
#define MILPDATA_H
class MilpData ;

#include "CairnCore_global.h"
#include <QObject>

class CAIRNCORESHARED_EXPORT MilpData : public QObject
{
    Q_OBJECT
public:

    MilpData(QObject* aParent, const QString& aName, const double& aPdt, const uint& aNpdtPast, const uint& aNpdtFuture, const uint& aTimeshift, const uint& aIHMFuturSize, const QString& aGlobalTimeStepFile, const QString& aGlobalTypicalPeriodFile);
    MilpData(QObject* aParent, const QString& aName, const QString& aGlobalTimeStepFile, const QString& aGlobalTypicalPeriodFile);

    virtual ~MilpData();

    void setSettings(const QSettings* aSettings);
    void updateSettings(QMap<QString, QString>& paramMap);

    bool setMilpDataFromSettings(const bool isStdAloneMode=true, const QMap<QString, QString> paramMap = {});
    bool configureFromSettingsFile(const bool isStdAloneMode = true);
    bool configureFromParam(QMap<QString, QString> paramMap, const bool isStdAloneMode = true);

    const QSettings& Settings() const { return (*mSettings); }


    virtual void prepareOptim() ;

    void setTimeSteps (QString aCsvTimeStepFileName);
    double TimeStep(uint i) const {return mTimeSteps[i];}    //TimeStep in HOUR
    std::vector<double> TimeSteps() const {return mTimeSteps;}    //TimeStep list in HOUR

    uint64_t TimeStepBeginLP() const {return mTimeStepBeginLP ;}
    uint64_t TimeStepBeginForecast() const {return mTimeStepBeginForecast ;}
    uint64_t DecreaseOptimizationHorizon() const {return mDecreaseOptimizationHorizon ;}

    void setTypicalPeriods (QString aCsvTimeStepFileName);
    std::vector<int> VectTypicalPeriods() const {return mVectTypicalPeriods;}    //TimeStep list in HOUR
    uint TypicalPeriods() const {return mTypicalPeriods ;}
    uint NDtTypicalPeriods() const {return mNDtTypicalPeriods ;}
    bool useTypicalPeriods() const {return mUseTypicalPeriods ;}

    double pdt() const {return mPdt ;}         /** Pas de temps en secondes */
    double pdtHeure() const {return mPdtHeure ;}   /** La meme grandeur mais en heures */
    uint npdtPast() const {return mNpdtPast ;}     /** Nombre de pas de temps passe */
    uint npdt() const {return mNpdt ;}         /** Nombre de pas de temps (futur) */
    uint npdtTot() const {return mNpdtTot ;}       /** Nombre total de pas de temps (passe + futur) */
    uint timeshift() const {return mTimeshift ;}    /** Time shifting */
    uint iHMFuturSize() const {return mIHMFuturSize ;}
    uint nbcycle() const {return mNbCycle ;}       /** Nombre cycle rolling horizon */
    QString rollingMode() const { return mRollingMode; }
    QString readingMode() const { return mReadingMode; }
    bool runUntilEnd() const { return mRunUntilSimulationEnd; }
    bool ExportResultsEveryCycle() const { return mExportResultsEveryCycle; }
    bool UseExtrapolationFactor() const { return mUseExtrapolationFactor; }

    QString ModeObjective () const {return mModeObjective;}

    uint startingAbsoluteTimeStep () const {return mStartingAbsoluteTimeStep ;} /** Starting absolute timestep number for the current optimization */
    const uint* getAbsoluteTimeStep () const {return &mStartingAbsoluteTimeStep ;} /** Starting absolute timestep number for the current optimization */
    const uint* getTimeshift () const {return &mTimeshift ;} /** Starting absolute timestep number for the current optimization */
    const uint* getIHMFuturSize () const {return &mIHMFuturSize ;} /** Starting absolute timestep number for the current optimization */

    const QString* getModeObjective () const {return  &mModeObjective ;}

    void setStartingAbsoluteTimeStep (const uint val) {mStartingAbsoluteTimeStep = val;}

    const QString getVariableTimeStepsFile() const { return  mGlobalTimeStepFile; }
    const QString getTypicalPeriodsFile() const { return  mGlobalTypicalPeriodFile; }

    void setVariableTimeStepsFile(const QString& a_FileName) { mGlobalTimeStepFile = a_FileName; }
    void setTypicalPeriodsFile(const QString& a_FileName) { mGlobalTypicalPeriodFile = a_FileName; }
protected:

    const QSettings* mSettings ;

    double mPdt;         /** Pas de temps en secondes */
    double mPdtHeure;    /** La meme grandeur mais en heures */
    uint mNpdtPast;     /** Nombre de pas de temps passe */
    uint mNpdt;         /** Nombre de pas de temps (futur) */
    uint mNpdtTot;       /** Nombre total de pas de temps (passe + futur) */
    uint mTimeshift;    /** Time shifting */
    uint mIHMFuturSize ; /** == mFutureVariableTimestep */
    uint mNbCycle ; /** number of cycling rolling horizon */
    QString mRollingMode; /** mode used to read time series data when end of the file reached*/
    QString mReadingMode; /** mode used to read time series data when TimeStep is large (e.g. take average of all values in between)*/
    bool mRunUntilSimulationEnd; //if true run until the end of all cycles; if false stop at cycle where there is a problem (no solution)
    bool mExportResultsEveryCycle; //if true generate PLAN/HIST every cycle
    bool mUseExtrapolationFactor;
    QString mModeObjective;
    QString mGlobalTimeStepFile ; /** Timestep file name */
    QString mGlobalTypicalPeriodFile ; /** Typical Period file name */

    uint mStartingAbsoluteTimeStep ; /** Starting absolute timestep number for the current optimization */

    //variable timesteps
    std::vector<double> mTimeSteps ; //Vector of Timesteps over planning horizon. Past timesteps use small timestep
    uint64_t mTimeStepBeginLP ;        //First timestep of the LP model- For smaller Timesteps, MILP model may be used, for larger Timesteps, use LP model only
    uint64_t mTimeStepBeginForecast ;        //First timestep of the LP model- For smaller Timesteps, MILP model may be used, for larger Timesteps, use LP model only
    uint64_t mPlanHorizon ;          // for convenience, number of timesteps = length of mTimeSteps, due to variable time step
    uint64_t mDecreaseOptimizationHorizon ;

    //typical periods
    std::vector<int> mVectTypicalPeriods ; //Vector of Timesteps over planning horizon. Past timesteps use small timestep
    uint mTypicalPeriods ; // number of typical period eg number of typical days, weeks...
    uint mNDtTypicalPeriods ;  // number of timesteps in one typical period : 24 hours...
    bool mUseTypicalPeriods ;
};

#endif // MILPDATA_H
