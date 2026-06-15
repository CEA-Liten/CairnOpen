#ifndef TechnicalSubModel_H
#define TechnicalSubModel_H

class TechnicalSubModel;

#include "SubModel.h"

extern bool CAIRNCORESHARED_EXPORT isBlended(class SubModel* ap_Model);

struct SEnvImpact {
    std::string Name;
    std::string ShortName = "";
    std::string Unit = "";  
};

using namespace IndicatorNames;

class CAIRNCORESHARED_EXPORT TechnicalSubModel : public SubModel
{
public:
    TechnicalSubModel(CairnObject* aParent = nullptr);
    ~TechnicalSubModel();

    virtual void setTimeData();

    /** -------------------------------------------------------------------------------------------------------------- */
    void buildModel() override final; /* prevent models from overriding buildModel() */
    void setExpInstalled();
    virtual void computeGeometricContribution();   /** MILP Model description : geometric expressions */
    virtual void computeEnvContribution();        /** MILP Model description : environment expressions */
    virtual void computeEconomicalContribution(); /** MILP Model description : economical expressions */
    void computeNetOpexContribution();            /** Compute Net Opex contribution expression */
    virtual void computeAgeingModelContribution() { /* only relevant for ConverterSubModel */ };
    virtual void computeDefaultIndicators(const double* optSol);
    void resetHistStoredVaues();
    /** -------------------------------------------------------------------------------------------------------------- */

    void removeImpactSettings(const std::string& impactName);
    void cleanNonSelectedEnvImpacts();
    virtual void createEnvImpacts();

    // ------------------------- Defaul Parameters ----------------------------------------
    virtual void declareDefaultModelConfigurationParameters() 
    {
        SubModel::declareDefaultModelConfigurationParameters();
        //bool
        addParameter("EcoInvestModel", &mEcoInvestModel, true, false, true, "Use EcoInvestModel - ie Use Capex and Opex if true", "", "EcoInvestModel");  
        addParameter("EnvironmentModel", &mEnvironmentModel, false, false, true, "Use EnvironmentModel", "", "EnvironmentModel");  
        addParameter("GeometryModel", &mGeometryModel, false, false, true, "Use GeometryModel", "", "GeometryModel");     
        addParameter("LPModelONLY", &mLPModelOnly, false, false, true, "Use LP Model - ie integer variables imposed or relaxed to real variables if true", "");    
        addParameter("PiecewiseCapex", &mPiecewiseCapex, false, false, true, "Provide a map of capex and sizes - see performances maps in doc", "", "EcoInvestModel");
        addParameter("PiecewiseArea", &mPiecewiseArea, false, false, true, "Provide a map of areas and component sizes - see performances maps in doc", "", "GeometryModel");
        addParameter("PiecewiseVolume", &mPiecewiseVolume, false, false, true, "Provide a map of volumes and sizes - see performances maps in doc", "", "GeometryModel");
        addParameter("PiecewiseMass", &mPiecewiseMass, false, false, true, "Provide a map of masses and sizes - see performances maps in doc", "", "GeometryModel");
        addParameter("CondenseVariablesOnTP", &mCondenseVariablesOnTP, false, false, true, "To condense variables on the typical periods definition except for the storage state : allows storage between typical periods", "", "AddOperationConstraints"); 
        addParameter("SeasonalPrevisions", &mSeasonalPrevisions, false, false, true, "Use long term time series forecasts", "", "TimeSeriesForecast");

        //int
        mNbInputPorts = getNbPorts(KCONS());
        mNbOutputPorts = getNbPorts(KPROD());
    
        if (!mVariablePortNumber) { 
            mNbInputFlux = mNbInputPorts;
            mNbOutputFlux = mNbOutputPorts;
        }

        /* Env Impacts are created here because config params are declared first. 
        Otherwise, they should be created in MilpComponent::initSubModelConfiguration. 
        Note, TecEcoAnalysis should be execluded ! 
        */
        createEnvImpacts();
        declareEnvImpactConfigurationParameters();
    }

