#ifndef TechnicalSubModel_H
#define TechnicalSubModel_H

#include "SubModel.h"

class CAIRNCORESHARED_EXPORT TechnicalSubModel : public SubModel
{
public:
    TechnicalSubModel(QObject* aParent=nullptr);
    ~TechnicalSubModel();
    /** -------------------------------------------------------------------------------------------------------------- */
    virtual void computeGeometricContribution();   /** MILP Model description : geometric expressions */
    virtual void computeEnvContribution();        /** MILP Model description : environment expressions */
    virtual void computeEconomicalContribution(); /** MILP Model description : economical expressions */
    void computeNetOpexContribution();            /** Compute Net Opex contribution expression, pureOpex + replacement + varcost */
    virtual void computeAllContribution();       /** MILP Model description : all expressions */
    virtual void computeDefaultIndicators(const double* optSol);
    // ------------------------- Defaul Parameters ----------------------------------------
    virtual void declareDefaultModelConfigurationParameters() 
    {
        SubModel::declareDefaultModelConfigurationParameters();
        //bool
        addParameter("EcoInvestModel", &mEcoInvestModel, true, false, true, "Use EcoInvestModel - ie Use Capex and Opex if true", "", { "EcoInvestModel" });    /** Use EcoInvestModel - ie Use Capex and Opex if true */
        addParameter("GeometryModel", &mGeometryModel, false, false, true, "Use GeometryModel", "", { "GeometryModel" });    /** Use GeometryModel */
        addParameter("EnvironmentModel", &mEnvironmentModel, false, false, true, "Use EnvironmentModel - ie Use EnvEmissionCost if true", "", { "EnvironmentModel" }); /** Use EnvironmentModel - ie Use EnvEmissionCost if true  */
        addParameter("UseWeightOptimization", &mUseWeightOptimization, false, false, true, "Use sizing based on Weight if true", "", { "" }); /** Use sizing based on Weight if true - default is false*/
        addParameter("LPModelONLY", &mLPModelOnly, false, false, true, "Use LP Model - ie integer variables imposed or relaxed to real variables if true", "", { "" });          /** Use LP Model - ie binary variable imposed if true */
        addParameter("UseAgeing", &mUseAgeing, false, false, true, "", "", { "Ageing" });          /** Use UseAgeing Model if true - default to false. Current Efficiency will be reduced by 1-EfficiencyAgeingCoeff*HistRunnningTime and reset to 1 when HistRunnningTime reaches EfficiencyMaxHours */
        addParameter("AddMinimumCapacity", &mAddMinimumCapacity, false, false, true, "Give a minimal size for the component", "", { "" });
        addParameter("PiecewiseCapex", &mPiecewiseCapex, false, false, true, "Provide a map of capex and sizes - see performances maps in doc", "", { "EcoInvestModel" });
        addParameter("PiecewiseArea", &mPiecewiseArea, false, false, true, "Provide a map of areas and component sizes - see performances maps in doc", "", { "GeometryModel" });
        addParameter("PiecewiseVolume", &mPiecewiseVolume, false, false, true, "Provide a map of volumes and sizes - see performances maps in doc", "", {"GeometryModel"});
        addParameter("PiecewiseMass", &mPiecewiseMass, false, false, true, "Provide a map of masses and sizes - see performances maps in doc", "", { "GeometryModel" });
        addParameter("CondenseVariablesOnTP", &mCondenseVariablesOnTP, false, false, true, "", "", { "AddOperationConstraints" }); /** to condense variables on the typical periods definition except for the storage state : allows storage between typical periods, not available for constraints linking several time steps for the moment */
        addParameter("SeasonalPrevisions", &mSeasonalPrevisions, false, false, true, "Use long term time series forecasts", "", { "TimeSeriesForecast" });

        //int
        mNbInputPorts = getNbPorts(KCONS());
        mNbOutputPorts = getNbPorts(KPROD());
    
        if (!mVariablePortNumber) { 
            mNbInputFlux = mNbInputPorts;
            mNbOutputFlux = mNbOutputPorts;
        }

        //QString
        mEnvImpacts.resize(mEnvImpactsList.size());
        for (int i = 0; i < mEnvImpactsList.size(); i++)
        {
            mEnvImpacts[i] = new EnvImpact(this, mEnvImpactsList.at(i), mEnvImpactsShortNamesList.at(i));

            MilpPort* port;
            QListIterator<MilpPort*> iport(mListPort);
            int j = 0;
            mEnvImpacts[i]->resizeCoeffs(mListPort.size());
            while (iport.hasNext())
            {
                port = iport.next();
                mEnvImpacts[i]->addModelConfigParameters(mInputEnvImpacts, mInputPortImpacts, &mEnvironmentModel, port->Name(), j);
                j++;
            }
        }
    }
    
