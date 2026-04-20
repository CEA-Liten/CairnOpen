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
#include "CarrierTypes.h"

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

    mProblem = new OptimProblem(this, "OptimProblem", mMilpData, mStdAloneMode) ;

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
    CarrierTypes::Load(exeDir);

    mMilpData = new MilpData(this, "MilpData", aGlobalTimeStepFile, aGlobalTypicalPeriodsFile);

    setStudyName(aStudyName, aResultFile);
    setScenarioName(aScenarioName);

    mProblem = new OptimProblem(this, "OptimProblem", mMilpData, mStdAloneMode) ;

    mTimeSeriesManager = new TimeSeriesManager(*mMilpData);
}

CairnCore::~CairnCore()
{
    delete mProblem;
    delete mMilpData;
    delete mTimeSeriesManager;

    // Avoid dangling pointer
    mProblem = nullptr;
    mMilpData = nullptr;
    mTimeSeriesManager = nullptr;
}

void CairnCore::setStudyName(const std::string& aStudyName, const std::string& aResultFile)
{
    // StudyName = <ProjectDir> / <StudyName> .json
    if (aStudyName == "") {
       // CairnLogger::CreateLogger(false);
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
    mIter = -1;

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

void CairnCore::setStdAloneMode(const bool& abool)
{ 
    mStdAloneMode = abool;
    if (mProblem) mProblem->setStdAloneMode(abool);
    if (mTimeSeriesManager) {
        if (!abool) {
            mTimeSeriesManager->setReaderKind("pegase");
        }
        else {
            mTimeSeriesManager->setReaderKind("csv");
        }
    }    
}

void CairnCore::importTS(const std::vector<std::string>& aTSfileList, const int& iShift) {
    importTS(CairnUtils::toWStringList(aTSfileList), iShift);
}

void CairnCore::importTS(const std::vector<std::wstring>& aTSfileList, const int& iShift)
{
    try {
        mTimeSeriesManager->importTS(aTSfileList, mProblem->ListSubscribedVariables(), !mStdAloneMode, iShift, mProblem->getSimulationControl()->isCheckTimeSeriesUnits());
    }
    catch (Cairn_Exception& error) {
        throw error;
    }
}

OrCheckUnits CairnCore::CheckUnits(const std::string& a_FileUnit, const std::string& a_Units, bool a_Check)
{
    return mTimeSeriesManager->CheckUnits(a_FileUnit, a_Units, a_Check);
}

int CairnCore::exportTS(const std::string& aTSfile,
    int iter, bool rh, const std::string& encoding)
{
    // Choose write or append mode
    std::ios_base::openmode mode = (iter > 0)
        ? std::ios_base::app | std::ios::binary
        : std::ios_base::out | std::ios::binary;

    std::fstream out;
    if (!CairnUtils::openFileForWriting(out, aTSfile, mode)) {
        cWarning() << "OptimProblem: couldn't open result file for writing: " << aTSfile;
        return 1;
    }

    cInfo() << " - Export RollingHorizon result timeseries " << aTSfile;

    // Write BOM only when:
    // 1) creating a new file (iter == 0)
    // 2) encoding explicitly equals "UTF-8"
    if (iter == 0 && encoding == "UTF-8") {
        writeUTF8BOM(out);
    }

    // Published variables
    t_mapExchange& published = mProblem->ListPublishedVariables();

    // Write header only on first iteration
    if (iter == 0) {
        out << "Time;";
        for (auto& entry : published) {
            ZEVariables* var = entry.second;
            out << var->Name() << ";";
        }
        out << "\n";
    }

    // Determine number of timesteps to export
    const int ts = rh ? mMilpData->timeshift()
        : mMilpData->npdt();

    const int past = npdtPast();
    const double pdt = mMilpData->pdt();

    // Export time series rows
    for (int j = 0; j < ts; ++j) {
        const double timeValue = (j + 1 + ts * iter) * pdt;
        out << timeValue << ";";

        for (auto& entry : published) {
            ZEVariables* var = entry.second;
            auto* vec = var->ptrVariable();

            if (!vec->empty()) {
                out << vec->at(j + past) << ";";
            }
        }

        out << "\n";
    }

    out.close();
    return 0;
}

int CairnCore::exportTS(const std::string& aTSfile,
    std::map<std::string, std::vector<double>>& resultats,
    const std::string& encoding)
{
    // Always open in binary mode to ensure BOM is written correctly
    std::ios_base::openmode mode = std::ios::out | std::ios::binary;

    std::fstream out;
    if (!CairnUtils::openFileForWriting(out, aTSfile, mode)) {
        cWarning() << "OptimProblem: Couldn't open result file for writing: " << aTSfile;
        return 1;
    }

    // Write UTF‑8 BOM
    if (encoding == "UTF-8") {
        CairnUtils::writeUTF8BOM(out);
    }

    // Write header
    out << "Time;";
    for (auto& [key, values] : resultats) {
        out << key << ";";
    }
    out << "\n";

    const int past = mMilpData->npdtPast();
    const int total = mMilpData->npdtTot();
    const double pdt = mMilpData->pdt();

    // Export time series rows
    for (int j = past; j < total; ++j) {
        out << (j * pdt) << ";";

        for (auto& [key, values] : resultats) {
            // values[j] is safe because caller guarantees correct vector sizes
            out << values[j] << ";";
        }

        out << "\n";
    }

    out.close();
    return 0;
}

int CairnCore::doStep(const std::string& encoding, const std::map<std::string, bool>& paramMap)
{
    mIter++;

    const bool isRollingHorizon = (nbcycle() > 1) || (!mStdAloneMode); //RollingHorizon is always ON if non-StdAlone

    // Import timeseries if non-StdAlone; in case of StdAlone the timeseries is imported before calling doStep
    if (!mStdAloneMode) {
        try {
            mTimeSeriesManager->importTS(mProblem->ListSubscribedVariables());
        }
        catch (const Cairn_Exception& e) {
            throw;
        }
    }

    cInfo() << "...DoStep CairnCore";
    CairnLogger::Flush();

    // Prepare optim problem
    mMilpData->prepareOptim();
    try {
        mProblem->prepareOptim();
    }
    catch (...) {
        throw Cairn_Exception("Fatal Error in prepareOptim Optim Problem", -1);
    }

    // Setup MILP model
    MIPModeler::MIPModel mipModel;
    MIPModeler::MIPExpression objective;

    mProblem->setMIPModel(&mipModel);
    mProblem->setObjective(&objective);
    mProblem->setStopSignal(mStopSignal);

    // Init external Modeler
    initExternalModeler(mipModel);

    // Build optim problem 
    try {
        mProblem->buildProblem();
    }
    catch (const std::exception& e) {
        throw Cairn_Exception(e.what(), -1);
    }
    catch (...) {
        throw Cairn_Exception("Fatal Error in building Optim Problem", -1);
    }

    // Fill in objective function and build matrix 
    mipModel.setObjective(objective);
    objective.close();
    const bool maximize = (mProblem->getOptimDirection() == "Maximize");
    mipModel.setObjectiveDirection(maximize
        ? MIPModeler::MIP_MAXIMIZE
        : MIPModeler::MIP_MINIMIZE);
    
    // Build MILP problem
    try {
        mipModel.buildProblem();
    }
    catch (const std::string& msg) {
        throw Cairn_Exception(msg, -1);
    }
    
    // Terminate if user stopped the simulation
    if (userStoppedSimu()) {
        CairnLogger::Flush();
        return 3; // TODO: enum
    }

    cInfo() << "Iteration: " << mIter;

    // Solve problem
    const int cycle = isRollingHorizon ? mIter + 1 : 0;
    mProblem->solveProblem(mOptimLogFile, cycle, paramMap, ExportResultsEveryCycle());

    const std::string status = mProblem->getOptimisationStatus();
    CairnLogger::Flush();
    cInfo() << "Optimization status: " << status;

    int istat = mProblem->getInterpretedOptimStatus();
    const int nbSol = mProblem->getNumberOfSolutions();
    cInfo() << "Total number of solutions:" << nbSol;

    mSolverRunningTimeAllCycles.push_back(mProblem->getSolverRunningTime());

    // Stop here in case of check-conflicts only; don't export results
    if (mProblem->getIsCheckConflicts()) {
        return istat;
    }

    // Export parameters, if needed
    if (mIter == 0 && mProblem->getSimulationControl()->isExportParameters()) {
        mProblem->exportParameters_all_files( mStudy.getScenarioFile("_Parameters.csv", 0, false));
    }

    // Process solutions
    if (istat == 2) {
        handleNoSolution(status, isRollingHorizon, istat);
    }
    else {
        processSolutions(nbSol, status, isRollingHorizon, istat);
    }

    CairnLogger::Flush();
    return istat;
}

void CairnCore::initExternalModeler(MIPModeler::MIPModel& mipModel)
{
    if (auto* pExternalModeler = mipModel.getExternalModeler()) {
        ModelerParams params;

        if (mMilpData) {
            params.addParam("nbTimeSteps", double(mMilpData->npdt()));
            params.addParam("TimeSteps", mMilpData->TimeSteps());
        }

        if (auto* pTecEcoAnalysis = mProblem->getTecEcoAnalysis()) {
            params.addParam("nbYears", double(pTecEcoAnalysis->NbYear()));
            params.addParam("DiscountRate", pTecEcoAnalysis->DiscountRate());
        }
        else {
            /* default values ? */
        }

        pExternalModeler->init(params);
    }
}

bool CairnCore::userStoppedSimu() const
{
    return mStopSignal && *mStopSignal == 1;
}

void CairnCore::handleNoSolution(const std::string& status, bool isRollingHorizon, int& istat)
{
    cWarning() << "CairnCore default solution due to no solution with status =" << status;
    mProblem->setDefaultsResults();

    if (mProblem->getSimulationControl()->isExportResults()) {
        istat = exportResults(0, isRollingHorizon, istat);
    }
}

void CairnCore::processSolutions(int nbSol, const std::string& status, bool isRollingHorizon, int& istat)
{
    const bool isExportResults = mProblem->getSimulationControl()->isExportResults();

    for (int i = nbSol - 1; i >= 0; --i) {
        mProblem->readSolution(i);

        if (istat == 0)
            cInfo() << "CairnCore solution optimale " << status << ", solution: " << i;
        else
            cWarning() << "CairnCore non optimal solution obtained by status =" << status << ", solution: " << i;

        if (i > 0 && isExportResults) {
            std::map<std::string, std::vector<double>> results;
            mProblem->writeSolution(i, results);
            exportTS(mStudy.getScenarioFile(".csv", i), results);
        }

        mProblem->computeTecEcoEnvAnalysis(i);

        if (isExportResults) {
            istat = exportResults(i, isRollingHorizon, istat);
        }
    }
}

void CairnCore::exportTotalTimeResolutionAllCycles(const std::string& fileName, const std::string& encoding)
{
    std::fstream out; 
    if (!CairnUtils::openFileForWriting(out, fileName, std::ios::out | std::ios::binary)) { 
        throw Cairn_Exception("CairnCore: Couldn't open file for writing: " + fileName, -1); 
    }

    // UTF‑8 BOM for Excel
    if (encoding == "UTF-8") {
        writeUTF8BOM(out);
    }

    // Header
    out << ";Solver Running Time;Cumulative Time\n";

    double cumulative = 0.0;

    for (size_t i = 0; i < mSolverRunningTimeAllCycles.size(); ++i) {
        const double current = mSolverRunningTimeAllCycles[i];
        cumulative += current;

        out << "Cycle " << (i + 1)
            << ";" << current
            << ";" << cumulative
            << "\n";
    }
}

int CairnCore::exportResults(int aNsol, bool isRollingHorizon, int istat, const std::string& encoding)
{
    // Update published variables
    mProblem->populatePublishedVars();

    // Export full time series for valid solutions
    if (istat >= 0) {
        exportTS(getResultsTimeSeriesFileName(aNsol));

        if (isRollingHorizon && ExportResultsEveryCycle()) {
            const std::string fileNameSuffix = "_Results_RH_" + std::to_string(mIter + 1) + ".csv";
            exportTS(mStudy.getScenarioFile(fileNameSuffix, aNsol), 0, false, encoding);
        }
    }

    // Export rolling horizon results (multi-cycle or Pegase)
    const bool needRollingHorizon = isRollingHorizon || !mStdAloneMode;
    if (needRollingHorizon) {
        exportTS(mStudy.getScenarioFile("_rollinghorizon.csv", aNsol),
            mIter, needRollingHorizon, encoding);
    }

    //Export TecEcoEnv analysis
    try {
        if (istat < 2) {
            exportAnalysis(aNsol, isRollingHorizon, encoding);
        }
    }
    catch (const Cairn_Exception&) {
        return -1;
    }

    // Save historical state
    if (auto* pTecEco = dynamic_cast<TecEcoCompo*>(mProblem->getTecEcoAnalysis()->parent()))
    {
        pTecEco->computeHistNbHours();
    }

    cInfo() << " - Export results done! ";
    return istat;
}

void CairnCore::exportAnalysis(int aNsol, bool isRollingHorizon, const std::string& encoding)
{
    auto* pTecEcoAnalysis = mProblem->getTecEcoAnalysis();
    if (!pTecEcoAnalysis)
        return;

    const bool showDesc = ShowIndicatorDescription();
    const bool exportRH = isRollingHorizon && ExportResultsEveryCycle();

    // ------------- 
    // HIST section
    // -------------
    try {
        // Main HIST export
        mProblem->exportAllTecEcoEnvAnalysis(
            mStudy.getScenarioFile("_HIST.csv", aNsol),
            "HIST", showDesc, encoding, false
        );

        // Rolling horizon HIST export
        if (exportRH) {
            const std::string fileRHSuffix = "_HIST_RH_" + std::to_string(mIter + 1) + ".csv";

            mProblem->exportAllTecEcoEnvAnalysis(
                mStudy.getScenarioFile(fileRHSuffix, aNsol),
                "HIST", showDesc, encoding, isRollingHorizon
            );
        }
    }
    catch (...) {
        throw Cairn_Exception("export analysis failed!!", -1);
    }

    // ------------- 
    // PLAN section
    // ------------- 
    try {
        // Main PLAN export
        mProblem->exportAllTecEcoEnvAnalysis(
            getGlobalResultsFileName(aNsol),
            "PLAN", showDesc, encoding, false, aNsol);

        // Additional PLAN-related exports : a dedicated file for env impact mass indicators
        mProblem->exportEnvImpactMassIndicators(
            mStudy.getScenarioFile("_EnvImpactMass.csv", aNsol),
            encoding
        );

        // Rolling horizon PLAN export
        if (exportRH) {
            const std::string fileRHSuffix = "_PLAN_RH_" + std::to_string(mIter + 1) + ".csv";

            mProblem->exportAllTecEcoEnvAnalysis(
                mStudy.getScenarioFile(fileRHSuffix, aNsol),
                "PLAN", showDesc, encoding, isRollingHorizon, aNsol
            );
        }

        // Rolling horizon optimal size
        if (isRollingHorizon) {
            mProblem->exportOptimaSizeAllCycles(
                mStudy.getScenarioFile("_optimalSize.csv", aNsol, false),
                mIter + 1
            );
        }

        // Solver running time export
        exportTotalTimeResolutionAllCycles(
            mStudy.getScenarioFile("_solverRunningTime.csv", aNsol, false),
            encoding
        );
    }
    catch (...) {
        throw Cairn_Exception("export analysis failed!!", -1);
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



