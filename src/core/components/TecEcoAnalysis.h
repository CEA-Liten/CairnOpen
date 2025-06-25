#ifndef TECECOANALYSISCOMPO_H
#define TECECOANALYSISCOMPO_H

#include <QtCore>
#include "InputParam.h"
#include "MIPModeler.h"
#include "GUIData.h"
#include "EnvImpact.h"

#include "CairnCore_global.h"
#include "TechnicalSubModel.h"

class CAIRNCORESHARED_EXPORT TecEcoAnalysis : public TechnicalSubModel
{
    Q_OBJECT
public:
    TecEcoAnalysis(QObject* aParent, const QMap<QString, QString> &aComponent={});
    virtual ~TecEcoAnalysis();

    void declareEnvImpactParam();
    void declareConfigurationParameters();
    int setConfigurationParameters(const QMap<QString, QString>& aComponent);

    InputParam* getConfigParam() { return mConfigParam; }
    InputParam* getCompoInputParam() { return mCompoInputParam; }  /** Get access to Model Parameters */
    InputParam* getCompoInputSettings() { return mCompoInputSettings; }  /** Get access to Model Parameters */
    InputParam* getCompoEnvImpactsParam() { return mCompoEnvImpactsParam; }
    QString getParameter(QString paramName) { return mCompoInputParam->getParamQSValue(paramName); }
    std::map<QString, InputParam::ModelParam*> getParameters();

    void jsonSaveGuiComponent(QJsonArray& componentsArray);

    QString Name() const {return mName;}
    QString Model() const {return mModelName;}
    QString Currency() const {return mCurrency;}
    QString ObjectiveUnit() const { return mObjectiveUnit; }
    QString Range() const {return mRange;}
    int NbYear() const {return mNbYear;}
    int NbYearOffset() const {return mNbYearOffset;}
    int NbYearInput() const {return mNbYearInput;}
    int LeapYearPos() const {return mLeapYearPos ;}
    double DiscountRate () const {return mDiscountRate;}
    double ImpactDiscountRate() const { return mImpactDiscountRate; }
    double InternalRateOfReturn() const {return mInternalRateOfReturn;}

    bool ForceExportAllIndicators() const { return mForceExportAllIndicators; }

    QStringList EnvImpactsList() const { return mEnvImpacts; }
    QStringList EnvImpactUnitsList() const { return mEnvImpactUnits; }
    std::vector<double> EnvImpactCosts();
    QVector<bool> EnvImpactConstraints() const { return mVBEnvImpactMaxConstraint; } //TODO: delete this method
    std::vector<double> EnvImpactMaxConstraints() const { return mVDEnvImpactMaxConstraint; } //TODO: delete this method
    QStringList getPossibleImpactNames() const;
    QStringList getPossibleImpactShortNames() const;
    QStringList getImpactUnitsFromSelectedImpacts(const QStringList& aSelectedImpactsList);
    QString getImpactShortName(const QString& name);
    QString getImpactLongName(const QString& name);
    int getImpactIndex(const QString& impactName); 

    void updateEnvImpactUnitsList() { mEnvImpactUnits = getImpactUnitsFromSelectedImpacts(mEnvImpacts); }

    // ******************************************* CompoModel ************************************************ //

    void buildModel();
    void computeAllIndicators(const double* optSol);
    void setTimeData();
    //----------------------------------------------------------------------------------------------------
    void declareModelConfigurationParameters()
    {
        mInputParam->addToConfigList({ "EcoInvestModel","EnvironmentModel" });

        TechnicalSubModel::declareDefaultModelConfigurationParameters();

        //Re-declare parameter "EcoInvestModel" in order to set its default value to false 
        addParameter("EcoInvestModel", &mEcoInvestModel, false, false, true, "Use EcoInvestModel - ie Use Capex and Opex if true", "", { "" });    /** Use EcoInvestModel - ie Use Capex and Opex if true */

        //bool
        addParameter("MinConstraint", &mMinConstraint, false, false, true);   /** MinConstraint option enabling */
        addParameter("MaxConstraint", &mMaxConstraint, false, false, true);   /** MaxConstraint option enabling */
    }

