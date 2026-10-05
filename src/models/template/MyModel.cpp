/**
 * \file    MyModel.h
 * \brief   Template for creating a new model
 * \version 1.0
 * \author  Ali KASSEM
 * \date    26/03/2026
 */

#include "MyModel.h"

// =============================================================================
// Factory function ; required by the plugin loader
// =============================================================================
extern "C" MODELS_DECLSPEC CairnObject* createModel(CairnObject* aParent)
{
    return new MyModel(aParent);
}

// =============================================================================
// Constructor
// =============================================================================
/**
* \brief Initializes member variables to safe default values.
*
* Rules:
*   - Always call the parent constructor first, passing aParent
*   - Initialize all non-parameter scalar members declared in the header
*   - Initialize indicator vectors declared in the header to (2, 0.) 
*   - Time-series and other vectors (std::vector<double>) are resized later in setTimeData()
*   - mPossibleModelClasses lists which class names this plugin can instantiate
*     Add derived class names here if you plan to subclass MyModel. Example: 
*     mPossibleModelClasses = { "Electrolyzer", "ElectrolyzerDetailed" };
*/
MyModel::MyModel(CairnObject* aParent) :
    StorageSubModel(aParent),
    // [TODO] Initialize your scalar non-parameter members here. Example:
    mSoeIntialValue(0.0),
    mSoe(0.0),
    // [TODO] Initialize all indicator-related vectors to size 2, value 0
    mInternalLosses(2, 0.)
{
    // [TODO] List all class names this factory can produce.
    mPossibleModelClasses = { "MyModel" /*, "MyModelVariant" */ };
    
    /* Contractor:
     All the variable members declared inside model.h and that are not parameters,
     timeseries or expressions should be initialized here.

     Indicator vectors are always initialized to a vector of size 2 :
     mMyIndicator[0] is used to store PLAN value, while mMyIndicator[1] is used to store HIST value.

     Time series mStoragePriceTS should be resized to e.g. mHorizon but it cannot be done here because mHorizon
     is not defined yet at this point. The correct place to resize mStoragePriceTS is inside setTimeData()
    */

    /* Enable Stateand StartUp / ShutDown constraints

    To always activate state‑related constraints, set the shared flags :

    mAddStateVariable = true;
    mAddStartUpShutDownVariable = true;

    Note that, these flags may also be exposed as configuration parameters in
    declareModelConfigurationParameters() :

    addParameter("AddStateConstrains",
            &mAddStateVariable,
            false, false, true,
            "If true: add State constraints", "bool");

    addParameter("AddStartUpShutDownConstrains",
            &mAddStartUpShutDownVariable,
            false, false, true,
            "If true: add StartUp/ShutDown constraints", "bool");
    */
}

// =============================================================================
// Destructor
// =============================================================================
MyModel::~MyModel()
{
    // [TOOD] Delete port objects owned by this class
    delete mPortFlow;

    // [TOOD] Delete other pointers member of this class, if any

    // All other resources cleaning (expressions, variables, constraints, 
    // carriers, etc.) is managed automatically by the framework.
}


// =============================================================================
// setTimeData
// =============================================================================
/**
* \brief Resize class member vectors to match the simulation horizon.
*
* This method adjusts the size of all time-dependent vectors used by the
* model. In particular, it must resize:
*
*   - all std::vector<double> input time series to size mHorizon
*   - all history buffers (vectors storing past values) to size
*         mHorizon + mNpdtPast
*   - all class-member vectors used inside computeModelContribution(),
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
*/
void MyModel::setTimeData()
{
    StorageSubModel::setTimeData();   // [TODO] Replace StorageSubModel with your actual base class

    // [TODO] Resize every std::vector<double> time-series member. Example:
    mStoragePriceTS.clear();
    mStoragePriceTS.resize(mHorizon);
    
    mCapacityMultiplierTS.clear();
    mCapacityMultiplierTS.resize(mHorizon);

    // [TODO] Resize every std::vector<double> member used to keep history. Example:
    mHistFlow.clear();
    mHistFlow.resize(mHorizon + mNpdtPast);
}