    void declareEnvImpactConfigurationParameters()
    {
        for (const auto& impact: mEnvImpacts) {
            if (!impact->isNewlySelected()) continue;
            std::size_t j = 0;
            for (const auto& port : mListPort) {
                impact->addConfigParameters(port->Name(), j++);
            }
        }
    }
    
    virtual void declareDefaultModelParameters()
    {
        //bool
        addParameter("LPWeightOptimization", &mLPWeightOptimization, false, false, true, "Use integer Weight if false ", ""); /** Use sizing based on Weight if true - default is false*/
        //double 
        addParameter("Weight", &mWeight, 1., false, true);	/** Weight of identical component, use negative value for optimization */
        addParameter("LifeTime", &mLifeTime, 1., false, SFunctionFlag({ eFTypeOrNot, { &mEcoInvestModel, &mEnvironmentModel} }), "LifeTime in years", "Year", "EcoInvestModel");              /** LifeTime in years */ 
        addParameter("Capex", &mCapex, 0., &mEcoInvestModel, &mEcoInvestModel, "Elementary Capex in Euro per unit installed nominal storage or production capacity", SFunctionUnit({ eFTypeDivision, { pCurrency(), pOptimalSizeUnit() }}), "EcoInvestModel");                /** Elementary Capex in Euro per unit installed nominal power, flowrate or capacity  */
        addParameter("TotalCapexCoefficient", &mTotalCapexCoefficient, 1., false, &mEcoInvestModel, "Multiplicative coefficient on elementary Capex", "-", "EcoInvestModel");  /** Multiplicative coefficient on elementary Capex to account for fees, land taxes, structure costs... ie cost += Capex*mTotalCapexCoefficient */
        addParameter("TotalCapexOffset", &mTotalCapexOffset, 0., false, &mEcoInvestModel, "Additive offset coefficient on elementary Capex", pCurrency(), "EcoInvestModel");  /** Offset coefficient on elementary Capex to account for fees, land taxes, structure costs... ie cost += Capex*mTotalCapexCoefficient */
        addParameter("FixedOpex", &mFixedOpex, 0., &mEcoInvestModel, &mEcoInvestModel, "Fixed Opex in proportion of Elementary Capex", "%CAPEX/year", "EcoInvestModel");					/** Opex in percent of Elementary Capex, ie Opex cost += Capex * Opex * levelization + Sum(VariableCosts*Timestep*levelization) */
        addParameter("FixedOpexConstant", &mFixedOpexConstant, 0., false, &mEcoInvestModel, "The constant part of the Opex", "-", "EcoInvestModel");					/** The constant part of the yearly Opex : Opex = mFixedOpex * mCapex + mFixedOpexConstant */
        addParameter("VariableOpex", &mVariableOpex, 0., false, true, "Variable Opex", "Currency/EnergyUnit", "EcoInvestModel");
        addParameter("Replacement", &mReplacement, 0., false, &mEcoInvestModel, "Replacement costs in proportion of Elementary Capex", "%CAPEX", "EcoInvestModel");   /** Replacement costs in percent of Elementary Capex, ie cost += Capex* Replacement*Use_Time*levelization */
        addParameter("ReplacementConstant", &mReplacementConstant, 0., false, &mEcoInvestModel, "The constant part of the replacement cost", pCurrency(), "EcoInvestModel");   /** The constant part of the Replacement cost : mReplacement * mCapex + mReplacementConstant */
        addParameter("MinSize", &mMinSize, 0., false, true, "Minimal size of the component", pOptimalSizeUnit()); /** Minimum capacity  */

        //bool
        addParameter("TryRelaxationCapex", &mTryRelaxationCapex, true, SFunctionFlag({ eFTypeNotAnd, {}, { &mEcoInvestModel,&mPiecewiseCapex} }), SFunctionFlag({ eFTypeNotAnd, {}, { &mEcoInvestModel,&mPiecewiseCapex} }), "If the CAPEX is a convex function of size the linearization variables will be continuous", "bool", "EcoInvestModel");
        addParameter("TryRelaxationArea", &mTryRelaxationArea, true, SFunctionFlag({ eFTypeNotAnd, {}, { &mGeometryModel, &mPiecewiseArea} }), SFunctionFlag({ eFTypeNotAnd, {}, { &mGeometryModel, &mPiecewiseArea} }), "If the Area is a convex function of size the linearization variables will be continuous", "bool", "GeometryModel");
        addParameter("TryRelaxationVolume", &mTryRelaxationVolume, true, SFunctionFlag({ eFTypeNotAnd, {}, { &mGeometryModel, &mPiecewiseVolume} }), SFunctionFlag({ eFTypeNotAnd, {}, { &mGeometryModel, &mPiecewiseVolume} }), "If the Volume is a convex function of size the linearization variables will be continuous", "bool", "GeometryModel");
        addParameter("TryRelaxationMass", &mTryRelaxationMass, true, SFunctionFlag({ eFTypeNotAnd, {}, { &mGeometryModel, &mPiecewiseMass} }), SFunctionFlag({ eFTypeNotAnd, {}, { &mGeometryModel, &mPiecewiseMass} }), "If the Mass is a convex function of size the linearization variables will be continuous", "bool", "GeometryModel");

        //double
        addParameter("Area", &mArea, 0., SFunctionFlag({ eFTypeNotAnd, { &mPiecewiseArea}, { &mGeometryModel} }), SFunctionFlag({ eFTypeNotAnd, { &mPiecewiseArea}, { &mGeometryModel} }), "footprint occupied by component per installed unit or weight - m2/OptimalSizeUnit", SFunctionUnit({ eFTypeDivision, {pOptimalSizeUnit()}, "", "m2"}), "GeometryModel");
        addParameter("Volume", &mVolume, 0., SFunctionFlag({ eFTypeNotAnd, { &mPiecewiseVolume}, { &mGeometryModel} }), SFunctionFlag({ eFTypeNotAnd, { &mPiecewiseArea}, { &mGeometryModel} }), "footprint occupied by component per installed unit or weight - m3/OptimalSizeUnit", SFunctionUnit({ eFTypeDivision, {pOptimalSizeUnit()}, "", "m3" }), "GeometryModel");
        addParameter("Mass", &mMass, 0., SFunctionFlag({ eFTypeNotAnd, { &mPiecewiseMass}, { &mGeometryModel} }), SFunctionFlag({ eFTypeNotAnd, { &mPiecewiseArea}, { &mGeometryModel} }), "footprint occupied by component per installed unit or weight - kg/OptimalSizeUnit", SFunctionUnit({ eFTypeDivision, {pOptimalSizeUnit()}, "", "kg" }), "GeometryModel");
        //vector
        addTimeSeries("ComponentAvailability", &mComponentAvailabilityTS, false, true, "Timeseries used to simulate unavailability during failures and maintenance: available if 1 and unavailable if 0", "", "Base", 1, 0, 1);

        addPerfParam("CapexCapacitySetPoint", &mCapexCapacitySetPoint, SFunctionFlag({ eFTypeNotAnd, {}, { &mEcoInvestModel,&mPiecewiseCapex} }), SFunctionFlag({ eFTypeNotAnd, {}, { &mEcoInvestModel,&mPiecewiseCapex} }), "name of vector capacity that will be defined from DataFile specification by the User", ""); /** length of vectors of capacity that will be defined from DataFile specification by the User */
        addPerfParam("CapexSetPoint", &mCapexSetPoint, SFunctionFlag({ eFTypeNotAnd, {}, { &mEcoInvestModel,&mPiecewiseCapex} }), SFunctionFlag({ eFTypeNotAnd, {}, { &mEcoInvestModel,&mPiecewiseCapex} }), "name of vector cost that will be defined from DataFile specification by the User", ""); /** length of vectors of cost that will be defined from DataFile specification by the User */

        addPerfParam("AreaCapacitySetPoint", &mAreaCapacitySetPoint, SFunctionFlag({ eFTypeNotAnd, {}, { &mGeometryModel, &mPiecewiseArea} }), SFunctionFlag({ eFTypeNotAnd, {}, { &mGeometryModel, &mPiecewiseArea} }), "name of vector area capacity that will be defined from DataFile specification by the User", "");
        addPerfParam("AreaSetPoint", &mAreaSetPoint, SFunctionFlag({ eFTypeNotAnd, {}, { &mGeometryModel, &mPiecewiseArea} }), SFunctionFlag({ eFTypeNotAnd, {}, { &mGeometryModel, &mPiecewiseArea} }), "name of vector area SetPoint that will be defined from DataFile specification by the User", "");
          
        addPerfParam("VolumeCapacitySetPoint", &mVolumeCapacitySetPoint, SFunctionFlag({ eFTypeNotAnd, {}, { &mGeometryModel, &mPiecewiseVolume} }), SFunctionFlag({ eFTypeNotAnd, {}, { &mGeometryModel, &mPiecewiseVolume} }), "name of vector volume capacity that will be defined from DataFile specification by the User", "");
        addPerfParam("VolumeSetPoint", &mVolumeSetPoint, SFunctionFlag({ eFTypeNotAnd, {}, { &mGeometryModel, &mPiecewiseVolume} }), SFunctionFlag({ eFTypeNotAnd, {}, { &mGeometryModel, &mPiecewiseVolume} }), "name of vector volume SetPoint that will be defined from DataFile specification by the User", "");
       
        addPerfParam("MassCapacitySetPoint", &mMassCapacitySetPoint, SFunctionFlag({ eFTypeNotAnd, {}, { &mGeometryModel, &mPiecewiseMass} }), SFunctionFlag({ eFTypeNotAnd, {}, { &mGeometryModel, &mPiecewiseMass} }), "name of vector mass capacity that will be defined from DataFile specification by the User", "");
        addPerfParam("MassSetPoint", &mMassSetPoint, SFunctionFlag({ eFTypeNotAnd, {}, { &mGeometryModel, &mPiecewiseMass} }), SFunctionFlag({ eFTypeNotAnd, {}, { &mGeometryModel, &mPiecewiseMass} }), "name of vector mass SetPoint that will be defined from DataFile specification by the User", "");
        
        //EnvImpacts
        declareEnvImpactParameters();
    }