    virtual void declareDefaultModelParameters()
    {
        //bool
        addParameter("LPWeightOptimization", &mLPWeightOptimization, false, false, true, "Use integer Weight if false ", "", { "" }); /** Use sizing based on Weight if true - default is false*/
        //double 
        addParameter("Weight", &mWeight, 1., &mUseWeightOptimization, true);	/** Weight of identical component, use negative value for optimization */
        addParameter("LifeTime", &mLifeTime, 1., false, SFunctionFlag({ eFTypeOrNot, { &mEcoInvestModel, &mEnvironmentModel} }), "LifeTime in years", "Year", { "EcoInvestModel","EnvironmentModel"});              /** LifeTime in years */
        addParameter("Capex", &mCapex, 0., &mEcoInvestModel, &mEcoInvestModel, "Elementary Capex in Euro per unit installed nominal storage or production capacity", mCurrency+"/OptimalSizeUnit", { "EcoInvestModel" });                /** Elementary Capex in Euro per unit installed nominal power, flowrate or capacity  */
        addParameter("TotalCapexCoefficient", &mTotalCapexCoefficient, 1., false, &mEcoInvestModel, "Multiplicative coefficient on elementary Capex", "-", { "EcoInvestModel" });  /** Multiplicative coefficient on elementary Capex to account for fees, land taxes, structure costs... ie cost += Capex*mTotalCapexCoefficient */
        addParameter("TotalCapexOffset", &mTotalCapexOffset, 0., false, &mEcoInvestModel, "Additive offset coefficient on elementary Capex", mCurrency, { "EcoInvestModel" });  /** Offset coefficient on elementary Capex to account for fees, land taxes, structure costs... ie cost += Capex*mTotalCapexCoefficient */
        addParameter("Opex", &mOpex, 0., &mEcoInvestModel, &mEcoInvestModel, "Opex in proportion of Elementary Capex", "-", { "EcoInvestModel" });					/** Opex in percent of Elementary Capex, ie Opex cost += Capex * Opex * levelization + Sum(VariableCosts*Timestep*levelization) */
        addParameter("Replacement", &mReplacement, 0., false, &mEcoInvestModel, "Replacement costs in proportion of Elementary Capex", mCurrency+"/OptimalSizeUnit", { "EcoInvestModel" });   /** Replacement costs in percent of Elementary Capex, ie cost += Capex* Replacement*Use_Time*levelization */
        addParameter("OpexConstant", &mOpexConstant, 0., false, &mEcoInvestModel, "The constant part of the Opex", "-", { "EcoInvestModel" });					/** The constant part of the yearly Opex : Opex = mOpex * mCapex + mOpexConstant */
        addParameter("ReplacementConstant", &mReplacementConstant, 0., false, &mEcoInvestModel, "The constant part of the replacement cost", mCurrency, { "EcoInvestModel" });   /** The constant part of the Replacement cost : mReplacement * mCapex + mReplacementConstant */
        addParameter("MinSize", &mMinSize, 0., &mAddMinimumCapacity, &mAddMinimumCapacity, "Storage minimum content in energy for electrical or thermal carriers or mass of fluids", "StorageUnit", { "" }); /** Minimum capacity  */

        //bool
        addParameter("TryRelaxationCapex", &mTryRelaxationCapex, true, SFunctionFlag({ eFTypeNotAnd, {}, { &mEcoInvestModel,&mPiecewiseCapex} }), SFunctionFlag({ eFTypeNotAnd, {}, { &mEcoInvestModel,&mPiecewiseCapex} }), "If the CAPEX is a convex function of size the linearization variables will be continuous", "bool", { "EcoInvestModel" });
        addParameter("TryRelaxationArea", &mTryRelaxationArea, true, SFunctionFlag({ eFTypeNotAnd, {}, { &mGeometryModel, &mPiecewiseArea} }), SFunctionFlag({ eFTypeNotAnd, {}, { &mGeometryModel, &mPiecewiseArea} }), "If the Area is a convex function of size the linearization variables will be continuous", "bool", { "GeometryModel" });
        addParameter("TryRelaxationVolume", &mTryRelaxationVolume, true, SFunctionFlag({ eFTypeNotAnd, {}, { &mGeometryModel, &mPiecewiseVolume} }), SFunctionFlag({ eFTypeNotAnd, {}, { &mGeometryModel, &mPiecewiseVolume} }), "If the Volume is a convex function of size the linearization variables will be continuous", "bool", { "GeometryModel" });
        addParameter("TryRelaxationMass", &mTryRelaxationMass, true, SFunctionFlag({ eFTypeNotAnd, {}, { &mGeometryModel, &mPiecewiseMass} }), SFunctionFlag({ eFTypeNotAnd, {}, { &mGeometryModel, &mPiecewiseMass} }), "If the Mass is a convex function of size the linearization variables will be continuous", "bool", { "GeometryModel" });

        //double
        addParameter("Area", &mArea, 0., SFunctionFlag({ eFTypeNotAnd, { &mPiecewiseArea}, { &mGeometryModel} }), SFunctionFlag({ eFTypeNotAnd, { &mPiecewiseArea}, { &mGeometryModel} }), "footprint occupied by component per installed unit or weight", "m2/OptimalSizeUnit",{"GeometryModel"});
        addParameter("Volume", &mVolume, 0., SFunctionFlag({ eFTypeNotAnd, { &mPiecewiseVolume}, { &mGeometryModel} }), SFunctionFlag({ eFTypeNotAnd, { &mPiecewiseArea}, { &mGeometryModel} }), "footprint occupied by component per installed unit or weight", "m3/OptimalSizeUnit", { "GeometryModel" });
        addParameter("Mass", &mMass, 0., SFunctionFlag({ eFTypeNotAnd, { &mPiecewiseMass}, { &mGeometryModel} }), SFunctionFlag({ eFTypeNotAnd, { &mPiecewiseArea}, { &mGeometryModel} }), "footprint occupied by component per installed unit or weight", "kg/OptimalSizeUnit", { "GeometryModel" });

        //vector
        addPerfParam("CapexCapacitySetPoint", &mCapexCapacitySetPoint, SFunctionFlag({ eFTypeNotAnd, {}, { &mEcoInvestModel,&mPiecewiseCapex} }), SFunctionFlag({ eFTypeNotAnd, {}, { &mEcoInvestModel,&mPiecewiseCapex} }), "name of vector capacity that will be defined from DataFile specification by the User", "", { "EcoInvestModel" }); /** length of vectors of capacity that will be defined from DataFile specification by the User */
        addPerfParam("CapexSetPoint", &mCapexSetPoint, SFunctionFlag({ eFTypeNotAnd, {}, { &mEcoInvestModel,&mPiecewiseCapex} }), SFunctionFlag({ eFTypeNotAnd, {}, { &mEcoInvestModel,&mPiecewiseCapex} }), "name of vector cost that will be defined from DataFile specification by the User", "", { "EcoInvestModel" }); /** length of vectors of cost that will be defined from DataFile specification by the User */

        addPerfParam("AreaCapacitySetPoint", &mAreaCapacitySetPoint, SFunctionFlag({ eFTypeNotAnd, {}, { &mGeometryModel, &mPiecewiseArea} }), SFunctionFlag({ eFTypeNotAnd, {}, { &mGeometryModel, &mPiecewiseArea} }), "name of vector area capacity that will be defined from DataFile specification by the User", "", { "GeometryModel" });
        addPerfParam("AreaSetPoint", &mAreaSetPoint, SFunctionFlag({ eFTypeNotAnd, {}, { &mGeometryModel, &mPiecewiseArea} }), SFunctionFlag({ eFTypeNotAnd, {}, { &mGeometryModel, &mPiecewiseArea} }), "name of vector area SetPoint that will be defined from DataFile specification by the User", "", { "GeometryModel" });
          
        addPerfParam("VolumeCapacitySetPoint", &mVolumeCapacitySetPoint, SFunctionFlag({ eFTypeNotAnd, {}, { &mGeometryModel, &mPiecewiseVolume} }), SFunctionFlag({ eFTypeNotAnd, {}, { &mGeometryModel, &mPiecewiseVolume} }), "name of vector volume capacity that will be defined from DataFile specification by the User", "", { "GeometryModel" });
        addPerfParam("VolumeSetPoint", &mVolumeSetPoint, SFunctionFlag({ eFTypeNotAnd, {}, { &mGeometryModel, &mPiecewiseVolume} }), SFunctionFlag({ eFTypeNotAnd, {}, { &mGeometryModel, &mPiecewiseVolume} }), "name of vector volume SetPoint that will be defined from DataFile specification by the User", "", { "GeometryModel" });
       
        addPerfParam("MassCapacitySetPoint", &mMassCapacitySetPoint, SFunctionFlag({ eFTypeNotAnd, {}, { &mGeometryModel, &mPiecewiseMass} }), SFunctionFlag({ eFTypeNotAnd, {}, { &mGeometryModel, &mPiecewiseMass} }), "name of vector mass capacity that will be defined from DataFile specification by the User", "", { "GeometryModel" });
        addPerfParam("MassSetPoint", &mMassSetPoint, SFunctionFlag({ eFTypeNotAnd, {}, { &mGeometryModel, &mPiecewiseMass} }), SFunctionFlag({ eFTypeNotAnd, {}, { &mGeometryModel, &mPiecewiseMass} }), "name of vector mass SetPoint that will be defined from DataFile specification by the User", "", { "GeometryModel" });
        
        //EnvImpacts
        mEnvImpacts.resize(mEnvImpactsList.size(), new EnvImpact());
        mEnvImpactCosts.resize(mEnvImpactsList.size(), 0.0);
        //
        for (int i = 0; i < mEnvImpactsList.size(); i++)
        {
            mEnvImpacts[i]->setTimesteps(mTimeSteps);
            mEnvImpacts[i]->setEnvImpactCost(mEnvImpactCosts[i]);
            if (mEnvImpactUnitsList.size() > i) {
                mEnvImpacts[i]->setEnvImpactUnit(mEnvImpactUnitsList[i]);
            }
            else {
                mEnvImpacts[i]->setEnvImpactUnit("-");
            }

            MilpPort* port;
            QListIterator<MilpPort*> iport(mListPort);
            int j = 0;
            mEnvImpacts[i]->resizeCoeffs(mListPort.size());
            while (iport.hasNext())
            {
                port = iport.next();
                QString aPortName = port->Name();
                mEnvImpacts[i]->addParameters(mInputPortImpacts, mTSInputPortImpacts, &mEnvironmentModel, port->Name(), j);
                j++;
            }
            mEnvImpacts[i]->addGreyParameters(mInputEnvImpacts, &mEnvironmentModel);
            mEnvImpacts[i]->addPerfParam(mInputPerfParam);
        }
    }

