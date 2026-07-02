#ifndef SUBMODEL_H
#define SUBMODEL_H

#include <unordered_set>
#include <unordered_map>
#include <cmath>
#include <string>
#include <vector>
#include <map>
#include <fstream>

#include "CairnCore_global.h"
#include "Cairn_Exception.h"
#include "GlobalSettings.h"
#include "MIPModeler.h"
#include "MilpComponent.h"
#include "InputParam.h"
#include "EnvImpact.h"
#include "ModelVar.h"

class MilpPort;

namespace Constants {
    inline const std::string VARIABLE     = "variable";
    inline const std::string STORAGE_NAME = "storagename";
    inline const std::string FLUX_NAME    = "fluxname";
    inline const std::string DIRECTION    = "direction";
}

struct SExpression1D {
    MIPModeler::MIPExpression1D* pExp1D{ nullptr };
    int* pSize{ nullptr };
};

using namespace GS;
using namespace Constants;

 /**
  * \brief Abstract base class for all MIPModeler models used by MilpComponent.
  * \details Use the following units (instead of strict SI units) to avoid scaling issues: 
      * Mass Flow    : kg/h
      * Power flow   : MW
      * Mass         : kg
      * Energy       : MWh
      * Time         : Hours
  */

extern std::string CAIRNCORESHARED_EXPORT indicatorName(SubModel* ap_Model, 
    MilpPort* ap_Port, const std::vector<std::string>& a_NameParts);

class CAIRNCORESHARED_EXPORT SubModel : public CairnObject
{
public:
    // ---------------------------------------------------------------------
    // Construction & identity
    // ---------------------------------------------------------------------
    explicit SubModel(CairnObject* aParent = nullptr);
    virtual ~SubModel();

    std::string ModelClassName() const;
    std::vector<std::string> possibleModelClasses() const { return mPossibleModelClasses; }

    // ---------------------------------------------------------------------
    // Core virtual interface (to be/can be overridden by derived models)
    // ---------------------------------------------------------------------
    virtual void declareModelConfigurationParameters() = 0;
    virtual void declareModelParameters() = 0;      ///< Model input parameters from settings file
    virtual void declareModelInterface() = 0;       ///< Model IO interface
    virtual void declareModelIndicators() = 0;      ///< Model indicators to be exported
    virtual void initDefaultPorts() = 0;

    // buildModel is overridden in TechnicalSubModel, BusSubModel, and OperationalSubModel.
    // Derived models should implement computeModelContribution() instead of redefining buildModel.
    virtual void buildModel() = 0;                  ///< MILP model description: variables, constraints

    /// Initialize model state and size-related variables.
    /// Typical responsibilities:
    /// - Call setMaxValue(...) when mExpSizeMax is declared via addSizeMaxIO(...)
    /// - Optionally call setMinValue(...)
    /// - Set flags mAddStateVariable and mAddStartUpShutDownVariable, etc.
    /// - Pre-compute initial state data for addControlIO(...)
    virtual void computeInitialData() {}

    virtual int  checkConsistency();
    virtual void buildControlVariables();
    virtual void resetHistStoredValues() {}         ///< Reset stored historical values (rolling horizon)
    virtual void declareInputFluxIOs(MilpPort* defaultPort = nullptr) {}
    virtual void declareOutputFluxIOs(MilpPort* defaultPort = nullptr) {}
    virtual void setPortPointers() {}
    virtual void computeModelContribution() {}      ///< Compute model-specific contribution
    virtual void computeAllIndicators(const double* optSol);
    virtual int  defineDefaultVarNames();
    virtual void setTimeData();
    virtual void setTypicalPeriods(const bool& useTypicalPeriods, const uint& aTypicalPeriods,
        const uint& aNDtTypicalPeriods, const std::vector<int>& aVectTypicalPeriods);
    virtual bool isSizeOptimized();
    virtual bool isPriceOptimized();                // Only SourceLoad
    virtual std::string ObjectiveType() { return {}; }
    virtual uint64_t exprMilpHorizon();             // Technical + Operational
    virtual uint64_t varMilpHorizon();              // Technical + Operational

