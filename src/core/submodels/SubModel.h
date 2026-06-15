#ifndef SubModel_H
#define SubModel_H

class MilpPort;

#include "CairnCore_global.h"
#include "Cairn_Exception.h"
#include "GlobalSettings.h"

#include "MIPModeler.h"
#include "MilpComponent.h"
#include "InputParam.h"
#include "EnvImpact.h"
#include "ModelVar.h"

#include <unordered_set>
#include <unordered_map>
#include <cmath>

/**
 * \brief The SubModel class is the virtual class of every MIPModeler Model used by a MilpComponent class object
 * \details Use the following Units, instead of the IS Units leading to "scaling" troubles during solving step
     * Mass Flow    : kg/h
     * Power flow   : MW
     * Mass         : kg
     * Energy       : MWh
     * Time         : Hours
 */

extern std::string CAIRNCORESHARED_EXPORT indicatorName(SubModel* ap_Model, MilpPort* ap_Port, const std::vector<std::string>& a_NameParts);

namespace Constants {
    inline const std::string VARIABLE = "variable";
    inline const std::string STORAGE_NAME = "storagename";
    inline const std::string FLUX_NAME = "fluxname";
    inline const std::string DIRECTION = "direction";
}

struct SExpression1D {
    MIPModeler::MIPExpression1D* pExp1D{ nullptr };
    int* pSize{ nullptr };
};

using namespace GS;
using namespace Constants;

class CAIRNCORESHARED_EXPORT SubModel : public CairnObject
{
    
public:
    SubModel(CairnObject* aParent = nullptr);
    ~SubModel();

    std::string ModelClassName() const;

    /**----------------------- Methods that must be instantiated (overridden) in the individual models -------------------------------*/
    virtual void declareModelConfigurationParameters() = 0;
    virtual void declareModelParameters() = 0;    /** MILP Model input parameters to be read from SettingsFile */
    virtual void declareModelInterface() = 0;     /** MILP Model input parameters to be read from SettingsFile */
    virtual void declareModelIndicators() = 0;    /** MILP Model input indicators to be read from SettingsFile */
    virtual void initDefaultPorts() = 0;

    // buildModel is overridden in TechnicalSubModel, BusSubModel, and OperationalSubModel
    // So, it should not be defined for a model. Instead, computeModelContribution should be filled for a model.
    virtual void buildModel() = 0;                /** MILP Model description : variables, cosntraint */

    /**----------------------- Methods that can be overridden in the individual models -------------------------------*/
    virtual void computeInitialData() {
        /* computeInitialData is used to initialize some important variables.
        * In particular, it is mandatory to call setMaxValue(?) when mExpSizeMax is declared using addSizeMaxIO(...):
        * - Usually the case for TechnicalSubModel with some exceptions, e.g, NeuralNetwork
        * - Possible for OperationalConstraintSubModel, e.g. Ramp
        * - Not used for BusSubModel
        * 
        * setMinValue(?) should also called inside computeInitialData 
        * 
        * Set flags mAddStateVariable and mAddStartUpShutDownVariable
        * 
        * It is also used to pre-compute initial state data for addControlIO(...)
        */
    };

    int checkPorts();
    virtual int checkConsistency();

    virtual void buildControlVariables();
    virtual void resetHistStoredVaues() {};
    virtual void declareInputFluxIOs(MilpPort* defaultPort = nullptr) {}; //overridden in Cogeneration and MultiConverter
    virtual void declareOutputFluxIOs(MilpPort* defaultPort = nullptr) {};//overridden in Cogeneration and MultiConverter
    virtual void setPortPointers() { };
    virtual void computeModelContribution() { }; /* Compute Model particular Contribution */
    virtual void computeAllIndicators(const double* optSol);
    virtual int defineDefaultVarNames();
    virtual void setTimeData();
    virtual void setTypicalPeriods(const bool& useTypicalPeriods, const uint& aTypicalPeriods, const uint& aNDtTypicalPeriods, const std::vector<int>& aVectTypicalPeriods);
    virtual bool isSizeOptimized();
    virtual bool isPriceOptimized(); //Only SourceLoad
    virtual std::string ObjectiveType() { return ""; };
    virtual uint64_t exprMilpHorizon(); //Technical + Operation 
    virtual uint64_t varMilpHorizon(); //Technical + Operation 
    /**------------- Other methods that are common to all model types : Technical, Bus and Operational ---------------*/
    virtual void declareInputParams(const std::string& name);
    void deleteInputParams();
    void resetIndicators();
    void exportIndicators(std::fstream& out, std::string name, const std::string &range, const std::vector< std::string >& refLabelList, 
        const bool showDescription, bool forced, const bool isRollingHorizon);
    void removeImpactExpressionName(const std::string& impactName);
    void deleteEnvImpacts();

