#include <iostream>
#include <string>
#include <math.h>       /* fabs, log, pow */
#include <fstream>
#include <vector>
#include <assert.h>
#include <fstream>
#include <filesystem>
namespace fs = std::filesystem;


#include "MIPModeler.h"

#include "CairnCore.h"
#include "GlobalSettings.h"
#include "OptimProblem.h"
#include "MilpData.h"
#include "ZEVariables.h"

#include "OrUnitsConverter.h"
#include "CairnUtils.h"
using namespace CairnUtils;

//using namespace MIPModeler;
using namespace GS;

// Non StdAloneMode : linked to Pegase, get Time Data from argument - No use of Cairn SimulationControl object
CairnCore::CairnCore(
                       const std::string & aName,
                       const double & aPdt,
                       const uint & aNpdtPast,
                       const uint & aNpdtFuture,
                       const uint & aTimeshift,
                       const uint & aIHMFuturSize,
                       const std::string & aGlobalTimeStepFile,
                       const std::string & aGlobalTypicalPeriodsFile,
                       const std::string & aStudyFile,
                       const std::string & aResultFile, 
                       const std::string& aScenarioName)
    : CairnObject(nullptr, aName),
    mStdAloneMode(false),       
    mStopSignal(NULL),
    mIter(-1),
    mOptimLogFile("")
{    
    GS::IDCount=0 ;
    GS::iVerbose=0 ;

      
    mMilpData = new MilpData (this, "MilpData", aPdt, aNpdtPast, aNpdtFuture, aTimeshift, aIHMFuturSize, aGlobalTimeStepFile, aGlobalTypicalPeriodsFile) ;
    setStudyName(aStudyFile, aResultFile);
    setScenarioName(aScenarioName);

    // Create master optim problem with MilpData, void TecEcoEnv and list of objects.
    TecEcoEnv aTecEcoEnv ;
    std::map<std::string,std::string> aMilpComponents ;
    aMilpComponents["id"]=std::string("SYSTEM") ;

    mProblem = new OptimProblem(this, "OptimProblem", mMilpData, aTecEcoEnv, mStdAloneMode, aMilpComponents) ;

    mTimeSeriesManager = new TimeSeriesManager(*mMilpData, "pegase");
}

// StdAloneMode : Time Data set from Cairn SimulationControl object from Json file.
CairnCore::CairnCore(
                       const std::string & aName,
                       const std::string& aStudyName,
                       const std::string& aResultFile,
                       const std::string &aGlobalTimeStepFile,
                       const std::string &aGlobalTypicalPeriodsFile,
                       const std::string& aScenarioName)
    : CairnObject(nullptr, aName),
    mStdAloneMode(true),
    mStopSignal(NULL),
    mIter(-1),
    mOptimLogFile("")
{
    GS::IDCount=0 ;
    GS::iVerbose=0 ;
    std::string exeDir = std::getenv("CAIRN_BIN") + (std::string)"/../resources/DefUnits.json";
    
    UnitsConverter::Load(exeDir);

    mMilpData = new MilpData(this, "MilpData", aGlobalTimeStepFile, aGlobalTypicalPeriodsFile);

    setStudyName(aStudyName, aResultFile);
    setScenarioName(aScenarioName);

    TecEcoEnv aTecEcoEnv ;
    std::map<std::string,std::string> aMilpComponents ;
    aMilpComponents["id"]=std::string("SYSTEM") ;

    mProblem = new OptimProblem(this, "OptimProblem", mMilpData, aTecEcoEnv, mStdAloneMode, aMilpComponents) ;

    mTimeSeriesManager = new TimeSeriesManager(*mMilpData);
}

CairnCore::~CairnCore()
{
    if (mMilpData) delete mMilpData;
    if (mProblem) delete mProblem;
    if (mTimeSeriesManager) delete mTimeSeriesManager;
}