    virtual void declareDefaultModelConfigurationParameters()
    {
        // Boolean: export component-specific indicators
        addParameter("ExportIndicators",
            &mExportIndicators,
            true,
            false,
            true,
            "Export component specific indicators",
            "");

        // String: weight unit (no unit by default)
        addParameter("WeightUnit",
            &mWeightUnit,
            "",
            false,
            true,
            "Unit of weight (no unit by default)",
            "");
    }

    virtual void declareDefaultModelInterface()
    {
        // Register IO expressions to be exported as results.
        // Note: the size of 1D IO expressions is always equal to mHorizon.

        // Register non-IO 0D expressions (none by default).

        // Register non-IO 1D expressions (none by default).
    }

    // ---------------------------------------------------------------------
    // Component identity, parent, ports & topology
    // ---------------------------------------------------------------------
    std::string Name() const { return parent()->objectName(); }
    virtual double Sens() { return 0.0; }           // To be overridden in Grid and SourceLoad

    void setParentCompo(MilpComponent* aCompo) { mParentCompo = aCompo; }
    void setControlType(const std::string& aControl) { mControl = aControl; }

    virtual void  defineMainCarrier() {}             // To be defined in individual models
    void          setMainCarrier(EnergyVector* aEnergyVector) { mMainCarrier = aEnergyVector; }
    EnergyVector* getMainCarrier() const { return mMainCarrier; }

    int  checkPorts();
    void setPortList(const std::vector<MilpPort*>& aListPort) { mListPort = aListPort; }
    const std::vector<MilpPort*>& PortList() { return mListPort; }
    void addPort(MilpPort* port) { mListPort.push_back(port); }
    void removePort(MilpPort* port);
    void removeBusPort(MilpPort* port);
    MilpPort* getPort(const std::string& aPortId);
    MilpPort* getPortByType(const std::string& aType, const std::string& aDirection = "ANY");

    std::map<std::string, std::map<std::string, std::string>> DefaultPorts() const { return mDefaultPorts; }
    std::map<std::string, std::string> getDefaultPortData(const std::string& portId) const;

    bool isPortIndicatorNameUnique(const MilpPort* targetPort);

    // ---------------------------------------------------------------------
    // Indicators
    // ---------------------------------------------------------------------
    void resetIndicators();

    void exportIndicators(std::fstream& out, 
        std::string name, 
        const std::string& range, 
        const std::vector< std::string >& refLabelList,
        const bool showDescription, 
        bool forced, 
        const bool isRollingHorizon);

    // ---------------------------------------------------------------------
    // Exception & flags
    // ---------------------------------------------------------------------
    Cairn_Exception getException() const { return mException; }
    void            setException(const Cairn_Exception& aException) { mException = aException; }

    bool ExportIndicators() const { return mExportIndicators; }

    void resetFlags()
    {
        mAllocate = true;
    }

    // ---------------------------------------------------------------------
    // Parameters & time series registration
    // ---------------------------------------------------------------------
    virtual void declareInputParams(const std::string& name);
    void         deleteInputParams();

    void addParameter(const std::string& aParamName,
        const t_pvalue& aPtr,
        t_value aDefaultValue,
        t_flag aIsBlocking = true,
        t_flag aIsUsed = true,
        const std::string& aDescription = "",
        const t_unit& aUnit = "",
        const std::string& aShowConfig = "Base");

    void addTimeSeries(const std::string& aParamName,
        std::vector<double>* aDblePtr,
        t_flag IsBlocking = true,
        t_flag aIsUsed = true,
        const std::string& aDescription = "",
        const t_unit& aUnit = "",
        const std::string& aShowConfig = "Base",
        double a_default = 1.0,
        double a_min = std::nan("1"),
        double a_max = std::nan("1"));

    void addPerfParam(const std::string& aParamName,
        std::vector<double>* aPtr,
        t_flag aIsBlocking = true,
        t_flag aIsUsed = true,
        const std::string& aDescription = "",
        const t_unit& aUnit = "");

    // ---------------------------------------------------------------------
    // IO & expressions (interface)
    // ---------------------------------------------------------------------
    void addIO(const std::string& aIOName,
        MIPModeler::MIPExpression* aExprPtr,
        t_flag aIsUsed,
        const t_unit& aUnit,
        const std::string& aDescription = "");

    void addIO(const std::string& aIOName,
        MIPModeler::MIPExpression1D* aExprPtr1D,
        t_flag aIsUsed,
        const t_unit& aUnit,
        const std::string& aDescription = "");

