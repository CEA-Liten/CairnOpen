#include "TecEcoAnalysis.h"

TecEcoAnalysis::TecEcoAnalysis(QObject* aParent, const QMap<QString, QString>& aComponent) :
    mConfigParam(nullptr),
    mCompoInputParam(nullptr),
    mCompoInputSettings(nullptr),
    mCompoEnvImpactsParam(nullptr),
    mEnvImpacts({}),
    mOptimImpactName("")
{    
    mPossibleImpacts = {
    { "Climate change#Global Warming Potential 100", "GWP", "kg CO2 eq"},
    { "Ozone depletion#Ozone Depletion Potential","ODP","kg CFC-11 eq"},
    { "Human toxicity-cancer effects#Comparative Toxic Unit for humans","HTox-c","CTUh"},
    { "Human toxicity-noncancer effects#Comparative Toxic Unit for humans","HTox-nc","CTUh"},
    { "Particulate matter-Respiratory inorganics#Human health effects associated with exposure to PM","PM","-"},
    { "Ionising radiation human health#Human exposure efficiency relative to U","IRP","kBq U"},
    { "Photochemical ozone formation#Tropospheric ozone concentration increase","POCP","kg NMVOC eq"},
    { "Acidification#Accumulated Exceedance","AP", "mol H+ eq"},
    { "Eutrophication-terrestrial#Accumulated Exceedance","EP-t","mol N eq"},
    { "Eutrophication-Aquatic freshwater#Fraction of nutrients reaching freshwater and compartment", "EP-fw","kg P eq"},
    { "Eutrophication-aquatic marine#Fraction of nutrients reaching marine and compartment","EP-m","kg N eq"},
    { "Ecotoxicity freshwater#Comparative Toxic Unit for ecosystems", "Ecotox","CTUe"},
    { "Land use#Soil quality index", "LU","-"},
    { "Water use#User Deprivation Potential", "WU","kg world eq deprived"},
    { "Resource use-Minerals and metals#Abiotic Resource Depletion","ADP-mm","kg Sb eq"},
    { "Resource use-Energy carriers#Abiotic Resource Depletion", "ADP-f","MJ" }
    };

    mVDEnvImpactMaxConstraint.resize(mPossibleImpacts.size(), INFINITY_VAL);
    mVBEnvImpactMaxConstraint.resize(mPossibleImpacts.size());
    mVBEnvImpactMaxConstraint.fill(false);
    mVDEnvImpactCost.resize(mPossibleImpacts.size(), 0.);

    doInit(aComponent);
}

TecEcoAnalysis::~TecEcoAnalysis()
{
    if (mConfigParam) delete mConfigParam;
    if (mCompoInputSettings) delete mCompoInputSettings;
    if (mCompoInputParam) delete mCompoInputParam;
    if (mCompoEnvImpactsParam) delete mCompoEnvImpactsParam;
    if (mGUIData) delete mGUIData;
}

void TecEcoAnalysis::doInit(const QMap<QString, QString>& aComponent)
{
    //Init list of Settings parameters (scalar, double) by direct reading from aSettings file

    //Declare and set configuration parameters
    declareConfigurationParameters();
    int ierr1 = setConfigurationParameters(aComponent);

    //Declare and set non-configuration parameters
    declareCompoInputParam();
    int ierr2 = setCompoInputParam(aComponent);

    //TODO: Ensure that the mandatory parameters exist in TNR .json files ("type", "id", "Model", "Currency", "DiscountRate")
    //if (ierr1 < 0 || ierr2 < 0) {
    //    Cairn_Exception cairn_error("Error reading Parameters of TecEcoAnalysis " + mName, -1);
    //    throw cairn_error;
    //}

    this->setObjectName(mName);

    if (mModelName.contains("OptimEnvImpact"))
    {
        int iSize = mModelName.size() - mModelName.indexOf("-") - 1;
        QString impactShortName = mModelName.right(iSize);
        if (!getPossibleImpactShortNames().contains(impactShortName)) {
            Cairn_Exception cairn_error("Error : unknown model name " + (mModelName)+" on component. Expected: OptimEnvImpact-$ImpactShortName" + (Name()), -1);
            throw cairn_error;
        }
        mOptimImpactName = getImpactLongName(impactShortName);
    }

    //Build list of units of the considered environmental impacts that will be used to export the indicators
    mEnvImpactUnits = getImpactUnitsFromSelectedImpacts(mEnvImpacts);

    //Init Gui data
    if (mGUIData) delete mGUIData;
    mGUIData = new GUIData(this, mName);
    mGUIData->doInit(mType, mType, mType, mXpos, mYpos);
}

