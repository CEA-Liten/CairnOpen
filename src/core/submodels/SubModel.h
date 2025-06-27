#ifndef SubModel_H
#define SubModel_H

class SubModel ;
class MilpComponent;
class MilpPort;

#include <QDebug>
#include <cmath>

#include "MILPComponent_global.h"
#include "Cairn_Exception.h"
#include "GlobalSettings.h"
#include "MIPModeler.h"
#include "InputParam.h"
#include "EnvImpact.h"
#include "ModelVar.h"

using namespace GS;

/**
 * \brief The SubModel class is the virtual class of every MIPModeler Model used by a MilpComponent class object
 * \details Use the following Units, instead of the IS Units leading to "scaling" troubles during solving step
     * Mass Flow    : kg/h
     * Power flow   : MW
     * Mass         : kg
     * Energy       : MWh
     * Time         : Hours
 */
class CAIRNCORESHARED_EXPORT SubModel : public QObject
{
    Q_OBJECT
public:
    SubModel(QObject* aParent=nullptr);
    ~SubModel();

    /**----------------------- Methods that must be instantiated (overridden) in the individual models -------------------------------*/
    virtual void declareModelConfigurationParameters() = 0;
    virtual void declareModelParameters() = 0;       /** MILP Model input parameters to be read from SettingsFile */
    virtual void declareModelInterface() = 0;        /** MILP Model input parameters to be read from SettingsFile */
    virtual void declareModelIndicators() = 0;       /** MILP Model input indicators to be read from SettingsFile */
    virtual void buildModel() = 0;                   /** MILP Model description : variables, cosntraint */
    virtual void buildControlVariables();
    virtual void finalizeModelData();                /** after reading of all parameters, possibly initialize internal data of Models */
    virtual void initDefaultPorts() = 0;
    /**----------------------- Methods that can be overridden in the individual models -------------------------------*/
    virtual void declareInputFluxIOs(MilpPort* defaultPort = nullptr) {}; //overridden in Cogeneration and MultiConverter
    virtual void declareOutputFluxIOs(MilpPort* defaultPort = nullptr) {};//overridden in Cogeneration and MultiConverter
    virtual void setPortPointers() { };
    virtual void computeInitialData() { /** do nothing if not instantiated (overridden ) */ }; /** pre-compute initial state data from input parameters for use by historical vectors for submodel initialization */
    virtual void computeAllContribution() { };
    virtual void computeAllIndicators(const double* optSol);
    virtual int checkUnit(MilpPort* port);
    virtual int defineDefaultVarNames();    
    virtual void setTimeData();
    virtual void setTypicalPeriods(const bool& useTypicalPeriods, const uint& aTypicalPeriods, const uint& aNDtTypicalPeriods, const std::vector<int>& aVectTypicalPeriods);
    virtual bool isSizeOptimized();
    virtual bool isPriceOptimized(); //Only SourceLoad
    virtual int checkConsistency() { return 0; };
    virtual QString ObjectiveType() { return ""; }; 
    virtual void addStateConstraints(const uint64_t& aNpdt, const uint64_t& aCondensedNpdt);
    virtual void addStartUpShutDown(const uint64_t& nPdtCond, const uint64_t& aNpdt); //Technical + Operation 
    virtual uint64_t exprMilpHorizon(); //Technical + Operation 
    virtual uint64_t varMilpHorizon(); //Technical + Operation 

    /**------------- Other methods that are common to all model types : Technical, Bus and Operational ---------------*/
    void exportIndicators(QTextStream& out, QString name, QString range, bool forced, const bool isRollingHorizon);
    void declareDefaultModelConfigurationParameters()
    {
        //bool 
        addParameter("ExportIndicators", &mExportIndicators, true, false, true, "Export component specific indicators", "", { "" });
        //QString
        setPossibleWeightUnits({ "-", "SurfaceUnit", "PeakUnit"}); //Only need to have the list for the GUI
        addParameter("WeightUnit", &mWeightUnit, "-", false, true, "Unit of Weight. By default it has no unit");

        addConfig("ControlType", &mControl, "", false, true, "", "", { "" }); /** Type of control : rolling horizon, mpc, or nothing*/
    }

