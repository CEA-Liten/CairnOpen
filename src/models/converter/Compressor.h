#ifndef Compressor_H
#define Compressor_H

#include "globalModel.h"
#include "ConverterSubModel.h"

/**
 * \details
This component models a compressor that compress an inlet flow, between MinFlowrate and MaxFlowrate, using electricity.

 .. figure:: ../images/Compressor.svg
   :alt: IO Compressor
   :name: IOCompressor
   :width: 200
   :align: center

   I/O Compressor

The default model calculated the electrical consumption based on the following formula:

.. math::

    P=\frac{1}{\eta_is}.n.\dot{m}.c_p.T_1.(\tau^(frac{\gamma-1}{\eta.\gamma})-1)

It requires:

- the motor efficiency multiplied by the isentropic efficiency :math:`\eta_is`,
- the number of compressor stages :math:`n`,
- the mass flow rate :math:`\dot{m}`,
- the heat capacity :math:`c_p`,
- the inlet temperature :math:`T_1`,
- the isentropic coefficient :math:`\gamma`

Additionally, there is a polytropic model that requires polytropic efficiency and polytropic coefficient.

Losses can be considered.
*/
class MODELS_DECLSPEC Compressor : public ConverterSubModel {

public:
//----------------------------------------------------------------------------------------------------
    Compressor(CairnObject* aParent);
    ~Compressor();
//----------------------------------------------------------------------------------------------------
    int checkConsistency();
//----------------------------------------------------------------------------------------------------
    void computeInitialData() override;
    void computeModelContribution() override;
    void computeEconomicalContribution();
//----------------------------------------------------------------------------------------------------
    void computeAllIndicators(const double* optSol);
//----------------------------------------------------------------------------------------------------
    void ComputeElecPowerMapPOut(double aCp_Gas, double ak, double aEta, const bool aRelaxedFormSOE,  const MIPModeler::MIPLinearType& methode);
//----------------------------------------------------------------------------------------------------
    void ComputeElecPowerMapTIn(double aCp_Gas, double ak, double aEta, const bool aRelaxedFormSOE,  const MIPModeler::MIPLinearType& methode);
//----------------------------------------------------------------------------------------------------
    void computeUsedPower_Steam_PressureOut(const bool aRelaxedFormSOE);
//---------------------------------------------------------------------------------------------------
    // Units: use the following, instead of the IS Units leading to "scaling" troubles during solving step
    // Mass Flow    : kg/h
    // Power flow   : MW
    // Mass         : kg
    // Energy       : MWh
    // Time         : Hours
    void declareModelConfigurationParameters() {
        ConverterSubModel::declareDefaultModelConfigurationParameters();
        //bool
        addParameter("UsePolytropicModel",&mUsePolytropicModel, false, false, true, "If true: use optional model of Polytropic compression of Ideal Gaz - default = false","", "PolytropicModel");
        addParameter("UseVariablePOut", &mUseVariablePOut, false, false, true, "If true: the power consumption of compressor depends on the pressure out - default = false","", "VariablePout");
        addParameter("UseVariableTIn", &mUseVariableTIn, false, false, true, "If true: the power consumption of compressor depends on the pressure out - default = false","", "VariableTin");
        addParameter("UseSteamMap", &mUseSteamMap, false, false, true, "If true: add a map of performance SteamMap which uses steam, pressure out to compute the power used - works in the case of a constant volume in the compressor - see bouin_7_cont for an example of use","", "SteamInput");
        addParameter("AddLosses", &mAddLosses, false, false, true, "If true: consider losses during compression");
    }

//----------------------------------------------------------------------------------------------------
    void declareModelParameters()
    {
        ConverterSubModel::declareDefaultModelParameters();
        // Supported types are: double, int, std::vector<double> or std::vector<int>
        // addParameter to InputParam instance for input data coming from User File : maximum power, performance maps...
        
        //bool
        addParameter("UseLOG", &mUseLOG, true, false, mUseVariablePOut || mUseVariableTIn, "Choose the model of linearization : if true: use MIP_LOG variables (more rapid to reach the optimal). If false: use MIP_SOS variables (more rapid to find first solution) - default true", "", "VariablePout");

        //int
        addParameter("PrecisionPressure", &mPrecisionPressure, 0, &mUseVariablePOut, &mUseVariablePOut, "Number of division in pressure axis on the map", "", "VariablePout");
        addParameter("PrecisionTemperature", &mPrecisionTemperature, 0, &mUseVariableTIn, &mUseVariableTIn, "Number of division in temperature axis on the map ", "", "VariableTin");
        addParameter("PrecisionMassFlow", &mPrecisionMassFlow, 0, SFunctionFlag({ eFTypeOrNot, { &mUseVariableTIn, &mUseVariablePOut} }), SFunctionFlag({ eFTypeOrNot, { &mUseVariableTIn, &mUseVariablePOut} }), "Number of division in flow of H2 axis on the map", "", "VariableTin");

        //double
        addParameter("POutletFixe", &mPOutletFixe, 0., false, true, "Allow the user to give a constant pressure out", "");


        addParameter("MinFlow", &mMinFlow, 0., false, true, "Optional Minimal flow", mMainCarrier->pFlowrateUnit()) ;
        addParameter("MaxFlow", &mMaxFlow, INFINITY_VAL, true, true, "Maximal flow - Carefull: Capex is per unit of Power of Compressor", mMainCarrier->pFlowrateUnit()) ;
        addParameter("MotorEfficiency", &mMotorEfficiency, 0., true, true, "Electrical Motor Efficiency", "-");
        addParameter("NbStages", &mNbStages, 0., true, true, "Number of compression stages", "");
        addParameter("TInlet", &mTInlet, 20., true, true, "Inlet Temperature", "degC");
		addParameter("PowerConsumption", &mPowerConsumption, 0., false, true, "Electrical consumption of the compressor");
        addParameter("PolytropicEfficiency",&mPolytropicEfficiency, 1., &mUsePolytropicModel, &mUsePolytropicModel, "PolytropicEfficiency efficiency","", "PolytropicModel");
        addParameter("PolytropicCoefficient",&mPolytropicCoefficient, 0., &mUsePolytropicModel, &mUsePolytropicModel, "PolytropicCoefficient","", "PolytropicModel");   
        addParameter("IsentropicEfficiency", &mIsentropicEfficiency, 1., SFunctionFlag({ eFTypeNotAnd, { &mUsePolytropicModel } }), SFunctionFlag({ eFTypeOrNot, {&mUsePolytropicModel} }), "Isentropic efficiency");   /**  */
        addParameter("Losses", &mLosses, 0., &mAddLosses, &mAddLosses, "Percentage of inlet flow lost during compression", "-");
        
        //vector
        addPerfParam("UsedElecPowerSetPoint", &mUsedElecPowerSetPoint, &mUseSteamMap, &mUseSteamMap, "z-axis in the map steamMap", "");
        addPerfParam("SteamSetPoint", &mSteamSetPoint, &mUseSteamMap, &mUseSteamMap, "x-axis in the map steamMap", "");
        addPerfParam("PressureOutSetPoint", &mPressureOutSetPoint, &mUseSteamMap, &mUseSteamMap, "y-axis in the map steamMap", "");
    }


