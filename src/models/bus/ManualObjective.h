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
#include "BusSubModel.h"

/**
* \details
 * Description: ManualObjective is used to parametrize manually the objective function. By default, objective function is written by TecEcoAnalysis component
 * that takes all the contributions of the components and write a net present value function.
 * ManualObjectives allows several options (see Options->ObjectiveType):
 * - add simply add all the connected variables and add it to the objective function
 * - lexicographic have to be used with several ManualObjective and optimizes iteratively depending on the objective level the different objectives.
 * Common variables creates a variable and writes each of all connected variables is smaller/greater than this variable, that replaces the sum.
*/
class MODELS_DECLSPEC ManualObjective : public BusSubModel
{
public:
//----------------------------------------------------------------------------------------------------
    ManualObjective(CairnObject* aParent);
    ~ManualObjective();
//----------------------------------------------------------------------------------------------------
    void setTimeData() override;
    void computeInitialData() override;

    //----------------------------------------------------------------------------------------------------
    void computeAllIndicators(const double* optSol);
//----------------------------------------------------------------------------------------------------
    void declareModelConfigurationParameters()
    {
        BusSubModel::declareDefaultModelConfigurationParameters() ;

        
        addParameter("StrictConstraint",&mStrictConstraint, false, false, true, "Strictconstraint option enabling : at each time sum of connected flows should be equal to StrictConstraintBusValue - default = true","", "Constraints");
        addParameter("MinConstraint", &mMinConstraint, false, false, true, "MinConstraint option enabling : at each time sum of connected flows should be >= MinConstraintBusValue - default = false", "", "Constraints");
        addParameter("MaxConstraint", &mMaxConstraint, false, false, true, "MaxConstraint option enabling : at each time sum of connected flows should be <= MaxConstraintBusValue - default = false ", "", "Constraints");
        addParameter("CommonMinVariable", &mUseCommonMinVariable, false, false, true, "Creates a variable CommonBound which is smaller than all connected variables.", "", "CommonVariables");
        addParameter("CommonMaxVariable", &mUseCommonMaxVariable, false, false, true, "Creates a variable CommonBound which is greater than all connected variables.", "", "CommonVariables");
        //std::string
        addParameter("ObjectiveType", &mObjectiveType, "Add", false, true, "Add or Lexicographic", "", "Base");
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
        addParameter("ObjectiveCoefficient", &mObjectiveCoefficient, 1., false, true, "Coefficient of objective. The default value is 1");
        addParameter("AbsTol", &mAbsTol, 0., false, true, "Absolute tolerance or degradation of the objective (lexicographic optim)","", "LexicographicObjective");
        addParameter("RelTol", &mRelTol, 0., false, true, "Relative tolerance or degradation of the objective (lexicographic optim)","", "LexicographicObjective");
        //vector
        addTimeSeries("UseProfileObjectiveCoeff", &mObjectiveCoeffTS, false, true, "time series coefficient");
        addParameter("TimeIntegration", &mTimeIntegration, false, false, &mUseCommonMaxVariable||&mUseCommonMinVariable, "If True then the common max or min variable contraint is written with the integral of the variables ", "Base");
    }
//----------------------------------------------------------------------------------------------------
    void declareModelInterface()
    {
        BusSubModel::declareDefaultModelInterface();

        addIO("BusBalance", &mBusBalance, true, mMainCarrier->pFluxUnit()) ; //FluxUnit of First Port
        addIO("BusBalance0D", &mBusBalance0D, true, mMainCarrier->pFluxUnit()) ;
        addIO("BusBalance1D", &mBusBalance1D, true, mMainCarrier->pFluxUnit()) ;
        addIO("SubObjectiveExpression", &mSubObjective, true, "-");
        addIO("MinVar", &mExpCommonMinVariable, true, mMainCarrier->pFluxUnit());
        addIO("MaxVar", &mExpCommonMaxVariable, true, mMainCarrier->pFluxUnit());

        setSubobjectiveExpression("SubObjectiveExpression");
    }

    void declareModelIndicators() {
        BusSubModel::declareDefaultModelIndicators();
        mInputIndicators->addIndicator("Integrated bus balance", &mBusEnergyBalance, &mExportIndicators, "Integrated bus balance", mMainCarrier->pStorageUnit(), "BusBalance");
    }
//----------------------------------------------------------------------------------------------------
    void setParameters(double aMinConstraintBusValue, double aMaxConstraintBusValue, double aStrictConstraintBusValue) ;
//----------------------------------------------------------------------------------------------------
    void computeModelContribution() override; 
    void computeSubObjectiveContribution();
//----------------------------------------------------------------------------------------------------
    MIPModeler::MIPExpression1D busBalance() {return mBusBalance1D;}
    void initBalance() ;
    void addExpressionToBalance(MIPModeler::MIPExpression1D &aFluxExpression) ;
    void addExpressionToBalance(MIPModeler::MIPExpression &aFluxExpression) ;
    void addStrictConstraint() ;
    void addMinConstraint() ;
    void addMaxConstraint() ;
    void addLexicographicObjective();
//----------------------------------------------------------------------------------------------------
    virtual void initDefaultPorts() { }; //Bus doesn't have default ports!

protected:

    int mObjectiveLevel;                        /** In case of lexicographic optimization, gives the rank (default 0) */
    double mObjectiveCoefficient;            
    
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

    std::vector<double> mObjectiveCoeffTS;

    MIPModeler::MIPVariable0D mCommonMinVariable;
    MIPModeler::MIPVariable0D mCommonMaxVariable;

    MIPModeler::MIPExpression mBusBalance;
    MIPModeler::MIPExpression mSubObjective;
    MIPModeler::MIPExpression mExpCommonMinVariable;
    MIPModeler::MIPExpression mExpCommonMaxVariable;

    MIPModeler::MIPExpression1D mBusBalance1D;
    MIPModeler::MIPExpression1D mBusBalance0D;

    //Indicateurs
    std::vector<double> mBusEnergyBalance;
};

#endif // ManualObjective_H