    //Technical + Operation 
    void addStateConstraints(const uint64_t& aCondensedNpdt, const MIPModeler::MIPExpression& aExpInstalled = MIPModeler::MIPExpression(1));
    void addStartUpShutDown(const uint64_t& aCondensedNpdt, const MIPModeler::MIPExpression& aExpInstalled = MIPModeler::MIPExpression(1));

    virtual void declareDefaultModelConfigurationParameters()
    {
        //bool 
        addParameter("ExportIndicators", &mExportIndicators, true, false, true, "Export component specific indicators", "");
        //std::string
        addParameter("WeightUnit", &mWeightUnit, "", false, true, "Unit of Weight. By default it has no unit");
    }

    virtual void declareDefaultModelInterface()
    {
        /* Register IO expressions to be exported (published) as results (to the external, e.g., Pegase)  
           Note, the size of 1D IO expressions is always equal to mHorizon
        */
        //..

        /* Register non-IO 0D-expressions in order to automatically allocate and close them */
        // no 0D expression needs to be declared here

        /* Register non-IO 1D-expressions in order to automatically allocate and close them */
        // no 1D expression needs to be declared here
    }

    void writeSolution(const double* optimalSolution, std::map<std::string, std::vector<double>>& resultats); /** get optimal solution and set  */

    void setPossibleWeightUnits(const std::vector<std::string> aPossibleWeightUnits) {
        mPossibleWeightUnits = aPossibleWeightUnits;
    }
    /**--------------------------------- Utility methods used by Milpcomponent --------------------------------------*/
    /** Methods for model parent, name, energyvector and topology */
    virtual double Sens() { return 0; }; /* To be overridden in Grid and SourceLoad*/
    void setControlType(const std::string& aControl) { mControl = aControl; }
    std::string Name() const { return this->parent()->objectName(); }
    void setParentCompo(MilpComponent* aCompo) { mParentCompo = aCompo; }

    virtual void defineMainCarrier() { /** do nothing: to be defined in individual models */ }; 
    void setMainCarrier(EnergyVector* aEnergyVector) { mMainCarrier = aEnergyVector; }
    EnergyVector* getMainCarrier() const { return mMainCarrier; }

    void setPortList(const std::vector<MilpPort*> aListPort) { mListPort = aListPort; }
    const std::vector<MilpPort*> &PortList() { return mListPort; }
    void addPort(MilpPort* lptrport) { mListPort.push_back(lptrport); }
    void removePort(MilpPort* lptrport);
    void removeBusPort(MilpPort* lptrport);
    MilpPort* getPort(const std::string& aPortId);
    MilpPort* getPortByType(const std::string& aType, const std::string& aDirection = "ANY");
    std::map <std::string, std::map<std::string, std::string>> const DefaultPorts() { return mDefaultPorts; }
    std::map<std::string, std::string> getDefaultPortData(const std::string& portId) const;

    /** Exception */
    Cairn_Exception  getException() const { return mException; }
    void  setException(const Cairn_Exception& aException) { mException = aException; }

    bool ExportIndicators() { return mExportIndicators; }

    /** Pointer to global Optimization Problem Model */
    MIPModeler::MIPModel* getModel() { return mModel; }
    void setMIPModel(MIPModeler::MIPModel* aModel) { mModel = aModel; }

    /** Add parameter to Model  */
    void addParameter(const std::string& aParamName, const t_pvalue& aPtr, t_value aDefaultValue, t_flag aIsBlocking = true, t_flag aIsUsed = true, 
        const std::string& aDescription = "", const t_unit& aUnit = "", const std::string& aShowConfig = "Base");