    void declareDefaultModelInterface(){ 
        addIO("isInstalled", &mExpInstalled, "bool");  /** Binary equals 1 if installed */
    }

    void writeSolution(const double* optimalSolution, std::map<std::string, std::vector<double>>& resultats); /** get optimal solution and set  */

    void setPossibleWeightUnits(const QStringList aPossibleWeightUnits) {
        mPossibleWeightUnits = aPossibleWeightUnits;
    }
    /**--------------------------------- Utility methods used by Milpcomponent --------------------------------------*/
    /** Methods for model parent, name, energyvector and topology */
    QString getCompoName() { return mParentCompo->Name(); }
    void setParentCompo(MilpComponent* aCompo) { mParentCompo = aCompo; }
    void setEnergyVector(EnergyVector* aEnergyVector) { mEnergyVector = aEnergyVector; }
    EnergyVector* getEnergyVector() { return mEnergyVector; }
    //void setTopo(QList<MilpPort*> aListPort) { mListPort = aListPort; }
    void setTopoCompo(QMap<QString, MilpComponent*>  aMapMilpComponents) { mMapMilpComponents = aMapMilpComponents; }

    QList<MilpPort*> PortList() { return mListPort; }
    void addPort(MilpPort* lptrport) { mListPort.push_back(lptrport); }
    void removePort(MilpPort* lptrport);
    void removeBusPort(MilpPort* lptrport);
    MilpPort* getPort(const QString& aPortId);
    MilpPort* getPortByType(const QString& aType, const QString& aDirection="ANY");
    QMap <QString, QMap<QString, QString>> const DefaultPorts() { return mDefaultPorts;  }

    /** Exception */
    Cairn_Exception  getException() const { return mException; }
    void  setException(const Cairn_Exception &aException) { mException = aException; }

    bool ExportIndicators() { return mExportIndicators; }
    bool hasStateVariable() const { return mAddStateVariable; }

    /** Pointer to global Optimization Problem Model */
    MIPModeler::MIPModel* getModel() { return mModel; }
    void setMIPModel(MIPModeler::MIPModel* aModel) { mModel = aModel; }

    /** Add configuration to Model  */
    void addConfig(const QString& aParamName, const t_pvalue &aPtr, t_value aDefaultValue, t_flag aIsBlocking = true, t_flag aIsUsed = true, const QString& aDescription = "", const QString& aUnit = "", const QList<QString>& aconfigList = { "" });

    /** Add parameter to Model  */
    void addParameter(const QString& aParamName, const t_pvalue &aPtr, t_value aDefaultValue, t_flag aIsBlocking = true, t_flag aIsUsed = true, const QString& aDescription = "", const QString& aUnit = "", const QList<QString>& aconfigList = { "" });
    void addParameter(const QString& aParamName, std::vector<double>* aPtr, t_flag aIsBlocking = true, t_flag aIsUsed = true, const QString& aDescription = "", const QString& aUnit = "", const QList<QString>& aconfigList = { "" });

    /** Add PerfParam to model */
    void addPerfParam(const QString& aParamName, const t_pvalue& aPtr, t_value aDefaultValue, t_flag aIsBlocking = true, t_flag aIsUsed = true, const QString& aDescription = "", const QString& aUnit = "", const QList<QString>& aconfigList = { "" });
    void addPerfParam(const QString& aParamName, std::vector<double>* aPtr, t_flag aIsBlocking = true, t_flag aIsUsed = true, const QString& aDescription = "", const QString& aUnit = "", const QList<QString>& aconfigList = { "" });

