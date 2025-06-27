#include <iostream>
#include <string>
#include <math.h>       /* fabs, log, pow */
#include <fstream>
#include <vector>
#include <assert.h>


#include "MIPModeler.h"

#include "CairnCore.h"
#include "GlobalSettings.h"
#include "OptimProblem.h"
#include "MilpData.h"
#include "ZEVariables.h"
#include <QDebug>
#include <QVector>
#include <QException>

#include "OrUnitsConverter.h"
#include "CairnUtils.h"
using namespace CairnUtils;

//using namespace MIPModeler;
using namespace GS;

// Non StdAloneMode : linked to Pegase, get Time Data from argument - No use of Cairn SimulationControl object
CairnCore::CairnCore(QObject *aParent,
                       const QString & aName,
                       const double & aPdt,
                       const uint & aNpdtPast,
                       const uint & aNpdtFuture,
                       const uint & aTimeshift,
                       const uint & aIHMFuturSize,
                       const QString & aGlobalTimeStepFile,
                       const QString & aGlobalTypicalPeriodsFile,
                       const QString & aStudyFile,
                       const QString & aResultFile, 
                       const QString& aScenarioName)
    : QObject(aParent),
    mStdAloneMode(false),       
    mStopSignal(NULL),
    mIter(-1),
    mOptimLogFile("")
{
    this->setObjectName(aName);
    GS::IDCount=0 ;
    GS::iVerbose=0 ;
    CairnUtils::resetInfoParam() ;
    CairnUtils::resetMissingParam();

      
    mMilpData = new MilpData (this, "MilpData", aPdt, aNpdtPast, aNpdtFuture, aTimeshift, aIHMFuturSize, aGlobalTimeStepFile, aGlobalTypicalPeriodsFile) ;
    setStudyName(aStudyFile, aResultFile);
    setScenarioName(aScenarioName);

    // Create master optim problem with MilpData, void TecEcoEnv and list of objects.
    TecEcoEnv aTecEcoEnv ;
    QMap<QString,QString> aMilpComponents ;
    aMilpComponents["id"]=QString("SYSTEM") ;

    mProblem = new OptimProblem(this, "OptimProblem", mMilpData, aTecEcoEnv, aMilpComponents) ;
    mProblem->setStdAloneMode(mStdAloneMode) ;

    mTimeSeriesManager = new TimeSeriesManager(*mMilpData, "pegase");
}

// StdAloneMode : Time Data set from Cairn SimulationControl object from Json file.
CairnCore::CairnCore(QObject *aParent,
                       const QString & aName,
                       const QString& aStudyName,
                       const QString& aResultFile,
                       const QString &aGlobalTimeStepFile,
                       const QString &aGlobalTypicalPeriodsFile,
                       const QString& aScenarioName)
    : QObject(aParent),
    mStdAloneMode(true),
    mStopSignal(NULL),
    mIter(-1),
    mOptimLogFile("")
{
    GS::IDCount=0 ;
    GS::iVerbose=0 ;
    CairnUtils::resetInfoParam() ;
    CairnUtils::resetMissingParam();
    QString exeDir = qEnvironmentVariable("CAIRN_BIN", QDir::currentPath());

    UnitsConverter::UnitsConverter(exeDir.toStdString() + (std::string)"/../resources/DefUnits.json");

    mMilpData = new MilpData(this, "MilpData", aGlobalTimeStepFile, aGlobalTypicalPeriodsFile);

    setStudyName(aStudyName, aResultFile);
    setScenarioName(aScenarioName);

    TecEcoEnv aTecEcoEnv ;
    QMap<QString,QString> aMilpComponents ;
    aMilpComponents["id"]=QString("SYSTEM") ;

    mProblem = new OptimProblem(this, "OptimProblem", mMilpData, aTecEcoEnv, aMilpComponents) ;
    mProblem->setStdAloneMode(mStdAloneMode) ;

    mTimeSeriesManager = new TimeSeriesManager(*mMilpData);
}

CairnCore::~CairnCore()
{
    if (mMilpData) delete mMilpData;
    if (mProblem) delete mProblem;
    if (mTimeSeriesManager) delete mTimeSeriesManager;
}