    virtual void declareDefaultModelInterface()
    {
        SubModel::declareDefaultModelInterface();

        setOptimalSizeExpression("SizeMax");  // defines default expression to be used for OptimalSize computation and use in Economic analysis 
        addIO("SizeMax", &mExpSizeMax, "Size");    /** Computed variable costs resulting from material/fuel consumption */

        addIO("VariableCosts", &mExpVariableCosts, "Currency");    /** Computed variable costs resulting from material/fuel consumption */
        setVariableCostsExpression("VariableCosts");  // defines default expression to be used for VariableCosts computation and use in Economic analysis

        if (mEcoInvestModel) {
            addIO("Capex", &mExpCapex, mEnergyVector->pFluxUnit()); /** Computed initial investment costs Capex */
            addIO("Opex", &mExpOpex, mEnergyVector->pStorageUnit());      /** Computed operational cost Net Opex */
            addIO("PureOpex", &mExpPureOpex, mEnergyVector->pStorageUnit());      /** Computed operational cost Pure Opex */
            addIO("Replacement", &mExpReplacement, mEnergyVector->pFluxUnit());      /** Computed variable replacement cost */
        }

        if (mGeometryModel) {
            addIO("Area", &mExpArea, "m2");
            addIO("Volume", &mExpVolume, "m3");
            addIO("Mass", &mExpMass, "kg");
        }

        if (mEnvironmentModel) {
            for (EnvImpact* impact : mEnvImpacts)
            {
                impact->addIOs(this);
            }
        }

        if (mAddStateVariable) {
            addControlIO("State", &mExpState, "bool", &mHistState); /* state of the component : 1 if on 0 if off */
        }

        if (mEcoInvestModel) {
            setCapexExpression("Capex");        // defines default expression to be used for OptimalSize computation and use in Economic analysis
            setOpexExpression("Opex");          // defines default expression to be used for OptimalSize computation and use in Economic analysis
            setPureOpexExpression("PureOpex");  // defines default expression to be used for OptimalSize computation and use in Economic analysis
            setReplacementExpression("Replacement");  // defines default expression to be used for OptimalSize computation and use in Economic analysis
        }

        if (mEnvironmentModel) {
            for (int i = 0; i < mEnvImpactsList.size(); i++)
            {
                setEnvImpactCostExpression(mEnvImpactsList[i] + " Env impact cost");  // defines default expression to be used for OptimalSize computation and use in Economic analysis
                setEnvImpactMassExpression(mEnvImpactsList[i] + " Env impact mass");  // defines default expression to be used for OptimalSize computation and use in Environmental analysis
                setEnvGreyImpactCostExpression(mEnvImpactsList[i] + " Env grey impact cost");  // defines default expression to be used for OptimalSize computation and use in Economic analysis
                setEnvGreyImpactMassExpression(mEnvImpactsList[i] + " Env grey impact mass");  // defines default expression to be used for OptimalSize computation and use in Environmental analysis
            }
        }
    }

