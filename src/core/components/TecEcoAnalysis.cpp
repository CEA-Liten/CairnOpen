#include "TecEcoAnalysis.h"
#include "OptimProblem.h"

#include <unordered_set>

class OptimProblem;

TecEcoAnalysis::TecEcoAnalysis(CairnObject* aParent, const std::map<std::string, std::string>& aComponent) :
    TechnicalSubModel(aParent),
    mConfigParam(nullptr),
    mCompoInputParam(nullptr),
    mCompoInputSettings(nullptr),
    mCompoEnvImpactsParam(nullptr),
    mSelectedEnvImpacts({}),
    mExtrapolationFactor(1.0)
{    
    /*
    * TecEcoAnalysis is a TechnicalSubModel. 
    * Its parent is OptimProblem which is an extension of MilpComponent
    * Its Name() = this->parent()->objectName();
    */

    mEcoInvestModel = false;
    mExportIndicators = true;

    mPossibleEnvImpacts = {
    { "Climate change#Global Warming Potential 100", "GWP", "kg CO2 eq"},
    { "Ozone depletion#Ozone Depletion Potential","ODP","kg CFC-11 eq"},
    { "Human toxicity-cancer effects#Comparative Toxic Unit for humans","HTox-c","CTUh"},
    { "Human toxicity-noncancer effects#Comparative Toxic Unit for humans","HTox-nc","CTUh"},
    { "Particulate matter-Respiratory inorganics#Human health effects associated with exposure to PM","PM", "disease incidences"},
    { "Ionising radiation human health#Human exposure efficiency relative to U","IRP","kBq U"},
    { "Photochemical ozone formation#Tropospheric ozone concentration increase","POCP","kg NMVOC eq"},
    { "Acidification#Accumulated Exceedance","AP", "mol H+ eq"},
    { "Eutrophication-terrestrial#Accumulated Exceedance","EP-t","mol N eq"},
    { "Eutrophication-Aquatic freshwater#Fraction of nutrients reaching freshwater and compartment", "EP-fw","kg P eq"},
    { "Eutrophication-aquatic marine#Fraction of nutrients reaching marine and compartment","EP-m","kg N eq"},
    { "Ecotoxicity freshwater#Comparative Toxic Unit for ecosystems", "Ecotox","CTUe"},
    { "Land use#Soil quality index", "LU", "pts"},
    { "Water use#User Deprivation Potential", "WU", "m3 world eq"},
    { "Resource use-Minerals and metals#Abiotic Resource Depletion","ADP-mm","kg Sb eq"},
    { "Resource use-Energy carriers#Abiotic Resource Depletion", "ADP-f","MJ" }
    };

    mVDEnvImpactMaxConstraint.resize(mPossibleEnvImpacts.size(), INFINITY_VAL);
    mVBEnvImpactMaxConstraint.clear();
    mVBEnvImpactMaxConstraint.resize(mPossibleEnvImpacts.size(), 0);    
    mVDEnvImpactCost.resize(mPossibleEnvImpacts.size(), 0.);

    doInit(aComponent);
}

TecEcoAnalysis::~TecEcoAnalysis()
{
    if (mConfigParam) delete mConfigParam;
    if (mCompoInputSettings) delete mCompoInputSettings;
    if (mCompoInputParam) delete mCompoInputParam;
    if (mCompoEnvImpactsParam) delete mCompoEnvImpactsParam;
}

std::map<std::string, MilpComponent*> TecEcoAnalysis::MilpComponents()
{
    if (mParentCompo) { 
        OptimProblem* pProblem = dynamic_cast<OptimProblem*>(mParentCompo->parent());
        if (pProblem) {
            return pProblem->MilpComponents();;
        }
    }
    return {};
}

std::vector<MilpComponent*> TecEcoAnalysis::NonBusMilpComponents()
{
    if (mParentCompo) {
        OptimProblem* pProblem = dynamic_cast<OptimProblem*>(mParentCompo->parent());
        if (pProblem) {
            return pProblem->NonBusMilpComponents();
        }
    }
    return {};
}

std::vector<BusCompo*> TecEcoAnalysis::BusComponents()
{
    if (mParentCompo) {
        OptimProblem* pProblem = dynamic_cast<OptimProblem*>(mParentCompo->parent());
        if (pProblem) {
            return pProblem->BusComponents();
        }
    }
    return {};
}

void TecEcoAnalysis::doInit(const std::map<std::string, std::string>& aComponent)
{
    //Init list of Settings parameters (scalar, double) by direct reading from aSettings file

    //Declare and set configuration parameters
    declareConfigurationParameters();
    setConfigurationParameters(aComponent);

    //Declare and set non-configuration parameters
    declareCompoInputParam();
    setCompoInputParam(aComponent);
}

void TecEcoAnalysis::declareConfigurationParameters()
{
    mConfigParam = new InputParam(this, "ConfigParam" + Name());
    //std::vector<std::string> (no default value)
    mConfigParam->addParameter("ConsideredEnvironmentalImpacts", &mSelectedEnvImpacts, {}, false, true, "Selected environmental impacts to be considered (EF method)", "-");
    //bool
    mConfigParam->addParameter("MinConstraint", &mMinConstraint, false, false, true, "MinConstraint option enabling ");    
    mConfigParam->addParameter("MaxConstraint", &mMaxConstraint, false, false, true, "MaxConstraint option enabling");  
}

void TecEcoAnalysis::setConfigurationParameters(const std::map<std::string, std::string>& aComponent)
{
    if (aComponent.empty()) {
        return; // component creation
    }

    CairnUtils::checkRead(mConfigParam->readParameters(aComponent), Name());
}

