/* --------------------------------------------------------------------------
 * File: AddNode.h
 * version 1.0
 * Author: Alain Ruby
 * Date 23/07/2019
 *---------------------------------------------------------------------------
 * Description: Model imposing Node addition node constraint on Bus Component
 * --------------------------------------------------------------------------
 */

#ifndef ManualObjective_H
#define ManualObjective_H

#include "globalModel.h"

#include "MIPModeler.h"
#include "TechnicalSubModel.h"

/**
* \details
 * Description: ManualObjective is used to parametrize manually the objective function. By default, objective function is written by TecEcoAnalysis component
 * that takes all the contributions of the components and write a net present value function.
 * ManualObjectives allows several options (see Options->ObjectiveType):
 * - add simply add all the connected variables and add it to the objective function
 * - blended operates the same levelization as tecEcoAnalysis to all time-dependant connected variables and add it to the objective function
 * - lexicographic have to be used with several ManualObjective and optimizes iteratively depending on the objective level the different objectives.
 * Common variables creates a variable and writes each of all connected variables is smaller/greater than this variable, that replaces the sum.
*/
class MODELS_DECLSPEC ManualObjective : public TechnicalSubModel
{
public:
//----------------------------------------------------------------------------------------------------
    ManualObjective(CairnObject* aParent);
    ~ManualObjective();
//----------------------------------------------------------------------------------------------------
    void closeExpressions() override;
    void setTimeData();
    //----------------------------------------------------------------------------------------------------
    void computeAllIndicators(const double* optSol);
//----------------------------------------------------------------------------------------------------
    void declareModelConfigurationParameters()
    {
        TechnicalSubModel::declareDefaultModelConfigurationParameters() ;

        //bool
        
        //re-declare and set EcoInvestModel to false
        addParameter("EcoInvestModel", &mEcoInvestModel, false, false, true, "Use EcoInvestModel - ie Use Capex and Opex if true", "", "Base");
       
        /* ------ Hide -----*/
        addParameter("EnvironmentModel", &mEnvironmentModel, false, false, true, "Use EnvironmentModel - ie Use EnvEmissionCost if true", "", "DONOTSHOW"); /** Hide EnvironmentModel */
        addParameter("GeometryModel", &mGeometryModel, false, false, true, "Use GeometryModel", "", "DONOTSHOW");    /** Hide GeometryModel */
        addParameter("PiecewiseArea", &mPiecewiseArea, false, false, true, "Provide a map of areas and component sizes - see performances maps in doc", "", "DONOTSHOW");
        addParameter("PiecewiseVolume", &mPiecewiseVolume, false, false, true, "Provide a map of volumes and sizes - see performances maps in doc", "", "DONOTSHOW");
        addParameter("PiecewiseMass", &mPiecewiseMass, false, false, true, "Provide a map of masses and sizes - see performances maps in doc", "", "DONOTSHOW");
        /* -------------- */

        addParameter("TimeIntegration", &mTimeIntegration, false, false, true);  /** If True then uses time integration for Add and Lexicographic objective types otherwise uses simple summation - default = false */
        addParameter("StrictConstraint",&mStrictConstraint, false, false, true, "Strictconstraint option enabling : at each time sum of connected flows should be equal to StrictConstraintBusValue - default = true","", "Constraints");
        addParameter("MinConstraint", &mMinConstraint, false, false, true, "MinConstraint option enabling : at each time sum of connected flows should be >= MinConstraintBusValue - default = false", "", "Constraints");
        addParameter("MaxConstraint", &mMaxConstraint, false, false, true, "MaxConstraint option enabling : at each time sum of connected flows should be <= MaxConstraintBusValue - default = false ", "", "Constraints");
        addParameter("CommonMinVariable", &mUseCommonMinVariable, false, false, true, "Creates a variable CommonBound which is smaller than all connected variables.", "", "CommonVariables");
        addParameter("CommonMaxVariable", &mUseCommonMaxVariable, false, false, true, "Creates a variable CommonBound which is greater than all connected variables.", "", "CommonVariables");
        //std::string
        addParameter("ObjectiveType", &mObjectiveType, "Add", false, true, "Add or Blended or Lexicographic", "", "Base");
    }

//----------------------------------------------------------------------------------------------------
    void declareModelParameters()
    {
        declareDefaultModelParameters();
        //bool
        addParameter("UseExtrapolationFactor", &mUseExtrapolationFactor, false, SFunctionFlag({ eFTypeOrNot, { &mMinConstraint, &mMaxConstraint, &mStrictConstraint} }), true, "When true the values of *BusValue are assumed over one year instead of optimization horizon");
        //int
        addParameter("ObjectiveLevel", &mObjectiveLevel, 0, false, true, "In case of lexicographic optimization, gives the rank -default 0-", "", "LexicographicObjective");
        //double
        addParameter("StrictConstraintBusValue", &mStrictConstraintBusValue, 0., false, &mStrictConstraint, "if Strictconstraint = true at each time sum of connected flows should be equal to this value - default is 0 - use for flow balance for example", "", "Constraints");
        addParameter("MinConstraintBusValue", &mMinConstraintBusValue, 0., &mMinConstraint, &mMinConstraint, "if MinConstraint = true at each time sum of connected flows should be >= this value", "", "Constraints");
        addParameter("MaxConstraintBusValue", &mMaxConstraintBusValue, INFINITY_VAL, &mMaxConstraint, &mMaxConstraint);/** if MaxConstraint=true at each time sum of connected flows should be <= this value */
        addParameter("ObjectiveCoefficient", &mObjectiveCoefficient, 1., false, true, "In case of blended objective the coefficient of objective in the linear combinaison. The default value is 1");
        addParameter("AbsTol", &mAbsTol, 0., false, true, "Absolute tolerance or degradation of the objective (lexicographic optim)","", "LexicographicObjective");
        addParameter("RelTol", &mRelTol, 0., false, true, "Relative tolerance or degradation of the objective (lexicographic optim)","", "LexicographicObjective");
        //vector
        addTimeSeries("UseProfileObjectiveCoeff", &mObjectiveCoeffTS, false, true, "time series coefficient");
    }
//----------------------------------------------------------------------------------------------------
    void declareModelInterface()
    {
        declareDefaultModelInterface();
        addIO("BusBalance", &mBusBalance, true, mMainCarrier->pFluxUnit()) ; //FluxUnit of First Port
        addIO("BusBalance0D", &mBusBalance0D, true, mMainCarrier->pFluxUnit()) ;
        addIO("BusBalance1D", &mBusBalance1D, true, mMainCarrier->pFluxUnit()) ;
        addIO("SubObjectiveExpression", &mSubObjective, true, "-");
        addIO("MinVar", &mExpCommonMinVariable, true, mMainCarrier->pFluxUnit());
        addIO("MaxVar", &mExpCommonMaxVariable, true, mMainCarrier->pFluxUnit());

        //ObjectiveType == "Blended"
        addIO("Capex", &mExpCapex, SExtFunctionFlag({ &isBlended, this }), mMainCarrier->pFluxUnit()); /** Computed initial investment costs Capex */
        addIO("Opex", &mExpOpex, SExtFunctionFlag({ &isBlended, this }), mMainCarrier->pStorageUnit());      /** Computed operational cost Net Opex */
        addIO("PureOpex", &mExpPureOpex, SExtFunctionFlag({ &isBlended, this }), mMainCarrier->pStorageUnit());      /** Computed operational cost Pure Opex */
        addIO("Replacement", &mExpReplacement, SExtFunctionFlag({ &isBlended, this }), mMainCarrier->pFluxUnit());      /** Computed variable replacement cost */
        
        setSubobjectiveExpression("SubObjectiveExpression");
    }