    /** Add TimeSeries to model */
    void addTimeSeries(const std::string& aParamName, std::vector<double>* aDblePtr,
        t_flag IsBlocking = true, t_flag aIsUsed = true,
        const std::string& aDescription = "", const t_unit& aUnit = "",
        const std::string& aShowConfig = "Base",
        double a_default = 1.0, double a_min = std::nan("1"), double a_max = std::nan("1"));

    /** Add PerfParam to model */
    void addPerfParam(const std::string& aParamName, std::vector<double>* aPtr, t_flag aIsBlocking = true, t_flag aIsUsed = true, 
        const std::string& aDescription = "", const t_unit& aUnit = "");

    /** Add expression to Model list of IO Interface */
    void addIO(const std::string& aIOName, MIPModeler::MIPExpression* aExprPtr, t_flag aIsUsed, const t_unit& aUnit, const std::string& aDescription = "");
    void addIO(const std::string& aIOName, MIPModeler::MIPExpression1D* aExprPtr1D, t_flag aIsUsed, const t_unit& aUnit, const std::string& aDescription = "");

    void addSizeMaxIO(const std::string& aIOName, MIPModeler::MIPExpression* aExprPtr, t_flag aIsUsed, const std::string& aUnit, const std::string& aDescription = "");
    void addSizeMaxIO(const std::string& aIOName, MIPModeler::MIPExpression* aExprPtr, t_flag aIsUsed, const std::string* pUnit, const std::string& aDescription = "");

    bool assertIONonExistence(const std::string& name, const t_pExpr expression); /* throw an error if an IO with given name but different expression already exist (and vice versa)*/
    void assertIsSizeMaxExp(MIPModeler::MIPExpression* aExprPtr); /* throw an error if a given pointer doesn't point to the expression mExpSizeMax */
    void assertIsNotSizeMaxExp(MIPModeler::MIPExpression* aExprPtr); /* throw an error if a given pointer points to the expression mExpSizeMax */

    /** Add expression to Model list of Rolling Horizon elements */
    void addControlIO(const std::string& aIOName, MIPModeler::MIPExpression1D* aExprPtr1D, t_flag aIsUsed, const t_unit& aUnit, 
        double* aValuePtr, double* aDefaultValue = nullptr, bool a_isMPC = true, const std::string& aDescription = "");
    void addControlIO(const std::string& aIOName, MIPModeler::MIPExpression1D* aExprPtr1D, t_flag aIsUsed, const t_unit& aUnit, 
        std::vector<double>* aHistPtr, double* aDefaultValue = nullptr, bool a_isMPC = true, const std::string& aDescription = "");

    void removeIO(const std::string& aName);
    void removeEnvImpactIOs(const std::string& aImpactName);
    void removeIOs();

    typedef std::map<std::string, ModelIO*> t_mapIOs;
    const t_mapIOs& getMapIOExpression() { return mIOExpressions; }      /** Get List of Readable Expressions */
    ModelIO* getIOExpression(const std::string& aName) const;
    std::vector<ModelIO*> getIOExpressions(const EIOModelType& aIOType = EIOModelType::eMIPExpression1D);
    MIPModeler::MIPExpression* getMIPExpression(std::string aExpressionName) const;
    MIPModeler::MIPExpression1D* getMIPExpression1D(std::string aExpressionName) const;
    MIPModeler::MIPExpression& getMIPExpression1D(uint i, std::string aExpressionName);
    void dumpIOExpressionList() const;
    void dumpIOExpression1DList() const;

    const std::vector<MIPModeler::MIPExpression*>& getListExpression0D() { return mExpressions0D; }
    void addExp(MIPModeler::MIPExpression* aExprPtr) { mExpressions0D.push_back(aExprPtr); };

    const std::vector<SExpression1D>& getListExpression1D() { return mExpressions1D; }
    void addExp(MIPModeler::MIPExpression1D* aExprPtr1D, int* aSize) { mExpressions1D.push_back({ aExprPtr1D, aSize }); };

    typedef std::map<std::string, ControlVar*> t_mapRHs;
    const t_mapRHs& getListControlIO() { return mListControlIO; }    /** Get List of Readable Vector of rolling horizon Expressions */

    void fillExpression(MIPModeler::MIPExpression1D& aExpress1D, MIPModeler::MIPVariable1D& aVariable);

    static void closeExpression(MIPModeler::MIPExpression& aExpress);
    static void closeExpression1D(MIPModeler::MIPExpression1D& aExpress1D);