    void addSizeMaxIO(const std::string& aIOName,
        MIPModeler::MIPExpression* aExprPtr,
        t_flag aIsUsed,
        const std::string& aUnit,
        const std::string& aDescription = "");

    void addSizeMaxIO(const std::string& aIOName,
        MIPModeler::MIPExpression* aExprPtr,
        t_flag aIsUsed,
        const std::string* pUnit,
        const std::string& aDescription = "");

    bool assertIONonExistence(const std::string& name,
        const t_pExpr expression);

    void assertIsSizeMaxExp(MIPModeler::MIPExpression* aExprPtr);
    void assertIsNotSizeMaxExp(MIPModeler::MIPExpression* aExprPtr);

    void addControlIO(const std::string& aIOName,
        MIPModeler::MIPExpression1D* aExprPtr1D,
        t_flag aIsUsed,
        const t_unit& aUnit,
        double* aValuePtr,
        double* aDefaultValue = nullptr,
        bool a_isMPC = true,
        const std::string& aDescription = "");

    void addControlIO(const std::string& aIOName,
        MIPModeler::MIPExpression1D* aExprPtr1D,
        t_flag aIsUsed,
        const t_unit& aUnit,
        std::vector<double>* aHistPtr,
        double* aDefaultValue = nullptr,
        bool a_isMPC = true,
        const std::string& aDescription = "");

    using t_mapIOs = std::map<std::string, ModelIO*>;
    using IOIterator = t_mapIOs::iterator;

    void removeIO(const std::string& aName);
    IOIterator removeIO(IOIterator it);
    void removeEnvImpactIOs(const std::string& aImpactName);
    void removeIOs();

    const t_mapIOs& getMapIOExpression() { return mIOExpressions; }

    ModelIO* getIOExpression(const std::string& aName) const;
    std::vector<ModelIO*> getIOExpressions(const EIOModelType& aIOType = EIOModelType::eMIPExpression1D);
    MIPModeler::MIPExpression* getMIPExpression(std::string aExpressionName) const;
    MIPModeler::MIPExpression1D* getMIPExpression1D(std::string aExpressionName) const;
    MIPModeler::MIPExpression& getMIPExpression1D(uint i, std::string aExpressionName);

    void dumpIOExpressionList() const;
    void dumpIOExpression1DList() const;

    // ---------------------------------------------------------------------
    // Model / solver integration
    // ---------------------------------------------------------------------
    MIPModeler::MIPModel* getModel() { return mModel; }
    void                  setMIPModel(MIPModeler::MIPModel* aModel) { mModel = aModel; }

    void addStateConstraints(const uint64_t& aCondensedNpdt,
        const MIPModeler::MIPExpression& aExpInstalled = MIPModeler::MIPExpression(1));
    void addStartUpShutDown(const uint64_t& aCondensedNpdt,
        const MIPModeler::MIPExpression& aExpInstalled = MIPModeler::MIPExpression(1));

    void addVariable(MIPModeler::MIPVariable0D& variable0D,
        const std::string& name,
        const double& lowerBound = -MIP_INFINITY,
        const double& upperBound = MIP_INFINITY,
        const MIPModeler::MIPVarType& varType = MIPModeler::MIP_FLOAT);

    void addVariable(MIPModeler::MIPVariable1D& variable1D,
        const std::string& name,
        const double& lowerBound = -MIP_INFINITY,
        const double& upperBound = MIP_INFINITY,
        const MIPModeler::MIPVarType& varType = MIPModeler::MIP_FLOAT,
        const int& cols = -1);

    void addConstraint(MIPModeler::MIPConstraint constraint,
        const std::string& name,
        const uint& t = 0);

    std::string CName(const std::string& radical, const uint& t) const
    {
        return "c" + radical + parent()->objectName() + std::to_string(t);
    }

    std::string VName(const std::string& radical) const
    {
        return "v" + radical + parent()->objectName();
    }

    // ---------------------------------------------------------------------
    // Expression allocation & lifecycle
    // ---------------------------------------------------------------------
    const std::vector<MIPModeler::MIPExpression*>& getListExpression0D() { return mExpressions0D; }
    void addExp(MIPModeler::MIPExpression* aExprPtr) { mExpressions0D.push_back(aExprPtr); }

