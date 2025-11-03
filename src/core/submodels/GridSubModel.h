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

    virtual void setTimeData();
    virtual void computeInitialData();

    double Sens();

    void declareDefaultModelConfigurationParameters()
    {
        TechnicalSubModel::declareDefaultModelConfigurationParameters();
        //Re-declare parameter "EcoInvestModel" in order to set its default value to false in case of Grid
        //bool
        addParameter("EcoInvestModel", &mEcoInvestModel, false, false, true, "Use EcoInvestModel - ie Use Capex and Opex if true", "");    /** Use EcoInvestModel - ie Use Capex and Opex if true */
        addParameter("AddVariableMaxFlow", &mAddVariableMaxFlow, false, false, true, "If true: use time variable maximum flow limitation defined by <UseGridVariableMaxFlow> if true - Default is false");
    }
    
    void declareDefaultModelParameters()
    {
        TechnicalSubModel::declareDefaultModelParameters();
        //double 
        addParameter("MaxFlow", &mMaxFlux, 1.e4, true, true, "Maximum allowed extraction or injection", mMainCarrier->pFluxUnit());
        addParameter("MinFlow", &mMinFlux, 0., false, true, "Minimum allowed extraction or injection", mMainCarrier->pFluxUnit());

        addTimeSeries("UseProfileSellPrice", &mSellPrice, SExtFunctionFlag({ &isInjection, this }), SExtFunctionFlag({ &isInjection, this }), "Grid specific profile sell price overwriting EnergyVector default value or profile", SFunctionUnit({ eFTypeDivision, { pCurrency(), mMainCarrier->pStorageUnit()} }) );
        addTimeSeries("UseProfileBuyPrice", &mBuyPrice, SExtFunctionFlag({ &isExtraction, this }), SExtFunctionFlag({ &isExtraction, this }), "Grid specific Profile buy price overwriting EnergyVector default value or profile", SFunctionUnit({ eFTypeDivision, { pCurrency(), mMainCarrier->pStorageUnit()} }));
        addTimeSeries("UseProfileBuyPriceSeasonal", &mBuyPriceSeasonal, SFunctionFlag({ eFTypeNotAnd, {}, { &mSeasonalPrevisions}, SExtFunctionFlag({ &isExtraction, this }) }), SExtFunctionFlag({ &isExtraction, this }), "Time Series of Purchase 'Extraction' price of energy - See energy vector", SFunctionUnit({ eFTypeDivision, { &mCurrency, mMainCarrier->pStorageUnit()} }), "TimeSeriesForecast");
        addTimeSeries("UseVariableMaximumGridFlow", &mGridVariableMaxFlow, false, true, "Time Series of grid maximum flow extraction or injection", mMainCarrier->pFluxUnit());
    }

    void declareDefaultModelInterface()
    {
        /*
        * Grid is expected to have only one default port: the main carrier should be the EnergyVector of this port.
        * If it is not the case, then re-visit MilpComponent::defineMainCarrier() or the Grid Model in question.
        */
        assert(mPortGridFlow != nullptr);
        assert(mPortGridFlow->getCarrier() == mMainCarrier);

        TechnicalSubModel::declareDefaultModelInterface();

        /* Register IO expressions to be exported (published) as results (to the external, e.g., Pegase) */
        addSizeMaxIO("MaxFlow", &mExpSizeMax, true, mMainCarrier->pFluxUnit()); /** Sizing Grid flow - Name must be that used for mInputParam MaxFlow imposed value !*/
        addIO("GridFlow", &mExpFlux, true, mMainCarrier->pFluxUnit());       /** Grid flow injected or extracted - Positive value means extraction (injection) if ExtractFromGrid (InjectToGrid) field is used */
        addIO("GridPrice", &mExpGridPrice, true, SFunctionUnit({ eFTypeDivision, {mMainCarrier->pFluxUnit(), pCurrency()} }));
    }

    void declareDefaultModelIndicators(bool* exp)
    {
        TechnicalSubModel::declareDefaultModelIndicators();

        //Indicators specific for Grids
        std::string sens;
        if (Sens() > 0) sens = "extraction";
        else sens = "injection";
        mInputIndicators->addIndicator("Grid " + sens + " time", &mRunningTime, exp, "Number of hours of grid " + sens, "h", sens + " Time");

        for (auto &port : mListPort)        
        {            
            std::string portName = port->Name();
            std::string varName = port->Variable();
            std::string storageName = port->getCarrier()->StorageName();
            std::string fluxName = port->getCarrier()->FluxName();
            bool isHeatCarrier = false;
            if (port->getCarrier()) {
                isHeatCarrier = port->getCarrier()->isHeatCarrier();
            }

            if (port->VarType() == "vector")
            {
                if (isHeatCarrier)
                {
                    fluxName = fluxName + " at " + std::to_string(port->getCarrier()->Potential()) + " degC";
                    storageName = storageName + " at " + std::to_string(port->getCarrier()->Potential()) + " degC";
                }

                std::string identifier = "";

                if ((port->Direction() == GS::KPROD() && sens == "extraction"))
                {
                    mProductionMap[portName] = std::vector<double>(2, 0.);
                    mProdLvlTotMap[portName] = std::vector<double>(2, 0.);
                    mProdMeanMap[portName] = std::vector<double>(2, 0.);
                    if (!isIndicatorNameUnique(port, "StorageName")) identifier = "(" + port->Name() + ")";
                    mInputIndicators->addIndicator("Grid " + sens + " " + storageName + " " + varName + " " + identifier, &mProductionMap[portName], exp, "Grid " + sens + "", port->pStorageUnit(), "Tot" + varName);
                    mInputIndicators->addIndicator("Levelized Grid " + sens + " " + storageName + " " + varName + " " + identifier, &mProdLvlTotMap[portName], exp, "Levelized Grid " + sens + "", port->pStorageUnit(), "LvlzdTot" + varName);
                    if (isIndicatorNameUnique(port, "FluxName")) identifier = ""; //put back to empty if name is unique w.r.t fluxName (rarely  happens!)
                    mInputIndicators->addIndicator("Mean " + sens + " " + fluxName + " " + varName + " " + identifier, &mProdMeanMap[portName], exp, "Mean " + sens + "", port->pFluxUnit(), "Mean" + varName);
                }
                else if (port->Direction() == GS::KCONS() && sens == "injection")
                {
                    mConsumptionMap[portName] = std::vector<double>(2, 0.);
                    mConsLvlTotMap[portName] = std::vector<double>(2, 0.);
                    mConsMeanMap[portName] = std::vector<double>(2, 0.);
                    if (!isIndicatorNameUnique(port, "StorageName")) identifier = "(" + port->Name() + ")";
                    mInputIndicators->addIndicator("Grid " + sens + " " + storageName + " " + varName + " " + identifier, &mConsumptionMap[portName], exp, "Grid " + sens + "", port->pStorageUnit(), "Tot" + varName);
                    mInputIndicators->addIndicator("Levelized Grid " + sens + " " + storageName + " " + varName + " " + identifier, &mConsLvlTotMap[portName], exp, "Levelized Grid " + sens + "", port->pStorageUnit(), "LvlzdTot" + varName);
                    if (isIndicatorNameUnique(port, "FluxName")) identifier = ""; //put back to empty if name is unique w.r.t fluxName (rarely  happens!)
                    mInputIndicators->addIndicator("Mean " + sens + " " + fluxName + " " + varName + " " + identifier, &mConsMeanMap[portName], exp, "Mean " + sens + "", port->pFluxUnit(), "Mean" + varName);
                }
                else if (port->Direction() == GS::KDATA())
                {
                    if (!isIndicatorNameUnique(port)) identifier = "(" + port->Name() + ")";
                    mExpEchData[portName] = std::vector<double>(2, 0.);
                    mInputIndicators->addIndicator("Data Port published " + varName + " - data computed " + identifier, &mExpEchData[portName], exp, "Data port", port->pStorageUnit(), "DataPort" + varName);
                }
            }
        }
    }

    virtual void computeAllIndicators(const double* optSol);

protected:
    MilpPort* mPortGridFlow; /* The default port of a Grid component which is used to set its direction (InjectToGrid or ExtractFromGrid) */

    //MILP Variable
    MIPModeler::MIPVariable1D mVarFluxGrid;

    //technical output
    MIPModeler::MIPExpression1D mExpFlux;
    MIPModeler::MIPExpression1D mExpGridPrice;

    //technical input
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