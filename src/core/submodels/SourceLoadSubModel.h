#ifndef SourceLoadSubModel_H
#define SourceLoadSubModel_H

#include "TechnicalSubModel.h"

class CAIRNCORESHARED_EXPORT SourceLoadSubModel : public TechnicalSubModel
{
public:
    SourceLoadSubModel(QObject* aParent=nullptr);
    ~SourceLoadSubModel();

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

        //Indicators specific for SourceLoads
        QString InstalledSizeUnit = getOptimalSizeUnit(); // default in case no output port found which would be strange !!
        mInputIndicators->addIndicator("Component Weight", &mOptimalSize, exp, "Component size", InstalledSizeUnit, "Weight");

        if (isPriceOptimized()) mInputIndicators->addIndicator("Component Optimal Price", &mOptimalSize, exp, "Component Optimal Price", InstalledSizeUnit, "OptPrice");

        QString sens;
        if (mParentCompo->Sens() > 0) sens = "source";
        else sens = "load";

        mInputIndicators->addIndicator("ImposedProfile " + sens + " time", &mRunningTime, exp, "Running time", "h", "ImposedProfileTime");

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
            bool isHeatCarrier = port->ptrEnergyVector()->isHeatCarrier();

            if (port->VarType() == "vector")
            {
                QString identifier = "";

                // Why "sens" is not included in the indicator's name like in the case of Grid ?

                if (port->Direction() == GS::KPROD() && sens == "source")  
                {
                    mProductionMap[portName] = std::vector<double>(2, 0.);
                    mProdLvlTotMap[portName] = std::vector<double>(2, 0.);
                    mProdMeanMap[portName] = std::vector<double>(2, 0.);
                    if (!isIndicatorNameUnique(port, "StorageName")) identifier = "(" + port->Name() + ")";
                    mInputIndicators->addIndicator("ImposedProfile " + storageName + " " + varName + " " + identifier, &mProductionMap[portName], exp, "ImposedProfile " + storageName + " " + varName, storageUnit, "TotImposedProfile" + varName);
                    mInputIndicators->addIndicator("Levelized ImposedProfile " + storageName + " " + varName + " " + identifier, &mProdLvlTotMap[portName], exp, "Levelized ImposedProfile " + storageName + " " + varName, storageUnit, "LvlzdTotImposedProfile" + varName);
                    if (isIndicatorNameUnique(port, "FluxName")) identifier = ""; //put back to empty if name is unique w.r.t fluxName (rarely  happens!)
                    mInputIndicators->addIndicator("Mean " + fluxName + " " + varName + " " + identifier, &mProdMeanMap[portName], exp, "Mean " + fluxName + " " + varName, fluxUnit, "MeanImposedProfile" + varName);
                }
                else if (port->Direction() == GS::KCONS() && sens == "load")
                {
                    mConsumptionMap[portName] = std::vector<double>(2, 0.);
                    mConsLvlTotMap[portName] = std::vector<double>(2, 0.);
                    mConsMeanMap[portName] = std::vector<double>(2, 0.);
                    if (!isIndicatorNameUnique(port, "StorageName")) identifier = "(" + port->Name() + ")";
                    mInputIndicators->addIndicator("ImposedProfile " + storageName + " " + varName + " " + identifier, &mConsumptionMap[portName], exp, "ImposedProfile " + storageName + " " + varName, storageUnit, "TotImposedProfile" + varName);
                    mInputIndicators->addIndicator("Levelized ImposedProfile " + storageName + " " + varName + " " + identifier, &mConsLvlTotMap[portName], exp, "Levelized ImposedProfile " + storageName + " " + varName, storageUnit, "LvlzdTotImposedProfile" + varName);
                    if (isIndicatorNameUnique(port, "FluxName")) identifier = ""; //put back to empty if name is unique w.r.t fluxName (rarely  happens!)
                    mInputIndicators->addIndicator("Mean " + fluxName + " " + varName + " " + identifier, &mConsMeanMap[portName], exp, "Mean " + fluxName + " " + varName, fluxUnit, "MeanImposedProfile" + varName);
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
    
    void declareSourceENRModelIndicators(bool* exp)
    {
        TechnicalSubModel::declareDefaultModelIndicators();

        QString InstalledSizeUnit = getOptimalSizeUnit(); // default in case no output port found which would be strange !!
        mInputIndicators->addIndicator("Component Weight", &mOptimalSize, exp, "Component size", InstalledSizeUnit, "Weight");

        MilpPort* port;
        QListIterator<MilpPort*> iport(mListPort);
        while (iport.hasNext())
        {
            port = iport.next();
            QString varName = port->Variable();
            QString storageName = port->ptrEnergyVector()->StorageName();
            QString storageUnit = port->ptrEnergyVector()->StorageUnit();
            QString fluxUnit = port->ptrEnergyVector()->FluxUnit();
            QString fluxName = port->ptrEnergyVector()->FluxName();

            if (port->VarType() == "vector")
            {
                if (port->Direction() == GS::KPROD())
                {
                    mProductionMap[varName] = std::vector<double>(2, 0.);
                    mProdLvlTotMap[varName] = std::vector<double>(2, 0.);
                    mProdMeanMap[varName] = std::vector<double>(2, 0.);
                    mInputIndicators->addIndicator("ENR injection time", &mRunningTime, exp, "Running time", "h", "ENRInjectionTime");
                    mInputIndicators->addIndicator("ENR injection " + storageName + " " + varName, &mProductionMap[varName], exp, "", storageUnit, "Tot" + varName);
                    mInputIndicators->addIndicator("Levelized ENR injection " + storageName + " " + varName, &mProdLvlTotMap[varName], exp, "", storageUnit, "LvlzdTot" + varName);
                    mInputIndicators->addIndicator("Mean " + fluxName + " " + varName, &mProdMeanMap[varName], exp, "Mean", fluxUnit, "Mean" + varName);
                }
                else if (port->Direction() == GS::KDATA())
                {
                    mExpEchData[varName] = std::vector<double>(2, 0.);
                    mInputIndicators->addIndicator("Data Port published " + varName + " - data computed ", &mExpEchData[varName], exp, "Data port", storageUnit, "DataPort" + varName);
                }
            }
        }
    }

    void computeSourceENRModelIndicators(const double* optSol); 
    void computeDefaultIndicators(const double* optSol);

    virtual void initDefaultPorts() {
        mDefaultPorts.clear();
        //PortSourceLoadFlow - left
        QMap<QString, QString> portSourceLoadFlow;
        portSourceLoadFlow["Name"] = "PortL0";
        portSourceLoadFlow["Position"] = "left";
        portSourceLoadFlow["CarrierType"] = ANY_TYPE();
        portSourceLoadFlow["Direction"] = KCONS();
        portSourceLoadFlow["Variable"] = "SourceLoadFlow";
        mDefaultPorts.insert("PortSourceLoadFlow", portSourceLoadFlow);
    }

protected:
    virtual int checkBusFlowBalanceVarName(MilpPort* port, int& inumberchange, QString& varUseCheck);
};

#endif // SourceLoadSubModel_H