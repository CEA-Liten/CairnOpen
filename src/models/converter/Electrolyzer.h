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
    Electrolyzer(CairnObject* aParent);
    ~Electrolyzer();
 
    //----------------------------------------------------------------------------------------------------
    virtual void computeInitialData();
    void computeEconomicalContribution();
    void computeModelContribution() override;
    int checkConsistency();
    //----------------------------------------------------------------------------------------------------
    void computeAllIndicators(const double* optSol) override;

    //----------------------------------------------------------------------------------------------------
    void declareModelInterface() override
    {
        ConverterSubModel::declareModelInterface();

        /* Register IO expressions to be exported (published) as results (to the external, e.g., Pegase) */
        addSizeMaxIO("MaxPower", &mExpSizeMax, true, mPortUsedPower->pFluxUnit());				    /** Computed sizing electrolysis system power */
        addIO("MaxUsablePower", &mExpUsablePower, true, mPortUsedPower->pFluxUnit());        	/** Computed allowed power available to electrolysis system */
        addIO("UsedPower", &mExpTotalPower, true, mPortUsedPower->pFluxUnit());					/** Computed electrolysis system power */
        addIO("H2MassFlowRate", &mExpFlow_H2, true, mPortH2MassFlowRate->pFluxUnit());  /** Computed electrolysis H2 flowrate production */

        /* Register non-IO 0D-expressions in order to automatically allocate and close them */
        // no 0D expression needs to be declared here

        /* Register non-IO 1D-expressions in order to automatically allocate and close them */
        addExp(&mExpPower_H2, &mHorizon);
        addExp(&mExpAuxConso, &mHorizon);
        addExp(&mExpStdByConso, &mHorizon);
    }
    //----------------------------------------------------------------------------------------------------
    void declareModelConfigurationParameters() override {
        ConverterSubModel::declareModelConfigurationParameters();
        //bool 
        addConfigParameter("EfficiencyLHVbased", &mEfficiencyLHVbased, true, false, true, "efficiency type for electrolyzer", "bool"); /** TODO: Add a control imposed (to test different controls with the same parameters) */
        addConfigParameter("AddAuxConso", &mAddAuxConso, false, false, true, "Constant elec consumption in proportion of maxpower: ", "bool", "AddOperationConstraints");
        addConfigParameter("AddStdByConso", &mAddStdByConso, false, false, true, "Constant elec consumption in proportion of maxpower: equals to 0 if converterUse =0 but not when state = 0", "bool", "AddOperationConstraints");
    }
    
    void declareModelParameters() override
    {
        ConverterSubModel::declareModelParameters();
        // Supported types are: double, int, std::vector<double> or std::vector<int>
        // InputParam instance for input data coming from User File : maximum power, performance maps...
        
        //Re-declare LifeTime and change default value
        addParameter("LifeTime", &mLifeTime, 10., false, SFunctionFlag({ eFTypeOrNot, { &mEcoInvestModel, &mEnvironmentModel} }), "LifeTime in years", "Year", "EcoInvestModel");   

        //double
        addParameter("Efficiency", &mEfficiency, 0.6, &mEfficiencyLHVbased, &mEfficiencyLHVbased, "Electrolyzer efficiency LHV based - computed only with variable part of energy used ie UsedPower - StdByConsumption Over Produced H2 flowrate");  
        addParameter("Efficiency_Global", &mEfficiency_Global, 0.5, SFunctionFlag({ eFTypeNotAnd, { &mEfficiencyLHVbased} }), SFunctionFlag({ eFTypeNotAnd, { &mEfficiencyLHVbased} }), "Electrolyzer global efficiency - computed only with variable part of energy used ie UsedPower - StdByConsumption Over Produced H2 flowrate");  
        addParameter("MaxPower", &mMaxPower_H2, 0., true, true, "Electroysis system nominal power", mMainCarrier->pPowerUnit());
        addParameter("MinPower", &mMinPower_H2, 0., true, true, "Electroysis system minimum power multiplying coefficient in the range 0 to 1");	  
        addParameter("AuxConso", &mAuxConso, 0., &mAddAuxConso, true, "Constant consumption in proportion of MaxPower", "", "AddOperationConstraints");
        addParameter("StdByConso", &mStdByConso, 0., &mAddStdByConso, true, "Constant consumption in proportion of MaxPower only when the electrolyzer state is on standby", "", "AddOperationConstraints");
        addParameter("Cost", &mCost, 0., false, true, "Cost per energy produced per hour", SFunctionUnit({ eFTypeDivision, { pCurrency(), pQuantity("EnergyUnit")}}));
    }

 
    //----------------------------------------------------------------------------------------------------
    void declareModelIndicators() override {
        ConverterSubModel::declareModelIndicators();
    }

    void initDefaultPorts() override
    {
        mDefaultPorts.clear();
        //PortUsedPower - left
        std::map<std::string, std::string> portUsedPower;
        portUsedPower["Name"] = "PortL0"; 
        portUsedPower["Position"] = "left";
        portUsedPower["CarrierType"] = Electrical();
        portUsedPower["Direction"] = KCONS();  
        portUsedPower["Variable"] = "UsedPower";
        mDefaultPorts["PortUsedPower"] = portUsedPower;  

        //PortH2MassFlowRate - right
        std::map<std::string, std::string> portH2MassFlowRate;
        portH2MassFlowRate["Name"] = "PortR0";
        portH2MassFlowRate["Position"] = "right";
        portH2MassFlowRate["CarrierType"] = FluidH2();
        portH2MassFlowRate["Direction"] = KPROD();
        portH2MassFlowRate["Variable"] = "H2MassFlowRate";
        mDefaultPorts["PortH2MassFlowRate"] = portH2MassFlowRate;
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