// =============================================================================
// computeInitialData
// =============================================================================
/**
* \brief Called once after all parameters are loaded but before the MILP
*        model is built. This step prepares all derived quantities needed
*        by the model.
*
* 1) Compute derived scalars
*    ------------------------
*    Use this method to compute any scalar values derived from parameters
*    (ratios, pre-scaled bounds, temperature coefficients, etc.). This is
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
*
* 2) Enable State and StartUp/ShutDown constraints
*    ---------------------------------------------
*    To activate state-related constraints, set the shared flags:
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
* 3) Set initial states for ControlledIO's
*    ----------------------------------
*    Unless provided as a parameter, the initial state (e.g., SOE/SOC)
*    should be initialized here:
*
*        mSoeIntialValue = mInitSoc * getMaxBound();
*
*    This ensures consistency between the initial condition and the
*    model's sizing bounds.
*
*    For reference:
*        getMinBound() = fabs(mMinValue) * fabs(mWeight)
*        getMaxBound() = fabs(mMaxValue) * fabs(mWeight)
*/
void MyModel::computeInitialData() 
{
    // [TODO] Set MinValue and MaxValue. Example: 
    setMinValue(mMinSize);
    setMaxValue(mMaxEsto);

    // [TODO] Add State, StartUp and ShutDown constraints, if needed
    // mAddStateVariable = true;
    // mAddStartUpShutDownVariable = true;

    // [TODO] Set initial state for ControlledIO's. Example:
    mSoeIntialValue = mInitSoc * getMaxBound(); /* intial satet of "SOE" */
}

// =============================================================================
// checkConsistency
// =============================================================================
/**
* \brief Validates parameter combinations before building the MILP model.
*
* Return 0 for success, -1 to abort with an error.
* Use cCritical() to log blocking errors (causes abort).
* Use cWarning()  to log non-blocking warnings.
*
* Always call the parent method first:
*   MyModelSubModel::checkConsistency();
*
* For technical models (if MyModelSubModel::checkConsistency() is not defined) call:
*   TechnicalSubModel::checkConsistency();
* 
* Typical checks:
*   - Mutually exclusive flags (e.g., two options that cannot both be true).
*   - Parameter values outside physically meaningful ranges.
*   - Required dependencies (e.g., flag B requires flag A to be enabled).
*   - Rolling Horizon past-horizon size vs. activation/deactivation times.
*/
int MyModel::checkConsistency()
{
    // [TODO] call the parent method first. Replace StorageSubModel with your actual base class
    StorageSubModel::checkConsistency();
    // or TechnicalSubModel::checkConsistency();

    // [TODO] Example checks:

    // --- Check: efficiency must be in (0, 1] ---
     if (mEfficiency <= 0. || mEfficiency > 1.) {
         cCritical() << "MyModel: Efficiency must be in (0, 1], got " << mEfficiency;
         return -1;
     }

    // --- Check: mutually exclusive options ---
    // if (mOptionA && mOptionB) {
    //     cCritical() << "MyModel: OptionA and OptionB cannot both be true.";
    //     return -1;
    // }

    // --- Warning: sub-optimal but allowed configuration ---
    if (mMaxEsto == 0.) {
        cWarning() << "MyModel: MaxEsto is 0 — component will have no effect.";
    }

    return 0;
}