    void declareEnvImpactParameters()
    {
        for (const auto& impact : mEnvImpacts) {
            if (!impact->isNewlySelected()) continue;
            impact->addGreyParameters();
            impact->addPerfParameters();
            std::size_t j = 0;
            for (const auto& port : mListPort) {
                impact->addPortParameters(port->Name(), j++, mMainCarrier);
            }
        }
    }

    virtual void declareDefaultModelInterface()
    {
        /* Register IO expressions to be exported (published) as results (to the external, e.g., Pegase)
           Note, the size of 1D IO expressions is always equal to mHorizon
        */
        SubModel::declareDefaultModelInterface();

        //General
        addIO("isInstalled", &mExpInstalled, true, "bool", "Binary equals 1 if installed");  

        addIO("VariableCosts", &mExpVariableCosts, true, pCurrency(), "Computed variable costs resulting from material/fuel consumption");  
        setVariableCostsExpression("VariableCosts");  // defines default expression to be used for VariableCosts computation and use in Economic analysis

        //EcoInvestModel
        addIO("Capex", &mExpCapex, &mEcoInvestModel, mMainCarrier->pFluxUnit(), "Computed initial investment costs Capex");  
        addIO("Opex", &mExpOpex, &mEcoInvestModel, mMainCarrier->pStorageUnit(), "Computed operational cost Net Opex");     
        addIO("FixedOpex", &mExpFixedOpex, &mEcoInvestModel, mMainCarrier->pStorageUnit(), "Computed operational cost Fixed Opex");  
        addIO("Replacement", &mExpReplacement, &mEcoInvestModel, mMainCarrier->pFluxUnit(), "Computed variable replacement cost");  

        setCapexExpression("Capex");        // defines default expression to be used for OptimalSize computation and use in Economic analysis
        setOpexExpression("Opex");          // defines default expression to be used for OptimalSize computation and use in Economic analysis
        setReplacementExpression("Replacement");  // defines default expression to be used for OptimalSize computation and use in Economic analysis

        //EnvironmentModel -- EnvImpact
        declareEnvImpactInterface();

        //GeometryModel
        addIO("Area", &mExpArea, &mGeometryModel, "m2");
        addIO("Volume", &mExpVolume, &mGeometryModel, "m3");
        addIO("Mass", &mExpMass, &mGeometryModel, "kg");

        //State
        addControlIO("State", &mExpState, &mAddStateVariable, "bool", &mHistState, nullptr, true, "state of the component : 1 if on 0 if off");  
        /* Note: ProductionUC uses ControlIO for StartUp and ShutDown */
        addIO("StartUp", &mExpStartUp, &mAddStartUpShutDownVariable, "bool");
        addIO("ShutDown", &mExpShutDown, &mAddStartUpShutDownVariable, "bool");

        /* Register non-IO 1D-expressions in order to automatically allocate and close them */
        addExp(&mExpVariableOpex, &mHorizon);

    }

