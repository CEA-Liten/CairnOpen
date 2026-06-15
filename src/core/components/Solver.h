#ifndef SOLVERCOMPO_H
#define SOLVERCOMPO_H

#include "InputParam.h"
#include "GUIData.h"

#include "MIPModeler.h"
#include "MIPSolverFactory.h"
#include "ModelerFactory.h"

#include "Cairn_Exception.h"

#include "GlobalSettings.h"
using namespace GS;

/**
 * \details
* This component defines the solver to be used. \\
* It is not mandatory. By default, Cbc solver will be used.
*/

class CAIRNCORESHARED_EXPORT Solver: public CairnObject
{
    
public:
    Solver(CairnObject* ap_Parent, const std::string& aName, const t_mapParamData& aComponent={});
    virtual ~Solver();
       
    void SolveProblem(MIPModeler::MIPModel* aModel, const std::string &location, const int cycle=0, const std::map<std::string, bool> paramMap = std::map<std::string, bool>());
    std::string getOptimisationStatus();
    int getNumberOfSolutions();
    double getSolverRunningTime();

    const double* getOptimalSolution(int aNsol, const std::string& varname="");
    bool getIsCheckConflicts();
    
    void jsonSaveGuiComponent(ojson &componentsArray) ;

    double getTimeLimit() const {return mTimeLimit;}
    double getGap() const {return mGap;}
    int getThreads() const {return mThreads;}
    std::string getModelType() const {return mModelType;};
    void setStopSignal(int* stopSignal);
    ModelerInterface *getExternalModeler();
   
    InputParam* getCompoInputParam() { return mCompoInputParam; }  /** Get access to Model Parameters */
    InputParam* getCompoInputSettings() { return mCompoInputSettings; }  /** Get access to Model Parameters */

    std::map<std::string, ModelParam*> getParameters();

    Cairn_Exception getException() const { return mException; };
    GUIData* getGUIData() { return mGUIData; }

    std::string Name() const { return std::string(this->objectName().c_str()); }
    void setName(const std::string& name) { this->setObjectName(name); }

    std::string SolverName() const { return mSolverName; }
    std::vector<InputParam*> get_InputParams();

    std::vector<InputParam*> get_ParamInputParams();
    std::vector<InputParam*> get_OptionInputParams();

    std::vector<std::string> getProblemTypes() const {
        return (mModelType == GS::GAMS()) ? mGAMSProblemTypes : std::vector<std::string>{ "MIP", "LP"};
    }

    std::vector<std::string> getPossibleModelTypes() const {
        return mPossibleModelTypes;
    }

private:
    void doInit(const t_mapParamData& aComponent);
    void declareCompoInputParam();
    bool setCompoInputParam(const t_mapParamData& aComponent);
    void InitSolverParam();

    static const std::vector<std::string> mPossibleModelTypes;
    static const std::vector<std::string> mGAMSProblemTypes;

    Cairn_Exception mException;

    ModelerFactory mModelers;
    ModelerInterface* mExternalModeler;
    ModelerResults mExternalResults;

    MIPSolverFactory mSolvers;
    MIPSolverResults mSolverResults;

    MIPModeler::MIPModel* mModel;
    GUIData* mGUIData;                 /** Pointer to GUI Data */

    InputParam* mCompoInputParam;      /** COMPONENT Input parameter List from XML File -> Options */
    InputParam* mCompoInputSettings;   /** COMPONENT Input parameter List from Settings File -> Params */

    std::string mModelId;
    std::string mModelType; 
    std::string mSolverName;    /* Solver name : Cplex, Cbc, Highs */
    std::string mProblemType;

    std::string mWriteLp;
    std::string mFileMipStart;
    std::string mWriteMipStart;
    std::string mReadParamFile;
    std::string mWriteGMS;
    bool mIsLpModel;
    int mNbSolToKeep;
    double mGap;
    double mTimeLimit;
    int mThreads;
    int mTreeMemoryLimit; // MB

    int* mTerminateSignal;

    double mSolverRunningTime;
};

#endif // SOLVERCOMPO_H