    void allocateExpressions();
    virtual void closeExpressions();

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
    void computeIndicator(const MIPModeler::MIPExpression1D& exp, const double* optSol, double& aUnDiscounted, double& aDiscounted, double& aHistUnDiscounted, double& aHistDiscounted, bool isEnvImpact = false);

    /** Model parameters */
    InputParam* getInputParam() { return mInputParam; }  /** Get access to Model Parameters */
    InputParam* getInputPerfParam() { return mInputPerfParam; }  /** Get access to Model Performance Parameters */
    InputParam* getInputTimeSeries() { return mInputTimeSeries; }    /** Get access to Model Data */

    InputParam* getInputIndicators() { return mInputIndicators; }  /** Get access to Model Parameters */
    InputParam* getInputEnvImpactsParam() { return mInputEnvImpacts; }  /** Get access to Model Parameters */
    InputParam* getInputPortImpactsParam() { return mInputPortImpacts; }
    InputParam* getInputPortImpactsParamTS() { return mTSInputPortImpacts; }

    /** Expressions */
    std::string getOptimalSizeExpression() { return mOptimalSizeExpression; }

    void setVariableCostsExpression(std::string aExpressionName) { mVariableCostsExpression = aExpressionName; }
    std::string getVariableCostsExpression() { return mVariableCostsExpression; }

    void setCapexExpression(std::string aExpressionName) { mCapexExpression = aExpressionName; }
    std::string getCapexExpression() { return mCapexExpression; }

    void setOpexExpression(std::string aExpressionName) { mOpexExpression = aExpressionName; }
    std::string getOpexExpression() { return mOpexExpression; }

    void setReplacementExpression(std::string aExpressionName) { mReplacementExpression = aExpressionName; }
    std::string getReplacementExpression() { return mReplacementExpression; }

    void setEnvImpactCostExpression(std::string aExpressionName) { mEnvImpactCostExpression.push_back(aExpressionName); }
    std::string getEnvImpactCostExpression(int i) { return mEnvImpactCostExpression.at(i); }

    void setEnvImpactMassExpression(std::string aExpressionName) { mEnvImpactMassExpression.push_back(aExpressionName); }
    std::string getEnvImpactMassExpression(int i) { return mEnvImpactMassExpression.at(i); }

    void setEnvGreyImpactCostExpression(std::string aExpressionName) { mEnvGreyImpactCostExpression.push_back(aExpressionName); }
    std::string getEnvGreyImpactCostExpression(int i) { return mEnvGreyImpactCostExpression.at(i); }

    void setEnvGreyImpactMassExpression(std::string aExpressionName) { mEnvGreyImpactMassExpression.push_back(aExpressionName); }
    std::string getEnvGreyImpactMassExpression(int i) { return mEnvGreyImpactMassExpression.at(i); }

    void    setPenaltyConstraintExpression(std::string aExpressionName) { mPenaltyConstraintExpression = aExpressionName; }
    std::string getPenaltyConstraintExpression() { return mPenaltyConstraintExpression; }

    void    setSubobjectiveExpression(std::string aExpressionName) { mSubObjectiveExpression = aExpressionName; }
    std::string getSubobjectiveExpression() { return mSubObjectiveExpression; }

    /** Env Impact Methods */
    void setEnvImpactsList(std::vector<std::string> aEnvImpactsList) { mEnvImpactsList = aEnvImpactsList; }
    void setEnvImpactsShortNamesList(std::vector<std::string> aEnvImpactsShortNamesList) { mEnvImpactsShortNamesList = aEnvImpactsShortNamesList; }
    void setEnvImpactUnitsList(std::vector<std::string> aEnvImpactUnitsList) { mEnvImpactUnitsList = aEnvImpactUnitsList; }
    void setEnvImpactCosts(std::vector<double> aEnvImpactCosts) { mEnvImpactCosts = aEnvImpactCosts; }

    std::vector<class EnvImpact*> getEnvImpacts() { return mEnvImpacts; }

    double* envGreyImpactMassContribution(const int aIdxEnvImpact);
    double* envGreyImpactCostContribution(const int aIdxEnvImpact);
    double* envImpactCostContribution(const int aIdxEnvImpact);
    double* envHistImpactCostContribution(const int aIdxEnvImpact);
    double* envImpactCostContributionDiscounted(const int aIdxEnvImpact);
    double* envHistImpactCostContributionDiscounted(const int aIdxEnvImpact);

