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
    NodeLaw(CairnObject* aParent);
    ~NodeLaw();
//----------------------------------------------------------------------------------------------------
    void computeModelContribution() override;
    void setTimeData();
//----------------------------------------------------------------------------------------------------
    void computeAllIndicators(const double* optSol);
//----------------------------------------------------------------------------------------------------
    void declareModelConfigurationParameters()
    {
        BusSubModel::declareDefaultModelConfigurationParameters();
    }

//----------------------------------------------------------------------------------------------------
    void declareModelParameters()
    {
        BusSubModel::declareDefaultModelParameters();
        //bool
        addParameter("UseExtrapolationFactor", &mUseExtrapolationFactor, false, false, true, "When true the values of *BusValue are assumed over one year instead of optimization horizon");  
        //double
        addParameter("InitBusValue", &mInitBusValue, 0., false, true, "Initial Bus value to be used for first step of rolling horizon - default to 0");
        addParameter("StrictConstraintBusValue", &mStrictConstraintBusValue, 0., false, true, "At each time sum of connected flows should be equal to this value - default is 0 - for flow balance for example");
    }
//----------------------------------------------------------------------------------------------------
    void declareModelInterface()
    {
        BusSubModel::declareDefaultModelInterface();

        addControlIO("BusBalance", &mBusBalance, true, mMainCarrier->pFluxUnit(), &mHistBusBalance, &mInitBusValue);
    }

    void declareModelIndicators() {
        // Supported types are: double
        BusSubModel::declareDefaultModelIndicators();
        mInputIndicators->addIndicator("Integrated bus balance", &mBusEnergyBalance, &mExportIndicators, "Integrated bus balance", mMainCarrier->pStorageUnit(),"BusBalance");
    }

    void computeInitialData() override;

    MIPModeler::MIPExpression1D busBalance() {return mBusBalance;}
    void addExpressionToBalance(MIPModeler::MIPExpression1D &aFluxExpression);
    void addExpressionToBalance(MIPModeler::MIPExpression &aFluxExpression);

    void addStrictConstraint() ;

//----------------------------------------------------------------------------------------------------
protected:
    bool mUseExtrapolationFactor;        /** When true the values of *BusValue are assumed over one year instead of optimization horizon*/
    double mStrictConstraintBusValue;   /** Instantaneous Equality constraint value, default to 0 to perform bus balance */
    double mInitBusValue;               /** Bus value at t=0 (default 0)*/

    MIPModeler::MIPExpression1D mBusBalance;

    MIPModeler::MIPData1D mHistBusBalance;

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
