#ifndef ResourceStock_H
#define ResourceStock_H

#include "globalModel.h"
#include "StorageSubModel.h"
//linkPerseeModelClass ResourceStock.h StorageLinearBounds.h StorageThermal.h Battery_V1.h StorageSeasonal.h 

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
    ResourceStock(QObject* aParent);
    ~ResourceStock();
    //----------------------------------------------------------------------------------------------------
    int checkConsistency();
    void setTimeData();
    void computeInitialData();
    //----------------------------------------------------------------------------------------------------
    void buildModel();
    //----------------------------------------------------------------------------------------------------
    void computeEconomicalContribution();
    void computeAllIndicators(const double* optSol) override;


    void declareModelInterface()
    {
        StorageSubModel::declareDefaultModelInterface();
        //StorageSubModel::declareDefaultModelInterface();
        // register model expression to be used for Interfacing with global MilpProblem, Bus, other MilpComponent
        // Caution : Flux will be signed wrt to Bus balance impact : >0 if energy source, <0 else.
        addIO("ResourceUsed", &mExpSizeMax, mPortFlux->pStorageUnit());	/** Storage balance of flows (including CapcityMuliplier) = Discharge Flow - Charge Flow, ie negative if charging, positive if discharging */
        //addIO("ResourceTotalCost", &mExpResourceCost, mPortFlux->pStorageUnit());	/** Storage discharged flow (including CapcityMuliplier) */
    
        setOptimalSizeExpression("ResourceUsed");  // defines default expression should be used for OptimalSize computation and use in Economic analysis
        setOptimalSizeUnit(mPortFlux->pStorageUnit());
    }
    //----------------------------------------------------------------------------------------------------

    void declareModelConfigurationParameters()
    {
        StorageSubModel::declareDefaultModelConfigurationParameters();
        //bool
        mInputParam->addToConfigList({ "EcoInvestModel","EnvironmentModel"});
        
  }

    void declareModelParameters()
    {
        StorageSubModel::declareDefaultModelParameters();
        //double
        //addParameter("TotalMaxSize", &mTotalMaxSize, 0., true, true, "total size of the stock", "", {""});
        addParameter("TotalMaxSize", &mTotalMaxSize, 1., false, true, "Nb of available stocks", "", { "" });
    }

    void declareModelIndicators()
    {
        StorageSubModel::declareDefaultModelIndicators(&mExportIndicators);
        QString InstalledSizeUnit = getOptimalSizeUnit();
    }


    void initDefaultPorts() {
        mDefaultPorts.clear();
        //PortElecPower - left
        QMap<QString, QString> portFlux;
        portFlux["Name"] = "PortL0";
        portFlux["Direction"] = KDATA();
        portFlux["Variable"] = "ResourceUsed";
        mDefaultPorts.insert("PortFlux", portFlux);

    }

    void setPortPointers() {
        mPortFlux = getPort("PortFlux");
    }

    //----------------------------------------------------------------------------------------------------
        //void setEnvParameters(double aCO2emission, std::vector <double> aCO2PriceProfile) ;
protected:

    MilpPort* mPortFlux;

    // MILP Variables

    // output for the resource stock

    double mTotalMaxSize;



};
#endif
