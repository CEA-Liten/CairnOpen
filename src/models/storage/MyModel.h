/**
 * \file    MyModel.h
 * \brief   Template for creating a new model
 * \version 1.0
 * \author  Ali KASSEM
 * \date    26/03/2026
 
 * ============================================================
 * HOW TO USE THIS TEMPLATE
 * ============================================================
 * 1. Rename "MyModel" everywhere to your actual model class name.
 * 2. Replace "StorageSubModel" with the actual base class you inherit from
 *    (GridSubModel, SourceLoadSubModel, StorageSubModel, ConverterSubModel, BusSubModel, OperationSubModel).
 * 3. Fill in the sections marked with [TODO].
 * 4. Remove comments that are no longer relevant once your model is finalized.
 * 
 * For reference: 
 * - the technical models: GridSubModel, SourceLoadSubModel, StorageSubModel, and ConverterSubModel 
     inherits from TechnicalSubModel
 * - BusSubModel, OperationSubModel and inherits from SubModel
 * ============================================================
 */
 
 /**
 * \details
 This section is used for the documentation of the model.
 Use **Bold** for bold font.
  
 Description
 -----------
 Briefly describe here what physical component this model represents.
 Example: "This model represents a battery energy storage system that can
 charge and discharge energy subject to capacity and power constraints."
 
 Main Features
 -------------
 - [Feature 1]: e.g., "**MaxPower** limits the charge/discharge rate."
 - [Feature 2]: e.g., "Optional degradation cost model."
 - [Feature 3]: e.g., "Rolling Horizon compatible."
 
 .. figure:: ../images/MyModel.svg
   :alt: IO MyModel
   :name: IOMyModel
   :width: 200
   :align: center
 
 The default charging power is based on the following formula:
 
 .. math::
 
    P=\frac{1}{\eta}.\phi_{discharge} - \eta.\phi_{charge}
 
 where:

- math:`\eta_` is the efficiency,
- math:`\phi_{discharge}` is the discharging flow, 
- math:`\phi_{charge}` is the charging flow

.. caution::

  This model cannot be used with ...
 */

#ifndef MyModel_H
#define MyModel_H

#include "globalModel.h"
#include "StorageSubModel.h"   // [TODO] Replace StorageSubModel with the actual base class header

// [TODO] Replace "StorageSubModel" with your actual base class throughout this file.
// The base class should be from (GridSubModel, SourceLoadSubModel, StorageSubModel, 
//                                ConverterSubModel, BusSubModel, OperationSubModel)

class MODELS_DECLSPEC MyModel : public StorageSubModel
{
public:
    // =========================================================================
    // Constructor / Destructor
    // =========================================================================

    MyModel(CairnObject* aParent);
    ~MyModel();

    // =========================================================================
    // Parameter & Interface Declaration - invoked once at model initialization
    //
    // The six methods below are mandatory for every model. Each must be
    // defined in the header, even if its body is left empty (e.g. a model
    // with no configuration parameters still needs an empty
    // inline void declareModelConfigurationParameters() { }).
    //
    //   1. declareModelConfigurationParameters()  - usually, boolean on/off feature flags
    //   2. declareModelParameters()               - scalar and time series inputs
    //   3. declareModelInterface()                - IO expressions (flux) used on ports
    //   4. declareModelIndicators()               - indicator: post-solve exports
    //   5. initDefaultPorts()                     - default ports of the model 
    //   6. computeModelContribution()             - compute Model particular contribution
    // 
    //   Note, initDefaultPorts() is mandatory unless the parent class already provides 
    //   an implementation of initDefaultPorts(). At present, only StorageSubModel and 
    //   BusSubModel (a Bus declares no default ports) implement this method.
    // 
    // =========================================================================

