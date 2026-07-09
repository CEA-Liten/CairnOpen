#ifndef GridSubModel_H
#define GridSubModel_H

#include "TechnicalSubModel.h"
extern bool CAIRNCORESHARED_EXPORT isExtraction(class SubModel* ap_Model);
extern bool CAIRNCORESHARED_EXPORT isInjection(class SubModel* ap_Model);

class CAIRNCORESHARED_EXPORT GridSubModel : public TechnicalSubModel
{
public:
    GridSubModel(CairnObject* aParent=nullptr);
    ~GridSubModel();

    double Sens() override;
    std::string Direction();

    virtual void setTimeData();
    virtual void computeInitialData();
    virtual void computeAllIndicators(const double* optSol);

    void defineMainCarrier() override; 

    void declareDefaultModelConfigurationParameters() {
        TechnicalSubModel::declareDefaultModelConfigurationParameters();

        //Re-declare parameter "EcoInvestModel" in order to set its default value to false in case of Grid
        addParameter("EcoInvestModel", &mEcoInvestModel, false, false, true, "Use EcoInvestModel - ie Use Capex and Opex if true", ""); 
        addParameter("AddVariableMaxFlow", &mAddVariableMaxFlow, false, false, true, "If true: use time variable maximum flow limitation defined by <UseGridVariableMaxFlow> if true - Default is false");
    }
    
    void declareDefaultModelParameters() {
        TechnicalSubModel::declareDefaultModelParameters();

        addParameter("MaxFlow", &mMaxFlux, 1.e4, true, true, "Maximum allowed extraction or injection", mPortGridFlow->pFluxUnit());
        addParameter("MinFlow", &mMinFlux, 0., false, true, "Minimum allowed extraction or injection", mPortGridFlow->pFluxUnit());

        addTimeSeries("UseProfileSellPrice", &mSellPrice, SExtFunctionFlag({ &isInjection, this }), SExtFunctionFlag({ &isInjection, this }), "Grid specific profile sell price overwriting EnergyVector default value or profile", SFunctionUnit({ eFTypeDivision, { pCurrency(), mPortGridFlow->pStorageUnit()} }) );
        addTimeSeries("UseProfileBuyPrice", &mBuyPrice, SExtFunctionFlag({ &isExtraction, this }), SExtFunctionFlag({ &isExtraction, this }), "Grid specific Profile buy price overwriting EnergyVector default value or profile", SFunctionUnit({ eFTypeDivision, { pCurrency(), mPortGridFlow->pStorageUnit()} }));
        addTimeSeries("UseProfileBuyPriceSeasonal", &mBuyPriceSeasonal, SFunctionFlag({ eFTypeNotAnd, {}, { &mSeasonalPrevisions}, SExtFunctionFlag({ &isExtraction, this }) }), SExtFunctionFlag({ &isExtraction, this }), "Time Series of Purchase 'Extraction' price of energy - See energy vector", SFunctionUnit({ eFTypeDivision, { pCurrency(), mPortGridFlow->pStorageUnit()} }), "TimeSeriesForecast");
        addTimeSeries("UseVariableMaximumGridFlow", &mGridVariableMaxFlow, false, true, "Time Series of grid maximum flow extraction or injection", mPortGridFlow->pFluxUnit());
    }

    void declareDefaultModelInterface() {

        if (!mPortGridFlow) {
            throw Cairn_Exception(Name() + ": the default Flow port of the Grid is not defined!", -1); 
        }

        TechnicalSubModel::declareDefaultModelInterface();

        /* Register IO expressions to be exported (published) as results (to the external, e.g., Pegase) */
        addSizeMaxIO("MaxFlow", &mExpSizeMax, true, mPortGridFlow->pFluxUnit()); /** Sizing Grid flow - Name must be that used for mInputParam MaxFlow imposed value !*/
        addIO("GridFlow", &mExpFlux, true, mPortGridFlow->pFluxUnit());       /** Grid flow injected or extracted - Positive value means extraction (injection) if ExtractFromGrid (InjectToGrid) field is used */
        addIO("GridPrice", &mExpGridPrice, true, SFunctionUnit({ eFTypeDivision, { mPortGridFlow->pFluxUnit(), pCurrency()} }));
    }

