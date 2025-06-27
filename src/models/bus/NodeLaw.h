/* --------------------------------------------------------------------------
 * File: AddNode.h
 * version 1.0
 * Author: Alain Ruby
 * Date 23/07/2019
 *---------------------------------------------------------------------------
 * Description: Model imposing Node addition node constraint on Bus Component
 * --------------------------------------------------------------------------
 */

#ifndef NODELAW_H
#define NODELAW_H

#include "globalModel.h"

#include "MIPModeler.h"
#include "BusSubModel.h"

/**
* \details
 * Description: NodeLaw must be used to impose flow balance constraint computation on a Bus component connecting several models together.
*/
class MODELS_DECLSPEC NodeLaw : public BusSubModel
{
public:
//----------------------------------------------------------------------------------------------------
    NodeLaw(QObject* aParent);
    ~NodeLaw();
//----------------------------------------------------------------------------------------------------
    void setTimeData();
    //----------------------------------------------------------------------------------------------------
    void computeAllIndicators(const double* optSol);
//----------------------------------------------------------------------------------------------------
    void declareModelConfigurationParameters()
    {
        BusSubModel::declareDefaultModelConfigurationParameters();

        //bool
        addParameter("TimeIntegration", &mTimeIntegration, true, false, true);       /** TimeIntegration option indicates that each time variables at ports wil be summed using timestep weighting if true or simply cumulated if false - default = true */
        addParameter("StrictConstraint", &mStrictConstraint, true, false, true); /** Strictconstraint option enabling : at each time sum of connected flows should be equal to StrictConstraintBusValue - default = true */
        addParameter("MinConstraint", &mMinConstraint, false, false, true);       /** MinConstraint option enabling : at each time sum of connected flows should be >= MinConstraintBusValue - default = false */
        addParameter("MaxConstraint", &mMaxConstraint, false, false, true);       /** MaxConstraint option enabling : at each time sum of connected flows should be <= MaxConstraintBusValue - default = false */
        addParameter("StrictIntegrateConstraint", &mStrictIntegrateConstraint, false, false, true, "Integrated Strictconstraint option enabling: time integration on horizon of sum of connected flows should be equal to StrictConstraintBusValue - default = false ");
        addParameter("MinIntegrateConstraint", &mMinIntegrateConstraint, false, false, true, "Integrated MinConstraint option enabling: time integration on horizon of sum of connected flows should be >= MinConstraintBusValue - default = false");
        addParameter("MaxIntegrateConstraint", &mMaxIntegrateConstraint, false, false, true, "Integrated MaxConstraint option enabling: time integration on horizon of sum of connected flows should be <= MaxConstraintBusValue - default = false");
        addParameter("MaxFlexIntegrateConstraint", &mMaxFlexIntegrateConstraint, false, false, true);        /** Flexible Integrated MaxConstraint option enabling, time integration on horizon of sum of connected flows should be <= MaxConstraintBusValue - default = false - If not possible, huge penalty is added to still make solving possible */
        addParameter("MinIntegrateSeparateConstraint", &mMinIntegrateSeparateConstraint, false, false, true);    /** Min integrate constraint on separated periods. Condition to use this functionality : FuturSize = k * intervalBetweenIntegrates + periodIntegrate ; intervalBetweenIntegrates <= periodIntegrate ; TimeShift = k' * intervalBetweenIntegrates*/
        addParameter("MaxIntegrateSeparateConstraint", &mMaxIntegrateSeparateConstraint, false, false, true);  /** Max integrate constraint on separated periods. Condition to use this functionality : FuturSize = k * intervalBetweenIntegrates + periodIntegrate ; intervalBetweenIntegrates <= periodIntegrate ; TimeShift = k' * intervalBetweenIntegrates*/
    }

//----------------------------------------------------------------------------------------------------
    void declareModelParameters()
    {
        BusSubModel::declareDefaultModelParameters();
        //bool
        addParameter("UseExtrapolationFactor", &mUseExtrapolationFactor, false, SFunctionFlag({ eFTypeOrNot, {  &mMinConstraint, &mMaxConstraint, &mStrictIntegrateConstraint, &mMinIntegrateConstraint, &mMaxIntegrateConstraint, &mMaxFlexIntegrateConstraint} }), true, "When true the values of *BusValue are assumed over one year instead of optimization horizon"); // || mStrictConstraint
        //double
        addParameter("PenaltyCost", &mPenaltyCost, 1.e12, false, &mMaxFlexIntegrateConstraint, "", "Currency/kg");        /** Huge penalty Cost, in Currency/kg, added by MaxFlexIntegrateContraint in case of constraint overhead */
        addParameter("InitBusValue", &mInitBusValue, 0., false, true, "Initial Bus value to be used for first step of rolling horizon - default to 0");
        addParameter("StrictConstraintBusValue", &mStrictConstraintBusValue, 0., false, &mStrictConstraint);/** if Strictconstraint=true, at each time, sum of connected flows should be equal to this value - default is 0, for flow balance for example */
        addParameter("MinConstraintBusValue", &mMinConstraintBusValue, 0., &mMinConstraint, &mMinConstraint);/** if MinConstraint=true, at each time, sum of connected flows should be >= this value */
        addParameter("MaxConstraintBusValue", &mMaxConstraintBusValue, 0., &mMaxConstraint, &mMaxConstraint);/** if MaxConstraint=true, at each time, sum of connected flows should be <= this value */
        addParameter("StrictIntegrateConstraintBusValue", &mStrictIntegrateConstraintBusValue, 0., &mStrictIntegrateConstraint, &mStrictIntegrateConstraint);/** if StrictIntegrateConstraint=true, time integration on horizon of sum of connected flows should be equal to this value - default is 0, for flow balance for example */
        addParameter("MinIntegrateConstraintBusValue", &mMinIntegrateConstraintBusValue, 0., SFunctionFlag({ eFTypeOrNot, {&mMinIntegrateConstraint, &mMinIntegrateSeparateConstraint} }), SFunctionFlag({ eFTypeOrNot, {&mMinIntegrateConstraint, &mMinIntegrateSeparateConstraint} }));/** if MinIntegrateConstraint=true, time integration on horizon of sum of connected flows should be >= this value */
        addParameter("MaxIntegrateConstraintBusValue", &mMaxIntegrateConstraintBusValue, 0., SFunctionFlag({ eFTypeOrNot, {&mMaxIntegrateConstraint, &mMaxIntegrateSeparateConstraint} }), SFunctionFlag({ eFTypeOrNot, {&mMaxIntegrateConstraint, &mMaxIntegrateSeparateConstraint} }));/** if MaxIntegrateConstraint=true, time integration on horizon of sum of connected flows should be <= this value */
        addParameter("MaxFlexIntegrateConstraintBusValue", &mMaxFlexIntegrateConstraintBusValue, 0., &mMaxFlexIntegrateConstraint, &mMaxFlexIntegrateConstraint);/** if MaxFlexIntegrateConstraint=true, time integration on horizon of sum of connected flows should be <= this value - If not possible, huge penalty is added to still make solving possible  */
        //int
        addParameter("PeriodIntegrateConstraint", &mPeriodIntegrateConstraint, 0, false, true, "", "Nb"); /** if different of 0, maxIntegrateConstraint is applied over the windows of size PeriodIntegrateConstraint. If mMaxIntegrateSeparateConstraint is true, the period are separated*/
        addParameter("IntervalBetweenIntegrals",&mIntervalBetweenIntegrals, 0, false, true, "Interval between integrate constraints","Nb");
        
    }
//----------------------------------------------------------------------------------------------------
    void declareModelInterface()
    {
        BusSubModel::declareDefaultModelInterface();

        addIO("PenaltyConstraintCosts", &mExpPenaltyConstraintCosts, mCurrency);    /** Computed penalty costs */
        setPenaltyConstraintExpression("PenaltyConstraintCosts");
        addControlIO("BusBalance", &mBusBalance, mEnergyVector->pFluxUnit(), &mHistBusBalance, &mInitBusValue);

        addIO("BusConstraintGap", &mExpConstraintGap, mEnergyVector->pFluxUnit());
        if(mMaxIntegrateConstraint || mMinIntegrateConstraint || mMinIntegrateSeparateConstraint || mMaxIntegrateSeparateConstraint)
        {
            addIO("Integration", &mExprIntegrate, mEnergyVector->pStorageUnit());
        }
    }