    /**
    * \brief Declares configuration parameters (usually booleans) that switch model features on or off.
    *        These appear as toggles in the UI and are always read from the
    *        study input file before any other parameter.
    *
    * Always call the parent method first:
    *   MyModelSubModel::declareDefaultModelConfigurationParameters();
    *
    * Then register your own parameters with:
    *   addParameter("MyParam", &mMyParam, defaultValue,
    *                isMandatory, isUsed, "Description", unit, "Category");
    *
    * --- Arguments ---------------------------------------------------------------
    *
    *  "MyParam"    Name used in the study input file and in the UI (string).
    *
    *  &mMyParam    Pointer to the class member that stores the value.
    *               The member must be declared as protected in the header.
    *               Supported types:
    *
    *                 bool, int, double, std::string
    *                     Standard scalar types - use freely with addParameter().
    *
    *                 std::vector<std::string>
    *                     Supported but reserved for specific cases;
    *                     usually not needed for model development.
    *
    *                 std::vector<double>
    *                     Do NOT use with addParameter(). Timeseries (vectors)
    *                     inputs must be declared with addTimeSeries() or addPerfParam() 
    *                     instead. See declareModelParameters() for usage.
    * 
    *  defaultValue      Initial value assigned if the parameter is absent from
    *                    the input file. Must match the member type.
    *
    *  isMandatory       If true, the parameter MUST be explicitly set in the
    *                    input file - even when its value equals the default.
    *                    Recommended to always be true for configuration parameters,
    *                    because other parameters depend on them.
    *                    Type: t_flag - see below.
    *
    *  isUsed            If true, the parameter is actively used when building
    *                    the optimization problem. Usually always true for
    *                    configuration parameters.
    *                    Type: t_flag - see below.
    *
    *  "Description"     Human-readable description shown in the UI (string).
    *
    *  unit              Physical unit of the parameter (string or t_unit -
    *                    see below). Use "" for dimensionless parameters.
    *
    *  "Category"        UI grouping. Built-in categories shared across all
    *                    models: "Base" (default), "EcoInvestModel",
    *                    "EnvironmentModel", "GeometryModel".
    *
    * --- t_flag: conditional boolean expressions ---------------------------------
    *
    *  t_flag controls isMandatory and isUsed. It accepts:
    *
    *  bool            A plain compile-time constant (true / false).
    *  bool*           A pointer to a member bool - evaluated at read time.
    *  SFunctionFlag   A logical expression built from member bools:
    *
    *    SFunctionFlag({ eFTypeNotAnd, {&mA}, {&mB, &mC} })
    *        -> !mA && mB && mC
    *
    *    SFunctionFlag({ eFTypeOrNot, {&mA}, {&mB, &mC} })
    *        -> mA || !mB || !mC
    *
    *  SExtFunctionFlag  Advanced use only. See "UseProfileBuyPrice"
    *                    in GridSubModel.h for a reference example.
    * 
    *  For full details see FlagParam in the documentation.
    * 
    * --- t_unit: dynamic unit strings ------------------------------------------
    *
    *  t_unit controls the unit string displayed in the UI and results file.
    *  It accepts three forms:
    *
    *  string          A plain unit literal, e.g. "MW" or "EUR/MWh".
    *
    *  string*         A pointer to a member string, resolved at read time.
    *                  Useful when the unit depends on a parameter value.
    *
    *  SFunctionUnit   A unit expression built by combining strings or pointers:
    *
    *    SFunctionUnit({ eFTypeDivision, {&mUnit1, &mUnit2} })
    *        -> "mUnit1/mUnit2"
    *
    *    SFunctionUnit({ eFTypeDivision, {&mUnit1}, "suffix" })
    *        -> "mUnit1/suffix"
    *
    *    SFunctionUnit({ eFTypeDivision, {&mUnit1}, "", "prefix" })
    *        -> "prefix/mUnit1"
    *
    *  For full details see UnitParam in the documentation.
    * 
    * --- Built-in unit pointers ------------------------------------------------
    *
    *  The following unit pointers are available in every model and can be
    *  passed directly wherever a t_unit is expected:
    *
    *  pCurrency()           Currency defined in TecEcoAnalysis (e.g. "EUR").
    *
    *  pOptimalSizeUnit()    Unit of the SizeMax variable, as defined by
    *                        addSizeMaxIO() in declareModelInterface().
    *
    * --- Carrier-based unit pointers -------------------------------------------
    *
    *  A carrier (energy vector) exposes a set of unit pointers that are
    *  automatically resolved at runtime. All units can be accessed through
    *  the generic method:
    *
    *        pQuantity("UnitName")
    *
    *  For example:
    *       pQuantity("FluxUnit")       Power unit for electrical or energy carriers (e.g. "MW"), 
    *                                   and mass flow rate unit for mass carriers (e.g. "kg/h")
    *       pQuantity("PowerUnit")      Power unit           (e.g. "MW")
    *       pQuantity("EnergyUnit")     Energy unit          (e.g. "MWh")
    *       pQuantity("StorageUnit")    Storage content unit (e.g. "MWh", "kg")
    *       pQuantity("MassUnit")       Mass unit            (e.g. "kg")
    *       pQuantity("FlowrateUnit")   Mass flow rate unit  (e.g. "kg/h")
    *       pQuantity("PressureUnit")   Pressure unit        (e.g. "bar")
    *
    *  The following units are available for ALL carriers:
    *        - "FluxUnit"
    *        - "PowerUnit"
    *        - "EnergyUnit"
    *        - "StorageUnit"
    *
    *  Material carriers expose additional units:
    *        - "MassUnit"
    *        - "FlowrateUnit"
    *        - "PressureUnit"
    *
    *  These unit pointers can also be accessed through any port that uses
    *  the corresponding carrier. 
    *
    *  There are two ways to reach them:
    *
    *  1. Via a specific port member (MilpPort* declared as protected and
    *     assigned in setPortPointers()):
    *
    *       mPortFlow->pQuantity("FluxUnit") : flux unit of the carrier used at mPortFlow
    *
    *  2. Via mMainCarrier, a built-in member by default pointing to the carrier 
    *     of the first input default port.  
    *
    *       mMainCarrier->pQuantity("FluxUnit") : flux unit of the main carrier
    *     
    *     Note, you can define mMainCarrier inside optional method defineMainCarrier(). 
    *     mMainCarrier is automatically used to drive some units for parameters and IO 
    *     variables (expressions) that are common to all models. 
    *
    * It is recommended to always use the first option (more specific). 
    * 
    * Example - a storage price parameter expressed in currency per flux unit:
    *
    *    SFunctionUnit({ eFTypeDivision, { pCurrency(), mPortFlow->pQuantity("FluxUnit")} })
    * 
    * The unit of the storage price is "Currency/Flux unit on the flow port", e.g., "EUR/MW"
    *     
    * --- Configuration vs. non-configuration parameters -----------------------
    *
    *  The rule is simple:
    *    If ParamA controls the isMandatory or isUsed of ParamB,
    *    then ParamA is a configuration parameter (declared here 
    *    in declareDefaultModelConfigurationParameters()),
    *    and  ParamB is a non-configuration parameter (declared in
    *    declareModelParameters()).
    * 
    *  This distinction matters because configuration parameters are read
    *  from the input file first, so that by the time non-configuration
    *  parameters are read, the framework already knows which of them are
    *  mandatory and which are optional.
    * 
    *  Note, a configuration parameter may also control the isUsed of an 
    *  IO expression defined in declareModelInterface()
    *
    * --- Example ---------------------------------------------------------------
    *
    *  // In declareModelConfigurationParameters():
    *  addParameter("UseStoragePriceTimeSeries", &mUseStoragePriceTS, false,
    *               true, true,
    *               "If true, use StoragePriceTimeSeries; otherwise use StoragePrice",
    *               "bool", "EcoInvestModel");
    *
    *  // In declareModelParameters():
    *  addParameter("StoragePrice", &mStoragePrice, 0.0,
    *               SFunctionFlag({ eFTypeNotAnd, {&mUseStoragePriceTS} }),
    *               SFunctionFlag({ eFTypeNotAnd, {&mUseStoragePriceTS} }),
    *               "Cost of storage flow",
    *               SFunctionUnit({ eFTypeDivision, { pCurrency(), mPortFlow->pQuantity("FluxUnit")} }), 
    *               "EcoInvestModel");
    *
    *  addTimeSeries("StoragePriceTimeSeries", &mStoragePriceTS,  
    *               &mUseStoragePriceTS, &mUseStoragePriceTS,
    *               "Timeseries cost of storage flow",
    *               SFunctionUnit({ eFTypeDivision, { pCurrency(), mPortFlow->pQuantity("FluxUnit")} }),
    *               "EcoInvestModel", 0.0);
    *
    *  StoragePrice is mandatory only when UseStoragePriceTimeSeries is false, 
    *  and StoragePriceTimeSeries is mandatory only when UseStoragePriceTimeSeries is true.
    * 
    * Remark: SFunctionFlag({ eFTypeNotAnd, {&mUseStoragePriceTS} }) -> !mUseStoragePriceTS
    * 
    */
    void declareModelConfigurationParameters() override
    {
        // [TODO] Replace StorageSubModel with your actual base class
        StorageSubModel::declareDefaultModelConfigurationParameters();

        // --- Boolean configuration parameters ---
        // [TODO] Add your configuration parameters here. Example:
        addParameter("UseStoragePriceTimeSeries", &mUseStoragePriceTS, false,
                    true, true,
                    "If true, use StoragePriceTimeSeries; otherwise use StoragePrice",
                    "bool", "EcoInvestModel");

        addParameter("EnableDegradationCost", &mEnableDegradationCost, false,
                    true, true,
                    "If true, a degradation cost is added to the objective",
                    "bool", "CostOptions");

        addParameter("AddSocConstraints", &mAddSocConstraints, false, 
                    false, true, 
                    "Use min and max constraints on the state of charge", 
                    "bool", "EcoInvestModel");
    }

