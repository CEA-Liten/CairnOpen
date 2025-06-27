#ifndef GridSubModel_H
#define GridSubModel_H

#include "TechnicalSubModel.h"
extern bool CAIRNCORESHARED_EXPORT isExtraction(class SubModel* ap_Model);
extern bool CAIRNCORESHARED_EXPORT isInjection(class SubModel* ap_Model);

class CAIRNCORESHARED_EXPORT GridSubModel : public TechnicalSubModel
{
public:
    GridSubModel(QObject* aParent=nullptr);
    ~GridSubModel();
    //-----------------------------------------------------------------------------------------------------
    virtual void setTimeData();

    void declareDefaultModelConfigurationParameters()
    {
        TechnicalSubModel::declareDefaultModelConfigurationParameters();
        //Re-declare parameter "EcoInvestModel" in order to set its default value to false in case of Grid
        //bool
        addParameter("EcoInvestModel", &mEcoInvestModel, false, false, true, "Use EcoInvestModel - ie Use Capex and Opex if true", "", { "" });    /** Use EcoInvestModel - ie Use Capex and Opex if true */
        addParameter("AddVariableMaxFlow", &mAddVariableMaxFlow, false, false, true, "If true: use time variable maximum flow limitation defined by <UseGridVariableMaxFlow> if true - Default is false");

        //double
        addConfig("Direction", &mSens, 1., true, true, "Injection / Extraction direction");
    }
    
    void declareDefaultModelParameters()
    {
        TechnicalSubModel::declareDefaultModelParameters();
        //double 
        addParameter("MaxFlow", &mMaxFlux, 1.e4, true, true, "Maximum allowed extraction or injection", "FluxUnit");
        addParameter("MinFlow", &mMinFlux, 0., false, true, "Minimum allowed extraction or injection", "FluxUnit");

        addTimeSeries("UseProfileSellPrice", &mSellPrice, SExtFunctionFlag({ &isInjection, this }), SExtFunctionFlag({ &isInjection, this }), "Grid specific profile sell price overwriting EnergyVector default value or profile", "Currency/StorageUnit");
        addTimeSeries("UseProfileBuyPrice", &mBuyPrice, SExtFunctionFlag({ &isExtraction, this }), SExtFunctionFlag({ &isExtraction, this }), "Grid specific Profile buy price overwriting EnergyVector default value or profile", "Currency/StorageUnit");
        addTimeSeries("UseProfileBuyPriceSeasonal", &mBuyPriceSeasonal, SFunctionFlag({ eFTypeNotAnd, {}, { &mSeasonalPrevisions}, SExtFunctionFlag({ &isExtraction, this }) }), SExtFunctionFlag({ &isExtraction, this }), "Time Series of Purchase (Extraction) price of energy - See energy vector", "Currency/StorageUnit", { "TimeSeriesForecast" });
        addTimeSeries("UseProfileGrid", &mGridUse, false, true, "Time Series of grid extraction allowance if 1 forbidden if 0 - Use to prevent extraction on basis of carbon content knowledge or grid unavailability");
        addTimeSeries("UseVariableMaximumGridFlow", &mGridVariableMaxFlow, false, true, "Time Series of grid maximum flow extraction or injection", "FluxUnit");

    }

    void declareDefaultModelInterface()
    {
        TechnicalSubModel::declareDefaultModelInterface();
        // register expression for model Interface with global MilpProblem, Bus, other MilpComponent
        addIO("GridFlow", &mExpFlux, mEnergyVector->pFluxUnit());       /** Grid flow injected or extracted - Positive value means extraction (injection) if ExtractFromGrid (InjectToGrid) field is used */
        addIO("MaxFlow", &mExpSizeMax, mEnergyVector->pFluxUnit()); /** Sizing Grid flow - Name must be that used for mInputParam MaxFlow imposed value !*/
        addIO("GridPrice", &mExpGridPrice, mEnergyVector->pFluxUnit(), mCurrency);

        setOptimalSizeExpression("MaxFlow");  // defines default expression should be used for OptimalSize computation and use in Economic analysis
        setOptimalSizeUnit(mEnergyVector->pFluxUnit());  // defines default expression should be used for OptimalSize computation and use in Economic analysis
    }

