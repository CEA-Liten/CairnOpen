#ifndef ResourceStock_H
#define ResourceStock_H

#include "globalModel.h"
#include "StorageSubModel.h"

/**
 * \details
* This component is made to represent the consumption of resources. That can typically be resources present in mines. 
* The cost of extraction will depend of the quantity "ResourceUsed" extracted. The more is extracted, the more it consumes energy.
* The costs and impact have to be filled as for the other component with tec eco model and environment model. Use piecewise functions to 
* represent advanced models of resource costs. 
*
*/
class MODELS_DECLSPEC ResourceStock : public StorageSubModel {

public:
    //----------------------------------------------------------------------------------------------------
    ResourceStock(CairnObject* aParent);
    ~ResourceStock();
    //----------------------------------------------------------------------------------------------------
    int checkConsistency();
    void setTimeData();
    //----------------------------------------------------------------------------------------------------
    void computeInitialData() override;
    void computeModelContribution() override;
    //----------------------------------------------------------------------------------------------------
    void computeEconomicalContribution();
    void computeAllIndicators(const double* optSol) override;


    void declareModelInterface()
    {
        StorageSubModel::declareDefaultModelInterface();

        // register model expression to be used for Interfacing with global MilpProblem, Bus, other MilpComponent
        // Caution : Flux will be signed wrt to Bus balance impact : >0 if energy source, <0 else.
        addSizeMaxIO("ResourceUsed", &mExpSizeMax, true, mPortFlux->pStorageUnit());	/** Storage balance of flows (including CapcityMuliplier) = Discharge Flow - Charge Flow, ie negative if charging, positive if discharging */
    }
    //----------------------------------------------------------------------------------------------------

    void declareModelConfigurationParameters()
    {
        StorageSubModel::declareDefaultModelConfigurationParameters();
  }

    void declareModelParameters()
    {
        StorageSubModel::declareDefaultModelParameters();
        //double
        addParameter("TotalMaxSize", &mTotalMaxSize, 1., false, true, "Nb of available stocks", "");
    }

    void declareModelIndicators()
    {
        StorageSubModel::declareDefaultModelIndicators(&mExportIndicators);
    }

    void initDefaultPorts() {
        mDefaultPorts.clear();
        //PortFlux - left
        std::map<std::string, std::string> portFlux;
        portFlux["Name"] = "PortL0";
        portFlux["Position"] = "top";
        portFlux["Direction"] = KDATA();
        portFlux["Variable"] = "ResourceUsed";
        mDefaultPorts["PortFlux"] = portFlux;

    }

    void setPortPointers() {
        mPortFlux = getPort("PortFlux");
    }

protected:
    MilpPort* mPortFlux;
    double mTotalMaxSize;
};
#endif
