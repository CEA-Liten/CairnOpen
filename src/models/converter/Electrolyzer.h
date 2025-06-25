/**
* \file		Electrolyzer.h
* \brief	H2 Electrolyzer model (simplified form)
* \version	1.0
* \author	A.Ruby
* \date		08/02/2019
*/

#ifndef Electrolyzer_H
#define Electrolyzer_H

#include "globalModel.h"
#include "ConverterSubModel.h"

//linkPerseeModelClass Electrolyzer.h ElectrolyzerDetailed.h 
/**
 * \details
This component models a H2 production from electrical power by electrolysis.

 .. figure:: ../images/Electrolyzer.svg
   :alt: IO Electrolyzer
   :name: IOElectrolyzer
   :width: 200
   :align: center

   I/O Electrolyzer

It is a default model with a constant efficiency.
The power consumption is between minPower and maxPower.

.. note::
    A heat consumption can be taken into account by adding a dedicated port.
*/
class MODELS_DECLSPEC Electrolyzer : public ConverterSubModel {
public:
    //----------------------------------------------------------------------------
    Electrolyzer(QObject* aParent);
    ~Electrolyzer();
 
    //----------------------------------------------------------------------------------------------------
    void buildModel();
    int checkConsistency();
    //----------------------------------------------------------------------------------------------------
    void computeEconomicalContribution();
    void computeAllIndicators(const double* optSol) override;

    //----------------------------------------------------------------------------------------------------
    void declareModelInterface() {
        ConverterSubModel::declareDefaultModelInterface();

        // register expression for model Interface with global MilpProblem, Bus, other MilpComponent
        addIO("MaxUsablePower", &mExpUsablePower, mPortUsedPower->pFluxUnit());        	/** Computed allowed power available to electrolysis system */
        addIO("UsedPower", &mExpTotalPower, mPortUsedPower->pFluxUnit());					/** Computed electrolysis system power */
        addIO("H2MassFlowRate", &mExpFlow_H2, mPortH2MassFlowRate->pFluxUnit());  /** Computed electrolysis H2 flowrate production */
        addIO("MaxPower", &mExpSizeMax, mPortUsedPower->pFluxUnit());				    /** Computed sizing electrolysis system power */

        setOptimalSizeExpression("MaxPower");  // defines default expression should be used for OptimalSize computation and use in Economic analysis
        setOptimalSizeUnit(mPortUsedPower->pFluxUnit());  // defines default expression should be used for OptimalSize computation and use in Economic analysis
    }
    //----------------------------------------------------------------------------------------------------
    void declareModelConfigurationParameters() {
        mInputParam->addToConfigList({ "EcoInvestModel","EnvironmentModel","AddOperationConstraints","OptimizationOptions","Ageing" });
        ConverterSubModel::declareDefaultModelConfigurationParameters();
        //bool 
        addParameter("EfficiencyLHVbased", &mEfficiencyLHVbased, true, false, true, "efficiency type for electrolyzer", "bool", {   }); /** Add a control imposed (to test different controls with the same parameters) (todo)*/
        addParameter("AddAuxConso", &mAddAuxConso, false, false, true, "Constant elec consumption in proportion of maxpower: ", "bool", { "AddOperationConstraints" });
        addParameter("AddStdByConso", &mAddStdByConso, false, false, true, "Constant elec consumption in proportion of maxpower: equals to 0 if converterUse =0 but not when state = 0", "bool", { "AddOperationConstraints" });
    }
    