    const std::vector<SExpression1D>& getListExpression1D() { return mExpressions1D; }
    void addExp(MIPModeler::MIPExpression1D* aExprPtr1D, int* aSize)
    {
        mExpressions1D.push_back({ aExprPtr1D, aSize });
    }

    using t_mapRHs = std::map<std::string, ControlVar*>;
    const t_mapRHs& getListControlIO() { return mListControlIO; }

    void fillExpression(MIPModeler::MIPExpression1D& aExpress1D,
        MIPModeler::MIPVariable1D& aVariable);

    static void closeExpression(MIPModeler::MIPExpression& aExpress);
    static void closeExpression1D(MIPModeler::MIPExpression1D& aExpress1D);

    void allocateExpressions();
    virtual void closeExpressions();

    // ---------------------------------------------------------------------
    // Computation helpers
    // ---------------------------------------------------------------------
    void computeTime(bool bsetValue,
        uint aNpdt,
        uint aShift,
        MIPModeler::MIPExpression1D exp,
        const double* optSol,
        double& ret);

    void computeTime(bool bsetValue,
        uint aNpdt,
        uint aShift,
        MIPModeler::MIPExpression1D exp,
        const double* optSol,
        double& retCharged,
        double& retDischarged);

    void computeProduction(bool bsetValue,
        uint aNpdt,
        uint aShift,
        MIPModeler::MIPExpression1D exp,
        const double* optSol,
        const double& aCoeff,
        const double& bCoeff,
        double& aProduction,
        const bool& aTimeIntegration = true);

    void computeProduction(bool bsetValue,
        uint aNpdt,
        uint aShift,
        MIPModeler::MIPExpression1D exp,
        const double* optSol,
        const double& aCoeff,
        const double& bCoeff,
        double& retCharged,
        double& retDischarged);

    void computeConsumption(bool bsetValue,
        uint aNpdt,
        uint aShift,
        MIPModeler::MIPExpression1D exp,
        const double* optSol,
        const double& aCoeff,
        const double& bCoeff,
        double& aConsumption);

    void computeLvlConsumption(bool bsetValue,
        uint aNpdt,
        uint aShift,
        MIPModeler::MIPExpression1D exp,
        const double* optSol,
        const double& aCoeff,
        const double& bCoeff,
        double& aConsumption);

    void computeLvlProduction(bool bsetValue,
        uint aNpdt,
        uint aShift,
        MIPModeler::MIPExpression1D exp,
        const double* optSol,
        const double& aCoeff,
        const double& bCoeff,
        double& aProduction);

    void computeLvlProduction(bool bsetValue,
        uint aNpdt,
        uint aShift,
        MIPModeler::MIPExpression1D exp,
        const double* optSol,
        const double& aCoeff,
        const double& bCoeff,
        double& retCharged,
        double& retDischarged);

    void computeLvlImpact(bool bsetValue,
        uint aNpdt,
        uint aShift,
        MIPModeler::MIPExpression1D exp,
        const double* optSol,
        const double& aCoeff,
        const double& bCoeff,
        double& aProduction);

    void computeDiscounted(uint aNpdt,
        uint aShift,
        MIPModeler::MIPExpression1D exp,
        const double* optSol,
        double& aDiscounted);

    void computeIndicator(const MIPModeler::MIPExpression1D& exp,
        const double* optSol,
        double& aUnDiscounted,
        double& aDiscounted,
        double& aHistUnDiscounted,
        double& aHistDiscounted,
        bool isEnvImpact = false);

    void writeSolution(const double* optimalSolution,
        std::map<std::string, std::vector<double>>& resultats);

    // ---------------------------------------------------------------------
    // Model parameters accessors
    // ---------------------------------------------------------------------
    InputParam* getInputParam() { return mInputParam; }
    InputParam* getInputPerfParam() { return mInputPerfParam; }
    InputParam* getInputTimeSeries() { return mInputTimeSeries; }
    InputParam* getInputIndicators() { return mInputIndicators; }
    InputParam* getInputEnvImpactsParam() { return mInputEnvImpacts; }
    InputParam* getInputPortImpactsParam() { return mInputPortImpacts; }
    InputParam* getInputPortImpactsParamTS() { return mTSInputPortImpacts; }

