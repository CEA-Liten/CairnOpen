#include "TecEcoAnalysis.h"
#include "OptimProblem.h"

#include <unordered_set>

class OptimProblem;

TecEcoAnalysis::TecEcoAnalysis(CairnObject* aParent, const t_mapParamData& aComponent) :
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
    auto* compo = parentComponent();
    if(!compo)
        return {};

    OptimProblem* problem = dynamic_cast<OptimProblem*>(compo->parent());
    if (!problem) 
        return {};

    return problem->MilpComponents();
}

std::vector<MilpComponent*> TecEcoAnalysis::NonBusMilpComponents()
{
    auto* compo = parentComponent();
    if (!compo)
        return {};

    OptimProblem* problem = dynamic_cast<OptimProblem*>(compo->parent());
    if (!problem)
        return {};

    return problem->NonBusMilpComponents();
}

std::vector<BusCompo*> TecEcoAnalysis::BusComponents()
{
    auto* compo = parentComponent();
    if (!compo)
        return {};

    OptimProblem* problem = dynamic_cast<OptimProblem*>(compo->parent());
    if (!problem)
        return {};

    return problem->BusComponents();
}

void TecEcoAnalysis::doInit(const t_mapParamData& aComponent)
{
    CAIRN_LOG_SCOPE(Name());

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
    mConfigParam->addParameter("ConsideredEnvironmentalImpacts", &mSelectedEnvImpacts, std::vector < std::string>(), false, true, "Selected environmental impacts to be considered (EF method)", "-");
    //bool
    mConfigParam->addParameter("MinConstraint", &mMinConstraint, false, false, true, "MinConstraint option enabling ");    
    mConfigParam->addParameter("MaxConstraint", &mMaxConstraint, false, false, true, "MaxConstraint option enabling");  
}

void TecEcoAnalysis::setConfigurationParameters(const t_mapParamData& aComponent)
{
    // TODO: use initProblem()
    if (aComponent.empty()) {
        return; // component creation
    }

    if (mConfigParam->readParameters(aComponent) < -1) {
        throw Cairn_Exception("Error while initializing " + Name() +
            ". A mandatory parameter is missing!", -1);
    }
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
            mCompoEnvImpactsParam->addParameter(mSelectedEnvImpacts[i] + " MaxConstraintValue", &mVDEnvImpactMaxConstraint[eIndex], INFINITY_VAL, false, true, "Total project maximum quantity of environmental impact", mPossibleEnvImpacts[eIndex].Unit);
        }
        if (!CairnUtils::contains(impactParamNames, mSelectedEnvImpacts[i] + " Cost")) {
            mCompoEnvImpactsParam->addParameter(mSelectedEnvImpacts[i] + " Cost", &mVDEnvImpactCost[eIndex], 0., false, true, "Cost of environmental impact in Currency", SFunctionUnit({ eFTypeDivision, {pCurrency()}, mPossibleEnvImpacts[eIndex].Unit}));
        }
    }
}

