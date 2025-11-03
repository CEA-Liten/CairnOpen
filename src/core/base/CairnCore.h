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
    void doInit(bool aLoad=true);
    int doStep(const std::string& encoding = "UTF-8", const std::map<std::string, bool> paramMap=std::map<std::string, bool>());
    int doTerminate();

    void importTS(const t_list &aTSfileList, const int &iShift) ;

    int exportTS(const std::string &aTSfile, int iter = 0, bool rh = false, const std::string& encoding = "UTF-8");
    int exportTS(const std::string& aTSfile, std::map<std::string, std::vector<double>>& resultats, const std::string& encoding = "UTF-8");
  
    const t_mapExchange &ListSubscribedVariables() { return mProblem->ListSubscribedVariables() ;}
    t_mapExchange &ListPublishedVariables() { return mProblem->ListPublishedVariables() ;}

    //Other API functions
    std::string StudyName() { return std::string(mStudy.StudyName().c_str()); }
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
    std::string resultFile()   const { return std::string(mStudy.resultFile().c_str()); }   
    std::string archFile()     const { return std::string(mStudy.archFile().c_str()); }
    std::string projectDir()     const { return std::string(mStudy.projectDir().c_str());}
    std::string resultsDir()     const { return std::string(mStudy.resultsDir().c_str());}

  
    OptimProblem* getProblem() {return mProblem ;}
    MilpComponent* getComponent(const std::string & aName) {return mProblem->findChild<MilpComponent>(aName); }
       
    void exportTotalTimeResolutionAllCycles(const std::string& aFileName);
    int exportResults(const int& aNsol, const bool& isRollingHorizon, const int& istat, const std::string& encoding = "UTF-8");
    void exportAnalysis(const int& aNsol, const bool& isRollingHorizon, const std::string& encoding = "UTF-8");

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

private:
    OptimProblem* mProblem ;
    MilpData *mMilpData ;
    TimeSeriesManager* mTimeSeriesManager;

    bool mStdAloneMode;
    int* mStopSignal;
    int mIter;

    std::string mOptimLogFile; 
    StudyPathManager mStudy;
    
    std::vector<double> mSolverRunningTimeAllCycles;
};

#endif // CAIRNCORE_H
