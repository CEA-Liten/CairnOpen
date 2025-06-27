#ifndef StorageGen_H
#define StorageGen_H

#include "globalModel.h"
#include "StorageSubModel.h"
//linkPerseeModelClass StorageGen.h StorageLinearBounds.h StorageThermal.h Battery_V1.h StorageSeasonal.h 

/**
 * \details
This component is a generic model for storage available with any energy vector.

.. figure:: ../images/StorageGen.svg
   :alt: IO StorageGen
   :name: IOStorageGen
   :width: 200
   :align: center

   I/O StorageGen

The stored quantity (ie electrical energy, thermal energy, fluid or material mass) and the corresponding flows (resp. power
or mass flow rates) are set by the energy vector managed by the bus it is connected to.

Hence the units are managed by the energy vector: use the following, instead of the IS Units leading to scaling troubles during solving step

-  Mass Flow : kg/h

-  Power flow : MW

-  Mass : kg

-  Energy : MWh

-  Time : h
*
*/
class MODELS_DECLSPEC StorageGen : public StorageSubModel {

public:
//----------------------------------------------------------------------------------------------------
    StorageGen(QObject* aParent);
    ~StorageGen();
//----------------------------------------------------------------------------------------------------
    int checkConsistency();
    void setTimeData() ;
    void computeInitialData() ;
//----------------------------------------------------------------------------------------------------
    void buildModel();
//----------------------------------------------------------------------------------------------------
    void computeEconomicalContribution();
    void computeAllIndicators(const double* optSol) override;
    void addPressureModel();
//----------------------------------------------------------------------------------------------------
    // Units: use the following, instead of the IS Units leading to "scaling" troubles during solving step
    // Mass Flow    : kg/h
    // Power flow   : MW
    // Mass         : kg
    // Energy       : MWh
    // Time         : Hours
    void declareModelInterface()
    {
        StorageSubModel::declareDefaultModelInterface();
        // register model expression to be used for Interfacing with global MilpProblem, Bus, other MilpComponent
        // Caution : Flux will be signed wrt to Bus balance impact : >0 if energy source, <0 else.
        addIO("Flow", &mExpFlowBank, mEnergyVector->pFluxUnit()) ;		/** Storage balance of flows (including CapcityMuliplier) = Discharge Flow - Charge Flow, ie negative if charging, positive if discharging */
        addIO("DischargeFlow", &mExpFlowDischargeBank, mEnergyVector->pFluxUnit()) ;	/** Storage discharged flow (including CapcityMuliplier) */
        addIO("ChargeFlow", &mExpFlowChargeBank, mEnergyVector->pFluxUnit()) ;			/** Storage charged flow (including CapcityMuliplier) */
        addIO("InternalLosses", &mExpLosses, mEnergyVector->pFluxUnit());	/** Storage discharged flow (including CapcityMuliplier) */
        addIO("EnergyVariation", &mExpEnergyBank, mEnergyVector->pStorageUnit()) ;			/** Computed current Storage content variation (including CapcityMuliplier), in energy (MWh electrical or thermal carriers) or mass (kg fluids) */
        addControlIO("Estock", &mExpEstoBank, mEnergyVector->pStorageUnit(), &mInitialSoe, &mInitialSoe_Def) ;		/** Computed current Storage content (including CapcityMuliplier), in energy (MWh electrical or thermal carriers) or mass (kg fluids) */
        addIO("MaxEsto", &mExpSizeMax, mEnergyVector->pStorageUnit()) ;	    /** Computed Storage Unit maximum content, in energy (MWh electrical or thermal carriers) or mass (kg fluids), ie optimized value if maximum content was given negative value as input data */
        addIO("EstockUnit", &mExpEsto, mEnergyVector->pStorageUnit()) ;		/** Computed current Storage Unit content, in energy (MWh electrical or thermal carriers) or mass (kg fluids) */
        addIO("FlowUnit", &mExpFlow, mEnergyVector->pFluxUnit()) ;		/** Storage Unit balance of flows = Discharge Flow - Charge Flow, ie negative if charging, positive if discharging */
        if (mAddPressureModel) {//Test bouin_7_cont uses a StoregeGen without any Input port !!
            MilpPort* portFluid = getPortByType("Fluid");  //getPortByType("Fluid", KCONS()); 
            if (portFluid != nullptr) { 
                addIO("PressureIn", &mExpPressure, portFluid->pFluxUnit()); /** Inlet Pressure if pressure mode used {"Bar/Pa"}*/
            }
            else {
                Cairn_Exception persee_error("Error: an input Fluid port is expected for StorageGen when AddPressureModel is used (componenet " + mParentCompo->Name() + ")", -1);
                throw persee_error;
            }
        }
        setOptimalSizeExpression("MaxEsto") ;  // defines default expression should be used for OptimalSize computation and use in Economic analysis
        setOptimalSizeUnit(mEnergyVector->pStorageUnit()) ;  // defines unit should be used for OptimalSize computation and use in Economic analysis
    }
//----------------------------------------------------------------------------------------------------

