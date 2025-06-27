/**
* \file		SourceLoad.h
* \brief	SourceLoad model
* \version	1.0
* \author	Alain Ruby
* \date		08/02/2019
*/

#ifndef SourceLoad_H
#define SourceLoad_H

#include "globalModel.h"
#include "SourceLoadSubModel.h"

//linkPerseeModelClass SourceLoad.h SourceLoadFlexible.h SourceLoadMinMax.h

/**
* \details
This component represents either a source (injection) or a load (extraction). To extend the definition, for instance, a source can be a powerplant or a photovoltaic panel, 
whilst a load could be the demand of hydrogen of a building. 

Thus, an **imposed flow** of energy or material can be inserted as input for:

- Power for electrical or thermal carriers
- Flow rates for fluids, biomass, etc.

Main Features
-------------

- **MaxFlow** limits the absolute value of injected or extracted flow.
- **Weight** parameter scales the flow profile (e.g., to represent multiple units or the installation of a certain amount of m� for a plant).

- Different strategies of load handling can be implemented:

  - **Load Shedding**: optional load shedding model for demand-side management (available only for loads, compatible with Rolling Horizon usage).
  - **Peak Shaving**: optional peak shaving model for demand-side management (available only for loads).
  - **Optimization of Price or Size**:
    - Can optimize the size (capacity) or the price signal for injected/extracted flow.


Focus
-----

- **Load shedding**:

  .. figure:: ../images/Shedding.JPG
    :alt: IO Shedding
    :name: IOShedding
    :width: 500
    :align: center

    Load Shedding Graphics


 This method introduces optional **load shedding** capability for a load component.
 Load shedding represents the deliberate reduction of demand when it's beneficial or necessary 
 (e.g., during system stress or high price events). This model could be used for 
- Demand Response modeling
- Robust optimization scenarios with load flexibility
- Scenario analysis for energy-constrained systems

- **Peak Shaving**:

  .. figure:: ../images/Shaving.JPG
    :alt: IO Shaving
    :name: IOShaving
    :width: 500
    :align: center

    Peak Shaving Graphics

This method introduces optional **peak shaving** capability for a load component. 
Peak shaving represents the redistribution of charge when a peak is highlighted, over hours where load is less important. 
The imposed flow (ImposedFlow) can be balanced in this way: it can be increased or decreased by MaxEffect but the total energy over the period TimeSpan must be conserved.

The imposed quantity depends on the connected energy vector.

ImposedFlow and MaxFlow define the input data on which to apply flexibility:

 - ImposedFlow: the time distribution to be balanced using flexibility.
 - MaxFlow: used to scale ImposedFlow. Scaling is done in order to set MaxFlow as the maximum of ImposedFlow. (The obtained distribution is referred to as "old flux")

.. caution:

 - Load shedding:

   - Is only allowed for loads (`mSens > 0.`).
   - Is incompatible with `mLPModelOnly = true`.
   - Requires that the past horizon (`mNpdtPast`) is larger than shedding activation/deactivation times if rolling horizon is used.

 - Do not forget to set EconomicModel to True if you want to consider costs

Generated Variables and Expressions
-----------------------------------

- Variables for controlled or weighted flux, shedding, and reactive power.

- Expressions for:

  - Total imposed flux
  - Input and output powers
  - Costs (regular and shedding)
*/
class MODELS_DECLSPEC SourceLoad : public SourceLoadSubModel {

public:
    //----------------------------------------------------------------------------------------------------
    SourceLoad(QObject* aParent);
    ~SourceLoad();
    //----------------------------------------------------------------------------------------------------
    int checkConsistency();