void CairnCore::setStudyName(const std::string& aStudyName, const std::string& aResultFile)
{
    // StudyName = <ProjectDir> / <StudyName> .json
    if (aStudyName == "") {
        CairnLogger::CreateLogger(false);
        return;
    }    
    fs::path vPath(aStudyName);
    if (vPath.extension().string() == ".json") vPath.replace_extension("");
    std::string vStudyName = vPath.string();
    this->setObjectName(vStudyName);
   
    if (mMilpData->getVariableTimeStepsFile() == std::string("")) setTimeStepFile(vStudyName + "_ListeOfTimeSteps.csv");
    if (mMilpData->getTypicalPeriodsFile() == std::string("")) setTypicalPeriodsFile(vStudyName + "_ListOfTypicalPeriods.csv");

    mStudy.setResultFile(aResultFile);
    mStudy.setStudyName(aStudyName);   
}
void CairnCore::setResultFile(const std::string& aResultFile) {
    mStudy.setResultFile(aResultFile);
}
void CairnCore::setResultsDir(const std::string& aResultsDir)
{
    mStudy.setResultsDir(aResultsDir);    
}
void CairnCore::setScenarioName(const std::string& aScenarioName) {
    mStudy.setScenarioName(aScenarioName);
}
void CairnCore::setTimeStepFile(const std::string& aTimeStepFile) {
    mMilpData->setVariableTimeStepsFile(aTimeStepFile);
}
void CairnCore::setTypicalPeriodsFile(const std::string& aTypicalPeriodsFile) {
    mMilpData->setTypicalPeriodsFile(aTypicalPeriodsFile);
}

void CairnCore::doInit(bool aLoad)
{
    if (mOptimLogFile == "") {
        /* This is the log file of the solver (used in doSetp) */
        mOptimLogFile = mStudy.getScenarioFile("_optim.log", 0, false);

    cInfo() << "  " ;
    cInfo() << " ############################################################################ " ;
    cInfo() << "  " ;
    cInfo() << " You are using Cairn release " << Cairn_Release << " based on " ;
    cInfo() << " CairnCore    Library " << OptimProblem::getRelease() ;
    cInfo() << " MIPModeler     Engine " << MIPModeler_Release ;
    cInfo() << "  " ;
    cInfo() << " ############################################################################ " ;
    cInfo() << "  " ;

        cInfo() << "... DoInit of CairnCore Module ...";
    }

    try {
        mProblem->doInit(mStudy, aLoad);       
    }
    catch (Cairn_Exception & cairn_err) {
        cCritical() << "Error in doInit of Cairn!";
        throw cairn_err;
    }

    if (npdtTimeshift() > npdtPast())
    {
        Cairn_Exception error("Error in time parameter settings past horizon " + std::to_string(npdtPast()) + " should be >= time shifting " + std::to_string(npdtTimeshift()) + " \nCannot start the simulation!", -1);
        throw error;
    }

    // ensure reset in case of several doInit from GUI
    mMilpData->setStartingAbsoluteTimeStep(0);

    mProblem->setDefaultsResults();
}

void CairnCore::setStopSignal(int* stopSignal){
    mStopSignal=stopSignal;
}

void CairnCore::importTS(const t_list& aTSfileList, const int& iShift = 0)
{
    mTimeSeriesManager->importTS(aTSfileList, mProblem->ListSubscribedVariables(), iShift, mProblem->getSimulationControl()->isCheckTimeSeriesUnits());
}

OrCheckUnits CairnCore::CheckUnits(const std::string& a_FileUnit, const std::string& a_Units, bool a_Check)
{
    return mTimeSeriesManager->CheckUnits(a_FileUnit, a_Units, a_Check);
}

int CairnCore::exportTS(const std::string &aTSfile, int iter, bool rh, const std::string& encoding)
{
    // #######################################
    // ############## EXPORT #################
    // #######################################   
    std::ios_base::openmode openMode = std::ios_base::out;
    if (iter>0){
        openMode = std::ios_base::app;
    }
    std::fstream FileOut;
    if (!CairnUtils::openFileForWriting(FileOut, aTSfile, openMode)) {
        cWarning () << "OptimProblem: couldn't open result file for writing : " << aTSfile;
        return 1;
    }
    cInfo() << " - Export RollingHorizon result timeseries " << aTSfile;
   
    t_mapExchange &vListPublishedVariables = mProblem->ListPublishedVariables() ;
    if(iter==0){        
        FileOut << "Time" << ";" ;
        for (auto& iPublishedVariable : vListPublishedVariables) {                    
            ZEVariables* var = iPublishedVariable.second ;            
            FileOut << var->Name() << ";" ;
        }
        FileOut << std::endl ;
    }
    int ts; //nombre de pdt à écrire : si rolling horizon, que ce qui ne sera pas recalculé (donc timeshift), sinon tout le futur size.
    if (rh)
        ts = mMilpData->timeshift();
    else
        ts = mMilpData->npdt();
    for ( size_t j = 0; j < ts; j++)
    {
        FileOut << (j+1+ts*iter)*mMilpData->pdt() << ";" ;
        for (auto& iPublishedVariable : vListPublishedVariables) {        
            ZEVariables* var = iPublishedVariable.second ;            
            if (var->ptrVariable()->size() > 0)
            {
                FileOut << var->ptrVariable()->at(j + npdtPast()) << ";" ;
            }
        }
        FileOut << std::endl ;
    }

    FileOut.close() ;
    return 0;
}