void CairnCore::setStudyName(const QString& aStudyName, const QString& aResultFile)
{
    // StudyName = <ProjectDir> / <StudyName> .json
    if (aStudyName == "") return;
    QString vStudyName = aStudyName;
    if (vStudyName.contains(".json")) vStudyName = vStudyName.replace(".json", "");
    this->setObjectName(vStudyName);
   
    if (mMilpData->getVariableTimeStepsFile() == QString("")) setTimeStepFile(vStudyName + "_ListeOfTimeSteps.csv");
    if (mMilpData->getTypicalPeriodsFile() == QString("")) setTypicalPeriodsFile(vStudyName + "_ListOfTypicalPeriods.csv");

    mStudy.setResultFile(aResultFile.toStdString());
    mStudy.setStudyName(aStudyName.toStdString());   
}
void CairnCore::setResultFile(const QString& aResultFile) {
    mStudy.setResultFile(aResultFile.toStdString());
}
void CairnCore::setResultsDir(const QString& aResultsDir)
{
    mStudy.setResultsDir(aResultsDir.toStdString());    
}
void CairnCore::setScenarioName(const QString& aScenarioName) {
    mStudy.setScenarioName(aScenarioName.toStdString());
}
void CairnCore::setTimeStepFile(const QString& aTimeStepFile) {
    mMilpData->setVariableTimeStepsFile(aTimeStepFile);
}
void CairnCore::setTypicalPeriodsFile(const QString& aTypicalPeriodsFile) {
    mMilpData->setTypicalPeriodsFile(aTypicalPeriodsFile);
}

void CairnCore::doInit(bool aLoad)
{
    qInfo() << "...DoInit of CairnCore Module " ;

    qInfo() << "  " ;
    qInfo() << " ############################################################################ " ;
    qInfo() << "  " ;
    qInfo() << " You are using Cairn release " << Cairn_Release << " based on " ;
    qInfo() << " CairnCore    Library " << OptimProblem::getRelease() ;
    qInfo() << " MIPModeler     Engine " << QString::fromStdString(MIPModeler_Release) ;
    qInfo() << "  " ;
    qInfo() << " ############################################################################ " ;
    qInfo() << "  " ;

    try {
        mProblem->doInit(mStudy, aLoad);       
    }
    catch (Cairn_Exception & cairn_err) {
        qCritical() << "Error in doInit of Cairn!";
        throw cairn_err;
    }

    if (npdtTimeshift() > npdtPast())
    {
        Cairn_Exception error("Error in time parameter settings past horizon " + QString::number(npdtPast()) + " should be >= time shifting " + QString::number(npdtTimeshift()) + " \nCannot start the simulation!", -1);
        throw error;
    }

    // ensure reset in case of several doInit from GUI
    mMilpData->setStartingAbsoluteTimeStep(0);

    mProblem->setDefaultsResults();
}

void CairnCore::setStopSignal(int* stopSignal){
    mStopSignal=stopSignal;
}

void CairnCore::importTS(const QStringList& aTSfileList, const int& iShift = 0)
{
    mTimeSeriesManager->importTS(aTSfileList, mProblem->ListSubscribedVariables(), iShift, mProblem->getSimulationControl()->isCheckTimeSeriesUnits());
}

OrCheckUnits CairnCore::CheckUnits(const QString& a_FileUnit, const QString& a_Units, bool a_Check)
{
    return mTimeSeriesManager->CheckUnits(a_FileUnit, a_Units, a_Check);
}