    /** Add TimeSeries to model */
    void addTimeSeries(const QString& aParamName, std::vector<double>* aDblePtr,        
        t_flag IsBlocking = true, t_flag aIsUsed = true,
        const QString& aDescription = "", const QString& aUnit = "",
        const QList<QString>& aconfigList = { "" },
        double a_default = 1.0, double a_min = std::nan("1"), double a_max = std::nan("1"));
    
 /** Add expression to Model list of IO Interface */   
    void addIO(const QString& aParamName, MIPModeler::MIPExpression* aExprPtr, const QString aUnit, const QString& aCurrency = "");
    void addIO(const QString& aParamName, MIPModeler::MIPExpression1D* aExprPtr1D, const QString aUnit, const QString& aCurrency = "");

    void addIO(const QString& aParamName, MIPModeler::MIPExpression* aExprPtr, const QString* aUnit, const QString& aCurrency = "");
    void addIO(const QString& aParamName, MIPModeler::MIPExpression1D* aExprPtr1D, const QString* aUnit, const QString& aCurrency = "");

    void removeIO(const QString& aName);

/** Add expression to Model list of Rolling Horizon elements */
    void addControlIO(const QString& aParamName, MIPModeler::MIPExpression1D* aExprPtr1D, const QString aUnit, double* aValuePtr, double* aDefaultValue = nullptr, bool a_isMPC = true, const QString& aCurrency = "");
    void addControlIO(const QString& aParamName, MIPModeler::MIPExpression1D* aExprPtr1D, const QString aUnit, std::vector<double>* aHistPtr, double* aDefaultValue = nullptr, bool a_isMPC = true, const QString& aCurrency = "");
    
    void addControlIO(const QString& aParamName, MIPModeler::MIPExpression1D* aExprPtr1D, const QString* aUnit, double* aValuePtr, double* aDefaultValue = nullptr, bool a_isMPC = true, const QString& aCurrency = "");
    void addControlIO(const QString& aParamName, MIPModeler::MIPExpression1D* aExprPtr1D, const QString* aUnit, std::vector<double>* aHistPtr, double* aDefaultValue = nullptr, bool a_isMPC = true, const QString& aCurrency = "");

    typedef std::map<QString, ModelIO*> t_mapIOs;
    const t_mapIOs& getMapIOExpression() { return mIOExpressions; }      /** Get List of Readable Expressions */
    ModelIO* getIOExpression(const QString& aName);
    std::vector<ModelIO*> getIOExpressions(const EIOModelType& aIOType=EIOModelType::eMIPExpression1D);
    MIPModeler::MIPExpression* getMIPExpression(QString aExpressionName);
    MIPModeler::MIPExpression1D* getMIPExpression1D(QString aExpressionName);
    MIPModeler::MIPExpression& getMIPExpression1D(uint i, QString aExpressionName);
    void dumpIOExpressionList();
    void dumpIOExpression1DList();

    typedef std::map<QString, ControlVar*> t_mapRHs;
    const t_mapRHs& getListControlIO() { return mListControlIO; }    /** Get List of Readable Vector of rolling horizon Expressions */

    void fillExpression(MIPModeler::MIPExpression1D& aExpress1D, MIPModeler::MIPVariable1D& aVariable);

    static void closeExpression(MIPModeler::MIPExpression& aExpress);
    static void closeExpression1D(MIPModeler::MIPExpression1D& aExpress1D);

    void cleanExpression();