    // ---------------------------------------------------------------------
    // Economic & size expressions
    // ---------------------------------------------------------------------
    std::string getOptimalSizeExpression() const { return mOptimalSizeExpression; }

    void        setVariableCostsExpression(const std::string& aExpressionName) { mVariableCostsExpression = aExpressionName; }
    std::string getVariableCostsExpression() const { return mVariableCostsExpression; }

    void        setCapexExpression(const std::string& aExpressionName) { mCapexExpression = aExpressionName; }
    std::string getCapexExpression() const { return mCapexExpression; }

    void        setOpexExpression(const std::string& aExpressionName) { mOpexExpression = aExpressionName; }
    std::string getOpexExpression() const { return mOpexExpression; }

    void        setReplacementExpression(const std::string& aExpressionName) { mReplacementExpression = aExpressionName; }
    std::string getReplacementExpression() const { return mReplacementExpression; }

    void        setEnvImpactCostExpression(const std::string& aExpressionName) { mEnvImpactCostExpression.push_back(aExpressionName); }
    std::string getEnvImpactCostExpression(int i) const { return mEnvImpactCostExpression.at(i); }

    void        setEnvImpactMassExpression(const std::string& aExpressionName) { mEnvImpactMassExpression.push_back(aExpressionName); }
    std::string getEnvImpactMassExpression(int i) const { return mEnvImpactMassExpression.at(i); }

    void        setEmbodiedCostExpression(const std::string& aExpressionName) { mEnvGreyImpactCostExpression.push_back(aExpressionName); }
    std::string setEmbodiedCostExpression(int i) const { return mEnvGreyImpactCostExpression.at(i); }

    void        setEmbodiedMassExpression(const std::string& aExpressionName) { mEmbodiedMassExpression.push_back(aExpressionName); }
    std::string getEmbodiedMassExpression(int i) const { return mEmbodiedMassExpression.at(i); }

    void        setPenaltyConstraintExpression(const std::string& aExpressionName) { mPenaltyConstraintExpression = aExpressionName; }
    std::string getPenaltyConstraintExpression() const { return mPenaltyConstraintExpression; }

    void        setSubobjectiveExpression(const std::string& aExpressionName) { mSubObjectiveExpression = aExpressionName; }
    std::string getSubobjectiveExpression() const { return mSubObjectiveExpression; }

    // ---------------------------------------------------------------------
    // Environmental impacts
    // ---------------------------------------------------------------------
    void setEnvImpactsList(const std::vector<std::string>& aEnvImpactsList) { mEnvImpactsList = aEnvImpactsList; }
    void setEnvImpactsShortNamesList(const std::vector<std::string>& aEnvImpactsShortNamesList) { mEnvImpactsShortNamesList = aEnvImpactsShortNamesList; }
    void setEnvImpactUnitsList(const std::vector<std::string>& aEnvImpactUnitsList) { mEnvImpactUnitsList = aEnvImpactUnitsList; }
    void setEnvImpactCosts(const std::vector<double>& aEnvImpactCosts) { mEnvImpactCosts = aEnvImpactCosts; }

    std::vector<class EnvImpact*> getEnvImpacts() { return mEnvImpacts; }

    double* envEmbodiedMassContribution(int aIdxEnvImpact);
    double* envGreyImpactCostContribution(int aIdxEnvImpact);
    double* envImpactCostContribution(int aIdxEnvImpact);
    double* envHistImpactCostContribution(int aIdxEnvImpact);
    double* envImpactCostContributionDiscounted(int aIdxEnvImpact);
    double* envHistImpactCostContributionDiscounted(int aIdxEnvImpact);

    double* envImpactMassContribution(int aIdxEnvImpact);
    double* envHistImpactMassContribution(int aIdxEnvImpact);
    double* envImpactMassContributionDiscounted(int aIdxEnvImpact);
    double* envHistImpactMassContributionDiscounted(int aIdxEnvImpact);

    void removeImpactExpressionName(const std::string& impactName);
    void deleteEnvImpacts();

    // ---------------------------------------------------------------------
    // Time step & horizon management
    // ---------------------------------------------------------------------
    inline double TimeStep(uint i) const { return mTimeSteps[i]; }
    std::vector<double>& timesteps() { return mTimeSteps; }

    void setNpdtPast(uint i) { mNpdtPast = i; }
    uint npdtPast() const { return mNpdtPast; }