    void declareModelConfigurationParameters()
    {
        StorageSubModel::declareDefaultModelConfigurationParameters() ;
        mInputParam->addToConfigList({ "EcoInvestModel","EnvironmentModel", "AddPressureModel","TimeSeriesForecast","StorageOperationConstraints" });
        //bool
        addParameter("SeasonalCosts",&mSeasonalCosts, false, false, true,"If True: add optional seasonal costs","",{"TimeSeriesForecast"});
        addParameter("FlowDirection",&mFlowDirection, true, false, true, "If True: prevent to charge and discharge at the same time", "bool", {""});
        addParameter("AddPressureModel",&mAddPressureModel, false, false, true,"If True: add optional pressure model of storage - default = false","",{"AddPressureModel"});
        addParameter("AddFinalStorageValue",&mAddFinalStorageValue, false, false, true,"If Tru: define a value of the energy stored at the end of the optimization horizon - time dependent for each absolute time step","EUR/StorageUnit",{""});
        addParameter("ImposeStrictFinalSOC", &mImposeStrictFinalSOC, false, false, true, "If True: use strict equality constraint on final SOC otherwise use minimal bound constraint. Relevant only if FinalSOC > 0.", "", {""});
        addParameter("AddSocConstraints", &mAddSocConstraints, false, false, true, "If True: use min and max constraints on the state of charge in addition to the min and max constraints on the storage capacity", "", {""});
        //double
        //Those parameters need to be inside declareModelConfigurationParameters because they are used in computeInitialData() 
        addParameter("InitSOC", &mInitSOC, 0.5, true, true, "Storage initial state of charge in the range 0-1", "", { "" });
        addParameter("MinEsto", &mMinEsto, 0., true, true, "Storage minimum content in energy or mass", "StorageUnit", { "" });
        addParameter("MaxEsto", &mMaxEsto, -1.e8, true, true, "Storage maximum content in energy for electrical or thermal carriers or mass for fluids and materials - if <0 optimized", "StorageUnit", { "" }); /** Unitary Storage maximum content, in StorageUnit */
    }

