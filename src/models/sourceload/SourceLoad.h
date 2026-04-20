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

//linkPerseeModelClass SourceLoad.h SourceLoadMinMax.h

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

.. caution:: 

  Load shedding and peak shaving can be used together but as the cost associated to peak shaving is yearly, it can be used to reduce the cost of load shedding, especially if the cost linked to load shedding is time dependent.

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

ImposedFlow defines the input data on which to apply flexibility:

 - ImposedFlow: the time distribution to be balanced using flexibility.

.. caution:

 - Load shedding:

   - Is only allowed for loads (`Sens() > 0.`).
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
    SourceLoad(CairnObject* aParent);
    ~SourceLoad();
    //----------------------------------------------------------------------------------------------------
    int checkConsistency();
    void computeInitialData() override;
    void computeModelContribution() override;
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

        //re-declare these parameters to change their default values
        addParameter("EcoInvestModel", &mEcoInvestModel, false, false, true, "Use EcoInvestModel - i.e. use Capex and Opex if true", "", "EcoInvestModel");
        addParameter("UseWeightOptimization", &mUseWeightOptimization, true, false, true, "Use sizing based on Weight if true - weight will be considered flat by default allowing for upward compatibility with previous computations", "");
        addParameter("LPModelONLY", &mLPModelOnly, true, false, true, "Use LP Model - i.e. integer variables imposed or relaxed to real variables if true", "");
        // bool
        addParameter("UseControlledFlux", &mUseControlledFlux, false, false, true, "Optional - If true: SourceLoadFlow will be set from another component using equality bus constraint instead of imposing SourceLoad flow from timeSeries - default = false", "", "ControlOptions");
        addParameter("UseWeightedFlux", &mUseWeightedFlux, false, false, true, "Optional - If true: SourceLoad flow imposed by timeSeries will be weighted by another component variable using equality bus constraint - default = false", "", "ControlOptions");
        addParameter("AddHeatConsumerModel", &mAddHeatConsumerModel, false, false, true, "Optional - If true model fluid flowrate corresponding to power source or load - default = false", "bool");
        addParameter("AddVariableCostModel", &mAddVariableCostModel, false, false, true, "Optional - If true add precomputed costs or revenues from timeseries UseProfileBuyPrice or UseProfileSellPrice or AddShedding - can be used to account for variable costs in addition to or in substitution of Capex and Opex contributions - default = false");
        addParameter("AddSheddingTS", &mAddSheddingTS, false, false, true, "Optional - If true compute the volume of power to be removed from the imposed profile for shedding - default = false");
        addParameter("ComputeOptimalPrice", &mComputeOptimalPrice, false, false, true, "Optional - If true compute constant optimal price - default = false");
        addParameter("SeasonalPrevisions", &mSeasonalPrevisions, false, false, true, "Optional - If true: use forecast time series instead of historical timeseries - default = false", "", "TimeSeriesForecast");
        addParameter("SeasonalCosts", &mSeasonalCosts, false, false, true, "Optional - If true compute SeasonalCosts - default = false", "", "TimeSeriesForecast");
        addParameter("AddSheddingDetailed", &mAddSheddingDetailed, false, false, true, "Optional - If true compute the volume of power to be removed from the imposed profile for shedding - default = false");
        addParameter("AddPeakShavingDetailed", &mAddPeakShavingDetailed, false, false, true, "Optional - If true compute the volume of power to be smoothed from the imposed profile for shaving - default = false");
        addParameter("AddStaticCompensation", &mAddStaticCompensation, false, false, true, "Optional - In cases where reactive power is taken into account, this can compensate the reactive power of the components", "", "CompensationConstraints");
        addParameter("FixedStaticCompensation", &mFixedStaticCompensation, false, false, true, "Optional - Choose if static compensation is free or not - default = false", "", "CompensationConstraints");
    }

    //----------------------------------------------------------------------------------------------------
        //Model Input Data
    inline void declareModelParameters()
    {
        SourceLoadSubModel::declareDefaultModelParameters();
        // Supported types are: double, int, std::vector<double> or std::vector<int>
        // addParameter to InputParam instance for input data coming from User File : maximum power, performance maps...
        
        //bool
        //re-declare this parameter to change its default value
        addParameter("LPWeightOptimization", &mLPWeightOptimization, true, false, true, "Use integer Weight if false ", ""); 

        //int
        addParameter("MaxTimeShedding", &mMaxTimeShedding, 0, &mAddSheddingDetailed, &mAddSheddingDetailed, "Minimum shedding time period, in number of timesteps. Put LPmodelonly on false and EcoInvest model on true", "Time"); /** Minimum shedding time period, in number of timesteps */
        addParameter("MinSheddingStandBy", &mMinSheddingStandBy, 0, &mAddSheddingDetailed, &mAddSheddingDetailed, "Minimum stand-by time period before a new power cut happen, in number of timesteps. Time ", "Time");	 
        addParameter("TimeSpan", &mTimeSpan, 1, &mAddPeakShavingDetailed, &mAddPeakShavingDetailed, "TimeSpan number of timesteps periods to be used for flexibility."); 	 

        //double
        addParameter("CostShedding", &mCostShedding, 0., &mAddSheddingDetailed, &mAddSheddingDetailed, "Penalty Cost associated to the shedding", SFunctionUnit({ eFTypeDivision, { pCurrency(), mMainCarrier->pPowerUnit()} }));  
        addParameter("MaxShedding", &mMaxShedding, 0., &mAddSheddingDetailed, &mAddSheddingDetailed, "Maximum shedding power on the imposed flow", mMainCarrier->pPowerUnit());  
        addParameter("MaxFlow", &mMaxFlux, 1.e4, true, true, "Maximum injected or extracted flow", mMainCarrier->pFluxUnit());		 
        addParameter("MaxPrice", &mMaxOptimalPrice, 100, &mComputeOptimalPrice, &mComputeOptimalPrice, "Maximum price", pCurrency());

        addParameter("StaticCompensationValue", &mStaticCompensationValue, 0., &mFixedStaticCompensation, &mFixedStaticCompensation, "Static Compensation imposed by the user", "", "CompensationConstraints");  
        
        //Peak shaving
        addParameter("MaxEffect", &mMaxEffect, 0., &mAddPeakShavingDetailed, &mAddPeakShavingDetailed, "Maximum quantity of flux for each timestep that can be removed from or added to the reference flux. It can be optimized by specifying a negative value", mMainCarrier->pFluxUnit());
        addParameter("MaxEffectCapex", &mMaxEffectCapex, 0., &mAddPeakShavingDetailed, &mAddPeakShavingDetailed, "Used for computing a contribution to the objective function. Applies proportionally on the maximum of the parameter mMaxEffect.", SFunctionUnit({ eFTypeDivision, { pCurrency(), mMainCarrier->pFluxUnit()} }) );
        addParameter("MaxEffectOpex", &mMaxEffectOpex, 0., &mAddPeakShavingDetailed, &mAddPeakShavingDetailed, "used for computing a contribution to the objective function. Applies proportionally on the product of  mMaxEffect and MaxEffectCapex (%MaxEffectCapex/year).", "-");
    
        //vector
        addTimeSeries("UseProfileLoadFlux", &mImposedFlux, SFunctionFlag({ eFTypeNotAnd, { &mUseControlledFlux} }), SFunctionFlag({ eFTypeNotAnd, { &mUseControlledFlux} }), "External Time series of Imposed flow injected (source) or extracted (load) if UseControlledFlux not activated", SFunctionUnit({ eFTypeDivision, { mMainCarrier->pFluxUnit(), &mWeightUnit} }), "Base", 0.0);	
        addTimeSeries("UseProfileEnergyPrice", &mEnergyPrice, &mAddVariableCostModel, &mAddVariableCostModel, " External TimeSeries of energy price defining variable cost for positive value or revenue if negative ", SFunctionUnit({ eFTypeDivision, { pCurrency(), mMainCarrier->pStorageUnit()} }), "Base", 0.0);
        addTimeSeries("UseStartStopProfile", &mStartStopProfile, &mUseWeightedFlux, &mUseWeightedFlux, "Add imposed startstop profile weight from External Time series if mUseWeightedFlux activated", "", "ControlOptions", 0.0);
        addTimeSeries("UseProfileLoadFluxSeasonal", &mImposedFluxSeasonal, &mSeasonalPrevisions, &mSeasonalPrevisions, "", SFunctionUnit({ eFTypeDivision, { mMainCarrier->pFluxUnit(), &mWeightUnit} }), "Base", 0.0);
        addTimeSeries("UseProfileMaxShedding", &mMaxSheddingTS, &mAddSheddingTS, &mAddSheddingTS, "External time series of power shedding giving the max that can be substracted to the imposed flux", mMainCarrier->pPowerUnit(), "Base", 0.0);
        addTimeSeries("UseProfileCostShedding", &mCostSheddingTS, &mAddSheddingTS, &mAddSheddingTS, "External TimeSeries defining the cost of shedding", SFunctionUnit({ eFTypeDivision, { pCurrency(), mMainCarrier->pPowerUnit()} }), "Base", 0.0);
    }

    inline void declareModelInterface()
    {
        SourceLoadSubModel::declareDefaultModelInterface();

        /* Register IO expressions to be exported (published) as results (to the external, e.g., Pegase)
           Caution : Flux must be signed wrt to Bus balance impact : >0 if energy source, <0 else.
        */
        
        if (mComputeOptimalPrice) {
            addSizeMaxIO("OptimalPrice", &mExpSizeMax, true, pCurrency());	/** Computed optimal price used by component, if optimized else equals input weight */
        }
        else {
            addSizeMaxIO("Weight", &mExpSizeMax, true, "Unit");	/** Computed weight of identical component, if optimized else equals input weight */
        }

        //UseWeightedFlux
        addIO("FluxWeight", &mExpFluxWeight, &mUseWeightedFlux, "Unit");       /** Input expression for flux weighting if mUseWeightedFlux=true */

        addIO("SourceLoadFlow", &mExpFlux, true, mMainCarrier->pFluxUnit()); /** Computed or Controlled Imposed flow injected (source) or extracted (sink) - Positive value means injection for Source field and extraction for Sink field */
        addIO("WeightedImposedFlux", &mExpImposedFlux, true, mMainCarrier->pFluxUnit());
        
        //AddHeatConsumerModel
        addIO("OUTPUTFlux1", &mExpPowerOut, true, mMainCarrier->pFluxUnit()); /** Computed output power output port 1 */
        addIO("INPUTFlux1", &mExpPowerIn, &mAddHeatConsumerModel, mMainCarrier->pFluxUnit()); /** Computed output power output port 1 */
        
        //AddStaticCompensation
        addIO("ReactivePower", &mExpReactivePower, &mAddStaticCompensation, mMainCarrier->pFluxUnit()); /** Reactive power associated to the production of the source load. If  static compensation is not given it is an optimized factor*/
        
        //AddPeakShaving
        addIO("PowerPeakShaving", &mExpPowerPeakShaving, &mAddPeakShavingDetailed, mMainCarrier->pPowerUnit()); /* Peak shaving power */

        //AddSheddingDetailed)
        addIO("PowerShedding", &mExpPowerShedding, &mAddSheddingDetailed, mMainCarrier->pPowerUnit()); /* Shedded power */
        addIO("CostShedding", &mExpCostShedding, &mAddSheddingDetailed, SFunctionUnit({ eFTypeDivision, { mMainCarrier->pPowerUnit(), pCurrency() } })); /* Shedding penalty cost */
        addControlIO("OnShedding", &mExpShedOn, &mAddSheddingDetailed, "bool", &mExpHistOn, &mOnIni); /** Shedding activation, 1 if shedding is activated, 0 otherwise */
        addControlIO("OffShedding", &mExpShedOff, &mAddSheddingDetailed, "bool", &mExpHistOff, &mOffIni); /** Shedding deactivation, 1 if shedding is deactivated, 0 otherwise */
        addControlIO("StateShedding", &mExpShedState, &mAddSheddingDetailed, "bool", &mShedStateIni, &mStateIni); /** Load shedding state, 1 if shedding, 0 otherwise */
   
        /* Register non-IO 0D-expressions in order to automatically allocate and close them */
        addExp(&mExpStaticCompensation);

        /* Register non-IO 1D-expressions in order to automatically allocate and close them */
        addExp(&mExpImposedFlux, &mHorizon);
        addExp(&mExpCost, &mHorizon);
        addExp(&mExpSums, &mHorizonTimeSpanRatio);
    }

    void declareModelIndicators() {
        SourceLoadSubModel::declareDefaultModelIndicators(&mExportIndicators);
    }

    bool isPriceOptimized();
    bool getAddVariableCostModel() { return mAddVariableCostModel; }
    void computeReactivePower();
    void addLoadShedding();
    void addPeakShaving();

    double getTemperature(const std::string& direction);

    void initDefaultPorts() {
        mDefaultPorts.clear();
        //PortSourceLoadFlow - left
        std::map<std::string, std::string> portSourceLoadFlow;
        portSourceLoadFlow["Name"] = "PortL0";
        portSourceLoadFlow["Position"] = "left";
        portSourceLoadFlow["CarrierType"] = ANY_TYPE();
        portSourceLoadFlow["Direction"] = KCONS();
        portSourceLoadFlow["Variable"] = "SourceLoadFlow";
        mDefaultPorts["PortSourceLoadFlow"] = portSourceLoadFlow;
    }

    void setPortPointers() {
        mSourceLoadDefaultPort = getPort("PortSourceLoadFlow");
    }

    //----------------------------------------------------------------------------------------------------