void TecEcoAnalysis::setCompoInputParam(const t_mapParamData& aComponent)
{
    if (aComponent.empty()) {
        return; // component creation
    }

    //read non-configuration parameters
    if (mCompoInputParam->readParameters(aComponent) < 0
        || mCompoInputSettings->readParameters(aComponent) < 0
        || mCompoEnvImpactsParam->readParameters(aComponent) < 0)
    {
        throw Cairn_Exception("Error while initializing " + Name() +
            ". A mandatory parameter is missing!", -1);
    }
    
    if (mCurrency.empty()) {
        mCurrency = "EUR";
    }

    //set ObjectiveUnit to Currency if not provided
    if (CairnUtils::getParamValue(aComponent,"ObjectiveUnit") == "") {
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
    const std::string mainCarrier = mMainCarrier ? mMainCarrier->Name() : "";
    auto* component = parentComponent();
    ojson compoObject = ojson::object();

    if(component && component->getGUIData())
        component->getGUIData()->jsonSaveGUILine(compoObject, mainCarrier);

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

    if (component) {
        for (const auto& side : { Left(), Right(), Bottom(), Top() })
        {
            const int portCount = static_cast<int>(component->listSidePorts(side).size());
            if (portCount > 0) {
                nodePorts[side] = portCount;
                component->jsonSaveGUINodePortsData(compoObject["nodePortsData"], side);
            }
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
    // Helper: identify buses contributing to the sub-objective
    auto isSubObjectiveBus = [&](const BusCompo* bus) {
        return bus &&
            bus->ModelClassName() == "ManualObjective" &&
            bus->ObjectiveType() == "Add";
        };

    for (BusCompo* bus : BusComponents())
    {
        if (!isSubObjectiveBus(bus))
            continue;

        MIPModeler::MIPExpression* exp = bus->getMIPExpression("SubObjectiveExpression");
        if (!exp) {
            cWarning() << Name() << ": Missing SubObjectiveExpression on bus " << bus->Name();
            continue;
        }

        mExpSubObjective += *exp;
    }
}

void TecEcoAnalysis::buildTecEcoModel()
{
    /*
    * Computation of Economical and Environmental should be made in computeAllTecEcoContribution()
    * Because they should be computed before adding Bus constraints (Bus::buildModel)
    */

    cInfo() << "Constructing TecEco model and adding related-optimization constraints...";

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

void TecEcoAnalysis::computeTecEcoContribution()
{
    cInfo() << "Computing pre-simulation TecEco expressions...";

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
    auto* compo = parentComponent();
    const double ExtrapolationFactor = compo ? compo->ExtrapolationFactor() : 1.0;
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

void TecEcoAnalysis::computeEconomicalContribution()
{
    const size_t horizon = mTimeSteps.size();
    const size_t Y_SIZE = mTableYearsHours.size();

    auto* tecEcoCompo = parentComponent();
    const double histHours = tecEcoCompo ? tecEcoCompo->HistNbHours() : 0.0;

    for (auto* compo : NonBusMilpComponents())
    {
        const bool ecoInvestModel = compo->EcoInvestModel();
        auto* model = compo->compoModel();
        const bool hasEnv = compo->EnvironmentModel();

        // ---------------------- 0D CAPEX ----------------------
        if (ecoInvestModel)
            mExpCapex += *(compo->getMIPExpression("Capex"));

        // ---------------------- 1D contributions ----------------------
        int year = 0;

        // Cache pointers once per component
        auto* expVarCost1D = compo->getMIPExpression1D("VariableCosts");
        auto* expVarOpex1D = compo->getMIPExpression1D("VariableOpex");
        auto* expFixedOpex1D = ecoInvestModel ? compo->getMIPExpression1D("FixedOpex") : nullptr;
        auto* expRepl1D = ecoInvestModel ? compo->getMIPExpression1D("Replacement") : nullptr;
        auto* expOpex1D = ecoInvestModel ? compo->getMIPExpression1D("Opex") : nullptr;

        // Pre-fetch environmental cost expression names if needed
        std::vector<std::string> envCostExprs;
        if (!ecoInvestModel && hasEnv)
        {
            envCostExprs.reserve(mSelectedEnvImpacts.size());
            for (size_t i = 0; i < mSelectedEnvImpacts.size(); ++i)
                envCostExprs.push_back(model->getEnvImpactCostExpression(i));
        }

        for (size_t t = 0; t < horizon; ++t)
        {
            const double ts = TimeStep(t);

            // Resolve year 
            const uint t_hour = static_cast<uint>(t * ts + 0.999999) + histHours;
            while (year + 1 < Y_SIZE && t_hour > mTableYearsHours[year])
                ++year;

            const double lvl = mLevelizationTable[year];

            // --- Variable Costs ---
            const auto expVarCost = expVarCost1D ? (*expVarCost1D)[t] : MIPModeler::MIPExpression{};
            mExpVariableCosts[t] += expVarCost;

            // --- Variable Opex ---
            const auto expVarOpex = expVarOpex1D ? (*expVarOpex1D)[t] : MIPModeler::MIPExpression{};
            mExpVariableOpex[t] += expVarOpex;

            // --- Fixed Opex & Replacement ---
            MIPModeler::MIPExpression expFixedOpex;
            MIPModeler::MIPExpression expRepl;

            if (ecoInvestModel)
            {
                expFixedOpex = expFixedOpex1D ? (*expFixedOpex1D)[t] : MIPModeler::MIPExpression{};
                expRepl = expRepl1D ? (*expRepl1D)[t] : MIPModeler::MIPExpression{};

                mExpFixedOpex[t] += expFixedOpex;
                mExpReplacement[t] += expRepl;
            }

            // --- Opex ---
            MIPModeler::MIPExpression expOpex;

            if (ecoInvestModel)
            {
                expOpex = expOpex1D ? (*expOpex1D)[t] : MIPModeler::MIPExpression{};
            }
            else
            {
                expOpex = expVarCost + expVarOpex;

                if (hasEnv)
                {
                    for (std::string expr : envCostExprs)
                        expOpex += compo->getMIPExpression1D(t, expr);
                }
            }

            mExpOpex[t] += expOpex;

            // --- Discounted Opex (0D) ---
            mExpOpexDiscounted += expOpex * lvl;
        }
    }
}

void TecEcoAnalysis::computeEnvContribution()
{
    // Resize vectors
    const size_t N_IMPACTS = mSelectedEnvImpacts.size();
    mExpEnvImpactEmbodiedCostVec.resize(N_IMPACTS);
    mExpEnvImpactEmbodiedVec.resize(N_IMPACTS);
    mExpEnvImpactMassVecDiscounted.resize(N_IMPACTS);
    mExpEnvImpactReplacementVecDiscounted.resize(N_IMPACTS);
    mExpCumulativeEnvImpact.resize(N_IMPACTS);

    const size_t horizon = mTimeSteps.size();
    const size_t Y_SIZE = mTableYearsHours.size();

    auto* tecEcoCompo = parentComponent();
    const double histHours = tecEcoCompo ? tecEcoCompo->HistNbHours() : 0.0;

    // Loop over components
    for (auto* compo : NonBusMilpComponents())
    {
        if (!compo->EnvironmentModel())
            continue;

        auto* model = compo->compoModel();
        if (!model)
            continue;

        const auto& impacts = model->getEnvImpacts();

        // Loop over selected impacts
        for (size_t i = 0; i < N_IMPACTS; ++i)
        {
            int year = 0;

            // Pre-fetch expressions for this impact
            const std::string impactMassExpName = model->getEnvImpactMassExpression(i);
            auto* expReplacement1D = impacts[i]->getExpEnvReplacement();
            auto* expOpCost1D = impacts[i]->getExpEnvOpCost();

            // Loop over timestep
            for (size_t t = 0; t < horizon; ++t)
            {
                const double ts = TimeStep(t);

                // Compute hour index
                const uint t_hour = static_cast<uint>(t * ts + 0.999999) + histHours;

                // Resolve year
                while (year + 1 < Y_SIZE && t_hour > mTableYearsHours[year])
                    ++year;

                const double lvl = mImpactLevelizationTable[year];

                // --- Mass impact ---
                const auto expMass = compo->getMIPExpression1D(t, impactMassExpName);
                mExpEnvImpactMassVec[i][t] += expMass;
                mExpEnvImpactMassVecDiscounted[i] += expMass * lvl;

                // --- Replacement impact ---
                const auto expRepl = (*expReplacement1D)[t];
                mExpEnvImpactReplacementVec[i][t] += expRepl;
                mExpEnvImpactReplacementVecDiscounted[i] += expRepl * lvl;

                // --- Cost impact ---
                mExpEnvImpactCostVec[i][t] += (*expOpCost1D)[t];
            }

            // Embodied impacts (0D)
            mExpEnvImpactEmbodiedVec[i] += *(impacts[i]->getExpEnvEmbodied());
            mExpEnvImpactEmbodiedCostVec[i] += *(impacts[i]->getExpEnvEmbodiedCost());

            // Total cumulative impact
            mExpCumulativeEnvImpact[i] =
                mExpEnvImpactMassVecDiscounted[i] +
                mExpEnvImpactEmbodiedVec[i] +
                mExpEnvImpactReplacementVecDiscounted[i];
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

        auto* compo = parentComponent();
        const double ExtrapolationFactor = compo ? compo->ExtrapolationFactor() : 1.0;

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
        SubModel::computeIndicator(mExpFixedOpex, optSol, mFixedOpexContribution.at(0), mFixedOpexContributionDiscounted.at(0), mFixedOpexContribution.at(1), mFixedOpexContributionDiscounted.at(1), false);
        SubModel::computeIndicator(mExpReplacement, optSol, mReplacementContribution.at(0), mReplacementContributionDiscounted.at(0), mReplacementContribution.at(1), mReplacementContributionDiscounted.at(1), false);
        SubModel::computeIndicator(mExpOpex, optSol, mOpexContribution.at(0), mOpexContributionDiscounted.at(0), mOpexContribution.at(1), mOpexContributionDiscounted.at(1), false);

        double opexDiscounted_redundant = mExpOpexDiscounted.evaluate(optSol);
        assert(abs(mOpexContributionDiscounted.at(0) - opexDiscounted_redundant)/(opexDiscounted_redundant+0.001) < 10e-3);

        //NPV = -capex -discounted_opex
        mNetPresentValue.at(0) = -mCapexContribution.at(0) - mOpexContributionDiscounted.at(0);

        double negNpv = mExpNegNPV.evaluate(optSol); //mExpNegNPV is used to add constraints on NPV
        assert(abs(mNetPresentValue.at(0) + negNpv)/(negNpv+0.001) < 10e-3);

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
        mVDEmbodiedCostContribution.resize(mSelectedEnvImpacts.size(), { 0., 0. });
        mVDEnvImpactsReplacementContribution.resize(mSelectedEnvImpacts.size(), { 0., 0. });
        mVDEnvImpactsReplacementContributionDiscounted.resize(mSelectedEnvImpacts.size(), { 0., 0. });
        for (int i = 0; i < mSelectedEnvImpacts.size(); i++) {
            mVDEmbodiedCostContribution[i].at(0) = mVDEmbodiedCostContribution[i].at(1) = mExpEnvImpactEmbodiedCostVec.at(i).evaluate(optSol);
            mVDEmbodiedMassContribution[i].at(0) = mVDEmbodiedMassContribution[i].at(1) = mExpEnvImpactEmbodiedVec.at(i).evaluate(optSol);

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
            mVDEnvImpactsTotalCostDiscounted[i].at(0) = mVDEmbodiedCostContribution.at(i).at(0) + mVDEnvImpactsCostContributionDiscounted.at(i).at(0) ;
            mVDEnvImpactsTotalCostDiscounted[i].at(1) = mVDEmbodiedCostContribution.at(i).at(1) + mVDEnvImpactsCostContributionDiscounted.at(i).at(1) ;
            mVDEnvImpactsTotalMassDiscounted[i].at(0) = mVDEmbodiedMassContribution.at(i).at(0) + mVDEnvImpactsMassContributionDiscounted.at(i).at(0) + mVDEnvImpactsReplacementContributionDiscounted[i].at(0);
            mVDEnvImpactsTotalMassDiscounted[i].at(1) = mVDEmbodiedMassContribution.at(i).at(1) + mVDEnvImpactsMassContributionDiscounted.at(i).at(1) + mVDEnvImpactsReplacementContributionDiscounted[i].at(1);

        }
    }
}