void TecEcoAnalysis::declareCompoInputParam()
{
    //------------------------ parameters ----------------------------------
    mCompoInputSettings = new InputParam(this, "CompoInputSettings" + Name());
    //bool
    mCompoInputSettings->addParameter("ForceExportAllIndicators", &mForceExportAllIndicators, false, false, true, "Force the export of all Indicators regardless of the values of EcoInvestModel and EnvironmentModel and ExportIndicators");
    //double
    mCompoInputSettings->addParameter("MinConstraintValue", &mMinConstraintValue, 0., &mMinConstraint, &mMinConstraint, "NPV > MinConstraintValue"); 
    mCompoInputSettings->addParameter("MaxConstraintValue", &mMaxConstraintValue, INFINITY_VAL, &mMaxConstraint, &mMaxConstraint, "NPV < MaxConstraintValue"); 

    //------------------------ options ----------------------------------
    mCompoInputParam = new InputParam(this, "CompoInputParam" + Name());
    //std::string
    mCompoInputParam->addParameter("Model", &mModelName, "OptimNPV", true, true, "Model used");
    mCompoInputParam->addParameter("Currency", &mCurrency, "EUR", true, true, "Currency unit - default to EUR");
    mCompoInputParam->addParameter("ObjectiveUnit", &mObjectiveUnit, "EUR", false, true, "Objective unit - default to currency unit");
    //int
    mCompoInputParam->addParameter("NbYear", &mNbYear, 20, false, true,"Number of year for economic data extrapolation");
    mCompoInputParam->addParameter("NbYearOffset", &mNbYearOffset, 0, false, true,"Offset of nb of year for discount cost computation");
    mCompoInputParam->addParameter("NbYearInput", &mNbYearInput, 1, false, true,"Number of years in the input time series");
    mCompoInputParam->addParameter("LeapYearPos", &mLeapYearPos, 0, false, true,"Position of the leap in the input time series if there is one eg : 0 if none and 1 if it is the first year");
    //double
    mCompoInputParam->addParameter("DiscountRate", &mDiscountRate, 0.07, true, true, "Discount rate");
    mCompoInputParam->addParameter("ImpactDiscountRate", &mImpactDiscountRate, 0., false, true, "Discount rate for environmental impacts");
    mCompoInputParam->addParameter("InternalRateOfReturn",&mInternalRateOfReturn, -1., false, true, "Target Internal Rate of Return chosen by the user");

    //------------------------ env impacts ----------------------------------
    mCompoEnvImpactsParam = new InputParam(this, "CompoEnvImpactsParam" + Name());
    declareEnvImpactParam();
}

void TecEcoAnalysis::declareEnvImpactParam() 
{
    /*
    * These are parameters that depend on the value of the configuration parameter "ConsideredEnvironmentalImpacts".
    * Every time "ConsideredEnvironmentalImpacts" changes, dependent parameters must be synchronized 
    * The method can be generalized to the declaration of other "dependent" parameters when needed.
    */

    // Get existing EnvImpact parameter names
    std::vector<std::string> impactParamNames;
    mCompoEnvImpactsParam->getParameters(impactParamNames);

    // Remove parameters that are no longer selected
    for (const auto& name : impactParamNames) {
        bool selected = std::any_of(mSelectedEnvImpacts.begin(), mSelectedEnvImpacts.end(),
            [&](std::string impactName) { 
                return CairnUtils::contains(name, impactName); 
            });
        if (!selected) {
            mCompoEnvImpactsParam->removeParameter(name);
        }
    }

    // Add missing parameters for selected impact Attention: re-adding already existing parameters may lead into losing their values 
    for (size_t i = 0; i < mSelectedEnvImpacts.size(); i++) {
        const int eIndex = getImpactIndex(mSelectedEnvImpacts[i]);
        if (eIndex < 0) continue;
        // Add parameter if not exist
        if (!CairnUtils::contains(impactParamNames, mSelectedEnvImpacts[i] + " MaxConstraint")) {
            mCompoEnvImpactsParam->addParameter(mSelectedEnvImpacts[i] + " MaxConstraint", &mVBEnvImpactMaxConstraint[eIndex], false, false, true, "Is max constraint considered?", "-");
        }
        if (!CairnUtils::contains(impactParamNames, mSelectedEnvImpacts[i] + " MaxConstraintValue")) {
            mCompoEnvImpactsParam->addParameter(mSelectedEnvImpacts[i] + " MaxConstraintValue", &mVDEnvImpactMaxConstraint[eIndex], INFINITY_VAL, false, true, "Maximum quantity of environmental impact", mPossibleEnvImpacts[eIndex].Unit);
        }
        if (!CairnUtils::contains(impactParamNames, mSelectedEnvImpacts[i] + " Cost")) {
            mCompoEnvImpactsParam->addParameter(mSelectedEnvImpacts[i] + " Cost", &mVDEnvImpactCost[eIndex], 0., false, true, "Cost of environmental impact in Currency", SFunctionUnit({ eFTypeDivision, {pCurrency()}, mPossibleEnvImpacts[eIndex].Unit}));
        }
    }
}

void TecEcoAnalysis::setCompoInputParam(const std::map<std::string, std::string>& aComponent)
{
    if (aComponent.empty()) {
        return; // component creation
    }

    //read non-configuration parameters
    CairnUtils::checkRead(mCompoInputParam->readParameters(aComponent), Name());
    CairnUtils::checkRead(mCompoInputSettings->readParameters(aComponent), Name());
    CairnUtils::checkRead(mCompoEnvImpactsParam->readParameters(aComponent), Name());
    
    if (mCurrency.empty()) {
        mCurrency = "EUR";
    }

    //set ObjectiveUnit to Currency if not provided
    if (CairnUtils::getParam(aComponent,"ObjectiveUnit") == "") {
        mObjectiveUnit = mCurrency;
    }

    if (mNbYearInput == 0) mNbYearInput = 1;
}

