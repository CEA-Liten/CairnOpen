#ifndef SourceLoadSubModel_H
#define SourceLoadSubModel_H

#include "TechnicalSubModel.h"

class CAIRNCORESHARED_EXPORT SourceLoadSubModel : public TechnicalSubModel
{
public:
    SourceLoadSubModel(CairnObject* aParent=nullptr);
    ~SourceLoadSubModel();
    virtual void setTimeData();

    double Sens();

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

        //Indicators specific for SourceLoads
        mInputIndicators->addIndicator("Component Weight", &mOptimalSize, exp, "Component size", pOptimalSizeUnit(), "Weight");

        if (isPriceOptimized()) mInputIndicators->addIndicator("Component Optimal Price", &mOptimalSize, exp, "Component Optimal Price", pOptimalSizeUnit(), "OptPrice");

        std::string sens;
        if (Sens() > 0) sens = "source";
        else sens = "load";

        mInputIndicators->addIndicator("ImposedProfile " + sens + " time", &mRunningTime, exp, "Running time", "h", "ImposedProfileTime");

        for (auto& port : mListPort) {        
            std::string portName = port->Name();
            std::string varName = port->Variable();
            std::string storageName = port->getCarrier()->StorageName();
            std::string fluxName = port->getCarrier()->FluxName();
            bool isHeatCarrier = port->getCarrier()->isHeatCarrier();

            if (port->VarType() == "vector")
            {
                std::string identifier = "";

                // Why "sens" is not included in the indicator's name like in the case of Grid ?

                if (port->Direction() == GS::KPROD() && sens == "source")  
                {
                    mProductionMap[portName] = std::vector<double>(2, 0.);
                    mProdLvlTotMap[portName] = std::vector<double>(2, 0.);
                    mProdMeanMap[portName] = std::vector<double>(2, 0.);
                    if (!isIndicatorNameUnique(port, "StorageName")) identifier = "(" + port->Name() + ")";
                    mInputIndicators->addIndicator("ImposedProfile " + storageName + " " + varName + " " + identifier, &mProductionMap[portName], exp, "ImposedProfile " + storageName + " " + varName, port->pStorageUnit(), "TotImposedProfile" + varName);
                    mInputIndicators->addIndicator("Levelized ImposedProfile " + storageName + " " + varName + " " + identifier, &mProdLvlTotMap[portName], exp, "Levelized ImposedProfile " + storageName + " " + varName, port->pStorageUnit(), "LvlzdTotImposedProfile" + varName);
                    if (isIndicatorNameUnique(port, "FluxName")) identifier = ""; //put back to empty if name is unique w.r.t fluxName (rarely  happens!)
                    mInputIndicators->addIndicator("Mean " + fluxName + " " + varName + " " + identifier, &mProdMeanMap[portName], exp, "Mean " + fluxName + " " + varName, port->pFluxUnit(), "MeanImposedProfile" + varName);
                }
                else if (port->Direction() == GS::KCONS() && sens == "load")
                {
                    mConsumptionMap[portName] = std::vector<double>(2, 0.);
                    mConsLvlTotMap[portName] = std::vector<double>(2, 0.);
                    mConsMeanMap[portName] = std::vector<double>(2, 0.);
                    if (!isIndicatorNameUnique(port, "StorageName")) identifier = "(" + port->Name() + ")";
                    mInputIndicators->addIndicator("ImposedProfile " + storageName + " " + varName + " " + identifier, &mConsumptionMap[portName], exp, "ImposedProfile " + storageName + " " + varName, port->pStorageUnit(), "TotImposedProfile" + varName);
                    mInputIndicators->addIndicator("Levelized ImposedProfile " + storageName + " " + varName + " " + identifier, &mConsLvlTotMap[portName], exp, "Levelized ImposedProfile " + storageName + " " + varName, port->pStorageUnit(), "LvlzdTotImposedProfile" + varName);
                    if (isIndicatorNameUnique(port, "FluxName")) identifier = ""; //put back to empty if name is unique w.r.t fluxName (rarely  happens!)
                    mInputIndicators->addIndicator("Mean " + fluxName + " " + varName + " " + identifier, &mConsMeanMap[portName], exp, "Mean " + fluxName + " " + varName, port->pFluxUnit(), "MeanImposedProfile" + varName);
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
    
    void declareSourceENRModelIndicators(bool* exp)
    {
        TechnicalSubModel::declareDefaultModelIndicators();

        std::string InstalledSizeUnit = OptimalSizeUnit(); // default in case no output port found which would be strange !!
        mInputIndicators->addIndicator("Component Weight", &mOptimalSize, exp, "Component size", InstalledSizeUnit, "Weight");

        for (auto& port : mListPort) {
            std::string varName = port->Variable();
            std::string storageName = port->getCarrier()->StorageName();
            std::string fluxName = port->getCarrier()->FluxName();

            if (port->VarType() == "vector")
            {
                if (port->Direction() == GS::KPROD())
                {
                    mProductionMap[varName] = std::vector<double>(2, 0.);
                    mProdLvlTotMap[varName] = std::vector<double>(2, 0.);
                    mProdMeanMap[varName] = std::vector<double>(2, 0.);
                    mInputIndicators->addIndicator("ENR injection time", &mRunningTime, exp, "Running time", "h", "ENRInjectionTime");
                    mInputIndicators->addIndicator("ENR injection " + storageName + " " + varName, &mProductionMap[varName], exp, "", port->pStorageUnit(), "Tot" + varName);
                    mInputIndicators->addIndicator("Levelized ENR injection " + storageName + " " + varName, &mProdLvlTotMap[varName], exp, "", port->pStorageUnit(), "LvlzdTot" + varName);
                    mInputIndicators->addIndicator("Mean " + fluxName + " " + varName, &mProdMeanMap[varName], exp, "Mean", port->pFluxUnit(), "Mean" + varName);
                }
                else if (port->Direction() == GS::KDATA())
                {
                    mExpEchData[varName] = std::vector<double>(2, 0.);
                    mInputIndicators->addIndicator("Data Port published " + varName + " - data computed ", &mExpEchData[varName], exp, "Data port", port->pStorageUnit(), "DataPort" + varName);
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