// =============================================================================
// computeModelContribution [Mandatory]
// =============================================================================
/**
* \brief Builds the MILP model: variables, expressions, and constraints.
*
* This is the core method of your model. Follow these three phases:
*
* PHASE 1 — Declare decision variables
* -------------------------------------
* addVariable(varObject, "Name", lowerBound, upperBound [, type])
*
*   - varObject : a MIPVariable0D or MIPVariable1D member declared in the header.
*
*   - Name      : string label used in the exported solution file.
*
*   - Bounds    : physical or financial limits of the variable. When bounds are
*                 derived from parameters that may be signed, use fabs() to
*                 ensure correct magnitude. For reference:
*
*                     getMaxBound() = fabs(mMaxValue) * fabs(mWeight)
*                     getMinBound() = fabs(mMinValue) * fabs(mWeight)
*
*   - type      : optional variable type. Omit for continuous variables
*                 (MIP_FLOAT is the default). Use MIPModeler::MIP_INT for
*                 binary or integer variables.
*
* Example:
*   addVariable(mVarFlowCharge, "ChargeFlow", 0., fabs(mMaxFlowCharge));
*   addVariable(mVarOnState,    "OnState",     0,   1, MIPModeler::MIP_INT);
*
* PHASE 2 — Build expressions
* ----------------------------
* Accumulate variable contributions into mExp* objects inside time loops.
* Always use "+=" (never "=") so that contributions from multiple models
* on the same bus are correctly summed.
*
* For reference: mExpSizeMax is the optimal size expression  - 
*                shared to all models
* 
* Example:
*   for (uint64_t t = 0; t < mHorizon; ++t) {
*       mExpFlux[t] += mVarChargePower(t);
*   }
*
* PHASE 3 — Add constraints
* --------------------------
* addConstraint(expression, "ConstraintName" [, timeIndex])
*
*   Constraints are created by applying standard comparison operators
*   (==, <=, >=) to MIP expressions. Each call registers the resulting
*   MIPConstraint object under the given name.
*
*   For time-indexed constraints, the optional third argument t is used
*   purely for labelling and traceability in the exported results.
*
* Example:
*   addConstraint(mExpFlux[t]   <= fabs(mMaxPower),              "MaxPower",      t);
*   addConstraint(mExpEnergy[t] == mExpEnergy[t-1] + mExpFlux[t], "EnergyBalance", t);
*/
void MyModel::computeModelContribution()
{
    // -------------------------------------------------------------------------
    // PHASE 1: Declare decision variables
    // -------------------------------------------------------------------------

    // [TODO] Add your variables. Example:

    addVariable(mVarEsto, "Esto", 0., getMaxBound()); 
    addVariable(mVarFlowCharge, "ChargeFlow", 0., fabs(mMaxFlowCharge));
    addVariable(mVarFlowDischarge, "DischargeFlow", 0., fabs(mMaxFlowDischarge));
    addVariable(mVarOnState, "OnState", 0, 1, MIPModeler::MIP_INT);

    // -------------------------------------------------------------------------
    // PHASE 2: Build expressions — time loop
    // -------------------------------------------------------------------------
    for (uint64_t t = 0; t < mHorizon; ++t)
    {
        // [TODO] Accumulate variable contributions into your expressions. Example:
        mExpFlow[t] += mExpFlowDischarge[t] - mExpFlowCharge[t];
        // mExpDegradationCost += ...
        // etc.  
    }

    // An expression can be filled from a variable. Example:
    fillExpression(mExpEsto, mVarEsto);
    fillExpression(mExpFlowCharge, mVarFlowCharge);
    fillExpression(mExpFlowDischarge, mVarFlowDischarge);

    // Example of using performance maps
    //
    // The following expressions apply a piecewise-linear interpolation to map
    // the SOE (state of energy) to its corresponding minimum and maximum power.
    //
    // The arguments are:
    //   (*mModel)        : shared model instance available to all components
    //   mExpSOE          : x-axis expression (SOE)
    //   mSoeSetPoint     : vector of x-coordinates (breakpoints)
    //   mPminSOESetPoint : vector of y-values for the lower envelope
    //   "PminSOE"        : name of the generated internal expression
    //   MIP_SOS          : linearisation type
    //
    //   The linearisation type can be:
    //      - MIP_SOS: the piecewise-linearisation is implemented using a Special
    //                 Ordered Set (SOS2) formulation.
    // 
    //      - MIP_LOG: alternative logarithmic formulation (not used here).
    //
    // For more information about linearisation methods see MIPModeler::MIPUtils.h

    mExpPminSOE = MIPModeler::MIPPiecewiseLinearisation(*mModel, mExpSOE, mSoeSetPoint, 
        mPminSOESetPoint, "PminSOE", MIPModeler::MIP_SOS);

    mExpPmaxSOE = MIPModeler::MIPPiecewiseLinearisation(*mModel, mExpSOE, mSoeSetPoint, 
        mPmaxSOESetPoint, "PmaxSOE", MIPModeler::MIP_SOS);

    // -------------------------------------------------------------------------
    // PHASE 3: Add constraints — time loop
    // -------------------------------------------------------------------------
    for (uint64_t t = 0; t < mHorizon; ++t)
    {
        // [TODO] Add per-timestep constraints. Example:
        addConstraint(mExpEsto[t] - mExpSizeMax <= 0, "MEsto", t);

        if (mAddSocConstraints) {
            addConstraint(mExpEsto[t] >= mMinSoc * mExpSizeMax, "EnergyMinSOC", t);
            addConstraint(mExpEsto[t] <= mMaxSoc * mExpSizeMax, "EnergyMaxSOC", t);
        }

        addConstraint(mExpEnergy[t] == mExpEnergy[t - 1] + mExpFlow[t], "EnergyBalance", t);
    }
}