    void declareModelParameters()
    {
        TechnicalSubModel::declareDefaultModelParameters();
        //double
        addParameter("MinConstraintValue", &mMinConstraintValue, 0., &mMinConstraint, &mMinConstraint); /**  NPV > Minconstraint value */
        addParameter("MaxConstraintValue", &mMaxConstraintValue, INFINITY_VAL, &mMaxConstraint, &mMaxConstraint); /**  NPV < Maxconstraint value */
    }

    void declareModelInterface()
    {
        //TecEco total values
        if (mEnergyVector != nullptr) {
            addIO("Total Capex", &mExpCapex, mEnergyVector->pFluxUnit()); /** Computed initial investment costs Total Capex */
            addIO("Total Undiscounted Opex", &mExpOpexUndiscounted, mEnergyVector->pStorageUnit());     /** Computed operational cost Total Undiscounted Net Opex */
        }
        else {
            addIO("Total Capex", &mExpCapex, "FluxUnit");             /** Computed initial investment costs Total Capex */
            addIO("Total Undiscounted Opex", &mExpOpexUndiscounted, "StorageUnit");     /** Computed operational cost Total Undiscounted Net Opex */
        }

        addIO("Total Undiscounted VariableCosts", &mExpVariableCostsUndiscounted, mCurrency); /** Computed Total undiscounted variable costs resulting from material/fuel consumption */

        /*
            mExpPenaltyConstraintCosts cannot be exported because PenaltyConstraintCosts is a Bus expression
        */

        resizeEnvImpactVectors();

        for (int i = 0; i < mEnvImpactsList.size(); i++)
        {
            addIO("Total Undiscounted " + mEnvImpactsList[i] + " EnvImpact Mass", &mExpEnvImpactMassUndiscountedVec[i], mEnvImpactUnitsList[i]); /** "TecEco undiscounted mName Env impact  mass" */
            addIO("Total Undiscounted " + mEnvImpactsList[i] + " Grey EnvImpact Mass", &mExpEnvGreyImpactMassUndiscountedVec[i], mEnvImpactUnitsList[i]); /** "TecEco undiscounted mName Env grey impact mass" */
        }
    }

