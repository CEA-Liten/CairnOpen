#ifndef MilpComponent_H
#define MilpComponent_H

class SubModel;
class TecEcoAnalysis;

#include <Eigen/Dense>
#include <map>

#include "CairnCore_global.h"
#include "Cairn_Exception.h"

#include "Solver.h"
#include "EnergyVector.h"

#include "JsonDescription.h"
#include "MilpData.h"
#include "MilpPort.h"
#include "GUIData.h"

#include "GlobalSettings.h"

using Eigen::VectorXf;
using Eigen::MatrixXf;

#include "MIPModeler.h"
#include "ZEVariables.h"
#include "ModelVar.h"
#include "ModelTS.h"

#include <optional>

using namespace std;

/**
 * \brief The MilpComponent class is the virtual class of every component of global MILP OptimProblem
 * Ports are used for flux and potential value exchanges between elements through bus connectors of types FluxBalance and BusSameValue
 * Ports will be build at constructor step on the basis of \<Port1\> \<Port2\>... fields
 */
typedef SubModel* (*f_submodel)(CairnObject* aParent);

class CAIRNCORESHARED_EXPORT MilpComponent : public CairnObject
{
public:
    MilpComponent(CairnObject* aParent, const std::string& aName, 
        MilpData *aMilpData, 
        TecEcoAnalysis *aTecEcoAnalysis,
        const t_mapParamData& aComponent,
        const std::map < std::string, t_mapParamData > &aPorts = {},
        class ModelFactory* aModelFactory = nullptr);

    virtual ~MilpComponent();
    
    void initMilpComponent();

    std::string Name() const {return this->objectName(); }
    void setName(const std::string& name) { this->setObjectName(name); }

    std::string Type() const { return mType; } 

    // Component IO with PEGASE

    //Published variables
    void createZEUserVariablesList(std::string Full_File_Name, t_mapExchange& a_Exchange) ;  // common function to publish other variables
    //Subscribed variables : hist persistent timeseries
    void exportRHVariableInModel(); /** function to export the list of the data saved for rolling horizon into models */
    int  createHistFXLists();

    virtual void prepareOptim();                                                                /** pre-treatment of PEGASE exchange Zone */
    virtual void populatePublishedVars(t_mapExchange& a_Export);                                                             /** export results to PEGASE exchange Zone */
    virtual void populatePublishedPortVars(t_mapExchange& a_Export, uint modinitTS);
    virtual void setDefaultsResults();                                                        /** define default values in case optimization fails */

    // Utility functions for PEGASE IO management
    virtual bool findFirstCoeff(std::string aVarName, t_mapExchange aList , float &coeff, float &offset) ;

    // MILP functions
    virtual int initSubModelConfiguration(const bool& readParams = true) ;      /** Read model Input parameters from SettingsFile and Input timeseries from Pegase Exchange Zone */
    virtual void redeclareEnvImpactParameters(); /** when selected impacts get modified */
    virtual int initSubModelInput() ;              /** Read model Input parameters from SettingsFile and Input timeseries from simulation environment */
    int initPorts();                                                                        /** port Milp flux expression init (allocation) */
    virtual int checkPorts() ;                                                                       /** port Milp flux expression checking and typing (scalar or vector) */
    void setSubModelEnvImpacts();

    virtual int createRHVariables();
    int initProblem(const bool& readParams=true);                                   /** define Milp Variable of component */
    void createCompoModel();                              
    void deleteCompoModel();

    virtual void buildProblem();  /** build Milp Model of component : own constraint, connection constraint, objective contribution */

    void resetCompoModel();
    void cleanTimeSeries();

    void exportSubmodelIO(Solver* aSolver, int aNsol);  /** Output Data (for link with PEGASE or OUTSIDE) */
    void removeIOs();
    
    void setBusFluxPortExpression(); 
    void assignFluxToPort(MilpPort* port, double sign); 

    void setBusSameValuePortExpression(); 
    void assignPotentialToPort(MilpPort* port);

    /* Methods that calls a SubModel function */
    MIPModeler::MIPExpression* getMIPExpression(std::string aExpressionName);
    MIPModeler::MIPExpression1D* getMIPExpression1D(std::string aExpressionName);
    MIPModeler::MIPExpression& getMIPExpression1D(uint i, std::string aExpressionName);
    /* ------------------------------------- */
    virtual void setMIPModel (MIPModeler::MIPModel* aModel) ;                        /** Set pointer to global Optimization Problem Model */

    virtual int defineDefaultVarNames();

    void createOnePort(const std::string& portId, const t_mapParamData & portParams,
        EnergyVector* carrier = nullptr);

std::string getUniquePortID();
    const std::vector<MilpPort*> &PortList();
    void addPort(MilpPort* lptrport);
    void removePort(MilpPort* lptrport);
    MilpPort* getPort(const std::string& portId);
    MilpPort* getPortByName(const std::string& aPortName);

    virtual void declareCompoInputParam();
    virtual void setCompoInputParam(const t_mapParamData& aComponent);
    void initGuiData(const t_mapParamData& paramMap);