    double* envImpactMassContribution(const int aIdxEnvImpact);
    double* envHistImpactMassContribution(const int aIdxEnvImpact);
    double* envImpactMassContributionDiscounted(const int aIdxEnvImpact);
    double* envHistImpactMassContributionDiscounted(const int aIdxEnvImpact);

    /** TimeStep management */
    inline double TimeStep(uint i) { return mTimeSteps[i]; }                                       /** List of timesteps */
    std::vector<double>& timesteps() { return mTimeSteps; }
    void setNpdtPast(uint i) { mNpdtPast = i; }                                       /** List of timesteps */
    void setAbsoluteTimeStep(const uint* i) { mptrAbsoluteTimeStep = i; }
    void setTimeshift(const uint* i) { mptrTimeshift = i; }
    void setFuturesize(const uint* i) { mptrFuturesize = i; }
    uint npdtPast() { return mNpdtPast; }
    
    bool getAllocate() const { return mAllocate; }
    void setTimeSteps(const bool& useVariableTimeSteps, std::vector<double> aTimeSteps, uint64_t aTimeStepBeginLP, uint64_t aTimeStepBeginForecast, uint64_t aDecreaseOptimizationHorizon);   /** TimeStep settings */
    void decreaseOptimizationHorizon();                /** Update mTimeSteps */
    std::vector<double> getOptimalSizeAllCycles() { return mOptimalSizeAllCycles; }

    void addVariable(MIPModeler::MIPVariable0D& variable0D, const std::string& name, const double& lowerBound = -MIP_INFINITY,
        const double& upperBound = MIP_INFINITY, const MIPModeler::MIPVarType& varType = MIPModeler::MIP_FLOAT);

    void addVariable(MIPModeler::MIPVariable1D& variable1D, const std::string& name, const double& lowerBound = -MIP_INFINITY, 
        const double& upperBound = MIP_INFINITY, const MIPModeler::MIPVarType& varType = MIPModeler::MIP_FLOAT, const int& cols = -1);

    void addConstraint(MIPModeler::MIPConstraint constraint, const std::string& name, const uint& t=0);

    std::string CName(const std::string aRadical, const uint& t) const
    {
        std::string aname = "c" + aRadical + parent()->objectName() + std::to_string(t);
        return aname;
    }

    std::string CName(std::string aRadical) const
    {
        std::string aname = "v" + aRadical + parent()->objectName();
        return aname;
    }

    void setMaxValue(const double& aMaxVal);     
    void setMinValue(const double& aMaxVal);      
    double getMaxBound(); /** Upper bound of size equal to weight * maxval */
    double getMinBound(); /** Lower bound of size equal to weight * minval */
    void addVarSizeMax(const double& aMaxVal, const std::string& aStrName);   // define upper bound for sizeMax variable ie weight or maxPower
    void setExpSizeMax(const MIPModeler::MIPExpression& aExpInstalled = MIPModeler::MIPExpression(1));

    bool isPortIndicatorNameUnique(const MilpPort* targetPort);

    void resetFlags() {
        mAllocate = true;
    }

    const std::string* pCurrency() const;
    const std::string* pQuantity(const std::string& a_Quantity) const;


    //std::string OptimalSizeUnit() const;
    const std::string* pOptimalSizeUnit() const;

    std::string ExpUnit(const std::string& aExpressionName); /* get the unit value of a given expression */
    const UnitParam* pExpUnitParam(const std::string& aExpressionName); /* get a pointer to the UnitParam of a given expression (IO) in order to dynamically pass it to e.g. ZEVariables */

    std::string getAbsoluteFileName(const std::string& filename) const;

    const std::map<std::string, std::string>& getLabelMap() const { return mLabelMap; };
    void setLabelMap(const std::map<std::string, std::string>& aLabelMap) { mLabelMap = aLabelMap; };
    void setLabel(const std::string& aLabel, const std::string& aValue) { mLabelMap[aLabel] = aValue; };
    std::string getLabelValue(const std::string& aLabel) const;