    /**
    * \brief Declares non-configuration parameters, time series, and performance maps.
    *
    * Always call the parent method first:
    *   MyModelSubModel::declareDefaultModelParameters();
    *
    * --- Non-configuration parameters -----------------------------------------
    *
    *  Added using addParameter(), with the same signature as in
    *  declareModelConfigurationParameters(). The typical types used here
    *  are double and int, though bool and std::string are rarely used.
    *
    *  The key difference from configuration parameters is that the
    *  isMandatory and isUsed arguments may depend on configuration parameters
    *  (declared earlier), using bool* or SFunctionFlag expressions.
    *  See declareModelConfigurationParameters() for the full argument reference.
    *
    * --- Time series -----------------------------------------------------------
    *
    *  Time-dependent inputs (std::vector<double>) must be declared with
    *  addTimeSeries() instead of addParameter():
    *
    *   addTimeSeries("MyTimeSeries", &mMyTimeSeries,
    *                 isMandatory, isUsed,
    *                 "Description", unit, "Category",
    *                 defaultValue, minValue, maxValue);
    *
    *  Arguments:
    *
    *  "MyTimeSeries"   Name used in the study input file and in the UI (string).
    *
    *  &mMyTimeSeries   Pointer to the std::vector<double> member that stores
    *                   the values. Must be declared as protected in the header
    *                   and resized to mHorizon in setTimeData().
    *
    *  isMandatory      If true, the time series must be explicitly provided in
    *                   the input file. Type: t_flag (bool, bool*, or
    *                   SFunctionFlag) - same rules as addParameter().
    *
    *  isUsed           If true, the time series is actively read and used when
    *                   building the optimization problem.
    *                   Type: t_flag - same rules as addParameter().
    *
    *  "Description"    Human-readable description shown in the UI (string).
    *
    *  unit             Physical unit of the time series values (string or
    *                   t_unit). See addParameter() for t_unit details.
    *
    *  "Category"       UI grouping - same built-in categories as addParameter():
    *                   "Base" (default), "EcoInvestModel", "EnvironmentModel",
    *                   "GeometryModel".
    *
    *  defaultValue     Scalar double used to initialise every element of the
    *                   vector when no input is provided. Default: 1.0.
    *
    *  minValue         Lower bound applied to each element when reading from
    *                   the input file. Optional - defaults to NaN (no bound).
    *
    *  maxValue         Upper bound applied to each element when reading from
    *                   the input file. Optional - defaults to NaN (no bound).
    *
    * --- Performance maps ------------------------------------------------------
    *
    *  A performance map models a non-linear multi-variable relationship, 
    *  e.g.,2D-relation z = f(x, y), where f cannot be expressed as a linear function.
    *  The framework linearises such behaviours internally using piecewise-linear
    *  approximations. Use this feature only when necessary, as it can
    *  significantly increase the solver resolution time.
    *
    *  Important constraints:
    *    - Size optimisation (SizeMax) and performance maps cannot be active
    *      simultaneously for the same component in a MILP. As a workaround,
    *      run two fixed-size cases (best-case and worst-case performance) to
    *      determine the component dimensions at the extremes, then explore
    *      the range between them with the performance map enabled.
    *    - Performance maps are only supported for models that do not require
    *      integer variables (LP models only).
    *
    *  Map input files:
    *    Maps are provided as CSV files. The file name(s) must be set in the
    *    common parameter "DataFile". Multiple files can be specified using
    *    a semicolon ";" as separator.
    *
    *  Performance parameters are declared with addPerfParam():
    *
    *    addPerfParam("MyPerfParam", &mMyPerfParam,
    *                 isMandatory, isUsed,
    *                 "Description", unit);
    *
    *  Arguments:
    *
    *  "MyPerfParam"   Name used in the CSV performance map file (string).
    *                  Must match the column header in the file exactly.
    *
    *  &mMyPerfParam   Pointer to the std::vector<double> member that stores
    *                  the values read from the map file. Must be declared as
    *                  protected in the header.
    *
    *  isMandatory     If true, this column must be present in the CSV file.
    *                  Type: t_flag (bool, bool*, or SFunctionFlag).
    *                  Default: true.
    *
    *  isUsed          If true, this parameter is actively used when building
    *                  the optimisation problem.
    *                  Type: t_flag. Default: true.
    *
    *  "Description"   Human-readable description of what this vector represents
    *                  in the performance map (string).
    *
    *  unit            Physical unit of the values (string or t_unit).
    *                  Default: "" (dimensionless or unit inherited from context).
    *
    * --- Example ---------------------------------------------------------------
    *
    *  The following example declares a performance map for a battery storage
    *  model, where min and max power depend on the state of energy (SOE) - 
    *  1D piecewise linear interpolation : y = f(x)
    *
    *    // x-axis: SOE set-point values (defines the length of all map vectors)
    *    addPerfParam("SoeSetPoint",    &mSoeSetPoint,    true, true,
    *                 "SOE set-point values along the x-axis of the performance map",
    *                 "%");
    *
    *    // y-axis: minimum power at each SOE set-point
    *    addPerfParam("PminSOESetPoint", &mPminSOESetPoint, true, true,
    *                 "Minimum admissible power at each SOE set-point",
    *                 mMainCarrier->pQuantity("PowerUnit"));
    *
    *    // y-axis: maximum power at each SOE set-point
    *    addPerfParam("PmaxSOESetPoint", &mPmaxSOESetPoint, true, true,
    *                 "Maximum admissible power at each SOE set-point",
    *                 mMainCarrier->pQuantity("PowerUnit"));
    *
    *  All vectors declared with addPerfParam() must have the same length,
    *  which is determined by the number of rows in the CSV file.
    */
    void declareModelParameters() override
    {
        // [TODO] Replace StorageSubModel with your actual base class
        StorageSubModel::declareDefaultModelParameters();

        // --- Parameters (scalars, usually doubles or integers) ---
        // [TODO] Add your non-configuration parameters here. Example:
        addParameter("StoragePrice", &mStoragePrice, 0.0,
                    SFunctionFlag({ eFTypeNotAnd, {&mUseStoragePriceTS} }),
                    SFunctionFlag({ eFTypeNotAnd, {&mUseStoragePriceTS} }),
                    "Cost of storage flow",
                    SFunctionUnit({ eFTypeDivision, { pCurrency(), mPortFlow->pQuantity("FluxUnit")} }),
                    "EcoInvestModel");

        addParameter("InitSOC", &mInitSoc, 0.5, true, true, 
            "Initial state of charge in the range 0-1", "");

        addParameter("Efficiency", &mEfficiency, 0.9, true, true, 
            "Round-trip efficiency", "" /** no unit */, "EcoInvestModel");

        addParameter("MaxEsto", &mMaxEsto, 1e8, true, true, 
            "Maximum energy storage", "MWh" /** static unit */, "EcoInvestModel");

        addParameter("MinSOC", &mMinSoc, 0., &mAddSocConstraints, &mAddSocConstraints,
            "Minimal state of the charge between 0 and 1", "MWh", "EcoInvestModel");

        addParameter("MaxSOC", &mMaxSoc, 1., &mAddSocConstraints, &mAddSocConstraints,
            "Maximal state of the charge between 0 and 1", "MWh", "EcoInvestModel");

        addParameter("MaxFlowCharge", &mMaxFlowCharge, 1.e8, true, true, 
            "Charge maximum flow per storage unit", 
            mPortFlow->pQuantity("FluxUnit"), "EcoInvestModel");

        addParameter("MaxFlowDischarge", &mMaxFlowDischarge, 1.e8, true, true, 
            "Discharge maximum flow per storage unit", 
            mPortFlow->pQuantity("FluxUnit"), "EcoInvestModel");

        // --- Time series (std::vector<double>, resized in setTimeData) ---
        // [TODO] Add your time series here. Example:
        addTimeSeries("StoragePriceTimeSeries", &mStoragePriceTS, 
                    &mUseStoragePriceTS, &mUseStoragePriceTS,
                    "Timeseries cost of storage flow",
                    SFunctionUnit({ eFTypeDivision, { pCurrency(), mPortFlow->pQuantity("FluxUnit")} }),
                    "EcoInvestModel", 0.0);

        addTimeSeries("CapacityMultiplierTimeSeries", &mCapacityMultiplierTS,
                    true, true, 
                    "Time Series of storage CapacityMultiplier acting on Storage unit capacity", 
                    "",     /* no unit */
                    "EcoInvestModel", 
                    1.0,    /* default value */
                    0.0,    /* min value */
                    100.0); /* max value */

        // --- Performance maps (std::vector<double>) ---
        // [TODO] Add your performance vectors here. Example:
        addPerfParam("SoeSetPoint", &mSoeSetPoint, true, true,
                    "SOE set-point values along the x-axis of the performance map",
                    "%");

        addPerfParam("PminSOESetPoint", &mPminSOESetPoint, true, true,
                    "Minimum admissible power at each SOE set-point",
                    mMainCarrier->pQuantity("PowerUnit"));
         
        addPerfParam("PmaxSOESetPoint", &mPmaxSOESetPoint, true, true,
                    "Maximum admissible power at each SOE set-point",
                    mMainCarrier->pQuantity("PowerUnit"));
    }