    void declareModelInterface()
    {
        ConverterSubModel::declareDefaultModelInterface();

        /* Register IO expressions to be exported (published) as results (to the external, e.g., Pegase) */
        addSizeMaxIO("MaxPower", &mExpSizeMax, true, mPortUsedPower->pFluxUnit());        /** Maximal power used by the compressor */

        addIO("UsedPower", &mExpUsedPower, true, mPortUsedPower->pFluxUnit());       /** Computed power used by the compressor */
        addIO("InMassFlowRate", &mExpInMassFlow, true, mPortInMassFlowRate->pFluxUnit());         /** input flow compressed by the compressor */
        addIO("OutMassFlowRate", &mExpOutMassFlow, true, mPortOutMassFlowRate->pFluxUnit());         /** output flow compressed by the compressor, can be different from input flow if losses are considered */
        addIO("Pressure_out", &mExpPOut, true, mPortOutMassFlowRate->pPotentialUnit());        /** Pressure at the exit of the compressor */

        addIO("Steam", &mExpSteam, &mUseSteamMap, mPortInMassFlowRate->pFluxUnit());    /** quantity of steam input in the compressor*/
        addIO("TemperatureIn", &mExpTIn, &mUseVariableTIn, "degC"); /** Temperature before compression */
    
        /* Register non-IO 0D-expressions in order to automatically allocate and close them */
        addExp(&mExpTOutlet);

        /* Register non-IO 1D-expressions in order to automatically allocate and close them */
        //....
    }

