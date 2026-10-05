#ifndef CAIRNCORE_H
#define CAIRNCORE_H
class CairnCore ;

#include "CairnObject.h"

#include <cmath>

#include "CairnCore_global.h"
#include "CairnLogger.h"
#include "OptimProblem.h"
#include "MilpData.h"
#include "GlobalSettings.h"
#include "StudyPathManager.h"
#include "TimeSeriesManager.h"
#include "ErrorCollector.h"
#include "Profiler.h"

struct CairnResult
{
    int status = 0;
    std::string error;
};

class CAIRNCORESHARED_EXPORT CairnCore : public CairnObject
{
public:
    CairnCore(const std::string &aName,
               const double &aPdt,
               const uint &aNpdtPast,
               const uint &aNpdtFuture,
               const uint &aTimeshift,
               const uint &aIHMFuturSize, const std::string &aGlobalTimeStepFile, const std::string &aGlobalTypicalPeriodsFile,
               const std::string &aArchFile, const std::string &aResultFile, const std::string& aScenarioName = "");

    CairnCore(const std::string &aName, const std::string& aStudyName = "", const std::string& aResultFile = "",
               const std::string & aGlobalTimeStepFile = "", const std::string & aGlobalTypicalPeriodsFile = "", const std::string& aScenarioName = "");

    ~CairnCore();

    void setStopSignal(int* stopSignal);
    bool userStoppedSimu() const;

    void doInit(bool aLoad=true);
    void importTimeSeriesIfNeeded();
    void flushErrorsIfAny();
    void buildMilpProblem(MIPModeler::MIPModel& mipModel,
        MIPModeler::MIPExpression& objective);

    void prepareProblem();
    CairnResult buildANDsolveProblem(const std::string& encoding = "UTF-8",
        const std::map<std::string, bool>& paramMap = std::map<std::string, bool>());
    CairnResult doStep(const std::string& encoding = "UTF-8",
        const std::map<std::string, bool>& paramMap = std::map<std::string, bool>());
    int doTerminate();

    void initExternalModeler(MIPModeler::MIPModel& mipModel);
    void handleNoSolution(const std::string& status, bool isRollingHorizon, int& istat);
    void processSolutions(int nbSol, const std::string& status, bool isRollingHorizon, int& istat);

    void setStdAloneMode(const bool& abool);

    void addTS(const std::wstring& a_fileName);
    void addTS(const t_dict& a_TS);
    bool checkTS(string& a_ErrMsg);
    void importTS(const int& iShift);
    void importTS(const std::vector<std::string>& aTSfileList, const int& iShift);
    void importTS(const std::vector<std::wstring>&aTSfileList, const int& iShift);

    int exportTS(const std::string &aTSfile, int iter = 0, bool rh = false, const std::string& encoding = "UTF-8");
    int exportTS(const std::string& aTSfile, std::map<std::string, std::vector<double>>& resultats, const std::string& encoding = "UTF-8");
  
    const t_mapExchange &ListSubscribedVariables() { return mProblem->ListSubscribedVariables() ;}
    t_mapExchange &ListPublishedVariables() { return mProblem->ListPublishedVariables() ;}

    //Other API functions
    std::string StudyName() { return mStudy.StudyName(); }
    MilpData* getTimeData() { return mMilpData; }

    double timeStep() const {return mMilpData->TimeStep(0);}
    uint npdtFutur() const {return mMilpData->npdt();}
    uint npdtPast() const {return mMilpData->npdtPast();}
    uint npdtTot() const {return mMilpData->npdtTot();}
    uint nbcycle() const {return mMilpData->nbcycle();}
    std::string rollingMode() const { return mMilpData->rollingMode(); }
    std::string readingMode() const { return mMilpData->readingMode(); }
    bool runUntilEnd() const { return mMilpData->runUntilEnd(); }
    bool ExportResultsEveryCycle() const { return mMilpData->ExportResultsEveryCycle(); }
    bool ShowIndicatorDescription() const { return mMilpData->ShowIndicatorDescription(); }
    bool UseExtrapolationFactor() const { return mMilpData->UseExtrapolationFactor(); }