int CairnCore::exportTS(const std::string& aTSfile, std::map<std::string, std::vector<double>>& resultats, const std::string& encoding)
{
    std::fstream FileOut;
    if (!CairnUtils::openFileForWriting(FileOut, aTSfile)) {
        cWarning() << "OptimProblem Couldn't open result file for writing : " << aTSfile;
        return 1;
    }
    
    FileOut << "Time" << ";";
    for (auto& [key, value] : resultats) {
        FileOut << key << ";";
    }    
    FileOut << std::endl;

    for (size_t j = mMilpData->npdtPast(); j < mMilpData->npdtTot(); j++)
    {
        FileOut << j * (mMilpData->pdt()) << ";";
        for (auto& [key, value] : resultats) {
            FileOut << value[j] << ";";
        }
        FileOut << std::endl;
    }

    FileOut.close();
    return 0;
}


int CairnCore::doStep(const std::string& encoding, const std::map<std::string, bool> paramMap)
{
    mIter += 1;

    bool isRollingHorizon = false;
    if (nbcycle() > 1 || !mStdAloneMode) isRollingHorizon = true; //RollingHorizon (in case of Pegase "!mStdAloneMode" always set)

    if (!mStdAloneMode) mTimeSeriesManager->importTS(mProblem->ListSubscribedVariables());

    cInfo() << "...DoStep CairnCore "  ;
    CairnLogger::Flush();

    /**  Update current absolute timestep and input variables due to TimeShifting */
    mMilpData->prepareOptim() ;
    try {
        mProblem->prepareOptim();
        //no catch because of QWidget error in FBSF
        if (mProblem->getException().error() != 0) {
            Cairn_Exception cairn_error("Fatal Error in prepareOptim Optim Problem: " + mProblem->getException().message(), -1);
            throw cairn_error;
        }
    } catch (...) {
        Cairn_Exception cairn_error((std::string)"Fatal Error in prepareOptim Optim Problem", -1);
        throw cairn_error;
    }

    /** Build optimization problem */
    MIPModeler::MIPModel mipModel;
    MIPModeler::MIPExpression objectiveExpression;

    mProblem->setMIPModel(&mipModel);
    mProblem->setObjective(&objectiveExpression);
    mProblem->setStopSignal(mStopSignal);

    ModelerInterface* pExternalModeler = mipModel.getExternalModeler();
    if (pExternalModeler) {
        ModelerParams vParams;
        vParams.addParam("nbYears", (double)mProblem->NbYear());
        vParams.addParam("nbTimeSteps", (double)mMilpData->npdt());
        vParams.addParam("TimeSteps", mMilpData->TimeSteps());
        vParams.addParam("DiscountRate", mProblem->DiscountRate());
        pExternalModeler->init(vParams);
    }

    try {
        mProblem->buildProblem();
        //no catch because of QWidget error in FBSF
        if (mProblem->getException().error() != 0) {
            cCritical() << "Fatal Error in building Optim Problem";
            throw mProblem->getException();
        }
    }
    catch (std::exception std_exp) {
        Cairn_Exception cairn_error((std::string)std_exp.what(), -1);
        throw cairn_error;
    }
    catch (...) {
        if (mProblem->getException().error() != 0)
            throw mProblem->getException();//TODO: why checked twice? and unify the exception handling 
        else {
            Cairn_Exception cairn_error((std::string)"Fatal Error in building Optim Problem", -1);
            throw cairn_error;
        }
    }

    /** fill in objective function and build matrix */
    mipModel.setObjective(objectiveExpression);
    objectiveExpression.close() ;

    if ( mProblem->getOptimDirection() == std::string("Maximize") ) {
         mipModel.setObjectiveDirection(MIPModeler::MIP_MAXIMIZE);
    }
    else  {
         mipModel.setObjectiveDirection(MIPModeler::MIP_MINIMIZE);
    }

    /** Solve MILP problem */
    try {
        mipModel.buildProblem();
    }
    catch (std::string message) {
        Cairn_Exception error(message, -1);
        throw& error;
    }

    //Don't call the solver if the user stopped the simulation during buildProblem
    if (mStopSignal) {
        if (*mStopSignal == 1) {
            CairnLogger::Flush();
            return 3; //stopped by user TODO: create enum for Status
        }
    }

    cInfo() << "Iteration: " << mIter;

    int cycle = 0;
    if(isRollingHorizon) cycle = mIter + 1;
    mProblem->solveProblem(mOptimLogFile, cycle, paramMap, ExportResultsEveryCycle()); //Need a class structure to support non-bool parameters inside paramMap

    std::string status = mProblem->getOptimisationStatus() ;
    //std::cout << "Flush pour avoir les sorties avec optimisation status " << std::flush;
    CairnLogger::Flush();
    cInfo() << "Optimization status optim : " << status ;
    int istat = mProblem->getInterpretedOptimStatus();
    
    /** Get Solution */
    int nbSol = mProblem->getNumberOfSolutions();
    cInfo() << "Total number of solutions:" << nbSol;

    //Solver Running time
    mSolverRunningTimeAllCycles.push_back(mProblem->getSolverRunningTime());

    bool isCheckConflicts = mProblem->getIsCheckConflicts();
    if (isCheckConflicts) {
        return istat;
    }

    bool isExportParameters = mProblem->getSimulationControl()->isExportParameters();
    if (mIter == 0 && isExportParameters) {
        /* No need to export parameters every iteration */
        mProblem->exportParameters_all_files(mStudy.getScenarioFile("_Parameters.csv", 0, false), encoding);
    }

    bool isExportResults = mProblem->getSimulationControl()->isExportResults();
    if (istat == 2) {
        cWarning() << "CairnCore default solution due to no solution with status =" << status;
        mProblem->setDefaultsResults();
        if (isExportResults)        
            istat = exportResults(0, isRollingHorizon, istat);
    }
    else {
        for (int i = nbSol - 1;i >= 0;i--) {            
            mProblem->readSolution(i);            
            if (istat == 0) {
                cInfo() << "CairnCore solution optimale " << status << ", solution: " << i;                
            }
            else {
                cWarning() << "CairnCore non optimal solution obtained by status =" << status << ", solution: " << i;                
            }
            if (i > 0 && isExportResults) {
                std::map<std::string, std::vector<double>> vResults;
                mProblem->writeSolution(i, vResults);
                exportTS(mStudy.getScenarioFile(".csv", i), vResults);
            }
            /** Export results */
            if (isExportResults) {
                istat = exportResults(i, isRollingHorizon, istat);
            }
        }        
    }
    CairnLogger::Flush();
    return istat ;
}