    /**
    * \brief Declare the model's IO expressions: inputs from and outputs to
    *        the network buses. Also register all internal expressions so the
    *        framework allocates and closes them properly.
    *
    * Always call the parent defaults first:
    *   MyModelSubModel::declareDefaultModelInterface();
    *
    * --- Published IO Expressions ---------------------------------------------------
    *
    *  Published IO expressions represent model quantities that are exported
    *  as results to the network buses and external systems (e.g., Pegase or post‑processing tools).
    *  These expressions are registered using addIO() and are automatically
    *  collected during the export phase.
    *
    *  Two categories of IO expressions are supported:
    *
    *    - Scalar IO: a single value per optimisation run.
    *    - 1D IO: a time‑series of values, always of length mHorizon.
    *
    *  IO expressions do not require external files. They reference existing
    *  model expressions (MIPExpression or MIPExpression1D) that must already
    *  be created and owned by the component.
    *
    *  IO expressions are declared with addIO():
    *
    *    addIO("MyIOName", ExprPtr,  isUsed, unit, "Description");
    *    addIO("MyIOName", ExprPtr1D, isUsed, unit, "Description");
    *
    *  Arguments:
    *
    *    "MyIOName"     Name under which the IO will appear in exported results.
    *                   Must be unique within the component (string).
    *
    *    ExprPtr        Pointer to a scalar MIPExpression representing the value
    *                   to export.
    *
    *    ExprPtr1D      Pointer to a MIPExpression1D representing a time‑series
    *                   of values. Its size must match mHorizon.
    *
    *    isUsed         If true, this IO is exported. Type: t_flag. Default: true.
    *
    *    unit           Physical unit of the exported value(s). Type: t_unit.
    *
    *    "Description"  Human‑readable description of the exported quantity.
    *                   Default: "".
    *
    *
    * --- SizeMax IO Registration ----------------------------------------------------
    *
    *  SizeMax IO is a specialised form of published IO used when a component
    *  exposes its SizeMax variable (e.g., maximum installed capacity).
    *  The expression registered here must be the common SizeMax expression
    *  mExpSizeMax, shared by all models.
    *
    *  Important: mExpSizeMax cannot be registered using addIO(). It must be
    *  registered exclusively through addSizeMaxIO().
    *
    *  SizeMax IO is declared with addSizeMaxIO():
    *
    *    addSizeMaxIO("MySizeMax", ExprPtr, isUsed, unit, "Description");
    *
    *  Arguments:
    *
    *    The arguments are identical to those of addIO(), with the additional
    *    requirement that aExprPtr must be the SizeMax expression mExpSizeMax.
    *
    * --- Controlled IO for Rolling Horizon -------------------------------------------
    *
    *  Controlled IO expressions represent stateful quantities that must persist
    *  across successive optimisation windows in a rolling‑horizon or MPC workflow.
    *
    *  These IOs serve two purposes:
    *    1. They are exported as results.
    *    2. Their final values (or full history) are stored and reused as initial
    *       conditions for the next optimisation horizon.
    *
    *  Controlled IO is declared with addControlIO():
    *
    *    addControlIO("MyCtrlIO", ExprPtr1D, isUsed, unit,
    *                 ValuePtr, DefaultValue, isMPC, "Description");
    *
    *    addControlIO("MyCtrlIO", ExprPtr1D, isUsed, unit,
    *                 HistPtr, DefaultValue, isMPC, "Description");
    *
    *  Arguments:
    *
    *    "MyCtrlIO"     Name of the controlled IO (string). Must match the
    *                   identifier used for rolling‑horizon state transfer.
    *
    *    ExprPtr1D      Pointer to a MIPExpression1D representing the time‑series
    *                   to export (size = mHorizon).
    *
    *    isUsed         If true, the IO is exported and stored. Type: t_flag.
    *
    *    unit           Physical unit of the time‑series values (t_unit).
    *
    *    ValuePtr       Pointer to a scalar double storing the initial value for
    *                   the next horizon. Used when only a single state value
    *                   needs to be propagated.
    *
    *    HistPtr        Pointer to a std::vector<double> storing the full history
    *                   of the IO. Used when the next horizon requires a complete
    *                   past trajectory.
    *
    *    DefaultValue   Optional fallback value if no previous horizon data is
    *                   available. Default: nullptr.
    *
    *    isMPC          If true, the IO participates in MPC state
    *                   propagation. Default: true.
    *
    *    "Description"  Human-readable description of the controlled IO.
    *
    * --- Expression Registration (non-IO) --------------------------------------------
    *
    *  All model expressions that are NOT exported as IO must still be registered
    *  so that the framework can automatically allocate, initialise, and close them.
    *  This ensures proper memory management and consistent handling of all internal
    *  model expressions.
    *
    *  Non‑IO expressions are registered using addExp(). Two categories exist:
    *
    *    - Scalar expressions (MIPExpression)
    *    - 1D expressions (MIPExpression1D), whose size may vary depending on the
    *      model configuration
    *
    *  Any expression that is a member of the model class and is not registered
    *  through addIO(), addSizeMaxIO(), or addControlIO() must be registered 
    *  with addExp():
    *
    *    addExp(ExprPtr);
    *    addExp(ExprPtr1D, size);
    *
    *  Arguments:
    *
    *    ExprPtr        Pointer to a scalar MIPExpression. The expression will be
    *                    allocated and closed automatically by the framework.
    *
    *    ExprPtr1D      Pointer to a MIPExpression1D representing a vector of
    *                    expressions. The vector length is given by size.
    *
    *    size           Pointer to an integer specifying the size of the 1D
    *                   expression. This allows the framework to allocate the
    *                   correct number of elements and to close them properly.
    *
    *  Notes:
    *
    *    - All internal model expressions must be registered exactly once.
    *    - Expressions registered here are NOT exported as results; they are
    *      strictly internal to the optimisation model.
    *    - For 1D expressions, the size pointer must remain valid throughout the
    *      model lifetime. The size is usually mHorizon, but not necessary. 
    *
    */
    void declareModelInterface() override
    {
        // [TODO] Replace StorageSubModel with your actual base class
        StorageSubModel::declareDefaultModelInterface();

        // [TODO] Add your IO expressions here. Example:
         
        // --- SizeMax IO ---
        addSizeMaxIO("MaxEsto", &mExpSizeMax, true,
                     mPortFlow->pQuantity("StorageUnit"),
                     "Computed Storage");

        // --- Always-active IO expressions ---
        addIO("Flow", &mExpFlow, true, 
              mPortFlow->pQuantity("FluxUnit"),
              "Charging/Discharging flow balance");

        // --- Conditionally active IO expressions ---
        addIO("DegradationCost", &mExpDegradationCost, &mEnableDegradationCost, 
              pCurrency(), 
              "Degradation cost");

        // --- Controled IO ---
        addControlIO("SOE", &mExpSOE, true,
                     mPortFlow->pQuantity("EnergyUnit"),
                     &mSoe, // pointer to scalar state
                     &mSoeIntialValue, 
                     true,
                     "State of energy trajectory used for rolling horizon");

        // [TODO] Add your internal (non-IO) expressions here. Example:

        // --- Internal expressions (not visible outside, but must be registered) ---
        addExp(&mExpEnergy, &mHorizon);   // 1D internal expression
        addExp(&mExpInstalledCapacity);           // 0D scalar expression
    }