    std::vector<std::string> possibleModelClasses() const { return mPossibleModelClasses; };

private:
    std::string m_OptimalSizeUnit;
    const std::string* p_OptimalSizeUnit;  
    bool mComputeSizeMax; /* If true then compute mExpSizeMax (call setExpSizeMax). It is automatically set to true on "add SizeMaxIO"*/
    std::map<std::string, std::string> mLabelMap;


protected:
    int checkBusSameValueVarName(MilpPort* port);
    virtual int checkBusFlowBalanceVarName(MilpPort* port, int &inumberchange, std::string &varUseCheck);
    virtual bool defineDefaultVarNames(MilpPort* port);
    
    int checkVariable(const std::string variable) const;
    int checkUnit(MilpPort* port);

    Cairn_Exception mException;

    std::vector<std::string> mPossibleModelClasses;

    /** Pointers **/
    MIPModeler::MIPModel* mModel;      /** Pointer to global MIPModel */
    MilpComponent* mParentCompo;       /** Parent Component wearing the submodel */
    EnergyVector* mMainCarrier;       /** Main carrier of the component (in case of Bus, it is the only carrier) */

    /** Ports */
    std::map <std::string, std::map<std::string, std::string>> mDefaultPorts;
    std::vector<MilpPort*> mListPort{};      /** List of MilpPort of Component  */
    bool mVariablePortNumber;          /** Model can have variable number of physical inlet and outlet - they must be <= number of total NbInputPorts or outputs */
    int mNbInputPorts;                 /** Number of Componen Input Ports */
    int mNbOutputPorts;                /** Number of Component Outputs Ports */
    int mNbInputFlux;                  /** Number of first Component NbInputPorts dedicated to Flux at inlet */
    int mNbOutputFlux;                 /** Number of first Component Outputs dedicated to Flux at inlet */

    /** Model IO Interface */
    t_mapIOs mIOExpressions; /** Model List of Readable Expressions */
    /** Model Rolling Horizon interface */
    t_mapRHs mListControlIO; /** Model List of variable that should remain in memory for rolling horizon */
    std::string mOptimalSizeExpression;           /** Name of expression to be used for OptimalSize evaluation */
    /** Expressions */
    std::vector<MIPModeler::MIPExpression*> mExpressions0D; /* A map of 0D expressions that are not IO (not in mIOExpressions) */
    std::vector<SExpression1D> mExpressions1D; /* A map of 1D expressions that are not IO (not in mIOExpressions) */

    MIPModeler::MIPExpression   mExpSizeMax;           /** Optimal capacity expression */

    /** MIPModel Variables */
    MIPModeler::MIPVariable0D mVarWeight;
    MIPModeler::MIPVariable0D mVarSizeMax;      /** MILP 0D Variable on component Optimal capacity (power or storage) */

    /** Expression Names TODO: use hard-coded names e.g. const std::string CapexExpName = "Capex" */ 
    std::string mSubObjectiveExpression;          /** Name of expression to be used for Subobjective evaluation */
    std::string mCapexExpression;                 /** Name of expression to be used for Capex evaluation */
    std::string mOpexExpression;                  /** Name of expression to be used for Opex evaluation */
    std::string mReplacementExpression;           /** Name of expression to be used for Replacement evaluation */
    std::string mVariableCostsExpression;         /** Name of expression to be used for VariableCosts evaluation */
    std::string mPenaltyConstraintExpression;     /** Name of expression to be used for Penalty evaluation */
    std::vector<std::string> mEnvGreyImpactCostExpression;       /** Name of expression to be used for EnvGreyImpactCost evaluation */
    std::vector<std::string> mEnvGreyImpactMassExpression;       /** Name of expression to be used for EnvGreyImpactMass evaluation */
    std::vector<std::string> mEnvImpactCostExpression;           /** Name of expression to be used for EnvImpactCost evaluation */
    std::vector<std::string> mEnvImpactMassExpression;           /** Name of expression to be used for EnvImpactMass evaluation */

    /** Flags */
    bool mAllocate;           /** Allocation flag to prevent from multiple allocations and memory leaks during rolling horizon process */
    bool mExportIndicators;   /** bool indicating the export of component specific indicators if = true*/

    std::string mControl;                          /** Control type taken into account : */

