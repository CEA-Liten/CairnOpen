#define NOMINMAX
#include "Solver.h"
#include "MilpComponent.h"
#include "GlobalSettings.h"
#include "CairnDefine.h"

#include <sys/types.h>
#include <sys/timeb.h>

#include <algorithm>
#include <chrono>
#include <ctime>
#include <iostream>


template<typename T1, typename T2>
using mul = std::ratio_multiply<T1, T2>;
using namespace std::chrono_literals;

static inline double CoinCpuTime()
{
    double cpu_temp;
    unsigned int ticksnow;        /* clock_t is same as int */
    ticksnow = (unsigned int)clock();
    cpu_temp = (double)((double)ticksnow / CLOCKS_PER_SEC);
    return cpu_temp;
}

using namespace GS ;

//Default Solver (used for API)
Solver::Solver(const QString& aSolverName, const QMap<QString, QString>& aComponent):
    mException(Cairn_Exception()),
    mSolvers(MIPSolverFactory()),
    mExternalModeler(nullptr),
    mModel(nullptr),
    mGUIData(nullptr),
    mCompoInputParam(nullptr),
    mCompoInputSettings(nullptr),
    mTerminateSignal(nullptr),
    mSolverRunningTime(0.),
    mSolverName(aSolverName)
{
    this->setObjectName(mName);
    doInit(aComponent);
}

Solver::~Solver()
{
    if (mGUIData) delete mGUIData;
    if (mCompoInputParam) delete mCompoInputParam;
    if (mCompoInputSettings) delete mCompoInputSettings;
}

void Solver::doInit(const QMap<QString, QString>& aComponent) 
{
    declareCompoInputParam();
    setCompoInputParam(aComponent);
    InitSolverParam();

    if (mXpos == 0) mXpos = 75;
    if (mYpos == 0) mYpos = 10;

    if (mGUIData) delete mGUIData;
    mGUIData = new GUIData(this, "Solver");
    mGUIData->doInit("Solver", "Solver", "Solver", mXpos, mYpos);
    mGUIData->setGuiNodeName(mName);
    mGUIData->setObjectName(mSolverName);
}

void Solver::declareCompoInputParam()
{
    mCompoInputParam = new InputParam(this, "CompoInputParam" + mName);
    //QString
    mCompoInputParam->addParameter("type", &mComponentCairnType, "Solver", true, true, "Component type", "", { "DONOTSHOW" });
    mCompoInputParam->addParameter("id", &mName, "Solver", true, true, "Component name", "", { "DONOTSHOW" });
    mCompoInputParam->addParameter("Model", &mModelType, "MIPModeler", true, true, "Model used");
    mCompoInputParam->addParameter("Solver", &mSolverName, CAIRN_DEFAULTSOLVER, true, true, "Solver name: Cbc or Cplex or Highs ..");
    mCompoInputParam->addParameter("Category", &mProblemType, "MIP", true, true, "Model type");
    mCompoInputParam->addParameter("Xpos", &mXpos, 75, false, true, "X position on planteditor", "", { "DONOTSHOW" });
    mCompoInputParam->addParameter("Ypos", &mYpos, 10, false, true, "Y position on planteditor", "", { "DONOTSHOW" });
    mCompoInputParam->addParameter("WriteLp", &mWriteLp, "YES", false, true, "Writing of Optimization problem in LP format is YES - default NO");
    mCompoInputParam->addParameter("ReadParamFile", &mReadParamFile, "NO", false, true, "Read a study_cplexParam.prm file to parameter cplex solving");
    mCompoInputParam->addParameter("WriteMipStart", &mWriteMipStart, "NO", false, true, "Write mst cplex file");
    mCompoInputParam->addParameter("FileMipStart", &mFileMipStart, "", false, true, "Give a .mst file to start for a full solution");

    mCompoInputSettings = new InputParam(this, "CompoInputSettings" + mName);
    //int
    mCompoInputSettings->addParameter("Threads", &mThreads, 8, false, true, "Number of threads for solving step", "-");
    mCompoInputSettings->addParameter("TreeMemoryLimit", &mTreeMemoryLimit, 50000, false, true, "Memory limit of the branch-and-cut tree of CPLEX solver (in MB). If this limit is exceeded, CPLEX terminates the optimization. Be careful to have enough available RAM memory before increasing the default value!", "MB");
    mCompoInputSettings->addParameter("NbSolToKeep", &mNbSolToKeep, 1, false, true, "max number of solutions", "");
    //double
    mCompoInputSettings->addParameter("TimeLimit", &mTimeLimit, 1e4, false, true, "Max Cpu time for solving process", "s");
    mCompoInputSettings->addParameter("Gap", &mGap, 1e-4, false, true, "Gap to optimal solution", "-");
}

bool Solver::setCompoInputParam(const QMap<QString, QString>& aComponent)
{
    bool vRet = true;

    if (aComponent.size() != 0) {
        int ierr1 = mCompoInputParam->readParameters(aComponent);
        int ierr2 = mCompoInputSettings->readParameters(aComponent);
        if (ierr1 < 0 || ierr2 < 0) { return false; }
    }

    //TODO: Ensure that the mandatory parameters exist in TNR .json files 
    //if (ierr1 < 0 || ierr2 < 0) {
    //    mException = Cairn_Exception("Error reading Parameters of Solver " + mName, -1);
    //    throw mException;
    //}

    return true;
}

