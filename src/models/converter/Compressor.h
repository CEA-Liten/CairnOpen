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
    Compressor(QObject* aParent);
    ~Compressor();
//----------------------------------------------------------------------------------------------------
    int checkConsistency();
//----------------------------------------------------------------------------------------------------
    void buildModel();
//----------------------------------------------------------------------------------------------------
    void computeEconomicalContribution();
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
        mInputParam->addToConfigList({"ConstantEfficiency","VariablePout","VariableTin","SteamInput","PolytropicModel"});
        //bool
        addParameter("UsePolytropicModel",&mUsePolytropicModel, false, false, true, "If true: use optional model of Polytropic compression of Ideal Gaz - default = false","",{"PolytropicModel"});
        addParameter("UseVariablePOut", &mUseVariablePOut, false, false, true, "If true: the power consumption of compressor depends on the pressure out - default = false","",{"VariablePout"});
        addParameter("UseVariableTIn", &mUseVariableTIn, false, false, true, "If true: the power consumption of compressor depends on the pressure out - default = false","",{"VariableTin"});
        addParameter("UseSteamMap", &mUseSteamMap, false, false, true, "If true: add a map of performance SteamMap which uses steam, pressure out to compute the power used - works in the case of a constant volume in the compressor - see bouin_7_cont for an example of use","",{"SteamInput"});
        addParameter("AddLosses", &mAddLosses, false, false, true, "If true: consider losses during compression");
    }

