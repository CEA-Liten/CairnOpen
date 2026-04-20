#define NOMINMAX
#include "Solver.h"
#include "MilpComponent.h"
#include "CairnDefine.h"
#include "CairnUtils.h"

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

const std::vector<std::string> Solver::mGAMSProblemTypes = {
    "LP", "MIP", "NLP", "MINLP", "DNLP", "QCP", "MIQCP"
};

const std::vector<std::string> Solver::mPossibleModelTypes = {
    "MIPModeler"
};

//Default Solver (used for API)
Solver::Solver(CairnObject* ap_Parent, const std::string& aName, const std::map<std::string, std::string>& aComponent):
    CairnObject(ap_Parent),
    mException(Cairn_Exception()),
    mSolvers(MIPSolverFactory()),
    mExternalModeler(nullptr),
    mModel(nullptr),
    mGUIData(nullptr),
    mCompoInputParam(nullptr),
    mCompoInputSettings(nullptr),
    mTerminateSignal(nullptr),
    mSolverRunningTime(0.)
{
    this->setObjectName(aName);
    this->setObjectType("Solver");
    doInit(aComponent);
}

Solver::~Solver()
{
    if (mGUIData) delete mGUIData;
    if (mCompoInputParam) delete mCompoInputParam;
    if (mCompoInputSettings) delete mCompoInputSettings;
}

void Solver::doInit(const std::map<std::string, std::string>& aComponent) 
{
    if (mGUIData) delete mGUIData;
    mGUIData = new GUIData(this);
    mGUIData->doInit("Solver", "Solver", "Solver",
        { {"Xpos", CairnUtils::getParam(aComponent,"Xpos")}, {"Ypos", CairnUtils::getParam(aComponent,"Ypos")} });

    declareCompoInputParam();
    setCompoInputParam(aComponent);
    InitSolverParam();

    mGUIData->setObjectName(mSolverName);
}

void Solver::declareCompoInputParam()
{
    std::string vDefaultSolver = CAIRN_DEFAULTSOLVER;
    // test if default solver exists
    t_list vSolverLoaded;
    mSolvers.getAllInfos(vSolverLoaded);
    if (!CairnUtils::contains(vSolverLoaded, vDefaultSolver)) {
        // if not exist, take the first solver
        if (vSolverLoaded.size()) vDefaultSolver = vSolverLoaded[0];
    }

    mCompoInputParam = new InputParam(this, "CompoInputParam" + Name());
    //std::string
    mCompoInputParam->addParameter("Model", &mModelType, "MIPModeler", true, true, "Model used: MIPModeler, GAMS, etc.");
    mCompoInputParam->addParameter("Solver", &mSolverName, vDefaultSolver, true, true, "Solver name: Cbc, Cplex, Highs, etc.");
    mCompoInputParam->addParameter("Category", &mProblemType, "MIP", true, true, "Problem type: MIP, LP, etc. Note, this parameter is not important for Cplex, and Highs");
    mCompoInputParam->addParameter("WriteLp", &mWriteLp, "YES", false, true, "Writing of Optimization problem in LP format is YES - default NO");
    mCompoInputParam->addParameter("ReadParamFile", &mReadParamFile, "NO", false, true, "Read a study_cplexParam.prm file to parameter cplex solving");
    mCompoInputParam->addParameter("WriteMipStart", &mWriteMipStart, "NO", false, true, "Write mst cplex file");
    mCompoInputParam->addParameter("FileMipStart", &mFileMipStart, "", false, true, "Give a .mst file to start for a full solution");

    mCompoInputSettings = new InputParam(this, "CompoInputSettings" + Name());
    //int
    mCompoInputSettings->addParameter("Threads", &mThreads, 8, false, true, "Number of threads for solving step", "-");
    mCompoInputSettings->addParameter("TreeMemoryLimit", &mTreeMemoryLimit, 50000, false, true, "Memory limit of the branch-and-cut tree of CPLEX solver (in MB). If this limit is exceeded then CPLEX terminates the optimization. Be careful to have enough available RAM memory before increasing the default value!", "MB");
    mCompoInputSettings->addParameter("NbSolToKeep", &mNbSolToKeep, 1, false, true, "max number of solutions", "");
    //double
    mCompoInputSettings->addParameter("TimeLimit", &mTimeLimit, 1e4, false, true, "Max Cpu time for solving process", "s");
    mCompoInputSettings->addParameter("Gap", &mGap, 1e-4, false, true, "Gap to optimal solution", "-");
}