    /** Computation methods */
    void computeTime(bool bsetValue, uint aNpdt, uint aShift, MIPModeler::MIPExpression1D exp, const double* optSol, double& ret);
    void computeTime(bool bsetValue, uint aNpdt, uint aShift, MIPModeler::MIPExpression1D exp, const double* optSol, double& retCharged, double& retDischarged);
    void computeProduction(bool bsetValue, uint aNpdt, uint aShift, MIPModeler::MIPExpression1D exp, const double* optSol, const double& aCoeff, const double& bCoeff, double& aProduction, const bool& aTimeIntegration = true);
    void computeProduction(bool bsetValue, uint aNpdt, uint aShift, MIPModeler::MIPExpression1D exp, const double* optSol, const double& aCoeff, const double& bCoeff, double& retCharged, double& retDischarged);
    void computeConsumption(bool bsetValue, uint aNpdt, uint aShift, MIPModeler::MIPExpression1D exp, const double* optSol, const double& aCoeff, const double& bCoeff, double& aConsumption);
    void computeLvlConsumption(bool bsetValue, uint aNpdt, uint aShift, MIPModeler::MIPExpression1D exp, const double* optSol, const double& aCoeff, const double& bCoeff, double& aConsumption);
    void computeLvlProduction(bool bsetValue, uint aNpdt, uint aShift, MIPModeler::MIPExpression1D exp, const double* optSol, const double& aCoeff, const double& bCoeff, double& aProduction);
    void computeLvlProduction(bool bsetValue, uint aNpdt, uint aShift, MIPModeler::MIPExpression1D exp, const double* optSol, const double& aCoeff, const double& bCoeff, double& retCharged, double& retDischarged);
    void computeLvlImpact(bool bsetValue, uint aNpdt, uint aShift, MIPModeler::MIPExpression1D exp, const double* optSol, const double& aCoeff, const double& bCoeff, double& aProduction);
    void computeDiscounted(uint aNpdt, uint aShift, MIPModeler::MIPExpression1D exp, const double* optSol, double& aDiscounted);

    //computes an indicator (PLAN and HIST values) from an 1D-Expression
    void computeIndicator(const MIPModeler::MIPExpression1D& exp, const double* optSol, double& aUnDiscounted, double& aDiscounted, double& aHistUnDiscounted, double& aHistDiscounted, bool isEnvImpact=false);

    /** Model parameters */
    InputParam* getInputParam() { return mInputParam; }  /** Get access to Model Parameters */
    InputParam* getInputPerfParam() { return mInputPerfParam; }  /** Get access to Model Performance Parameters */
    InputParam* getInputData() { return mInputData; }    /** Get access to Model Data */
    InputParam* getInputDataTS() { return mInputDataTS; }    /** Get access to Model Data */
    
    InputParam* getInputIndicators() { return mInputIndicators; }  /** Get access to Model Parameters */
    InputParam* getInputEnvImpactsParam() { return mInputEnvImpacts; }  /** Get access to Model Parameters */
    InputParam* getInputPortImpactsParam() { return mInputPortImpacts; }
    InputParam* getInputPortImpactsParamTS() { return mTSInputPortImpacts; }

    /** Expressions */
    QString getOptimalSizeExpression() { return mOptimalSizeExpression; }
    void   setOptimalSizeExpression(QString aExpressionName) { mOptimalSizeExpression = aExpressionName; }

    void    setVariableCostsExpression(QString aExpressionName) { mVariableCostsExpression = aExpressionName; }
    QString getVariableCostsExpression() { return mVariableCostsExpression; }

    void    setCapexExpression(QString aExpressionName) { mCapexExpression = aExpressionName; }
    QString getCapexExpression() { return mCapexExpression; }

    void    setOpexExpression(QString aExpressionName) { mOpexExpression = aExpressionName; }
    QString getOpexExpression() { return mOpexExpression; }

    void    setPureOpexExpression(QString aExpressionName) { mPureOpexExpression = aExpressionName; }
    QString getPureOpexExpression() { return mPureOpexExpression; }

    void    setReplacementExpression(QString aExpressionName) { mReplacementExpression = aExpressionName; }
    QString getReplacementExpression() { return mReplacementExpression; }

    void    setEnvImpactCostExpression(QString aExpressionName) { mEnvImpactCostExpression.push_back(aExpressionName); }
    QString getEnvImpactCostExpression(int i) { return mEnvImpactCostExpression.at(i); }