void TecEcoAnalysis::computeExtrapolationFactor(const MilpData* aMilpData)
{
    const bool useFactor = aMilpData->UseExtrapolationFactor();
    const bool singleYear = (mNbYearInput <= 1);
    const bool leapFirst = (mLeapYearPos == 1);

    mExtrapolationFactor = 1.0;

    if (useFactor && singleYear) {
        const double hours = leapFirst ? 8784.0 : 8760.0;
        mExtrapolationFactor = hours / (aMilpData->npdt() * aMilpData->pdtHeure());
    }

    computeLevelizationTable();
}

void TecEcoAnalysis::computeLevelizationTable()
{
    mLevelizationTable = CairnUtils::levelizationTable(mDiscountRate, mNbYear, mNbYearInput, 
        mLeapYearPos, mNbYearOffset, mExtrapolationFactor);

    mImpactLevelizationTable = CairnUtils::levelizationTable(mImpactDiscountRate, mNbYear, mNbYearInput, 
        mLeapYearPos, mNbYearOffset, mExtrapolationFactor);

    mTableYearsHours = CairnUtils::yearHourTable(mNbYearInput, mLeapYearPos);
}

bool TecEcoAnalysis::isValidLabel(const std::string& aLabel) const
{
    /*
    * veify if the label is defined in TecEconAnalysis
    */
    if (std::find(mLabelList.begin(), mLabelList.end(), aLabel) != mLabelList.end())
    {
        return true;
    }
    else {
        return false;
    }
}

void TecEcoAnalysis::jsonSaveGuiComponent(ojson &componentsArray)
{
    ojson compoObject = ojson::object();
    std::string mainCarrier = "";
    if (mMainCarrier) {
        mainCarrier = mMainCarrier->Name();
    }
    mParentCompo->getGUIData()->jsonSaveGUILine(compoObject, mainCarrier);

    compoObject["paramListJson"] = ojson::array();
    compoObject["optionListJson"] = ojson::array();
    compoObject["envImpactsListJson"] = ojson::array();

    ojson& paramArray = compoObject["paramListJson"];
   
    mConfigParam->jsonSaveGUIInputParam(paramArray);
    mCompoInputSettings->jsonSaveGUIInputParam(paramArray);
    mCompoInputParam->jsonSaveGUIInputParam(compoObject["optionListJson"]);
    mCompoEnvImpactsParam->jsonSaveGUIInputParam(compoObject["envImpactsListJson"]);
        
    compoObject["labelList"] = mLabelList;

    compoObject["nodePortsData"] = ojson::array();
    //ojson& nodePortsArray = compoObject["nodePortsData"];

    compoObject["nodePorts"] = ojson::object();
    ojson& nodePorts = compoObject["nodePorts"];

    if (mParentCompo) {
        int portCount = mParentCompo->listSidePorts(Left()).size();
        if (portCount) {
            nodePorts[Left()] = portCount;
            mParentCompo->jsonSaveGUINodePortsData(compoObject["nodePortsData"], Left());
        }
        portCount = mParentCompo->listSidePorts(Right()).size();
        if (portCount) {
            nodePorts[Right()] = portCount;
            mParentCompo->jsonSaveGUINodePortsData(compoObject["nodePortsData"], Right());
        }
        portCount = mParentCompo->listSidePorts(Bottom()).size();
        if (portCount) {
            nodePorts[Bottom()] = portCount;
            mParentCompo->jsonSaveGUINodePortsData(compoObject["nodePortsData"], Bottom());
        }
        portCount = mParentCompo->listSidePorts(Top()).size();
        if (portCount) {
            nodePorts[Top()] = portCount;
            mParentCompo->jsonSaveGUINodePortsData(compoObject["nodePortsData"], Top());
        }
    }
       
    componentsArray.push_back(compoObject) ;
}


std::map<std::string, ModelParam*> TecEcoAnalysis::getParameters()
{
    std::map<std::string, ModelParam*> paramMap;

    paramMap.insert(getCompoInputParam()->getMapParams().begin(), getCompoInputParam()->getMapParams().end());
    paramMap.insert(getCompoInputSettings()->getMapParams().begin(), getCompoInputSettings()->getMapParams().end());
    paramMap.insert(getCompoEnvImpactsParam()->getMapParams().begin(), getCompoEnvImpactsParam()->getMapParams().end());

    return paramMap;
}


int TecEcoAnalysis::getImpactIndex(const std::string& impactName)
{
    /*
    * returns the index of a given impactName in mPossibleEnvImpacts
    * The goal is to associate a fixed index for every Env Impact regardless if it is selected or not
    */
    for (int i = 0; i < mPossibleEnvImpacts.size(); ++i) {
        if (mPossibleEnvImpacts[i].Name == impactName) {
            return i;
        }
    }
    return -1;
}


std::vector<double> TecEcoAnalysis::EnvImpactCosts() {
    /*
    * Only return values for the selected impacts!
    */
    std::vector<double> aVDEnvImpactCost;
    aVDEnvImpactCost.resize(mSelectedEnvImpacts.size(), 0.);
    if (mVDEnvImpactCost.size() < mSelectedEnvImpacts.size())
        return aVDEnvImpactCost;
    for (int i = 0; i < mSelectedEnvImpacts.size(); i++)
    {
        const int eIndex = getImpactIndex(mSelectedEnvImpacts[i]);
        if (eIndex < 0) continue;
        aVDEnvImpactCost[i] = mVDEnvImpactCost[eIndex];
    }
    return aVDEnvImpactCost;
}

std::vector<std::string> TecEcoAnalysis::getPossibleImpactNames() const
{
    std::vector<std::string> vRet;
    for (auto& vImpact : mPossibleEnvImpacts) {
        vRet.push_back(vImpact.Name);
    }
    return vRet;
}

