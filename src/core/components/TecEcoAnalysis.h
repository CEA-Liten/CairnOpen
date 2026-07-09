#ifndef TECECOANALYSISCOMPO_H
#define TECECOANALYSISCOMPO_H

#include "CairnCore_global.h"
#include "TechnicalSubModel.h"
#include "InputParam.h"
#include "MIPModeler.h"
#include "GUIData.h"
#include "EnvImpact.h"

class CAIRNCORESHARED_EXPORT TecEcoAnalysis : public TechnicalSubModel
{
    
public:
    TecEcoAnalysis(CairnObject* aParent, const t_mapParamData& aComponent = {});
    virtual ~TecEcoAnalysis();

    void declareEnvImpactParam();

    void createEnvImpacts() override { /* do nothing */ }

    void resetFlags() {
        mAllocate = true;
    }

    InputParam* getConfigParam() { return mConfigParam; }
    InputParam* getCompoInputParam() { return mCompoInputParam; }  /** Get access to Model Parameters */
    InputParam* getCompoInputSettings() { return mCompoInputSettings; }  /** Get access to Model Parameters */
    InputParam* getCompoEnvImpactsParam() { return mCompoEnvImpactsParam; }
    std::map<std::string, ModelParam*> getParameters();
    InputParam* getInputIndicators() { return mInputIndicators; }

    void jsonSaveGuiComponent(ojson& componentsArray);

    std::string Model() const {return mModelName;}
    std::string Currency() const {return mCurrency;}
    const std::string* pCurrency() const  { return &mCurrency; };
    std::string ObjectiveUnit() const { return mObjectiveUnit; }

    int NbYear() const {return mNbYear;}
    int NbYearOffset() const {return mNbYearOffset;}
    int NbYearInput() const {return mNbYearInput;}
    int LeapYearPos() const {return mLeapYearPos ;}
    double DiscountRate () const {return mDiscountRate;}
    double ImpactDiscountRate() const { return mImpactDiscountRate; }
    double InternalRateOfReturn() const {return mInternalRateOfReturn;}

    bool ForceExportAllIndicators() const { return mForceExportAllIndicators; }

    std::vector<std::string> getPossibleImpactNames() const;
    std::vector<std::string> getPossibleImpactShortNames() const;

    std::vector<std::string> EnvImpactsList() const { return mSelectedEnvImpacts; }
    std::vector<std::string> EnvImpactUnitsList() const;
    std::vector<std::string> EnvImpactShortNamesList() const;
    std::vector<double> EnvImpactCosts();

    std::string EnvImpactUnit(const std::string& impactName) const;
    std::string EnvImpactShortName(const std::string& name) const;
    std::string EnvImpactLongName(const std::string& name);
    int getImpactIndex(const std::string& impactName); 

    //----------------------------------------------------------------------------------------------------
    void declareModelConfigurationParameters()
    {
    }

    void declareModelParameters()
    {
    }