    /** InputParam */
    InputParam* mInputParam;                   /** Parameters are MILP constant parameters comming from Settings File */
    InputParam* mInputTimeSeries;              /** Time series comming from Description files */
    InputParam* mInputEnvImpacts;              /** Environmental impacts that the user wants to compute */
    InputParam* mInputPortImpacts;             /** Port Environmental impacts that the user wants to compute */
    InputParam* mTSInputPortImpacts;           /** Timeseries of Port Environmental impacts */    
    InputParam* mInputPerfParam;               /** Performance Parameters are MILP constant vectors comming from DataFile csv File */
    InputParam* mInputIndicators;              /** Indicators that the user wants to export */

    /** Weight */ //TODO: move mLPModelOnly to TechnicalSubModel and mLPWeightOptimization to TechnicalSubModel or SourceLoadSubModel ?
    double mWeight;
    bool mLPWeightOptimization;           /** bool indicating use LP sizing if Weight if = true - default to false*/ 
    bool mLPModelOnly;                    /** bool indicating use of LPModelOnly if = true - default to false*/
    std::string mWeightUnit;
    std::vector<std::string> mPossibleWeightUnits;     /** List of possible weight units */

    double mMaxValue;                     
    double mMinValue;                      

    /** Timestep management */
    std::vector<double> mTimeSteps;         /** vector of timesteps (variable or constant timestep on planning horizon)*/
    int mHorizon;                      /** for convenience in all submodels horizon name mHorizon : number of timesteps (size of mTimeSteps) */
    uint mNpdtPast;                         /** Number of past timestep */
    uint64_t mTimeStepBeginLP;              /** first timestep of the LP model / if timestep lower, use MILP model, else use LP */
    uint64_t mTimeStepBeginForecast;        /** first timestep of the LP model / if timestep lower, use MILP model, else use LP */
    uint64_t mMilpNpdt;                     /** Number of timesteps using MILP scheme instead of LP */
    uint64_t mDecreaseOptimizationHorizon;  /** activate the decreasing optimization horizon if equal to 1 */
    const uint* mptrAbsoluteTimeStep;       /** pointer to current absolute time step */
    const uint* mptrTimeshift;              /** pointer to timeShift */
    const uint* mptrFuturesize;             /** pointer to futuresize */
    bool mUseVariableTimeSteps;

    /** Typical Periods management */
    bool mUseTypicalPeriods{ false };
    std::vector<int> mVectTypicalPeriods; //Vector of Timesteps over planning horizon. Past timesteps use small timestep
    uint mTypicalPeriods;                // number of typical period eg number of typical days, weeks...
    uint mNDtTypicalPeriods;            // number of timesteps in one typical period : 24 hours...
    uint mCondensedNpdt;               // number of condensed timesteps resulting from typical period use
    std::vector<int> mFullVectTypicalPeriods; //Vector of Timesteps over planning horizon. Past timesteps use small timestep

    /** Economic */ 
    double mHistVariableCostsDiscounted;

    /** Env Impacts */
    std::vector<class EnvImpact*> mEnvImpacts;
    std::vector<std::string> mEnvImpactsList;                      /** List of the considered environmental impacts */
    std::vector<std::string> mEnvImpactsShortNamesList;           /** List of the considered environmental impacts short names*/
    std::vector<std::string> mEnvImpactUnitsList;                /** List of the units of the considered environmental impacts */
    std::vector<double> mEnvImpactCosts;

    std::vector<double> mOptimalSizeAllCycles; //Size = Number of cycles; PLAN from each cycle

    /* State and StartUpShutDown */
    bool mAddStateVariable;   /** bool to add state variables like ON / OFF */
    MIPModeler::MIPData1D mHistState;
    MIPModeler::MIPVariable1D mState;
    MIPModeler::MIPExpression1D mExpState;
    int mAbsInitialState;

    bool mAddStartUpShutDownVariable;      /** bool to add StartUp and ShutDown variables */
    MIPModeler::MIPData1D mHistStartUp;
    MIPModeler::MIPVariable1D mStartUp;
    MIPModeler::MIPExpression1D mExpStartUp;

    MIPModeler::MIPData1D mHistShutDown;
    MIPModeler::MIPVariable1D mShutDown;
    MIPModeler::MIPExpression1D mExpShutDown;

    /** if condense variables on typical periods */
    bool mCondenseVariablesOnTP;
    bool mCondenseBinariesOnly;
    bool mActivateConstraintsBetweenTP;
};

#endif // SubModel_H