std::vector<std::string> TecEcoAnalysis::getPossibleImpactShortNames() const
{
    std::vector<std::string> vRet;
    for (auto& vImpact : mPossibleEnvImpacts) {
        vRet.push_back(vImpact.ShortName);
    }
    return vRet;
}

std::string TecEcoAnalysis::EnvImpactUnit(const std::string& impactName) const
{
    const SEnvImpact* selectedImpact = nullptr;
    for (auto& vImpact : mPossibleEnvImpacts) {
        if (impactName == vImpact.Name) {
            selectedImpact = &vImpact;
            break;
        }
    }
    return selectedImpact ? selectedImpact->Unit : "-";
}

std::vector<std::string> TecEcoAnalysis::EnvImpactUnitsList() const
{
    std::vector<std::string> selectedImpactUnitsList;
    selectedImpactUnitsList.reserve(mSelectedEnvImpacts.size());
    for (const auto& selectedName : mSelectedEnvImpacts) {
        selectedImpactUnitsList.push_back(EnvImpactUnit(selectedName));
    }
    return selectedImpactUnitsList;
}

std::string TecEcoAnalysis::EnvImpactShortName(const std::string& name) const {
    //name can be any string that contains an impact name as a sub-string
    for (auto& vImpact : mPossibleEnvImpacts) {
        if (CairnUtils::contains(name, vImpact.Name)) {
            std::string shortName = name;
            return CairnUtils::replace(shortName, vImpact.Name, vImpact.ShortName);
        }
    }
    return name;
}

std::vector<std::string> TecEcoAnalysis::EnvImpactShortNamesList() const
{
    std::vector<std::string> selectedImpactShortNamesList;
    selectedImpactShortNamesList.reserve(mSelectedEnvImpacts.size());
    for (const auto& selectedName : mSelectedEnvImpacts) {
        selectedImpactShortNamesList.push_back(EnvImpactShortName(selectedName));
    }
    return selectedImpactShortNamesList;
}

std::string TecEcoAnalysis::EnvImpactLongName(const std::string& name) {
    //name can be any string that contains an impact short name as a sub-string
    for (auto& vImpact : mPossibleEnvImpacts) {
        if (CairnUtils::contains(name, vImpact.ShortName) && !CairnUtils::contains(name, vImpact.Name)) {
            std::string longName = name;
            return CairnUtils::replace(longName, vImpact.ShortName, vImpact.Name);
        }
    }
    return name;
}



// ******************************************* CompoModel ************************************************ //

void TecEcoAnalysis::setTimeData()
{
    SubModel::setTimeData();
}

void TecEcoAnalysis::computePenaltyConstraintCosts() {
    //Compute the total PenaltyConstraintCosts from all NodeLaws
    for (auto& lptrBus : BusComponents()) {
        if (lptrBus->getMIPExpression("PenaltyConstraintCosts")) {
            mExpPenaltyConstraintCosts += *(lptrBus->getMIPExpression("PenaltyConstraintCosts"));
        }
    }
}

void TecEcoAnalysis::computeSubObjective()  
{
    for (auto& lptrBus : BusComponents()) {
        if (lptrBus->ModelClassName() == "ManualObjective" && lptrBus->ObjectiveType() == "Add") {
            MIPModeler::MIPExpression* expSubObjective = lptrBus->getMIPExpression("SubObjectiveExpression");
            if (lptrBus->ObjectiveType() == "Add") {
                mExpSubObjective += *expSubObjective;
            }
        }
    }
}

void TecEcoAnalysis::buildModel()
{
    /*
    * Computation of Economical and Environmental should be made in computeAllTecEcoContribution()
    * Because they should be computed before adding Bus constraints (Bus::buildModel)
    */

    //Compute the contributions related to Bus components (have to be done after Bus::buildModel)
    computeSubObjective();
    computePenaltyConstraintCosts();

    //Objective
    if (CairnUtils::contains(mModelName, "OptimEnvImpact"))
    {
        // Find the impact short name by checking which one matches the suffix of mModelName
        std::string impactShortName = "";

        for (const auto& shortName : getPossibleImpactShortNames()) {
            const std::string suffix = "-" + shortName;
            if (mModelName.size() >= suffix.size() &&
                mModelName.compare(mModelName.size() - suffix.size(), suffix.size(), suffix) == 0) {
                impactShortName = shortName;
                break;
            }
        }

        if (impactShortName.empty()) {
            throw Cairn_Exception(
                "Error: unknown model name " + mModelName + " on component " + Name() +
                ". Expected: OptimEnvImpact-$ImpactShortName", -1);
        }

        std::string impactName = EnvImpactLongName(impactShortName);

        // Find and add the matching environmental impact
        for (int i = 0; i < mSelectedEnvImpacts.size(); i++) {
            if (mSelectedEnvImpacts[i] == impactName) {
                mExpObjective += mExpCumulativeEnvImpact[i];
                break;
            }
        }
    }
    else if (mModelName == "OptimManualObjective") {
        //do nothing: only SubObjective is counted
    }
    else if (mModelName == "OptimizeControlOnly") {
        mExpObjective = mExpOpexDiscounted + mExpPenaltyConstraintCosts;
    }
    else {// == "OptimNPV" -- default option
        mExpObjective = mExpOpexDiscounted + mExpCapex + mExpPenaltyConstraintCosts;
    }

    //Add SubObjective
    mExpObjective += mExpSubObjective;

    //NPV 
    mExpNegNPV = mExpCapex + mExpOpexDiscounted;

    //add constraints on NPV 
    if (mMinConstraint) addConstraint(mExpNegNPV >= mMinConstraintValue, "NPV");
    if (mMaxConstraint) addConstraint(mExpNegNPV <= mMaxConstraintValue, "NPV");

    //add constraints on EnvImpact Mass 
    for (int i = 0; i < mSelectedEnvImpacts.size(); i++) {
        const int eIndex = getImpactIndex(mSelectedEnvImpacts.at(i));
        if (eIndex < 0) continue;
        if (mVBEnvImpactMaxConstraint[eIndex]) {
            addConstraint(mExpCumulativeEnvImpact.at(i) <= mVDEnvImpactMaxConstraint[eIndex], "EnvImpactMaxConstraint");
        }
    }

    mAllocate = false;
}