void TecEcoAnalysis::declareConfigurationParameters()
{
    mConfigParam = new InputParam(this, "ConfigParam" + mName);
    //QStringList (no default value)
    mConfigParam->addParameter("ConsideredEnvironmentalImpacts", &mEnvImpacts, false, true, "Selected environmental impacts to be considered (EF method)", "-");
}

int TecEcoAnalysis::setConfigurationParameters(const QMap<QString, QString>& aComponent)
{
    int ierr = mConfigParam->readParameters(aComponent);
    return ierr;
}

void TecEcoAnalysis::declareCompoInputParam()
{
    //------------------------ parameters ----------------------------------
    mCompoInputSettings = new InputParam(this, "CompoInputSettings" + mName);
    //bool
    mCompoInputSettings->addParameter("ForceExportAllIndicators", &mForceExportAllIndicators, false, false, true, "Force the export of all Indicators regardless of the values of EcoInvestModel and EnvironmentModel and ExportIndicators");

    //------------------------ options ----------------------------------
    mCompoInputParam = new InputParam(this, "CompoInputParam" + mName);
    //QString
    mCompoInputParam->addParameter("type", &mType, "TecEcoAnalysis", true, true, "Component type", "", {"DONOTSHOW"});
    mCompoInputParam->addParameter("id", &mName, "TecEco", true, true, "Component name", "", {"DONOTSHOW"});
    mCompoInputParam->addParameter("Model", &mModelName, "OptimNPV", true, true, "Model used");
    mCompoInputParam->addParameter("Xpos", &mXpos, 100, false, true, "X position on planteditor", "", {"DONOTSHOW"});
    mCompoInputParam->addParameter("Ypos", &mYpos, 10, false, true, "Y position on planteditor", "", {"DONOTSHOW"});
    mCompoInputParam->addParameter("Currency", &mCurrency, "EUR", true, true, "Currency unit - default to EUR");
    mCompoInputParam->addParameter("ObjectiveUnit", &mObjectiveUnit, "EUR", false, true, "Objective unit - default to currency unit");
    mCompoInputParam->addParameter("Range", &mRange, "HISTandPLAN", false, true, "Evaluation range used for TecEco analysis : HIST = past operation - PLAN = planned operation");
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
    mCompoEnvImpactsParam = new InputParam(this, "CompoEnvImpactsParam" + mName);
    declareEnvImpactParam();
}

void TecEcoAnalysis::declareEnvImpactParam() 
{
    /*
    * These are parameters that depend on the value of the configuration parameter "ConsideredEnvironmentalImpacts".
    * So, every time "ConsideredEnvironmentalImpacts" is modified, they have to get updated. 
    * Ex: API::read_study then set (modify) the value of "ConsideredEnvironmentalImpacts"
    * The method can be generalized to the declaration of "dependent" parameters when needed.
    */
    
    //mVDEnvImpactMaxConstraint.resize(mEnvImpacts.size());
    //mVBEnvImpactMaxConstraint.resize(mEnvImpacts.size());
    //mVDEnvImpactCost.resize(mEnvImpacts.size());
    //Case API: clean unchecked EnvImpacts
    QList<QString> paramNames;
    mCompoEnvImpactsParam->getParameters(paramNames);
    for (int i = 0; i < paramNames.size(); i++) {
        bool found = false;
        for (int j = 0; j < mEnvImpacts.size(); j++) {
            if (paramNames[i].contains(mEnvImpacts[j])) {
                found = true;
                break;
            }
        }
        if (!found) {
            mCompoEnvImpactsParam->removeParameter(paramNames[i]);
        }
    }
    //Add selected but non-existing EnvImpacts. Attention: re-adding already existing parameters may lead into losing their values (case API).
    for (int i = 0; i < mEnvImpacts.size(); i++) {
        int eIndex = getImpactIndex(mEnvImpacts[i]);
        //bool
        if (!paramNames.contains(mEnvImpacts[i] + " MaxConstraint")) {
            mCompoEnvImpactsParam->addParameter(mEnvImpacts[i] + " MaxConstraint", &mVBEnvImpactMaxConstraint[eIndex], false, false, true, "Is max constraint considered?", "-");
        }
        //double 
        if (!paramNames.contains(mEnvImpacts[i] + " MaxConstraintValue")) {
            mCompoEnvImpactsParam->addParameter(mEnvImpacts[i] + " MaxConstraintValue", &mVDEnvImpactMaxConstraint[eIndex], INFINITY_VAL, false, true, "Maximum quantity of environmental impact", "ImpactUnit");
        }
        if (!paramNames.contains(mEnvImpacts[i] + " Cost")) {
            mCompoEnvImpactsParam->addParameter(mEnvImpacts[i] + " Cost", &mVDEnvImpactCost[eIndex], 0., false, true, "Cost of environmental impact in Currency", "EUR/ImpactUnit");
        }
    }
}