int CairnCore::exportTS(const QString &aTSfile, int iter, bool rh, QString encoding)
{
    // #######################################
    // ############## EXPORT #################
    // #######################################
    QString TSfileMod;

    TSfileMod = aTSfile;
    QFile FileOut(TSfileMod);
    QIODevice::OpenMode openMode = QIODevice::WriteOnly;
    if (iter>0){
        openMode = QIODevice::Append;
    }
    if (!CairnUtils::openFileForWriting(FileOut, openMode)) {
        qWarning () << "OptimProblem: couldn't open result file for writing : " << TSfileMod;
        return 1;
    }
    qCInfo(log) << " - Export RollingHorizon result timeseries "<<TSfileMod;
    QTextStream io(&FileOut) ;
    io.setCodec(QTextCodec::codecForName(encoding.toUtf8()));
    QMap<QString, ZEVariables*> ListPublishedVariables = mProblem->ListPublishedVariables() ;
    if(iter==0){
        QMapIterator<QString, ZEVariables*> iPublishedVariable(ListPublishedVariables) ;
        io << "Time" << ";" ;
        while (iPublishedVariable.hasNext())
        {
            iPublishedVariable.next() ;
            ZEVariables* var = iPublishedVariable.value() ;
            QString zeVarName = var->Name();
            io << zeVarName << ";" ;
        }
        io << Qt::endl ;
    }
    int ts; //nombre de pdt à écrire : si rolling horizon, que ce qui ne sera pas recalculé (donc timeshift), sinon tout le futur size.
    if (rh)
        ts = mMilpData->timeshift();
    else
        ts = mMilpData->npdt();
    for ( size_t j = 0; j < ts; j++)
    {
        io << (j+1+ts*iter)*mMilpData->pdt() << ";" ;
        QMapIterator<QString, ZEVariables*> iPublishedVariable2(ListPublishedVariables) ;
        while (iPublishedVariable2.hasNext())
        {
            iPublishedVariable2.next() ;
            ZEVariables* var = iPublishedVariable2.value() ;
            QString zeVarName = var->Name();
            if (var->ptrVariable()->size() > 0)
            {
                io << var->ptrVariable()->at(j + npdtPast()) << ";" ;
            }
        }
        io << Qt::endl ;
    }

    FileOut.close() ;
    return 0;
}

int CairnCore::exportTS(const QString& aTSfile, std::map<std::string, std::vector<double>>& resultats, QString encoding)
{
    QFile FileOut(aTSfile);
    if (!CairnUtils::openFileForWriting(FileOut)) {
        qWarning() << "OptimProblem Couldn't open result file for writing : " << aTSfile;
        return 1;
    }

    QTextStream io(&FileOut);
    io.setCodec(QTextCodec::codecForName(encoding.toUtf8()));
    io << "Time" << ";";
    for (auto& [key, value] : resultats) {
        io << QString(key.c_str()) << ";";
    }    
    io << Qt::endl;

    for (size_t j = mMilpData->npdtPast(); j < mMilpData->npdtTot(); j++)
    {
        io << j * (mMilpData->pdt()) << ";";
        for (auto& [key, value] : resultats) {
            io << value[j] << ";";
        }
        io << Qt::endl;
    }

    FileOut.close();
    return 0;
}