    void declareModelIndicators()
    {
        mInputIndicators->addIndicator("NbYear", &mNbYearIndicator, &mExportIndicators, "NbYear", "year", "NbYear");
        mInputIndicators->addIndicator("Discount Rate", &mDiscountRateIndicator, &mExportIndicators, "Discount Rate", "-", "DiscountRate");
        mInputIndicators->addIndicator("Impact Discount Rate", &mImpactDiscountRateIndicator, &mExportIndicators, "Impact Discount Rate", "-", "ImpactDiscountRate");
        mInputIndicators->addIndicator("NbYears in TS", &mNbYearInputIndicator, &mExportIndicators, "NbYears in TS", "year", "NbYearsinTS");
        mInputIndicators->addIndicator("Leap Year Position", &mLeapYearPosIndicator, &mExportIndicators, "Leap Year Position", "-", "LeapYearPosition");
        mInputIndicators->addIndicator("Payback period offset", &mNbYearOffsetIndicator, &mExportIndicators, "Payback period offset", "-", "PaybackPeriodOffset");
        mInputIndicators->addIndicator("Discount Factor", &mDiscountFactorIndicator, &mExportIndicators, "Discount Factor", "-", "DiscountFactor");
        if (mParentCompo->LevelizationTable().size() > 1) {
            mDiscountFactorListIndicator.resize(mParentCompo->LevelizationTable().size(), { 0, 0 });
            for (int i = 0; i < mParentCompo->LevelizationTable().size(); i++) {
                mInputIndicators->addIndicator("Discount Factor " + QString::number(i + 1), &mDiscountFactorListIndicator.at(i), &mExportIndicators, "Discount Factor " + QString::number(i + 1), "-", "DiscountFactor" + QString::number(i + 1));
            }
        }
        mInputIndicators->addIndicator("Impact Discount Factor", &mImpactDiscountFactorIndicator, &mExportIndicators, "Impact Discount Factor", "-", "ImpactDiscountFactor");
        if (mParentCompo->ImpactLevelizationTable().size() > 1) {
            mImpactDiscountFactorListIndicator.resize(mParentCompo->ImpactLevelizationTable().size(), { 0, 0 });
            int size = mParentCompo->ImpactLevelizationTable().size();
            for (int i = 0; i < mParentCompo->ImpactLevelizationTable().size(); i++) {
                mInputIndicators->addIndicator("Impact Discount Factor " + QString::number(i + 1), &mImpactDiscountFactorListIndicator.at(i), &mExportIndicators, "Impact Discount Factor " + QString::number(i + 1), "-", "ImpactDiscountFactor" + QString::number(i + 1));
            }
        }
        mInputIndicators->addIndicator("Extrapolation Factor", &mExtraFactorIndicator, &mExportIndicators, "Extrapolation Factor (only important for PLAN)", "-", "ExtrapolationFactor");
        //
        QString ObjectiveUnit = mParentCompo->ObjectiveUnit();
        mInputIndicators->addIndicator("OBJECTIVE", &mObjectiveContribution, &mExportIndicators, "OBJECTIVE", ObjectiveUnit, "Objective");
        mInputIndicators->addIndicator("Net Present Value (Levelized Profit)", &mNetPresentValue, &mExportIndicators, "Net Present Value (Levelized Profit)", mCurrency, "NPV");
        //
        if (mParentCompo->InternalRateOfReturn() >= 0.) {
            mInputIndicators->addIndicator("Imposed Internal Rate Of Return PerCent", &mInternalRateOfReturnPerCent, &mExportIndicators, "Imposed Internal Rate Of Return PerCent", "%", "ImposedInternalRateOfReturnPerCent");
            mInputIndicators->addIndicator("Imposed Internal Rate Of Return Factor", &mInternalRateOfReturnFactor, &mExportIndicators, "Imposed Internal Rate Of Return Factor", "-", "ImposedInternalRateOfReturnFactor");
            mInputIndicators->addIndicator("Net Present Value at Internal Rate Of Return", &mNetPresentValueAtIRR, &mExportIndicators, "Net Present Value at Internal Rate Of Return", mCurrency, "NetPresentValueAtInternalRateOfReturn");
        }
        mInputIndicators->addIndicator("Computed Average Rate Of Return Factor", &mAverageRateOfReturnFactor, &mExportIndicators, "Computed Average Rate Of Return Factor", "-", "ComputedAverageRateOfReturnFactor");
        mInputIndicators->addIndicator("Computed Average Rate Of Return perCent", &mAverageRateOfReturnPerCent, &mExportIndicators, "Computed Average Rate Of Return perCent", "%", "ComputedAverageRateOfReturnPerCent");
        mInputIndicators->addIndicator("Current Rate Of Return perCent", &mCurrentRateOfReturnPerCent, &mExportIndicators, "Current Rate Of Return perCent", "%", "CurrentRateOfReturnperCent");
        //
        mInputIndicators->addIndicator("Total SubObjective (Type Add)", &mSubObjectiveContribution, &mExportIndicators, "Sum of SubObjective (Type Add)", ObjectiveUnit, "SubObjective");
        mInputIndicators->addIndicator("Total CAPEX", &mCapexContribution, &mExportIndicators, "Total CAPEX", mCurrency, "CAPEX");
        mInputIndicators->addIndicator("Total PenaltyConstraintContribution", &mPenaltyConstraintContribution, &mExportIndicators, "Total PenaltyConstraintContribution", "-", "PenaltyPart");
        mInputIndicators->addIndicator("Total Discounted Net OPEX", &mOpexContributionDiscounted, &mExportIndicators, "Total Discounted Net OPEX", mCurrency, "DiscountedNetOPEX");
        mInputIndicators->addIndicator("Total Discounted Pure OPEX", &mPureOpexContributionDiscounted, &mExportIndicators, "Total Discounted Pure OPEX", mCurrency, "DiscountedPureOPEX");
        mInputIndicators->addIndicator("Total Discounted Replacement Part", &mReplacementContributionDiscounted, &mExportIndicators, "Total Discounted Replacement Part", mCurrency, "DiscountedReplacement");
        mInputIndicators->addIndicator("Total Discounted Sell Part", &mSellVariableCostsContributionDiscounted, &mExportIndicators, "Total Discounted Sell Part", mCurrency, "DiscountedSellPart");
        mInputIndicators->addIndicator("Total Discounted Buy Part", &mBuyVariableCostsContributionDiscounted, &mExportIndicators, "Total Discounted Buy Part", mCurrency, "DiscountedBuyPart");

        for (int i = 0; i < mEnvImpactsList.size(); i++) {
            QString aImpactShortName = mParentCompo->EnvImpactsShortNamesList().at(i);
            mInputIndicators->addIndicator("Total Discounted EnvImpact Part (Flow and Grey) " + mEnvImpactsList[i], &mVDEnvImpactsTotalCostDiscounted[i], &mExportIndicators, "Total Discounted EnvImpact Part (Flow and Grey) " + mEnvImpactsList[i], mCurrency, aImpactShortName + "DiscountedEnvImpactPart");
            mInputIndicators->addIndicator("Total Discounted EnvImpact Mass (Flow and Grey) " + mEnvImpactsList[i], &mVDEnvImpactsTotalMassDiscounted[i], &mExportIndicators, "Total Discounted EnvImpact Mass (Flow and Grey) " + mEnvImpactsList[i], mEnvImpactUnitsList[i], aImpactShortName + "DiscountedEnvImpactMass");
        }

        for (int i = 0; i < mEnvImpactsList.size(); i++) {
            QString aImpactShortName = mParentCompo->EnvImpactsShortNamesList().at(i);
            mInputIndicators->addIndicator("Total GreyEnvImpact Part " + mEnvImpactsList[i], &mVDEnvGreyImpactsCostContribution[i], &mExportIndicators, "Total GreyEnvImpact Part " + mEnvImpactsList[i], mCurrency, aImpactShortName + "GreyEnvImpactPart");
            mInputIndicators->addIndicator("Total GreyEnvImpact Mass " + mEnvImpactsList[i], &mVDEnvGreyImpactsMassContribution[i], &mExportIndicators, "Total GreyEnvImpact Mass " + mEnvImpactsList[i], mEnvImpactUnitsList[i], aImpactShortName + "GreyEnvImpactMass");
        }
        for (int i = 0; i < mEnvImpactsList.size(); i++) {
            QString aImpactShortName = mParentCompo->EnvImpactsShortNamesList().at(i);
            mInputIndicators->addIndicator("Total Discounted FlowEnvImpact Part " + mEnvImpactsList[i], &mVDEnvImpactsCostContributionDiscounted[i], &mExportIndicators, "Total Discounted FlowEnvImpact Part " + mEnvImpactsList[i], mCurrency, aImpactShortName + "DiscountedFlowEnvImpactPart");
            mInputIndicators->addIndicator("Total Discounted FlowEnvImpact Mass " + mEnvImpactsList[i], &mVDEnvImpactsMassContributionDiscounted[i], &mExportIndicators, "Total Discounted FlowEnvImpact Mass " + mEnvImpactsList[i], mEnvImpactUnitsList[i], aImpactShortName + "DiscountedFlowEnvImpactMass");
        }

        mInputIndicators->addIndicator("Total Undiscounted Net OPEX", &mOpexContribution, &mExportIndicators, "Total Undiscounted Net OPEX", mCurrency, "NetOPEX");
        mInputIndicators->addIndicator("Total Undiscounted Pure OPEX", &mPureOpexContribution, &mExportIndicators, "Total Undiscounted Pure OPEX", mCurrency, "PureOPEX");
        mInputIndicators->addIndicator("Total Undiscounted Replacement Part", &mReplacementContribution, &mExportIndicators, "Total Undiscounted Replacement Part", mCurrency, "ReplacementPart");
        mInputIndicators->addIndicator("Total Undiscounted Sell Part", &mSellVariableCostsContribution, &mExportIndicators, "Total Undiscounted Sell Part", mCurrency, "SellPart");
        mInputIndicators->addIndicator("Total Undiscounted Buy Part", &mBuyVariableCostsContribution, &mExportIndicators, "Total Undiscounted Buy Part", mCurrency, "BuyPart");

        for (int i = 0; i < mEnvImpactsList.size(); i++)
        {
            QString aImpactShortName = mParentCompo->EnvImpactsShortNamesList().at(i);
            mInputIndicators->addIndicator("Total Undiscounted FlowEnvImpact Part " + mEnvImpactsList[i], &mVDEnvImpactsCostContribution[i], &mExportIndicators, "Total Undiscounted FlowEnvImpact Part " + mEnvImpactsList[i], mCurrency, aImpactShortName + "FlowEnvImpactPart");
            mInputIndicators->addIndicator("Total Undiscounted FlowEnvImpact Mass " + mEnvImpactsList[i], &mVDEnvImpactsMassContribution[i], &mExportIndicators, "Total Undiscounted FlowEnvImpact Mass " + mEnvImpactsList[i], mEnvImpactUnitsList[i], aImpactShortName + "FlowEnvImpactMass");
        }
    }
    //----------------------------------------------------------------------------------------------------
    void resizeEnvImpactVectors();
    void allocateExpressions();
    void closeExpressions();

