#ifndef StorageSubModel_H
#define StorageSubModel_H

#include "TechnicalSubModel.h"

class CAIRNCORESHARED_EXPORT StorageSubModel : public TechnicalSubModel
{
public:
    StorageSubModel(CairnObject* aParent);
    ~StorageSubModel();
    virtual void setTimeData();

    void declareModelConfigurationParameters() override
    {
        TechnicalSubModel::declareModelConfigurationParameters();
    }
    
    void declareModelParameters() override
    {
        TechnicalSubModel::declareModelParameters();
    }

    void declareModelInterface() override
    {
        TechnicalSubModel::declareModelInterface();
    }

    void declareModelIndicators() override
    {
        TechnicalSubModel::declareModelIndicators();

        // ----------- Indicators specific for Storages -----------

        mInputIndicators->addIndicator("Storage Capacity", &mOptimalSize, &mExportIndicators, "Component size", pOptimalSizeUnit(), "Capacity");
        mInputIndicators->addIndicator("Charging time", &mChargingTime, &mExportIndicators, "Charging time", "h", "ChargingTime");
        mInputIndicators->addIndicator("Discharging time ", &mDischargingTime, &mExportIndicators, "Discharging time", "h", "DischargingTime");

        for (const auto& port : mListPort) {
            if (!port->getCarrier())
                continue;
            const std::string portId = port->ID();
            const MIPModeler::MIPExpression1D* ptrExp1D = getMIPExpression1D(port->Variable());
            if (ptrExp1D) {
                if (port->Direction() == GS::KPROD())
                {
                    mChargedEnergyMap.try_emplace(portId, 2, 0.0);
                    mDischargedEnergyMap.try_emplace(portId, 2, 0.0);
                    mNLevChargedEnergyMap.try_emplace(portId, 2, 0.0);
                    mNLevDischargedEnergyMap.try_emplace(portId, 2, 0.0);
                    mNbCylesMap.try_emplace(portId, 2, 0.0);
                    mChargedMeanMap.try_emplace(portId, 2, 0.0);
                    mDischargedMeanMap.try_emplace(portId, 2, 0.0);

                    mInputIndicators->addIndicator(
                        SExtFunctionName({ this, port, &indicatorName, { "Charged", STORAGE_NAME, VARIABLE } }),
                        &mChargedEnergyMap[portId], &mExportIndicators, "Charged", port->pStorageUnit(),
                        SExtFunctionName({ this, port, &indicatorName, { "TotCharged", VARIABLE } })
                    );
                   
                    mInputIndicators->addIndicator(
                        SExtFunctionName({ this, port, &indicatorName, { "Discharged", STORAGE_NAME, VARIABLE } }),
                        &mDischargedEnergyMap[portId], &mExportIndicators, "Discharged", port->pStorageUnit(),
                        SExtFunctionName({ this, port, &indicatorName, { "TotDischarged", VARIABLE } })
                    );
                   
                    mInputIndicators->addIndicator(
                        SExtFunctionName({ this, port, &indicatorName, { "Levelized Charged", STORAGE_NAME, VARIABLE } }),
                        &mNLevChargedEnergyMap[portId], &mExportIndicators, "Levelized Charged", port->pStorageUnit(),
                        SExtFunctionName({ this, port, &indicatorName, { "LvlzdTotCharged", VARIABLE } })
                    );
                   
                    mInputIndicators->addIndicator(
                        SExtFunctionName({ this, port, &indicatorName, { "Levelized Discharged", STORAGE_NAME, VARIABLE } }),
                        &mNLevDischargedEnergyMap[portId], &mExportIndicators, "Levelized Discharged", port->pStorageUnit(),
                        SExtFunctionName({ this, port, &indicatorName, { "LvlzdDischarged", VARIABLE } })
                    );
                    
                    mInputIndicators->addIndicator(
                        SExtFunctionName({ this, port, &indicatorName, { "Mean Charged", FLUX_NAME, VARIABLE } }),
                        &mChargedMeanMap[portId], &mExportIndicators, "Mean Charged", port->pFluxUnit(),
                        SExtFunctionName({ this, port, &indicatorName, { "MeanCharged", VARIABLE } })
                    );
                    
                    mInputIndicators->addIndicator(
                        SExtFunctionName({ this, port, &indicatorName, { "Mean Discharged", FLUX_NAME, VARIABLE } }),
                        &mDischargedMeanMap[portId], &mExportIndicators, "Mean Discharged", port->pFluxUnit(),
                        SExtFunctionName({ this, port, &indicatorName, { "MeanDischarged", VARIABLE } })
                    );
                   
                    mInputIndicators->addIndicator(
                        SExtFunctionName({ this, port, &indicatorName, { "Equivalent number of cycles", FLUX_NAME, VARIABLE } }),
                        &mNbCylesMap[portId], &mExportIndicators, "Number of cycles", "-",
                        "NbCycles"
                    );
                }
                else if (port->Direction() == GS::KCONS())
                {
                    mConsumptionMap.try_emplace(portId, 2, 0.0);
                    mConsLvlTotMap.try_emplace(portId, 2, 0.0);
                    mConsMeanMap.try_emplace(portId, 2, 0.0);

                    mInputIndicators->addIndicator(
                        SExtFunctionName({ this, port, &indicatorName, { "CONS Total", STORAGE_NAME, VARIABLE } }),
                        &mConsumptionMap[portId], &mExportIndicators, "CONS Tot", port->pStorageUnit(), 
                        SExtFunctionName({ this, port, &indicatorName, { "TotCons", VARIABLE } })
                    );
                  
                    mInputIndicators->addIndicator(
                        SExtFunctionName({ this, port, &indicatorName, { "CONS Levelized Total", STORAGE_NAME, VARIABLE } }),
                        &mConsLvlTotMap[portId], &mExportIndicators, "CONS Levelized Tot", port->pStorageUnit(), 
                        SExtFunctionName({ this, port, &indicatorName, { "LvlzdTotCons", VARIABLE } })
                    );

                    mInputIndicators->addIndicator(
                        SExtFunctionName({ this, port, &indicatorName, { "CONS Mean Total", FLUX_NAME, VARIABLE } }),
                        &mConsMeanMap[portId], &mExportIndicators, "CONS Mean Tot", port->pFluxUnit(), 
                        SExtFunctionName({ this, port, &indicatorName, { "MeanCons", VARIABLE } })
                    );
                }
                else if (port->Direction() == GS::KDATA())
                {
                    mExpEchData.try_emplace(portId, 2, 0.0);

                    mInputIndicators->addIndicator(
                        SExtFunctionName({ this, port, &indicatorName, { "Data Port published", VARIABLE, "- data computed" } }),
                        &mExpEchData[portId], &mExportIndicators, "Data port", port->pStorageUnit(), 
                        SExtFunctionName({ this, port, &indicatorName, { "DataPort", VARIABLE } })
                    );
                }
            }
        }
    }

    void computeAllIndicators(const double* optSol) override;

    virtual void initDefaultPorts() {
        mDefaultPorts.clear();
        //PortFlow - left
        std::map<std::string, std::string> portFlow;
        portFlow["Name"] = "PortL0";
        portFlow["Position"] = "left";
        portFlow["CarrierType"] = ANY_TYPE();
        portFlow["Direction"] = KPROD();
        portFlow["Variable"] = "Flow";
        mDefaultPorts["PortFlow"] = portFlow;
    }
    
protected:
};

#endif // StorageSubModel_H