bool Solver::setCompoInputParam(const std::map<std::string, std::string>& aComponent)
{
    bool vRet = true;

    if (aComponent.size() != 0) {
        int ierr1 = mCompoInputParam->readParameters(aComponent);
        int ierr2 = mCompoInputSettings->readParameters(aComponent);
        if (ierr1 < 0 || ierr2 < 0) { return false; }
    }

    //TODO: Ensure that the mandatory parameters exist in TNR .json files 
    //if (ierr1 < 0 || ierr2 < 0) {
    //    mException = Cairn_Exception("Error reading Parameters of Solver " + Name(), -1);
    //    throw mException;
    //}

    return true;
}

void Solver::InitSolverParam() {
    //retrieves the list of possible solvers
    t_list vSolverLoaded;
    mSolvers.getAllInfos(vSolverLoaded);

    //retrieves the list of possible modelers
    t_list vModelerLoaded;
    mModelers.getModelersName(vModelerLoaded);

    // to be compatible with older projects
    if (mModelType == GS::MIPMODELER() || mModelType == "" || mModelType == "Cplex" || mModelType == "MIPModeler")
    {
        mModelType = GS::MIPMODELER();
        if (mSolverName == "") mSolverName = Name(); // old studies 

        // Does the solver exist among the list of possible solvers?
        if (!CairnUtils::contains(vSolverLoaded, mSolverName)) {
            mException = Cairn_Exception("Error: solver asked " + mSolverName + ", possible solver names are " + std::string(CairnUtils::joinStrings(vSolverLoaded).c_str()), -1);            
        }
    }
    else if (CairnUtils::contains(vModelerLoaded, mModelType))
    {
        if (mModelType == GS::GAMS() && !CairnUtils::contains(mGAMSProblemTypes, mProblemType))
        {
            const std::string validTypes = CairnUtils::joinStrings(mGAMSProblemTypes, ", ");
            mException = Cairn_Exception(
                "Error: possible problem types are: " + validTypes, -1);
        }
        mExternalModeler = mModelers.getModeler(mModelType);
    }
    else {
        mException = Cairn_Exception("Error: Modeler asked " + mModelType + ", possible modeler names are MIPModeler are " + std::string(CairnUtils::joinStrings(vModelerLoaded).c_str()), -1);
    }
}

ModelerInterface* Solver::getExternalModeler() {    
    return mExternalModeler;
}