    void    setEnvImpactMassExpression(QString aExpressionName) { mEnvImpactMassExpression.push_back(aExpressionName); }
    QString getEnvImpactMassExpression(int i) { return mEnvImpactMassExpression.at(i); }

    void    setEnvGreyImpactCostExpression(QString aExpressionName) { mEnvGreyImpactCostExpression.push_back(aExpressionName); }
    QString getEnvGreyImpactCostExpression(int i) { return mEnvGreyImpactCostExpression.at(i); }

    void    setEnvGreyImpactMassExpression(QString aExpressionName) { mEnvGreyImpactMassExpression.push_back(aExpressionName); }
    QString getEnvGreyImpactMassExpression(int i) { return mEnvGreyImpactMassExpression.at(i); }

    void    setPenaltyConstraintExpression(QString aExpressionName) { mPenaltyConstraintExpression = aExpressionName; }
    QString getPenaltyConstraintExpression() { return mPenaltyConstraintExpression; }

    void    setSubobjectiveExpression(QString aExpressionName) { mSubObjectiveExpression = aExpressionName; }
    QString getSubobjectiveExpression() { return mSubObjectiveExpression; }

    /** Env Impact Methods */
    void setEnvImpactsList(QStringList aEnvImpactsList) { mEnvImpactsList = aEnvImpactsList; }
    void setEnvImpactsShortNamesList(QStringList aEnvImpactsShortNamesList) { mEnvImpactsShortNamesList = aEnvImpactsShortNamesList; }
    void setEnvImpactUnitsList(QStringList aEnvImpactUnitsList) { mEnvImpactUnitsList = aEnvImpactUnitsList; }
    void setEnvImpactCosts(std::vector<double> aEnvImpactCosts) { mEnvImpactCosts = aEnvImpactCosts; }

    double* envGreyImpactMassContribution(const int aIdxEnvImpact) { return mEnvImpacts.at(aIdxEnvImpact)->getEnvGreyImpactMass(); }
    double* envGreyImpactCostContribution(const int aIdxEnvImpact) { return mEnvImpacts.at(aIdxEnvImpact)->getEnvGreyImpactPart(); }
    double* envImpactCostContribution(const int aIdxEnvImpact) { return mEnvImpacts.at(aIdxEnvImpact)->getEnvImpactPartPLAN(); }
    double* envHistImpactCostContribution(const int aIdxEnvImpact) { return mEnvImpacts.at(aIdxEnvImpact)->getEnvImpactPartHIST(); }
    double* envImpactCostContributionDiscounted(const int aIdxEnvImpact) { return mEnvImpacts.at(aIdxEnvImpact)->getEnvImpactPartDiscountedPLAN(); }
    double* envHistImpactCostContributionDiscounted(const int aIdxEnvImpact) { return mEnvImpacts.at(aIdxEnvImpact)->getEnvImpactPartDiscountedHIST(); }

    double* envImpactMassContribution(const int aIdxEnvImpact) { return mEnvImpacts.at(aIdxEnvImpact)->getEnvImpactMassPLAN(); }
    double* envHistImpactMassContribution(const int aIdxEnvImpact) { return mEnvImpacts.at(aIdxEnvImpact)->getEnvImpactMassHIST(); }
    double* envImpactMassContributionDiscounted(const int aIdxEnvImpact) { return mEnvImpacts.at(aIdxEnvImpact)->getEnvImpactMassDiscountedPLAN(); }
    double* envHistImpactMassContributionDiscounted(const int aIdxEnvImpact) { return mEnvImpacts.at(aIdxEnvImpact)->getEnvImpactMassDiscountedHIST(); }

    //MIPModeler::MIPExpression1D* getExpEnvMass(const int aIdxEnvImpact) { return mEnvImpacts.at(aIdxEnvImpact)->getExpEnvMass(); }
    //MIPModeler::MIPExpression* getExpEnvGreyMass(const int aIdxEnvImpact) { return mEnvImpacts.at(aIdxEnvImpact)->getExpEnvGreyMass(); }