    void declareEnvImpactInterface() {
        for (EnvImpact* impact : mEnvImpacts)
        {
            //Declare for all impacts because all IOs are removed in MilpComponent::declareIOVariables()
            //if (!impact->isNewlySelected()) continue;
            impact->addIOExpressions();
            setEnvImpactCostExpression(impact->Name() + " Env impact cost");  // defines default expression to be used for OptimalSize computation and use in Economic analysis
            setEnvImpactMassExpression(impact->Name() + " Env impact mass");  // defines default expression to be used for OptimalSize computation and use in Environmental analysis
            setEnvGreyImpactCostExpression(impact->Name() + " Env grey impact cost");  // defines default expression to be used for OptimalSize computation and use in Economic analysis
            setEnvGreyImpactMassExpression(impact->Name() + " Env grey impact mass");  // defines default expression to be used for OptimalSize computation and use in Environmental analysis
        }
    }

    virtual void declareDefaultModelIndicators() 
    {
        mInputIndicators->addIndicator("CAPEX", &mCapexContribution, &mEcoInvestModel, "Investment cost", pCurrency(), "CAPEX"); 
        mInputIndicators->addIndicator(IS_INSTALLED, &mExistence, &mExportIndicators, "Component installed", "-", "IsInstalled");
        mInputIndicators->addIndicator("Annual operation cost", &mOpexContribution, &mExportIndicators, "OPEX + replacement + buying cost + other cost - income", pCurrency(), "OPEX"); 
        mInputIndicators->addIndicator("Annual fixed OPEX", &mFixedOpexContribution, &mEcoInvestModel, "Opex part excluding energy costs", pCurrency(), "PureOPEX");
        mInputIndicators->addIndicator("Annual variable OPEX", &mVariableOpexContribution, &mExportIndicators, "Opex part linked to Variable OPEX parameter", pCurrency(), "VariableOPEX");
        mInputIndicators->addIndicator("Annual replacement costs", &mReplacementPart, &mEcoInvestModel, "Replacement part", pCurrency(), "Replacement"); 

        mInputIndicators->addIndicator("Mass", &mMassContribution, &mGeometryModel, "Mass", "kg", "Mass");
        mInputIndicators->addIndicator("Area", &mAreaContribution, &mGeometryModel, "Area", "m2", "Area");
        mInputIndicators->addIndicator("Volume", &mVolumeContribution, &mGeometryModel, "Volume", "m3", "Volume");

        //EnvImpact
        declareEnvImpactIndicators();
    }