int TecEcoAnalysis::setCompoInputParam(const QMap<QString, QString>& aComponent)
{
    //read non-configuration parameters
    if (aComponent.size() != 0) {
        int ierr1 = mCompoInputParam->readParameters(aComponent);
        int ierr2 = mCompoInputSettings->readParameters(aComponent);
        int ierr3 = mCompoEnvImpactsParam->readParameters(aComponent);
        if (ierr1 < 0 || ierr2 < 0 || ierr3 < 0) { return -1; }
    }
    
    //set ObjectiveUnit to Currency if not provided
    if (aComponent["ObjectiveUnit"] == "") {
        mObjectiveUnit = mCurrency;
    }

    if (mNbYearInput == 0) mNbYearInput = 1;

    return 0;
}

void TecEcoAnalysis::jsonSaveGuiComponent(QJsonArray &componentsArray)
{
    QJsonArray paramArray;
    QJsonArray optionArray;
    QJsonArray timeSeriesArray;
    QJsonArray envImpactArray;
    
    mConfigParam->jsonSaveGUIInputParam(paramArray);
    mCompoInputSettings->jsonSaveGUIInputParam(paramArray);
    mCompoInputParam->jsonSaveGUIInputParam(optionArray);
    mCompoEnvImpactsParam->jsonSaveGUIInputParam(envImpactArray);
    
    QJsonObject nodePorts;
    QJsonArray nodePortsArray;

    int portCount = mParentCompo->listSidePorts(Left()).size();
    if (portCount) {
        nodePorts.insert(Left(), QJsonValue::fromVariant(portCount));
        mParentCompo->jsonSaveGUINodePortsData(nodePortsArray, Left());
    }
    portCount = mParentCompo->listSidePorts(Right()).size();
    if (portCount) {
        nodePorts.insert(Right(), QJsonValue::fromVariant(portCount));
        mParentCompo->jsonSaveGUINodePortsData(nodePortsArray, Right());
    }
    portCount = mParentCompo->listSidePorts(Bottom()).size();
    if (portCount) {
        nodePorts.insert(Bottom(), QJsonValue::fromVariant(portCount));
        mParentCompo->jsonSaveGUINodePortsData(nodePortsArray, Bottom());
    }
    portCount = mParentCompo->listSidePorts(Top()).size();
    if (portCount) {
        nodePorts.insert(Top(), QJsonValue::fromVariant(portCount));
        mParentCompo->jsonSaveGUINodePortsData(nodePortsArray, Top());
    }

    QJsonObject compoObject;
    QString mainCarrier = "";  
    if (mEnergyVector) {
        mainCarrier = mEnergyVector->Name();
    }
    mGUIData->jsonSaveGUILine(compoObject, mainCarrier);
    compoObject.insert("nodePorts", nodePorts);
    compoObject.insert("nodePortsData", nodePortsArray);
    compoObject.insert("paramListJson", paramArray);
    compoObject.insert("optionListJson", optionArray);
    compoObject.insert("envImpactsListJson", envImpactArray);
    
    componentsArray.push_back(compoObject) ;
}


std::map<QString, InputParam::ModelParam*> TecEcoAnalysis::getParameters()
{
    std::map<QString, InputParam::ModelParam*> paramMap;

    paramMap.insert(getCompoInputParam()->getMapParams().begin(), getCompoInputParam()->getMapParams().end());
    paramMap.insert(getCompoInputSettings()->getMapParams().begin(), getCompoInputSettings()->getMapParams().end());
    paramMap.insert(getCompoEnvImpactsParam()->getMapParams().begin(), getCompoEnvImpactsParam()->getMapParams().end());

    return paramMap;
}