    void declareModelIndicators() {
        // Supported types are: double
        TechnicalSubModel::declareDefaultModelIndicators();
        mInputIndicators->addIndicator("Integrated bus balance", &mBusEnergyBalance, &mExportIndicators, "Integrated bus balance", mMainCarrier->pStorageUnit(), "BusBalance");
    }
//----------------------------------------------------------------------------------------------------
    void setParameters(double aMinConstraintBusValue, double aMaxConstraintBusValue, double aStrictConstraintBusValue) ;
//----------------------------------------------------------------------------------------------------
    void buildModel();                                                              // build minimum formulation
    void finalizeModelData();
    void computeEconomicalContribution();
    void computeSubObjectiveContribution();
//----------------------------------------------------------------------------------------------------
    MIPModeler::MIPExpression1D busBalance() {return mBusBalance1D;}
    void initBalance() ;
    void addExpressionToBalance(MIPModeler::MIPExpression1D &aFluxExpression) ;
    void addExpressionToBalance(MIPModeler::MIPExpression &aFluxExpression) ;
    void addStrictConstraint() ;
    void addMinConstraint() ;
    void addMaxConstraint() ;
//----------------------------------------------------------------------------------------------------
    virtual void initDefaultPorts() { }; //Bus doesn't have default ports!

protected:

    int mObjectiveLevel;                        /** In case of lexicographic optimization, gives the rank (default 0) */
    double mObjectiveCoefficient;                /** In case of blended objective, the coefficient of objective in the linear combinaison (default 1)*/
    
    bool mTimeIntegration ;                     /** Use timestep weighting of each time variable at ports if true or simply add them if false - Default to false */
    bool mUseExtrapolationFactor;               /** When true the values of *BusValue are assumed over one year instead of optimization horizon*/
    bool mStrictConstraint;                     /** Use strict equality of instantaneous value as aggregation constraint model - Default to false*/
    bool mMinConstraint;                        /** Use Min inequality of instantaneous value as aggregation constraint model - Default to false*/
    bool mMaxConstraint;                        /** Use Max inequality of instantaneous value as aggregation constraint model - Default to false*/

    bool mUseCommonMaxVariable;
    bool mUseCommonMinVariable;

    double mStrictConstraintBusValue ;           /** Instantaneous Equality constraint value, default to 0 to perform bus balance */
    double mMinConstraintBusValue ;              /** Instantaneous Min constraint value, default to 0 to perform bus balance */
    double mMaxConstraintBusValue ;              /** Instantaneous Max constraint value, default to 0 to perform bus balance */

    double mAbsTol;
    double mRelTol;

    MIPModeler::MIPExpression1D mBusBalance1D ;
    MIPModeler::MIPExpression1D mBusBalance0D ;
    MIPModeler::MIPExpression mBusBalance;
    MIPModeler::MIPExpression mSubObjective;

    std::vector<double> mObjectiveCoeffTS;
    MIPModeler::MIPVariable0D mCommonMinVariable;
    MIPModeler::MIPExpression mExpCommonMinVariable;
    MIPModeler::MIPVariable0D mCommonMaxVariable;
    MIPModeler::MIPExpression mExpCommonMaxVariable;

    //Indicateurs
    std::vector<double> mBusEnergyBalance;
};

#endif // ManualObjective_H
