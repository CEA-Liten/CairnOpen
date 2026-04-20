#ifndef OptimProblem_H
#define OptimProblem_H

class OptimProblem;

#include <math.h>   
#include <iostream>
#include <fstream>

#include <Eigen/SparseCore>
#include <Eigen/Dense>

#include "CairnCore_global.h"
#include "GlobalSettings.h"

#include "MilpData.h"
#include "MilpComponent.h"
#include "ZEVariables.h"
#include "JsonDescription.h"

#include "TecEcoCompo.h"
#include "GridCompo.h"
#include "SourceLoadCompo.h"
#include "BusCompo.h"
#include "MultiObjCompo.h"

#include "Solver.h"
#include "TecEcoAnalysis.h"
#include "EnergyVector.h"
#include "SimulationControl.h"

#include "ModelFactory.h"
#include "DynamicIndicator.h"
#include "StudyPathManager.h"
#include "ModelVar.h"

#include "CairnAPI.h"

/**
 * \brief La classe OptimProblem permet de definir une collection de pb MILP OptimProblem
 */

typedef MilpComponent* (*f_MilpComponent)(CairnObject* aParent, std::string aName, MilpData* aMilpData, TecEcoAnalysis* aTecEcoAnalysis,
                                          const std::map<std::string, std::string> &aComponent, const std::map < std::string, std::map<std::string, std::string> >& aPorts);

class CAIRNCORESHARED_EXPORT OptimProblem : public CairnObject
{
    
public:
    OptimProblem(CairnObject* aParent, std::string aName, MilpData* aMilpData, const bool& aStdAloneMode = true);
    ~OptimProblem();

    TecEcoAnalysis* getTecEcoAnalysis() { return mTecEcoAnalysis; }; 
    void createTecEcoAnalysisFromParamMap();
    bool createTecEcoAnalysis();

    SimulationControl* getSimulationControl() { return mSimulationControl; };
    void createSimulationControlFromParamMap();
    bool createSimulationControl(const std::string& mSimulationControlName = "Cairn", const std::map<std::string, std::string>& paramMap = {});

    Solver* getSolver() { return mSolver; }
    bool createSolver(const std::string& aName = "Solver", const std::map<std::string, std::string>& paramMap = {});

    //getEnergyVector()
    bool createEnergyVector(const std::string& aName, const std::string& aType, const std::map<std::string, std::string> paramMap = {});

    std::vector<BusCompo*> BusComponents();
    std::vector<MilpComponent*> NonBusMilpComponents();
    std::map<std::string, MilpComponent*> MilpComponents(); //all MilpComponents including Buses
    std::vector<EnergyVector*> EnergyVectors();

    void doInit(const StudyPathManager& aStudy, bool aLoad);

    void setMIPModel(MIPModeler::MIPModel* aModel) ;
    void setObjective(MIPModeler::MIPExpression* aExpObjective) ;

    std::string getOptimDirection() ;

    int initProblem();
    void redeclareEnvImpactParameters();
    int initSubModelInput();
    int setParameters();
    int getNumberOfSolutions();

    void computeExtrapolationFactor();

    void buildProblem();
    
    void readSolution(int n=0);
    void closeExpressions();

    void writeSolution(int n, std::map<std::string, std::vector<double>>& resultats);    

    void exportEnvImpactMassIndicators(const std::string& aFileName = "", const std::string& encoding = "UTF-8");
    void exportEnvImpactParameters(const std::string& aFileName = "", const std::string& encoding = "UTF-8");
    void exportPortEnvImpactParameters(const std::string& aFileName = "", const std::string& encoding = "UTF-8");

    ExportParameterData collectParameterData(const std::map<std::string, bool>& optionsMap = {});
    void exportParameters(const std::string& aFileName, const std::string& encoding = "UTF-8", 
        const std::map<std::string, bool>& optionsMap = {},
        const std::map< std::string, std::vector<ExtraParameterData> >& extraData = {});

    void exportParameters_all_files(std::string aFileName, const std::string& encoding = "UTF-8", 
        const std::map<std::string, bool>& optionsMap = {},
        const std::map< std::string, std::vector<ExtraParameterData> >& extraData = {}); 

    void setStopSignal(int* stopSignal);
    void solveProblem(std::string& optimLogFileName, const int cycle, const std::map<std::string, bool> paramMap = std::map<std::string, bool>(), const bool aExportResultsEveryCycle = false);

