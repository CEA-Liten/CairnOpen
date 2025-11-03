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

        //Indicators specific for Storages
        mInputIndicators->addIndicator("Storage Capacity", &mOptimalSize, exp, "Component size", pOptimalSizeUnit(), "Capacity");

        bool once = true;

        for (auto& port : mListPort) {        
            std::string portName = port->Name();
            std::string varName = port->Variable();
            std::string storageName = port->getCarrier()->StorageName();
            std::string fluxName = port->getCarrier()->FluxName();
            bool isHeatCarrier = false;
            if (port->getCarrier()) {
                isHeatCarrier = port->getCarrier()->isHeatCarrier();
            }

            if (isHeatCarrier)
            {
                fluxName = fluxName + " at " + std::to_string(port->getCarrier()->Potential()) + " degC";
                storageName = storageName + " at " + std::to_string(port->getCarrier()->Potential()) + " degC";
            }

            if (port->VarType() == "vector")
            {
                std::string identifier = "";

                if (port->Direction() == GS::KPROD())
                {
                    mChargedEnergyMap[portName] = std::vector<double>(2, 0.);
                    mDischargedEnergyMap[portName] = std::vector<double>(2, 0.);
                    mNLevChargedEnergyMap[portName] = std::vector<double>(2, 0.);
                    mNLevDischargedEnergyMap[portName] = std::vector<double>(2, 0.);
                    mNbCylesMap[portName] = std::vector<double>(2, 0.);
                    mChargedMeanMap[portName] = std::vector<double>(2, 0.);
                    mDischargedMeanMap[portName] = std::vector<double>(2, 0.);
                    if (once) {
                        mInputIndicators->addIndicator("Charging time", &mChargingTime, exp, "Charging time", "h", "ChargingTime");
                        mInputIndicators->addIndicator("Discharging time ", &mDischargingTime, exp, "Discharging time", "h", "DischargingTime");
                    }
                    if (!isIndicatorNameUnique(port, "StorageName")) identifier = "(" + port->Name() + ")";
                    mInputIndicators->addIndicator("Charged    " + storageName + " " + varName + " " + identifier, &mChargedEnergyMap[portName], exp, "Charged", port->pStorageUnit(), "TotCharged" + varName);
                    mInputIndicators->addIndicator("Discharged " + storageName + " " + varName + " " + identifier, &mDischargedEnergyMap[portName], exp, "Discharged", port->pStorageUnit(), "TotDischarged" + varName);
                    mInputIndicators->addIndicator("Levelized Charged    " + storageName + " " + varName + " " + identifier, &mNLevChargedEnergyMap[portName], exp, "Levelized Charged", port->pStorageUnit(), "LvlzdTotCharged" + varName);
                    mInputIndicators->addIndicator("Levelized Discharged " + storageName + " " + varName + " " + identifier, &mNLevDischargedEnergyMap[portName], exp, "Levelized Discharged", port->pStorageUnit(), "LvlzdDischarged" + varName);
                    if (isIndicatorNameUnique(port, "FluxName")) identifier = ""; //put back to empty if name is unique w.r.t fluxName (rarely  happens!)
                    mInputIndicators->addIndicator("Mean Charged " + fluxName + " " + varName + " " + identifier, &mChargedMeanMap[portName], exp, "Mean Charged", port->pFluxUnit(), "MeanCharged" + varName);
                    mInputIndicators->addIndicator("Mean Discharged " + fluxName + " " + varName + " " + identifier, &mDischargedMeanMap[portName], exp, "Mean Discharged", port->pFluxUnit(), "MeanDischarged" + varName);
                    mInputIndicators->addIndicator("Equivalent number of cycles " + fluxName + " " + varName + " " + identifier, &mNbCylesMap[portName], exp, "Number of cycles", "-", "NbCycles");
                    once = false;
                }
                else if (port->Direction() == GS::KCONS())
                {
                    mConsumptionMap[portName] = std::vector<double>(2, 0.);
                    mConsLvlTotMap[portName] = std::vector<double>(2, 0.);
                    mConsMeanMap[portName] = std::vector<double>(2, 0.);
                    if (!isIndicatorNameUnique(port, "StorageName")) identifier = "(" + port->Name() + ")";
                    mInputIndicators->addIndicator("CONS Total " + storageName + " " + varName + " " + identifier, &mConsumptionMap[portName], exp, "CONS Tot", port->pStorageUnit(), "TotCons" + varName);
                    mInputIndicators->addIndicator("CONS Levelized Total " + storageName + " " + varName + " " + identifier, &mConsLvlTotMap[portName], exp, "CONS Levelized Tot", port->pStorageUnit(), "LvlzdTotCons" + varName);
                    if (isIndicatorNameUnique(port, "FluxName")) identifier = ""; //put back to empty if name is unique w.r.t fluxName (rarely  happens!)
                    mInputIndicators->addIndicator("CONS Mean Total " + fluxName + " " + varName + " " + identifier, &mConsMeanMap[portName], exp, "CONS Mean Tot", port->pFluxUnit(), "MeanCons" + varName);
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