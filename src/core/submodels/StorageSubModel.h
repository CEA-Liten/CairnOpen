#ifndef StorageSubModel_H
#define StorageSubModel_H

#include "TechnicalSubModel.h"

class CAIRNCORESHARED_EXPORT StorageSubModel : public TechnicalSubModel
{
public:
    StorageSubModel(CairnObject* aParent=nullptr);
    ~StorageSubModel();
    virtual void setTimeData();

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
        TechnicalSubModel::declareDefaultModelInterface();
    }

    void declareDefaultModelIndicators(bool* exp)
    {
        TechnicalSubModel::declareDefaultModelIndicators();

        // ----------- Indicators specific for Storages -----------

        mInputIndicators->addIndicator("Storage Capacity", &mOptimalSize, exp, "Component size", pOptimalSizeUnit(), "Capacity");
        mInputIndicators->addIndicator("Charging time", &mChargingTime, exp, "Charging time", "h", "ChargingTime");
        mInputIndicators->addIndicator("Discharging time ", &mDischargingTime, exp, "Discharging time", "h", "DischargingTime");

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
                        &mChargedEnergyMap[portId], exp, "Charged", port->pStorageUnit(),
                        SExtFunctionName({ this, port, &indicatorName, { "TotCharged", VARIABLE } })
                    );
                   
                    mInputIndicators->addIndicator(
                        SExtFunctionName({ this, port, &indicatorName, { "Discharged", STORAGE_NAME, VARIABLE } }),
                        &mDischargedEnergyMap[portId], exp, "Discharged", port->pStorageUnit(),
                        SExtFunctionName({ this, port, &indicatorName, { "TotDischarged", VARIABLE } })
                    );
                   
                    mInputIndicators->addIndicator(
                        SExtFunctionName({ this, port, &indicatorName, { "Levelized Charged", STORAGE_NAME, VARIABLE } }),
                        &mNLevChargedEnergyMap[portId], exp, "Levelized Charged", port->pStorageUnit(),
                        SExtFunctionName({ this, port, &indicatorName, { "LvlzdTotCharged", VARIABLE } })
                    );
                   
                    mInputIndicators->addIndicator(
                        SExtFunctionName({ this, port, &indicatorName, { "Levelized Discharged", STORAGE_NAME, VARIABLE } }),
                        &mNLevDischargedEnergyMap[portId], exp, "Levelized Discharged", port->pStorageUnit(),
                        SExtFunctionName({ this, port, &indicatorName, { "LvlzdDischarged", VARIABLE } })
                    );
                    
                    mInputIndicators->addIndicator(
                        SExtFunctionName({ this, port, &indicatorName, { "Mean Charged", FLUX_NAME, VARIABLE } }),
                        &mChargedMeanMap[portId], exp, "Mean Charged", port->pFluxUnit(),
                        SExtFunctionName({ this, port, &indicatorName, { "MeanCharged", VARIABLE } })
                    );
                    
                    mInputIndicators->addIndicator(
                        SExtFunctionName({ this, port, &indicatorName, { "Mean Discharged", FLUX_NAME, VARIABLE } }),
                        &mDischargedMeanMap[portId], exp, "Mean Discharged", port->pFluxUnit(),
                        SExtFunctionName({ this, port, &indicatorName, { "MeanDischarged", VARIABLE } })
                    );
                   
                    mInputIndicators->addIndicator(
                        SExtFunctionName({ this, port, &indicatorName, { "Equivalent number of cycles", FLUX_NAME, VARIABLE } }),
                        &mNbCylesMap[portId], exp, "Number of cycles", "-",
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
                        &mConsumptionMap[portId], exp, "CONS Tot", port->pStorageUnit(), 
                        SExtFunctionName({ this, port, &indicatorName, { "TotCons", VARIABLE } })
                    );
                  
                    mInputIndicators->addIndicator(
                        SExtFunctionName({ this, port, &indicatorName, { "CONS Levelized Total", STORAGE_NAME, VARIABLE } }),
                        &mConsLvlTotMap[portId], exp, "CONS Levelized Tot", port->pStorageUnit(), 
                        SExtFunctionName({ this, port, &indicatorName, { "LvlzdTotCons", VARIABLE } })
                    );

                    mInputIndicators->addIndicator(
                        SExtFunctionName({ this, port, &indicatorName, { "CONS Mean Total", FLUX_NAME, VARIABLE } }),
                        &mConsMeanMap[portId], exp, "CONS Mean Tot", port->pFluxUnit(), 
                        SExtFunctionName({ this, port, &indicatorName, { "MeanCons", VARIABLE } })
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