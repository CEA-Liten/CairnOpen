#ifndef ConverterSubModel_H
#define ConverterSubModel_H

#include "TechnicalSubModel.h"
#include "AgeingRunningHours.h"

class CAIRNCORESHARED_EXPORT ConverterSubModel : public TechnicalSubModel
{
public:
    ConverterSubModel(QObject* aParent=nullptr);
    ~ConverterSubModel();
    
    void setTimeData();

    void setMinPower(MIPModeler::MIPExpression1D aPower, std::vector<double> aMinPowList, double aNomPower);
    void setMinPower(MIPModeler::MIPExpression1D aPower, double aMinPow, double aNomPower);
    void setMinPower(MIPModeler::MIPExpression1D aPower, MIPModeler::MIPVariable1D aZ, std::vector<double> aMinPowList, double aNomPower);

    void declareDefaultModelConfigurationParameters()
    {
        mInputParam->addToConfigList({ "EcoInvestModel","EnvironmentModel"});
        TechnicalSubModel::declareDefaultModelConfigurationParameters();
    }
    
    void declareDefaultModelParameters()
    {
        TechnicalSubModel::declareDefaultModelParameters();
        //vector
        addTimeSeries("UseProfileConverterUse", &mConverterUse, false, true, "Time Series of converter allowance to use if 1 and forbidden if 0 - Use to simulate unavailability for failiures or maintenance");

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
        QString InstalledSizeUnit = getOptimalSizeUnit(); // default in case no output port found which would be strange !!

        mInputIndicators->addIndicator("Sum up at", &mSumUp, exp, "Component size (found by optimizer)", "", "SumUp");
        mInputIndicators->addIndicator("Installed Size", &mOptimalSize, exp, "Component size", InstalledSizeUnit, "Size");

        mInputIndicators->addIndicator("Running time at power >0.", &mRunningTime, exp, "Running time", "h", "RunningTime");
        mInputIndicators->addIndicator("Running time availability", &mRunningTimeAvlblt, exp, "Maximum possible running time", "-", "RunningTimeAvailable");
        if (mUseAgeing) {
            mInputIndicators->addIndicator("Efficiency after running time << ", &mEfficiency_Ageing, exp, "Efficiency after running time", "-","Efficiency");
        }

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

                if (port->Direction() == GS::KPROD()) {
                    mProductionMap[portName] = std::vector<double>(2, 0.);
                    mProdLvlTotMap[portName] = std::vector<double>(2, 0.);
                    mProdMeanMap[portName] = std::vector<double>(2, 0.);
                    mProdContributionMap[portName] = std::vector<double>(2, 0.);
                    if (!isIndicatorNameUnique(port, "StorageName")) identifier = "(" + port->Name() + ")";
                    mInputIndicators->addIndicator("PROD Total " + storageName + " " + varName + " " + identifier, &mProductionMap[portName], exp, "Total production of "+varName, storageUnit, "TotProd"+varName);
                    mInputIndicators->addIndicator("PROD Levelized Total " + storageName + " " + varName + " " + identifier, &mProdLvlTotMap[portName], exp, "Total levelized production of " + varName, storageUnit, "LvlzdTotProd"+varName);
                    if (isIndicatorNameUnique(port, "FluxName")) identifier = ""; //put back to empty if name is unique w.r.t fluxName (rarely  happens!)
                    mInputIndicators->addIndicator("PROD Mean Total " + fluxName + " " + varName + " " + identifier, &mProdMeanMap[portName], exp, "Mean production of " + varName, fluxUnit,"MeanProd"+varName);
                }
                else if (port->Direction() == GS::KCONS()) {
                    mConsumptionMap[portName] = std::vector<double>(2, 0.);
                    mConsLvlTotMap[portName] = std::vector<double>(2, 0.);
                    mConsMeanMap[portName] = std::vector<double>(2, 0.);
                    mConsPFMap[portName] = std::vector<double>(2, 0.);
                    mRateOfUse[portName] = std::vector<double>(2, 0.);
                    if (!isIndicatorNameUnique(port, "StorageName")) identifier = "(" + port->Name() + ")";
                    mInputIndicators->addIndicator("CONS Total " + storageName + " " + varName + " " + identifier, &mConsumptionMap[portName], exp, "Total consumption of " + varName, storageUnit, "TotCons" + varName);
                    mInputIndicators->addIndicator("CONS Levelized Total " + storageName + " " + varName + " " + identifier, &mConsLvlTotMap[portName], exp, "Levelized total consumption of " + varName, storageUnit, "LvlzdTotCons" + varName);
                    if (isIndicatorNameUnique(port, "FluxName")) identifier = ""; //put back to empty if name is unique w.r.t fluxName (rarely  happens!)
                    mInputIndicators->addIndicator("CONS Mean Total " + fluxName + " " + varName + " " + identifier, &mConsMeanMap[portName], exp, "Mean consumption of " + varName, fluxUnit, "MeanCons" + varName);
                    if (!isIndicatorNameUnique(port)) identifier = "(" + port->Name() + ")";
                    mInputIndicators->addIndicator("CONS Total Power Factor " + varName + " " + identifier, &mConsPFMap[portName], exp, "Power factor of " + varName, "-", "PowerFactor"+varName);
                    mInputIndicators->addIndicator("Rate of use " + varName + " " + identifier, &mRateOfUse[portName], exp, "Rate of use", "-", "UseRate"+varName);
                }
                else if (port->Direction() == GS::KDATA()) {
                    if (!isIndicatorNameUnique(port)) identifier = "(" + port->Name() + ")";
                    mExpEchData[portName] = std::vector<double>(2, 0.);
                    mInputIndicators->addIndicator("Data Port published " + varName + " - data computed " + identifier, &mExpEchData[portName], exp, "Data port", storageUnit, "DataPort"+varName);
                }
            }
        }
    }

    void computeDefaultIndicators(const double* optSol);

   

    //used in MultiConverter and Cogeneration
    void cleanFluxIOs(QString name); 
    virtual void declareInputFluxIOs(MilpPort* defaultPort = nullptr);
    virtual void declareOutputFluxIOs(MilpPort* defaultPort = nullptr);
    
    virtual void buildModel();
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
    std::vector<double> mConverterUse;      /** time series for converter use availability */

    //used in MultiConverter and Cogeneration
    std::vector <MIPModeler::MIPExpression1D> mExpInput;
    std::vector <MIPModeler::MIPExpression1D> mExpOutput;

    AgeingRunningHours* mAgeingModel ;
};

#endif // ConverterSubModel_H