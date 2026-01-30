#ifndef SourceLoadSubModel_H
#define SourceLoadSubModel_H

#include "TechnicalSubModel.h"

class CAIRNCORESHARED_EXPORT SourceLoadSubModel : public TechnicalSubModel
{
public:
    SourceLoadSubModel(CairnObject* aParent=nullptr);
    ~SourceLoadSubModel();
    virtual void setTimeData();

    double Sens() override;
    std::string Direction();

    void declareDefaultModelConfigurationParameters()
    {
        TechnicalSubModel::declareDefaultModelConfigurationParameters();
    }
    
    void declareDefaultModelParameters()
    {
        TechnicalSubModel::declareDefaultModelParameters();
    }

    void declareDefaultModelInterface()
    {
        /*
        * SourceLoad is expected to have only one default port: the main carrier should be the EnergyVector of this port.
        * If it is not the case, then re-visit MilpComponent::defineMainCarrier() or the SourceLoad Model in question.
        */
        assert(mSourceLoadDefaultPort != nullptr);
        assert(mSourceLoadDefaultPort->getCarrier() == mMainCarrier);

        TechnicalSubModel::declareDefaultModelInterface();
    }

    void declareDefaultModelIndicators(bool* exp)
    {
        TechnicalSubModel::declareDefaultModelIndicators();

        // ----------- Indicators specific for SourceLoads -----------

        mInputIndicators->addIndicator("Component Weight", &mOptimalSize, exp, "Component size", pOptimalSizeUnit(), "Weight");
        if (isPriceOptimized()) {
            mInputIndicators->addIndicator("Component Optimal Price", &mOptimalSize, exp, "Component Optimal Price", pOptimalSizeUnit(), "OptPrice");
        }
        mInputIndicators->addIndicator("ImposedProfile " + Direction() + " time", &mRunningTime, exp, "Running time", "h", "ImposedProfileTime");

        for (const auto& port : mListPort) {
            const std::string portId = port->ID();
            const std::string varName = port->Variable();
            const std::string storageName = port->getCarrier()->StorageName();
            const std::string fluxName = port->getCarrier()->FluxName();

            // Why "sens" is not included in the indicator's name like in the case of Grid ?

            const MIPModeler::MIPExpression1D* ptrExp1D = getMIPExpression1D(port->Variable());
            if (ptrExp1D) {
                /* Note, Sens() is that of the default port not that of $port */
                if (port->Direction() == GS::KPROD() && Sens() > 0) 
                { 
                    mProductionMap.try_emplace(portId, 2, 0.0);
                    mProdLvlTotMap.try_emplace(portId, 2, 0.0);
                    mProdMeanMap.try_emplace(portId, 2, 0.0);

                    mInputIndicators->addIndicator(
                        SExtFunctionName({ this, port, &indicatorName, { "ImposedProfile", STORAGE_NAME, VARIABLE } }),
                        &mProductionMap[portId], exp, "ImposedProfile " + storageName + " " + varName, port->pStorageUnit(), 
                        SExtFunctionName({ this, port, &indicatorName, { "TotImposedProfile", VARIABLE } })
                    );
                   
                    mInputIndicators->addIndicator(
                        SExtFunctionName({ this, port, &indicatorName, { "Levelized ImposedProfile", STORAGE_NAME, VARIABLE } }),
                        &mProdLvlTotMap[portId], exp, "Levelized ImposedProfile " + storageName + " " + varName, port->pStorageUnit(), 
                        SExtFunctionName({ this, port, &indicatorName, { "LvlzdTotImposedProfile", VARIABLE } })
                    );

                    mInputIndicators->addIndicator(
                        SExtFunctionName({ this, port, &indicatorName, { "Mean", FLUX_NAME, VARIABLE } }),
                        &mProdMeanMap[portId], exp, "Mean " + fluxName + " " + varName, port->pFluxUnit(), 
                        SExtFunctionName({ this, port, &indicatorName, { "MeanImposedProfile", VARIABLE } })
                    );
                }
                else if (port->Direction() == GS::KCONS() && Sens() <= 0)  
                {
                    mConsumptionMap.try_emplace(portId, 2, 0.0);
                    mConsLvlTotMap.try_emplace(portId, 2, 0.0);
                    mConsMeanMap.try_emplace(portId, 2, 0.0);

                    mInputIndicators->addIndicator(
                        SExtFunctionName({ this, port, &indicatorName, { "ImposedProfile", STORAGE_NAME, VARIABLE } }),
                        &mConsumptionMap[portId], exp, "ImposedProfile " + storageName + " " + varName, port->pStorageUnit(), 
                        SExtFunctionName({ this, port, &indicatorName, { "TotImposedProfile", VARIABLE } })
                        );

                    mInputIndicators->addIndicator(
                        SExtFunctionName({ this, port, &indicatorName, { "Levelized ImposedProfile", STORAGE_NAME, VARIABLE } }),
                        &mConsLvlTotMap[portId], exp, "Levelized ImposedProfile " + storageName + " " + varName, port->pStorageUnit(), 
                        SExtFunctionName({ this, port, &indicatorName, { "LvlzdTotImposedProfile", VARIABLE } })
                        );
                   
                    mInputIndicators->addIndicator(
                        SExtFunctionName({ this, port, &indicatorName, { "Mean", FLUX_NAME, VARIABLE } }),
                        &mConsMeanMap[portId], exp, "Mean " + fluxName + " " + varName, port->pFluxUnit(), 
                        SExtFunctionName({ this, port, &indicatorName, { "MeanImposedProfile", VARIABLE } })
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
    
    void declareSourceENRModelIndicators(bool* exp)
    {
        TechnicalSubModel::declareDefaultModelIndicators();

        // OptimalSizeUnit is the default in case no output port found which would be strange !!
        mInputIndicators->addIndicator("Component Weight", &mOptimalSize, exp, "Component size", pOptimalSizeUnit(), "Weight");

        for (auto& port : mListPort) {
            const std::string portId = port->ID();
            const MIPModeler::MIPExpression1D* ptrExp1D = getMIPExpression1D(port->Variable());
            if (ptrExp1D) {
                if (port->Direction() == GS::KPROD())
                {
                    mProductionMap.try_emplace(portId, 2, 0.0);
                    mProdLvlTotMap.try_emplace(portId, 2, 0.0);
                    mProdMeanMap.try_emplace(portId, 2, 0.0);

                    mInputIndicators->addIndicator(
                        SExtFunctionName({ this, port, &indicatorName, { "ENR injection time"} }),
                        &mRunningTime, exp, "Running time", "h", 
                        SExtFunctionName({ this, port, &indicatorName, { "ENRInjectionTime"} })
                    );
                  
                    mInputIndicators->addIndicator(
                        SExtFunctionName({ this, port, &indicatorName, { "ENR injection", STORAGE_NAME, VARIABLE } }),
                        &mProductionMap[portId], exp, "", port->pStorageUnit(),
                        SExtFunctionName({ this, port, &indicatorName, { "Tot", VARIABLE } })
                    );
                   
                    mInputIndicators->addIndicator(
                        SExtFunctionName({ this, port, &indicatorName, { "Levelized ENR injection", STORAGE_NAME, VARIABLE } }),
                        &mProdLvlTotMap[portId], exp, "", port->pStorageUnit(),
                        SExtFunctionName({ this, port, &indicatorName, { "LvlzdTot", VARIABLE } })
                    );
                   
                    mInputIndicators->addIndicator(
                        SExtFunctionName({ this, port, &indicatorName, { "Mean", FLUX_NAME, VARIABLE } }),
                        &mProdMeanMap[portId], exp, "Mean", port->pFluxUnit(),
                        SExtFunctionName({ this, port, &indicatorName, { "Mean", VARIABLE } })
                    );
                }
                else if (port->Direction() == GS::KDATA())
                {
                    mExpEchData.try_emplace(portId, 2, 0.0);

                    mInputIndicators->addIndicator(
                        SExtFunctionName({ this, port, &indicatorName, { "Data Port published", VARIABLE, "- data computed"} }),
                        &mExpEchData[portId], exp, "Data port", port->pStorageUnit(),
                        SExtFunctionName({ this, port, &indicatorName, { "DataPort", VARIABLE } })
                    );
                }
            }
        }
    }

    void computeSourceENRModelIndicators(const double* optSol); 
    void computeDefaultIndicators(const double* optSol);

    //----------------------------------------------------------------------------------------------------

protected:
    MilpPort* mSourceLoadDefaultPort; /* The default port (Flow or Phi) of a SourceLoad component which is used to set its direction (Source or Load) */
};

#endif // SourceLoadSubModel_H