    void declareModelIndicators() {
        ConverterSubModel::declareDefaultModelIndicators(&mExportIndicators);
        mInputIndicators->addIndicator("Max MassFlowRate", &mMaxMFR, &mExportIndicators, "Maximal mass flow rate", SFunctionUnit({ eFTypeDivision, { mPortUsedPower->pMassUnit() }, "h"}), "MaxMFR");
    }

    double getInletPressure();
    double getOutletPressure();

    void initDefaultPorts() 
    {
        mDefaultPorts.clear();
        //PortInMassFlowRate - left
        std::map<std::string, std::string> portInMassFlowRate;
        portInMassFlowRate["Name"] = "PortL0";
        portInMassFlowRate["Position"] = "left";
        portInMassFlowRate["CarrierType"] = ANY_Fluid();
        portInMassFlowRate["Direction"] = KCONS();
        portInMassFlowRate["Variable"] = "InMassFlowRate";
        mDefaultPorts["PortInMassFlowRate"] = portInMassFlowRate;
        //PortOutMassFlowRate - left
        std::map<std::string, std::string> portOutMassFlowRate;
        portOutMassFlowRate["Name"] = "PortL1";
        portOutMassFlowRate["Position"] = "left";
        portOutMassFlowRate["CarrierType"] = ANY_Fluid();
        portOutMassFlowRate["Direction"] = KPROD();
        portOutMassFlowRate["Variable"] = "OutMassFlowRate";
        mDefaultPorts["PortOutMassFlowRate"] = portOutMassFlowRate;

        //PortUsedPower - right
        std::map<std::string, std::string> portUsedPower;
        portUsedPower["Name"] = "PortR0";
        portUsedPower["Position"] = "right";
        portUsedPower["CarrierType"] = Electrical();
        portUsedPower["Direction"] = KCONS();
        portUsedPower["Variable"] = "UsedPower";
        mDefaultPorts["PortUsedPower"] = portUsedPower;
    }

    void setPortPointers() {
        mPortInMassFlowRate = getPort("PortInMassFlowRate");
        mPortOutMassFlowRate = getPort("PortOutMassFlowRate");
        mPortUsedPower = getPort("PortUsedPower");
    }

//----------------------------------------------------------------------------------------------------
protected:
    MilpPort* mPortInMassFlowRate;
    MilpPort* mPortOutMassFlowRate;
    MilpPort* mPortUsedPower;

    std::string mPowerUnit;
    std::string mMassUnit;

    // MILP variables
    MIPModeler::MIPVariable0D mVarTOutlet;
    MIPModeler::MIPVariable1D mUsedPower;
    MIPModeler::MIPVariable1D mMassFlow;
    MIPModeler::MIPVariable1D mMassFlowOut;
    MIPModeler::MIPVariable1D mPOut;
    MIPModeler::MIPVariable1D mTIn;
    MIPModeler::MIPVariable1D mSteam;

    MIPModeler::MIPExpression mExpTOutlet;
    MIPModeler::MIPExpression1D mExpUsedPower;
    MIPModeler::MIPExpression1D mExpInMassFlow;
    MIPModeler::MIPExpression1D mExpOutMassFlow;
    MIPModeler::MIPExpression1D mExpPOut;
    MIPModeler::MIPExpression1D mExpTIn;
    MIPModeler::MIPExpression1D mExpSteam;

    double mSpecificHeatRatio;
    double mCp_Gas;
    double mK;
    double mEta;
    double mPInlet;  //Inlet Pressure
    double mPOutlet;  //Outlet Pressure

    // model parameters
    double mPowerConsumption;
    double mMinPower;
    double mMinFlow;
    double mMaxPower;
    double mMaxFlow;
    double mNbStages ;
    double mMotorEfficiency ;
    double mIsentropicEfficiency ;
    double mPolytropicEfficiency ;
    double mPolytropicCoefficient ;
    double mTInlet ;
    double mPOutletFixe;
    double mLosses;

    bool mUsePolytropicModel ;
    bool mUseVariablePOut = false;
    bool mUseVariableTIn = false;
    bool mUseLOG = true;
    bool mUseSteamMap = false;
    bool mAddLosses;

    int mPrecisionPressure;
    int mPrecisionMassFlow;
    int mPrecisionTemperature;

    std::vector<double> mUsedElecPowerSetPoint;
    std::vector<double> mSteamSetPoint;
    std::vector<double> mPressureOutSetPoint;
    std::vector<double> mMaxMFR;
};

#endif