    void declareEnvImpactIndicators() {
        //Declare for all impacts for now because all indicators are removed in MilpComponent::declareIOVariables()
        for (EnvImpact* impact : mEnvImpacts) {
            //if (!impact->isNewlySelected()) continue;
            impact->addIndicators();
        }
    }

    void addMinimumCapacity(double& aSizeMax); 

    int checkConsistency() override;

    std::vector<double> getCapexContribution() { return mCapexContribution; }

    void computePiecewiseContribution(const MIPModeler::MIPData1D& aCapacitySetPoint, const MIPModeler::MIPData1D& aCostSetPoint, 
        const bool& aTryRelaxation, const double& aOffset, MIPModeler::MIPExpression& aExp);

    bool* pEnvironmentModel() { return &mEnvironmentModel; }
    bool* pEcoInvestModel() { return &mEcoInvestModel; }

    double LifeTime() const { return mLifeTime; }

    int getNbPorts(const std::string& direction) {
        int nbPorts = 0;
        for(MilpPort * lptrport: mListPort)
        {
            if (lptrport->Direction() == direction)
            {
                nbPorts++;
            }
        }
        return nbPorts;
    }

protected:

    void computeAllContribution();       /** MILP Model description : all expressions */

    /** Flags */
    bool mEcoInvestModel;               /** bool indicating use of Economic Model if = true - default to true*/
    bool mEnvironmentModel;             /** bool indicating use of Environment Model if = true - default to true*/
    bool mGeometryModel;                /** bool indicating use of Geometric Model if = true - default to false*/