void CairnCore::exportTotalTimeResolutionAllCycles(const std::string& aFileName)
{
    std::fstream FileOut;
    if (!CairnUtils::openFileForWriting(FileOut, aFileName))
    {
        Cairn_Exception cairn_error((std::string)"CairnCore: Couldn't open file solverRunningTime.csv for writing.", -1);
        throw cairn_error;
    }
    
    FileOut << "sep=;\n";

    std::string header = "; Solver Running Time ; Cumulative Time";

    FileOut << header << "\n";

    for (int i = 0; i < mSolverRunningTimeAllCycles.size(); i++)
    {
        std::string iCycleLine = "Cycle " + std::to_string(i + 1);
        double cumulativeTime = 0.0;
        for (int j = 0; j < i+1; j++)
            cumulativeTime += mSolverRunningTimeAllCycles[j];
        iCycleLine += ";" + std::to_string(mSolverRunningTimeAllCycles[i]) + ";" + std::to_string(cumulativeTime) + "\n";
        FileOut << iCycleLine;
    }
}

int CairnCore::exportResults(const int& aNsol, const bool& isRollingHorizon, const int &istat, const std::string& encoding)
{
    /** Export results */    
    mProblem->exportResults();

    if (istat >= 0) {
        //iter = 0, and rh = false to save all the npdt() points of this cycle
        exportTS(getResultsTimeSeriesFileName(aNsol));
        if (isRollingHorizon && ExportResultsEveryCycle()) {
            std::string vName = "_Results_RH_" + std::to_string(mIter+1) + ".csv";
            exportTS(mStudy.getScenarioFile(vName, aNsol), 0, false, encoding);
        }
    }

    /** Save rollinghorizon results. Standard format: it takes only timeShift points from each iteration if nbcycle() > 1 or Pegase */ 
    if (isRollingHorizon || !mStdAloneMode) {
        exportTS(std::string(mStudy.getScenarioFile("_rollinghorizon.csv", aNsol).c_str()), mIter, (isRollingHorizon || !mStdAloneMode), encoding);
    }

    /** Perform TecEcoEnv analysis */
    if (mProblem->getTecEcoEnv() != nullptr) {
        mProblem->computeTecEcoEnvAnalysis(aNsol, istat);
        try {
            if (istat < 2) exportAnalysis(aNsol, isRollingHorizon, encoding);
        }
        catch (Cairn_Exception& cairn_error) {
            return -1;
        }
    }

    /** Save Hist State */
    mProblem->computeHistNbHours();

    cInfo() << " - Export results done ";

    return istat;
}