void Solver::InitSolverParam() {
    //retrieves the list of possible solvers
    QStringList vSolverLoaded;
    mSolvers.getAllInfos(vSolverLoaded);

    //retrieves the list of possible modelers
    QStringList vModelerLoaded;
    mModelers.getModelersName(vModelerLoaded);

    // to be compatible with older projects
    if (mModelType == GS::MIPMODELER() || mModelType == "" || mModelType == "Cplex" || mModelType == "MIPModeler")
    {
        mModelType = GS::MIPMODELER();
        if (mSolverName == "") mSolverName = mName;

        // Does the solver exist among the list of possible solvers?
        if (!vSolverLoaded.contains(mSolverName)) {
            mException = Cairn_Exception("Error: solver asked " + mSolverName + ", possible solver names are " + vSolverLoaded.join(","), -1);
            throw mException;
        }
    }
    else if (vModelerLoaded.contains(mModelType)) {
        if (mModelType == GS::GAMS()) {
            if (mProblemType != "LP" &&
                mProblemType != "MIP" &&
                mProblemType != "NLP" &&
                mProblemType != "MINLP" &&
                mProblemType != "DNLP" &&
                mProblemType != "QCP" &&
                mProblemType != "MIQCP") {
                mException = Cairn_Exception("Error: possible problem types are \"LP\", \"MIP\", \"NLP\", \"MINLP\", \"DNLP\", \"QCP\" or \"MIQCP\".", -1);
                throw mException;
            }
        }
        mExternalModeler = mModelers.getModeler(mModelType);
    }
    else {
        mException = Cairn_Exception("Error: Modeler asked " + mModelType + ", possible modeler names are MIPModeler are " + vModelerLoaded.join(","), -1);
        throw mException;
    }
}

ModelerInterface* Solver::getExternalModeler() {    
    return mExternalModeler;
}

void Solver::SolveProblem(MIPModeler::MIPModel* aModel, const QString &location, const int cycle, const QMap<QString, bool> paramMap)
{
    mModel = aModel;

    if (mThreads == 0) mThreads = 8;
    mThreads = std::min(mThreads, (int)std::thread::hardware_concurrency());
    qInfo() << "Using " << mThreads << " / max logical Threads " << std::thread::hardware_concurrency();
   
    if (mModelType == GS::MIPMODELER()) {
        MIPSolverParams vParams;
        if (mSolverName == "Cplex")
        {            
            if (mTimeLimit > 0.) {
                vParams.addParam("TimeLimit", mTimeLimit);                
                qInfo() << "Setting solver properties TimeLimit" << mTimeLimit ;
            }
            if (mGap > 0.) {
                vParams.addParam("Gap", mGap);                
                qInfo() << "Setting solver properties Gap" << mGap ;
            }
            if (mThreads > 0 ) {
                vParams.addParam("Threads", mThreads);                
                qInfo() << "Setting solver properties Threads" << mThreads ;
            }
            vParams.addParam("TerminateSignal", mTerminateSignal);

            vParams.addParam("Location", location);
      
            if (mTreeMemoryLimit > 0) {
                vParams.addParam("TreeMemoryLimit", mTreeMemoryLimit);
                qInfo() << "Setting TreeMemoryLimit" << mTreeMemoryLimit;
            }
            if (mNbSolToKeep > 0) {
                vParams.addParam("NbSolToKeep", mNbSolToKeep);
                qInfo() << "maxnumberof solutions" << mNbSolToKeep;
            }            
            vParams.addParam("SolverPrint", 1);
                        
            if (mWriteLp == "YES") vParams.addParam("WriteLp", 1);
            vParams.addParam("WriteLpCycle", cycle);
            if(mReadParamFile=="YES") vParams.addParam("ReadParamFile", 1); 
            if (mWriteMipStart == "YES") vParams.addParam("WriteMipStart", 1); 

            if (mFileMipStart != "") {
                vParams.addParam("FileMipStart", mFileMipStart);       
            }     
            //Add parameters from GUI Debug Interface
            if (!paramMap.isEmpty()) {

                QMapIterator<QString, bool> paramItr(paramMap);
                while (paramItr.hasNext())
                {
                    paramItr.next();
                    vParams.addBoolParam(paramItr.key(), paramItr.value());
                }
            }         
        }
        else if (mSolverName == "Cbc")
        {
            if (mTimeLimit > 0.) {
                vParams.addParam("TimeLimit", mTimeLimit);
                qInfo() << "Setting solver properties TimeLimit" << mTimeLimit;
            }
            if (mGap > 0.) {
                vParams.addParam("Gap", mGap);
                qInfo() << "Setting solver properties Gap" << mGap;
            }
            if (mThreads > 0) {
                vParams.addParam("Threads", mThreads);
                qInfo() << "Setting solver properties Threads" << mThreads;
            }
            if (mWriteLp == "YES") vParams.addParam("WriteLp", 1);
			vParams.addParam("WriteLpCycle", cycle);
            vParams.addParam("SolverPrint", 1);            
        }
        else if (mSolverName == "Clp")
        {
            if (mWriteLp == "YES") vParams.addParam("WriteLp", 1);
			vParams.addParam("WriteLpCycle", cycle);
            vParams.addParam("SolverPrint", 1);            
        }
        else if (mSolverName == "Highs")
        {
            vParams.addParam("Location", location);
            vParams.addParam("SolverPrint", 1);
            if (mWriteLp == "YES") vParams.addParam("WriteLp", 1);            
        }

        qInfo() << "- Begin problem solving with " << mSolverName ;

        std::chrono::time_point<std::chrono::steady_clock> start, end;
        start = std::chrono::steady_clock::now();

        int ierr = mSolvers.solve(mSolverName, mModel, vParams, mSolverResults);

        end = std::chrono::steady_clock::now();
        std::chrono::duration<double> elapsed_seconds = end - start;
        //qInfo() << "Elapsed time" << elapsed_seconds.count();
        mSolverRunningTime = elapsed_seconds.count();

        if(ierr < 0)
            qCritical() << "An error has found while building the optimal problem: NAN value " << mSolverName;
        else 
            qInfo() << "- End problem solving with " << mSolverName;        
    }
    else if (mExternalModeler!=nullptr) {
        ModelerParams vParams;
        if (mTimeLimit > 0.) {
            vParams.addParam("TimeLimit", mTimeLimit);            
            qInfo() << "Setting solver properties TimeLimit" << mTimeLimit;
        }

        if (mGap > 0.) {
            vParams.addParam("Gap", mGap);            
            qInfo() << "Setting solver properties Gap" << mGap;
        }

        if (mThreads > 0) {
            vParams.addParam("Threads", (double)mThreads);            
            qInfo() << "Setting solver properties Threads" << mThreads;
        }
        vParams.addParam("SolverName", mSolverName.toStdString());
        vParams.addParam("ProblemType", mProblemType.toStdString());
        vParams.addParam("location", location.toStdString());
        
        qInfo() << "- Begin problem solving with ExternalModeler " << mSolverName;
        mExternalModeler->solve(vParams, mExternalResults);
        qInfo() << "- End problem solving with ExternalModeler " << mSolverName;

        //ToDo: copy .lp and _optim.log
    }
    else
        qCritical () << "Bad ModelName: " << mModelType;   
}