    /**
    * \brief Declare result indicators that are computed after
    *        optimization and exported to the results file.
    *
    * --- declareModelIndicators() ---------------------------------------------
    *
    *  Always call the parent method first:
    *    MyModelSubModel::declareDefaultModelIndicators(&mExportIndicators);
    *
    *  This registers all standard indicators inherited from the base class.
    *  Additional model-specific indicators are declared with addIndicator():
    *
    *    mInputIndicators->addIndicator("IndicatorName", &mMyIndicator,
    *                                   &mExportIndicators,
    *                                   "Description", unit, "ShortName");
    *
    *  Arguments:
    *
    *  "IndicatorName"   Full name of the indicator as it appears in the
    *                    results file and in the UI (string).
    *
    *  &mMyIndicator     Pointer to the std::vector<double> member that stores
    *                    the indicator values, computed in computeAllIndicators().
    *                    Must be declared as protected in the header.
    *
    *  &mExportIndicators  Pointer to the boolean flag that controls whether
    *                      this indicator is exported to the results file.
    *                      Typically the shared mExportIndicators flag inherited
    *                      from the base class, so all indicators are exported
    *                      or suppressed together.
    *
    *  "Description"     Human-readable description of what the indicator
    *                    represents (string).
    *
    *  unit              Physical unit of the indicator values (string or
    *                    t_unit). Use the built-in unit pointers where possible
    *                    (e.g. pOptimalSizeUnit(), mMainCarrier->pQuantity("EnergyUnit")).
    *
    *  "ShortName"       Compact identifier used internally and in exported
    *                    column headers (string). Should be concise, with no
    *                    spaces, e.g. "CumulatedLosses".
    *
    */
    void declareModelIndicators()
    {
        // Shared indicators for all models of type Storage 
        // [TODO] Replace StorageSubModel with your actual base class
        StorageSubModel::declareDefaultModelIndicators(&mExportIndicators);

        // An additional indicator tracking the cumulated energy losses
        mInputIndicators->addIndicator(
                        "Cumulated losses",   // full name in results
                        &mInternalLosses,     // vector populated in computeAllIndicators()
                        &mExportIndicators,   // export flag shared with all indicators
                        "Cumulated losses over the optimisation horizon",
                        pOptimalSizeUnit(),   // unit follows the optimal size variable
                        "CumulatedLosses");   // short name used in column headers
    }

