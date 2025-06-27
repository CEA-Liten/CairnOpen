/**
* \file		electrolyzer.h
* \brief	H2 electrolyzer model (simplified form)
* \version	1.0
* \author	Yacine Gaoua
* \date		08/02/2019
*/

#ifndef Converter_H
#define Converter_H

#include "globalModel.h"
#include "ConverterSubModel.h"


/**
 * \details
This component models electrical converter by efficiency model.

 .. figure:: ../images/Converter.svg
   :alt: IO Converter
   :name: IOConverter
   :width: 200
   :align: center

   I/O Converter

System may be ON or OFF and gives acccess to its States variable for combined constraints. 
Power in is limited to MaxPower value. 
This MaxPower can be optimized if the corresponding given parameter is negative. 

The efficiency can follow three models:

- simple (by default): the parameter Efficiency is constant

- given by a timeseries: the parameter Efficiency is multiplied by the timeseries UseProfilConverterUse

- given by a performance map. See the dedicated part in the documentation for more information.

.. caution::

  Additional input and output fluxes can be added but if they represent another energy carrier than the variable, do not forget to set CheckUnit to NO.
  For instance, if we want to represent a water consumption that represent 1% of the input power, we can add an input dedicated port with the variable InputFlux and a coefficient A of 0.01 but we must set CheckUnit to NO!

*/
class MODELS_DECLSPEC Converter : public ConverterSubModel {

public:
//----------------------------------------------------------------------------------------------------
    Converter(QObject* aParent);
    ~Converter();
//----------------------------------------------------------------------------------------------------
    void setTimeData() ;
//----------------------------------------------------------------------------------------------------
    void buildModel();
    int checkConsistency();

//----------------------------------------------------------------------------------------------------
    void computeEconomicalContribution();
    void computeAllIndicators(const double* optSol) override;
//----------------------------------------------------------------------------------------------------
    void declareModelInterface()
    {   // register expression for model Interface with global MilpProblem, Bus, other MilpComponent
        // Caution : Flux must be signed wrt to Bus balance impact : >0 if energy source, <0 else.
        ConverterSubModel::declareDefaultModelInterface();

        addIO("PowerIn", &mExpPower_In, mPortPowerIn->pFluxUnit()); /* Input power at voltage1 */
        addIO("MaxPower", &mExpSizeMax, mPortPowerIn->pFluxUnit()); /* Maximum sizing power, ie optimal power if negative maximum power was given in input */
        addIO("PowerOut", &mExpPower_Out, mPortPowerOut->pFluxUnit()); /* Output power */

        setOptimalSizeExpression("MaxPower") ;  // defines default expression should be used for OptimalSize computation and use in Economic analysis

        //setOptimalSizeUnit("Electrical","Flux") ;  // defines default expression should be used for OptimalSize computation and use in Economic analysis
        setOptimalSizeUnit(mPortPowerIn->pFluxUnit());  // defines default expression should be used for OptimalSize computation and use in Economic analysis
    }