bool Solver::getIsCheckConflicts() {
    return mSolverResults.getIsCheckConflicts();
}

QString Solver::getOptimisationStatus()
{
   if (mModelType == GS::MIPMODELER ()) {
       if (mSolverName == "Cplex")
           qInfo() << " - See local file cplex_optim.log for optimization details - ";
       return QString::fromStdString(mSolverResults.getOptimisationStatus());

   }
   else if (mExternalModeler != nullptr) {
       qInfo() << " - See local file \".lst\" for optimization details - ";
       qInfo() << "ModelState: " << QString::fromStdString(mExternalResults.getModelStatus());
       qInfo() << "SolveState: " << QString::fromStdString(mExternalResults.getSolverStatus());
       return QString::fromStdString(mExternalResults.getModelStatus());
   }

    return "! NO OPTIMIZATION PERFORMED : NO MILP SOLVER FOUND !" ;
}

const double* Solver::getOptimalSolution(int aNsol, const std::string& varname)
{
    if (mModelType == GS::MIPMODELER ()) {
        return mSolverResults.getOptimalSolution(aNsol);
    }
    else if (mExternalModeler != nullptr) {
        return mExternalModeler->getOptValue(varname);
    }
    return nullptr;
}

int Solver::getNumberOfSolutions(){
    if (mModelType == GS::MIPMODELER ()) {
        return mSolverResults.getNumberOfSolutions();
     }
    return 1;
}

double Solver::getSolverRunningTime() { 
    return mSolverRunningTime; 
}

void Solver::jsonSaveGuiComponent(QJsonArray &componentsArray)
{
    QJsonArray optionArray ;
    QJsonArray paramArray ;

    mCompoInputSettings->jsonSaveGUIInputParam(paramArray);
    mCompoInputParam->jsonSaveGUIInputParam(optionArray);
    
    QJsonObject compoObject;
    mGUIData->jsonSaveGUILine(compoObject);
    compoObject.insert("optionListJson", optionArray) ;
    compoObject.insert("paramListJson", paramArray) ;

    componentsArray.push_back(compoObject) ;
}

void Solver::setStopSignal(int *stopSignal){
    mTerminateSignal = stopSignal;
}

std::map<QString, InputParam::ModelParam*> Solver::getParameters()
{
    std::map<QString, InputParam::ModelParam*> paramMap;

    paramMap.insert(getCompoInputParam()->getMapParams().begin(), getCompoInputParam()->getMapParams().end());
    paramMap.insert(getCompoInputSettings()->getMapParams().begin(), getCompoInputSettings()->getMapParams().end());

    return paramMap;
}