    /** TimeStep management */
    inline double TimeStep(uint i) { return mTimeSteps[i]; }                                       /** List of timesteps */
    std::vector<double> timesteps() { return mTimeSteps; }
    void setNpdtPast(uint i) { mNpdtPast = i; }                                       /** List of timesteps */
    void setAbsoluteTimeStep(const uint* i) { mptrAbsoluteTimeStep = i; }
    void setTimeshift(const uint* i) { mptrTimeshift = i; }
    void setFuturesize(const uint* i) { mptrFuturesize = i; }
    uint npdtPast() { return mNpdtPast; }
    

    /** Util Methods */
    bool getAllocate() const { return mAllocate; }
    void setTimeSteps(std::vector<double> aTimeSteps, uint64_t aTimeStepBeginLP, uint64_t aTimeStepBeginForecast, uint64_t aDecreaseOptimizationHorizon);   /** TimeStep settings */    
    QString convertUnits(EnergyVector* ptrEnergyVector, const QList<QString> &aUnit);
    QString getUnit(const QString &aExpressionName);
    void decreaseOptimizationHorizon();                /** Update mTimeSteps */
    std::vector<double> getOptimalSizeAllCycles() { return mOptimalSizeAllCycles; }

    void addVariable(MIPModeler::MIPVariable0D& variable0D, const std::string& name);
    void addVariable(MIPModeler::MIPVariable1D& variable1D, const std::string& name);
    void addVariable(MIPModeler::MIPVariable2D& variable2D, const std::string& name);
    void addConstraint(MIPModeler::MIPConstraint constraint, const std::string& name, const uint& t=0);

    std::string CName(const std::string aRadical, const uint& t) const
    {
        std::string aname = "c" + aRadical + parent()->objectName().toStdString() + std::to_string(t);
        return aname;
    }

    std::string CName(std::string aRadical) const
    {
        std::string aname = "v" + aRadical + parent()->objectName().toStdString();
        return aname;
    }

    void setMaxBound(const double& aMaxVal);     // set maxBound = maxPower * weight
    void setMinBound(const double& aMaxVal);     // set minBound = minPower * weight
    void setVarSizeMax(const double& aMaxVal);   // define upper bound for sizeMax variable ie weight or maxPower
    void setExpSizeMax(const double& aMaxVal, const std::string& aStrName);

    double& getMaxBound() {
        if (mMaxBound == -1) {
            qCritical() << "ERROR : submodel" << parent()->objectName() << "maxbound not defined";
        }
        return mMaxBound;
    }
    double& getMinBound() {
        if (mMinBound == -1) {
            qCritical() << "ERROR : submodel" << parent()->objectName() << "minBound not defined";
        }
        return mMinBound;
    }

    bool isIndicatorNameUnique(MilpPort* targetPort, QString quantityName = "");
    void resetFlags() {
        mAllocate = true;
    }

    QString Currency() const { return mCurrency; }
    void setCurrency(const QString& aCurrency) { mCurrency = aCurrency; }

    QString getOptimalSizeUnit() const;
    void setOptimalSizeUnit(const QString aSizeUnit);
    void setOptimalSizeUnit(const QString* paSizeUnit);
    
private:
    /** Unit of Optimal size variable */
    QString m_OptimalSizeUnit;                 
    const QString* p_OptimalSizeUnit;  

protected:
    int checkBusSameValueVarName(MilpPort* port);
    virtual int checkBusFlowBalanceVarName(MilpPort* port, int &inumberchange, QString &varUseCheck);
    virtual bool defineDefaultVarNames(MilpPort* port);

    QString mCurrency;
    QString convertUnit(EnergyVector* ptrEnergyVector, QString aUnit);

    Cairn_Exception mException;