// =============================================================================
// computeGeometricContribution [Optional] - needed only in particular cases
// =============================================================================
/**
* \brief build geometric expressions of the model (area, volume, mass).
*
* This method is available only for technical models (Grid, SourceLoad, Storage, Converter)
*
* The method is optional. It may be implemented to extend the geometric
* expressions already defined by the parent class.
*
* When overriding this method, always call the parent implementation first:
*
*     TechnicalSubModel::computeGeometricContribution();
*
* After that, additional geometric expressions may be appended, if the
* component requires model-specific geometry, to:
*   - mExpArea    : geometric area
*   - mExpVolume  : geometric volume
*   - mExpMass    : geometric mass
*
* All geometric expressions are scalar (0D) and do not depend on time.
*/
void MyModel::computeGeometricContribution()
{
    TechnicalSubModel::computeGeometricContribution();

    // [TODO] add your additional geometric contributions, if any
    // ...
}

// =============================================================================
// computeEnvContribution [Optional] - rarely needed in practice
// =============================================================================
/**
* \brief build environmental-impact expression that contributions
*        to the objective function.
*
* This method is available only for technical models (Grid, SourceLoad, Storage, Converter)
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
*/
void MyModel::computeEnvContribution()
{
    TechnicalSubModel::computeEnvContribution();

    // [TODO] add your additional environmental contributions, if any
    // ...
}

// =============================================================================
// computeEconomicalContribution [Optional]
// =============================================================================
/**
* \brief build cost and revenue expressions that contributes to the objective function.
*
* This method is available only for technical models (Grid, SourceLoad, Storage, Converter)
*
* The method is optional. It may be implemented to extend the economic
* contributions already defined in the TechnicalSubModel class.  
*
* When overriding this method, always call the parent implementation first:
*
*     TechnicalSubModel::computeEconomicalContribution();
*
* After that, additional cost or revenue terms may be appended to the
* economic expressions: 
*   - mExpCapex           : investment cost
*   - mExpFixedOpex       : time-indexed fixed operating cost
*   - mExpVariableOpex    : time-indexed variable operating cost
*   - mExpReplacement     : time-indexed replacement cost
*   - mExpVariableCosts   : time-indexed variable cost [currency/timestep]
* 
* Note that, total Opex mExpOpex is automatically built at the end, 
* and should not be manipulated inside the individual models.
*/
void MyModel::computeEconomicalContribution()
{
    // Always call the parent implementation first
    TechnicalSubModel::computeEconomicalContribution();  

    // [TODO] add your additional cost or revenue contributions. Example:
    
    // -------------------------------------------------------------------------
    // Example: CAPEX contribution from an optimised size variable
    // -------------------------------------------------------------------------
    // mExpCapex += mCapexPerUnit * mVarInstalledSize;

    for (uint64_t t = 0; t < mHorizon; ++t) {
        // -------------------------------------------------------------------------
        // Example: variable energy cost from a time-series price
        // -------------------------------------------------------------------------
        mExpVariableCosts[t] += mStoragePriceTS[t] * mExpFlow[t] * TimeStep(t);
    }
}

// =============================================================================
// computeAllIndicators
// =============================================================================
/**
* \brief Extracts solution values after the MILP solve and populates KPI
*        indicators for export to the results file.
*
* 
* Delegates to the SubModel default helper, which computes standard
* indicators (energy totals, capacity, costs, etc.).
*
* Add custom indicator computations below the delegate call if needed.
*/

/**
* \brief Post-optimization: extract solution values and populate result
*        indicators. Typically delegates to the parent class helper:
*
*   MyModelSubModel::computeAllIndicators(optSol);
*
* All the indicators defined using addIndicator() are automatically
* computed in MyModelSubModel::computeDefaultIndicators();
*
* The optSol pointer gives access to all primal solution values.
* 
* Add custom post-processing computations here if needed.
*/
void MyModel::computeAllIndicators(const double* optSol)
{
    // [TODO]: Replace StorageSubModel with your actual base class
    StorageSubModel::computeAllIndicators(optSol);

    // [TODO] Add custom post-processing computation here. Example:
    //computeProduction(true, mHorizon, mNpdtPast, mExpLosses, optSol, 1., 0., mInternalLosses.at(0));
    //computeProduction(false, *mptrTimeshift, mNpdtPast, mExpLosses, optSol, 1., 0., mInternalLosses.at(1));

    /*
     * Helper method computeProduction() uses the optimal solution (optSol) to
     * evaluate the expression mExpLosses and update the internal indicator value:
     *
     *     mInternalLosses.at(0)  : PLAN value
     *     mInternalLosses.at(1) : HIST value
     *
     * For additional details on computeProduction() and related helper
     * functions, refer to SubModel.cpp.
     */
}
