/* --------------------------------------------------------------------------
 * File: Ramp.cpp
 * version 1.0
 * Author: Pimprenelle Parmentier
 * Date 09/03/2022
 *---------------------------------------------------------------------------
 * Description: Model imposing ramps or adding ramp costs for variable ConnectRamp.
 * --------------------------------------------------------------------------
 */

#ifndef Ramp_H
#define Ramp_H

#include "globalModel.h"

#include "MIPModeler.h"
#include "OperationSubModel.h"

/**
* \details

Model imposing constraints on ramp-down and ramp-up.

ConnectRamp must be connected to the flux of the component on which the ramp constraints must be applied.

Ramp-down and ramp-up costs can be considered.

 .. figure:: ../images/Ramp_ConstantProd.svg
   :alt: IO Ramp
   :name: IORamp
   :width: 200
   :align: center

   I/O Ramp

*/
class MODELS_DECLSPEC Ramp : public OperationSubModel
{
public:
//----------------------------------------------------------------------------------------------------
    Ramp(QObject* aParent);
    ~Ramp();
//----------------------------------------------------------------------------------------------------
    void setTimeData();
    void computeInitialData() override;
    void computeModelContribution() override;
//----------------------------------------------------------------------------------------------------
    void declareModelConfigurationParameters()
    {
        OperationSubModel::declareDefaultModelConfigurationParameters();
        //bool
        addParameter("ConstantRamp", &mConstantRamp, true, false, true, "If true: the ramps are definied as a parameter else by dataseries","bool");
        addParameter("AllowShutDown", &mAllowShutDown,false, false, true, "If true: the variable can break the ramp to jump from MinValue to 0. Else it is not possible.","bool");
        addParameter("AddRampUpConstraint", &mAddRampUp, true, false, true, "If true: add a constraint on ramp up", "");
        addParameter("AddRampDownConstraint", &mAddRampDown, true, false, true, "If true: add a constraint on ramp down", "");
        addParameter("AddRampCost", &mAddRampCost, false, false, true, "If true: add ramp cost", "");
    }
//----------------------------------------------------------------------------------------------------
    void declareModelParameters()
    {
        OperationSubModel::declareDefaultModelParameters();
        //double
        addParameter("MaxVariable", &mMaxWeight, 1., true, true, "Default 1: Maximum of connected variable. If negative optimized between 0 and absolute value and Component Size has to be connected.","Flux");
        addParameter("MinInput", &mMinInput, 0., &mAllowShutDown, &mAllowShutDown,"Min abolute when weight is 1 relative else associated to the variable connected ", " * ComponentSize * MaxInput");
       
        addParameter("ConstantRampUpLimit", &mConstantRampUpLimit, 0., SFunctionFlag({ eFTypeNotAnd, {},  { &mConstantRamp, &mAddRampUp} }), SFunctionFlag({ eFTypeNotAnd, {},  { &mConstantRamp, &mAddRampUp} }), "Constant ramp up value (rate of MaxComponentSize)", "-/hour");
        addParameter("ConstantRampDownLimit", &mConstantRampDownLimit, 0., SFunctionFlag({ eFTypeNotAnd, {},  { &mConstantRamp, &mAddRampDown} }), SFunctionFlag({ eFTypeNotAnd, {},  { &mConstantRamp, &mAddRampDown} }), "Constant ramp down value (rate of MaxComponentSize)", "-/hour");

        addParameter("RampCost", &mRampCost, 0., &mAddRampCost, &mAddRampCost, "", "Currency/Flux");
        addParameter("MinSize", &mMinSize, 0., false, 1, "Minimal size of the component", "StorageUnit", "EcoInvestModel"); /** Minimum capacity  */


        //vector
        addTimeSeries("RampUpLimit", &mRampUpLimit, false, SFunctionFlag({ eFTypeNotAnd, {&mConstantRamp},  {&mAddRampUp} }), "Ramp up limit compared to MaxComponentSize per hour.", "-/hour");
        addTimeSeries("RampDownLimit", &mRampDownLimit, false, SFunctionFlag({ eFTypeNotAnd, {&mConstantRamp},  {&mAddRampDown} }), "Ramp down limit compared to MaxComponentSize per hour.", "-/hour");
        addTimeSeries("UseProfileConverterUse", &mConverterUse, false, true, "Time series of 0-1 when equal to 0 the ramp constraint is desactivated", "-");
    }
//----------------------------------------------------------------------------------------------------

    void declareModelInterface()
    {
        OperationSubModel::declareDefaultModelInterface();
        
        /* Register IO expressions to be exported (published) as results (to the external, e.g., Pegase) */
        addSizeMaxIO("ComponentSize", &mExpSizeMax, true, mEnergyVector->pFluxUnit()); // multiplier of the size of the ramp
        addControlIO("ConnectRamp", &mExpInput, true, mEnergyVector->pFluxUnit(), &mHistInput, &mInitialValue);

        //AllowShutDown
        addIO("State", &mExpState, &mAllowShutDown, "bool");  /** ON OFF state of the element connected to ramp */
        addIO("StartUp", &mExpStartUp, &mAllowShutDown, "bool");
        addIO("ShutDown", &mExpShutDown, &mAllowShutDown, "bool");

        /* Register non-IO 0D-expressions in order to automatically allocate and close them */
        addExp(&mExpWeight);

        /* Register non-IO 1D-expressions in order to automatically allocate and close them */
        addExp(&mExpRampCostVar, &mHorizon);
    }

    void declareModelIndicators() {
        OperationSubModel::declareDefaultModelIndicators();
    }
    void computeAllIndicators(const double* optSol) override;
//----------------------------------------------------------------------------------------------------
    void setParameters(double aMinConstraintBusValue, double aMaxConstraintBusValue, double aStrictConstraintBusValue) ;
//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------
    void addRelativeRamp();
    void addRelativeRampWithMinPower();
    void addRampCost();
//----------------------------------------------------------------------------------------------------

    void initDefaultPorts() {
        mDefaultPorts.clear();
        //PortConnectRamp - bottom
        QMap<QString, QString> portConnectRamp;
        portConnectRamp["Name"] = "PortB0";
        portConnectRamp["Position"] = "bottom";
        portConnectRamp["CarrierType"] = ANY_TYPE(); //Should be "Electrical" ?!
        portConnectRamp["Direction"] = KDATA();
        portConnectRamp["Variable"] = "ConnectRamp";
        mDefaultPorts.insert("PortConnectRamp", portConnectRamp);
    }

    //void setPortPointers() {
    //    mPortConnectRamp = getPort("PortConnectRamp");
    //}

protected:
    //MilpPort* mPortConnectRamp; 

    MIPModeler::MIPVariable1D mVarInput;
    MIPModeler::MIPVariable1D mRampCostVar;

    MIPModeler::MIPExpression mExpWeight;
    MIPModeler::MIPExpression1D mExpInput ;
    MIPModeler::MIPExpression1D mExpRampCostVar;

    double mMaxWeight;
    double mConstantRampUpLimit;
    double mConstantRampDownLimit;
    double mInitialValue;
    double mMinInput;
    double mRampCost;
    double mHistInput;
    double mMinSize;

    std::vector<double> mRampUpLimit;
    std::vector<double> mRampDownLimit;
    std::vector<double> mConverterUse;      /** time series for converter use availability */

    bool mConstantRamp;
    bool mAllowShutDown;
    bool mAddRampCost;
    bool mAddRampUp;
    bool mAddRampDown;
};

#endif // Ramp_H