int TecEcoAnalysis::getImpactIndex(const QString& impactName)
{
    /*
    * returns the index of a given impactName in mPossibleImpacts
    * The goal is to associate a fixed index for every Env Impact regardless if it is selected or not
    */
    int vRet = 0;
    for (auto& vImpact : mPossibleImpacts) {
        if (vImpact.Name() == impactName) {
            break;
        }
        vRet++;
    }
    return vRet;
}

std::vector<double> TecEcoAnalysis::EnvImpactCosts() {
    /*
    * Only return values for the selected impacts!
    */
    std::vector<double> aVDEnvImpactCost;
    aVDEnvImpactCost.resize(mEnvImpacts.size(), 0.);
    if (mVDEnvImpactCost.size() < mEnvImpacts.size())
        return aVDEnvImpactCost;
    for (int i = 0; i < mEnvImpacts.size(); i++)
    {
        int eIndex = getImpactIndex(mEnvImpacts[i]);
        aVDEnvImpactCost[i] = mVDEnvImpactCost[eIndex];
    }
    return aVDEnvImpactCost;
}

QStringList TecEcoAnalysis::getPossibleImpactNames() const
{
    QStringList vRet;
    for (auto& vImpact : mPossibleImpacts) {
        vRet.push_back(vImpact.Name());
    }
    return vRet;
}

QStringList TecEcoAnalysis::getPossibleImpactShortNames() const
{
    QStringList vRet;
    for (auto& vImpact : mPossibleImpacts) {
        vRet.push_back(vImpact.ShortName());
    }
    return vRet;
}

QStringList TecEcoAnalysis::getImpactUnitsFromSelectedImpacts(const QStringList& aSelectedImpactsList) {
    QList<QString> selectedImpactUnitsList;
    for (int i = 0; i < aSelectedImpactsList.size(); i++) {
        bool found = false;
        for (auto& vImpact : mPossibleImpacts) {
            if (aSelectedImpactsList[i] == vImpact.Name()) {
                selectedImpactUnitsList.append(vImpact.Unit());
                found = true;
                break;
            }
        }
        if (!found) {
            selectedImpactUnitsList.append("-");
        }
    }
    return selectedImpactUnitsList;
}

QString TecEcoAnalysis::getImpactShortName(const QString& name) {//name can be any string that contains an impact name as a sub-string
    for (auto& vImpact : mPossibleImpacts) {
        if (name.contains(vImpact.Name())) {
            QString shortName = name;
            return shortName.replace(vImpact.Name(), vImpact.ShortName());
        }
    }
    return name;
}

QString TecEcoAnalysis::getImpactLongName(const QString& name) {//name can be any string that contains an impact short name as a sub-string
    for (auto& vImpact : mPossibleImpacts) {
        if (name.contains(vImpact.ShortName()) && !name.contains(vImpact.Name())) {
            QString longName = name;
            return longName.replace(vImpact.ShortName(), vImpact.Name());
        }
    }
    return name;
}



// ******************************************* CompoModel ************************************************ //

void TecEcoAnalysis::setTimeData()
{
    SubModel::setTimeData();
}