    void buildModel();
    //----------------------------------------------------------------------------------------------------
    void computeEconomicalContribution();
    void computeAllIndicators(const double* optSol) override;
    //----------------------------------------------------------------------------------------------------
        //for external model testing
    void setTimeData();
    //----------------------------------------------------------------------------------------------------
        //Model Input Data
        // Units: use the following, instead of the IS Units leading to "scaling" troubles during solving step
        // Mass Flow    : kg/h
        // Power flow   : MW
        // Mass         : kg
        // Energy       : MWh
        // Time         : Hours
    void declareModelConfigurationParameters()
    {
        SourceLoadSubModel::declareDefaultModelConfigurationParameters();

        mInputParam->addToConfigList({ "EcoInvestModel","EnvironmentModel","AddThermalModel","ControlOptions","CompensationConstraints", "TimeSeriesForecast" });

        //re-declare these parameters to change their default values
        addParameter("EcoInvestModel", &mEcoInvestModel, false, false, true, "Use EcoInvestModel - ie Use Capex and Opex if true", "", { "EcoInvestModel" });    /** Use EcoInvestModel - ie Use Capex and Opex if true */
        addParameter("UseWeightOptimization", &mUseWeightOptimization, true, false, true, "Use sizing based on Weight if true", "", { "" }); // this way weight will be considered flat by default allowing for upward compatibility with previous computations
        addParameter("LPModelONLY", &mLPModelOnly, true, false, true, "Use LP Model - ie integer variables imposed or relaxed to real variables if true", "", { "" });          /** Use LP Model - ie binary variable imposed if true */

        //bool
        addParameter("UseWeightOptimization", &mUseWeightOptimization, true, false, true, "Use sizing based on Weight if true", "", { "" }); /** Use sizing based on Weight if true - default is false*/
        addParameter("UseControlledFlux", &mUseControlledFlux, false, false, true, "", "", { "ControlOptions" });   /** Optional parameter - If true: SourceLoadFlow will be set from another component using equality bus constraint instead of imposing SourceLoad flow from timeSeries - False by default */
        addParameter("UseWeightedFlux", &mUseWeightedFlux, false, false, true, "", "", { "ControlOptions" });       /** Optional parameter - If true: SourceLoad flow imposed by timeSeries will be weighted by another component variable using equality bus constraint - False by default */
        addParameter("AddHeatConsumerModel", &mAddHeatConsumerModel, false, false, true, "", "bool", { "" }); 	/** Optional parameter - If true model fluid flowrate corresponding to power source or load - default = false */
        addParameter("AddVariableCostModel", &mAddVariableCostModel, false, false, true); 	/** Optional parameter - If true add precomputed costs or revenues from timeseries UseProfileBuyPrice or UseProfileSellPrice or AddShedding - It can be used to account for variable costs in addition to or in subsitution of Capex and Opex contributions - default = false */
        addParameter("AddSheddingTS", &mAddSheddingTS, false, false, true); /** Optional parameter - If true compute the volume of power to be removed from the imposed profile for shedding - default = false */
        addParameter("ComputeOptimalPrice", &mComputeOptimalPrice, false, false, true); 	/** Optional parameter - If true compute constant optimal price - default = false */
        addParameter("SeasonalPrevisions", &mSeasonalPrevisions, false, false, true, "", "", { "TimeSeriesForecast" }); /** Optional parameter - If true: use forecast time series instead of historical timeseries - default = false */
        addParameter("SeasonalCosts", &mSeasonalCosts, false, false, true, "", "", { "TimeSeriesForecast" });/** Optional parameter - If true compute SeasonalCosts - default = false */
        addParameter("AddSheddingDetailed", &mAddSheddingDetailed, false, false, true); 	/** Optional parameter - If true compute the volume of power to be removed from the imposed profile for shedding - default = false */
        addParameter("AddPeakShavingDetailed", &mAddPeakShavingDetailed, false, false, true); 	/** Optional parameter - If true compute the volume of power to be smooth from the imposed profile for shaving - default = false */
        addParameter("AddStaticCompensation", &mAddStaticCompensation, false, false, true, "", "", { "CompensationConstraints" }); /** In the cases where reactive power in taken into account, this can compensate the reactive power of the components  */
        addParameter("FixedStaticCompensation", &mFixedStaticCompensation, false, false, true, "", "", { "CompensationConstraints" }); /** Choose id static compensation is free or not (default=false)*/
        //double
        addConfig("Direction", &mSens, -1.);           /**  Injection / Extraction direction - automatically set by PERSEE */
    }