    // =========================================================================
    // Port topology - defines default bus connections in the graphical editor
    // =========================================================================

    /**
    * \brief Declares the default ports (connection points) of this component
    *        in the system diagram. Each port is described as a key-value map
    *        with the following fields:
    *
    *    "Name"        : Internal port identifier (e.g., "PortL0"). This name 
    *                    must be unique.
    *
    *    "Position"    : Visual placement of the port in the diagram:
    *                    "left", "right", "top", or "bottom".
    *
    *    "CarrierType" : Energy carrier associated with the port. Typical values:
    *                    ANY_TYPE(), "Electrical", "Material", etc.
    *
    *    "Direction"   : Flow direction:
    *                       KCONS()  -> consumption / input
    *                       KPROD()  -> production / output
    *                       KDATA()  -> data‑exchange (no physical flow)
    *
    *    "Variable"    : Name of the IO expression associated with this port.
    *                    This must match an IO registered using addIO() in
    *                    declareModelInterface().
    *
    *  This method is mandatory unless the parent class already provides an
    *  implementation of initDefaultPorts(). At present, only StorageSubModel
    *  and BusSubModel implement this method.
    *
    *  Every non-Bus model must define at least one port. If the parent class
    *  already implements initDefaultPorts(), the child class may omit its own
    *  implementation and simply inherit the parent's default topology. For
    *  example, StorageGen inherits the ports defined in StorageSubModel.
    *
    *  Bus models typically do not define default ports. In particular,
    *  BusSubModel::initDefaultPorts() is intentionally empty and declares no
    *  default ports.
    * 
    */
    void initDefaultPorts() override
    {
        // Always clear the list for safety. This guarantees a clean state even if
        // initDefaultPorts() is invoked more than once during model construction.
        mDefaultPorts.clear();

        // If you want to inherit the parent class ports and then add additional
        // ports, call the parent implementation first:
        //
        //     MyModelSubModel::initDefaultPorts();
        //
        // This preserves the parent topology before extending it with new ports.

        // [TODO] define default ports. Example: a port connected to the main flow
        mDefaultPorts["PortFlow"] = {
            { "Name",        "PortL0" },
            { "Position",    "left" },
            { "CarrierType", ANY_TYPE() }, // Accepts any carrier
            { "Direction",   KCONS() },    // Input port
            { "Variable",    "Flow" }      // Must correspond to an addIO() entry
        };
    }

    /**
    * \brief Binds port objects (retrieved by ID) to member pointer variables
    *        so they can be accessed programmatically after the topology is set.
    *
    * Example:
    *    mPortFlow = getPort("PortFlow");
    *
    * This step is optional: ports can always be accessed directly using
    * getPort("PortID"), but binding them to member pointers improves clarity
    * and avoids repeated lookups.
    */
    void setPortPointers()
    {
        // [TODO] ports binding. Example: 
        mPortFlow = getPort("PortFlow");
    }

    /**
    * \brief Defines the main carrier of the model.
    *
    * This method is optional. If the model does not explicitly assign a main
    * carrier, it will be determined automatically based on the default ports:
    *
    *   - If the model has only one default port, the carrier of that port 
    *     becomes the main carrier.
    * 
    *   - If the model has more than one default port, the carrier of the first 
    *     default input (consumption) port becomes the main carrier.
    *
    *   - If the model doesn't have any input default port, the carrier of the 
    *     first port becomes the main carrier.
    *
    * The main carrier (mMainCarrier) is used implicitly to drive units for
    * parameters and IO expressions that are common to all models, such as
    * "Capex", "Opex", and other generic economic or technical quantities.
    *
    * For Bus models, the main carrier is simply the Bus carrier. All ports of
    * a Bus share the same carrier by construction.
    */
    void defineMainCarrier() {
        // Example implementation:
        mMainCarrier = mPortFlow ? mPortFlow->getCarrier() : nullptr;
    };

    // =========================================================================
    // Time-data sizing - called when the optimization horizon is set/updated
    // =========================================================================

    /**
    * \brief Resize class member vectors to match the simulation horizon.
    *
    * This method adjusts the size of all time‑dependent vectors used by the
    * model. In particular, it must resize:
    *
    *   - all std::vector<double> input time series to size mHorizon
    *   - all history buffers (vectors storing past values) to size
    *         mHorizon + mNpdtPast
    *   - all class‑member vectors used inside computeModelContribution(),
    *     computeEconomicalContribution(), etc., to either mHorizon 
    *     or mHorizon + mNpdtPast depending on their role
    *
    * It is recommended to clear each vector before resizing it.
    *
    * Important:
    *   Do NOT resize indicator vectors here. Indicators declared via
    *   addIndicator(...) must always be initialised in the constructor as:
    *         std::vector<double>(2, 0.0)   // PLAN, HIST
    *
    * Always call the parent implementation first, then resize the vectors
    * specific to your model.
    *
    * Example:
    *   MySubModel::setTimeData();
    *
    *   mStoragePriceTS.clear();
    *   mStoragePriceTS.resize(mHorizon);
    *
    *   mHistFlow.clear();
    *   mHistFlow.resize(mHorizon + mNpdtPast);
    */
    void setTimeData() override;