    MIPModeler::MIPVariable0D mVarInstalled;
    MIPModeler::MIPExpression mExpInstalled;

    /** if another temporal profile is forecasted over the long-term */
    bool mSeasonalPrevisions;

    std::vector<double> mComponentAvailabilityTS;      /** time series for component use availability */

    /** Economic model*/
    MIPModeler::MIPVariable0D mInvest;      /** MILP 0D Variable on component Optimal capacity (power or storage) */
    MIPModeler::MIPData1D mCapexCapacitySetPoint;
    MIPModeler::MIPData1D mCapexSetPoint;
    bool mPiecewiseCapex;
    bool mTryRelaxationCapex;
    double mMinSize;            /**  minimum storage capacity for size optimization (kg mass on fluid vectors, MWh energy for electrical and thermal vectors) */

    /** Expressions */
    MIPModeler::MIPExpression mExpCapex;                        /** Capex contribution expression */
    MIPModeler::MIPExpression1D mExpOpex;                       /** Net Opex contribution expression */
    MIPModeler::MIPExpression1D mExpFixedOpex;                  /** Pure Opex contribution expression, in fraction of Capex */
    MIPModeler::MIPExpression1D mExpVariableOpex;               /** Variable Opex contribution expression, relative to a ref IO */

    MIPModeler::MIPExpression1D mExpReplacement;                /** Variable Replacement contribution expression, in Capex/h    */
    MIPModeler::MIPExpression1D mExpVariableCosts;    /** Variable costs contribution expression   */

    MIPModeler::MIPExpression mExpArea;
    MIPModeler::MIPExpression mExpVolume;
    MIPModeler::MIPExpression mExpMass;

    /** Indicators calculation */
    //Vector of two elements: first one is _PLAN and second one is HIST
    std::vector<double> mOptimalSize;
    std::vector<double> mTotalCostFunction;
    std::vector<double> mCapexContribution;
    std::vector<double> mExistence;
    std::vector<double> mWeightResult;
    std::vector<double> mOpexContribution;
    std::vector<double> mFixedOpexContribution;
    std::vector<double> mVariableOpexContribution;
    std::vector<double> mReplacementPart;
    std::vector<double> mVariableCosts;
    std::vector<double> mEnvImpactPart;
    std::vector<double> mEnvGreyImpactCost;
    //