    void declareModelParameters()
    {
        ConverterSubModel::declareDefaultModelParameters();
        // Supported types are: double, int, std::vector<double> or std::vector<int>
        // InputParam instance for input data coming from User File : maximum power, performance maps...
        // to mInputData instance for input data coming from Persee/PEGASE memory : time series (coming from PEGASE), state variables...
        // 
        
        //Re-declare LifeTime and change default value
        addParameter("LifeTime", &mLifeTime, 10., false, SFunctionFlag({ eFTypeOrNot, { &mEcoInvestModel, &mEnvironmentModel} }), "LifeTime in years", "Year", { "EcoInvestModel" });  /** LifeTime in years */

        //double
        addParameter("Efficiency", &mEfficiency, 0.6, &mEfficiencyLHVbased, &mEfficiencyLHVbased, "Electrolyzer efficiency LHV based - computed only with variable part of energy used ie UsedPower - StdByConsumption Over Produced H2 flowrate ", "", {}); /** Total constant Converter efficiency*/
        addParameter("Efficiency_Global", &mEfficiency_Global, 0.5, SFunctionFlag({ eFTypeNotAnd, { &mEfficiencyLHVbased} }), SFunctionFlag({ eFTypeNotAnd, { &mEfficiencyLHVbased} }), "Electrolyzer global efficiency - computed only with variable part of energy used ie UsedPower - StdByConsumption Over Produced H2 flowrate ", "", {}); /** Total constant Converter efficiency*/
        addParameter("MaxPower", &mMaxPower_H2, 0., true, true, "Electroysis system nominal power", "PowerUnit", {});
        addParameter("MinPower", &mMinPower_H2, 0., true, true, "Electroysis system minimum power multiplying coefficient in the range 0 to 1", "", {});	  /** Electroysis system minimum power coefficient in the range 0 to 1 */
        addParameter("AuxConso", &mAuxConso, 0., &mAddAuxConso, true, "Constant consumption in proportion of MaxPower", { "AddOperationConstraints" });
        addParameter("StdByConso", &mStdByConso, 0., &mAddStdByConso, true, "Constant consumption in proportion of MaxPower only when the electrolyzer state is on standby", { "AddOperationConstraints" });
        addParameter("Cost", &mCost, 0., false, true, "Cost per energy produced per hour (EUR/EnergyUnit)", "EUR/EnergyUnit"); /** Cost per energy produced per hour (EUR/EnergyUnit) */        
    }

 
    //----------------------------------------------------------------------------------------------------
    void declareModelIndicators() {
        ConverterSubModel::declareDefaultModelIndicators(&mExportIndicators);
    }

    void initDefaultPorts()
    {
        mDefaultPorts.clear();
        //PortUsedPower - left
        QMap<QString, QString> portUsedPower;
        portUsedPower["Name"] = "PortL0"; 
        portUsedPower["Position"] = "left";
        portUsedPower["CarrierType"] = Electrical();
        portUsedPower["Direction"] = KCONS();  
        portUsedPower["Variable"] = "UsedPower";
        mDefaultPorts.insert("PortUsedPower", portUsedPower);  

        //PortH2MassFlowRate - right
        QMap<QString, QString> portH2MassFlowRate;
        portH2MassFlowRate["Name"] = "PortR0";
        portH2MassFlowRate["Position"] = "right";
        portH2MassFlowRate["CarrierType"] = FluidH2();
        portH2MassFlowRate["Direction"] = KPROD();
        portH2MassFlowRate["Variable"] = "H2MassFlowRate";
        mDefaultPorts.insert("PortH2MassFlowRate", portH2MassFlowRate);
    }

    void setPortPointers() {
        mPortUsedPower = getPort("PortUsedPower");
        mPortH2MassFlowRate = getPort("PortH2MassFlowRate");
    }

    //----------------------------------------------------------------------------
protected:
    MilpPort* mPortUsedPower;
    MilpPort* mPortH2MassFlowRate;

    // MILP variables
    MIPModeler::MIPVariable0D mVarMinPower_H2;
    MIPModeler::MIPVariable0D mVarUsablePower;

    MIPModeler::MIPVariable1D mVarPower_H2;
    MIPModeler::MIPVariable1D mVarZ2; // variable for computation - No physical signifiaction
    MIPModeler::MIPVariable1D mVarFlow_H2;
    MIPModeler::MIPVariable1D mVarAuxConso;
    MIPModeler::MIPVariable1D mVarStdByConso;

    MIPModeler::MIPExpression mExpUsablePower;

    MIPModeler::MIPExpression1D mExpTotalPower;
    MIPModeler::MIPExpression1D mExpPower_H2;
    MIPModeler::MIPExpression1D mExpFlow_H2;
    MIPModeler::MIPExpression1D mExpCost;
    MIPModeler::MIPExpression1D mExpAuxConso;
    MIPModeler::MIPExpression1D mExpStdByConso;

    double mPci_H2;

    // model parameters
    double mEfficiency;
    double mEfficiency_Global;
    double mMaxPower_H2;
    double mMinPower_H2;
    double mMaxFlow_H2;
    double mMinFlow_H2;
    double mCost;
    double mAuxConso;
    double mStdByConso;

    bool mEfficiencyLHVbased; 
    bool mAddAuxConso;
    bool mAddStdByConso;
};

#endif