void Solver::SolveProblem(MIPModeler::MIPModel* aModel, const std::string &location, const int cycle, const std::map<std::string, bool> paramMap)
{
    mModel = aModel;

    if (mThreads == 0) mThreads = 8;
    mThreads = std::min(mThreads, (int)std::thread::hardware_concurrency());
    cInfo() << "Using " << mThreads << " / max logical Threads " << std::thread::hardware_concurrency();
   
    if (mModelType == GS::MIPMODELER()) {
        MIPSolverParams vParams;
        if (mSolverName == "Cplex")
        {            
            if (mTimeLimit > 0.) {
                vParams.addParam("TimeLimit", mTimeLimit);                
                cInfo() << "Setting solver properties TimeLimit" << mTimeLimit ;
            }
            if (mGap > 0.) {
                vParams.addParam("Gap", mGap);                
                cInfo() << "Setting solver properties Gap" << mGap ;
            }
            if (mThreads > 0 ) {
                vParams.addParam("Threads", mThreads);                
                cInfo() << "Setting solver properties Threads" << mThreads ;
            }
            vParams.addParam("TerminateSignal", mTerminateSignal);

            vParams.addParam("Location", location);
      
            if (mTreeMemoryLimit > 0) {
                vParams.addParam("TreeMemoryLimit", mTreeMemoryLimit);
                cInfo() << "Setting TreeMemoryLimit" << mTreeMemoryLimit;
            }
            if (mNbSolToKeep > 0) {
                vParams.addParam("NbSolToKeep", mNbSolToKeep);
                cInfo() << "maxnumberof solutions" << mNbSolToKeep;
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
            if (paramMap.size()) {
                for (auto &[key , value] : paramMap) {                
                    vParams.addBoolParam(key, value);
                }
            }         
        }
        else if (mSolverName == "Cbc")
        {
            if (mTimeLimit > 0.) {
                vParams.addParam("TimeLimit", mTimeLimit);
                cInfo() << "Setting solver properties TimeLimit" << mTimeLimit;
            }
            if (mGap > 0.) {
                vParams.addParam("Gap", mGap);
                cInfo() << "Setting solver properties Gap" << mGap;
            }
            if (mThreads > 0) {
                vParams.addParam("Threads", mThreads);
                cInfo() << "Setting solver properties Threads" << mThreads;
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
            vParams.addParam("TerminateSignal", mTerminateSignal);
        }

        cInfo() << "- Begin problem solving with " << mSolverName;

        std::chrono::time_point<std::chrono::steady_clock> start, end;
        start = std::chrono::steady_clock::now();

        int ierr = mSolvers.solve(mSolverName, mModel, vParams, mSolverResults);

        end = std::chrono::steady_clock::now();
        std::chrono::duration<double> elapsed_seconds = end - start;
        //cInfo() << "Elapsed time" << elapsed_seconds.count();
        mSolverRunningTime = elapsed_seconds.count();

        if(ierr < 0)
            cCritical() << "An error has found while building the optimal problem: NAN value " << mSolverName;
        else 
            cInfo() << "- End problem solving with " << mSolverName;
    }
    else if (mExternalModeler!=nullptr) {
        ModelerParams vParams;
        if (mTimeLimit > 0.) {
            vParams.addParam("TimeLimit", mTimeLimit);            
            cInfo() << "Setting solver properties TimeLimit" << mTimeLimit;
        }

        if (mGap > 0.) {
            vParams.addParam("Gap", mGap);            
            cInfo() << "Setting solver properties Gap" << mGap;
        }

        if (mThreads > 0) {
            vParams.addParam("Threads", (double)mThreads);            
            cInfo() << "Setting solver properties Threads" << mThreads;
        }
        vParams.addParam("SolverName", mSolverName);
        vParams.addParam("ProblemType", mProblemType);
        vParams.addParam("location", location);
        
        cInfo() << "- Begin problem solving with ExternalModeler " << mSolverName;
        mExternalModeler->solve(vParams, mExternalResults);
        cInfo() << "- End problem solving with ExternalModeler " << mSolverName;

        //ToDo: copy .lp and _optim.log
    }
    else
        cCritical () << "Bad ModelName: " << mModelType;
}

bool Solver::getIsCheckConflicts() {
    return mSolverResults.getIsCheckConflicts();
}

std::string Solver::getOptimisationStatus()
{
   if (mModelType == GS::MIPMODELER ()) {
       if (mSolverName == "Cplex")
           cInfo() << " - See local file cplex_optim.log for optimization details - ";
       return (mSolverResults.getOptimisationStatus());

   }
   else if (mExternalModeler != nullptr) {
       cInfo() << " - See local file \".lst\" for optimization details - ";
       cInfo() << "ModelState: " << mExternalResults.getModelStatus();
       cInfo() << "SolveState: " << mExternalResults.getSolverStatus();
       return (mExternalResults.getModelStatus());
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

void Solver::jsonSaveGuiComponent(ojson &componentsArray)
{
    ojson compoObject;    
    mGUIData->jsonSaveGUILine(compoObject);
   
    compoObject["paramListJson"] = ojson::array();
    compoObject["optionListJson"] = ojson::array();
  
    mCompoInputSettings->jsonSaveGUIInputParam(compoObject["paramListJson"]);
    mCompoInputParam->jsonSaveGUIInputParam(compoObject["optionListJson"]);
        
    componentsArray.push_back(compoObject) ;
}

void Solver::setStopSignal(int *stopSignal){
    mTerminateSignal = stopSignal;
}

std::map<std::string, ModelParam*> Solver::getParameters()
{
    std::map<std::string, ModelParam*> paramMap;

    paramMap.insert(getCompoInputParam()->getMapParams().begin(), getCompoInputParam()->getMapParams().end());
    paramMap.insert(getCompoInputSettings()->getMapParams().begin(), getCompoInputSettings()->getMapParams().end());

    return paramMap;
}

std::vector<InputParam*> Solver::get_InputParams()
{
    std::vector<InputParam*> result;
    result.reserve(3);   // avoid reallocations

    // Always available
    result.push_back(getCompoInputParam());
    result.push_back(getCompoInputSettings());

    // Add GUI parameters if available
    if (auto* gui = getGUIData()) {
        result.push_back(gui->getGuiInputParam());
    }

    return result;
}

std::vector<InputParam*> Solver::get_ParamInputParams()
{
    std::vector<InputParam*> result;
    result.push_back(getCompoInputSettings());
    return result;
}

std::vector<InputParam*> Solver::get_OptionInputParams()
{
    std::vector<InputParam*> result;
    result.push_back(getCompoInputParam());
    return result;
}