    virtual void declareDefaultModelIndicators()
    {
        QString currency = mParentCompo->Currency();
        mInputIndicators->addIndicator("Total Cost Function", &mTotalCostFunction, &mExportIndicators, "Total contribution of the component", currency, "TotCosts"); /** Contribution of the component to objective function */
        mInputIndicators->addIndicator("Capex part", &mCapexContribution, &mEcoInvestModel, "CAPEX part", currency, "CAPEX");  /**  */
        mInputIndicators->addIndicator("is installed", &mExistence, &mExportIndicators, "Component installed", "-", "IsInstalled");
        mInputIndicators->addIndicator("Opex part (including energy cost)", &mOpexContribution, &mExportIndicators, "Opex part including energy costs", currency, "OPEX");  /**  */
        mInputIndicators->addIndicator("Pure Opex part", &mPureOpexContribution, &mEcoInvestModel, "Opex part excluding energy costs", currency, "PureOPEX");					/**  */
        mInputIndicators->addIndicator("Replacement part", &mReplacementPart, &mEcoInvestModel, "Replacement part", currency, "Replacement");              /**  */

        mInputIndicators->addIndicator("Mass", &mMassContribution, &mGeometryModel, "Mass", "kg", "Mass");
        mInputIndicators->addIndicator("Area", &mAreaContribution, &mGeometryModel, "Area", "m2", "Area");
        mInputIndicators->addIndicator("Volume", &mVolumeContribution, &mGeometryModel, "Volume", "m3", "Volume");

        for (int i = 0; i < mEnvImpacts.size(); i++)
        {
            mEnvImpacts[i]->addIndicators(mInputIndicators, &mEnvironmentModel, currency);
        }
    }

