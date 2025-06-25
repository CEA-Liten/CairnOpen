#ifndef GridFree_H
#define GridFree_H

#include "globalModel.h"
#include "GridSubModel.h"
/**
 * \details

This submodel represents a grid.
Either it is an extraction grid either it is an injection grid.
The quantity transported depends on the connected energy vector.

 .. figure:: ../images/GridFree.svg
   :alt: IO Grid Free
   :name: IOGridFree
   :width: 200
   :align: center

   I/O Grid Free

The optimization variable is the maximum flow.

The buy price (resp. the sell price) is that of the energy vector by default but it can be overwritten in the model by a constant value or a timeseries.
*/


class MODELS_DECLSPEC GridFree : public GridSubModel {
public:
//----------------------------------------------------------------------------------------------------
    GridFree(QObject* aParent);
    ~GridFree();
//-----------------------------------------------------------------------------------------------------
    void setTimeData() ;
//----------------------------------------------------------------------------------------------------
    void buildModel();
//----------------------------------------------------------------------------------------------------
    void computeEconomicalContribution();
//----------------------------------------------------------------------------------------------------
    void declareModelConfigurationParameters() 
    {
        GridSubModel::declareDefaultModelConfigurationParameters() ;
        mInputParam->addToConfigList({ "EcoInvestModel","EnvironmentModel","TimeSeriesForecast","ReactivePowerModel" });
        //bool
        addParameter("UseConstantPrice", &mUseConstantPrice, false, false, true, "If true: override default price in energy carrier and temporal price in timeseries by ConstantBuyPrice and ConstantSellPrice parameters");
        addParameter("SeasonalCosts", &mSeasonalCosts, false, false, true, "", "", { "TimeSeriesForecast" }); //To be documented
        addParameter("SeasonalCostsFree", &mSeasonalCostsFree, false, false, true, "", "", { "TimeSeriesForecast" }); //To be documented       
    }
    //----------------------------------------------------------------------------------------------------
    void declareModelInterface() {
        GridSubModel::declareDefaultModelInterface();
    }

//----------------------------------------------------------------------------------------------------
    void declareModelParameters() {
        GridSubModel::declareDefaultModelParameters();

    
        addParameter("ConstantBuyPrice", &mConstantBuyPrice, 0., &mUseConstantPrice, SFunctionFlag({ eFTypeNotAnd, {}, { &mUseConstantPrice}, SExtFunctionFlag({ &isExtraction, this }) }), "Constant buy price to override default price in energy carrier and temporal price in timeseries. Only used if UseConstantPrice is True.", "Currency/StorageUnit");
        addParameter("ConstantSellPrice", &mConstantSellPrice, 0., &mUseConstantPrice, SFunctionFlag({ eFTypeNotAnd, {}, { &mUseConstantPrice}, SExtFunctionFlag({ &isInjection, this }) }), "Constant sell price to override default price in energy carrier and temporal price in timeseries. Only used if UseConstantPrice is True.", "Currency/StorageUnit");
        addParameter("PriceMultiplier", &mPriceMultiplier, 1., false, true, "Multiplier coefficient on price acting on constant prices or timeseries.", "-");            
    }

    void declareModelIndicators() {
        GridSubModel::declareDefaultModelIndicators(&mExportIndicators);
    }
//----------------------------------------------------------------------------------------------------

    void initDefaultPorts() {
        mDefaultPorts.clear();
        //PortGridFlow - right
        QMap<QString, QString> portGridFlow;
        portGridFlow["Name"] = "PortR0"; //Needed only old versions
        portGridFlow["Position"] = "right";
        portGridFlow["CarrierType"] = ANY_TYPE();
        portGridFlow["Direction"] = KPROD(); //OUTPUT but should be able to be changed
        portGridFlow["Variable"] = "GridFlow";
        mDefaultPorts.insert("PortGridFlow", portGridFlow);  //ID, paramMap
    }

protected:     
    //technical input
    bool mSeasonalCosts ;
    bool mSeasonalCostsFree;
    bool mUseConstantPrice;

    
    double mConstantBuyPrice;  // constant buy price to override carrier default price or grid price timeseries
    double mConstantSellPrice; // constant sell price to override carrier default price or grid price timeseries
    double mPriceMultiplier;

    std::vector<double> mTemporalPrice;
};

#endif

