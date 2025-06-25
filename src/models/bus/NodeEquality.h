/* --------------------------------------------------------------------------
 * File: NodeEquality.h
 * version 1.0
 * Author: Alain Ruby
 * Date 23/07/2019
 *---------------------------------------------------------------------------
 * Description: Model imposing Node addition node constraint on Bus Component
 * --------------------------------------------------------------------------
 */

#ifndef NODEEQUALITY_H
#define NODEEQUALITY_H

#include "globalModel.h"

#include "MIPModeler.h"
#include "BusSubModel.h"

/**
* \details
 * Description: NodeEquality must be used to set equality constraint between several models connected together through SameValue Bus.
*/
class MODELS_DECLSPEC NodeEquality : public BusSubModel
{
public:
//----------------------------------------------------------------------------------------------------
    NodeEquality(QObject* aParent);
    ~NodeEquality();
//----------------------------------------------------------------------------------------------------
    void setTimeData();
//----------------------------------------------------------------------------------------------------
    void computeAllIndicators(const double* optSol);
//----------------------------------------------------------------------------------------------------
    void declareModelConfigurationParameters()
    {
        SubModel::declareDefaultModelConfigurationParameters() ;
        //bool
        addParameter("ImposedBusValue", &mImposedBusValue, false, false, true);     /** If true: enforce ImposedBusValue value to node connected to the bus - Default is false*/
    }

//----------------------------------------------------------------------------------------------------
    void declareModelParameters()
    {
        declareDefaultModelParameters();
        //double
        addParameter("BusValue", &mBusValue, -1.e33, &mImposedBusValue, &mImposedBusValue);     /** Optionnal imposed value at ValueNode Bus if ImposedBusValue = true */
        addParameter("MaxBusValue", &mMaxBusValue, 1.e15, true, true);            /** Mandatory maximum value at ValueNode Bus n */
        addParameter("MinBusValue", &mMinBusValue, 0., false, true);  /** Optional minimal value for the bus potential */
    }
//----------------------------------------------------------------------------------------------------
    void declareModelInterface()
    {
        declareDefaultModelInterface();
        addIO("BusValue", &mExprBusValue, "unknown") ;
    }

    void declareModelIndicators() {
        // Supported types are: double
        BusSubModel::declareDefaultModelIndicators();
        mInputIndicators->addIndicator("Mean value during time period", &mBusMeanValue, &mExportIndicators, "", "","Mean");
    }
//----------------------------------------------------------------------------------------------------
    void setParameters(double aBusValue, double aMaxBusValue);
//----------------------------------------------------------------------------------------------------
    void buildModel();                                                              // build minimum formulation
//----------------------------------------------------------------------------------------------------
    MIPModeler::MIPExpression1D busValue() {return mExprBusValue;}
//----------------------------------------------------------------------------------------------------

protected:
    // optional setting
    bool mImposedBusValue ;          /** Impose Bus value or it computed  - false bu default*/
    double mMaxBusValue ;            /** Maximum (if >0) or minimum (if <0) value of BusSameValue bus potential */
    double mBusValue ;               /** Optional imposed BusSameValue bus potential */
    double mMinBusValue;            /** Optional minimal value for the bus potential */

    // MILP variables
    MIPModeler::MIPVariable1D mVarBusValue;     /** MILP Variable on BusSameValue Bus potential */

    /** MILP 0D Variable on component maximum power sizing */
    MIPModeler::MIPExpression1D mExprBusValue ;

    //Indicators
    std::vector<double> mBusMeanValue;
};

#endif // NODEEQUALITY_H
