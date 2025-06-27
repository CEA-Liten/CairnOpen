#ifndef StorageSubModel_H
#define StorageSubModel_H

#include "TechnicalSubModel.h"

class CAIRNCORESHARED_EXPORT StorageSubModel : public TechnicalSubModel
{
public:
    StorageSubModel(QObject* aParent=nullptr);
    ~StorageSubModel();

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
        QString InstalledSizeUnit = getOptimalSizeUnit(); // default in case no output port found which would be strange !!
        mInputIndicators->addIndicator("Storage Capacity", &mOptimalSize, exp, "Component size", InstalledSizeUnit, "Capacity");

        bool once = true;

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

            if (isHeatCarrier)
            {
                fluxName = fluxName + " at " + QString::number(port->ptrEnergyVector()->Potential()) + " degC";
                storageName = storageName + " at " + QString::number(port->ptrEnergyVector()->Potential()) + " degC";
            }

            if (port->VarType() == "vector")
            {
                QString identifier = "";

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
                    mInputIndicators->addIndicator("Charged    " + storageName + " " + varName + " " + identifier, &mChargedEnergyMap[portName], exp, "Charged", storageUnit, "TotCharged" + varName);
                    mInputIndicators->addIndicator("Discharged " + storageName + " " + varName + " " + identifier, &mDischargedEnergyMap[portName], exp, "Discharged", storageUnit, "TotDischarged" + varName);
                    mInputIndicators->addIndicator("Levelized Charged    " + storageName + " " + varName + " " + identifier, &mNLevChargedEnergyMap[portName], exp, "Levelized Charged", storageUnit, "LvlzdTotCharged" + varName);
                    mInputIndicators->addIndicator("Levelized Discharged " + storageName + " " + varName + " " + identifier, &mNLevDischargedEnergyMap[portName], exp, "Levelized Discharged", storageUnit, "LvlzdDischarged" + varName);
                    if (isIndicatorNameUnique(port, "FluxName")) identifier = ""; //put back to empty if name is unique w.r.t fluxName (rarely  happens!)
                    mInputIndicators->addIndicator("Mean Charged " + fluxName + " " + varName + " " + identifier, &mChargedMeanMap[portName], exp, "Mean Charged", fluxUnit, "MeanCharged" + varName);
                    mInputIndicators->addIndicator("Mean Discharged " + fluxName + " " + varName + " " + identifier, &mDischargedMeanMap[portName], exp, "Mean Discharged", fluxUnit, "MeanDischarged" + varName);
                    mInputIndicators->addIndicator("Equivalent number of cycles " + fluxName + " " + varName + " " + identifier, &mNbCylesMap[portName], exp, "Number of cycles", "-", "NbCycles");
                    once = false;
                }
                else if (port->Direction() == GS::KCONS())
                {
                    mConsumptionMap[portName] = std::vector<double>(2, 0.);
                    mConsLvlTotMap[portName] = std::vector<double>(2, 0.);
                    mConsMeanMap[portName] = std::vector<double>(2, 0.);
                    if (!isIndicatorNameUnique(port, "StorageName")) identifier = "(" + port->Name() + ")";
                    mInputIndicators->addIndicator("CONS Total " + storageName + " " + varName + " " + identifier, &mConsumptionMap[portName], exp, "CONS Tot", storageUnit, "TotCons" + varName);
                    mInputIndicators->addIndicator("CONS Levelized Total " + storageName + " " + varName + " " + identifier, &mConsLvlTotMap[portName], exp, "CONS Levelized Tot", storageUnit, "LvlzdTotCons" + varName);
                    if (isIndicatorNameUnique(port, "FluxName")) identifier = ""; //put back to empty if name is unique w.r.t fluxName (rarely  happens!)
                    mInputIndicators->addIndicator("CONS Mean Total " + fluxName + " " + varName + " " + identifier, &mConsMeanMap[portName], exp, "CONS Mean Tot", fluxUnit, "MeanCons" + varName);
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

    void computeDefaultIndicators(const double* optSol);

    virtual void initDefaultPorts() {
        mDefaultPorts.clear();
        //PortFlow - left
        QMap<QString, QString> portFlow;
        portFlow["Name"] = "PortL0";
        portFlow["Position"] = "left";
        portFlow["CarrierType"] = ANY_TYPE();
        portFlow["Direction"] = KPROD();
        portFlow["Variable"] = "Flow";
        mDefaultPorts.insert("PortFlow", portFlow);
    }
    
protected:
    virtual int checkBusFlowBalanceVarName(MilpPort* port, int& inumberchange, QString& varUseCheck);
};

#endif // StorageSubModel_H