    void addMinimumCapacity(double& aSizeMax); 

    int checkConsistency();

    std::vector<double> getCapexContribution() { return mCapexContribution; }

    void computePiecewiseContribution(const MIPModeler::MIPData1D& aCapacitySetPoint, const MIPModeler::MIPData1D& aCostSetPoint, 
        const bool& aTryRelaxation, const double& aOffset, MIPModeler::MIPExpression& aExp);

    bool EnvironmentModel() { return mEnvironmentModel; }
    bool EcoInvestModel() { return mEcoInvestModel; }

    int getNbPorts(const QString& direction) {
        int nbPorts = 0;
        foreach(MilpPort * lptrport, mListPort)
        {
            if (lptrport->Direction() == direction)
            {
                nbPorts++;
            }
        }
        return nbPorts;
    }

protected:
    /** Flags */
    bool mEcoInvestModel;               /** bool indicating use of Economic Model if = true - default to true*/
    bool mEnvironmentModel;             /** bool indicating use of Environment Model if = true - default to true*/
    bool mGeometryModel;                /** bool indicating use of Geometric Model if = true - default to false*/

    /** if another temporal profile is forecasted over the long-term */
    bool mSeasonalPrevisions;

    /** Economic model*/
    MIPModeler::MIPVariable0D mInvest;      /** MILP 0D Variable on component Optimal capacity (power or storage) */
    MIPModeler::MIPData1D mCapexCapacitySetPoint;
    MIPModeler::MIPData1D mCapexSetPoint;
    bool mPiecewiseCapex;
    bool mTryRelaxationCapex;
    bool mAddMinimumCapacity;
    double mMinSize;            /**  minimum storage capacity for size optimization (kg mass on fluid vectors, MWh energy for electrical and thermal vectors) */