    //----------------------------------------------------------------------------------------------------
        //Model Input Data
    inline void declareModelParameters()
    {
        SourceLoadSubModel::declareDefaultModelParameters();
        // Supported types are: double, int, std::vector<double> or std::vector<int>
        // addParameter to InputParam instance for input data coming from User File : maximum power, performance maps...
        // addParameter to mInputData instance for input data coming from Cairn/PEGASE memory : time series (coming from PEGASE), state variables...
        
        //bool
        //re-declare this parameter to change its default value
        addParameter("LPWeightOptimization", &mLPWeightOptimization, true, false, true, "Use integer Weight if false ", "", { "" }); /** Use sizing based on Weight if true - default is false*/

        //int
        addParameter("MaxTimeShedding", &mMaxTimeShedding, 0, &mAddSheddingDetailed, &mAddSheddingDetailed, "Time shedding will be on. Put LPmodelonly on false and EcoInvest model on true", "Time"); /** Minimum shedding time period, in number of timesteps */
        addParameter("MinSheddingStandBy", &mMinSheddingStandBy, 0, &mAddSheddingDetailed, &mAddSheddingDetailed, "Time before a new power cut happen", "Time");	/** Minimum stand-by time period, in number of timesteps */
        addParameter("TimeSpan", &mTimeSpan, 0, &mAddPeakShavingDetailed, &mAddPeakShavingDetailed, "TimeSpan number of timesteps periods to be used for flexibility."); 	/** */

        //double
        addParameter("CostShedding", &mCostShedding, 0., &mAddSheddingDetailed, &mAddSheddingDetailed, "Penalty Cost associated to the shedding", "Currency/PowerUnit"); /* Penalty introduced when power shedding is activated*/
        addParameter("MaxShedding", &mMaxShedding, 0., &mAddSheddingDetailed, &mAddSheddingDetailed, "Max Shedded Power", "PowerUnit"); /* Maximum power shedding on the imposed flow */
        addParameter("MaxFlow", &mMaxFlux, 1.e4, true, true, "", "FluxUnit");		/** Maximum injected or extracted flow */

        addParameter("StaticCompensationValue", &mStaticCompensationValue, 0., &mFixedStaticCompensation, &mFixedStaticCompensation, "Static Compensation imposed by the user", "", { "CompensationConstraints" }); /**  */
        addParameter("StaticCompensationValue", &mStaticCompensationValue, 0., mFixedStaticCompensation, mFixedStaticCompensation, "Static Compensation imposed by the user", "", { "CompensationConstraints" }); /**  */
        addParameter("MaxEffect", &mMaxEffect, 0, &mAddPeakShavingDetailed, &mAddPeakShavingDetailed, "Maximum quantity of flux for each timestep that can be removed from or added to the reference flux. It can be optimized by specifying a negative value", "FluxUnit");
        addParameter("MaxEffectCapex", &mMaxEffectCapex, 0, &mAddPeakShavingDetailed, &mAddPeakShavingDetailed, "Used for computing a contribution to the objective function. Applies proportionally on the maximum of the parameter/variable mMaxEffect.", "");
        addParameter("MaxEffectOpex", &mMaxEffectOpex, 0, &mAddPeakShavingDetailed, &mAddPeakShavingDetailed, "used for computing a contribution to the objective function. Applies proportionally on the maximum of the parameter/variable mMaxEffect.", "");
    
        //vector
        addTimeSeries("UseProfileLoadFlux", &mImposedFlux, SFunctionFlag({ eFTypeNotAnd, { &mUseControlledFlux} }), SFunctionFlag({ eFTypeNotAnd, { &mUseControlledFlux} }), "", "FluxUnit/WeightUnit" );	/** External Time series of Imposed flow injected (source) or extracted (sink) if UseControlledFlux not activated*/
        addTimeSeries("UseProfileEnergyPrice", &mEnergyPrice, &mAddVariableCostModel, &mAddVariableCostModel, " External TimeSeries of energy price defining variable cost for positive value or revenue if negative ", "Currency/StorageUnit");
        addTimeSeries("UseStartStopProfile", &mStartStopProfile, &mUseWeightedFlux, &mUseWeightedFlux, "", "", { "ControlOptions" }, 0);	/** Add imposed startstop profile weight from External Time series if mUseWeightedFlux activated*/
        addTimeSeries("UseProfileLoadFluxSeasonal", &mImposedFluxSeasonal, &mSeasonalPrevisions, &mSeasonalPrevisions, "", "FluxUnit/WeightUnit");
        addTimeSeries("UseProfileMaxShedding", &mMaxSheddingTS, &mAddSheddingTS, &mAddSheddingTS, "External TimeSeries defining the shedding", "PowerUnit"); /* External time series of power shedding giving the max that can be substracted to the imposed flux*/
        addTimeSeries("UseProfileCostShedding", &mCostSheddingTS, &mAddSheddingTS, &mAddSheddingTS, "External TimeSeries defining the cost of shedding", "Currency/PowerUnit"); /* External time series of the cost of power shedding over the desired time horizon*/
    }