    void computeAllContribution();   /** MILP Model description : all expressions */
    void computeEconomicalContribution(); /** MILP Model description : objective contribution */
    void computeEnvContribution();       /** MILP Model description : environment constraints */

    void computePenaltyConstraintCosts();
    void computeBuyAndSellExpressions(const double* optSol, MIPModeler::MIPExpression1D& expBuyPart, MIPModeler::MIPExpression1D& expSellPart);
    void computeUndiscountedExpressions();

    double objective(const int& i) { return mObjectiveContribution.at(i); } // i in {1, 2}
    MIPModeler::MIPExpression objectiveExpression() { return mExpObjective; }
    QMap<QString, MIPModeler::MIPExpression*> mapSubObjective() { return mMapSubObjective; }

    void initDefaultPorts() { };

private :
    void doInit(const QMap<QString, QString>& aComponent);
    void declareCompoInputParam();
    int setCompoInputParam(const QMap<QString, QString>& aComponent);

    GUIData* mGUIData{ nullptr };       /** Pointer to GUI Data */

    InputParam* mConfigParam;          /** Config parameters should be read first */
    InputParam* mCompoInputParam ;     /** COMPONENT Input parameter List from XML File -> Options */
    InputParam* mCompoInputSettings ;  /** COMPONENT Input parameter List from Settings File -> Params */
    InputParam* mCompoEnvImpactsParam; /** COMPONENT Input parameter List for all the environmental impacts considered */
    