    double mHistFixedOpexContributionDiscounted;
    double mHistReplacementPartDiscounted;

    std::vector<double> mSumUp;
    std::vector<double> mRunningTime;
    std::vector<double> mRunningTimeAvlblt;
    std::vector<double> mMaxRunningTime;
    std::vector<double> mChargingTime;
    std::vector<double> mDischargingTime;
    std::vector<double> mEfficiency_Ageing;
    std::map<std::string, std::vector<double>> mExpEchData;
    std::map<std::string, std::vector<double>> mConsumptionMap;
    std::map<std::string, std::vector<double>> mConsLvlTotMap;
    std::map<std::string, std::vector<double>> mConsPFMap;
    std::map<std::string, std::vector<double>> mRateOfUse;
    std::map<std::string, std::vector<double>> mConsMeanMap;
    std::map<std::string, std::vector<double>> mProductionMap;
    std::map<std::string, std::vector<double>> mProdLvlTotMap;
    std::map<std::string, std::vector<double>> mProdMeanMap;
    std::map<std::string, std::vector<double>> mProdContributionMap;
    std::map<std::string, std::vector<double>> mChargedEnergyMap;
    std::map<std::string, std::vector<double>> mDischargedEnergyMap;
    std::map<std::string, std::vector<double>> mNLevChargedEnergyMap;
    std::map<std::string, std::vector<double>> mNLevDischargedEnergyMap;
    std::map<std::string, std::vector<double>> mChargedMeanMap;
    std::map<std::string, std::vector<double>> mDischargedMeanMap;
    std::map<std::string, std::vector<double>> mNbCylesMap;

    /** footprint optional attributes (geometry Model) */
    double mArea;
    double mVolume;
    double mMass;

    std::vector<double> mAreaContribution;
    std::vector<double> mVolumeContribution;
    std::vector<double> mMassContribution;

    MIPModeler::MIPData1D mAreaCapacitySetPoint;
    MIPModeler::MIPData1D mAreaSetPoint;
    MIPModeler::MIPData1D mVolumeCapacitySetPoint;
    MIPModeler::MIPData1D mVolumeSetPoint;
    MIPModeler::MIPData1D mMassCapacitySetPoint;
    MIPModeler::MIPData1D mMassSetPoint;

    bool mTryRelaxationArea;
    bool mTryRelaxationVolume;
    bool mTryRelaxationMass;

    bool mPiecewiseArea;
    bool mPiecewiseVolume;
    bool mPiecewiseMass;
 
    /** Economic model input data */
    double mCapex;                             /** Capex Component Elementary Capex per unit of max installed capacity (production or storage) mVarSizeMax - SourceLoad unit cost of component*/
    double mTotalCapexCoefficient;             /** Capex multiplicative factor on Elementary Capex to account for fees, infrastructures, ... */
    double mTotalCapexOffset;                  /** Capex constant on Elementary Capex to account for fees, infrastructures, ... */

    //Opex = mFixedOpex * mCapex + mFixedOpexConstant and Replacement = mReplacement * mCapex + mReplacementConstant
    double mFixedOpex;                              /** Opex yearly Opex of component, per unit Capex **/
    double mVariableOpex;
    double mReplacement;                       /** Replacement cost, in proportion of Capex **/
    double mLifeTime;                           /** Component LifeTime in years **/
    double mFixedOpexConstant;                       /** The constant part of the yearly Opex : Opex = mFixedOpex * mCapex + mFixedOpexConstant **/
    double mReplacementConstant;                /** The constant part of the Replacement cost : mReplacement * mCapex + mReplacementConstant**/
};

#endif // TechnicalSubModel_H