void TecEcoAnalysis::computeAllContribution() {
    if (mAllocate) {
        allocateExpressions();
    }
    else {
        closeExpressions();
    }

    /** Compute possible environment expressions and add corresponding constraints, if EnvironmentModel=true */
    computeEnvContribution();

    /** Compute economical expressions and add corresponding constraints, if EncoInvestModel=true */
    computeEconomicalContribution();

    /** Compute undiscounted expressions to publish them on ports (IO interface) */
    computeUndiscountedExpressions();
}

void TecEcoAnalysis::computeUndiscountedExpressions() {
    //Compute undiscounted expressions from raw expressions
    double ExtrapolationFactor = mParentCompo->ExtrapolationFactor();
    for (unsigned int t = 0; t < mTimeSteps.size(); ++t)
    {
        mExpOpexUndiscounted[t] = mExpOpex[t] * ExtrapolationFactor;
        mExpVariableCostsUndiscounted[t] = mExpVariableCosts[t] * ExtrapolationFactor;
        mExpVariableOpexUndiscounted[t] = mExpVariableOpex[t] * ExtrapolationFactor;
    }

    for (int i = 0; i < mSelectedEnvImpacts.size(); i++)
    {
        mExpEnvImpactEmbodiedUndiscountedVec[i] = mExpEnvImpactEmbodiedVec[i]; //* ExtrapolationFactor; Why was it multiplied by ExtrapolationFactor?
        for (unsigned int t = 0; t < mTimeSteps.size(); ++t)
        {
            mExpEnvImpactMassUndiscountedVec[i][t] = mExpEnvImpactMassVec[i][t] * ExtrapolationFactor;
        }
    }
}

void TecEcoAnalysis::allocateExpressions() {
    //Only 1D
    mExpFixedOpex = MIPModeler::MIPExpression1D(mTimeSteps.size());
    mExpVariableOpex = MIPModeler::MIPExpression1D(mTimeSteps.size());
    mExpReplacement = MIPModeler::MIPExpression1D(mTimeSteps.size());
    mExpVariableCosts = MIPModeler::MIPExpression1D(mTimeSteps.size());
    mExpOpex = MIPModeler::MIPExpression1D(mTimeSteps.size());

    mExpOpexUndiscounted = MIPModeler::MIPExpression1D(mTimeSteps.size());
    mExpVariableCostsUndiscounted = MIPModeler::MIPExpression1D(mTimeSteps.size());
    mExpVariableOpexUndiscounted = MIPModeler::MIPExpression1D(mTimeSteps.size());

    mExpEnvImpactMassVec.resize(mSelectedEnvImpacts.size());
    mExpEnvImpactCostVec.resize(mSelectedEnvImpacts.size());
    mExpEnvImpactReplacementVec.resize(mSelectedEnvImpacts.size());
    for (int i = 0; i < mSelectedEnvImpacts.size(); i++) {
        mExpEnvImpactMassVec[i] = MIPModeler::MIPExpression1D(mTimeSteps.size());
        mExpEnvImpactCostVec[i] = MIPModeler::MIPExpression1D(mTimeSteps.size());
        mExpEnvImpactMassUndiscountedVec[i] = MIPModeler::MIPExpression1D(mTimeSteps.size()); // already resized in declareModelInterfaces
        mExpEnvImpactReplacementVec[i] = MIPModeler::MIPExpression1D(mTimeSteps.size());
    }
}

void TecEcoAnalysis::closeExpressions() {
    if (mAllocate) return;
    //Economical 0D
    closeExpression(mExpObjective);
    closeExpression(mExpNegNPV);
    closeExpression(mExpSubObjective);
    closeExpression(mExpPenaltyConstraintCosts);
    closeExpression(mExpCapex);

    closeExpression(mExpOpexDiscounted);

    //Economical 1D
    closeExpression1D(mExpOpex);
    closeExpression1D(mExpFixedOpex);
    closeExpression1D(mExpVariableOpex);
    closeExpression1D(mExpReplacement);
    closeExpression1D(mExpVariableCosts);

    //Env Impacts
    for (int i = 0; i < mSelectedEnvImpacts.size(); i++) {
        //0D
        closeExpression(mExpEnvImpactEmbodiedCostVec[i]);
        closeExpression(mExpEnvImpactEmbodiedVec[i]);

        closeExpression(mExpEnvImpactMassVecDiscounted[i]);
        closeExpression(mExpEnvImpactReplacementVecDiscounted[i]);

        closeExpression(mExpCumulativeEnvImpact[i]);

        //1D
        closeExpression1D(mExpEnvImpactCostVec[i]);
        closeExpression1D(mExpEnvImpactMassVec[i]);
        closeExpression1D(mExpEnvImpactReplacementVec[i]);
    }
}