    //----------------------------------------------------------------------------
    void declareModelConfigurationParameters() {
        ConverterSubModel::declareDefaultModelConfigurationParameters();
        mInputParam->addToConfigList({ "EcoInvestModel","EnvironmentModel", "AddOperationConstraints","PiecewiseEfficiency" });
        //bool
        addParameter("PiecewiseEfficiency", &mPiecewiseEfficiency, false, false, true, "Add optional production piecewise efficiency if true - default = false", "", {"PiecewiseEfficiency"});
        //The default value of mTimeSeriesPiecewiseEfficiency should be false. Otherwise, the parameter "NbSetpoints" will become always mandatory. 
        //This is because parameter "NbSetpoints" is declared before reading the value of "TimeSeriesPiecewiseEfficiency" from .json. Maybe we should consider passing isBlocking by reference to addParameter
        //Note, "NbSetpoints" should stay in declareModelConfigurationParameters
        addParameter("TimeSeriesPiecewiseEfficiency", &mTimeSeriesPiecewiseEfficiency, false, false, true, "", "", { "" }); /** Add optional production piecewise efficiency if true - default = false */
        //int
        addParameter("NbSetpoints", &mNbSetpoints, 0, &mTimeSeriesPiecewiseEfficiency, &mTimeSeriesPiecewiseEfficiency); /** Number of Setpoints : CompoName.InputSetPoint#1, .., CompoName.InputSetPoint#NbSetpoints and CompoName.OutputSetPoint#1, .., CompoName.OutputSetPoint#NbSetpoints */
    }

//----------------------------------------------------------------------------------------------------
    void declareModelParameters()
    {
        ConverterSubModel::declareDefaultModelParameters();
        // Supported types are: double, int, std::vector<double> or std::vector<int>
        // InputParam instance for input data coming from User File : maximum power, performance maps...
        // InputData instance for input data coming from Persee/PEGASE memory : time series (coming from PEGASE), state variables...

        //double
        addParameter("Efficiency", &mEfficiency, 1., SFunctionFlag({ eFTypeNotAnd, { &mPiecewiseEfficiency, &mTimeSeriesPiecewiseEfficiency} }), true, "", "", { "" }); /** Total constant Converter efficiency*/
        addParameter("Offset", &mOffset, 0., false, true, " Offset of consumption to add to PowerIn (PowerIn is then an affine function of PowerOut)","FluxUnit", { "" });
        addParameter("MaxPower",&mMaxPower, INFINITY_VAL, true, true, "maximum input flux through converter", "FluxUnit",{""});
        addParameter("MinPower", &mMinPower, 0., false, true, "optional minimum flux through converter - 0 by default.Relative value to MaxPower", "%MaxPower",{""});
        //vectors
        addTimeSeries("UseProfileConverterCoeff", &mConverterCoeff, false, true, "time series coeff for the efficiency", "", {""});
        addTimeSeries("UseProfileConverterLowerBound", &mConverterLowerBound, false, true, "time series coeff for the powerMin", "", {""});
        addTimeSeries("UseProfileConverterUpperBound", &mConverterUpperBound,false, true, "time series coeff for the powerMax", "", {""});
        addTimeSeries("UseProfileConverterUse", &mConverterUse, false, true, "Time Series of converter allowance used to simulate unavailability for failures or maintenance", "", {""});
        //
        mPowerInSetPointVec.resize(mNbSetpoints);
        mPowerOutSetPointVec.resize(mNbSetpoints);
        for (int i = 0; i < mNbSetpoints; i++) {
            mPowerInSetPointVec[i].resize(mHorizon);
            mPowerOutSetPointVec[i].resize(mHorizon);
        }
        QString aName = getCompoName();
        for (int i = 0; i < mNbSetpoints; i++) {
            addTimeSeries(aName + ".InputSetPoint#" + QString::number(i + 1), &mPowerInSetPointVec[i], &mTimeSeriesPiecewiseEfficiency, &mTimeSeriesPiecewiseEfficiency);
            addTimeSeries(aName + ".OutputSetPoint#" + QString::number(i + 1), &mPowerOutSetPointVec[i], &mTimeSeriesPiecewiseEfficiency, &mTimeSeriesPiecewiseEfficiency);
        }
        //
        addPerfParam("PowerInSetPoint", &mPowerInSetPoint, &mPiecewiseEfficiency, &mPiecewiseEfficiency, " z-axis in the 1D-map of efficiency", "PowerUnit", { "PiecewiseEfficiency" });
        addPerfParam("PowerOutSetPoint", &mPowerOutSetPoint, &mPiecewiseEfficiency, &mPiecewiseEfficiency, "x-axis in the 1D-map of efficiency", "PowerUnit", { "PiecewiseEfficiency" });
    }

    void declareModelIndicators() {
        ConverterSubModel::declareDefaultModelIndicators(&mExportIndicators);
    }

//-------------------------------------------------------------------------------------------------
    void mapEfficiency(std::vector<double> aPowerInSetPoint , std::vector<double> aPowerOutSetPoint, const bool aRelaxedFormSOE);
    void timeSeriesMapEfficiency(std::vector<std::vector<double>> aPowerInSetPoint, std::vector<std::vector<double>> aPowerOutSetPoint, const bool aRelaxedFormSOE);
//----------------------------------------------------------------------------------------------------
  
    void initDefaultPorts()
    {
        mDefaultPorts.clear();
        //PortPowerIn - left
        QMap<QString, QString> portPowerIn;
        portPowerIn["Name"] = "PortL0";  
        portPowerIn["Position"] = "left";
        portPowerIn["CarrierType"] = ANY_TYPE();
        portPowerIn["Direction"] = KCONS();  
        portPowerIn["Variable"] = "PowerIn";
        mDefaultPorts.insert("PortPowerIn", portPowerIn);  

        //PortPowerOut - right
        QMap<QString, QString> portPowerOut;
        portPowerOut["Name"] = "PortR0";
        portPowerOut["Position"] = "right";
        portPowerOut["CarrierType"] = ANY_TYPE();
        portPowerOut["Direction"] = KPROD();
        portPowerOut["Variable"] = "PowerOut";
        mDefaultPorts.insert("PortPowerOut", portPowerOut);
    }

    void setPortPointers() {
        mPortPowerIn = getPort("PortPowerIn");
        mPortPowerOut = getPort("PortPowerOut");
    }

protected:
    MilpPort* mPortPowerIn;
    MilpPort* mPortPowerOut;

    MIPModeler::MIPVariable1D mPower_In;
    MIPModeler::MIPVariable1D mPower_Out;

    MIPModeler::MIPExpression1D mExpPower_In;
    MIPModeler::MIPExpression1D mExpPower_Out;

    MIPModeler::MIPData1D mPowerInSetPoint;
    MIPModeler::MIPData1D mPowerOutSetPoint;

    std::vector<std::vector<double>> mPowerInSetPointVec;
    std::vector<std::vector<double>> mPowerOutSetPointVec;

    // model parameters
    int mNbSetpoints; //default is 0
    double mEfficiency;
    double mMaxPower;		
    double mMaxPowerOut;
    double mMinPower;
    double mOffset;
    bool mPiecewiseEfficiency;
    bool mTimeSeriesPiecewiseEfficiency; // default = false; !!! Don't make it true !!!

    std::vector<double> mConverterCoeff;
	std::vector<double> mConverterLowerBound;
    std::vector<double> mConverterUpperBound;
	std::vector<double> mMaxPowerTS;
    std::vector<double> mMinPowerTS;
};

#endif
