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
#include "Constants.h"

using namespace CairnConstants;
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
    if (!mProblem || !mProblem->getSimulationControl())
        throw Cairn_Exception("Error: problem or simulationcontrol is not defined!", -1);

    CAIRN_PROFILE_SCOPE("importTS", mIter + 1);

    const bool isCheckUnits = mProblem->getSimulationControl()->isCheckTimeSeriesUnits();
    mTimeSeriesManager->importTS(mProblem->ListSubscribedVariables(), !mStdAloneMode, iShift, isCheckUnits);
}

void CairnCore::importTS(const std::vector<std::string>& aTSfileList, const int& iShift) {
    importTS(CairnUtils::toWStringList(aTSfileList), iShift);
}

void CairnCore::importTS(const std::vector<std::wstring>& aTSfileList, const int& iShift)
{
    if (!mProblem || !mProblem->getSimulationControl())
        throw Cairn_Exception("Error: problem or simulationcontrol is not defined!", -1);

    CAIRN_PROFILE_SCOPE("importTS", mIter + 1);

    const bool isCheckUnits = mProblem->getSimulationControl()->isCheckTimeSeriesUnits();
    mTimeSeriesManager->importTS(aTSfileList, mProblem->ListSubscribedVariables(), !mStdAloneMode, iShift, isCheckUnits);
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

    if (rh)
        cInfo() << "Exporting rollinghorizon timeseries " << aTSfile;
    else
        cInfo() << "Exporting result timeseries " << aTSfile;

    // Write BOM only when:
    // 1) creating a new file (iter == 0)
    // 2) encoding explicitly equals "UTF-8"
    if (iter == 0 && encoding == "UTF-8") {
        writeUTF8BOM(out);
    }

    // Published variables (cache reference)
    t_mapExchange& published = mProblem->ListPublishedVariables();

    // Build a compact list of used variables once to avoid repeated checks in the inner loop.
    struct VarEntry { ZEVariables* var; std::vector<double>* vec; };
    std::vector<VarEntry> usedVars;
    usedVars.reserve(published.size());

    for (auto& entry : published) {
        ZEVariables* var = entry.second;
        if (!var) continue;
        if (!var->IsUsed()) continue;
        // ptrVariable() may return nullptr 
        auto* vec = var->ptrVariable();
        usedVars.push_back({ var, vec });
    }

    // Write header only on first iteration
    if (iter == 0) {
        // Build header line in a single string
        std::string header;
        header.reserve(8 + usedVars.size() * 16); 
        header += "Time;";
        for (const auto& ve : usedVars) {
            header += ve.var->Name();
            header += ';';
        }
        header += '\n';
        out.write(header.data(), static_cast<std::streamsize>(header.size()));
    }

    // Determine number of timesteps to export and cache values
    const int ts = rh ? mMilpData->timeshift() : mMilpData->npdt();
    const int past = npdtPast();
    const double pdt = mMilpData->pdt();

    // Small stack buffer for numeric formatting
    char numbuf[64];

    // Export time series rows
    // For each row, build a single string and write it once.
    for (int j = 0; j < ts; ++j) {
        const double timeValue = (j + 1 + ts * iter) * pdt;

        std::string line;
        // Reserve approximate size: time + separators + values
        line.reserve(32 + usedVars.size() * 16);

        // Format timeValue 
        int len = std::snprintf(numbuf, sizeof(numbuf), DOUBLE_FMT_C, timeValue);
        if (len > 0) line.append(numbuf, static_cast<size_t>(len));
        line.push_back(';');

        // Append each used variable's value if its vector is non-empty 
        const std::size_t idx = static_cast<std::size_t>(j + past);
        for (const auto& ve : usedVars) {
            auto* vec = ve.vec;
            if (vec && !vec->empty()) {
                double v = vec->at(idx);
                int l = std::snprintf(numbuf, sizeof(numbuf), DOUBLE_FMT_C, v);
                if (l > 0) line.append(numbuf, static_cast<size_t>(l));
                line.push_back(';');
            }
        }

        line.push_back('\n');

        // Single write call per line
        out.write(line.data(), static_cast<std::streamsize>(line.size()));
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

    // Write UTF‑8 BOM if requested
    if (encoding == "UTF-8") {
        CairnUtils::writeUTF8BOM(out);
    }

    // Prepare a compact ordered list of keys and pointers to their vectors
    struct KV { const std::string* key; std::vector<double>* values; };
    std::vector<KV> entries;
    entries.reserve(resultats.size());
    for (auto& kv : resultats) {
        entries.push_back({ &kv.first, &kv.second });
    }

    // Write header in one write
    {
        std::string header;
        header.reserve(8 + entries.size() * 16);
        header += "Time;";
        for (const auto& e : entries) {
            header += *e.key;
            header.push_back(';');
        }
        header.push_back('\n');
        out.write(header.data(), static_cast<std::streamsize>(header.size()));
    }

    // Cache time-related values
    const int past = mMilpData->npdtPast();
    const int total = mMilpData->npdtTot();
    const double pdt = mMilpData->pdt();

    // Buffer for numeric formatting
    char numbuf[64];

    // Export rows: j runs from past to total-1 
    for (int j = past; j < total; ++j) {
        const double timeValue = (static_cast<double>(j) * pdt);

        // Build line in a single string and write once
        std::string line;
        line.reserve(32 + entries.size() * 16);

        int len = std::snprintf(numbuf, sizeof(numbuf), "%.15g", timeValue);
        if (len > 0) line.append(numbuf, static_cast<size_t>(len));
        line.push_back(';');

        for (const auto& e : entries) {
            const std::vector<double>* vec = e.values;
            double v = (*vec)[static_cast<std::size_t>(j)];
            int l = std::snprintf(numbuf, sizeof(numbuf), "%.15g", v);
            if (l > 0) line.append(numbuf, static_cast<size_t>(l));
            line.push_back(';');
        }

        line.push_back('\n');
        out.write(line.data(), static_cast<std::streamsize>(line.size()));
    }

    out.close();
    return 0;
}

void CairnCore::importTimeSeriesIfNeeded()
{
    if (mStdAloneMode)
        return;

    try {
        mTimeSeriesManager->clearTS();
        mTimeSeriesManager->addTS(L" "); // virtual file to force import
        mTimeSeriesManager->importTS(mProblem->ListSubscribedVariables(), true);
    }
    catch (const Cairn_Exception&) {
        throw;
    }
}

void CairnCore::flushErrorsIfAny()
{
    const bool hasError = hasErrors();
    const auto entries = flushWarningANDErrors();

    std::ostringstream msg;

    if (!entries.empty())
    {
        constexpr int levelWidth = 10;
        constexpr int nameWidth = 16;

        msg << std::left
            << "Collected warnings and errors:\n"
            << std::setw(levelWidth) << "LEVEL" << ' '
            << std::setw(nameWidth) << "NAME" << ' '
            << "MESSAGE\n";

        msg << std::string(levelWidth + 1 + nameWidth + 1 + 50, '-') << '\n';

        for (const auto& e : entries)
        {
            msg << std::left
                << std::setw(levelWidth)
                << e.levelStr()
                << ' '
                << std::setw(nameWidth)
                << e.objectName
                << ' '
                << e.message
                << '\n';
        }
        cInfo() << msg.str();
    }

    if (hasError)
        throw Cairn_Exception("Error while problem initialization/preparation", -1);

    clearWarningANDErrors();
}

void CairnCore::buildMilpProblem(MIPModeler::MIPModel& mipModel,
    MIPModeler::MIPExpression& objective)
{
    mProblem->buildProblem();
 
    mipModel.setObjective(objective);
    objective.close();

    const bool maximize = (mProblem->getOptimDirection() == "Maximize");
    mipModel.setObjectiveDirection(maximize
        ? MIPModeler::MIP_MAXIMIZE
        : MIPModeler::MIP_MINIMIZE);

    mipModel.buildProblem();
}

void CairnCore::prepareProblem()
{
    CairnLogger::Flush();

    // --- [PROFILING] Sub-phase: problem preparation ---------------------------
    {
        CAIRN_PROFILE_SCOPE("prepareProblem (sub-phase of doStep)", mIter + 1);
        mMilpData->prepareOptim();
        mProblem->prepareOptim();
    }
}

CairnResult CairnCore::buildANDsolveProblem(const std::string& encoding, const std::map<std::string, bool>& paramMap)
{
    const bool isRollingHorizon = (nbcycle() > 1) || (!mStdAloneMode);
    const int cycle = isRollingHorizon ? mIter + 1 : 0;
    const std::string cycleStr = std::to_string(cycle);

    CairnResult result;

    try {
        // --- Setup modeler --------------------------------------------------------
        MIPModeler::MIPModel mipModel;
        MIPModeler::MIPExpression objective;
        auto* problem = mProblem;

        problem->setMIPModel(&mipModel);
        problem->setObjective(&objective);
        problem->setStopSignal(mStopSignal);

        initExternalModeler(mipModel);

        // --- [PROFILING] Sub-phase: build the MILP matrix --------------------------
        {
            CAIRN_PROFILE_SCOPE("buildProblem (sub-phase of doStep)", mIter + 1);
            buildMilpProblem(mipModel, objective);
        }

        if (userStoppedSimu()) {
            CairnLogger::Flush();
            result.status = 3;
            return result;
        }

        // --- Solve problem ------------------------------------------------------------
        cInfo() << "  ";
        cInfo() << "----- Solving problem (Cycle #" << cycleStr << " ) -----";
        cInfo() << "  ";

        // --- [PROFILING] Sub-phase: solver execution -----------------------------
        {
            CAIRN_PROFILE_SCOPE("solveProblem (sub-phase of doStep)", mIter + 1);
            problem->solveProblem(mOptimLogFile, cycle, paramMap, ExportResultsEveryCycle());
        }

        const std::string status = problem->getOptimisationStatus();
        result.status = problem->getInterpretedOptimStatus();
        const int nbSolutions = problem->getNumberOfSolutions();

        cInfo() << "Optimization status: " << status;
        cInfo() << "Total number of solutions: " << nbSolutions;

        CairnLogger::Flush();

        // Stop here in case of check-conflicts only; don't export results
        if (problem->getIsCheckConflicts()) {
            return result;
        }

        // Export parameters, if needed (only on first iteration)
        if (mIter == 0 && problem->getSimulationControl()->isExportParameters()) {
            problem->exportParameters_all_files(mStudy.getScenarioFile("_Parameters.csv", 0, false));
        }

        // --- Export results ---------------------------------------------------------------------
        cInfo() << "  ";
        cInfo() << "----- Export Results (Cycle #" << cycleStr << " ) -----";

        // --- [PROFILING] Sub-phase: result export -------------------------------
        {
            CAIRN_PROFILE_SCOPE("exportResults (sub-phase of doStep)", mIter + 1);
            if (result.status == 2) {
                handleNoSolution(status, isRollingHorizon, result.status);
            }
            else {
                processSolutions(nbSolutions, status, isRollingHorizon, result.status);
            }
        }

        cInfo() << "  ";
        cInfo() << "----- End of Cycle #" << cycleStr << " -----";
        cInfo() << "  ";

        CairnLogger::Flush();
    }
    catch (const Cairn_Exception& e)
    {
        result.status = -1;
        result.error = "Error while building or solving the problem: ";
        result.error += e.message();
    }
    catch (const std::exception& e)
    {
        result.status = -1;
        result.error = "Error while building or solving the problem: ";
        result.error += e.what();
    }
    catch (...)
    {
        result.status = -1;
        result.error = "Unknown error while building or solving the problem.";
    }

    return result;
}

CairnResult CairnCore::doStep(const std::string& encoding, const std::map<std::string, bool>& paramMap)
{
    CairnResult result;

    {
        // --- [PROFILING] entire doStep body: full iteration time + memory ---------
        CAIRN_PROFILE_ITERATION(mIter + 1);

        try
        {
            incrementCycleNum();

            const bool isRollingHorizon = (nbcycle() > 1) || (!mStdAloneMode);
            const int cycle = isRollingHorizon ? mIter + 1 : 0;

            importTimeSeriesIfNeeded();

            cInfo() << "  ";
            cInfo() << "----- DoStep CairnCore (Cycle #" << std::to_string(cycle) << " ) -----";
            cInfo() << "  ";

            prepareProblem();

            flushErrorsIfAny();
        }
        catch (const Cairn_Exception& e)
        {
            result.status = -1;
            result.error = "Error while preparing the problem: ";
            result.error += e.message();
            return result;
        }
        catch (const std::exception& e)
        {
            result.status = -1;
            result.error = "Error while preparing the problem: ";
            result.error += e.what();
            return result;
        }
        catch (...)
        {
            result.status = -1;
            result.error = "Unknown error while preparing the problem.";
            return result;
        }

        result = buildANDsolveProblem(encoding, paramMap);
    }

    return result;
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
    auto* problem = mProblem;
    auto& study = mStudy;

    const bool isExportResults = problem->getSimulationControl()->isExportResults();

    // Reuse the results map across iterations to avoid repeated allocations
    std::map<std::string, std::vector<double>> results;

    for (int i = nbSol - 1; i >= 0; --i) 
    {
        problem->readSolution(i);

        cInfo() << "  ";
        cInfo() << "Exporting results for "
            << (istat == 0 ? "solution" : "non‑optimal solution")
            << " #" << i << " (status = " << status << ")";

        if (i > 0 && isExportResults) {
            // Clear previous contents but keep allocated memory
            for (auto& kv : results) {
                kv.second.clear();
            }
            results.clear();

            problem->writeSolution(i, results);

            const std::string fileName = study.getScenarioFile(".csv", i);
            exportTS(fileName, results);
        }

        problem->computeTecEcoEnvAnalysis(i);

        if (isExportResults) 
            istat = exportResults(i, isRollingHorizon, istat);
    }
}

int CairnCore::exportResults(int aNsol, bool isRollingHorizon, int istat, const std::string& encoding)
{
    auto* problem = mProblem;
    auto& study = mStudy;

    // Update published variables
    problem->populatePublishedVars();

    // Export full time series for valid solutions
    if (istat >= 0) {
        exportTS(getResultsTimeSeriesFileName(aNsol));

        if (isRollingHorizon && ExportResultsEveryCycle()) {
            const std::string fileNameSuffix = "_Results_RH_" + std::to_string(mIter + 1) + ".csv";
            exportTS(study.getScenarioFile(fileNameSuffix, aNsol), 0, false, encoding);
        }
    }

    // Export rolling horizon results (multi-cycle or Pegase)
    const bool needRollingHorizon = isRollingHorizon || !mStdAloneMode;
    if (needRollingHorizon) {
        exportTS(study.getScenarioFile("_rollinghorizon.csv", aNsol),
            mIter, needRollingHorizon, encoding);
    }

    // Export TecEcoEnv analysis
    try {
        if (istat < 2)
            exportAnalysis(aNsol, isRollingHorizon, encoding);
    }
    catch (const Cairn_Exception&) {
        return -1;
    }

    // Save historical state 
    if (auto* pTecEco = dynamic_cast<TecEcoCompo*>(problem->getTecEcoAnalysis()->parent()))
    {
        pTecEco->computeHistNbHours();
    }

    cInfo() << "Exporting results done!";
    return istat;
}

void CairnCore::exportAnalysis(int aNsol, bool isRollingHorizon, const std::string& encoding)
{
    auto* problem = mProblem;
    if (!problem)
        return;

    auto* pTecEcoAnalysis = problem->getTecEcoAnalysis();
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
        problem->exportAllTecEcoEnvAnalysis(
            histFile,
            "HIST", showDesc, encoding, false
        );

        // Rolling horizon HIST export
        if (exportRH) {
            const std::string fileRHSuffix = "_HIST_RH_" + std::to_string(mIter + 1) + ".csv";
            problem->exportAllTecEcoEnvAnalysis(
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
        problem->exportAllTecEcoEnvAnalysis(
            planFile,
            "PLAN", showDesc, encoding, false, aNsol);

        // Additional PLAN-related exports : a dedicated file for env impact mass indicators
        problem->exportEnvImpactMassIndicators(
            mStudy.getScenarioFile("_EnvImpactMass.csv", aNsol),
            encoding
        );

        // Rolling horizon PLAN export
        if (exportRH) {
            const std::string fileRHSuffix = "_PLAN_RH_" + std::to_string(mIter + 1) + ".csv";
            problem->exportAllTecEcoEnvAnalysis(
                mStudy.getScenarioFile(fileRHSuffix, aNsol),
                "PLAN", showDesc, encoding, isRollingHorizon, aNsol
            );
        }

        // Rolling horizon optimal size
        if (isRollingHorizon) {
            problem->exportOptimaSizeAllCycles(
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
int CairnCore::saveStudy(const std::string& filename, const std::string& posAlgorithm)
{
    if (!mProblem)
        return -1;

    // --- [PROFILING] Phase: save study (JSON architecture export) ------------
    //   iterationId = -1 marks it as a global phase (only once).
    {
        CAIRN_PROFILE_SCOPE("saveStudy", -1);
        return mProblem->SaveFullArchitecture(filename, posAlgorithm);
    }
}


int CairnCore::doTerminate()
{
    cInfo() << "----- DoTerminate CairnCore -----";

    if (mProblem && mProblem->getSimulationControl()->isExportJson())
    {
        saveStudy();
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

#if ENABLE_PROFILING

CairnProfiling::ScopedProfiler
CairnCore::makeIterationProfiler() const
{
    return CairnProfiling::ScopedProfiler(
        CAIRN_PROFILE_ITERATION_PHASE,
        mIter + 1);
}

#endif