    void declareModelParameters()
    {
        StorageSubModel::declareDefaultModelParameters();
        //double
        addParameter("Eta", &mEta, 1., false, true, "Charge and Discharge efficiency : means that Eta*extractedPower will be charged and Eta*injectedPower will be discharged - For mass flowrates :avoid Eta<>1 as it acts as a leak ! ","",{""});
        addParameter("KLoss", &mKlosses, 0., false, true,"Loss coefficient: proportion of Estock lost per hour","*Esto/h",{""}) ;
        addParameter("MaxFlowCharge", &mMaxFlowCharge, 1.e8, true, true, "Charge maximum flow per storage unit -will be multiplied by CapacityMultiplier if any)","FluxUnit",{""});
        addParameter("MinFlowCharge", &mMinFlowCharge, 0., true, true, "Charge maximum flow per storage unit -will be multiplied by CapacityMultiplier if any)","FluxUnit",{""});
        addParameter("MaxFlowDischarge", &mMaxFlowDischarge, 1.e8, true, true, "Discharge maximum flow per storage unit -will be multiplied by CapacityMultiplier if any)","FluxUnit",{""});
        addParameter("MinFlowDischarge", &mMinFlowDischarge, 0., true, true, "Discharge minimum flow per storage unit -will be multiplied by CapacityMultiplier if any)","FluxUnit",{""});
        addParameter("MinSOC", &mMinSoc, 0., &mAddSocConstraints, &mAddSocConstraints, "Minimal state of the charge between 0 and 1 (if AddSOCConstraints is True)", "-", {""}); /** Minimal SOC */
        addParameter("MaxSOC", &mMaxSoc, 1., &mAddSocConstraints, &mAddSocConstraints, "Maximal state of the charge between 0 and 1 (if AddSOCConstraints is True)", "-", {""}); /** Maximal SOC */
        addParameter("FinalSOC", &mFinalSoc, 1., true, true, "Storage final state of charge to reach in fraction of InitSOC is equal to -1 to suppress final constraint.","",{""}) ;
        addParameter("StoragePrice", &mStoragePrice, 0., false, true, "Storage variable cost ie cost linked to storage flow","Currency/FluxUnit",{""}) ;
        addParameter("MaxPressure",&mPressureMax, 350., &mAddPressureModel, &mAddPressureModel,"Maximal pressure in the storage","Bar",{"AddPressureModel"});

        //vector
        addTimeSeries("UseProfileCapacityMultiplier",&mCapacityMultiplier, true, true, "Time Series of storage CapacityMultiplier acting on Storage unit capacity and Maximum Charge and Discharge Flow : if > 1, represents number of identical capacities - if < 1, represents reduced storage capacity", "", { "" }, 1, 0, 1);
        addTimeSeries("UseProfileAllowCharge", &mAllowCharge, &mFlowDirection, &mFlowDirection, "Time Series of storage charging availability : use if 1, forbidden if 0 - Use to simulate unavailability for storage presence, failures or maintenance", "", { "" }, 1, 0, 1);
        addTimeSeries("UseProfileAllowDischarge",&mAllowDischarge, &mFlowDirection, &mFlowDirection, "Time Series of storage discharging availability : use if 1, forbidden if 0 - Use to simulate unavailability for for storage presence, failures or maintenance", "", { "" }, 1, 0, 1);
        addTimeSeries("UseProfileAllowStorage",&mAllowStorage, true, true, "Time Series of storage availability: use if 1, forbidden if 0 - Use to forbid storage during some times - the capacity of the storage is 0", "", { "" }, 1, 0, 1);
        addTimeSeries("UseProfileFinalStorageValue", &mFinalStorageValue, &mAddFinalStorageValue, &mAddFinalStorageValue, "Time Series of values of the energy stored at the end of the optimization horizon - time dependent for each absolute time step", "EUR/StorageUnit");
    }

    void declareModelIndicators() 
    {
        StorageSubModel::declareDefaultModelIndicators(&mExportIndicators);
        QString InstalledSizeUnit = getOptimalSizeUnit();
        mInputIndicators->addIndicator("Cumulated losses", &mInternalLosses, &mExportIndicators, "Cumulated losses", InstalledSizeUnit, "CumulatedLosses");
    }


//----------------------------------------------------------------------------------------------------
    //void setEnvParameters(double aCO2emission, std::vector <double> aCO2PriceProfile) ;
protected:
    // MILP Variables
    MIPModeler::MIPVariable0D mVarStoIni;           /** MILP 0D Variable on SoC at first timestep */
    MIPModeler::MIPVariable1D mVarFlowCharge;       /** MILP 1D Variable on optimal charging flux at each timestep */
    MIPModeler::MIPVariable1D mVarFlowDischarge;    /** MILP 1D Variable on optimal discharging flux at each timestep */
    MIPModeler::MIPVariable1D mVarEsto;             /** MILP 1D Variable on optimal stored energy (fluid mass in kg, electrical or thermal energy in MWh) */
    MIPModeler::MIPVariable1D mVarFlowDirection ;   /** MILP 1D integer variable to avoid charging while discharging ! */
    MIPModeler::MIPVariable1D mVarPressureIn;