void TecEcoAnalysis::computeEconomicalContribution() {
    for (auto& lptr : NonBusMilpComponents()) {
        // ---------------------- 0D ----------------------
        if (lptr->EcoInvestModel()) {
            mExpCapex += *(lptr->getMIPExpression("Capex"));
        }

        // ---------------------- 1D ----------------------
        int year = 0;
        for (unsigned int t = 0; t < mTimeSteps.size(); ++t)
        {
            MIPModeler::MIPExpression expVarCost_t;
            MIPModeler::MIPExpression expFixedOpex_t;
            MIPModeler::MIPExpression expVarOpex_t;
            MIPModeler::MIPExpression exReplacement_t;
            MIPModeler::MIPExpression expOpex_t;

            uint t_hour = std::ceil(t * TimeStep(t)) + mParentCompo->HistNbHours();
            while (t_hour > mTableYearsHours.at(year) && year < mTableYearsHours.size() - 1) {
                year += 1;
            }

            if (lptr->getMIPExpression1D("VariableCosts") != nullptr) {
                expVarCost_t = lptr->getMIPExpression1D(t, "VariableCosts");
            }

            if (lptr->getMIPExpression1D("VariableOpex") != nullptr) {
                expVarOpex_t = lptr->getMIPExpression1D(t, "VariableOpex");
            }

            if (lptr->EcoInvestModel()) {
                expFixedOpex_t = lptr->getMIPExpression1D(t, "FixedOpex");
                exReplacement_t = lptr->getMIPExpression1D(t, "Replacement");
                //Net opex includes all costs (Variable Costs, Replacement and Env Impact Cost)
                expOpex_t = lptr->getMIPExpression1D(t, "Opex");
            }//Count Variable Costs and Env Impact Costs for Net Opex if EcoInvestModel == false
            else {
                //Variable Costs exists for all models even when EcoInvestModel == false
                expOpex_t = expVarCost_t;
                expOpex_t += expVarOpex_t;
                //Env Impact Costs exist only for TechnicalSubModel if EnvironmentModel == true 
                if (lptr->EnvironmentModel()) {
                    for (int i = 0; i < mSelectedEnvImpacts.size(); i++) {
                        expOpex_t += lptr->getMIPExpression1D(t, lptr->compoModel()->getEnvImpactCostExpression(i));
                    }
                }
            }
            mExpVariableCosts[t] += expVarCost_t;
            mExpFixedOpex[t] += expFixedOpex_t;
            mExpVariableOpex[t] += expVarOpex_t;
            mExpReplacement[t] += exReplacement_t;
            mExpOpex[t] += expOpex_t;
            //0D Discounted Opex Exp used to compute mExpObjective and add constraints in buildModel()
            mExpOpexDiscounted += expOpex_t * mLevelizationTable.at(year);
        }
    }
}

void TecEcoAnalysis::computeEnvContribution()
{   
    //Computes total Env contribution
    mExpEnvImpactEmbodiedCostVec.resize(mSelectedEnvImpacts.size());
    mExpEnvImpactEmbodiedVec.resize(mSelectedEnvImpacts.size());
    mExpEnvImpactMassVecDiscounted.resize(mSelectedEnvImpacts.size());
    mExpEnvImpactReplacementVecDiscounted.resize(mSelectedEnvImpacts.size());
    mExpCumulativeEnvImpact.resize(mSelectedEnvImpacts.size());
    for (auto& lptr : NonBusMilpComponents()) 
    {
        if (lptr->EnvironmentModel()) {
            for (int i = 0; i < mSelectedEnvImpacts.size(); i++)
            {
                int year = 0;
                for (unsigned int t = 0; t < mTimeSteps.size(); ++t)
                {
                    MIPModeler::MIPExpression expImpactMass_i_t;
                    MIPModeler::MIPExpression expImpactCost_i_t;
                    MIPModeler::MIPExpression expImpactReplacement_i_t;

                    uint t_hour = std::ceil(t * TimeStep(t)) + mParentCompo->HistNbHours();
                    while (t_hour > mTableYearsHours.at(year) && year < mTableYearsHours.size() - 1) {
                        year += 1;
                    }
                    //operation
                    expImpactMass_i_t = lptr->getMIPExpression1D(t, lptr->compoModel()->getEnvImpactMassExpression(i));
                    mExpEnvImpactMassVec[i][t] += expImpactMass_i_t;
                    //0D Discounted Mass Exp used to add constraints in buildModel()
                    mExpEnvImpactMassVecDiscounted[i] += expImpactMass_i_t * mImpactLevelizationTable.at(year);
                    
                    //replacement
                    expImpactReplacement_i_t = (lptr->compoModel()->getEnvImpacts()[i]->getExpEnvReplacement())->at(t);
                    mExpEnvImpactReplacementVec[i][t] += expImpactReplacement_i_t;
                    mExpEnvImpactReplacementVecDiscounted[i] += expImpactReplacement_i_t * mImpactLevelizationTable.at(year);

                    //costs
                    mExpEnvImpactCostVec[i][t] += (lptr->compoModel()->getEnvImpacts()[i]->getExpEnvOpCost())->at(t);
                }

                // ---------------------------- Embodied Env Impacts ----------------------------
                mExpEnvImpactEmbodiedVec[i] += *(lptr->compoModel()->getEnvImpacts()[i]->getExpEnvEmbodied());
                mExpEnvImpactEmbodiedCostVec[i] += *(lptr->compoModel()->getEnvImpacts()[i]->getExpEnvEmbodiedCost());
                //Expression for total env impacts
                mExpCumulativeEnvImpact[i] = mExpEnvImpactMassVecDiscounted[i] + mExpEnvImpactEmbodiedVec[i] + mExpEnvImpactReplacementVecDiscounted[i];
            }
        }
    }
}

void TecEcoAnalysis::computeBuyAndSellExpressions(const double* optSol, MIPModeler::MIPExpression1D& expBuyPart, MIPModeler::MIPExpression1D& expSellPart)
{
    /*
        Cannot be computed from mExpVariableCosts
    */
    expBuyPart = MIPModeler::MIPExpression1D(mTimeSteps.size());
    expSellPart = MIPModeler::MIPExpression1D(mTimeSteps.size());

    for (auto& [key,lptr]:MilpComponents()) {

    }

    for (auto& lptr : NonBusMilpComponents()) {
        if (lptr->getMIPExpression1D("VariableCosts") == nullptr)
            continue; //case of non-TechnicalSubModel componenets 

        MIPModeler::MIPExpression expVarCost_t;

        for (unsigned int t = 0; t < mTimeSteps.size(); ++t)
        {
            expVarCost_t = lptr->getMIPExpression1D(t, "VariableCosts");

            if (expVarCost_t.evaluate(optSol) > 0) {
                expBuyPart[t] += expVarCost_t;
            }
            else {
                expSellPart[t] += expVarCost_t;
            }
        }
    }
}