    void declareDefaultModelIndicators(bool* exp)
    {
        TechnicalSubModel::declareDefaultModelIndicators();

        //Indicators specific for Grids
        QString sens;
        if (mParentCompo->Sens() > 0) sens = "extraction";
        else sens = "injection";
        mInputIndicators->addIndicator("Grid " + sens + "  time", &mRunningTime, exp, "Number of hours of grid " + sens, "h", sens + " Time");

        MilpPort* port;
        QListIterator<MilpPort*> iport(mListPort);
        while (iport.hasNext())
        {
            port = iport.next();
            QString portName = port->Name();
            QString varName = port->Variable();
            QString storageName = port->ptrEnergyVector()->StorageName();
            QString storageUnit = port->ptrEnergyVector()->StorageUnit();
            QString fluxUnit = port->ptrEnergyVector()->FluxUnit();
            QString fluxName = port->ptrEnergyVector()->FluxName();
            bool isHeatCarrier = false;
            if (port->ptrEnergyVector()) {
                isHeatCarrier = port->ptrEnergyVector()->isHeatCarrier();
            }

            if (port->VarType() == "vector")
            {
                if (isHeatCarrier)
                {
                    fluxName = fluxName + " at " + QString::number(port->ptrEnergyVector()->Potential()) + " degC";
                    storageName = storageName + " at " + QString::number(port->ptrEnergyVector()->Potential()) + " degC";
                }

                QString identifier = "";

                if ((port->Direction() == GS::KPROD() && sens == "extraction"))
                {
                    mProductionMap[portName] = std::vector<double>(2, 0.);
                    mProdLvlTotMap[portName] = std::vector<double>(2, 0.);
                    mProdMeanMap[portName] = std::vector<double>(2, 0.);
                    if (!isIndicatorNameUnique(port, "StorageName")) identifier = "(" + port->Name() + ")";
                    mInputIndicators->addIndicator("Grid " + sens + " " + storageName + " " + varName + " " + identifier, &mProductionMap[portName], exp, "Grid " + sens + "", storageUnit, "Tot" + varName);
                    mInputIndicators->addIndicator("Levelized Grid " + sens + " " + storageName + " " + varName + " " + identifier, &mProdLvlTotMap[portName], exp, "Levelized Grid " + sens + "", storageUnit, "LvlzdTot" + varName);
                    if (isIndicatorNameUnique(port, "FluxName")) identifier = ""; //put back to empty if name is unique w.r.t fluxName (rarely  happens!)
                    mInputIndicators->addIndicator("Mean " + sens + " " + fluxName + " " + varName + " " + identifier, &mProdMeanMap[portName], exp, "Mean " + sens + "", fluxUnit, "Mean" + varName);
                }
                else if (port->Direction() == GS::KCONS() && sens == "injection")
                {
                    mConsumptionMap[portName] = std::vector<double>(2, 0.);
                    mConsLvlTotMap[portName] = std::vector<double>(2, 0.);
                    mConsMeanMap[portName] = std::vector<double>(2, 0.);
                    if (!isIndicatorNameUnique(port, "StorageName")) identifier = "(" + port->Name() + ")";
                    mInputIndicators->addIndicator("Grid " + sens + " " + storageName + " " + varName + " " + identifier, &mConsumptionMap[portName], exp, "Grid " + sens + "", storageUnit, "Tot" + varName);
                    mInputIndicators->addIndicator("Levelized Grid " + sens + " " + storageName + " " + varName + " " + identifier, &mConsLvlTotMap[portName], exp, "Levelized Grid " + sens + "", storageUnit, "LvlzdTot" + varName);
                    if (isIndicatorNameUnique(port, "FluxName")) identifier = ""; //put back to empty if name is unique w.r.t fluxName (rarely  happens!)
                    mInputIndicators->addIndicator("Mean " + sens + " " + fluxName + " " + varName + " " + identifier, &mConsMeanMap[portName], exp, "Mean " + sens + "", fluxUnit, "Mean" + varName);
                }
                else if (port->Direction() == GS::KDATA())
                {
                    if (!isIndicatorNameUnique(port)) identifier = "(" + port->Name() + ")";
                    mExpEchData[portName] = std::vector<double>(2, 0.);
                    mInputIndicators->addIndicator("Data Port published " + varName + " - data computed " + identifier, &mExpEchData[portName], exp, "Data port", storageUnit, "DataPort" + varName);
                }
            }
        }
    }

    virtual void computeAllIndicators(const double* optSol);
    double getSens() const { return mSens; }

protected:
    virtual int checkBusFlowBalanceVarName(MilpPort* port, int& inumberchange, QString& varUseCheck);

    //MILP Variable
    MIPModeler::MIPVariable1D mVarFluxGrid;

  //technical output
    MIPModeler::MIPExpression1D mExpFlux;
    MIPModeler::MIPExpression1D mExpGridPrice;

    //technical input
    double mSens;
    bool mAddVariableMaxFlow;
    double mMaxFlux;
    double mMinFlux;

    std::vector<double> mEnergyPrice; //EnergyPrice equal to SellPrice or BuyPrice based on mSens  
    std::vector<double> mSellPrice;
    std::vector<double> mBuyPrice;
    std::vector<double> mBuyPriceSeasonal;
    std::vector<double> mGridUse;
    std::vector<double> mGridVariableMaxFlow;
};

#endif // GridSubModel_H