    //technical output for one storage unit
    MIPModeler::MIPExpression mExpStoIni;           /** Initial SoC (before the first iteration that takes into account initial fluxes)*/
    MIPModeler::MIPExpression1D mExpEsto;           /** Energy storage, for one storage unit */
    MIPModeler::MIPExpression1D mExpFlowCharge;     /** Charging flow, for one storage unit */
    MIPModeler::MIPExpression1D mExpFlowDischarge;  /** Discharging flow, for one storage unit */
    MIPModeler::MIPExpression1D mExpFlow;           /** Charging/Discharging flow balance, for one storage unit */
    MIPModeler::MIPExpression1D mExpEnergy;         /** Charging/Discharging energy balance, for one storage unit */
    MIPModeler::MIPExpression1D mExpLosses;         /** Internal losses for one storage unit */

    //technical output for a bank of identical storage units, variable in time depending on CapacityMultiplier
    MIPModeler::MIPExpression1D mExpEstoBank;           /** Energy storage, for bank of units */
    MIPModeler::MIPExpression1D mExpFlowChargeBank;     /** Charging flow, for bank of units */
    MIPModeler::MIPExpression1D mExpFlowDischargeBank;  /** Discharging flow, for bank of units */
    MIPModeler::MIPExpression1D mExpFlowBank;           /** Charging/Discharging flow balance, for bank of units */
    MIPModeler::MIPExpression1D mExpEnergyBank;         /** Charging/Discharging energy balance, for bank of units */
    MIPModeler::MIPExpression1D mExpEstoThBank;         /** Energy storage, for bank of units if thermal model */
    MIPModeler::MIPExpression1D mExpFlowThBank;         /** Charging/Discharging flow balance, for bank of units if thermal model */
    MIPModeler::MIPExpression1D mExpFlowDirection;      /** To avoid charging while discharging */
    MIPModeler::MIPExpression1D mExpPressure;           /** Pressure in the tanks (Bar ?) */

    //technical input for one storage unit
    double mEta;                    /**  dissymetry Charging  / Discharging */
    double mKlosses;                /**  Storage losses */
    double mMaxFlowCharge;          /**  maximum flux in charge (flowrate or power) */
    double mMinFlowCharge;          /**  minimum flux in charge (flowrate or power) */
    double mMaxFlowDischarge;       /**  maximum flux in discharge (flowrate or power) */
    double mMinFlowDischarge;       /**  minimum flux in discharge (flowrate or power) */
    double mMinEsto;                /**  minimum storage capacity (kg mass on fluid vectors, MWh energy for electrical and thermal vectors) */
    double mMaxEsto;                /**  maximum storage capacity (kg mass on fluid vectors, MWh energy for electrical and thermal vectors) */
    double mInitialSoe;
    double mInitialSoe_Def;
    double mInitSOC ;               /**  initial storage State Of Charge */
    double mFinalSoc ;              /**  final State Of Charge constraint if >0, no constraint else*/
    double mPressureMax;            /** Pressure max in the storage */
    double mMinSoc;                 /** Minimal value for state of charge */
    double mMaxSoc;                 /** Maximal value for state of charge */

    //economic input
    double mStoragePrice ;                      /** Total cost of using the storage - in function of the stored mass flow rate - Use with option final SoC = init SoC for consistency **/

    //timeseries
    std::vector<double> mCapacityMultiplier;  /** time series for storage capacity multiplier */
    std::vector<double> mAllowCharge;         /** time series for storage charging availability */
    std::vector<double> mAllowDischarge;      /** time series for storage dischaging availability */
    std::vector<double> mAllowStorage;        /** time series for storage availability */
    std::vector<double> mFinalStorageValue;   /** time series for final storage values */

    //options
    bool mSeasonalCosts;
    bool mFlowDirection;
    bool mAddPressureModel;             /** Add pressure model - linearity between Pressure and Maxstorage*/
    bool mAddFinalStorageValue;
    bool mImposeStrictFinalSOC;         /** Impose equality constraint on final state of charge if true, or use default minimal bound constraint if false */
    bool mAddSocConstraints;            /** If True, then use min and max constraints on the state of charge in addition to the min and max constraints on the storage capacity */

    //Indicators
    std::vector<double> mInternalLosses;
};
#endif