    QString mName;
    QString mType; 
    QString mModelName;
    QString mOptimImpactName; //In case of Model Type = OptimEnvImpact

    int mXpos;
    int mYpos;

    QString mCurrency ;     /** Currency unit - default to EUR */
    QString mObjectiveUnit; /** Objective unit - default to currency unit */
    QString mRange ;        /** Evaluation range used for TecEco analysis : HIST = past operation, PLAN = planned operation */
    int mNbYear ;           /** Number of year for economic data extrapolation */
    int mNbYearOffset ;     /** Offset of nb of year for discount cost computation */
    int mNbYearInput ;      /** Number of years in the input time series */
    int mLeapYearPos ;      /** Position of the leap in the input time series if there is one (0 if not, 1 if it is the first year, ...) */
    double mDiscountRate ;  /** Discount Rate */
    double mImpactDiscountRate;          /** Discount Rate for Env Impacts*/
    double mInternalRateOfReturn ;         /** Target Internal Rate of Return chosen by the user*/
    double mRateOfReturnDiscountFactor ; /** Average Rate of Return */
    bool mForceExportAllIndicators;
    
    std::vector<EnvImpact> mPossibleImpacts;  /** List of possible impacts */
    QStringList mEnvImpacts;                  /** List of selected impacts */
    QStringList mEnvImpactUnits;             /** List of units of selected impacts */
    QVector<bool> mVBEnvImpactMaxConstraint;
    std::vector<double> mVDEnvImpactMaxConstraint;
    std::vector<double> mVDEnvImpactCost;

