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
        addParameter("UseAgeing", &mUseAgeing, false, false, true, "Use Ageing Model if true - default to false. Current Efficiency will be reduced by 1-EfficiencyAgeingCoeff*HistRunnningTime and reset to 1 when HistRunnningTime reaches EfficiencyMaxHours", "", "Ageing");  
    }
    
    void declareDefaultModelParameters()
    {
        TechnicalSubModel::declareDefaultModelParameters();
        if (mAgeingModel) { //&& mUseAgeing) { // Always declare even if UseAgeing is false
            mAgeingModel->declareModelParameters();
        }
    }

    void declareDefaultModelInterface()
    {
        TechnicalSubModel::declareDefaultModelInterface();
    }

    void declareDefaultModelIndicators(bool* exp)
    {
        TechnicalSubModel::declareDefaultModelIndicators();

        // ----------- Indicators specific for Converters -----------

        mInputIndicators->addIndicator("Installed Size", &mOptimalSize, exp, "Component size", pOptimalSizeUnit(), "Size");
        if (mWeight < 0) {
            mInputIndicators->addIndicator("Optimal Weight", &mWeightResult, exp, "Component optimal weight", "-", "Weight");
        }
        else {
            mInputIndicators->addIndicator("Weight", &mWeightResult, exp, "Component weight", "-", "Weight");
        }
        mInputIndicators->addIndicator("Running time at power >0.", &mRunningTime, exp, "Running time", "h", "RunningTime");
        mInputIndicators->addIndicator("Running time availability", &mRunningTimeAvlblt, exp, "Maximum possible running time", "-", "RunningTimeAvailable");
        if (mUseAgeing) {
            mInputIndicators->addIndicator("Efficiency after running time << ", &mEfficiency_Ageing, exp, "Efficiency after running time", "-","Efficiency");
        }

        for (const auto& port : mListPort) {
            if (!port->getCarrier())
                continue;
            const std::string portId = port->ID();
            const std::string varName = port->Variable();
            const MIPModeler::MIPExpression1D* ptrExp1D = getMIPExpression1D(port->Variable());
            if (ptrExp1D) {
                if (port->Direction() == GS::KPROD()) 
                {
                    mProductionMap.try_emplace(portId, 2, 0.0);
                    mProdLvlTotMap.try_emplace(portId, 2, 0.0);
                    mProdMeanMap.try_emplace(portId, 2, 0.0);
                    mProdContributionMap.try_emplace(portId, 2, 0.0);

                    mInputIndicators->addIndicator(
                        SExtFunctionName({ this, port, &indicatorName, { "Annual production of", STORAGE_NAME, VARIABLE } }),
                        &mProductionMap[portId], exp, "Annual production of "+varName, port->pStorageUnit(), 
                        SExtFunctionName({ this, port, &indicatorName, { "TotProd", VARIABLE } })
                        );

                    mInputIndicators->addIndicator(
                        SExtFunctionName({ this, port, &indicatorName, { "Mean production of", FLUX_NAME, VARIABLE } }),
                        &mProdMeanMap[portId], exp, "Mean production of " + varName, port->pFluxUnit(), 
                        SExtFunctionName({ this, port, &indicatorName, { "MeanProd", VARIABLE } })
                    );
                }
                else if (port->Direction() == GS::KCONS()) 
                {
                    mConsumptionMap.try_emplace(portId, 2, 0.0);
                    mConsLvlTotMap.try_emplace(portId, 2, 0.0);
                    mConsMeanMap.try_emplace(portId, 2, 0.0);
                    mConsPFMap.try_emplace(portId, 2, 0.0);
                    mRateOfUse.try_emplace(portId, 2, 0.0);

                    mInputIndicators->addIndicator(
                        SExtFunctionName({ this, port, &indicatorName, { "Annual consumption of", STORAGE_NAME, VARIABLE } }),
                        &mConsumptionMap[portId], exp, "Annual consumption of " + varName, port->pStorageUnit(), 
                        SExtFunctionName({ this, port, &indicatorName, { "TotCons", VARIABLE } })
                        );
                   
                    mInputIndicators->addIndicator(
                        SExtFunctionName({ this, port, &indicatorName, { "Mean consumption of", FLUX_NAME, VARIABLE } }),
                        &mConsMeanMap[portId], exp, "Mean consumption of " + varName, port->pFluxUnit(), 
                        SExtFunctionName({ this, port, &indicatorName, { "MeanCons", VARIABLE } })
                        );
                   
                    mInputIndicators->addIndicator(
                        SExtFunctionName({ this, port, &indicatorName, { "Load factor", VARIABLE } }),
                        &mRateOfUse[portId], exp, "Mean/Max", "-", 
                        SExtFunctionName({ this, port, &indicatorName, { "UseRate", VARIABLE } })
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

    void computeDefaultIndicators(const double* optSol);

    //used in MultiConverter and Cogeneration
    void cleanFluxIOs(const std::string& base);
    virtual void declareInputFluxIOs(MilpPort* defaultPort = nullptr);
    virtual void declareOutputFluxIOs(MilpPort* defaultPort = nullptr);
    
    inline std::string fluxName(const std::string& base, int index) const
    {
        return base + std::to_string(index + 1);
    }

    inline double EfficiencyAgeing() const {
        if (mAgeingModel && mUseAgeing) {
            const double efficiencyAgeing = mAgeingModel->EfficiencyAgeing();
            cInfo() << Name() + " efficiency ageing:" << efficiencyAgeing;
            return efficiencyAgeing;
        }
        else {
            return 1.0;
        }
    }
    inline double CapacityAgeing() const {
        if (mAgeingModel && mUseAgeing) {
            const double capacityAgeing = mAgeingModel->CapacityAgeing();
            cInfo() << Name() + " capcity ageing:" << capacityAgeing;
            return capacityAgeing;
        }
        else {
            return 1.0;
        }
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