    void setAbsoluteTimeStep(const uint* i) { mptrAbsoluteTimeStep = i; }
    void setTimeshift(const uint* i) { mptrTimeshift = i; }
    void setFuturesize(const uint* i) { mptrFuturesize = i; }

    bool getAllocate() const { return mAllocate; }

    void setTimeSteps(const bool& useVariableTimeSteps,
        std::vector<double> aTimeSteps,
        uint64_t aTimeStepBeginLP,
        uint64_t aTimeStepBeginForecast,
        uint64_t aDecreaseOptimizationHorizon);

    void decreaseOptimizationHorizon();
    std::vector<double> getOptimalSizeAllCycles() const { return mOptimalSizeAllCycles; }

    // ---------------------------------------------------------------------
    // Size max & bounds
    // ---------------------------------------------------------------------
    void   setMaxValue(const double& aMaxVal);
    void   setMinValue(const double& aMinVal);
    double getMaxBound();   ///< Upper bound of size = weight * maxVal
    double getMinBound();   ///< Lower bound of size = weight * minVal

    void addVarSizeMax(const double& aMaxVal,
        const std::string& aStrName); // define upper bound for sizeMax variable ie weight or maxPower

    void setExpSizeMax(const MIPModeler::MIPExpression& aExpInstalled = MIPModeler::MIPExpression(1));

    // ---------------------------------------------------------------------
    // Units, labels, metadata
    // ---------------------------------------------------------------------
    void setPossibleWeightUnits(const std::vector<std::string>& aPossibleWeightUnits)
    {
        mPossibleWeightUnits = aPossibleWeightUnits;
    }

    const std::string* pCurrency() const;
    const std::string* pQuantity(const std::string& a_Quantity) const;

    const std::string* pOptimalSizeUnit() const;

    std::string      ExpUnit(const std::string& aExpressionName);       // unit value of a given expression
    const UnitParam* pExpUnitParam(const std::string& aExpressionName); // a pointer to the UnitParam of a given expression

    std::string getAbsoluteFileName(const std::string& filename) const;

    const std::map<std::string, std::string>& getLabelMap() const { return mLabelMap; }
    void setLabelMap(const std::map<std::string, std::string>& aLabelMap) { mLabelMap = aLabelMap; }
    void setLabel(const std::string& aLabel, const std::string& aValue) { mLabelMap[aLabel] = aValue; }
    std::string getLabelValue(const std::string& aLabel) const;

private:
    // Units & labels
    std::string m_OptimalSizeUnit;
    const std::string* p_OptimalSizeUnit;
    bool mComputeSizeMax;                         // If true, compute mExpSizeMax (-> call setExpSizeMax) 
    std::map<std::string, std::string> mLabelMap;

protected:
    // ---------------------------------------------------------------------
    // Private helpers
    // ---------------------------------------------------------------------

    // Validate that all bus-connected ports use consistent variable names
    int checkBusSameValueVarName(MilpPort* port);
     
    // Validate flow-balance variable naming for bus-type models
    virtual int checkBusFlowBalanceVarName(
        MilpPort* port,
        int& inumberchange,
        std::string& varUseCheck
    );
    
    // Assign default variable names for a port when not explicitly provided
    virtual bool defineDefaultVarNames(MilpPort* port);
    
    // Ensure a variable name follows model naming conventions
    int checkVariable(const std::string variable) const;
    
    // Validate that the port unit is consistent with model expectations
    int checkUnit(MilpPort* port);
    
    // ---------------------------------------------------------------------
    // Exception handling 
    // ---------------------------------------------------------------------

    Cairn_Exception mException; /** Stores the last exception raised inside the submodel */

    // ---------------------------------------------------------------------
    // Model identity & classification 
    // ---------------------------------------------------------------------

    std::vector<std::string> mPossibleModelClasses; /** List of model class names this submodel can represent */

    // ---------------------------------------------------------------------
    // Core pointers (model, component, carrier) 
    // ---------------------------------------------------------------------

    MIPModeler::MIPModel* mModel;       /** Pointer to the global MILP model */
    MilpComponent* mParentCompo;        /** Pointer to the parent component that owns this submodel */
    EnergyVector* mMainCarrier;         /** Pointer to the main energy carrier */

    // ---------------------------------------------------------------------
    // Ports & topology 
    // ---------------------------------------------------------------------

