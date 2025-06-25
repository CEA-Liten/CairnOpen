#ifndef CAIRNCORE_H
#define CAIRNCORE_H
class CairnCore ;

#include <QObject>
#include <QtCore>
#include <QVector>
#include <QStringList>

#include <cmath>

//#include "MILPComponent_global.h"
#include "CairnCore_global.h"
#include "OptimProblem.h"
#include "MilpData.h"
#include "GlobalSettings.h"
#include "StudyPathManager.h"
#include "TimeSeriesManager.h"

class CAIRNCORESHARED_EXPORT CairnCore : public QObject
{
Q_OBJECT

public:
    CairnCore(QObject *aParent, const QString &aName,
               const double &aPdt,
               const uint &aNpdtPast,
               const uint &aNpdtFuture,
               const uint &aTimeshift,
               const uint &aIHMFuturSize, const QString &aGlobalTimeStepFile, const QString &aGlobalTypicalPeriodsFile,
               const QString &aArchFile, const QString &aResultFile, const QString& aScenarioName = "");

    CairnCore(QObject *aParent, const QString &aName, const QString& aStudyName = "", const QString& aResultFile = "",
               const QString & aGlobalTimeStepFile = "", const QString & aGlobalTypicalPeriodsFile = "", const QString& aScenarioName = "");

    ~CairnCore();

    void setStopSignal(int* stopSignal);
    void doInit(bool aLoad=true);
    int doStep(const QString encoding="UTF-8", const QMap<QString, bool> paramMap=QMap<QString, bool>());
    int doTerminate();

    void importTS(const QStringList &aTSfileList, const int &iShift) ;

    int exportTS(const QString &aTSfile, int iter = 0, bool rh = false, QString encoding="UTF-8");
    int exportTS(const QString& aTSfile, std::map<std::string, std::vector<double>>& resultats, QString encoding = "UTF-8");
  
    const QMap<QString, ZEVariables*> ListSubscribedVariables() { return mProblem->ListSubscribedVariables() ;}
    const QMap<QString, ZEVariables*> ListPublishedVariables() { return mProblem->ListPublishedVariables() ;}

    //Other API functions
    QString StudyName() { return QString(mStudy.StudyName().c_str()); }
    MilpData* getTimeData() { return mMilpData; }

    double timeStep() const {return mMilpData->TimeStep(0);}
    uint npdtFutur() const {return mMilpData->npdt();}
    uint npdtPast() const {return mMilpData->npdtPast();}
    uint npdtTot() const {return mMilpData->npdtTot();}
    uint nbcycle() const {return mMilpData->nbcycle();}
    QString rollingMode() const { return mMilpData->rollingMode(); }
    QString readingMode() const { return mMilpData->readingMode(); }
    bool runUntilEnd() const { return mMilpData->runUntilEnd(); }
    bool ExportResultsEveryCycle() const { return mMilpData->ExportResultsEveryCycle(); }
    bool UseExtrapolationFactor() const { return mMilpData->UseExtrapolationFactor(); }

    uint npdtTimeshift() const {return mMilpData->timeshift();}
    uint npdtLongTerm() const {return mMilpData->iHMFuturSize();}

    // get results and input files
    QString resultFile()   const { return QString(mStudy.resultFile().c_str()); }   
    QString archFile()     const { return QString(mStudy.archFile().c_str()); }
    QString projectDir()     const { return QString(mStudy.projectDir().c_str());}
    QString resultsDir()     const { return QString(mStudy.resultsDir().c_str());}

  
    OptimProblem* getProblem() {return mProblem ;}

    // get component list of problem
    QObjectList objectList() {return mProblem->children();}

    QList<MilpComponent*> getComponentList() {return mProblem->findChildren<MilpComponent*>();}
    MilpComponent* getComponent(const QString & aName) {return mProblem->findChild<MilpComponent*>(aName);}

    //QList<BusCompo*> getBusComponentList() {return mProblem->findChildren<BusCompo*>();}
    //BusCompo* getBusComponent(const QString & aName) {return mProblem->findChild<BusCompo*>(aName);}

    QList<EnergyVector*> getEnergyVectorList() {return mProblem->findChildren<EnergyVector*>();}
    EnergyVector* getEnergyVector(const QString & aName) {return mProblem->findChild<EnergyVector*>(aName);}

    QList<MilpPort*> getMilpPortList(MilpComponent* aMilpComponent) {return aMilpComponent->findChildren<MilpPort*>();}
    MilpPort* getMilpPort(MilpComponent* aMilpComponent, const QString & aName) {return aMilpComponent->findChild<MilpPort*>(aName);}

    // set parameter &component param=value
    // void setValue(MilpComponent* aMilpComponent, const QString & aParam, const QVariant & aValue);

   
    void exportTotalTimeResolutionAllCycles(const QString& aFileName);
    int exportResults(const int& aNsol, const bool& isRollingHorizon, const int& istat, QString encoding = "UTF-8");
    void exportAnalysis(const int& aNsol, const bool& isRollingHorizon, QString encoding = "UTF-8");

    void setStudyName(const QString& aStudyName, const QString& aResultFile="");
    void setResultFile(const QString& aResultFile);
    void setResultsDir(const QString& aResultsDir);
    void setScenarioName(const QString& aScenarioName);
    void setTimeStepFile(const QString& aTimeStepFile);
    void setTypicalPeriodsFile(const QString& aTypicalPeriodsFile);

    class OrCheckUnits CheckUnits(const QString& a_FileUnit, const QString& a_Units, bool a_Check = true);

    /* 
        Get Possible parameter values for GUI
    */
    QStringList getPossibleImpactNames() const {
        if (mProblem != nullptr && mProblem->getTecEcoAnalysis() != nullptr) 
            return mProblem->getTecEcoAnalysis()->getPossibleImpactNames();
        return {};
    }

    QStringList getPossibleImpactShortNames() const  {
        if (mProblem != nullptr && mProblem->getTecEcoAnalysis() != nullptr)
            return mProblem->getTecEcoAnalysis()->getPossibleImpactShortNames();
        return {};
    }
    //----------------------------------------------------
   
    int getNumberOfSolutions() { return mProblem->getNumberOfSolutions();  }
    QString getOptimLogFile() { return mOptimLogFile; }
    QString getResultsTimeSeriesFileName(const int& aNsol) { return QString(mStudy.getScenarioFile("_Results.csv", aNsol).c_str()); }
    QString getGlobalResultsFileName(const int& aNsol) { return QString(mStudy.getScenarioFile("_PLAN.csv", aNsol).c_str()); }

private:
    OptimProblem* mProblem ;
    MilpData *mMilpData ;
    TimeSeriesManager* mTimeSeriesManager;

    bool mStdAloneMode;
    int* mStopSignal;
    int mIter;

    QString mOptimLogFile; 
    StudyPathManager mStudy;
    
    std::vector<double> mSolverRunningTimeAllCycles;

    Q_LOGGING_CATEGORY(log, "CairnCore", QtDebugMsg)
};

#endif // CAIRNCORE_H