protected:
    //MILP Variable
    MIPModeler::MIPVariable1D mVarPowerPeakShaving;
    MIPModeler::MIPVariable0D mVarOptimalPrice;
    MIPModeler::MIPVariable0D mVarMaxEffect;
    MIPModeler::MIPVariable1D mVarControlledFlux; // if mUseControlledFlux is true
    MIPModeler::MIPVariable1D mVarFluxWeight; // if mUseWeigthedFlux is true
    MIPModeler::MIPVariable1D mVarPowerShedding;
    MIPModeler::MIPVariable1D mShedState;
    MIPModeler::MIPVariable1D mShedOn;
    MIPModeler::MIPVariable1D mShedOff;

    MIPModeler::MIPVariable0D mStaticCompensation;
    MIPModeler::MIPVariable1D mReactivePower;

    // Rolling Horizon
    MIPModeler::MIPData1D mExpHistOn;
    MIPModeler::MIPData1D mExpHistOff;

    //technical output
    MIPModeler::MIPExpression1D mExpFlux;
    MIPModeler::MIPExpression1D mExpCost;
    MIPModeler::MIPExpression1D mExpPowerOut;
    MIPModeler::MIPExpression1D mExpPowerIn;
    MIPModeler::MIPExpression1D mExpFluxWeight;
    MIPModeler::MIPExpression1D mExpImposedFlux;
    MIPModeler::MIPExpression1D mExpPowerPeakShaving; // peak shaving power
    MIPModeler::MIPExpression1D mExpPowerShedding; // shedded power
    MIPModeler::MIPExpression1D mExpCostShedding; // shedding penalty cost 
    MIPModeler::MIPExpression1D mExpShedState;
    MIPModeler::MIPExpression1D mExpShedOn;
    MIPModeler::MIPExpression1D mExpShedOff;
    MIPModeler::MIPExpression1D mExpSums; //intermediate step to store sums of variables
    MIPModeler::MIPExpression1D mExpReactivePower;

    MIPModeler::MIPExpression mExpStaticCompensation;

    int mHorizonTimeSpanRatio; 

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
    bool mAddStaticCompensation = false;

    //economic input
    double mMaxEffectCapex;
    double mMaxEffectOpex;

    //temperature for heat consumer
    double mTemperature_in1;                   /** Inlet Temperature */
    double mTemperature_out1;                  /** Outlet Temperature **/
};

#endif