    // =========================================================================
    // Core lifecycle methods - called automatically by the framework
    // 
    // Methods:
    // - computeModelContribution()
    // - computeGeometricContribution()  [only for technical models]
    // - computeEnvContribution()        [only for technical models]
    // - computeEconomicalContribution() [only for technical models]
    // construct the symbolic expressions and define constraints on them. 
    // The actual  numerical evaluation of those expressions happens later 
    // inside computeAllIndicators(const double* optSol)
    // 
    // The framework executes the following methods in a fixed sequence.
    // Model parameters are set from the input JSON file immediately after
    // computeInitialData() and before finalizeModelData().
    // 
    // =========================================================================
    
    /**
    * \brief Called once after all parameters are loaded but before the MILP
    *        model is built. This step prepares all derived quantities needed
    *        by the model.
    *
    * 1) Compute derived scalars
    *    ------------------------
    *    Use this method to compute any scalar values derived from parameters
    *    (ratios, pre‑scaled bounds, temperature coefficients, etc.). This is
    *    also where sizing bounds mMinValue and mMaxValue must be defined using:
    *
    *        setMinValue(...)
    *        setMaxValue(...)
    *
    *    Defining MinValue and MaxValue is mandatory whenever the SizeMax
    *    expression is introduced via addSizeMaxIO() in declareModelInterface().
    *
    *    These bounds determine the automatic sizing constraints added for all
    *    technical models (GridSubModel, SourceLoadSubModel, StorageSubModel, 
    *    ConverterSubModel):
    *
    *        addConstraint(mExpSizeMax <= aExpInstalled
    *                      * fabs(mMaxValue) * fabs(mWeight), "sBigMInstalled");
    *
    *        addConstraint(mExpSizeMax >= aExpInstalled
    *                      * mMinValue, "sMinSizeInstalled");
    *
    *    All technical models share a default MinSize parameter,
    *    but advanced models may define their own (e.g., mMinEsto) inside
    *    declareModelParameters().
    *
    *    Example:
    *        mHorizonRatio = mHorizon / mTimeSpan + 1;
    *        setMinValue(mMinSize);
    *        setMaxValue(mMaxEsto);
    *
    *
    * 2) Enable State and StartUp/ShutDown constraints
    *    ---------------------------------------------
    *    To activate state‑related constraints, set the shared flags:
    *
    *        mAddStateVariable = true;
    *        mAddStartUpShutDownVariable = true;
    *
    *    or conditionally:
    *
    *        mAddStateVariable = (mParam1 || mParam2);
    *
    *    These flags may also be exposed as configuration parameters in
    *    declareModelConfigurationParameters():
    *
    *        addParameter("AddStateConstrains",
    *                     &mAddStateVariable,
    *                     false, false, true,
    *                     "If true: add State constraints", "bool");
    *
    *        addParameter("AddStartUpShutDownConstrains",
    *                     &mAddStartUpShutDownVariable,
    *                     false, false, true,
    *                     "If true: add StartUp/ShutDown constraints", "bool");
    *
    *
    * 3) Set initial state for ControlledIO
    *    ----------------------------------
    *    Unless provided as a parameter, the initial state (e.g., SOE/SOC)
    *    should be initialized here:
    *
    *        mSOEFinalValue = mInitSoc * getMaxBound();
    *
    *    This ensures consistency between the initial condition and the
    *    model's sizing bounds.
    * 
    *    For reference:
    *        getMinBound() = fabs(mMinValue) * fabs(mWeight)
    *        getMaxBound() = fabs(mMaxValue) * fabs(mWeight)
    */
    void computeInitialData() override;

    /**
    * \brief Called once before the optimization to validate parameter
    *        combinations and detect configuration errors early.
    * \return 0 if consistent, -1 if an error is detected.
    *
    * Always call the parent method first:
    *   MyModelSubModel::checkConsistency();
    * 
    * For technical models (if MyModelSubModel::checkConsistency() is not defined) call:
    *   TechnicalSubModel::checkConsistency();
    * 
    * Use cCritical() to log blocking errors, cWarning() for non-blocking ones.
    * Example checks:
    *   - Incompatible flag combinations (e.g., two mutually exclusive options).
    *   - Parameter values outside a physically meaningful range.
    *   - Missing required inputs.
    */
    int checkConsistency() override;

    /**
    * \brief Main method for building the MILP model: declare optimization
    *        variables, build linear expressions, and add constraints.
    *
    * It is mandatory to provide an implementation of computeModelContribution()
    * 
    * Typical structure:
    *   1. addVariable(...)       - declare decision variables with bounds.
    *   2. Build expressions      - accumulate variables into mExp* objects
    *                               using += inside time-step loops.
    *   3. addConstraint(...)     - add equality / inequality constraints.
    *
    * Guidelines:
    *   - All loops should run over [0, mHorizon).
    *   - Use fabs() on bounds that can be negative (e.g., fabs(mMaxEsto)).
    *   - Close temporary MIPExpression objects with sumExp.close() after use.
    */
    void computeModelContribution() override;

    /**
    * \brief build geometric expressions of the model (area, volume, mass).
    *
    * This method is available only for technical models, i.e. models that
    * inherit from one of the following base classes (which all inherit from
    * TechnicalSubModel):
    *   - GridSubModel
    *   - SourceLoadSubModel
    *   - StorageSubModel
    *   - ConverterSubModel
    *
    * The method is optional. It may be implemented to extend the geometric
    * expressions already defined in the parent class. The base
    * TechnicalSubModel implementation uses the SizeMax expression
    * (mExpSizeMax) to compute the following non‑time‑indexed expressions:
    *
    *   - mExpArea    : geometric area
    *   - mExpVolume  : geometric volume
    *   - mExpMass    : geometric mass
    *
    * When overriding this method, always call the parent implementation first:
    *
    *     TechnicalSubModel::computeGeometricContribution();
    *
    * After that, additional geometric expressions may be appended if the
    * component requires model‑specific geometry (e.g., surface corrections,
    * insulation thickness, structural mass, etc.).
    *
    * All geometric expressions are scalar (0D) and do not depend on time.
    */
    void computeGeometricContribution() override;

