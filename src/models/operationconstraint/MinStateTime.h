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
    MinStateTime(QObject* aParent);
    ~MinStateTime();
//----------------------------------------------------------------------------------------------------
    void setTimeData();
    int checkConsistency();
//----------------------------------------------------------------------------------------------------
    void declareModelConfigurationParameters()
    {
        OperationSubModel::declareDefaultModelConfigurationParameters() ;
        mInputParam->addToConfigList({ "EcoInvestModel","EnvironmentModel" });
        //bool
        addParameter("AddMinProductionTime", &mAddMinProductionTime, false, false, true, "If true: imposes a minimum production time", "bool", { "" });
        addParameter("AddMinStandByTime", &mAddMinStandbyTime, false, false, true, "If true : imposes a minimum stand by time", "bool", { "" });
        addParameter("AddStartUpCost", &mAddStartUpCost, false, false, true, "If true: add start up cost", "", { "" });                            
        addParameter("AddShutDownCost", &mAddShutDownCost, false, false, true, "If true: add shut down cost", "", { "" });

    }
//----------------------------------------------------------------------------------------------------
    void declareModelParameters()
    {
        OperationSubModel::declareDefaultModelParameters();
        //double
        addParameter("MinProductionTime", &mMinProductionTime, 0., false, true, "Minimal number of hours to keep the production on", "hour", { "" });
        addParameter("MinStandByTime", &mMinStandbyTime, 0., false, true, "Minimal number of hours to stay in stand by", "hour", { "" });
        addParameter("StartUpCost", &mStartUpCost, 0., &mAddStartUpCost, &mAddStartUpCost, "Cost paid when moving from \"off\" to \"on\" status while cold", "Currency", { "" });
        addParameter("ShutDownCost", &mShutDownCost, 0., &mAddShutDownCost, &mAddShutDownCost, "Cost paid when moving from \"on\" to \"off\" status", "Currency", { "" });

    }
//----------------------------------------------------------------------------------------------------
    void declareModelInterface()
    {
        declareDefaultModelInterface();
        addIO("State", &mExpState, "bool") ;      /** ON OFF state of the element connected to ramp */
        addControlIO("StartUp", &mExpStartUp, "bool", &mHistStartUp);
        addControlIO("ShutDown", &mExpShutDown, "bool", &mHistShutDown);        
    }

    void declareModelIndicators() {
        OperationSubModel::declareDefaultModelIndicators();

        QString currency = mParentCompo->Currency();
        mInputIndicators->addIndicator("Undiscounted number of startups", &mNbStartUps, &mExportIndicators, "Total nb of startups (undiscounted)", "-", "NbStartUps");
        mInputIndicators->addIndicator("Undiscounted number of shutdowns", &mNbShutDowns, &mExportIndicators, "Total nb of shutdowns (undiscounted)", "-", "NbShutDowns");
        mInputIndicators->addIndicator("Levelized cost of startups", &mLevStartUpsCost, &mExportIndicators, "Levelized cost of startups", currency, "LevStartUpsCost");
        mInputIndicators->addIndicator("Levelized cost of shutdowns", &mLevShutDownsCost, &mExportIndicators, "Levelized cost of shutdowns", currency, "LevShutDownsCost");
    }

    void computeAllIndicators(const double* optSol) override;
//----------------------------------------------------------------------------------------------------
    void setParameters(double aMinConstraintBusValue, double aMaxConstraintBusValue, double aStrictConstraintBusValue) ;
    void addMinProductionTime();
    void addMinStandByTime();
    
 //----------------------------------------------------------------------------------------------------
    void buildModel();                                                              // build minimum formulation
    void computeAllContribution();
//----------------------------------------------------------------------------------------------------


    void initDefaultPorts() {
        mDefaultPorts.clear();
        //PortState - bottom
        QMap<QString, QString> portState;
        portState["Name"] = "PortB0";
        portState["Position"] = "bottom";
        portState["CarrierType"] = ANY_TYPE();
        portState["Direction"] = KDATA();
        portState["Variable"] = "State";
        mDefaultPorts.insert("PortState", portState);
    }

protected:
    std::vector<double> mConverterUse;      /** time series for converter use availability */

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

    MIPModeler::MIPData1D mHistStartUp;
    MIPModeler::MIPData1D mHistShutDown;

    MIPModeler::MIPExpression1D mExpStartUpCosts;
    MIPModeler::MIPExpression1D mExpShutDownCosts;
};

#endif // MinStateTime_H