    inline void declareModelInterface()
    {
        SourceLoadSubModel::declareDefaultModelInterface();
        // register expression for model Interface with global MilpProblem, Bus, other MilpComponent
        // Caution : Flux must be signed wrt to Bus balance impact : >0 if energy source, <0 else.
        if (mUseWeightedFlux)
            addIO("FluxWeight", &mExpFluxWeight, "Unit");       /** Input expression for flux weighting if mUseWeightedFlux=true */

        addIO("SourceLoadFlow", &mExpFlux, mEnergyVector->pFluxUnit()); /** Computed or Controlled Imposed flow injected (source) or extracted (sink) - Positive value means injection for Source field and extraction for Sink field */
        addIO("MaxFlow", &mExpSizeMax, mEnergyVector->pFluxUnit()); /** Maximum injected or extracted flow */
        addIO("Weight", &mExpSizeMax, "Unit");	/** Computed weight of identical component, if optimized else equals input weight */
        addIO("OptimalPrice", &mExpSizeMax, mCurrency);	/** Computed optimal price used by component, if optimized else equals input weight */
        addIO("OUTPUTFlux1", &mExpPowerOut, mEnergyVector->pFluxUnit()); /** Computed output power output port 1 */

        if (mAddHeatConsumerModel)
        {
            addIO("INPUTFlux1", &mExpPowerIn, mEnergyVector->pFluxUnit()); /** Computed output power output port 1 */
        }
        if (mAddStaticCompensation) {
            addIO("ReactivePower", &mExpReactivePower, mEnergyVector->pFluxUnit()); /** Reactive power associated to the production of the source load. If  static compensation is not given it is an optimized factor*/
            //addIO("OptimalStaticCompensation", &mExpStaticCompensation, "Units");
            //setOptimalSizeExpression("OptimalStaticCompensation");
        }
        if (mComputeOptimalPrice) {
            setOptimalSizeExpression("OptimalPrice");  // defines default expression should be used for OptimalSize computation and use in Economic analysis
            setOptimalSizeUnit(mCurrency);  // defines default expression should be used for OptimalSize computation and use in Economic analysis
        }
        else {
            setOptimalSizeExpression("Weight");  // defines default expression should be used for OptimalSize computation and use in Economic analysis
            setOptimalSizeUnit("Unit");  // defines default expression should be used for OptimalSize computation and use in Economic analysis
        }

        if (mAddSheddingDetailed)
        {
            addIO("PowerShedding", &mExpPowerShedding, mEnergyVector->pPowerUnit()); /* Shedded power */
            addIO("CostShedding", &mExpCostShedding, mEnergyVector->pPowerUnit(), mCurrency); /* Shedding penalty cost */
            addControlIO("OnShedding", &mExpShedOn, "bool", &mExpHistOn, &mOnIni); /** Shedding activation, 1 if shedding is activated, 0 otherwise */
            addControlIO("OffShedding", &mExpShedOff, "bool", &mExpHistOff, &mOffIni); /** Shedding deactivation, 1 if shedding is deactivated, 0 otherwise */
            addControlIO("StateShedding", &mExpShedState, "bool", &mShedStateIni, &mStateIni); /** Load shedding state, 1 if shedding, 0 otherwise */                      
        }
    }