    void declareDefaultModelIndicators(bool* exp) {
        TechnicalSubModel::declareDefaultModelIndicators();

        // ----------- Indicators specific for Grids  -----------

        mInputIndicators->addIndicator(
            SExtFunctionName({ this, nullptr, &indicatorName, { "Grid", DIRECTION, "time" } }),  //port is not needed => nullptr
            &mRunningTime, exp, "Number of hours of grid " + Direction(), "h", 
            SExtFunctionName({ this, nullptr, &indicatorName, { DIRECTION, "Time" } })
        );

        for (const auto &port : mListPort) {
            if (!port->getCarrier())
                continue;
            const std::string portId = port->ID();
            const MIPModeler::MIPExpression1D* ptrExp1D = getMIPExpression1D(port->Variable());
            if (ptrExp1D) {
                /* Note, Sens() is that of the default port not that of $port */
                if ((port->Direction() == GS::KPROD() && Sens() > 0)) 
                {
                    mProductionMap.try_emplace(portId, 2, 0.0);
                    mProdLvlTotMap.try_emplace(portId, 2, 0.0);
                    mProdMeanMap.try_emplace(portId, 2, 0.0);

                    mInputIndicators->addIndicator(
                        SExtFunctionName({ this, port, &indicatorName, { "Grid", DIRECTION, STORAGE_NAME, VARIABLE } }),
                        &mProductionMap[portId], exp, "Grid " + Direction() + "", port->pStorageUnit(),
                        SExtFunctionName({ this, port, &indicatorName, { "Tot", VARIABLE } })
                    );
                    
                    mInputIndicators->addIndicator(
                        SExtFunctionName({ this, port, &indicatorName, { "Levelized Grid", DIRECTION, STORAGE_NAME, VARIABLE } }), 
                        &mProdLvlTotMap[portId], exp, "Levelized Grid " + Direction() + "", port->pStorageUnit(),
                        SExtFunctionName({ this, port, &indicatorName, { "LvlzdTot", VARIABLE } })
                    );

                    mInputIndicators->addIndicator(
                        SExtFunctionName({ this, port, &indicatorName, { "Mean", DIRECTION, FLUX_NAME, VARIABLE } }),
                        &mProdMeanMap[portId], exp, "Mean " + Direction() + "", port->pFluxUnit(),
                        SExtFunctionName({ this, port, &indicatorName, { "Mean", VARIABLE } })
                    );
                }
                else if (port->Direction() == GS::KCONS() && Sens() <= 0)
                {
                    mConsumptionMap.try_emplace(portId, 2, 0.0);
                    mConsLvlTotMap.try_emplace(portId, 2, 0.0);
                    mConsMeanMap.try_emplace(portId, 2, 0.0);
  
                    mInputIndicators->addIndicator(
                        SExtFunctionName({ this, port, &indicatorName, { "Grid", DIRECTION, STORAGE_NAME, VARIABLE } }),
                        &mConsumptionMap[portId], exp, "Grid " + Direction() + "", port->pStorageUnit(),
                        SExtFunctionName({ this, port, &indicatorName, { "Tot", VARIABLE } })
                    );
                    
                    mInputIndicators->addIndicator(
                        SExtFunctionName({ this, port, &indicatorName, { "Levelized Grid ", DIRECTION, STORAGE_NAME, VARIABLE } }),
                        &mConsLvlTotMap[portId], exp, "Levelized Grid " + Direction() + "", port->pStorageUnit(),
                        SExtFunctionName({ this, port, &indicatorName, { "LvlzdTot", VARIABLE } })
                    );
                    
                    mInputIndicators->addIndicator(
                        SExtFunctionName({ this, port, &indicatorName, { "Mean", DIRECTION, FLUX_NAME, VARIABLE } }),
                        &mConsMeanMap[portId], exp, "Mean " + Direction() + "", port->pFluxUnit(),
                        SExtFunctionName({ this, port, &indicatorName, { "Mean", VARIABLE } })
                    );
                }
                else if (port->Direction() == GS::KDATA())
                {
                    mExpEchData.try_emplace(portId, 2, 0.0);

                    mInputIndicators->addIndicator(
                        SExtFunctionName({ this, port, &indicatorName, { "Data Port published", VARIABLE, "- data computed" } }),
                        &mExpEchData[portId], exp, "Data port", port->pStorageUnit(),
                        SExtFunctionName({ this, port, &indicatorName, { "DataPort", VARIABLE } })
                    );
                }
            }
        }
    }

protected:
    // The default port of a Grid component which is used to set its direction (injection or extraction) 
    MilpPort* mPortGridFlow; 

    // MILP Variables
    MIPModeler::MIPVariable1D mVarFluxGrid;

    // Output MIP Expressions 
    MIPModeler::MIPExpression1D mExpFlux;
    MIPModeler::MIPExpression1D mExpGridPrice;

    // Input Parameters 
    bool mAddVariableMaxFlow;
    double mMaxFlux;
    double mMinFlux;

    std::vector<double> mEnergyPrice; //EnergyPrice equal to SellPrice or BuyPrice based on Sens 
    std::vector<double> mSellPrice;
    std::vector<double> mBuyPrice;
    std::vector<double> mBuyPriceSeasonal;
    std::vector<double> mGridVariableMaxFlow;
};

#endif // GridSubModel_H 