    bool allDefaultPortsHaveVariables();
    void declareIndicators();

    bool allDefaultPortsHaveCarriers();
    void declareIOVariables();

    virtual std::vector<std::string> get_IOVarNames() const;
    virtual std::string get_IOVarDescription(const std::string& varName) const;

    virtual void jsonSaveGuiComponent(ojson &componentsArray, const std::string& componentCarrier, 
        const std::vector<std::string>& refLabelList) ;
    void jsonSaveGUINodePortsData(ojson &nodePortsArray, const std::string & aSide);
    virtual void jsonSaveGUIlistPortsData(ojson &nodePortArray, const std::string& aSide);
    void jsonSaveGUITimeSeries(ojson& paramArray, const InputParam* const inputParam);// = nullptr);
    void jsonSaveGUICompoNodePortsData(ojson& nodePortsArray, ojson& nodePortsData);
    virtual std::vector<MilpPort*> listSidePorts(const std::string& aside);

    std::string ModelName() {return mCompoModelName;}
    std::string ModelClassName() { return mCompoModelClassName; }
    void setModelClassName(const std::string& modelClass) { mCompoModelClassName = modelClass; }
    GUIData* getGUIData() {return mGUIData;}

    InputParam* getPlugSubmodelIO() {return mPlugSubmodelIO;}
    InputParam* getCompoInputParam(){return mCompoInputParam;}

    SubModel* compoModel() const { return mCompoModel; }
    MilpData* getMilpData() const {  return mMilpData ; }

    // shortcut access to time data
    double TimeStep(int i) const {return mMilpData->TimeStep(i);}    //TimeStep in HOUR
    std::vector<double> TimeSteps() const {return mMilpData->TimeSteps();}    //TimeStep list in HOUR

    float pdt() const { return mMilpData->pdt(); }              /** Pas de temps en secondes */
    double pdtHeure() const { return mMilpData->pdtHeure(); }   /** La meme grandeur mais en heures */
    uint npdtPast() const { return mMilpData->npdtPast(); }     /** Nombre de pas de temps passe */
    uint npdt() const { return mMilpData->npdt(); }             /** Nombre de pas de temps (futur) */
    uint npdtTot() const { return mMilpData->npdtTot(); }       /** Nombre total de pas de temps (passe + futur) */
    uint timeshift() const { return mMilpData->timeshift(); }   /** Time shifting */
    uint iHMFuturSize() const { return mMilpData->iHMFuturSize(); }
    uint startingAbsoluteTimeStep() const { return mMilpData->startingAbsoluteTimeStep(); } /** Starting absolute timestep number for the current optimization */

    virtual void defineMainCarrier(); /** set the main carrier of the component **/
    void setMainCarrier(EnergyVector* aptrEnergyVector);  /** Set a pointer to the main carrier of the component */
    EnergyVector* getMainCarrier() const; /** Pointer to the main carrier of the component */

    Cairn_Exception getException() const {return mException;}
    void setException (const Cairn_Exception &aException) {mException = aException;}
    
    bool createModelTS(const std::string& varName, const std::string& tsName, ModelParam* aParamTS);

    virtual void readTSVariablesFromModel();

    std::map<std::string, std::string> getTimeSeriesNames();
    std::string getTimeSeriesName(const std::string& ts_paramName);
    void setTimeSeriesName(const std::string& ts_paramName, const std::string& ts_name);

    const std::vector<double>* getTimeSeries(const std::string& varName) const;

    void computeHistNbHours();
 
    virtual std::vector<std::string> get_ModelClassList();
    std::map<std::string, ModelParam*> getParameters(const bool includePortParams=false); //get all the parameters of the componenet

    bool EnvironmentModel();
    bool EcoInvestModel();
    bool isBus();

    int getXpos() const { return mGUIData->getXpos(); }
    int getYpos() const { return mGUIData->getYpos();; }

    void setXpos(const int& aXpos);
    void setYpos(const int& aYpos);

    void updateCompoParamMap(const std::string& a_SettingName, 
        const std::string& a_AttributeName, const std::string& a_AttributeValue);

    void updatePortParamMap(const std::string& portId, const std::string& portName, 
        const std::string& a_SettingName, const std::string& a_AttributeName, const std::string& a_AttributeValue);

    MilpPort* mapDefaultPort(const std::string& portId, const std::map<std::string, std::string>& portParams);

    std::string getAbsoluteFileName(const std::string& filename);

    void createImportListVars(t_mapExchange& a_Import);
    void createExportListVars(t_mapExchange& a_Export);
    void createPortsExportListVars(t_mapExchange& a_Exchange);

    t_mapParamData portData(const std::string& portId, const std::string& portName) const;

    void setTecEcoAnalysis(TecEcoAnalysis* mTecEcoAnalysis);

    const std::string* pCurrency() const;
    double& ExtrapolationFactor();
    std::vector<double>& LevelizationTable();
    std::vector<double>& ImpactLevelizationTable();
    std::vector<double>& TableYearsHours();

