#ifndef MilpComponent_H
#define MilpComponent_H
class MilpComponent ;
class SubModel;

#include <QtCore>
#include <qdom.h>
#include <QFile>
#include <QVector>
#include <QMap>
#include <QSettings>
#include <QDebug>
#include <QString>
#include <Eigen/Dense>
#include <map>

#include "CairnCore_global.h"
#include "Cairn_Exception.h"

#include "EnergyVector.h"
#include "TecEcoEnv.h"
#include "JsonDescription.h"
#include "MilpData.h"
#include "MilpPort.h"
#include "GUIData.h"
#include "Solver.h"
#include "GlobalSettings.h"

using Eigen::VectorXf;
using Eigen::MatrixXf;

#include "MIPModeler.h"
#include "ZEVariables.h"
#include "ModelVar.h"
#include "ModelTS.h"

using namespace std;


/**
 * \brief The MilpComponent class is the virtual class of every component of global MILP OptimProblem
 * Ports are used for flux and potential value exchanges between elements through bus connectors of types FluxBalance and BusSameValue
 * Ports will be build at constructor step on the basis of \<Port1\> \<Port2\>... fields
 */
typedef SubModel* (*f_submodel)(QObject* aParent);

class CAIRNCORESHARED_EXPORT MilpComponent : public TecEcoEnv
{
public:
    MilpComponent(QObject* aParent, QString aName, MilpData *aMilpData, TecEcoEnv &aTecEcoEnv,  
                  const QMap<QString, QString> &aComponent, const QMap < QString, QMap<QString, QString> > &aPorts={}, class ModelFactory* aModelFactory=nullptr);

    virtual ~MilpComponent();
    virtual void initMilpComponent();

    virtual QString Name() const {return mName ;}  
    virtual QString Type() const { return mCompoModelName; } 

    virtual double Sens() const {return mSens ;}                                             /** get component Sink/Source direction */

    // Component IO with PEGASE

    //Published variables
    virtual void createZEVariablesList();
    void createZEUserVariablesList(QString Full_File_Name, t_mapExchange& a_Exchange) ;  // common function to publish other variables
    void createExchangeListVars(t_mapExchange &a_Import, t_mapExchange &a_Export);
    //Subscribed variables : hist persistent timeseries
    void exportRHVariableInModel(); /** function to export the list of the data saved for rolling horizon into models */
    int  createHistFXLists();

    virtual void prepareOptim();                                                                /** pre-treatment of PEGASE exchange Zone */
    virtual void exportResults(t_mapExchange& a_Export);                                                             /** export results to PEGASE exchange Zone */
    virtual void exportPortResults(t_mapExchange& a_Export, uint modinitTS);
    virtual void setDefaultsResults();                                                        /** define default values in case optimization fails */

    // Utility functions for PEGASE IO management
    virtual bool findFirstCoeff(QString aVarName, QMap<QString, ZEVariables*> aList , float &coeff, float &offset) ;

    // MILP functions
    virtual void initSubModelTopology() ; /**Send topology data to submodel : list of ports*/
    virtual int initSubModelConfiguration() ;      /** Read model Input parameters from SettingsFile and Input timeseries from Pegase Exchange Zone */
    virtual int initSubModelInput() ;              /** Read model Input parameters from SettingsFile and Input timeseries from simulation environment */
    virtual int initPorts() ;                                                                        /** port Milp flux expression init (allocation) */
    virtual int checkPorts() ;                                                                       /** port Milp flux expression checking and typing (scalar or vector) */

    virtual int setParameters();
    virtual int initProblem();                                   /** define Milp Variable of component */
    void createCompoModel();                              
    void deleteCompoModel();

    virtual void buildProblem();  /** build Milp Model of component : own constraint, connection constraint, objective contribution */
    void resetFlags();

    void exportSubmodelIO(Solver* aSolver, int aNsol);  /** Output Data (for link with PEGASE or OUTSIDE) */

    virtual void cleanExpressions();                                /** Clean expressions of the submodel */
    