    std::map<std::string, std::map<std::string, std::string>> mDefaultPorts; /** Default port definitions (id -> attributes) */
    std::vector<MilpPort*> mListPort;          /** List of all ports attached to this component */
    bool mVariablePortNumber;                  /** True if the model supports a variable number of inlets/outlets - must be <= NbInputPorts/NbOutputPorts */
    int  mNbInputPorts;                        /** Total number of input ports */
    int  mNbOutputPorts;                       /** Total number of output ports */
    int  mNbInputFlux;                         /** Number of input ports dedicated to flux */
    int  mNbOutputFlux;                        /** Number of output ports dedicated to flux */

    // ---------------------------------------------------------------------
    // IO interface & expressions
    // ---------------------------------------------------------------------

    t_mapIOs                                mIOExpressions;          /** Map of named IO expressions exposed by the model */
    t_mapRHs                                mListControlIO;          /** Rolling-horizon IO variables that must persist across solves */
    std::string                             mOptimalSizeExpression;  /** Name of the expression used to compute optimal size */
    std::vector<MIPModeler::MIPExpression*> mExpressions0D;          /** List of internal 0D expressions (not part of IO interface) */
    std::vector<SExpression1D>              mExpressions1D;          /** List of internal 1D expressions (not part of IO interface) */
    MIPModeler::MIPExpression               mExpSizeMax;             /** Expression representing the maximum installable capacity */

    // ---------------------------------------------------------------------
    // MILP variables 
    // ---------------------------------------------------------------------

    MIPModeler::MIPVariable0D mVarWeight;  /** Weight variable (used in size optimization) */
    MIPModeler::MIPVariable0D mVarSizeMax; /** 0D variable representing maximum capacity (e.g. power, storage, etc.) */

    // ---------------------------------------------------------------------
    // Expression names (economic, environmental, penalties) 
    // ---------------------------------------------------------------------

    std::string mSubObjectiveExpression;      /** Expression used for sub-objective evaluation */
    std::string mCapexExpression;             /** Capital expenditure expression */
    std::string mOpexExpression;              /** Operational expenditure expression */
    std::string mReplacementExpression;       /** Replacement cost expression */
    std::string mVariableCostsExpression;     /** Variable cost expression */
    std::string mPenaltyConstraintExpression; /** Penalty constraint expression */
    std::vector<std::string> mEnvGreyImpactCostExpression; /** Grey environmental impact cost expressions */
    std::vector<std::string> mEmbodiedMassExpression; /** Grey environmental impact mass expressions */
    std::vector<std::string> mEnvImpactCostExpression;     /** Environmental impact cost expressions */
    std::vector<std::string> mEnvImpactMassExpression;     /** Environmental impact mass expressions */

    // ---------------------------------------------------------------------
    // Flags & control 
    // ---------------------------------------------------------------------

    bool mAllocate;          /** Prevents repeated allocations during rolling-horizon updates */
    bool mExportIndicators;  /** Whether component-specific indicators should be exported */
    std::string mControl;    /** Control mode used by the model (e.g. ON/OFF, MPC) */

    // ---------------------------------------------------------------------
    // Input parameters (settings, time series, impacts, performance) 
    // ---------------------------------------------------------------------

    InputParam* mInputParam;          /** Constant parameters from settings file */
    InputParam* mInputTimeSeries;     /** Time-series data from description files */
    InputParam* mInputEnvImpacts;     /** Environmental impacts selected by the user */
    InputParam* mInputPortImpacts;    /** Port-level environmental impacts */
    InputParam* mTSInputPortImpacts;  /** Time-series of port environmental impacts */
    InputParam* mInputPerfParam;      /** Performance parameters from CSV data files */
    InputParam* mInputIndicators;     /** Indicators selected for export */

    // ---------------------------------------------------------------------
    // Weight & sizing 
    // ---------------------------------------------------------------------

    double mWeight;                                /** Component weight (used in size optimization) */
    bool   mLPWeightOptimization;                  /** Whether LP sizing is used for weight optimization */
    bool   mLPModelOnly;                           /** Whether only the LP model is used (no MILP) */
    std::string mWeightUnit;                       /** Unit of the weight parameter */
    std::vector<std::string> mPossibleWeightUnits; /** Allowed units for weight */
    double mMaxValue;                              /** Maximum allowed size value */
    double mMinValue;                              /** Minimum allowed size value */