    void declareModelIndicators() {
        // Supported types are: double
        BusSubModel::declareDefaultModelIndicators();
        QString storageUnit = *(mEnergyVector->pStorageUnit());
        mInputIndicators->addIndicator("Integrated bus balance", &mBusEnergyBalance, &mExportIndicators, "Integrated bus balance", storageUnit,"BusBalance");
    }
//----------------------------------------------------------------------------------------------------
    void setParameters(double aMinConstraintBusValue, double aMaxConstraintBusValue, double aStrictConstraintBusValue, double aMinIntegrateConstraintBusValue, 
        double aMaxIntegrateConstraintBusValue, double aStrictIntegrateConstraintBusValue, double aMaxFlexIntegrateConstraintBusValue) ;
//----------------------------------------------------------------------------------------------------
    void buildModel();                                                              // build minimum formulation
    void finalizeModelData();
    void computeAllContribution();
//----------------------------------------------------------------------------------------------------
    MIPModeler::MIPExpression1D busBalance() {return mBusBalance;}
    void initBalance() ;
    void addExpressionToBalance(MIPModeler::MIPExpression1D &aFluxExpression) ;
    void addExpressionToBalance(MIPModeler::MIPExpression &aFluxExpression) ;
    void addStrictConstraint() ;
    void addMinConstraint() ;
    void addMaxConstraint() ;
    void addStrictIntegrateConstraint() ;
    void addMinIntegrateConstraint() ;
    void addMinIntegrateConstraint(int period);
    void addMaxIntegrateConstraint() ;
    void addMaxIntegrateConstraint(int period);
    void addMaxFlexIntegrateConstraint() ;
    //void addMaxIntegrateSeparateConstraint(int period, int intervalBetween);
    void addIntegrateSeparateConstraint(int period, int intervalBetween);

//----------------------------------------------------------------------------------------------------
protected:
    bool mTimeIntegration ;                     /** Use timestep weighting of each time variable at ports if true or simply add them if false - Default to true */