    /** Expressions */
    MIPModeler::MIPExpression mExpCapex;                        /** Capex contribution expression */
    MIPModeler::MIPExpression1D mExpOpex;                       /** Net Opex contribution expression, pureOpex + replacement + varcost */
    MIPModeler::MIPExpression1D mExpPureOpex;                   /** Pure Opex contribution expression, in fraction of Capex */
    MIPModeler::MIPExpression1D mExpReplacement;                /** Variable Replacement contribution expression, in Capex/h    */
    MIPModeler::MIPExpression1D mExpVariableCosts;    /** Variable costs contribution expression   */

    MIPModeler::MIPExpression mExpArea;
    MIPModeler::MIPExpression mExpVolume;
    MIPModeler::MIPExpression mExpMass;

    MIPModeler::MIPExpression1D mExpAuxiliaryPower;  /** if AddAuxiliaryPower : model output expressions */ //Only ProductionUC and ThermalGroup

    /** Indicators calculation */
    //Vector of two elements: first one is _PLAN and second one is HIST
    std::vector<double> mOptimalSize;
    std::vector<double> mTotalCostFunction;
    std::vector<double> mCapexContribution;
    std::vector<double> mExistence;
    std::vector<double> mOpexContribution;
    std::vector<double> mPureOpexContribution;
    std::vector<double> mReplacementPart;
    std::vector<double> mVariableCosts;
    std::vector<double> mEnvImpactPart;
    std::vector<double> mEnvGreyImpactCost;
    //