    /**
    * \brief build environmental‑impact expression that contributions
    *        to the objective function.
    *
    * This method is available only for technical models, i.e. models that
    * inherit from one of the following base classes:
    *   - GridSubModel
    *   - SourceLoadSubModel
    *   - StorageSubModel
    *   - ConverterSubModel
    *
    * This method is optional. It may be implemented to extend the environmental
    * contributions already defined in the TechnicalSubModel class. The base
    * implementation evaluates the environmental impacts associated with the model.
    *
    * When overriding this method, always call the parent implementation first:
    *
    *     TechnicalSubModel::computeEnvContribution();
    *
    * Additional environmental contributions may be added if needed, although
    * this is rarely required in practice. Any extension should iterate over
    * the list of environmental impacts stored in mEnvImpacts and update
    * the corresponding expressions accordingly.
    *
    */
    void computeEnvContribution() override;

    /**
    * \brief build cost and revenue expressions that contributes to the objective function.
    *
    * This method is available only for technical models, i.e. models that
    * inherit from one of the following base classes:
    *   - GridSubModel
    *   - SourceLoadSubModel
    *   - StorageSubModel
    *   - ConverterSubModel
    *
    * The method is optional. It may be implemented to extend the economic contributions
    * already defined in the TechnicalSubModel class. The base TechnicalSubModel 
    * implementation automatically builds the following expressions:
    *
    *   - mExpCapex           : investment cost
    *   - mExpFixedOpex       : time‑indexed fixed operating cost
    *   - mExpVariableOpex    : time‑indexed variable operating cost
    *   - mExpReplacement     : time‑indexed replacement cost
    *   - mExpVariableCosts   : time‑indexed variable cost [currency/timestep]
    *
    * Note that, total Opex mExpOpex is automatically built at the end, 
    * and should not be manipulated inside the individual models.
    * 
    * When overriding this method, always call the parent implementation first:
    *
    *     TechnicalSubModel::computeEconomicalContribution();
    *
    * After that, additional cost or revenue terms may be appended to the
    * economic expressions. For example, variable costs should be added to
    * mExpVariableCosts. Since variable costs expression is time‑indexed 
    * (of type MIPModeler::MIPExpression1D), multiply by TimeStep(t) when 
    * converting a power‑based cost rate into a currency amount.
    *
    * Example:
    *     mExpVariableCosts[t] += mStoragePriceTS[t] * mExpFlow[t] * TimeStep(t);
    *
    */
    void computeEconomicalContribution() override;

    /**
    * \brief Post-optimization: extract solution values and populate result
    *        indicators. Typically delegates to the parent class helper:
    *
    *   MyModelSubModel::computeDefaultIndicators(optSol);
    * 
    * All the indicators defined using addIndicator() are automatically 
    * computed in MyModelSubModel::computeDefaultIndicators();
    *
    * The optSol pointer gives access to all primal solution values.
    * 
    * Add custom post-processing computations here if needed.
    */
    void computeAllIndicators(const double* optSol) override;

protected:
    // =========================================================================
    // Boolean configuration parameters
    // 
    // Note: class members registered via addParameter() or addTimeSeries()
    // are automatically initialised to their defaultValue by the
    // framework.They do not need to be initialised at the point of
    // declaration in the header, nor in the constructor.
    // =========================================================================

    // [TODO] Declare bool flags that switch other parameters and/or IO variables on/off. 
    // Example:
    bool mUseStoragePriceTS;
    bool mEnableDegradationCost;
    bool mAddSocConstraints;

    // =========================================================================
    // Scalar parameters (read from model data file)
    // =========================================================================
    
    // [TODO] Declare scalar doubles/ints. Example:
    double mStoragePrice;  
    double mInitSoc;
    double mEfficiency;
    double mMaxEsto;    
    double mMinSoc;
    double mMaxSoc;
    double mMaxFlowCharge;   
    double mMaxFlowDischarge;

    // =========================================================================
    // Non-parameters scalar class members 
    // =========================================================================

    // [TODO] Declare non-parameters scalar class members. Example:
    // Must be initialised in the constructor.
    double mSoe;
    double mSoeIntialValue;

    // =========================================================================
    // Time series parameters (read from model data file, resized in setTimeData)
    // =========================================================================
     
    // [TODO] Declare std::vector<double> for time-dependent inputs. Example:
    std::vector<double> mStoragePriceTS;
    std::vector<double> mCapacityMultiplierTS;

    // =========================================================================
    // Non-time series vector class members (resized in setTimeData)
    // =========================================================================

    // [TODO] Declare other std::vector<double>. Example:
    std::vector<double> mHistFlow; /* keep tracking of the flow history */

    // =========================================================================
    // MILP data 
    // =========================================================================
    // 
    // [TODO] Declare MILP data. Example:

    MIPModeler::MIPData1D mSoeSetPoint;
    MIPModeler::MIPData1D mPminSOESetPoint;
    MIPModeler::MIPData1D mPmaxSOESetPoint;

    // =========================================================================
    // MILP variables
    // =========================================================================

    // [TODO] Declare MILP variables. Example:
    MIPModeler::MIPVariable1D mVarEsto;
    MIPModeler::MIPVariable1D mVarFlowCharge;       
    MIPModeler::MIPVariable1D mVarFlowDischarge;  
    MIPModeler::MIPVariable1D mVarOnState;

    // =========================================================================
    // MILP expressions - linear combinations of variables built incrementally
    // =========================================================================

    // [TODO] Declare MILP expressions. Example:
    MIPModeler::MIPExpression mExpInstalledCapacity;
    MIPModeler::MIPExpression mExpDegradationCost;

    MIPModeler::MIPExpression1D mExpEsto;
    MIPModeler::MIPExpression1D mExpFlow;
    MIPModeler::MIPExpression1D mExpFlowCharge;
    MIPModeler::MIPExpression1D mExpFlowDischarge;
    MIPModeler::MIPExpression1D mExpSOE;
    MIPModeler::MIPExpression1D mExpEnergy;

    MIPModeler::MIPExpression1D mExpPminSOE;
    MIPModeler::MIPExpression1D mExpPmaxSOE;

    // =========================================================================
    // Declare indicators as vectors of doubles - to be resized to 
    // length 2 (PLAN and HIST) in the constructor.
    // =========================================================================

    // [TODO] Declare indicator vectors. Example:
    std::vector<double> mInternalLosses;

    // =========================================================================
    // Port pointers (set in setPortPointers)
    // =========================================================================

    // [TODO] Declare port pointers. Example:
    MilpPort* mPortFlow = nullptr;
};

#endif // MyModel_H