    // ******************************************* CompoModel ************************************************ //

    bool mMinConstraint;             /** Use Min inequality of instantaneous value as aggregation constraint model - Default to false*/
    bool mMaxConstraint;             /** Use Max inequality of instantaneous value as aggregation constraint model - Default to false*/

    double mMinConstraintValue;      /** Instantaneous Min constraint value, default to 0 to perform bus balance */
    double mMaxConstraintValue;      /** Instantaneous Max constraint value, default to 0 to perform bus balance */

    /** ---------------------------- Expressions ------------------------------------------- */

    MIPModeler::MIPExpression mExpObjective;
    MIPModeler::MIPExpression mExpNegNPV;  //Global cost of the project (== - Net Present Value  = discountedCAPEX + discountedPureOPEX + other costs - Revenues)

    MIPModeler::MIPExpression mExpSubObjective;  //Sum of the SubObjectives of MultiObjectives with Type == Add
    QMap<QString, MIPModeler::MIPExpression*> mMapSubObjective;  //List of SubObjectives (all types); used to write in _results_multiObj.csv

    MIPModeler::MIPExpression  mExpPenaltyConstraintCosts;   //A sum of all NodeLaw::mExpPenaltyConstraintCosts

    /** Expressions already esist in TechnicalSubModel (and can be used to carry TecEco (total) contribution)
        MIPModeler::MIPExpression mExpCapex;
        MIPModeler::MIPExpression1D mExpOpex;
        MIPModeler::MIPExpression1D mExpPureOpex;
        MIPModeler::MIPExpression1D mExpReplacement;

        MIPModeler::MIPExpression1D mExpVariableCosts;

        Note that, mExpVariableCosts is not used to compute Buy and Sell parts.
        It is used to compute mExpVariableCostsUndiscounted which is published on ports
        There is an assertion to enure the equality.
    */

    std::vector<MIPModeler::MIPExpression> mExpEnvGreyImpactCostVec;
    std::vector<MIPModeler::MIPExpression> mExpEnvGreyImpactMassVec;
    std::vector<MIPModeler::MIPExpression1D> mExpEnvImpactCostVec;
    std::vector<MIPModeler::MIPExpression1D> mExpEnvImpactMassVec;

    /* Expressions published on ports */
    MIPModeler::MIPExpression1D mExpOpexUndiscounted;
    MIPModeler::MIPExpression1D mExpVariableCostsUndiscounted;
    std::vector<MIPModeler::MIPExpression> mExpEnvGreyImpactMassUndiscountedVec;
    std::vector<MIPModeler::MIPExpression1D> mExpEnvImpactMassUndiscountedVec;

    /* 0D-Expressions used to compute Objective and NPV, and to add constraints.
       Not used to compute the related total discounted contributions.
       Discounted Opex and Discounted EnvImpactMass contributions are computed from mExpOpex and mExpEnvImpactMassVec.
       There are assertions to ensure that the evaluation of mExpOpexDiscounted and mExpEnvImpactMassVecDiscounted results in the same values.
     */
    MIPModeler::MIPExpression mExpOpexDiscounted;
    std::vector<MIPModeler::MIPExpression> mExpEnvImpactMassVecDiscounted;

    /** ------------------------------- Contributions ------------------------------------ */

    //Factors
    std::vector<double> mNbYearIndicator = { 0., 0. };
    std::vector<double> mDiscountRateIndicator = { 0., 0. };
    std::vector<double> mImpactDiscountRateIndicator = { 0., 0. };
    std::vector<double> mNbYearInputIndicator = { 0., 0. };
    std::vector<double> mLeapYearPosIndicator = { 0., 0. };
    std::vector<double> mNbYearOffsetIndicator = { 0., 0. };
    std::vector<double> mExtraFactorIndicator = { 0., 0. };
    std::vector<double> mDiscountFactorIndicator = { 0., 0. };
    std::vector<std::vector<double>> mDiscountFactorListIndicator = { { 0., 0. } };
    std::vector<double> mImpactDiscountFactorIndicator = { 0., 0. };
    std::vector<std::vector<double>> mImpactDiscountFactorListIndicator = { { 0., 0. } };