    void setHistNbHours(const double& aHistTime) { mHistNbHours += aHistTime; };
    double HistNbHours() { return mHistNbHours; }
    void setOptimalSize(double aOptimalSize) { mOptimalSize = aOptimalSize; }

    std::vector<std::string> possibleModelClasses() const;

    virtual std::vector<class InputParam*> get_InputParams();

    std::vector<InputParam*> get_ParamInputParams();
    std::vector<InputParam*> get_OptionInputParams();
    virtual std::vector<InputParam*> get_TimeSeriesInputParams();
    virtual std::vector<InputParam*> get_EnvImpactInputParams();
    virtual std::vector<InputParam*> get_PortEnvImpactInputParams();
    virtual InputParam* get_PerfParam();

    std::optional<double> getIndicatorValue(const std::string& indicatorName, const std::string& range = "PLAN") const;
    bool isInstalled() const;

    inline t_list getPossibleControlValues() const { return { "", "RollingHorizon", "MPC" }; }

protected:
    Cairn_Exception mException;
    MIPModeler::MIPModel* mModel;     /** Pointer to global Optimization Problem Model */
    GUIData* mGUIData{ nullptr };      /** Pointer to GUI Data */

    SubModel* mCompoModel{ nullptr };  /** Pointer to global component SubModel of name mCompoModelName*/
    TecEcoAnalysis* mTecEcoAnalysis;

    // Component settings
    t_mapParamData mComponent ;         /** Map of Topological data from .json */
    std::map < std::string, t_mapParamData > mPorts;  /** Map of port data from .json, portName: std::map<std::string, std::string> */

    int mHistNbHours;          /** nombre d'heures déjà décompté par le passé (permet de faire la bonne actualisation) */
    double mOptimalSize; //Not used anywhere!  /** Optimal size of component, if optimized eg Nominal power, storage capacity...*/

    InputParam* mCompoInputParam{ nullptr };      /** COMPONENT Input parameter List from XML File -> Options */
    InputParam* mInputParam{ nullptr };                           /** Pointer to COMPONENT Input Data List (for link with PEGASE or OUTSIDE) */
    InputParam* mModelParam{ nullptr };                           /** Pointer to SubModel Input parameter List (constant of the Milp submodel)*/
    InputParam* mModelEnvImpactParam{ nullptr };                   /** Pointer to SubModel Environmental impacts parameter List */
    InputParam* mModelPortImpactParam{ nullptr };                   /** Pointer to SubModel Port Environmental impacts parameter List */
    InputParam* mModelPortImpactParamTS{ nullptr };                 /** Pointer to SubModel Port Environmental impacts time series List (contains only time series) */
    InputParam* mModelPerfParam{ nullptr };                        /** Pointer to SubModel Input performance parameter List (constant of the Milp submodel)*/
    InputParam* mModelData{ nullptr };                            /** Pointer to SubModel Input Data List (constant of the Milp submodel) on future horizon only*/
    InputParam* mModelDataTS{ nullptr };                           /** Pointer to SubModel Input TimeSeries List (constant of the Milp submodel) on future horizon only*/

    InputParam* mPlugSubmodelIO{ nullptr };                       /** Pointer to SubModel Output Data List (for link with PEGASE or OUTSIDE) */
    InputParam* mTimeSeriesSubmodel{ nullptr };                   /** Pointer to SubModel TimeSeries List (for link with PEGASE or OUTSIDE) */

    MilpData* mMilpData{ nullptr };                          /** Pointer to Milp Time Data */

    //topology
    std::string mCompoModelName ;                       /** Component Model Name to be used eg an Electrolyzer */ 
    std::string mCompoTechnoType;                       /** Component TechnoType Name to be used only by GUI for image eg an PVSource */
    std::string mCompoModelClassName ;                  /** Component Model Class Name to be used eg a Basic Electrolyzer or SOEC Electrolyzer */

    //technical input
    bool  mIsImposedWeight ;                            /** if true: add constraint setting component weight to imposed value - if false, weight of component - useful to relax on load flux profiles */
 
    int mFirstInit ; /** 0 if firstInit, else */
    int mFirstInitTS ; /** 0 if firstInit, else */
    
    std::string mType;  
    std::string mControl;
    std::string mDataFile;
    std::string mPublishUserVariable;
    std::string mSubmodelFile; //Do we still need this?!

    int initializeModel();
    int readPerfMapFiles();

    // Time Series
    typedef std::map<std::string, ModelTS> t_mapTS;
    t_mapTS m_timeSeries = {};
    void readTSVariables(InputParam* aMapParamTS);
    void readEnergyVectorTS(const EnergyVector* carrier, const InputParam* aMapParamTS);

    // compo model    
    class ModelFactory* mModelFactory;
    virtual bool newCompoModel();  
    void createPorts();
    EnergyVector* getCarrier(const std::string& carrierName);
};

#endif // MilpComponent_H
