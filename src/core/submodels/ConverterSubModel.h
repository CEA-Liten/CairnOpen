#ifndef ConverterSubModel_H
#define ConverterSubModel_H

#include "TechnicalSubModel.h"
#include "AgeingRunningHours.h"

class CAIRNCORESHARED_EXPORT ConverterSubModel : public TechnicalSubModel
{
public:
    ConverterSubModel(CairnObject* aParent=nullptr);
    ~ConverterSubModel();
    
    void declareInputParams(const std::string& name);

    void computeAgeingModelContribution() override;

    void setTimeData();

    void setMinPower(MIPModeler::MIPExpression1D aPower, std::vector<double> aMinPowList, double aNomPower);
    void setMinPower(MIPModeler::MIPExpression1D aPower, double aMinPow, double aNomPower);
    void setMinPower(MIPModeler::MIPExpression1D aPower, MIPModeler::MIPVariable1D aZ, std::vector<double> aMinPowList, double aNomPower);

    void declareDefaultModelConfigurationParameters()
    {
        TechnicalSubModel::declareDefaultModelConfigurationParameters();
        addParameter("UseAgeing", &mUseAgeing, false, false, true, "", "", "Ageing");          /** Use UseAgeing Model if true - default to false. Current Efficiency will be reduced by 1-EfficiencyAgeingCoeff*HistRunnningTime and reset to 1 when HistRunnningTime reaches EfficiencyMaxHours */
    }
    
    void declareDefaultModelParameters()
    {
        TechnicalSubModel::declareDefaultModelParameters();
        if (mAgeingModel && mUseAgeing) mAgeingModel->declareModelParameters();
    }

    void declareDefaultModelInterface()
    {
        TechnicalSubModel::declareDefaultModelInterface();
    }

    void declareDefaultModelIndicators(bool* exp)
    {
        TechnicalSubModel::declareDefaultModelIndicators();

        //Indicators specific for Converters
        mInputIndicators->addIndicator("Installed Size", &mOptimalSize, exp, "Component size", pOptimalSizeUnit(), "Size");

        mInputIndicators->addIndicator("Running time at power >0.", &mRunningTime, exp, "Running time", "h", "RunningTime");
        mInputIndicators->addIndicator("Running time availability", &mRunningTimeAvlblt, exp, "Maximum possible running time", "-", "RunningTimeAvailable");
        if (mUseAgeing) {
            mInputIndicators->addIndicator("Efficiency after running time << ", &mEfficiency_Ageing, exp, "Efficiency after running time", "-","Efficiency");
        }

        for (auto& port : mListPort) {
            std::string portName = port->Name();
            std::string varName = port->Variable();
            std::string storageName = port->getCarrier()->StorageName();
            std::string fluxName = port->getCarrier()->FluxName();
            bool isHeatCarrier = port->getCarrier()->isHeatCarrier();

            if (port->VarType() == "vector" )
            {
                std::string identifier = "";

                if (port->Direction() == GS::KPROD()) {
                    mProductionMap[portName] = std::vector<double>(2, 0.);
                    mProdLvlTotMap[portName] = std::vector<double>(2, 0.);
                    mProdMeanMap[portName] = std::vector<double>(2, 0.);
                    mProdContributionMap[portName] = std::vector<double>(2, 0.);
                    if (!isIndicatorNameUnique(port, "StorageName")) identifier = "(" + port->Name() + ")";
                    mInputIndicators->addIndicator("Annual production of " + storageName + " " + varName + " " + identifier, &mProductionMap[portName], exp, "Annual production of "+varName, port->pStorageUnit(), "TotProd" + varName + identifier);
                    if (isIndicatorNameUnique(port, "FluxName")) identifier = ""; //put back to empty if name is unique w.r.t fluxName (rarely  happens!)
                    mInputIndicators->addIndicator("Mean production of " + fluxName + " " + varName + " " + identifier, &mProdMeanMap[portName], exp, "Mean production of " + varName, port->pFluxUnit(), "MeanProd" + varName + identifier);
                }
                else if (port->Direction() == GS::KCONS()) {
                    mConsumptionMap[portName] = std::vector<double>(2, 0.);
                    mConsLvlTotMap[portName] = std::vector<double>(2, 0.);
                    mConsMeanMap[portName] = std::vector<double>(2, 0.);
                    mConsPFMap[portName] = std::vector<double>(2, 0.);
                    mRateOfUse[portName] = std::vector<double>(2, 0.);
                    if (!isIndicatorNameUnique(port, "StorageName")) identifier = "(" + port->Name() + ")";
                    mInputIndicators->addIndicator("Annual consumption of " + storageName + " " + varName + " " + identifier, &mConsumptionMap[portName], exp, "Annual consumption of " + varName, port->pStorageUnit(), "TotCons" + varName+ identifier);
                   if (isIndicatorNameUnique(port, "FluxName")) identifier = ""; //put back to empty if name is unique w.r.t fluxName (rarely  happens!)
                    mInputIndicators->addIndicator("Mean consumption of " + fluxName + " " + varName + " " + identifier, &mConsMeanMap[portName], exp, "Mean consumption of " + varName, port->pFluxUnit(), "MeanCons" + varName + identifier);
                    if (!isIndicatorNameUnique(port)) identifier = "(" + port->Name() + ")";
                    mInputIndicators->addIndicator("Load factor " + varName + " " + identifier, &mRateOfUse[portName], exp, "Mean/Max", "-", "UseRate"+varName+identifier);
                }
                else if (port->Direction() == GS::KDATA()) {
                    if (!isIndicatorNameUnique(port)) identifier = "(" + port->Name() + ")";
                    mExpEchData[portName] = std::vector<double>(2, 0.);
                    mInputIndicators->addIndicator("Data Port published " + varName + " - data computed " + identifier, &mExpEchData[portName], exp, "Data port", port->pStorageUnit(), "DataPort"+varName+identifier);
                }
            }
        }
    }

    void computeDefaultIndicators(const double* optSol);

    //used in MultiConverter and Cogeneration
    void cleanFluxIOs(std::string name); 
    virtual void declareInputFluxIOs(MilpPort* defaultPort = nullptr);
    virtual void declareOutputFluxIOs(MilpPort* defaultPort = nullptr);
    
    double Efficiency() const { 
        if (mAgeingModel && mUseAgeing)
            return mAgeingModel->EfficiencyAgeing();
        else
            return 1;
    }
    double CapacityAgeing() const {
        if (mAgeingModel && mUseAgeing)
            return mAgeingModel->CapacityAgeing();
        else
            return 1;
    }
protected:
    
    MIPModeler::MIPVariable1D mZ;

    //used in MultiConverter and Cogeneration
    std::vector <MIPModeler::MIPExpression1D> mExpInput;
    std::vector <MIPModeler::MIPExpression1D> mExpOutput;

    bool mUseAgeing;          /** bool indicating the use of an ageing model if true - default to false */
    AgeingRunningHours* mAgeingModel ;
};

#endif // ConverterSubModel_H