    // ---------------------------------------------------------------------
    // Time-step & horizon management 
    // ---------------------------------------------------------------------

    std::vector<double> mTimeSteps;              /** Time-step vector (variable or constant) */
    int      mHorizon;                           /** Number of time steps */
    uint     mNpdtPast;                          /** Number of past time steps */
    uint64_t mTimeStepBeginLP;                   /** First time step where LP model is used */
    uint64_t mTimeStepBeginForecast;             /** First time step where forecast model is used */
    uint64_t mMilpNpdt;                          /** Number of MILP time steps */
    uint64_t mDecreaseOptimizationHorizon;       /** Whether decreasing-horizon mode is active */
    const uint* mptrAbsoluteTimeStep;            /** Pointer to current absolute time step */
    const uint* mptrTimeshift;                   /** Pointer to time-shift value */
    const uint* mptrFuturesize;                  /** Pointer to future-size value */
    bool mUseVariableTimeSteps;                  /** Whether variable time steps are enabled */

    // ---------------------------------------------------------------------
    // Typical periods 
    // ---------------------------------------------------------------------

    bool mUseTypicalPeriods;                     /** Whether typical periods are used */
    uint mTypicalPeriods;                        /** Number of typical periods */
    uint mNDtTypicalPeriods;                     /** Number of time steps per typical period */
    uint mCondensedNpdt;                         /** Number of condensed time steps */
    std::vector<int> mVectTypicalPeriods;        /** Mapping of time steps to typical periods */
    std::vector<int> mFullVectTypicalPeriods;    /** Full mapping of time steps (expanded from typical periods) */

    // ---------------------------------------------------------------------
    // Economic state 
    // ---------------------------------------------------------------------

    double mHistVariableCostsDiscounted; /** Historical discounted variable costs */

    // ---------------------------------------------------------------------
    // Environmental impacts 
    // ---------------------------------------------------------------------

    std::vector<EnvImpact*>  mEnvImpacts;              /** List of environmental impact objects */
    std::vector<std::string> mEnvImpactsList;          /** Names of environmental impacts */
    std::vector<std::string> mEnvImpactsShortNamesList;/** Short names of environmental impacts */
    std::vector<std::string> mEnvImpactUnitsList;      /** Units of environmental impacts */
    std::vector<double>      mEnvImpactCosts;          /** Cost coefficients for environmental impacts */

    // ---------------------------------------------------------------------
    // Size results across cycles 
    // ---------------------------------------------------------------------

    std::vector<double> mOptimalSizeAllCycles; /** Optimal size for each cycle (planning horizon) */

    // ---------------------------------------------------------------------
    // State & startup/shutdown variables 
    // ---------------------------------------------------------------------

    bool mAddStateVariable;                  /** Whether ON/OFF state variables should be added */
    MIPModeler::MIPData1D       mHistState;  /** Historical state values (rolling horizon) */
    MIPModeler::MIPVariable1D   mState;      /** ON/OFF state variable */
    MIPModeler::MIPExpression1D mExpState;   /** Expression for state variable */
    int  mAbsInitialState;                   /** Absolute initial state value */

    bool mAddStartUpShutDownVariable;        /** Whether startup/shutdown variables should be added */
    MIPModeler::MIPData1D       mHistStartUp;/** Historical startup values (rolling horizon) */
    MIPModeler::MIPVariable1D   mStartUp;    /** Startup variable */
    MIPModeler::MIPExpression1D mExpStartUp; /** Expression for startup variable */

    MIPModeler::MIPData1D       mHistShutDown; /** Historical shutdown values (rolling horizon) */
    MIPModeler::MIPVariable1D   mShutDown;     /** Shutdown variable */
    MIPModeler::MIPExpression1D mExpShutDown;  /** Expression for shutdown variable */

    // ---------------------------------------------------------------------
    // Typical-period condensation flags 
    // ---------------------------------------------------------------------

    bool mCondenseVariablesOnTP;        /** Whether variables are condensed on typical periods */
    bool mCondenseBinariesOnly;         /** Whether only binary variables are condensed */
    bool mActivateConstraintsBetweenTP; /** Whether constraints between typical periods are active */
};

#endif // SUBMODEL_H