    virtual void setBusFluxPortExpression();                                              /** Fill in expression at BusSameValue port connexion */
    virtual void setBusFluxPortExpression(const double &aSignedCoefficient);                                              /** Fill in expression at BusSameValue port connexion */
    virtual void setBusFluxPortExpression(MilpPort *port, const double &aSignedCoefficient);                                              /** Fill in expression at BusSameValue port connexion */
    virtual void setBusSameValuePortExpression() ;                                           /** Fill in expression at BusSameValue port connexion */

    /* Methods that calls a SubModel function */
    MIPModeler::MIPExpression* getMIPExpression(QString aExpressionName);
    MIPModeler::MIPExpression1D* getMIPExpression1D(QString aExpressionName);
    MIPModeler::MIPExpression& getMIPExpression1D(uint i, QString aExpressionName);
    QString ObjectiveType(); 
    /* ------------------------------------- */
    virtual void setMIPModel (MIPModeler::MIPModel* aModel) ;                        /** Set pointer to global Optimization Problem Model */

    virtual int defineDefaultVarNames();
    void createOnePort(const QString& portId, const QMap<QString, QString>& portParams);
    QString getUniquePortID();
    QList<MilpPort*> PortList();
    virtual void addPort(MilpPort* lptrport);
    void removePort(MilpPort* lptrport);
    MilpPort* getPort(const QString& portId);
    MilpPort* getPortByName(const QString& aPortName);

    virtual void declareCompoInputParam();
    virtual void setCompoInputParam(const QMap<QString, QString> aComponent);
    virtual void initGuiData();

    virtual void jsonSaveGuiComponent(QJsonArray &componentsArray, const QString& componentCarrier) ;
    void jsonSaveGUINodePortsData(QJsonArray &nodePortsArray, const QString & aSide);
    virtual void jsonSaveGUIlistPortsData(QJsonArray &nodePortArray, const QString& aSide);
    void jsonSaveGUITimeSeries(QJsonArray& paramArray, const InputParam* const inputParam);// = nullptr);
    virtual QList<MilpPort*> listSidePorts(const QString& aside);

    QString ModelName() {return mCompoModelName;}
    QString ModelClassName() { return mCompoModelClassName; }
    void setModelClassName(const QString& modelClass) { mCompoModelClassName = modelClass; }
    GUIData* getGUIData() {return mGUIData;}

    InputParam* getPlugSubmodelIO() {return mPlugSubmodelIO;}
    InputParam* getCompoInputParam(){return mCompoInputParam;}

    SubModel* compoModel() { return mCompoModel; }
    MilpData* getMilpData() {  return mMilpData ; }

    // shortcut access to time data
    double TimeStep(int i) const {return mMilpData->TimeStep(i);}    //TimeStep in HOUR
    std::vector<double> TimeSteps() const {return mMilpData->TimeSteps();}    //TimeStep list in HOUR

    float pdt() const {return mMilpData->pdt() ;}              /** Pas de temps en secondes */
    double pdtHeure() const {return mMilpData->pdtHeure() ;}    /** La meme grandeur mais en heures */
    uint npdtPast() const {return mMilpData->npdtPast() ;}     /** Nombre de pas de temps passe */
    uint npdt() const {return mMilpData->npdt() ;}             /** Nombre de pas de temps (futur) */
    uint npdtTot() const {return mMilpData->npdtTot() ;}       /** Nombre total de pas de temps (passe + futur) */
    uint timeshift() const {return mMilpData->timeshift() ;}    /** Time shifting */
    uint iHMFuturSize () const {return mMilpData->iHMFuturSize() ;}
    uint startingAbsoluteTimeStep () const {return mMilpData->startingAbsoluteTimeStep() ;} /** Starting absolute timestep number for the current optimization */

    virtual void defineMainEnergyVector(); /** set the main carrier of the component **/
    void setEnergyVector(EnergyVector* aptrEnergyVector);    /** Set a pointer to the main carrier of the component */
    EnergyVector* getEnergyVector(); /** Pointer to the main carrier of the component */