    /** Pointers **/
    MIPModeler::MIPModel* mModel;      /** Pointer to global MIPModel */
    MilpComponent* mParentCompo;       /** Parent Component wearing the submodel */
    EnergyVector* mEnergyVector;       /** Equals to parent EnergyVector (taken from linked Bus) ! Why having this ! */
    QMap<QString, MilpComponent*>   mMapMilpComponents;   // pointeurs des composants MILPs du probleme

    /** Ports */
    QMap <QString, QMap<QString, QString>> mDefaultPorts;
    QList<MilpPort*> mListPort;        /** List of connexion MilpPort of Component  */
    bool mVariablePortNumber;          /** Model can have variable number of physical inlet and outlet - they must be <= number of total NbInputPorts or outputs */
    int mNbInputPorts;                 /** Number of Componen Input Ports */
    int mNbOutputPorts;                /** Number of Component Outputs Ports */
    int mNbInputFlux;                  /** Number of first Component NbInputPorts dedicated to Flux at inlet */
    int mNbOutputFlux;                 /** Number of first Component Outputs dedicated to Flux at inlet */

    /** Model IO Interface */
    t_mapIOs mIOExpressions; /** Model List of Readable Expressions */
    /** Model Rolling Horizon interface */
    t_mapRHs mListControlIO; /** Model List of variable that should remain in memory for rolling horizon */
   
    /** Expressions */
    MIPModeler::MIPExpression   mExpSizeMax;           /** Optimal capacity expression */
    MIPModeler::MIPExpression   mExpInstalled;         /** Binary equals 1 if installed */
    MIPModeler::MIPExpression1D mExpState;
    MIPModeler::MIPExpression1D mExpStartUp;
    MIPModeler::MIPExpression1D mExpShutDown;

    /** MIPModel Variables */
    MIPModeler::MIPVariable0D mVarInstalled;
    MIPModeler::MIPVariable0D mVarWeight;
    MIPModeler::MIPVariable0D mVarSizeMax;      /** MILP 0D Variable on component Optimal capacity (power or storage) */
    MIPModeler::MIPVariable1D mStartUp;
    MIPModeler::MIPVariable1D mShutDown;
    MIPModeler::MIPData1D mHistState;
    MIPModeler::MIPVariable1D mState;

    /** Expression Names TODO: use hard-coded names e.g. const QString CapexExpName = "Capex" */ 
    QString mOptimalSizeExpression;           /** Name of expression to be used for OptimalSize evaluation */
    QString mSubObjectiveExpression;          /** Name of expression to be used for Subobjective evaluation */
    QString mCapexExpression;                 /** Name of expression to be used for Capex evaluation */
    QString mOpexExpression;                  /** Name of expression to be used for Opex evaluation */
    QString mPureOpexExpression;              /** Name of expression to be used for PureOpex evaluation */
    QString mReplacementExpression;           /** Name of expression to be used for Replacement evaluation */
    QString mVariableCostsExpression;         /** Name of expression to be used for VariableCosts evaluation */
    QString mPenaltyConstraintExpression;     /** Name of expression to be used for Penalty evaluation */
    QStringList mEnvGreyImpactCostExpression;       /** Name of expression to be used for EnvGreyImpactCost evaluation */
    QStringList mEnvGreyImpactMassExpression;       /** Name of expression to be used for EnvGreyImpactMass evaluation */
    QStringList mEnvImpactCostExpression;           /** Name of expression to be used for EnvImpactCost evaluation */
    QStringList mEnvImpactMassExpression;           /** Name of expression to be used for EnvImpactMass evaluation */

    /** Flags */
    bool mAllocate;           /** Allocation flag to prevent from multiple allocations and memory leaks during rolling horizon process */
    bool mExportIndicators;   /** bool indicating the export of component specific indicators if = true*/
    bool mAddStateVariable;   /** bool to add state variables like ON / OFF */
    bool mUseAgeing;          /** bool indicating the use of an ageing model if true - default to false */

    QString mControl;                          /** Control type taken into account : */