int CairnCore::doStep(const QString encoding, const QMap<QString, bool> paramMap)
{
    mIter += 1;

    bool isRollingHorizon = false;
    if (nbcycle() > 1 || !mStdAloneMode) isRollingHorizon = true; //RollingHorizon (in case of Pegase "!mStdAloneMode" always set)

    if (!mStdAloneMode) mTimeSeriesManager->importTS(mProblem->ListSubscribedVariables());

    qCInfo(log) << "...DoStep CairnCore " << Qt::endl << Qt::flush  ;

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
        Cairn_Exception cairn_error("Fatal Error in prepareOptim Optim Problem", -1);
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
            qCritical() << "Fatal Error in building Optim Problem";
            throw mProblem->getException();
        }
    }
    catch (std::exception std_exp) {
        Cairn_Exception cairn_error(std_exp.what(), -1);
        throw cairn_error;
    }
    catch (...) {
        if (mProblem->getException().error() != 0)
            throw mProblem->getException();//TODO: why checked twice? and unify the exception handling 
        else {
            Cairn_Exception cairn_error("Fatal Error in building Optim Problem", -1);
            throw cairn_error;
        }
    }

    /** fill in objective function and build matrix */
    mipModel.setObjective(objectiveExpression);
    objectiveExpression.close() ;

    if ( mProblem->getOptimDirection() == QString("Maximize") ) {
         mipModel.setObjectiveDirection(MIPModeler::MIP_MAXIMIZE);
    }
    else  {
         mipModel.setObjectiveDirection(MIPModeler::MIP_MINIMIZE);
    }

    /** Solve MILP problem */
    try {
        mipModel.buildProblem();
    }
    catch (QString message) {
        Cairn_Exception error(message, -1);
        throw& error;
    }
    qCInfo(log) << "Iteration: " << mIter;

    int cycle = 0;
    if(isRollingHorizon) cycle = mIter + 1;
    if (mOptimLogFile == "") mOptimLogFile = QString(mStudy.getScenarioFile("_optim.log", 0, false).c_str());
    mProblem->solveProblem(mOptimLogFile, cycle, paramMap, ExportResultsEveryCycle()); //Need a class structure to support non-bool parameters inside paramMap

    QString status = mProblem->getOptimisationStatus() ;
    std::cout << "Flush pour avoir les sorties avec optimisation status " << std::flush;
    qCInfo(log) << "Optimization status optim : " << status ;
    int istat = mProblem->getInterpretedOptimStatus();
    
    /** Get Solution */
    int nbSol = mProblem->getNumberOfSolutions();
    qCInfo(log) << "Total number of solutions:" << nbSol;

    //Solver Running time
    mSolverRunningTimeAllCycles.push_back(mProblem->getSolverRunningTime());

    bool isCheckConflicts = mProblem->getIsCheckConflicts();
    if (isCheckConflicts) {
        return istat;
    }
    bool isExportResults = mProblem->getSimulationControl()->isExportResults();
    if (istat == 2) {
        qCWarning(log) << "CairnCore default solution due to no solution with status =" << status;
        mProblem->setDefaultsResults();
        if (isExportResults)        
            istat = exportResults(0, isRollingHorizon, istat, encoding);
    }
    else {
        for (int i = nbSol - 1;i >= 0;i--) {            
            mProblem->readSolution(i);            
            if (istat == 0)
            {
                qCInfo(log) << "CairnCore solution optimale " << status << ", solution: " << i;                
            }
            else {
                qCWarning(log) << "CairnCore non optimal solution obtained by status =" << status << ", solution: " << i;                
            }
            if (i > 0 && isExportResults) {
                std::map<std::string, std::vector<double>> vResults;
                mProblem->writeSolution(i, vResults);
                exportTS(QString(mStudy.getScenarioFile(".csv", i).c_str()), vResults, encoding);
            }
            /** Export results */
            if (isExportResults)
                istat = exportResults(i, isRollingHorizon, istat, encoding);
        }        
    }

    mProblem->cleanExpressions();
    return istat ;
}

void CairnCore::exportTotalTimeResolutionAllCycles(const QString& aFileName)
{
    QFile FileOut(aFileName);
    if (!CairnUtils::openFileForWriting(FileOut))
    {
        Cairn_Exception cairn_error("CairnCore: Couldn't open file solverRunningTime.csv for writing.", -1);
        throw cairn_error;
    }

    QTextStream out(&FileOut);

    out << "sep=;\n";

    QString header = "; Solver Running Time ; Cumulative Time";

    out << header << "\n";

    for (int i = 0; i < mSolverRunningTimeAllCycles.size(); i++)
    {
        QString iCycleLine = "Cycle " + QString::number(i + 1);
        double cumulativeTime = 0.0;
        for (int j = 0; j < i+1; j++)
            cumulativeTime += mSolverRunningTimeAllCycles[j];
        iCycleLine += ";" + QString::number(mSolverRunningTimeAllCycles[i]) + ";" + QString::number(cumulativeTime) + "\n";
        out << iCycleLine;
    }
}