    Cairn_Exception  getException () const {return mException;}
    void  setException (const Cairn_Exception &aException) {mException = aException;}
    
    virtual void readTSVariablesFromModel();
    void setTimeSeriesName(const QString& ts_paramName, const QString& ts_name);
    QString getTimeSeriesName(const QString& ts_paramName);
    QMap<QString, QString> getTimeSeriesNames();

    void computeHistNbHours();
 
    virtual std::vector<std::string> get_ModelClassList();
    std::map<QString, InputParam::ModelParam*> getParameters(); //get all the parameters of the componenet

    bool EnvironmentModel();
    bool EcoInvestModel();
    bool isBus();

    int getXpos() const { return mXpos; }
    int getYpos() const { return mYpos; }

    void setXpos(const int& aXpos);
    void setYpos(const int& aYpos);

    void setTecEcoEnvSettings(TecEcoEnv& aTecEcoEnv);
    void updateCompoParamMap(const std::string& a_SettingName, const t_value& a_SettingValue);
    void updateCompoParamMap(const t_dict& a_SettingValues);

    MilpPort* mapDefaultPort(const QString& portId, const QMap<QString, QString>& portParams);

protected:
    Cairn_Exception mException ;
    MIPModeler::MIPModel* mModel ;                      /** Pointer to global Optimization Problem Model */

    // Component settings
    QMap<QString, QString> mComponent ;         /** Map of Topological data from .json */
    QMap < QString, QMap<QString, QString> > mPorts;  /** Map of port data from .json, portName: QMap<QString, QString> */

    SubModel* mCompoModel{ nullptr };                             /** Pointer to global component SubModel of name mCompoModelName*/
    InputParam* mCompoInputParam{ nullptr };      /** COMPONENT Input parameter List from XML File -> Options */
    InputParam* mInputParam{ nullptr };                           /** Pointer to COMPONENT Input Data List (for link with PEGASE or OUTSIDE) */
    InputParam* mCompoToModel{ nullptr };                         /** Pointer to COMPONENT Data List to be sent to SubModel (for link with PEGASE or OUTSIDE) */
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
    QString mName ;                                 /** Component Name */
    QString mCompoModelName ;                       /** Component Model Name to be used eg an Electrolyzer */ 
    QString mCompoTechnoType;                       /** Component TechnoType Name to be used only by GUI for image eg an PVSource */
    QString mCompoModelClassName ;                  /** Component Model Class Name to be used eg a Basic Electrolyzer or SOEC Electrolyzer */
    int mNports ;                                       /** Component number of ports */
    int mNbInputPorts ;                                       /** Component number of Input ports */
    int mNbOutputPorts ;                                       /** Component number of Output ports */
    int mNbDataPorts ;                                       /** Component number of Data exchange ports */

    //technical input
    double mSens ;                                       /** Component flux direction : = 1 if outgoing direction with respect to component, -1 if incoming */
    bool  mIsImposedWeight ;                            /** if true: add constraint setting component weight to imposed value - if false, weight of component - useful to relax on load flux profiles */
 
    int mFirstInit ; /** 0 if firstInit, else */
    int mFirstInitTS ; /** 0 if firstInit, else */
    
    int mXpos;
    int mYpos;
    QString mDirection;
    QString mType;  
    QString mControl;
    QString mDataFile;
    QString mPublishUserVariable;
    QString mSubmodelFile; //Do we still need this?!

    // Time Series
    typedef std::map<QString, ModelTS> t_mapTS;
    t_mapTS m_timeSeries;
    void createImportListVars(t_mapExchange& a_Import);
    void createExportListVars(t_mapExchange& a_Export);
    virtual void createPortsExportListVars(t_mapExchange& a_Exchange);
    void readTSVariables(InputParam* aMapParamTS);

    // compo model    
    class ModelFactory* mModelFactory;
    virtual bool newCompoModel();  
    void createPorts();
};

#endif // MilpComponent_H
