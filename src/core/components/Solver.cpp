#define NOMINMAX
#include "Solver.h"
#include "MilpComponent.h"
#include "CairnDefine.h"
#include "CairnUtils.h"
#include "Constants.h"

#include <sys/types.h>
#include <sys/timeb.h>

#include <algorithm>
#include <ctime>
#include <iostream>

using namespace CairnConstants;

template<typename T1, typename T2>
using mul = std::ratio_multiply<T1, T2>;

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
Solver::Solver(CairnObject* ap_Parent, const std::string& aName, const t_mapParamData& aComponent):
    CairnObject(ap_Parent, aName),
    mSolvers(MIPSolverFactory()),
    mExternalModeler(nullptr),
    mModel(nullptr),
    mGUIData(nullptr),
    mCompoInputParam(nullptr),
    mCompoInputSettings(nullptr),
    mTerminateSignal(nullptr)
{
    this->setObjectType("Solver");
    doInit(aComponent);
}

Solver::~Solver()
{
    if (mGUIData) delete mGUIData;
    if (mCompoInputParam) delete mCompoInputParam;
    if (mCompoInputSettings) delete mCompoInputSettings;
}

void Solver::doInit(const t_mapParamData& aComponent)
{
    CAIRN_LOG_SCOPE(Name());

    delete mGUIData;

    mGUIData = new GUIData(this);
    t_mapParamData extractedParams = CairnUtils::extractGuiParams(aComponent);
    mGUIData->doInit("Solver", "Solver", "Solver", extractedParams);

    declareCompoInputParam();
    setCompoInputParam(aComponent);
    InitSolverParam();

    mGUIData->setObjectName(mSolverName);
}

void Solver::solverNameChanged() 
{
    if(mGUIData)
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
    mCompoInputParam->addParameter(PARAM_SOLVER_NAME, &mSolverName, vDefaultSolver, true, true, "Solver name: Cbc, Cplex, Highs, etc.");
    mCompoInputParam->addParameter("Category", &mProblemType, "MIP", true, true, "Problem type: MIP, LP, etc. Swich to LP with Cplex to get faster optimization if the problem has no integer values.");
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

bool Solver::setCompoInputParam(const t_mapParamData& aComponent)
{
    bool vRet = true;

    if (aComponent.size() != 0) {
        mCompoInputParam->readParameters(aComponent);
        mCompoInputSettings->readParameters(aComponent);
    }

    return true;
}

void Solver::InitSolverParam() {
    // Retrieve available solvers
    t_list vSolverLoaded;
    mSolvers.getAllInfos(vSolverLoaded);

    // Retrieve available modelers
    t_list vModelerLoaded;
    mModelers.getModelersName(vModelerLoaded);

    // Default modeler handling (compatible with older projects) 
    if (mModelType == GS::MIPMODELER() || mModelType == "" || 
        mModelType == "Cplex" || mModelType == "MIPModeler")
    {
        mModelType = GS::MIPMODELER();

        // Old studies: solver name defaults to component name
        if (mSolverName.empty()) 
            mSolverName = Name();  

        if (!CairnUtils::contains(vSolverLoaded, mSolverName)) 
        {
            cWarning() << "Solver requested: " << mSolverName
                << ", possible solver names are: "
                << CairnUtils::joinStrings(vSolverLoaded);
        }
    }
    else if (CairnUtils::contains(vModelerLoaded, mModelType))
    {
        if (mModelType == GS::GAMS() && !CairnUtils::contains(mGAMSProblemTypes, mProblemType))
        {
            const std::string validTypes = CairnUtils::joinStrings(mGAMSProblemTypes, ", ");
            throw Cairn_Exception("Error: possible problem types are: " + validTypes, -1);
        }
        mExternalModeler = mModelers.getModeler(mModelType);
    }
    else {
        throw Cairn_Exception("Error: Modeler asked " + mModelType + ", possible modeler names are MIPModeler are " + CairnUtils::joinStrings(vModelerLoaded), -1);
    }
}

ModelerInterface* Solver::getExternalModeler() {    
    return mExternalModeler;
}

void Solver::SolveProblem(MIPModeler::MIPModel* aModel, const std::string &location, const int cycle, const std::map<std::string, bool> paramMap)
{
    mModel = aModel;

    cInfo() << "Setting solver properties...";

    if (mThreads == 0) mThreads = 8;
    mThreads = std::min(mThreads, (int)std::thread::hardware_concurrency());
    cInfo() << "Using " << mThreads << " / max logical Threads " << std::thread::hardware_concurrency();
   
    if (mModelType == GS::MIPMODELER()) {
        MIPSolverParams vParams;
        if (mSolverName == "Cplex")
        {            
            if (mTimeLimit > 0.) {
                vParams.addParam("TimeLimit", mTimeLimit);                
                cInfo() << "Setting TimeLimit to " << mTimeLimit ;
            }
            if (mGap > 0.) {
                vParams.addParam("Gap", mGap);                
                cInfo() << "Setting Gap to " << mGap ;
            }
            if (mThreads > 0 ) {
                vParams.addParam("Threads", mThreads);                
                cInfo() << "Setting Threads to " << mThreads ;
            }
            vParams.addParam("TerminateSignal", mTerminateSignal);

            vParams.addParam("Location", location);
      
            if (mTreeMemoryLimit > 0) {
                vParams.addParam("TreeMemoryLimit", mTreeMemoryLimit);
                cInfo() << "Setting TreeMemoryLimit to " << mTreeMemoryLimit;
            }
            if (mNbSolToKeep > 0) {
                vParams.addParam("NbSolToKeep", mNbSolToKeep);
                cInfo() << "Setting max number of solutions to " << mNbSolToKeep;
            }            
            vParams.addParam("SolverPrint", 1);
                        
            if (mWriteLp == "YES") vParams.addParam("WriteLp", 1);
            vParams.addParam("WriteLpCycle", cycle);
            if(mReadParamFile=="YES") vParams.addParam("ReadParamFile", 1); 
            if (mWriteMipStart == "YES") vParams.addParam("WriteMipStart", 1); 
            if (mProblemType == "LP") vParams.addParam("LpModel", true);

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
                cInfo() << "Setting TimeLimit to " << mTimeLimit;
            }
            if (mGap > 0.) {
                vParams.addParam("Gap", mGap);
                cInfo() << "Setting Gap to " << mGap;
            }
            if (mThreads > 0) {
                vParams.addParam("Threads", mThreads);
                cInfo() << "Setting Threads to " << mThreads;
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

        cInfo() << "  ";
        cInfo() << "Begin problem solving with " << mSolverName;
        //cInfo() << "  ";

        int ierr = mSolvers.solve(mSolverName, mModel, vParams, mSolverResults);

        if(ierr < 0)
            cCritical() << "An error has found while building the optimal problem " << mSolverName;
        else {
            //cInfo() << "  ";
            cInfo() << "End problem solving with " << mSolverName;
            cInfo() << "See local file cplex_optim.log for optimization details";
        }
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
        
        cInfo() << "  ";
        cInfo() << "Begin problem solving with ExternalModeler " << mSolverName;
        //cInfo() << "  ";

        mExternalModeler->solve(vParams, mExternalResults);

        //cInfo() << "  ";
        cInfo() << "End problem solving with ExternalModeler " << mSolverName;
        cInfo() << "See local file \".lst\" for optimization details";
        cInfo() << "ModelState: " << mExternalResults.getModelStatus();
        cInfo() << "SolveState: " << mExternalResults.getSolverStatus();

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
       return (mSolverResults.getOptimisationStatus());

   }
   else if (mExternalModeler != nullptr) {
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
