#include <iostream>
#include <string>
#include <math.h> 
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
#include "Profiler.h"

using namespace CairnUtils;
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
    GS::iVerbose=0;

    loadDefUnits();

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
    GS::iVerbose=0;

    loadDefUnits();

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

void CairnCore::loadDefUnits() const 
{
    const std::string exeDir = std::getenv("CAIRN_BIN") + (std::string)"/../resources/DefUnits.json";
    UnitsConverter::Load(exeDir);
    CarrierTypes::Load(exeDir);
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
    // [PROFILING] Stamp program start as early as possible
    CAIRN_PROFILE_PROGRAM_START();

    // --- [PROFILING] Measure the entire init phase ------------------
    // iterationId = -1 -> "not part of a rolling-horizon cycle"
    CAIRN_PROFILE_SCOPE("doInit", -1);

    mIter = -1;

    if (mOptimLogFile == "") {
        /* This is the log file of the solver (used in doSetp) */
        mOptimLogFile = mStudy.getScenarioFile("_optim.log", 0, false);
    }

    cInfo() << "  ";
    cInfo() << " ############################################################################ " ;
    cInfo() << "  ";
    cInfo() << " You are using " << Cairn_Release;
    cInfo() << " MIPModeler Engine: " << MIPModeler_Release ;
    cInfo() << "  ";
    cInfo() << " ############################################################################ " ;
    cInfo() << "  ";

    cInfo() << "----- DoInit of CairnCore -----";
    cInfo() << "  ";

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

    //cInfo() << "  ";
    //cInfo() << "----- Successful DoInit step -----";
    //cInfo() << "  ";
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


void CairnCore::addTS(const std::wstring& a_fileName)
{
    if (mTimeSeriesManager) {
        mTimeSeriesManager->addTS(a_fileName);
    }
}

void CairnCore::addTS(const t_dict& a_TS)
{
    if (mTimeSeriesManager) {
        mTimeSeriesManager->addTS(a_TS);
    }
}

bool CairnCore::checkTS(string& a_ErrMsg)
{
    bool vRet = false;
    if (mTimeSeriesManager) {
        vRet = mTimeSeriesManager->checkTS(a_ErrMsg);
    }
    return vRet;
}

void CairnCore::importTS(const int& iShift)
{
    try {
        mTimeSeriesManager->importTS(mProblem->ListSubscribedVariables(), !mStdAloneMode, iShift, mProblem->getSimulationControl()->isCheckTimeSeriesUnits());
    }
    catch (Cairn_Exception& error) {
        throw error;
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
        cError() << "OptimProblem: couldn't open result file for writing: " << aTSfile;
        return 1;
    }

    if(rh)
        cInfo() << "Exporting rollinghorizon timeseries " << aTSfile;
    else 
        cInfo() << "Exporting result timeseries " << aTSfile;

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
        cError() << "OptimProblem: Couldn't open result file for writing: " << aTSfile;
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

    // --- [PROFILING] Outer scope: full iteration time + memory --------------
    // This record tracks the entire doStep body: rssAfter - rssBefore
    // gives net memory retained after one rolling-horizon cycle.
    CAIRN_PROFILE_ITERATION(mIter + 1);
    // CAIRN_PROFILE_SCOPE("doStep", mIter + 1); // works but "doStep" must match Profile::CAIRN_PROFILE_ITERATION_PHASE

    const bool isRollingHorizon = (nbcycle() > 1) || (!mStdAloneMode); //RollingHorizon is always ON if non-StdAlone

    // Import timeseries if non-StdAlone; in case of StdAlone the timeseries is imported before calling doStep
    if (!mStdAloneMode) {
        try {
            mTimeSeriesManager->clearTS();
            mTimeSeriesManager->addTS(L" "); // add a virtual file to force importTS
            mTimeSeriesManager->importTS(mProblem->ListSubscribedVariables(), true);
        }
        catch (const Cairn_Exception& e) {
            throw;
        }
    }

    const int cycle = isRollingHorizon ? mIter + 1 : 0;

    cInfo() << "  ";
    cInfo() << "----- DoStep CairnCore (Cycle #" << int(cycle) << " ) -----";
    cInfo() << "  ";

    CairnLogger::Flush();

    // --- [PROFILING] Sub-phase: problem preparation ---------------------------
    // Covers mMilpData->prepareOptim() + mProblem->prepareOptim().
    // These build the timeseries update and reset component state before each
    // MILP solve. Their duration is often dominated by matrix re-indexing.
    {
        CAIRN_PROFILE_SCOPE("prepareProblem (sub-phase of doStep)", mIter + 1);

        mMilpData->prepareOptim();
        try {
            mProblem->prepareOptim();
        }
        catch (...) {
            throw Cairn_Exception("Fatal Error in prepareOptim Optim Problem", -1);
        }
    }

    // Setup MILP model
    MIPModeler::MIPModel mipModel;
    MIPModeler::MIPExpression objective;

    mProblem->setMIPModel(&mipModel);
    mProblem->setObjective(&objective);
    mProblem->setStopSignal(mStopSignal);

    // Init external Modeler
    initExternalModeler(mipModel);

    // --- [PROFILING] Sub-phase: build the MILP matrix -------------------------
    // Covers OptimProblem::buildProblem() (component constraints, bus
    // constraints, objective assembly) plus the MIPModel::buildProblem() call
    // that translates the MIPExpression tree into the solver matrix.
    // Peak memory here captures the largest in-memory MILP representation.
    {
        CAIRN_PROFILE_SCOPE("buildProblem (sub-phase of doStep)", mIter + 1);

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
		catch (const std::runtime_error& e) {
			throw Cairn_Exception(e.what(), -1);
		}
    }
    
    // Terminate if user stopped the simulation
    if (userStoppedSimu()) {
        CairnLogger::Flush();
        return 3; // TODO: enum
    }

    //cInfo() << "  ";
    //cInfo() << "----- Successful DoStep (Cycle #" << int(cycle) << " ) -----";
    //cInfo() << "  ";

    // Solve problem
    cInfo() << "  ";
    cInfo() << "----- Solving problem (Cycle #" << int(cycle) << " ) -----";
    cInfo() << "  ";

    // --- [PROFILING] Sub-phase: solver execution -----------------------------
    // This is the most variable and often dominant phase. Its duration is almost
    // entirely inside the MIP solver (Cplex / Highs)
    // Memory delta here shows whether the solver heap-allocates branch nodes.
    {
        CAIRN_PROFILE_SCOPE("solveProblem (sub-phase of doStep)", mIter + 1);
        mProblem->solveProblem(mOptimLogFile, cycle, paramMap, ExportResultsEveryCycle());
    }
    
    const std::string status = mProblem->getOptimisationStatus();

    CairnLogger::Flush();

    cInfo() << "Optimization status: " << status;

    int istat = mProblem->getInterpretedOptimStatus();
    const int nbSol = mProblem->getNumberOfSolutions();

    cInfo() << "Total number of solutions: " << nbSol;

    // Stop here in case of check-conflicts only; don't export results
    if (mProblem->getIsCheckConflicts()) {
        return istat;
    }

    // Export parameters, if needed
    if (mIter == 0 && mProblem->getSimulationControl()->isExportParameters()) {
        mProblem->exportParameters_all_files( mStudy.getScenarioFile("_Parameters.csv", 0, false));
    }

    // Process solutions
    cInfo() << "  ";
    cInfo() << "----- Export Results (Cycle #" << int(cycle) << " ) -----";

    // --- [PROFILING] Sub-phase: result export -------------------------------
    // Covers reading solution, computing indicators, and CSV writing
    // Memory growth here indicates accumulation of per-cycle CSV buffers.
    {
        CAIRN_PROFILE_SCOPE("exportResults (sub-phase of doStep)", mIter + 1);
        if (istat == 2) {
            handleNoSolution(status, isRollingHorizon, istat);
        }
        else {
            processSolutions(nbSol, status, isRollingHorizon, istat);
        }
    }

    cInfo() << "  ";
    cInfo() << "----- End of Cycle #" << int(cycle) << "-----";
    cInfo() << "  ";

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
    cInfo() << "Export default solution as no solution has been found by the solver" << " (status = " << status << ")";

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

        cInfo() << "  ";
        cInfo() << "Exporting results for "
            << (istat == 0 ? "solution" : "non‑optimal solution")
            << " #" << i << " (status = " << status << ")";

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

    cInfo() << "Exporting results done!";
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
        const std::string histFile = mStudy.getScenarioFile("_HIST.csv", aNsol);
        cInfo() << "Exporting HIST file " << histFile;
        mProblem->exportAllTecEcoEnvAnalysis(
            histFile,
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
        const std::string planFile = getGlobalResultsFileName(aNsol);
        cInfo() << "Exporting PLAN file " << planFile;
        mProblem->exportAllTecEcoEnvAnalysis(
            planFile,
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
    }
    catch (...) {
        throw Cairn_Exception("export analysis failed!!", -1);
    }
}

//------------------------------------------------------------------------------
int CairnCore::doTerminate()
{
    cInfo() << "----- DoTerminate CairnCore -----";

    // --- [PROFILING] Phase: save study (JSON architecture export) ------------
    //   SaveFullArchitecture() serialises the full component graph to a .json
    //   file. It only runs when isExportJson() is true, so the record only
    //   appears in the CSV when the save actually took place. iterationId = -1
    //   marks it as a global phase (only once like doInit).
    {
        CAIRN_PROFILE_SCOPE("saveStudy", -1);
        if (mProblem && mProblem->getSimulationControl()->isExportJson())
        {
            mProblem->SaveFullArchitecture();
        }
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

    // --- [PROFILING] Flush profiling data to the results directory --------------
    // If the results directory is not yet known, "."is used.
    {
        std::string outDir = mStudy.resultsDir();
        if (outDir.empty()) outDir = ".";
        CAIRN_PROFILE_FLUSH(outDir, mStudy.StudyName());
    }

    return 0 ;
}



