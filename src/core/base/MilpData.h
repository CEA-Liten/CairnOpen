#ifndef MILPDATA_H
#define MILPDATA_H
class MilpData ;

#include "InputParam.h"

#include "CairnCore_global.h"


class CAIRNCORESHARED_EXPORT MilpData : public CairnObject
{
    
public:

    MilpData(CairnObject* aParent, const std::string& aName, const double& aPdt, const uint& aNpdtPast, const uint& aNpdtFuture, const uint& aTimeshift, const uint& aIHMFuturSize, const std::string& aGlobalTimeStepFile, const std::string& aGlobalTypicalPeriodFile);
    MilpData(CairnObject* aParent, const std::string& aName, const std::string& aGlobalTimeStepFile, const std::string& aGlobalTypicalPeriodFile);

    virtual ~MilpData();

    bool setMilpDataFromSettings(const std::map<std::string, ModelParam*>& paramMap = {}, const bool& isStdAloneMode = true);

    virtual void prepareOptim() ;

    void setTimeSteps (std::string aCsvTimeStepFileName);
    double TimeStep(uint i) const {return mTimeSteps[i];}    //TimeStep in HOUR
    std::vector<double> TimeSteps() const {return mTimeSteps;}    //TimeStep list in HOUR

    uint64_t TimeStepBeginLP() const {return mTimeStepBeginLP ;}
    uint64_t TimeStepBeginForecast() const {return mTimeStepBeginForecast ;}
    uint64_t DecreaseOptimizationHorizon() const {return mDecreaseOptimizationHorizon ;}
    bool useVariableTimeSteps() const { return mUseVariableTimeSteps; }

    void setTypicalPeriods (std::string aCsvTimeStepFileName);
    std::vector<int> VectTypicalPeriods() const {return mVectTypicalPeriods;}    //TimeStep list in HOUR
    uint TypicalPeriods() const {return mTypicalPeriods ;}
    uint NDtTypicalPeriods() const {return mNDtTypicalPeriods ;}
    bool useTypicalPeriods() const {return mUseTypicalPeriods ;}

    int startTime() const { return mStartTime; }
    int endTime() const { return mEndTime; }

    double pdt() const {return mPdt ;}         /** Pas de temps en secondes */
    double pdtHeure() const {return mPdtHeure ;}   /** La meme grandeur mais en heures */
    uint npdtPast() const {return mNpdtPast ;}     /** Nombre de pas de temps passe */
    uint npdt() const {return mNpdt ;}         /** Nombre de pas de temps (futur) */
    uint npdtTot() const {return mNpdtTot ;}       /** Nombre total de pas de temps (passe + futur) */
    uint timeshift() const {return mTimeshift ;}    /** Time shifting */
    uint iHMFuturSize() const {return mIHMFuturSize ;}
    uint nbcycle() const {return mNbCycle ;}       /** Nombre cycle rolling horizon */
    std::string rollingMode() const { return mRollingMode; }
    std::string readingMode() const { return mReadingMode; }
    bool runUntilEnd() const { return mRunUntilSimulationEnd; }
    bool ExportResultsEveryCycle() const { return mExportResultsEveryCycle; }
    bool ShowIndicatorDescription() const { return mShowIndicatorDescription; }
    bool UseExtrapolationFactor() const { return mUseExtrapolationFactor; }

    uint startingAbsoluteTimeStep () const {return mStartingAbsoluteTimeStep ;} /** Starting absolute timestep number for the current optimization */
    const uint* getAbsoluteTimeStep () const {return &mStartingAbsoluteTimeStep ;} /** Starting absolute timestep number for the current optimization */
    const uint* getTimeshift () const {return &mTimeshift ;} /** Starting absolute timestep number for the current optimization */
    const uint* getIHMFuturSize () const {return &mIHMFuturSize ;} /** Starting absolute timestep number for the current optimization */

    void setStartingAbsoluteTimeStep (const uint val) {mStartingAbsoluteTimeStep = val;}

    const std::string getVariableTimeStepsFile() const { return  mGlobalTimeStepFile; }
    const std::string getTypicalPeriodsFile() const { return  mGlobalTypicalPeriodFile; }

    void setVariableTimeStepsFile(const std::string& a_FileName) { mGlobalTimeStepFile = a_FileName; }
    void setTypicalPeriodsFile(const std::string& a_FileName) { mGlobalTypicalPeriodFile = a_FileName; }
protected:
    int mStartTime{ 0 }; /** Temps initial en pdt (Indice de l'initial pas de temps) */
    int mEndTime{ -1 };  /** Dernière Temps en pdt */
    double mPdt;         /** Pas de temps en secondes */
    double mPdtHeure;    /** La meme grandeur mais en heures */
    uint mNpdtPast;      /** Nombre de pas de temps passe */
    uint mNpdt;          /** Nombre de pas de temps (futur) */
    uint mNpdtTot;       /** Nombre total de pas de temps (passe + futur) */
    uint mTimeshift;     /** Time shifting */
    uint mIHMFuturSize ; /** == mFutureVariableTimestep */
    uint mNbCycle ; /** number of cycling rolling horizon */
    std::string mRollingMode; /** mode used to read time series data when end of the file reached*/
    std::string mReadingMode; /** mode used to read time series data when TimeStep is large (e.g. take average of all values in between)*/
    bool mRunUntilSimulationEnd; //if true run until the end of all cycles; if false stop at cycle where there is a problem (no solution)
    bool mExportResultsEveryCycle; //if true generate PLAN/HIST every cycle
    bool mShowIndicatorDescription; //if true add a column for indicators Description in PLAN/HIST
    bool mUseExtrapolationFactor;
    std::string mGlobalTimeStepFile ; /** Timestep file name */
    std::string mGlobalTypicalPeriodFile ; /** Typical Period file name */

    uint mStartingAbsoluteTimeStep ; /** Starting absolute timestep number for the current optimization */

    //variable timesteps
    std::vector<double> mTimeSteps ; //Vector of Timesteps over planning horizon. Past timesteps use small timestep
    uint64_t mTimeStepBeginLP ;        //First timestep of the LP model- For smaller Timesteps, MILP model may be used, for larger Timesteps, use LP model only
    uint64_t mTimeStepBeginForecast ;        //First timestep of the LP model- For smaller Timesteps, MILP model may be used, for larger Timesteps, use LP model only
    uint64_t mPlanHorizon ;          // for convenience, number of timesteps = length of mTimeSteps, due to variable time step
    uint64_t mDecreaseOptimizationHorizon ;
    bool mUseVariableTimeSteps;

    //typical periods
    std::vector<int> mVectTypicalPeriods ; //Vector of Timesteps over planning horizon. Past timesteps use small timestep
    uint mTypicalPeriods ; // number of typical period eg number of typical days, weeks...
    uint mNDtTypicalPeriods ;  // number of timesteps in one typical period : 24 hours...
    bool mUseTypicalPeriods ;
};

#endif // MILPDATA_H