    /** InputParam */
    InputParam* mInputParam;                   /** Parameters are MILP constant parameters comming from Settings File */
    InputParam* mInputPerfParam;               /** Performance Parameters are MILP constant vectors comming from DataFile csv File */
    InputParam* mInputData;                    /** Data are MILP constant (scalar or vectors) comming from Description files */
    InputParam* mInputDataTS;                  /** Data are MILP time series comming from Description files */
   
    InputParam* mInputIndicators;              /** Indicators that the user wants to export */
    InputParam* mInputEnvImpacts;              /** Environmental impacts that the user wants to compute */
    InputParam* mInputPortImpacts;             /** Port Environmental impacts that the user wants to compute */
    InputParam* mTSInputPortImpacts;           /** Timeseries of Port Environmental impacts */
   
    /** Weight */ //TODO: move mLPModelOnly to TechnicalSubModel and mLPWeightOptimization to TechnicalSubModel or SourceLoadSubModel ?
    double mWeight;
    bool mUseWeightOptimization;          /** bool indicating use sizing based on Weight if = true - default to false*/
    bool mLPWeightOptimization;           /** bool indicating use LP sizing if Weight if = true - default to false*/ 
    bool mLPModelOnly;                    /** bool indicating use of LPModelOnly if = true - default to false*/
    QString mWeightUnit;
    QStringList mPossibleWeightUnits;     /** List of possible weight units */
    double mMaxBound;                     /** Upper bound of siz equal to weight * maxval */
    double mMinBound;                     /** Lower bound of siz equal to weight * minval */

    /** Timestep management */
    std::vector<double> mTimeSteps;         /** vector of timesteps (variable or constant timestep on planning horizon)*/
    uint64_t mHorizon;                      /** for convenience in all submodels horizon name mHorizon : number of timesteps (size of mTimeSteps) */
    uint mNpdtPast;                         /** Number of past timestep */
    uint64_t mTimeStepBeginLP;              /** first timestep of the LP model / if timestep lower, use MILP model, else use LP */
    uint64_t mTimeStepBeginForecast;        /** first timestep of the LP model / if timestep lower, use MILP model, else use LP */
    uint64_t mMilpNpdt;                     /** Number of timesteps using MILP scheme instead of LP */
    uint64_t mDecreaseOptimizationHorizon;  /** activate the decreasing optimization horizon if equal to 1 */
    const uint* mptrAbsoluteTimeStep;       /** pointer to current absolute time step */
    const uint* mptrTimeshift;              /** pointer to timeShift */
    const uint* mptrFuturesize;             /** pointer to futuresize */

    /** Typical Periods management */
    bool mUseTypicalPeriods;
    std::vector<int> mVectTypicalPeriods; //Vector of Timesteps over planning horizon. Past timesteps use small timestep
    uint mTypicalPeriods;                // number of typical period eg number of typical days, weeks...
    uint mNDtTypicalPeriods;            // number of timesteps in one typical period : 24 hours...
    uint mCondensedNpdt;               // number of condensed timesteps resulting from typical period use
    std::vector<int> mFullVectTypicalPeriods; //Vector of Timesteps over planning horizon. Past timesteps use small timestep

    /** Economic */ 
    double mHistVariableCostsDiscounted;

    /** Env Impacts */
    std::vector<class EnvImpact*> mEnvImpacts;
    QStringList mEnvImpactsList;                            /** List of the considered environmental impacts */
    QStringList mEnvImpactsShortNamesList;             /** List of the considered environmental impacts short names*/
    QStringList mEnvImpactUnitsList;                /** List of the units of the considered environmental impacts */
    std::vector<double> mEnvImpactCosts;

    std::vector<double> mOptimalSizeAllCycles; //Size = Number of cycles; PLAN from each cycle

    /** if condense variables on typical periods */
    bool mCondenseVariablesOnTP;
    bool mCondenseBinariesOnly;
    bool mActivateConstraintsBetweenTP;
    int mAbsInitialState;
};

#endif // SubModel_H