    uint npdtTimeshift() const {return mMilpData->timeshift();}
    uint npdtLongTerm() const {return mMilpData->iHMFuturSize();}

    // get results and input files
    std::string resultFile() const { return mStudy.resultFile(); }   
    std::string archFile()   const { return mStudy.archFile();   }
    std::string projectDir() const { return mStudy.projectDir(); }
    std::string resultsDir() const { return mStudy.resultsDir(); }

    std::string studyVersion() const { return mProblem ? mProblem->studyVersion() : ""; }

    OptimProblem* getProblem() {return mProblem ;}
    MilpComponent* getComponent(const std::string & aName) {return mProblem->findChild<MilpComponent>(aName); }
       
    int  exportResults( int aNsol, bool isRollingHorizon, int istat, const std::string& encoding = "UTF-8");
    void exportAnalysis(int aNsol, bool isRollingHorizon, const std::string& encoding = "UTF-8");

    void setStudyName(const std::string& aStudyName, const std::string& aResultFile="");
    void setResultFile(const std::string& aResultFile);
    void setResultsDir(const std::string& aResultsDir);
    void setScenarioName(const std::string& aScenarioName);
    void setTimeStepFile(const std::string& aTimeStepFile);
    void setTypicalPeriodsFile(const std::string& aTypicalPeriodsFile);

    class OrCheckUnits CheckUnits(const std::string& a_FileUnit, const std::string& a_Units, bool a_Check = true);

    /* 
        Get Possible parameter values for GUI
    */
    std::vector<std::string> getPossibleImpactNames() const {
        if (mProblem != nullptr && mProblem->getTecEcoAnalysis() != nullptr) 
            return mProblem->getTecEcoAnalysis()->getPossibleImpactNames();
        return {};
    }

    std::vector<std::string> getPossibleImpactShortNames() const  {
        if (mProblem != nullptr && mProblem->getTecEcoAnalysis() != nullptr)
            return mProblem->getTecEcoAnalysis()->getPossibleImpactShortNames();
        return {};
    }
    //----------------------------------------------------
   
    int getNumberOfSolutions() { return mProblem->getNumberOfSolutions();  }
    std::string getOptimLogFile() { return mOptimLogFile; }
    std::string getResultsTimeSeriesFileName(const int& aNsol) { return mStudy.getScenarioFile("_Results.csv", aNsol); }
    std::string getGlobalResultsFileName(const int& aNsol) { return mStudy.getScenarioFile("_PLAN.csv", aNsol); }

    int getNumCycle() { return mIter; }
    void incrementCycleNum() { mIter++; }
    void loadDefUnits() const;

    int saveStudy(const std::string& filename = {}, const std::string& posAlgorithm = {});

    // ------------------------------------------------------------------------ //

    /** Returns all collected warnings and errors since last flush and clears the list */
    std::vector<CairnLogger::ErrorEntry> flushWarningANDErrors()
    {
        return CairnLogger::ErrorCollector::flush();
    }

    /** Returns true if any errors occurred since last flush (warnings are not counted) */
    bool hasErrors() const
    {
        return CairnLogger::ErrorCollector::hasErrors();
    }

    /** Clears warnings and errors without returning them */
    void clearWarningANDErrors()
    {
        CairnLogger::ErrorCollector::clear();
    }

#if ENABLE_PROFILING
    CairnProfiling::ScopedProfiler makeIterationProfiler() const;
#endif

private:
    OptimProblem* mProblem ;
    MilpData *mMilpData ;
    TimeSeriesManager* mTimeSeriesManager;

    bool mStdAloneMode;
    int* mStopSignal;
    int mIter;

    std::string mOptimLogFile; 
    StudyPathManager mStudy;
};

#endif // CAIRNCORE_H