    void prepareOptim();
    void populatePublishedVars();
    void setDefaultsResults();
    void computeTecEcoEnvAnalysis(const int& aNsol);
    void exportMultiObjFile(std::fstream& out, int aNsol, const bool showDescription);
    void exportAllTecEcoEnvAnalysis(const std::string &aResultFile, const std::string& range, 
        bool showDescription = false, const std::string& encoding = "UTF-8", bool isRollingHorizon=false, int aNsol=0);
    void exportResultsPLAN(std::string aResultFile, const int& aNsol = 0);
    
    void computeHistState();
    void exportOptimaSizeAllCycles(const std::string& aFileName, int cycle, const std::string &encoding = "UTF-8");

    double getSolverRunningTime() { return mSolver->getSolverRunningTime();  }

    void buildComponentConstraints();
    void buildBusConstraints();

    void createEnergyVectorsFromParamMap();
    void createMilpComponentsFromParamMap();
    void createLinksToBus();
    void createDynamicIndicators();
    void computeDynamicIndicators(const int& aNsol); //should be called after the end of the simulation

    bool createComponent(const std::string& componentType, const std::map<std::string, std::string> &paramsMap = {},
        const std::map < std::string, std::map<std::string, std::string> >& portsMap={});
    void configureBusCarrier(MilpComponent* lptrBus, const std::string& carrierName);
    void createLinksToBus(MilpComponent* lptrComponent);
    void deleteComponent(MilpComponent* lptrComponent);

    const t_mapExchange& ListSubscribedVariables() { return mListSubscribedVars; } /** define INPUT Variable of component */
    t_mapExchange& ListPublishedVariables() { return mListPublishedVars; }        /** define OUTPUT Variable of component */

    void createImportZEVariablesList();
    void createExportZEVariablesList();

    int SaveFullArchitecture(const std::string& filename = "", const std::string & posAlgorithm = "");
               
    f_MilpComponent LoadDllMilpComponent(std::string Filename, std::string ModuleName) ;

    std::string getOptimisationStatus() ;
    int getInterpretedOptimStatus();
    bool getIsCheckConflicts();

    void computeGUICompoAndBusLocations();

    void computeGUIBusLocations();
    void computeGUIComponentLocations();

    static std::string getRelease() {return GS::Cairn_Release;}

    void setStdAloneMode (const bool & abool) {mStdAloneMode = abool ;}

    void computeObjectiveFunction(MIPModeler::MIPExpression& objective);
    void resetFlags();

    std::string projectDir() const { return mStudyFile->projectDir().c_str(); }
    std::string getAbsoluteFileName(const std::string& filename);

    MilpData* getMilpData() { return mMilpData; }

private:    
    MilpData* mMilpData{ nullptr };  /** Pointer to Milp Time Data */
    MIPModeler::MIPModel* mModel;    /** Pointer to global Optimization Problem Model */

    ModelFactory* mModelFactory;
    JsonDescription* mJsonDescription ;
    Solver* mSolver{ nullptr };
    TecEcoAnalysis* mTecEcoAnalysis;  
    SimulationControl* mSimulationControl;
    
    const StudyPathManager* mStudyFile{ nullptr };   // the architecture of study file

    bool mStdAloneMode;  // true indicates no link with Pegase

    MIPModeler::MIPExpression* mExpObjective;
    int mOptimStatus;

    std::vector< t_mapParams > mMilpComponents ;      // Listes cle / valeur de description des composants MILPs a partir du fichier de description

    t_mapExchange mListPublishedVars{};  // export
    t_mapExchange mListSubscribedVars{}; // import

    //------------------------------ Dynamic Indicators ----------------------------------------------------------------- 
    std::vector< t_mapParams > mDynamicIndicatorsData{}; // List of dynamic indicators data in the form "key: value"  
    std::vector<DynamicIndicator*> mDynamicIndicators{}; // List of pointers to DynamicIndicator objects  

    bool mExportIndicators;

    void jsonSaveDocument(ojson& jsonOutputFile);
    void jsonSaveGuiComponents(ojson& componentsArray);
    void jsonSaveGuiLinks(ojson& linksArray);
    void jsonSaveGuiLinkNodes(ojson& linksArray, const std::string& compoName, const std::string& compoPortName, const std::string& busName, const std::string& busPortName,
        const int& compoX, const int& compoY, const int& busX, const int& busY);
};

#endif // OptimProblem_H