void TecEcoAnalysis::computeAllIndicators(const double* optSol)
{
    //Evaluate TecEco expressions
    if (optSol) {
        //General Indicators (factors)
        mNbYearIndicator.at(0) = mNbYearIndicator.at(1) = mNbYear;
        mDiscountRateIndicator.at(0) = mDiscountRateIndicator.at(1) = mDiscountRate;
        mImpactDiscountRateIndicator.at(0) = mImpactDiscountRateIndicator.at(1) = mImpactDiscountRate;
        mNbYearInputIndicator.at(0) = mNbYearInputIndicator.at(1) = mNbYearInput;
        mLeapYearPosIndicator.at(0) = mLeapYearPosIndicator.at(1) = mLeapYearPos;
        mNbYearOffsetIndicator.at(0) = mNbYearOffsetIndicator.at(1) = mNbYearOffset;
        mDiscountFactorIndicator.at(0) = CairnUtils::levelization(mDiscountRate, mNbYear, mNbYearOffset, 1); //PLAN
        mDiscountFactorIndicator.at(1) = CairnUtils::levelization(mDiscountRate, mNbYear, mNbYearOffset, 1); //HIST
        if (mLevelizationTable.size() > 1) {
            for (int i = 0; i < mLevelizationTable.size(); i++) {
                mDiscountFactorListIndicator.at(i).at(0) = mDiscountFactorListIndicator.at(i).at(1) = mLevelizationTable.at(i);
            }
        }
        mImpactDiscountFactorIndicator.at(0) = CairnUtils::levelization(mImpactDiscountRate, mNbYear, mNbYearOffset, 1); //PLAN
        mImpactDiscountFactorIndicator.at(1) = CairnUtils::levelization(mImpactDiscountRate, mNbYear, mNbYearOffset, 1); //HIST
        if (mImpactLevelizationTable.size() > 1) {
            for (int i = 0; i < mImpactLevelizationTable.size(); i++) {
                mImpactDiscountFactorListIndicator.at(i).at(0) = mImpactDiscountFactorListIndicator.at(i).at(1) = mImpactLevelizationTable.at(i);
            }
        }

        double ExtrapolationFactor = mParentCompo->ExtrapolationFactor();
        mExtraFactorIndicator.at(0) = mExtraFactorIndicator.at(1) = ExtrapolationFactor;

        //Objective
        mObjectiveContribution.at(0) = mObjectiveContribution.at(1) = mExpObjective.evaluate(optSol);

        //SubObjective
        mSubObjectiveContribution.at(0) = mSubObjectiveContribution.at(1) = mExpSubObjective.evaluate(optSol);

        //PenaltyConstraintCosts
        mPenaltyConstraintContribution.at(0) = mPenaltyConstraintContribution.at(1) = mExpPenaltyConstraintCosts.evaluate(optSol);

        //Capex
        mCapexContribution.at(0) = mCapexContribution.at(1) = mExpCapex.evaluate(optSol);

        //Opex
        SubModel::computeIndicator(mExpFixedOpex, optSol, mPureOpexContribution.at(0), mPureOpexContributionDiscounted.at(0), mPureOpexContribution.at(1), mPureOpexContributionDiscounted.at(1), false);
        SubModel::computeIndicator(mExpReplacement, optSol, mReplacementContribution.at(0), mReplacementContributionDiscounted.at(0), mReplacementContribution.at(1), mReplacementContributionDiscounted.at(1), false);
        SubModel::computeIndicator(mExpOpex, optSol, mOpexContribution.at(0), mOpexContributionDiscounted.at(0), mOpexContribution.at(1), mOpexContributionDiscounted.at(1), false);

        double opexDiscounted_redundant = mExpOpexDiscounted.evaluate(optSol);
        assert(abs(mOpexContributionDiscounted.at(0) - opexDiscounted_redundant) < 10e-3);

        //NPV = -capex -discounted_opex
        mNetPresentValue.at(0) = -mCapexContribution.at(0) - mOpexContributionDiscounted.at(0);

        double negNpv = mExpNegNPV.evaluate(optSol); //mExpNegNPV is used to add constraints on NPV
        assert(abs(mNetPresentValue.at(0) + negNpv) < 10e-3);

        //histNPV = -capex -discounted_hist_opex
        mNetPresentValue.at(1) = -mCapexContribution.at(1) - mOpexContributionDiscounted.at(1);

        //double opexDiscounted = mOpexContributionDiscounted(index);
        if (mInternalRateOfReturn >= 0.) {
            mInternalRateOfReturnPerCent.at(0) = mInternalRateOfReturnPerCent.at(1) = mInternalRateOfReturn * 100.;
            double RateOfReturnDiscountFactor = 0; // Why it is always 0 ?!!
            mInternalRateOfReturnFactor.at(0) = mInternalRateOfReturnFactor.at(1) = RateOfReturnDiscountFactor;
            mNetPresentValueAtIRR.at(0) = -mCapexContribution.at(0) + (mOpexContribution.at(0) * RateOfReturnDiscountFactor);
            mNetPresentValueAtIRR.at(1) = -mCapexContribution.at(1) + (mOpexContribution.at(1) * RateOfReturnDiscountFactor);
        }

        mAverageRateOfReturnFactor.at(0) = -mCapexContribution.at(0) / (mOpexContribution.at(0) / ExtrapolationFactor); //PLAN
        mAverageRateOfReturnFactor.at(1) = -mCapexContribution.at(1) / mOpexContribution.at(1); //HIST
        mAverageRateOfReturnPerCent.at(0) = 100. * CairnUtils::discountRate(mAverageRateOfReturnFactor.at(0), mNbYear, mNbYearOffset, ExtrapolationFactor); //PLAN
        mAverageRateOfReturnPerCent.at(1) = 100. * CairnUtils::discountRate(mAverageRateOfReturnFactor.at(1), mNbYear, mNbYearOffset, ExtrapolationFactor);  //HIST
        mCurrentRateOfReturnPerCent.at(0) = 100. * (-mCapexContribution.at(0) / mOpexContributionDiscounted.at(0)); //PLAN
        mCurrentRateOfReturnPerCent.at(1) = 100. * (-mCapexContribution.at(1) / (mOpexContributionDiscounted.at(1) * ExtrapolationFactor)); //HIST

        //--------------------------------------------------------------------

        //Variable Costs
        MIPModeler::MIPExpression1D expBuyPart;
        MIPModeler::MIPExpression1D expSellPart;

        computeBuyAndSellExpressions(optSol, expBuyPart, expSellPart);
        SubModel::computeIndicator(expBuyPart, optSol, mBuyVariableCostsContribution.at(0), mBuyVariableCostsContributionDiscounted.at(0), mBuyVariableCostsContribution.at(1), mBuyVariableCostsContributionDiscounted.at(1), false);
        SubModel::computeIndicator(expSellPart, optSol, mSellVariableCostsContribution.at(0), mSellVariableCostsContributionDiscounted.at(0), mSellVariableCostsContribution.at(1), mSellVariableCostsContributionDiscounted.at(1), false);

        double varCost = 0.0;
        for (unsigned int t = 0; t < mTimeSteps.size(); ++t) {
            varCost += ExtrapolationFactor * mExpVariableCosts[t].evaluate(optSol);
        }
        assert(abs(varCost - mBuyVariableCostsContribution.at(0) - mSellVariableCostsContribution.at(0)) < 10e-3);

        // ---------------------------- Env Impacts ----------------------------
        mVDEnvGreyImpactsCostContribution.resize(mSelectedEnvImpacts.size(), { 0., 0. });
        mVDEnvImpactsReplacementContribution.resize(mSelectedEnvImpacts.size(), { 0., 0. });
        mVDEnvImpactsReplacementContributionDiscounted.resize(mSelectedEnvImpacts.size(), { 0., 0. });
        for (int i = 0; i < mSelectedEnvImpacts.size(); i++) {
            mVDEnvGreyImpactsCostContribution[i].at(0) = mVDEnvGreyImpactsCostContribution[i].at(1) = mExpEnvImpactEmbodiedCostVec.at(i).evaluate(optSol);
            mVDEnvGreyImpactsMassContribution[i].at(0) = mVDEnvGreyImpactsMassContribution[i].at(1) = mExpEnvImpactEmbodiedVec.at(i).evaluate(optSol);

            SubModel::computeIndicator(mExpEnvImpactMassVec[i], optSol, mVDEnvImpactsMassContribution[i].at(0), mVDEnvImpactsMassContributionDiscounted[i].at(0), mVDEnvImpactsMassContribution[i].at(1), mVDEnvImpactsMassContributionDiscounted[i].at(1), true);
            SubModel::computeIndicator(mExpEnvImpactCostVec[i], optSol, mVDEnvImpactsCostContribution[i].at(0), mVDEnvImpactsCostContributionDiscounted[i].at(0), mVDEnvImpactsCostContribution[i].at(1), mVDEnvImpactsCostContributionDiscounted[i].at(1), false);

            //Replacement
            SubModel::computeIndicator(mExpEnvImpactReplacementVec[i], optSol, mVDEnvImpactsReplacementContribution[i].at(0), mVDEnvImpactsReplacementContributionDiscounted[i].at(0), mVDEnvImpactsReplacementContribution[i].at(1), mVDEnvImpactsReplacementContributionDiscounted[i].at(1), true);

            //Cumulative env impact
            //mExpCumulativeEnvImpact[i].evaluate(optSol);

            double envImpactMassDiscounted_redundant_i = mExpEnvImpactMassVecDiscounted.at(i).evaluate(optSol);
            assert(abs(mVDEnvImpactsMassContributionDiscounted[i].at(0) - envImpactMassDiscounted_redundant_i) < 10e-3);

            double envImpactReplacementDiscounted_redundant_i = mExpEnvImpactReplacementVecDiscounted.at(i).evaluate(optSol);
            assert(abs(mVDEnvImpactsReplacementContributionDiscounted[i].at(0) - envImpactReplacementDiscounted_redundant_i) < 10e-3);

            //Flow + Grey + Replacement
            mVDEnvImpactsTotalCostDiscounted[i].at(0) = mVDEnvGreyImpactsCostContribution.at(i).at(0) + mVDEnvImpactsCostContributionDiscounted.at(i).at(0) ;
            mVDEnvImpactsTotalCostDiscounted[i].at(1) = mVDEnvGreyImpactsCostContribution.at(i).at(1) + mVDEnvImpactsCostContributionDiscounted.at(i).at(1) ;
            mVDEnvImpactsTotalMassDiscounted[i].at(0) = mVDEnvGreyImpactsMassContribution.at(i).at(0) + mVDEnvImpactsMassContributionDiscounted.at(i).at(0) + mVDEnvImpactsReplacementContributionDiscounted[i].at(0);
            mVDEnvImpactsTotalMassDiscounted[i].at(1) = mVDEnvGreyImpactsMassContribution.at(i).at(1) + mVDEnvImpactsMassContributionDiscounted.at(i).at(1) + mVDEnvImpactsReplacementContributionDiscounted[i].at(1);

        }
    }
}