    void declareModelInterface()
    {
        //TecEco total values
        if (mMainCarrier) {
            addIO("Total Capex", &mExpCapex, true, mMainCarrier->pFluxUnit()); /** Computed initial investment costs Total Capex */
            addIO("Total Undiscounted Opex", &mExpOpexUndiscounted, true, mMainCarrier->pStorageUnit());     /** Computed operational cost Total Undiscounted Net Opex */
        }
        else {
            addIO("Total Capex", &mExpCapex, true, "FluxUnit");             /** Computed initial investment costs Total Capex */
            addIO("Total Undiscounted Opex", &mExpOpexUndiscounted, true, "StorageUnit");     /** Computed operational cost Total Undiscounted Net Opex */
        }

        addIO("Total Undiscounted VariableCosts", &mExpVariableCostsUndiscounted, true, &mCurrency); /** Computed Total undiscounted variable costs resulting from material/fuel consumption */

        /*
            mExpPenaltyConstraintCosts should not be exported because PenaltyConstraintCosts is a Bus expression
            Similarly for mExpSubObjective
        */

        mExpEnvImpactMassUndiscountedVec.resize(mSelectedEnvImpacts.size());
        mExpEnvImpactEmbodiedUndiscountedVec.resize(mSelectedEnvImpacts.size());

        for (std::size_t i = 0; i < mSelectedEnvImpacts.size(); ++i) { //mSelectedEnvImpacts == SubModel::mEnvImpactsList
            const auto& impact = mSelectedEnvImpacts[i];
            const auto unit = EnvImpactUnit(impact);
            addIO("Total Undiscounted " + impact + " EnvImpact Mass", &mExpEnvImpactMassUndiscountedVec[i], true, unit); /** "TecEco undiscounted impactName Env impact  mass" */
            addIO("Total Undiscounted " + impact + " Embodied EnvImpact Mass", &mExpEnvImpactEmbodiedUndiscountedVec[i], true, unit); /** "TecEco undiscounted impactName Env grey impact mass" */
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
        if (mLevelizationTable.size() > 1) {
            mDiscountFactorListIndicator.resize(mLevelizationTable.size(), { 0, 0 });
            for (int i = 0; i < mLevelizationTable.size(); i++) {
                mInputIndicators->addIndicator("Discount Factor " + std::to_string(i + 1), &mDiscountFactorListIndicator.at(i), &mExportIndicators, "Discount Factor " + std::to_string(i + 1), "-", "DiscountFactor" + std::to_string(i + 1));
            }
        }
        mInputIndicators->addIndicator("Impact Discount Factor", &mImpactDiscountFactorIndicator, &mExportIndicators, "Impact Discount Factor", "-", "ImpactDiscountFactor");
        if (mImpactLevelizationTable.size() > 1) {
            mImpactDiscountFactorListIndicator.resize(mImpactLevelizationTable.size(), { 0, 0 });
            int size = mImpactLevelizationTable.size();
            for (int i = 0; i < mImpactLevelizationTable.size(); i++) {
                mInputIndicators->addIndicator("Impact Discount Factor " + std::to_string(i + 1), &mImpactDiscountFactorListIndicator.at(i), &mExportIndicators, "Impact Discount Factor " + std::to_string(i + 1), "-", "ImpactDiscountFactor" + std::to_string(i + 1));
            }
        }
        mInputIndicators->addIndicator("Extrapolation Factor", &mExtraFactorIndicator, &mExportIndicators, "Extrapolation Factor (only important for PLAN)", "-", "ExtrapolationFactor");
        //
        mInputIndicators->addIndicator("OBJECTIVE", &mObjectiveContribution, &mExportIndicators, "OBJECTIVE", &mObjectiveUnit, "Objective");
        mInputIndicators->addIndicator("Net Present Value (Levelized Profit)", &mNetPresentValue, &mExportIndicators, "Net Present Value (Levelized Profit)", &mCurrency, "NPV");
        //
        if (mInternalRateOfReturn >= 0.) {
            mInputIndicators->addIndicator("Imposed Internal Rate Of Return PerCent", &mInternalRateOfReturnPerCent, &mExportIndicators, "Imposed Internal Rate Of Return PerCent", "%", "ImposedInternalRateOfReturnPerCent");
            mInputIndicators->addIndicator("Imposed Internal Rate Of Return Factor", &mInternalRateOfReturnFactor, &mExportIndicators, "Imposed Internal Rate Of Return Factor", "-", "ImposedInternalRateOfReturnFactor");
        }

        mInputIndicators->addIndicator("Total SubObjective (Type Add)", &mSubObjectiveContribution, &mExportIndicators, "Sum of SubObjective (Type Add)", &mObjectiveUnit, "SubObjective");
        mInputIndicators->addIndicator("Total CAPEX", &mCapexContribution, &mExportIndicators, "Total CAPEX", &mCurrency, "CAPEX");
        mInputIndicators->addIndicator("Total PenaltyConstraintContribution", &mPenaltyConstraintContribution, &mExportIndicators, "Total PenaltyConstraintContribution", "-", "PenaltyPart");
        mInputIndicators->addIndicator("Total project discounted operation cost", &mOpexContributionDiscounted, &mExportIndicators, "Total project discounted operation cost (OPEX + replacement + buying cost + other cost - income)", &mCurrency, "DiscountedNetOPEX");
        mInputIndicators->addIndicator("Total project discounted OPEX", &mFixedOpexContributionDiscounted, &mExportIndicators, "Total project discounted OPEX", &mCurrency, "DiscountedPureOPEX");
        mInputIndicators->addIndicator("Total project replacement costs", &mReplacementContributionDiscounted, &mExportIndicators, "Total project replacement costs", &mCurrency, "DiscountedReplacement");
        mInputIndicators->addIndicator("Total project income", &mSellVariableCostsContributionDiscounted, &mExportIndicators, "Total project income", &mCurrency, "DiscountedSellPart");
        mInputIndicators->addIndicator("Total project buying costs", &mBuyVariableCostsContributionDiscounted, &mExportIndicators, "Total project buying cost", &mCurrency, "DiscountedBuyPart");

        mVDEnvImpactsTotalCostDiscounted.resize(mSelectedEnvImpacts.size(), { 0., 0. });
        mVDEnvImpactsTotalMassDiscounted.resize(mSelectedEnvImpacts.size(), { 0., 0. });
        mVDEmbodiedMassContribution.resize(mSelectedEnvImpacts.size(), { 0., 0. });
        mVDEnvImpactsCostContributionDiscounted.resize(mSelectedEnvImpacts.size(), { 0., 0. });
        mVDEnvImpactsMassContributionDiscounted.resize(mSelectedEnvImpacts.size(), { 0., 0. });
        mVDEnvImpactsCostContribution.resize(mSelectedEnvImpacts.size(), { 0., 0. });
        mVDEnvImpactsMassContribution.resize(mSelectedEnvImpacts.size(), { 0., 0. });

        for (std::size_t i = 0; i < mSelectedEnvImpacts.size(); ++i) {
            const std::string& impact= mSelectedEnvImpacts[i];
            mInputIndicators->addIndicator("Total project cost of env impact of " + impact, &mVDEnvImpactsTotalCostDiscounted[i], &mExportIndicators, "Total project cost of env impact of " + impact, &mCurrency, EnvImpactShortName(impact) + "DiscountedEnvImpactPart");
            mInputIndicators->addIndicator("Total Project env impact of " + impact, &mVDEnvImpactsTotalMassDiscounted[i], &mExportIndicators, "Total Project env impact of " + impact, EnvImpactUnit(impact), EnvImpactShortName(impact) + "DiscountedEnvImpactMass");
        }

        for (std::size_t i = 0; i < mSelectedEnvImpacts.size(); ++i) {
            const std::string& impact = mSelectedEnvImpacts[i];
            mInputIndicators->addIndicator( "Total Project Embodied env impact of " + impact, &mVDEmbodiedMassContribution[i], &mExportIndicators, "Total Project Embodied env impact of " + impact, EnvImpactUnit(impact), EnvImpactShortName(impact) + "GreyEnvImpactMass");
        }

        for (std::size_t i = 0; i < mSelectedEnvImpacts.size(); ++i) {
            const std::string& impact = mSelectedEnvImpacts[i];
            mInputIndicators->addIndicator("Total Project Operational impact cost of " + impact, &mVDEnvImpactsCostContributionDiscounted[i], &mExportIndicators, "Total Project Operational impact cost of " + impact, &mCurrency, EnvImpactShortName(impact) + "DiscountedFlowEnvImpactCost");
            mInputIndicators->addIndicator( "Total Project Operational impact of " + impact, &mVDEnvImpactsMassContributionDiscounted[i], &mExportIndicators, "Total Project Operational impact of " + impact, EnvImpactUnit(impact), EnvImpactShortName(impact) + "DiscountedFlowEnvImpactMass");
        }

        mInputIndicators->addIndicator("Annual operation cost", &mOpexContribution, &mExportIndicators, "Total annual operation cost (OPEX + replacement + buying cost + other cost - income)", &mCurrency, "NetOPEX");
        mInputIndicators->addIndicator("Annual OPEX", &mFixedOpexContribution, &mExportIndicators, "Total Undiscounted Pure OPEX", &mCurrency, "PureOPEX");
        mInputIndicators->addIndicator("Annual replacement costs", &mReplacementContribution, &mExportIndicators, "Total Undiscounted Replacement Part", &mCurrency, "ReplacementPart");
        mInputIndicators->addIndicator("Annual income", &mSellVariableCostsContribution, &mExportIndicators, "Annual income", &mCurrency, "SellPart");
        mInputIndicators->addIndicator("Annual buying costs", &mBuyVariableCostsContribution, &mExportIndicators, "Annual buying costs", &mCurrency, "BuyPart");

        for (std::size_t i = 0; i < mSelectedEnvImpacts.size(); ++i) {
            const std::string& impact = mSelectedEnvImpacts[i];
            mInputIndicators->addIndicator("Total Undiscounted FlowEnvImpact Part " + impact, &mVDEnvImpactsCostContribution[i], &mExportIndicators, "Total Undiscounted FlowEnvImpact Part " + impact, &mCurrency, EnvImpactShortName(impact) + "FlowEnvImpactPart" );
            mInputIndicators->addIndicator("Total Undiscounted FlowEnvImpact Mass " + impact, &mVDEnvImpactsMassContribution[i], &mExportIndicators, "Total Undiscounted FlowEnvImpact Mass " + impact, EnvImpactUnit(impact), EnvImpactShortName(impact) + "FlowEnvImpactMass" );
        }
    }
    //----------------------------------------------------------------------------------------------------
    void setTimeData();

    void allocateExpressions();
    void closeExpressions();

    void computeEconomicalContribution(); /** MILP Model description : objective contribution */
    void computeEnvContribution();       /** MILP Model description : environment constraints */
    void computeTecEcoContribution();   /** MILP Model description : all expressions */
    void buildTecEcoModel(); /* !!! Don't use buildModel() for TecEcoAnalysis */

    void computeAllIndicators(const double* optSol);

    double objective(const int& i) { return mObjectiveContribution.at(i); } // i in {1, 2}
    MIPModeler::MIPExpression objectiveExpression() { return mExpObjective; }

    void initDefaultPorts() { };

    void addLabel(const std::string& aLabel) { mLabelList.push_back(aLabel); };
    void removeLabel(const std::string& aLabel) {
        mLabelList.erase(std::remove(mLabelList.begin(), mLabelList.end(), aLabel), mLabelList.end());
    };
    const std::vector< std::string >& getLabelList() const { return mLabelList; };
    void setLabelList(const std::vector< std::string >& aLabelList) { mLabelList = aLabelList; };
    bool isValidLabel(const std::string& aLabel) const;

    void computeExtrapolationFactor(const MilpData* aMilpData);
    void computeLevelizationTable();

    double& ExtrapolationFactor() { return mExtrapolationFactor; };
    std::vector<double>& LevelizationTable() { return mLevelizationTable; }
    std::vector<double>& ImpactLevelizationTable() { return mImpactLevelizationTable; }
    std::vector<double>& TableYearsHours() { return mTableYearsHours; }

    int checkPortCount() override { return 0; }

private :
    void doInit(const t_mapParamData& aComponent);
    void declareCompoInputParam();
    void declareConfigurationParameters();
    void setConfigurationParameters(const t_mapParamData& aComponent);
    void setCompoInputParam(const t_mapParamData& aComponent);
    std::map<std::string, MilpComponent*> MilpComponents();
    std::vector<MilpComponent*> NonBusMilpComponents();
    std::vector<BusCompo*> BusComponents();
    void computeSubObjective();
    void computePenaltyConstraintCosts();
    void computeBuyAndSellExpressions(const double* optSol, MIPModeler::MIPExpression1D& expBuyPart, MIPModeler::MIPExpression1D& expSellPart);
    void computeUndiscountedExpressions();


    /*---------------------------------------------------*/

    double mExtrapolationFactor;

    std::vector< std::string > mLabelList;

    InputParam* mConfigParam;          /** Config parameters should be read first */
    InputParam* mCompoInputParam ;     /** COMPONENT Input parameter List from XML File -> Options */
    InputParam* mCompoInputSettings ;  /** COMPONENT Input parameter List from Settings File -> Params */
    InputParam* mCompoEnvImpactsParam; /** COMPONENT Input parameter List for all the environmental impacts considered */
    
    std::string mModelName;  //e.g. "OptimNPV"

    std::string mCurrency ;     /** Currency unit - default to EUR */
    std::string mObjectiveUnit; /** Objective unit - default to currency unit */

    int mNbYear ;           /** Number of year for economic data extrapolation */
    int mNbYearOffset ;     /** Offset of nb of year for discount cost computation */
    int mNbYearInput ;      /** Number of years in the input time series */
    int mLeapYearPos ;      /** Position of the leap in the input time series if there is one (0 if not, 1 if it is the first year, ...) */
    double mDiscountRate ;  /** Discount Rate */
    double mImpactDiscountRate;          /** Discount Rate for Env Impacts*/
    double mInternalRateOfReturn ;         /** Target Internal Rate of Return chosen by the user*/
    bool mForceExportAllIndicators;
    
    std::vector<double> mLevelizationTable;             /** levelization factor table by calculated year */
    std::vector<double> mImpactLevelizationTable;       /** levelization factor table for env impacts by calculated year */
    std::vector<double> mTableYearsHours;               /** Table of cumulative hours per year */

    std::vector<SEnvImpact>  mPossibleEnvImpacts;     /** List of possible impacts */
    std::vector<std::string> mSelectedEnvImpacts;     /** List of selected impacts */

    /** Attention: the EnvImpact-related vectors have the size of mPossibleImpacts and not mEnvImpacts
    * This is to not loss the reference in case of impacts re-declaration */
    std::deque<bool> mVBEnvImpactMaxConstraint;     /** use std::deque because std::vector doens't provide a referance &mVBEnvImpactMaxConstraint[j] */
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
    MIPModeler::MIPExpression mExpSubObjective; //Sum of all sub objectives of type Add
    MIPModeler::MIPExpression  mExpPenaltyConstraintCosts;   //A sum of all NodeLaw::mExpPenaltyConstraintCosts

    /** Expressions already esist in TechnicalSubModel (and can be used to carry TecEco (total) contribution)
        MIPModeler::MIPExpression mExpCapex;
        MIPModeler::MIPExpression1D mExpOpex;
        MIPModeler::MIPExpression1D mExpFixedOpex;
        MIPModeler::MIPExpression1D mExpReplacement;

        MIPModeler::MIPExpression1D mExpVariableCosts;

        Note that, mExpVariableCosts is not used to compute Buy and Sell parts.
        It is used to compute mExpVariableCostsUndiscounted which is published on ports
        There is an assertion to enure the equality.
    */

    std::vector<MIPModeler::MIPExpression> mExpEnvImpactEmbodiedCostVec;
    std::vector<MIPModeler::MIPExpression> mExpEnvImpactEmbodiedVec;
    std::vector<MIPModeler::MIPExpression1D> mExpEnvImpactCostVec;
    std::vector<MIPModeler::MIPExpression1D> mExpEnvImpactMassVec;
    std::vector<MIPModeler::MIPExpression1D> mExpEnvImpactReplacementVec;

    /* Expressions published on ports */
    MIPModeler::MIPExpression1D mExpOpexUndiscounted;
    MIPModeler::MIPExpression1D mExpVariableCostsUndiscounted;
    MIPModeler::MIPExpression1D mExpVariableOpexUndiscounted;
    std::vector<MIPModeler::MIPExpression> mExpEnvImpactEmbodiedUndiscountedVec;
    std::vector<MIPModeler::MIPExpression1D> mExpEnvImpactMassUndiscountedVec;

    /* 0D-Expressions used to compute Objective and NPV, and to add constraints.
       Not used to compute the related total discounted contributions.
       Discounted Opex and Discounted EnvImpactMass contributions are computed from mExpOpex and mExpEnvImpactMassVec.
       There are assertions to ensure that the evaluation of mExpOpexDiscounted and mExpEnvImpactMassVecDiscounted results in the same values.
     */
    MIPModeler::MIPExpression mExpOpexDiscounted;

    std::vector<MIPModeler::MIPExpression> mExpCumulativeEnvImpact;
    std::vector<MIPModeler::MIPExpression> mExpEnvImpactMassVecDiscounted;
    std::vector<MIPModeler::MIPExpression> mExpEnvImpactReplacementVecDiscounted;

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
    std::vector<double> mFixedOpexContribution = { 0., 0. };           /** Resulting Pure Opex contribution */
    std::vector<double> mReplacementContribution = { 0., 0. };        /** Resulting Replacement contribution */
    std::vector<double> mBuyVariableCostsContribution = { 0., 0. };   /** Resulting Variable Cost expenses contribution part*/
    std::vector<double> mSellVariableCostsContribution = { 0., 0. };  /** Resulting Variable Cost revenue contribution part*/

    // discounted 
    std::vector<double> mOpexContributionDiscounted = { 0., 0. };                  /** Resulting Opex contribution including VariableCosts due to energy costs*/
    std::vector<double> mFixedOpexContributionDiscounted = { 0., 0. };             /** Resulting Pure Opex contribution*/
    std::vector<double> mReplacementContributionDiscounted = { 0., 0. };         /** Resulting Replacement contribution part*/
    std::vector<double> mBuyVariableCostsContributionDiscounted = { 0., 0. };   /** Resulting Variable Cost expenses contribution part*/
    std::vector<double> mSellVariableCostsContributionDiscounted = { 0., 0. };  /** Resulting Variable Cost revenue contribution part*/

    // ----------- Env Impacts ------------- 
    // grey
    std::vector<std::vector<double>> mVDEmbodiedCostContribution; /** Resulting cost of all the embodied environmental impacts the user wants to consider*/
    std::vector<std::vector<double>> mVDEmbodiedMassContribution; /** Resulting cost of all the embodied environmental impacts the user wants to consider*/

    // undiscounted
    std::vector<std::vector<double>> mVDEnvImpactsCostContribution; /** Resulting cost of all the Direct environmental impacts the user wants to consider*/
    std::vector<std::vector<double>> mVDEnvImpactsMassContribution; /** Resulting mass of all the Direct environmental impacts the user wants to consider*/
    std::vector<std::vector<double>> mVDEnvImpactsReplacementContribution; /** Resulting EnvImpactReplacement contribution*/

    // discounted
    std::vector<std::vector<double>> mVDEnvImpactsCostContributionDiscounted;  /** Resulting EnvImpactCost contribution*/
    std::vector<std::vector<double>> mVDEnvImpactsMassContributionDiscounted;  /** Resulting EnvImpactMass contribution*/
    std::vector<std::vector<double>> mVDEnvImpactsReplacementContributionDiscounted; /** Resulting EnvImpactReplacement contribution*/

    //Flow + Grey
    std::vector<std::vector<double>> mVDEnvImpactsTotalCostDiscounted;
    std::vector<std::vector<double>> mVDEnvImpactsTotalMassDiscounted;
};

#endif // TECECOANALYSISCOMPO_H