//------------------------------------------------------------------------------
int CairnCore::exportResults(const int& aNsol, const bool& isRollingHorizon, const int &istat, QString encoding)
{
    /** Export results */    
    mProblem->exportResults();

    if (istat >= 0) {
        //iter = 0, and rh = false to save all the npdt() points of this cycle
        exportTS(getResultsTimeSeriesFileName(aNsol));
        if (isRollingHorizon && ExportResultsEveryCycle()) {
            QString vName = "_Results_RH_" + QString::number(mIter+1) + ".csv";
            exportTS(QString(mStudy.getScenarioFile(vName.toStdString(), aNsol).c_str()), 0, false, encoding);
        }
    }

    /** Save rollinghorizon results. Standard format: it takes only timeShift points from each iteration if nbcycle() > 1 or Pegase */ 
    if (isRollingHorizon || !mStdAloneMode) {
        exportTS(QString(mStudy.getScenarioFile("_rollinghorizon.csv", aNsol).c_str()), mIter, (isRollingHorizon || !mStdAloneMode), encoding);
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

    qCInfo(log) << " - Export results done ";

    return istat;
}

void CairnCore::exportAnalysis(const int &aNsol, const bool& isRollingHorizon, QString encoding)
{
    int ierr = 0;
    if (mProblem->getTecEcoEnv() != nullptr)
    {
        if (mProblem->getTecEcoEnv()->Range() == "HIST" || mProblem->getTecEcoEnv()->Range() == "HISTandPLAN")
        {
            try {
                mProblem->exportAllTecEcoEnvAnalysis(QString(mStudy.getScenarioFile("_HIST.csv", aNsol).c_str()), "HIST", encoding, false);
                if (isRollingHorizon && ExportResultsEveryCycle()) {
                    QString vName = "_HIST_RH_" + QString::number(mIter + 1) + ".csv";
                    mProblem->exportAllTecEcoEnvAnalysis(QString(mStudy.getScenarioFile(vName.toStdString(), aNsol).c_str()), "HIST", encoding, isRollingHorizon);
                }
            }
            catch (Cairn_Exception& cairn_err) {
                Cairn_Exception cairn_error("export analysis failed!!", -1);
                throw cairn_error;
            }
        }

        if (mProblem->getTecEcoEnv()->Range() == "PLAN" || mProblem->getTecEcoEnv()->Range() == "HISTandPLAN")
        {
            try {
                mProblem->exportAllTecEcoEnvAnalysis(getGlobalResultsFileName(aNsol), "PLAN", encoding, false, aNsol); // always isRollingHorizon=false // main _PLAN.csv
                if (mProblem->getSimulationControl()->isExportParameters()) mProblem->exportParameters(QString(mStudy.getScenarioFile("_Parameters.csv", aNsol, false).c_str()), encoding);
                //Env Impacts coeff and results - special files
                mProblem->exportEnvImpactParameters(QString(mStudy.getScenarioFile("_EnvImpactParameters.csv", aNsol, false).c_str()), encoding);
                mProblem->exportPortEnvImpactParameters(QString(mStudy.getScenarioFile("_PortEnvImpactParameters.csv", aNsol, false).c_str()), encoding);
                mProblem->exportEnvImpactMassIndicators(QString(mStudy.getScenarioFile("_EnvImpactMass.csv", aNsol).c_str()), encoding);
                //
                if (isRollingHorizon && ExportResultsEveryCycle()) {
                    QString vName = "_PLAN_RH_" + QString::number(mIter + 1) + ".csv";
                    mProblem->exportAllTecEcoEnvAnalysis(QString(mStudy.getScenarioFile(vName.toStdString(), aNsol).c_str()), "PLAN", encoding, isRollingHorizon, aNsol);
                }
                if (isRollingHorizon) mProblem->exportOptimaSizeAllCycles(QString(mStudy.getScenarioFile("_optimalSize.csv", aNsol, false).c_str()), mIter + 1);
                exportTotalTimeResolutionAllCycles(QString(mStudy.getScenarioFile("_solverRunningTime.csv", aNsol, false).c_str()));
            }
            catch (Cairn_Exception& cairn_err) {
                Cairn_Exception cairn_error("export analysis failed!!", -1);
                throw cairn_error;
            }
        }
    }
}

//------------------------------------------------------------------------------
int CairnCore::doTerminate()
{   
    qInfo() << "...doTerminate CairnCore"<< Qt::endl << Qt::flush  ;

    if(mProblem && mProblem->getSimulationControl()->isExportJson())
    {
        mProblem->SaveFullArchitecture();
    }

    //Delete _rollingHorizon.csv if non-RollingHorizon because it is the same as _Results.csv in this case (case Pegase)
    if(mIter == 0 && !mStdAloneMode){
        for (int aNsol = 0; aNsol < mProblem->getNumberOfSolutions(); aNsol++) {
            QString filename = QString(mStudy.getScenarioFile("_rollinghorizon.csv", aNsol).c_str());
            QFile rhFile(filename);
            if (rhFile.exists()) {
                rhFile.remove();
            }
        }
    }
    
    // make sure no attempt to read settings later on as local QSettings will have been destroyed 
    mMilpData->setSettings(nullptr);

    if(mProblem) delete mProblem;
	mProblem = nullptr;
    if(mMilpData) delete mMilpData;
	mMilpData = nullptr;

    return 0 ;
}