    bool mUseExtrapolationFactor;               /** When true the values of *BusValue are assumed over one year instead of optimization horizon*/
    bool mStrictConstraint;                     /** Use strict equality of instantaneous value as aggregation constraint model - Default to true*/
    bool mMinConstraint;                        /** Use Min inequality of instantaneous value as aggregation constraint model - Default to false*/
    bool mMaxConstraint;                        /** Use Max inequality of instantaneous value as aggregation constraint model - Default to false*/
    bool mStrictIntegrateConstraint;            /** Use strict equality of integrated value as aggregation constraint model - Default to false*/
    bool mMinIntegrateConstraint;               /** Use Min inequality of integrated value as aggregation constraint model - Default to false*/
    bool mMaxIntegrateConstraint;               /** Use Max inequality of integrated value as aggregation constraint model - Default to false*/
    bool mMaxFlexIntegrateConstraint;           /** Use Max flexible integrated value as aggregation constraint model - Default to false*/
    bool mMinIntegrateSeparateConstraint;       /** Use Min Separate constraint - default to false */
    bool mMaxIntegrateSeparateConstraint;       /** Use Max Separate constraint - default to false */

    double mStrictConstraintBusValue ;           /** Instantaneous Equality constraint value, default to 0 to perform bus balance */
    double mMinConstraintBusValue ;              /** Instantaneous Min constraint value, default to 0 to perform bus balance */
    double mMaxConstraintBusValue ;              /** Instantaneous Max constraint value, default to 0 to perform bus balance */
    double mStrictIntegrateConstraintBusValue ;  /** Equality constraint on cumulated value , default to 0 to perform bus balance */
    double mMinIntegrateConstraintBusValue ;     /** Min constraint on cumulated value, default to 0 to perform bus balance */
    double mMaxIntegrateConstraintBusValue ;     /** Max constraint on cumulated value, default to 0 to perform bus balance */
    double mMaxFlexIntegrateConstraintBusValue ; /** Max flexible constraint on cumulated value, default to 0 to perform bus balance */

    double mInitBusValue;                       /** Bus value at t=0 (default 0)*/
    double mPenaltyCost;
    int mIntervalBetweenIntegrals;              /** Interval to skip between each integrate constraint (int)*/
    int mPeriodIntegrateConstraint; /** Size of the period of integration to compute contraint over time */

    MIPModeler::MIPExpression   mExpPenaltyConstraintCosts;      /** Scalar penalty for not meeting constraint */
    MIPModeler::MIPExpression1D mBusBalance ;

    MIPModeler::MIPExpression mExpConstraintGap ;
    MIPModeler::MIPVariable0D mVarConstraintGap;

    MIPModeler::MIPData1D mHistBusBalance;
    MIPModeler::MIPExpression1D  mExprIntegrate ;

    // for GAMS
    std::vector<std::string> mPortVarSet;
    std::vector<double> mPortVarCoeff;
    std::vector<double> mPortVarOffSet;
    std::vector<double> mPortVarDirection;
    std::vector<double> mPortVarTimeDepend;

    //Indicateurs
    std::vector<double> mBusEnergyBalance;
};

#endif // NODELAW_H
