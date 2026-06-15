/* --------------------------------------------------------------------------
 * File: MinStateTime.cpp
 * version 1.0
 * Author: Pimprenelle Parmentier
 * Date 09/03/2022
 *---------------------------------------------------------------------------
 * Description: Model imposing a minimal time to stay on or off .
 * --------------------------------------------------------------------------
 */

#ifndef MinStateTime_H
#define MinStateTime_H

#include "globalModel.h"

#include "MIPModeler.h"
#include "OperationSubModel.h"

/**
* \details

 Model imposing a minimal time to stay on or off.

 State must be connected to the state of the component on which the operation constraint must be applied.

 Startups and shutdowns costs can be considered.

 .. figure:: ../images/MinStateTime.svg
   :alt: IO MinStateTime
   :name: IOMinStateTime
   :width: 200
   :align: center

   I/O MinStateTime
 
 .. caution::

    Do not forget to add a minimum power on the linked component to enable state.
 
 .. caution::
    
    Adding this model can increase the time of resolution.
  
*/
class MODELS_DECLSPEC MinStateTime : public OperationSubModel
{
public:
//----------------------------------------------------------------------------------------------------
    MinStateTime(CairnObject* aParent);
    ~MinStateTime();
//----------------------------------------------------------------------------------------------------
    void computeInitialData() override;
    void computeModelContribution() override;
    void setTimeData();
    int checkConsistency() override;
//----------------------------------------------------------------------------------------------------
    void declareModelConfigurationParameters()
    {
        OperationSubModel::declareDefaultModelConfigurationParameters() ;
        //bool
        addParameter("AddMinProductionTime", &mAddMinProductionTime, false, false, true, "If true: imposes a minimum production time", "bool");
        addParameter("AddMinStandByTime", &mAddMinStandbyTime, false, false, true, "If true : imposes a minimum stand by time", "bool");
        addParameter("AddStartUpCost", &mAddStartUpCost, false, false, true, "If true: add start up cost", "");                            
        addParameter("AddShutDownCost", &mAddShutDownCost, false, false, true, "If true: add shut down cost", "");

    }
//----------------------------------------------------------------------------------------------------
    void declareModelParameters()
    {
        OperationSubModel::declareDefaultModelParameters();
        //double
        addParameter("MinProductionTime", &mMinProductionTime, 0., false, true, "Minimal number of hours to keep the production on", "hour");
        addParameter("MinStandByTime", &mMinStandbyTime, 0., false, true, "Minimal number of hours to stay in stand by", "hour");
        addParameter("StartUpCost", &mStartUpCost, 0., &mAddStartUpCost, &mAddStartUpCost, "Cost paid when moving from \"off\" to \"on\" status while cold", pCurrency());
        addParameter("ShutDownCost", &mShutDownCost, 0., &mAddShutDownCost, &mAddShutDownCost, "Cost paid when moving from \"on\" to \"off\" status", pCurrency());

    }
//----------------------------------------------------------------------------------------------------
    void declareModelInterface()
    {
        declareDefaultModelInterface();

        /* Register IO expressions to be exported (published) as results (to the external, e.g., Pegase) */
        //...

        /* Register non-IO 0D-expressions in order to automatically allocate and close them */
        // no ...

        /* Register non-IO 1D-expressions in order to automatically allocate and close them */
        addExp(&mExpStartUpCosts, &mHorizon);
        addExp(&mExpShutDownCosts, &mHorizon);
    }

    void declareModelIndicators() {
        OperationSubModel::declareDefaultModelIndicators();

        mInputIndicators->addIndicator("Undiscounted number of startups", &mNbStartUps, &mExportIndicators, "Total nb of startups (undiscounted)", "-", "NbStartUps");
        mInputIndicators->addIndicator("Undiscounted number of shutdowns", &mNbShutDowns, &mExportIndicators, "Total nb of shutdowns (undiscounted)", "-", "NbShutDowns");
        mInputIndicators->addIndicator("Levelized cost of startups", &mLevStartUpsCost, &mExportIndicators, "Levelized cost of startups", pCurrency(), "LevStartUpsCost");
        mInputIndicators->addIndicator("Levelized cost of shutdowns", &mLevShutDownsCost, &mExportIndicators, "Levelized cost of shutdowns", pCurrency(), "LevShutDownsCost");
    }

    void computeAllIndicators(const double* optSol) override;
//----------------------------------------------------------------------------------------------------
    void setParameters(double aMinConstraintBusValue, double aMaxConstraintBusValue, double aStrictConstraintBusValue) ;
    void addMinProductionTime();
    void addMinStandByTime();
   
    void initDefaultPorts() {
        mDefaultPorts.clear();
        //PortState - bottom
        std::map<std::string, std::string> portState;
        portState["Name"] = "PortB0";
        portState["Position"] = "bottom";
        portState["CarrierType"] = ANY_TYPE();
        portState["Direction"] = KDATA();
        portState["Variable"] = "State";
        mDefaultPorts["PortState"] = portState;
    }

protected:
    //indicators
    std::vector<double> mNbStartUps;
    std::vector<double> mNbShutDowns;
    std::vector<double> mLevStartUpsCost;
    std::vector<double> mLevShutDownsCost;

    bool mAddMinProductionTime;
    bool mAddMinStandbyTime;
    bool mAddStartUpCost;
    bool mAddShutDownCost;

    double mMinStandbyTime;
    double mMinProductionTime;
    double mStartUpCost;
    double mShutDownCost;

    MIPModeler::MIPExpression1D mExpStartUpCosts;
    MIPModeler::MIPExpression1D mExpShutDownCosts;
};

#endif // MinStateTime_H