void CairnCore::exportAnalysis(const int &aNsol, const bool& isRollingHorizon, const std::string& encoding)
{
    int ierr = 0;
    if (mProblem->getTecEcoEnv() != nullptr)
    {
        if (mProblem->getTecEcoEnv()->Range() == "HIST" || mProblem->getTecEcoEnv()->Range() == "HISTandPLAN")
        {
            try {
                mProblem->exportAllTecEcoEnvAnalysis(mStudy.getScenarioFile("_HIST.csv", aNsol), "HIST", ShowIndicatorDescription(), encoding, false);
                if (isRollingHorizon && ExportResultsEveryCycle()) {
                    std::string vName = "_HIST_RH_" + std::to_string(mIter + 1) + ".csv";
                    mProblem->exportAllTecEcoEnvAnalysis(mStudy.getScenarioFile(vName, aNsol), "HIST", ShowIndicatorDescription(), encoding, isRollingHorizon);
                }
            }
            catch (Cairn_Exception& cairn_err) {
                Cairn_Exception cairn_error((std::string)"export analysis failed!!", -1);
                throw cairn_error;
            }
        }

        if (mProblem->getTecEcoEnv()->Range() == "PLAN" || mProblem->getTecEcoEnv()->Range() == "HISTandPLAN")
        {
            try {
                mProblem->exportAllTecEcoEnvAnalysis(getGlobalResultsFileName(aNsol), "PLAN", ShowIndicatorDescription(), encoding, false, aNsol); // always isRollingHorizon=false // main _PLAN.csv
                mProblem->exportEnvImpactMassIndicators(mStudy.getScenarioFile("_EnvImpactMass.csv", aNsol), encoding);
                if (isRollingHorizon && ExportResultsEveryCycle()) {
                    std::string vName = "_PLAN_RH_" + std::to_string(mIter + 1) + ".csv";
                    mProblem->exportAllTecEcoEnvAnalysis(std::string(mStudy.getScenarioFile(vName, aNsol).c_str()), "PLAN", ShowIndicatorDescription(), encoding, isRollingHorizon, aNsol);
                }
                if (isRollingHorizon) mProblem->exportOptimaSizeAllCycles(mStudy.getScenarioFile("_optimalSize.csv", aNsol, false), mIter + 1);
                exportTotalTimeResolutionAllCycles(mStudy.getScenarioFile("_solverRunningTime.csv", aNsol, false));
            }
            catch (Cairn_Exception& cairn_err) {
                Cairn_Exception cairn_error((std::string)"export analysis failed!!", -1);
                throw cairn_error;
            }
        }
    }
}

//------------------------------------------------------------------------------
int CairnCore::doTerminate()
{   
    cInfo() << "...doTerminate CairnCore"  ;

    if(mProblem && mProblem->getSimulationControl()->isExportJson())
    {
        mProblem->SaveFullArchitecture();
    }

    //Delete _rollingHorizon.csv if non-RollingHorizon because it is the same as _Results.csv in this case (case Pegase)
    if(mIter == 0 && !mStdAloneMode){
        for (int aNsol = 0; aNsol < mProblem->getNumberOfSolutions(); aNsol++) {
            fs::path filename = mStudy.getScenarioFile("_rollinghorizon.csv", aNsol);
            if (fs::exists(filename)) {
                fs::remove(filename);
            }            
        }
    }

    if (mProblem) {
        mProblem->closeExpressions();
    }

    return 0 ;
}