    void declareModelIndicators() {
        SourceLoadSubModel::declareDefaultModelIndicators(&mExportIndicators);
    }

    bool isPriceOptimized();
    bool getAddVariableCostModel() { return mAddVariableCostModel; }
    void computeReactivePower();
    void addLoadShedding();
    void addPeakShaving();

    double getTemperature(const QString& direction);

    //----------------------------------------------------------------------------------------------------
protected:
    virtual int checkBusFlowBalanceVarName(MilpPort* port, int& inumberchange, QString& varUseCheck);

    //MILP Variable
    MIPModeler::MIPVariable1D mVarFluxGrid;
    MIPModeler::MIPVariable0D mVarOptimalPrice;
    MIPModeler::MIPVariable0D mVarMaxEffect;
    MIPModeler::MIPVariable1D mVarControlledFlux; // if mUseControlledFlux is true
    MIPModeler::MIPVariable1D mVarFluxWeight; // if mUseWeigthedFlux is true
    MIPModeler::MIPVariable1D mVarPowerShedding;
    MIPModeler::MIPVariable1D mShedState;
    MIPModeler::MIPVariable1D mShedOn;
    MIPModeler::MIPVariable1D mShedOff;

    // Rolling Horizon
    MIPModeler::MIPData1D mExpHistOn;
    MIPModeler::MIPData1D mExpHistOff;
    

    //technical output
    MIPModeler::MIPExpression1D mExpFlux;
    MIPModeler::MIPExpression1D mExpCost;
    MIPModeler::MIPExpression1D mExpPowerOut;
    MIPModeler::MIPExpression1D mExpPowerIn;
    MIPModeler::MIPExpression1D mExpFluxWeight;
    MIPModeler::MIPExpression1D mExpPowerShedding;
    MIPModeler::MIPExpression1D mExpCostShedding;
    MIPModeler::MIPExpression1D mExpShedState;
    MIPModeler::MIPExpression1D mExpShedOn;
    MIPModeler::MIPExpression1D mExpShedOff;
    MIPModeler::MIPExpression1D mExpSums;

    //technical input
    double mMaxFlux;
    double mMaxOptimalPrice;
    double mCostShedding;
    double mMaxShedding;
    double mMaxEffect;
    double mOnIni{ 0 };
    double mOffIni{ 0 };
    double mStateIni{ 0 };
    double mShedStateIni{ 0 };
    int mMaxTimeShedding;
    int mMinSheddingStandBy;
    int mTimeSpan;
    std::vector<double> mMaxSheddingTS;
    std::vector<double> mCostSheddingTS;
    std::vector<double> mEnergyPrice;
    std::vector<double> mImposedFlux;
    std::vector<double> mImposedFluxSeasonal;
    std::vector<double> mStartStopProfile;

    double mSens;

    bool mAddHeatConsumerModel;
    bool mAddVariableCostModel;
    bool mComputeOptimalPrice;
    bool mAddSheddingDetailed;
    bool mAddPeakShavingDetailed;
    bool mAddSheddingTS;
    bool mUseControlledFlux;
    bool mUseWeightedFlux;
    bool mSeasonalCosts;
    bool mFixedStaticCompensation = false;
    double mStaticCompensationValue = 0;

    //static compensation for reactive power
    bool mAddStaticCompensation = false;
    MIPModeler::MIPVariable0D mStaticCompensation;
    MIPModeler::MIPExpression mExpStaticCompensation;
    MIPModeler::MIPVariable1D mReactivePower;
    MIPModeler::MIPExpression1D mExpReactivePower;

    //economic input
    double mMaxEffectCapex;
    double mMaxEffectOpex;

    //temperature for heat consumer
    double mTemperature_in1;                   /** Inlet Temperature */
    double mTemperature_out1;                  /** Outlet Temperature **/
};

#endif