    //Objective
    std::vector<double> mObjectiveContribution = { 0., 0. };   /** Resulting optimal contribution to Objective function */
    std::vector<double> mNetPresentValue = { 0., 0. };        /** Net Present Value(Levelized Profit) */
    std::vector<double> mSubObjectiveContribution = { 0., 0. };         /** Sum of all SubObjectives of Type Add */
    std::vector<double> mPenaltyConstraintContribution = { 0., 0. };

    std::vector<double>  mInternalRateOfReturnPerCent = { 0., 0. };
    std::vector<double>  mInternalRateOfReturnFactor = { 0., 0. };
    std::vector<double>  mNetPresentValueAtIRR = { 0., 0. };

    std::vector<double>  mAverageRateOfReturnFactor = { 0., 0. };
    std::vector<double>  mAverageRateOfReturnPerCent = { 0., 0. };
    std::vector<double>  mCurrentRateOfReturnPerCent = { 0., 0. };

    // undiscounted 
    std::vector<double> mCapexContribution = { 0., 0. };              /** Resulting Capex contribution */
    std::vector<double> mOpexContribution = { 0., 0. };               /** Resulting Opex contribution including VariableCosts due to energy costs*/
    std::vector<double> mPureOpexContribution = { 0., 0. };           /** Resulting Pure Opex contribution */
    std::vector<double> mReplacementContribution = { 0., 0. };        /** Resulting Replacement contribution */
    std::vector<double> mBuyVariableCostsContribution = { 0., 0. };   /** Resulting Variable Cost expenses contribution part*/
    std::vector<double> mSellVariableCostsContribution = { 0., 0. };  /** Resulting Variable Cost revenue contribution part*/

    // discounted 
    std::vector<double> mOpexContributionDiscounted = { 0., 0. };                  /** Resulting Opex contribution including VariableCosts due to energy costs*/
    std::vector<double> mPureOpexContributionDiscounted = { 0., 0. };             /** Resulting Pure Opex contribution*/
    std::vector<double> mReplacementContributionDiscounted = { 0., 0. };         /** Resulting Replacement contribution part*/
    std::vector<double> mBuyVariableCostsContributionDiscounted = { 0., 0. };   /** Resulting Variable Cost expenses contribution part*/
    std::vector<double> mSellVariableCostsContributionDiscounted = { 0., 0. };  /** Resulting Variable Cost revenue contribution part*/

    // ----------- Env Impacts ------------- 
    // grey
    std::vector<std::vector<double>> mVDEnvGreyImpactsCostContribution; /** Resulting cost of all the Grey environmental impacts the user wants to consider*/
    std::vector<std::vector<double>> mVDEnvGreyImpactsMassContribution; /** Resulting cost of all the Grey environmental impacts the user wants to consider*/

    // undiscounted
    std::vector<std::vector<double>> mVDEnvImpactsCostContribution; /** Resulting cost of all the Direct environmental impacts the user wants to consider*/
    std::vector<std::vector<double>> mVDEnvImpactsMassContribution; /** Resulting mass of all the Direct environmental impacts the user wants to consider*/

    // discounted
    std::vector<std::vector<double>> mVDEnvImpactsCostContributionDiscounted;  /** Resulting EnvImpactCost contribution part*/
    std::vector<std::vector<double>> mVDEnvImpactsMassContributionDiscounted;  /** Resulting EnvImpactMass contribution part*/

    //Flow + Grey
    std::vector<std::vector<double>> mVDEnvImpactsTotalCostDiscounted;
    std::vector<std::vector<double>> mVDEnvImpactsTotalMassDiscounted;

};

#endif // TECECOANALYSISCOMPO_H
