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
                                          const t_mapParamData& aComponent, const std::map < std::string, t_mapParamData>& aPorts);

struct CompoData {
    std::string rawName; /* oldname - name from the input file */
    std::string name;
    std::string type;
};

struct PairHash {
    std::size_t operator()(const std::pair<std::string, std::string>& p) const noexcept {
        std::size_t h1 = std::hash<std::string>{}(p.first);
        std::size_t h2 = std::hash<std::string>{}(p.second);
        return h1 ^ (h2 << 1);
    }
};


class CAIRNCORESHARED_EXPORT OptimProblem : public CairnObject
{
    
public:
    OptimProblem(CairnObject* aParent, std::string aName, MilpData* aMilpData, const bool& aStdAloneMode = true);
    ~OptimProblem();

    TecEcoAnalysis* getTecEcoAnalysis() { return mTecEcoAnalysis; };
    SimulationControl* getSimulationControl() { return mSimulationControl; };
    Solver* getSolver() { return mSolver; }

    std::vector<BusCompo*> BusComponents();
    std::vector<MilpComponent*> NonBusMilpComponents();
    std::map<std::string, MilpComponent*> MilpComponents(); //all MilpComponents including Buses
    std::vector<EnergyVector*> EnergyVectors();

    void createComponentsFromJsonData( const std::string& vJsonFile,
        std::vector<CompoData>* importedComponents = nullptr,
        bool isGroup = false, std::string* groupName = nullptr, 
        std::string* mainNode = nullptr);

    bool createTecEcoAnalysis(const std::string& componentType = "TecEcoAnalysis", 
        const t_mapParamData& paramsMap = {},
        const std::map < std::string, t_mapParamData>& portsMap = {}, 
        const std::vector<std::string>& labelList = {});

    bool createSimulationControl(const std::string& aName = "Cairn", 
        const t_mapParamData& paramMap = {});

    void createSolver(const std::string& aName = "Solver",
        const t_mapParamData& paramMap = {});

    bool createEnergyVector(const std::string& aName, const std::string& aType, 
        const std::string& aTechnoType, const t_mapParamData& paramMap = {});

    MilpComponent* createMilpComponent(const std::string& compoName, const std::string& compoType, 
        const t_mapParamData& paramsMap = {}, const std::map < std::string, t_mapParamData>& portsMap = {});

    void doInit(const StudyPathManager& aStudy, bool aLoad);

    void setMIPModel(MIPModeler::MIPModel* aModel) ;
    void setObjective(MIPModeler::MIPExpression* aExpObjective) ;

    std::string getOptimDirection() ;

    int initProblem();
    void redeclareEnvImpactParameters();
    int initSubModelInput();
    void exportRHVariableInModel();
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

    ExportParameterRows collectParameterData(const std::map<std::string, bool>& optionsMap = {});
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

    void buildComponentConstraints();
    void buildBusConstraints();

    void createLinksToBus();
    void createDynamicIndicators();
    void computeDynamicIndicators(const int& aNsol); //should be called after the end of the simulation

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

    //static std::string getRelease() {return GS::Cairn_Release;}

    void setStdAloneMode (const bool & abool) {mStdAloneMode = abool ;}

    void computeObjectiveFunction(MIPModeler::MIPExpression& objective);
    void resetFlags();

    const StudyPathManager* studyManager() const { return mStudyPathManager;}

    std::string projectDir() const { return mStudyPathManager->projectDir().c_str(); }
    std::string getAbsoluteFileName(const std::string& filename);

    MilpData* getMilpData() { return mMilpData; }

    std::vector<std::string> GroupNames() const;

    void addGroup(const std::vector<std::string>& compoNames, 
        const std::string& mainCompo = "", const std::string& groupName = "");

    /** Returns the study version extracted from the last loaded JSON, or empty if none loaded. */
    std::string studyVersion() const { return mStudyVersion; }

    /** Checks mStudyVersion against the current Cairn version. No operation if mStudyVersion is empty. */
    int checkVersion() const;

    bool studyVersionMatchesCairn() const { return mStudyVersionMatchesCairn; }
    void setStudyVersionMatchesCairn(bool value) { mStudyVersionMatchesCairn = value; }
private:    
    std::string mStudyVersion;   /** version string extracted from the last loaded JSON study; empty if none loaded */
    bool mStudyVersionMatchesCairn = true;

    MilpData* mMilpData{ nullptr };  /** Pointer to Milp Time Data */
    MIPModeler::MIPModel* mModel;    /** Pointer to global Optimization Problem Model */

    ModelFactory* mModelFactory;
    Solver* mSolver{ nullptr };
    TecEcoAnalysis* mTecEcoAnalysis;  
    SimulationControl* mSimulationControl;
    
    const StudyPathManager* mStudyPathManager{ nullptr };   // the architecture of study file

    bool mStdAloneMode;  // true indicates no link with Pegase

    MIPModeler::MIPExpression* mExpObjective;
    int mOptimStatus;

    t_mapExchange mListPublishedVars{};  // export
    t_mapExchange mListSubscribedVars{}; // import

    bool mExportIndicators;

    std::vector< t_mapUserIndicator > mDynamicIndicatorsData{}; // List of dynamic indicators data in the form "key: value"  
    std::vector<DynamicIndicator*> mDynamicIndicators{}; // List of pointers to DynamicIndicator objects  

    std::vector<t_mapGroups> mGroups{};

    void jsonSaveDocument(ojson& jsonOutputFile);
    void jsonSaveGuiComponents(ojson& componentsArray);
    void jsonSaveGuiLinks(ojson& linksArray);
    void jsonSaveGuiLinkNodes(ojson& linksArray, const std::string& compoName, const std::string& compoPortName, const std::string& busName, const std::string& busPortName,
        const int& compoX, const int& compoY, const int& busX, const int& busY);
    void jsonSaveGuiGroups(ojson& groupsArray) const;

    void createBaseComponents(JsonDescription* jsonDesc);

    void createEnergyVectors(JsonDescription* jsonDesc,
        std::vector<CompoData>* importedComponents = nullptr);

    void createUniqueComponents(JsonDescription* jsonDesc);

    void createComponents(JsonDescription* jsonDesc,
        std::vector<CompoData>* importedComponents = nullptr);

    void createComponent(
        const t_mapParamData& compoParamData,
        const std::map<std::string, t_mapParamData>& ports,
        const std::map<std::string, t_mapLabels>& labels,
        std::vector<CompoData>* importedComponents = nullptr, /* Also used as a flag isUniqueName */
        // bool isUniqueName = true,
        const std::vector<std::string>& existingComponents = {});

    inline void addImportedComponent(std::vector<CompoData>* list,
        const std::string& rawName,
        const std::string& name,
        const std::string& type)
    {
        if (!list)
            return;

        list->push_back({ rawName, name, type });
    }
};

#endif // OptimProblem_H