void TecEcoAnalysis::buildModel()
{
    /*
    * Computation of Economical and Environmental should be made in computeAllTecEcoContribution()
    * Because they should be computed before adding Bus constraints (Bus::buildModel)
    */

    //Has to stay here because PenaltyConstraintCost is computed from Bus componenets (So, it has to be done after Bus::buildModel)
    TecEcoAnalysis::computePenaltyConstraintCosts();

    //Objective
    if (mModelName.contains("OptimEnvImpact")) {
        for (int i = 0; i < mEnvImpactsList.size(); i++) {
            if (mEnvImpactsList[i] == mOptimImpactName) { 
                mExpObjective += mExpEnvImpactMassVecDiscounted[i];
                mExpObjective += mExpEnvGreyImpactMassVec[i];
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
    for (int i = 0; i < mEnvImpacts.size(); i++) {
        int eIndex = getImpactIndex(mEnvImpacts.at(i));
        if (mVBEnvImpactMaxConstraint[eIndex]) addConstraint(mExpEnvImpactMassVecDiscounted.at(eIndex) + mExpEnvGreyImpactMassVec.at(eIndex) <= mVDEnvImpactMaxConstraint[eIndex], "EnvImpactMaxConstraint");
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
    }

    for (int i = 0; i < mEnvImpactsList.size(); i++)
    {
        mExpEnvGreyImpactMassUndiscountedVec[i] = mExpEnvGreyImpactMassVec[i] * ExtrapolationFactor;
        for (unsigned int t = 0; t < mTimeSteps.size(); ++t)
        {
            mExpEnvImpactMassUndiscountedVec[i][t] = mExpEnvImpactMassVec[i][t] * ExtrapolationFactor;
        }
    }
}

void TecEcoAnalysis::resizeEnvImpactVectors() {
    // ----------- Expressions ------------------------
    mExpEnvGreyImpactCostVec.resize(mEnvImpactsList.size());
    mExpEnvGreyImpactMassVec.resize(mEnvImpactsList.size());
    mExpEnvImpactCostVec.resize(mEnvImpactsList.size());
    mExpEnvImpactMassVec.resize(mEnvImpactsList.size());
    mExpEnvImpactMassVecDiscounted.resize(mEnvImpactsList.size());//Used to add constraints in buildModel()

    //Used to publish on ports (IO interface)
    mExpEnvGreyImpactMassUndiscountedVec.resize(mEnvImpactsList.size());
    mExpEnvImpactMassUndiscountedVec.resize(mEnvImpactsList.size());

    // ----------- Contribution Vectors ------------------------
    //Grey
    mVDEnvGreyImpactsCostContribution.resize(mEnvImpactsList.size(), { 0., 0. });
    mVDEnvGreyImpactsMassContribution.resize(mEnvImpactsList.size(), { 0., 0. });

    //Undiscounted
    mVDEnvImpactsCostContribution.resize(mEnvImpactsList.size(), { 0., 0. });
    mVDEnvImpactsMassContribution.resize(mEnvImpactsList.size(), { 0., 0. });

    //Discounted
    mVDEnvImpactsCostContributionDiscounted.resize(mEnvImpactsList.size(), { 0., 0. });
    mVDEnvImpactsMassContributionDiscounted.resize(mEnvImpactsList.size(), { 0., 0. });

    mVDEnvImpactsTotalCostDiscounted.resize(mEnvImpactsList.size(), { 0., 0. });
    mVDEnvImpactsTotalMassDiscounted.resize(mEnvImpactsList.size(), { 0., 0. });

    //for (int i = 0; i < mEnvImpactsList.size(); i++) {
    //    mVDEnvGreyImpactsCostContribution[i] = { 0., 0. };
    //    mVDEnvGreyImpactsMassContribution[i] = { 0., 0. };
    //    mVDEnvImpactsCostContribution[i] = { 0., 0. };
    //    mVDEnvImpactsMassContribution[i] = { 0., 0. };
    //    mVDEnvImpactsCostContributionDiscounted[i] = { 0., 0. };
    //    mVDEnvImpactsMassContributionDiscounted[i] = { 0., 0. };
    //}
}

void TecEcoAnalysis::allocateExpressions() {
    //Only 1D
    mExpPureOpex = MIPModeler::MIPExpression1D(mTimeSteps.size());
    mExpReplacement = MIPModeler::MIPExpression1D(mTimeSteps.size());
    mExpVariableCosts = MIPModeler::MIPExpression1D(mTimeSteps.size());
    mExpOpex = MIPModeler::MIPExpression1D(mTimeSteps.size());

    mExpOpexUndiscounted = MIPModeler::MIPExpression1D(mTimeSteps.size());
    mExpVariableCostsUndiscounted = MIPModeler::MIPExpression1D(mTimeSteps.size());

    for (int i = 0; i < mEnvImpactsList.size(); i++)
    {
        mExpEnvImpactMassVec[i] = MIPModeler::MIPExpression1D(mTimeSteps.size());
        mExpEnvImpactCostVec[i] = MIPModeler::MIPExpression1D(mTimeSteps.size());

        mExpEnvImpactMassUndiscountedVec[i] = MIPModeler::MIPExpression1D(mTimeSteps.size());
    }
}

void TecEcoAnalysis::closeExpressions() {
    //Economical 0D
    closeExpression(mExpObjective);
    closeExpression(mExpNegNPV);
    closeExpression(mExpSubObjective);
    closeExpression(mExpPenaltyConstraintCosts);
    closeExpression(mExpCapex);

    closeExpression(mExpOpexDiscounted);

    //Economical 1D
    closeExpression1D(mExpOpex);
    closeExpression1D(mExpPureOpex);
    closeExpression1D(mExpReplacement);
    closeExpression1D(mExpVariableCosts);

    //Env Impacts
    for (int i = 0; i < mEnvImpactsList.size(); i++) {
        //0D
        closeExpression(mExpEnvGreyImpactCostVec[i]);
        closeExpression(mExpEnvGreyImpactMassVec[i]);

        closeExpression(mExpEnvImpactMassVecDiscounted[i]);

        //1D
        closeExpression1D(mExpEnvImpactCostVec[i]);
        closeExpression1D(mExpEnvImpactMassVec[i]);
    }
}

void TecEcoAnalysis::computePenaltyConstraintCosts() {
    //Compute the total PenaltyConstraintCosts from all NodeLaws
    QMapIterator<QString, MilpComponent*> icomponent(mMapMilpComponents);
    while (icomponent.hasNext())
    {
        icomponent.next();
        MilpComponent* lptr = icomponent.value();
        if (lptr->getMIPExpression("PenaltyConstraintCosts") != nullptr) {
            mExpPenaltyConstraintCosts += *(lptr->getMIPExpression("PenaltyConstraintCosts"));
        }
    }
}

void TecEcoAnalysis::computeEconomicalContribution() {
    QMapIterator<QString, MilpComponent*> icomponent(mMapMilpComponents);
    while (icomponent.hasNext())
    {
        icomponent.next();
        MilpComponent* lptr = icomponent.value();

        // ---------------------- 0D ----------------------
        if (lptr->EcoInvestModel()) {
            mExpCapex += *(lptr->getMIPExpression("Capex"));
        }

        if (lptr->ModelClassName() == "ManualObjective") {
            MIPModeler::MIPExpression* expSubObjective = lptr->getMIPExpression("SubObjectiveExpression");
            mMapSubObjective[icomponent.key()] = expSubObjective;
            if (lptr->ObjectiveType() == "Add") {
                mExpSubObjective += *expSubObjective;
            }
        }

        // ---------------------- 1D ----------------------
        int year = 0;
        for (unsigned int t = 0; t < mTimeSteps.size(); ++t)
        {
            MIPModeler::MIPExpression expVarCost_t;
            MIPModeler::MIPExpression expPureOpex_t;
            MIPModeler::MIPExpression exReplacement_t;
            MIPModeler::MIPExpression expOpex_t;

            uint t_hour = qCeil(t * TimeStep(t)) + mParentCompo->HistNbHours();
            while (t_hour > mParentCompo->TableYearsHours().at(year) && year < mParentCompo->TableYearsHours().size() - 1) {
                year += 1;
            }

            if (lptr->getMIPExpression1D("VariableCosts") != nullptr) {
                expVarCost_t = lptr->getMIPExpression1D(t, "VariableCosts");
            }

            if (lptr->EcoInvestModel()) {
                expPureOpex_t = lptr->getMIPExpression1D(t, "PureOpex");
                exReplacement_t = lptr->getMIPExpression1D(t, "Replacement");
                //Net opex includes all costs (Variable Costs, Replacement and Env Impact Cost)
                expOpex_t = lptr->getMIPExpression1D(t, "Opex");
            }//Count Variable Costs and Env Impact Costs for Net Opex if EcoInvestModel == false
            else {
                //Variable Costs exists for all models even when EcoInvestModel == false
                expOpex_t = expVarCost_t;
                //Env Impact Costs exist only for TechnicalSubModel if EnvironmentModel == true 
                if (lptr->EnvironmentModel()) {
                    for (int i = 0; i < mEnvImpactsList.size(); i++) {
                        expOpex_t += lptr->getMIPExpression1D(t, lptr->compoModel()->getEnvImpactCostExpression(i));
                    }
                }
            }
            mExpVariableCosts[t] += expVarCost_t;
            mExpPureOpex[t] += expPureOpex_t;
            mExpReplacement[t] += exReplacement_t;
            mExpOpex[t] += expOpex_t;
            //0D Discounted Opex Exp used to compute mExpObjective and add constraints in buildModel()
            mExpOpexDiscounted += expOpex_t * mParentCompo->LevelizationTable().at(year);
        }
    }
}

void TecEcoAnalysis::computeEnvContribution()
{   //Computes total Env contribution
    QMapIterator<QString, MilpComponent*> icomponent(mMapMilpComponents);
    while (icomponent.hasNext())
    {
        icomponent.next();
        MilpComponent* lptr = icomponent.value();

        if (lptr->EnvironmentModel()) {
            for (int i = 0; i < mEnvImpactsList.size(); i++)
            {
                int year = 0;
                for (unsigned int t = 0; t < mTimeSteps.size(); ++t)
                {
                    MIPModeler::MIPExpression expImpactMass_i_t;
                    MIPModeler::MIPExpression expImpactCost_i_t;

                    uint t_hour = qCeil(t * TimeStep(t)) + mParentCompo->HistNbHours();
                    while (t_hour > mParentCompo->TableYearsHours().at(year) && year < mParentCompo->TableYearsHours().size() - 1) {
                        year += 1;
                    }

                    expImpactMass_i_t = lptr->getMIPExpression1D(t, lptr->compoModel()->getEnvImpactMassExpression(i));
                    mExpEnvImpactMassVec[i][t] += expImpactMass_i_t;
                    //0D Discounted Mass Exp used to add constraints in buildModel()
                    mExpEnvImpactMassVecDiscounted[i] += expImpactMass_i_t * mParentCompo->ImpactLevelizationTable().at(year);

                    expImpactCost_i_t = lptr->getMIPExpression1D(t, lptr->compoModel()->getEnvImpactCostExpression(i));
                    mExpEnvImpactCostVec[i][t] += expImpactCost_i_t;
                }

                // ---------------------------- Grey Env Impacts ----------------------------
                mExpEnvGreyImpactMassVec[i] += *(lptr->getMIPExpression(lptr->compoModel()->getEnvGreyImpactMassExpression(i)));
                mExpEnvGreyImpactCostVec[i] += *(lptr->getMIPExpression(lptr->compoModel()->getEnvGreyImpactCostExpression(i)));
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

    QMapIterator<QString, MilpComponent*> icomponent(mMapMilpComponents);
    while (icomponent.hasNext())
    {
        icomponent.next();
        MilpComponent* lptr = icomponent.value();

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
        mNbYearIndicator.at(0) = mNbYearIndicator.at(1) = mParentCompo->NbYear();
        mDiscountRateIndicator.at(0) = mDiscountRateIndicator.at(1) = mParentCompo->DiscountRate();
        mImpactDiscountRateIndicator.at(0) = mImpactDiscountRateIndicator.at(1) = mParentCompo->ImpactDiscountRate();
        mNbYearInputIndicator.at(0) = mNbYearInputIndicator.at(1) = mParentCompo->NbYearInput();
        mLeapYearPosIndicator.at(0) = mLeapYearPosIndicator.at(1) = mParentCompo->LeapYearPos();
        mNbYearOffsetIndicator.at(0) = mNbYearOffsetIndicator.at(1) = mParentCompo->NbYearOffset();
        mDiscountFactorIndicator.at(0) = levelization(mParentCompo->DiscountRate(), mParentCompo->NbYear(), mParentCompo->NbYearOffset(), 1); //PLAN
        mDiscountFactorIndicator.at(1) = levelization(mParentCompo->DiscountRate(), mParentCompo->NbYear(), mParentCompo->NbYearOffset(), 1); //HIST
        if (mParentCompo->LevelizationTable().size() > 1) {
            for (int i = 0; i < mParentCompo->LevelizationTable().size(); i++) {
                mDiscountFactorListIndicator.at(i).at(0) = mDiscountFactorListIndicator.at(i).at(1) = mParentCompo->LevelizationTable().at(i);
            }
        }
        mImpactDiscountFactorIndicator.at(0) = levelization(mParentCompo->ImpactDiscountRate(), mParentCompo->NbYear(), mParentCompo->NbYearOffset(), 1); //PLAN
        mImpactDiscountFactorIndicator.at(1) = levelization(mParentCompo->ImpactDiscountRate(), mParentCompo->NbYear(), mParentCompo->NbYearOffset(), 1); //HIST
        if (mParentCompo->ImpactLevelizationTable().size() > 1) {
            for (int i = 0; i < mParentCompo->ImpactLevelizationTable().size(); i++) {
                mImpactDiscountFactorListIndicator.at(i).at(0) = mImpactDiscountFactorListIndicator.at(i).at(1) = mParentCompo->ImpactLevelizationTable().at(i);
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
        SubModel::computeIndicator(mExpPureOpex, optSol, mPureOpexContribution.at(0), mPureOpexContributionDiscounted.at(0), mPureOpexContribution.at(1), mPureOpexContributionDiscounted.at(1), false);
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
        if (mParentCompo->InternalRateOfReturn() >= 0.) {
            mInternalRateOfReturnPerCent.at(0) = mInternalRateOfReturnPerCent.at(1) = mParentCompo->InternalRateOfReturn() * 100.;
            mInternalRateOfReturnFactor.at(0) = mInternalRateOfReturnFactor.at(1) = mParentCompo->RateOfReturnDiscountFactor();
            mNetPresentValueAtIRR.at(0) = -mCapexContribution.at(0) + (mOpexContribution.at(0) * mParentCompo->RateOfReturnDiscountFactor());
            mNetPresentValueAtIRR.at(1) = -mCapexContribution.at(1) + (mOpexContribution.at(1) * mParentCompo->RateOfReturnDiscountFactor());
        }

        mAverageRateOfReturnFactor.at(0) = -mCapexContribution.at(0) / (mOpexContribution.at(0) / ExtrapolationFactor); //PLAN
        mAverageRateOfReturnFactor.at(1) = -mCapexContribution.at(1) / mOpexContribution.at(1); //HIST
        mAverageRateOfReturnPerCent.at(0) = 100. * discountRate(mAverageRateOfReturnFactor.at(0), mParentCompo->NbYear(), mParentCompo->NbYearOffset(), ExtrapolationFactor); //PLAN
        mAverageRateOfReturnPerCent.at(1) = 100. * discountRate(mAverageRateOfReturnFactor.at(1), mParentCompo->NbYear(), mParentCompo->NbYearOffset(), ExtrapolationFactor);  //HIST
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
        for (int i = 0; i < mEnvImpactsList.size(); i++) {
            mVDEnvGreyImpactsCostContribution[i].at(0) = mVDEnvGreyImpactsCostContribution[i].at(1) = mExpEnvGreyImpactCostVec.at(i).evaluate(optSol);
            mVDEnvGreyImpactsMassContribution[i].at(0) = mVDEnvGreyImpactsMassContribution[i].at(1) = mExpEnvGreyImpactMassVec.at(i).evaluate(optSol);

            SubModel::computeIndicator(mExpEnvImpactCostVec[i], optSol, mVDEnvImpactsCostContribution[i].at(0), mVDEnvImpactsCostContributionDiscounted[i].at(0), mVDEnvImpactsCostContribution[i].at(1), mVDEnvImpactsCostContributionDiscounted[i].at(1), true);
            SubModel::computeIndicator(mExpEnvImpactMassVec[i], optSol, mVDEnvImpactsMassContribution[i].at(0), mVDEnvImpactsMassContributionDiscounted[i].at(0), mVDEnvImpactsMassContribution[i].at(1), mVDEnvImpactsMassContributionDiscounted[i].at(1), true);

            double envImpactMassDiscounted_redundant_i = mExpEnvImpactMassVecDiscounted.at(i).evaluate(optSol);
            assert(abs(mVDEnvImpactsMassContributionDiscounted[i].at(0) - envImpactMassDiscounted_redundant_i) < 10e-3);

            //Flow + Grey
            mVDEnvImpactsTotalCostDiscounted[i].at(0) = mVDEnvGreyImpactsCostContribution.at(i).at(0) + mVDEnvImpactsCostContributionDiscounted.at(i).at(0);
            mVDEnvImpactsTotalCostDiscounted[i].at(1) = mVDEnvGreyImpactsCostContribution.at(i).at(1) + mVDEnvImpactsCostContributionDiscounted.at(i).at(1);
            mVDEnvImpactsTotalMassDiscounted[i].at(0) = mVDEnvGreyImpactsMassContribution.at(i).at(0) + mVDEnvImpactsMassContributionDiscounted.at(i).at(0);
            mVDEnvImpactsTotalMassDiscounted[i].at(1) = mVDEnvGreyImpactsMassContribution.at(i).at(1) + mVDEnvImpactsMassContributionDiscounted.at(i).at(1);

        }
    }
}