//----------------------------------------------------------------------------------------------------
    void declareModelParameters()
    {
        ConverterSubModel::declareDefaultModelParameters();
        // Supported types are: double, int, std::vector<double> or std::vector<int>
        // addParameter to InputParam instance for input data coming from User File : maximum power, performance maps...
        // addParameter to mInputData instance for input data coming from Persee/PEGASE memory : time series (coming from PEGASE), state variables...
        
        //bool
        addParameter("UseLOG", &mUseLOG, true, false, mUseVariablePOut || mUseVariableTIn, "Choose the model of linearization : if true: use MIP_LOG variables (more rapid to reach the optimal). If false: use MIP_SOS variables (more rapid to find first solution) - default true", "", { "VariablePout" });

        //int
        addParameter("PrecisionPressure", &mPrecisionPressure, 0, &mUseVariablePOut, &mUseVariablePOut, "Number of division in pressure axis on the map", "", { "VariablePout" });
        addParameter("PrecisionTemperature", &mPrecisionTemperature, 0, &mUseVariableTIn, &mUseVariableTIn, "Number of division in temperature axis on the map ", "", { "VariableTin" });
        addParameter("PrecisionMassFlow", &mPrecisionMassFlow, 0, SFunctionFlag({ eFTypeOrNot, { &mUseVariableTIn, &mUseVariablePOut} }), SFunctionFlag({ eFTypeOrNot, { &mUseVariableTIn, &mUseVariablePOut} }), "Number of division in flow of H2 axis on the map", "", { "VariableTin" });

        //double
        addParameter("POutletFixe", &mPOutletFixe, 0., false, true, "Allow the user to give a constant pressure out", "");
        addParameter("MinFlow", &mMinFlow, 0., false, true, "Optional Minimal flow",{"FlowrateUnit"},{""}) ;
        addParameter("MaxFlow", &mMaxFlow, INFINITY_VAL, true, true, "Maximal flow - Carefull: Capex is per unit of Power of Compressor",{"FlowrateUnit"},{""}) ;
        addParameter("MotorEfficiency", &mMotorEfficiency, 0., true, true, "Electrical Motor Efficiency", "-");
        addParameter("NbStages", &mNbStages, 0., true, true, "Number of compression stages", "");
        addParameter("TInlet", &mTInlet, 20., true, true, "Inlet Temperature", "degC");
		addParameter("PowerConsumption", &mPowerConsumption, 0., false, true, "Electrical consumption of the compressor");
        addParameter("PolytropicEfficiency",&mPolytropicEfficiency, 1., &mUsePolytropicModel, &mUsePolytropicModel, "PolytropicEfficiency efficiency","",{"PolytropicModel"});
        addParameter("PolytropicCoefficient",&mPolytropicCoefficient, 0., &mUsePolytropicModel, &mUsePolytropicModel, "PolytropicCoefficient","",{"PolytropicModel"});   
        addParameter("IsentropicEfficiency", &mIsentropicEfficiency, 1., SFunctionFlag({ eFTypeNotAnd, { &mUsePolytropicModel } }), SFunctionFlag({ eFTypeOrNot, {&mUsePolytropicModel} }), "Isentropic efficiency", "");   /**  */
        addParameter("Losses", &mLosses, 0., &mAddLosses, &mAddLosses, "Percentage of inlet flow lost during compression", "-");
        
        //vector
        addPerfParam("UsedElecPowerSetPoint", &mUsedElecPowerSetPoint, &mUseSteamMap, &mUseSteamMap, "z-axis in the map steamMap", "", {"SteamInput"});
        addPerfParam("SteamSetPoint", &mSteamSetPoint, &mUseSteamMap, &mUseSteamMap, "x-axis in the map steamMap", "", {"SteamInput"});
        addPerfParam("PressureOutSetPoint", &mPressureOutSetPoint, &mUseSteamMap, &mUseSteamMap, "y-axis in the map steamMap", "", {"SteamInput"});
    }


    void declareModelInterface()
    {
        ConverterSubModel::declareDefaultModelInterface();

        addIO("UsedPower", &mExpUsedPower, mPortUsedPower->pFluxUnit());       /** Computed power used by the compressor */
        addIO("MaxPower", &mExpSizeMax, mPortUsedPower->pFluxUnit());        /** Maximal power used by the compressor */
        addIO("InMassFlowRate", &mExpInMassFlow, mPortInMassFlowRate->pFluxUnit());         /** input flow compressed by the compressor */
        addIO("OutMassFlowRate", &mExpOutMassFlow, mPortOutMassFlowRate->pFluxUnit());         /** output flow compressed by the compressor, can be different from input flow if losses are considered */
        addIO("State", &mExpState, mPortUsedPower->pFluxUnit());         /** ON / OFF state of the compressor */
        addIO("Pressure_out", &mExpPOut, mPortOutMassFlowRate->pPotentialUnit());        /** Pressure at the exit of the compressor */

        if (mUseSteamMap) {
            addIO("Steam", &mExpSteam, mPortInMassFlowRate->pFluxUnit());    /** quantity of steam input in the compressor*/
        }
        if (mUseVariableTIn) {
            addIO("TemperatureIn", &mExpTIn, "degC"); /** Temperature before compression */
        }
        setOptimalSizeExpression("MaxPower");  // defines default expression should be used for OptimalSize computation and use in Economic analysis
        setOptimalSizeUnit(mPortUsedPower->pPowerUnit());  // defines default expression should be used for OptimalSize computation and use in Economic analysis
    }

    void declareModelIndicators() {
        ConverterSubModel::declareDefaultModelIndicators(&mExportIndicators);
        QString massUnit = mPortUsedPower->ptrEnergyVector()->MassUnit();
        mInputIndicators->addIndicator("Max MassFlowRate", &mMaxMFR, &mExportIndicators, "Maximal mass flow rate", massUnit + "/h","MaxMFR");
    }

    double getInletPressure();
    double getOutletPressure();

    void initDefaultPorts() 
    {
        mDefaultPorts.clear();
        //PortInMassFlowRate - left
        QMap<QString, QString> portInMassFlowRate;
        portInMassFlowRate["Name"] = "PortL0";
        portInMassFlowRate["Position"] = "left";
        portInMassFlowRate["CarrierType"] = ANY_Fluid();
        portInMassFlowRate["Direction"] = KCONS();
        portInMassFlowRate["Variable"] = "InMassFlowRate";
        mDefaultPorts.insert("PortInMassFlowRate", portInMassFlowRate);
        //PortOutMassFlowRate - left
        QMap<QString, QString> portOutMassFlowRate;
        portOutMassFlowRate["Name"] = "PortL1";
        portOutMassFlowRate["Position"] = "left";
        portOutMassFlowRate["CarrierType"] = ANY_Fluid();
        portOutMassFlowRate["Direction"] = KPROD();
        portOutMassFlowRate["Variable"] = "OutMassFlowRate";
        mDefaultPorts.insert("PortOutMassFlowRate", portOutMassFlowRate);

        //PortUsedPower - right
        QMap<QString, QString> portUsedPower;
        portUsedPower["Name"] = "PortR0";
        portUsedPower["Position"] = "right";
        portUsedPower["CarrierType"] = Electrical();
        portUsedPower["Direction"] = KCONS();
        portUsedPower["Variable"] = "UsedPower";
        mDefaultPorts.insert("PortUsedPower", portUsedPower);
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

    QString mPowerUnit;
    QString mMassUnit;

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