    double mHistPureOpexContributionDiscounted;
    double mHistReplacementPartDiscounted;

    std::vector<double> mSumUp;
    std::vector<double> mRunningTime;
    std::vector<double> mRunningTimeAvlblt;
    std::vector<double> mMaxRunningTime;
    std::vector<double> mChargingTime;
    std::vector<double> mDischargingTime;
    std::vector<double> mEfficiency_Ageing;
    QMap<QString, std::vector<double>> mExpEchData;
    QMap<QString, std::vector<double>> mConsumptionMap;
    QMap<QString, std::vector<double>> mConsLvlTotMap;
    QMap<QString, std::vector<double>> mConsPFMap;
    QMap<QString, std::vector<double>> mRateOfUse;
    QMap<QString, std::vector<double>> mConsMeanMap;
    QMap<QString, std::vector<double>> mProductionMap;
    QMap<QString, std::vector<double>> mProdLvlTotMap;
    QMap<QString, std::vector<double>> mProdMeanMap;
    QMap<QString, std::vector<double>> mProdContributionMap;
    QMap<QString, std::vector<double>> mChargedEnergyMap;
    QMap<QString, std::vector<double>> mDischargedEnergyMap;
    QMap<QString, std::vector<double>> mNLevChargedEnergyMap;
    QMap<QString, std::vector<double>> mNLevDischargedEnergyMap;
    QMap<QString, std::vector<double>> mChargedMeanMap;
    QMap<QString, std::vector<double>> mDischargedMeanMap;
    QMap<QString, std::vector<double>> mNbCylesMap;

    /** footprint optional attributes (geometry Model) */
    double mArea;
    double mVolume;
    double mMass;

    std::vector<double> mAreaContribution;
    std::vector<double> mVolumeContribution;
    std::vector<double> mMassContribution;

    MIPModeler::MIPData1D mAreaCapacitySetPoint;
    MIPModeler::MIPData1D mAreaSetPoint;
    MIPModeler::MIPData1D mVolumeCapacitySetPoint;
    MIPModeler::MIPData1D mVolumeSetPoint;
    MIPModeler::MIPData1D mMassCapacitySetPoint;
    MIPModeler::MIPData1D mMassSetPoint;

    bool mTryRelaxationArea;
    bool mTryRelaxationVolume;
    bool mTryRelaxationMass;

    bool mPiecewiseArea;
    bool mPiecewiseVolume;
    bool mPiecewiseMass;
 
    /** Economic model input data */
    double mCapex;                             /** Capex Component Elementary Capex per unit of max installed capacity (production or storage) mVarSizeMax - SourceLoad unit cost of component*/
    double mTotalCapexCoefficient;             /** Capex multiplicative factor on Elementary Capex to account for fees, infrastructures, ... */
    double mTotalCapexOffset;                  /** Capex constant on Elementary Capex to account for fees, infrastructures, ... */

    //Opex = mOpex * mCapex + mOpexConstant and Replacement = mReplacement * mCapex + mReplacementConstant
    double mOpex;                              /** Opex yearly Opex of component, per unit Capex **/
    double mReplacement;                       /** Replacement cost, in proportion of Capex **/
    double mLifeTime;                           /** Component LifeTime in years **/
    double mOpexConstant;                       /** The constant part of the yearly Opex : Opex = mOpex * mCapex + mOpexConstant **/
    double mReplacementConstant;                /** The constant part of the Replacement cost : mReplacement * mCapex + mReplacementConstant**/
};

#endif // TechnicalSubModel_H