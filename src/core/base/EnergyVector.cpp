#include "EnergyVector.h"
#include "MilpComponent.h"
#include "CarrierTypes.h"
#include "OrUnitsConverter.h"

EnergyVector::EnergyVector(CairnObject* aParent, const std::string& aName, const std::string& aType, 
    const std::map<std::string, std::string> aComponent)
    : CairnObject(aParent, aName),
    mCarrierType(aType),
    mEnergyColour(""),
    mIsHeatCarrier(false),
    mIsMassCarrier(false),
    mIsFuelCarrier(false)
{
    setObjectType("EnergyVector");
    declareConfigurationParameters();
    setConfigurationParameters(aComponent);
    declareCompoInputParam();
    setCompoInputParam(aComponent);
    if (aComponent.find("Type") == aComponent.end()) {
        mCarrierType = aType;
    }
    InitEnergyVectorParam(aComponent);

    cInfo() << " Energy Vector " <<objectName() << " of type " << mCarrierType;
    cInfo() << " Energy Vector " << objectName() << " use MassCarrier property " << isMassCarrier() << " RHO " << mRHO ;
    cInfo() << " Energy Vector " << objectName() << " use HeatCarrier property " << isHeatCarrier() << " CP  " << mCP  ;
    cInfo() << " Energy Vector " << objectName() << " use FuelCarrier property " << isFuelCarrier() << " LHV " << mLHV ;
}

EnergyVector::~EnergyVector()
{
    if (mGUIData) delete mGUIData;
    if (mConfigParam) delete mConfigParam;
    if (mCompoInputParam) delete mCompoInputParam;
    if (mCompoInputSettings) delete mCompoInputSettings;
    if (mTimeSeriesParam) delete mTimeSeriesParam;
    if (mGridTimeSeries) delete mGridTimeSeries;
}

void EnergyVector::declareConfigurationParameters()
{
    mConfigParam = new InputParam(this, "ConfigParam" + Name());
    //bool
    /** isMandatory is false because UseProfileLHV and UseProfileGHV are missing in old studies */
    mConfigParam->addParameter("UseProfileLHV", &mUseProfileLHV, false, false, true, "If true use timeseries LHV, otherwise use scalar value.", "bool");
    mConfigParam->addParameter("UseProfileGHV", &mUseProfileGHV, false, false, true, "If true use timeseries LHV, otherwise use scalar value.", "bool");
}

void EnergyVector::setConfigurationParameters(const std::map<std::string, std::string>& aComponent)
{
    if (aComponent.empty()) {
        return; // component creation
    }

    CairnUtils::checkRead(mConfigParam->readParameters(aComponent), Name());
}

void EnergyVector::declareCompoInputParam()
{
    //------------------------------ options ------------------------------
    mCompoInputParam = new InputParam (this,"CompoInputParam"+ Name()) ;
    //std::string
    mCompoInputParam->addParameter("Type", &mCarrierType, "Electrical", false, true,"Energy vector type specifying the energy form among: Electrical - Thermal - Fluid - Material");
    mCompoInputParam->addParameter("MassUnit", &mMassUnit, "kg", false, true,"Unit to be used for mass - default is kg","-");
    mCompoInputParam->addParameter("EnergyUnit", &mEnergyUnit, "MWh", false, true,"Unit to be used for energy - default is MWh","-");
    mCompoInputParam->addParameter("PowerUnit", &mPowerUnit, "MW", false, true,"Unit to be used for power - default is MW","-");
    mCompoInputParam->addParameter("FlowrateUnit", &mFlowrateUnit, "kg/h", false, true,"Unit to be used for mass flow - default is kg/h","-");
    mCompoInputParam->addParameter("FluxUnit", &mFluxUnit, "", false, true,"Unit to be used for Flow","", "DONOTSHOW");
    mCompoInputParam->addParameter("FluxName", &mFluxName, "", false, true, "Name to be used for Flow", "", "UnitNames");
    mCompoInputParam->addParameter("StorageName", &mStorageName, "", false, true, "Name to be used for storage capacity", "", "UnitNames");
    mCompoInputParam->addParameter("StorageUnit", &mStorageUnit, "", false, true, "Unit to be used for storage capacity", "", "DONOTSHOW");
    mCompoInputParam->addParameter("PotentialName", &mPotentialName, "", false, true,"Name to be used for potential eg Pressure - Temperature...");
    mCompoInputParam->addParameter("PotentialUnit", &mPotentialUnit, "", false, true,"Unit to be used for constant potential","Bar");
    mCompoInputParam->addParameter("EnergyName", &mEnergyName, "Energy", false, true,"Name to be used for Energy","", "UnitNames");
    mCompoInputParam->addParameter("PowerName", &mPowerName, "Power", false, true,"Name to be used for Power","", "UnitNames");
    
    //bool: should be moved to declareConfigurationParameters but this breaks TNR due to missing parameters
    mCompoInputParam->addParameter("IsMassCarrier",&mIsMassCarrier, false, false, true,"Option meaning mass is carried by energy vector - Default to false for Electrical and Thermal types - true for Fluids and Material");
    mCompoInputParam->addParameter("IsHeatCarrier",&mIsHeatCarrier, false, false, true,"Option giving heat capacity ability to energy vector - Default to true for Electrical and Thermal types - false for Fluids and Material");
    mCompoInputParam->addParameter("IsFuelCarrier",&mIsFuelCarrier, false, false, true,"Option giving heating vaue ability to energy vector - Default to false for Electrical and Thermal types or non fuel Fluids - true for Fluid and Material fuels");
   
    //------------------------------ ! timeseries Names ! ------------------------------
    mTimeSeriesParam = new InputParam(this, "TimeSeriesSettings" + Name());
    //std::string
    mTimeSeriesParam->addParameter(ProfileLHV(), &mProfileLHV, "", SFunctionFlag({ eFTypeNotAnd, {}, {&mUseProfileLHV, &mIsFuelCarrier} }),
        SFunctionFlag({ eFTypeNotAnd, {}, {&mUseProfileLHV , &mIsFuelCarrier} }), "Profile of heat value of fuel type carriers - Use 1. for pure energy model",
        SFunctionUnit({ eFTypeDivision, { &mEnergyUnit, &mMassUnit } }));
    mTimeSeriesParam->addParameter(ProfileGHV(), &mProfileGHV, "", SFunctionFlag({ eFTypeNotAnd, {}, {&mUseProfileGHV, &mIsFuelCarrier} }),
        SFunctionFlag({ eFTypeNotAnd, {}, {&mUseProfileGHV , &mIsFuelCarrier} }), "Profile of gross Heat Value - Use 1. for pure energy model ",
        SFunctionUnit({ eFTypeDivision, { &mEnergyUnit, &mMassUnit } }));
   
    mGridTimeSeries = new InputParam(this, "GridTimeSeries" + Name());
    mGridTimeSeries->addParameter("UseProfileBuyPrice", &mUseProfileBuyPrice, "", false, true,"Optional buy price time profile", SFunctionUnit({ eFTypeDivision, { &mCurrency, &mStorageUnit } }));
    mGridTimeSeries->addParameter("UseProfileSellPrice", &mUseProfileSellPrice, "", false, true,"Optional sell price time profile", SFunctionUnit({ eFTypeDivision, { &mCurrency, &mStorageUnit } }));
    mGridTimeSeries->addParameter("UseProfileBuyPriceSeasonal", &mUseProfileBuyPriceSeasonal, "", false, true, "Optional buy price seasonal time profile", SFunctionUnit({ eFTypeDivision, { &mCurrency, &mStorageUnit } }));
    //------------------------------ parameters ------------------------------
    mCompoInputSettings = new InputParam (this,"CompoInputSettings"+ Name()) ;
    //double
    mCompoInputSettings->addParameter("Potential", &mPotential, 0., false, true, "Voltage- Pressure- Temperature", "V-Bar-degC");
    mCompoInputSettings->addParameter("LHV", &mLHV, 1., SFunctionFlag({ eFTypeNotAnd, {&mUseProfileLHV}, {&mIsFuelCarrier} }),
        SFunctionFlag({ eFTypeNotAnd, {&mUseProfileLHV}, {&mIsFuelCarrier} }), "Heat Value of fuel type carriers - Use 1. for pure energy model",
        SFunctionUnit({ eFTypeDivision, { &mEnergyUnit, &mMassUnit } }));
    mCompoInputSettings->addParameter("GHV", &mGHV, 0., SFunctionFlag({ eFTypeNotAnd, {&mUseProfileGHV}, {&mIsFuelCarrier} }),
        SFunctionFlag({ eFTypeNotAnd, {&mUseProfileGHV}, {&mIsFuelCarrier} }), "Gross Heat Value - Use 1. for pure energy model ",
        SFunctionUnit({ eFTypeDivision, { &mEnergyUnit, &mMassUnit } }));
    mCompoInputSettings->addParameter("CP", &mCP, 0., &mIsHeatCarrier, true, "Heat Capacity of heat carriers ", "J/K/kg");
    mCompoInputSettings->addParameter("RHO", &mRHO, 0., &mIsMassCarrier, true, "Density of fluid type carriers", "kg/m3");
    mCompoInputSettings->addParameter("BuyPrice", &mBuyPrice, 0., false, true, "Constant BuyPrice per mass or energy units", SFunctionUnit({ eFTypeDivision, { &mCurrency, &mStorageUnit } }));
    mCompoInputSettings->addParameter("BuyPriceSeasonal", &mBuyPriceSeasonal, 0., false, true, "Constant BuyPriceSeasonal per mass or energy units", SFunctionUnit({ eFTypeDivision, { &mCurrency, &mStorageUnit } }));
    mCompoInputSettings->addParameter("SellPrice", &mSellPrice, 0., false, true, "Constant SellPrice per mass or energy units", SFunctionUnit({ eFTypeDivision, { &mCurrency, &mStorageUnit } }));
}

void EnergyVector::setCompoInputParam(const std::map<std::string, std::string>& aComponent)
{
    if (aComponent.empty()) {
        return; // component creation
    }

    CairnUtils::checkRead(mCompoInputParam->readParameters(aComponent), Name());
    CairnUtils::checkRead(mCompoInputSettings->readParameters(aComponent), Name());
    CairnUtils::checkRead(mTimeSeriesParam->readParameters(aComponent), Name());
    CairnUtils::checkRead(mGridTimeSeries->readParameters(aComponent), Name());

    mEnergyColour = CairnUtils::getParam(aComponent, "EnergyColor");
    mModel = CairnUtils::getParam(aComponent, "Model");
}

bool EnergyVector::InitEnergyVectorParam(const std::map<std::string, std::string> &aComponent)
{
    if (mGUIData) delete mGUIData;
    mGUIData = new GUIData(this);
    if (mModel == "") {
        mModel = getDefaultEnergyVectorType();
    }
    mGUIData->doInit(mModel, mModel, "EnergyVector", { {"Xpos", CairnUtils::getParam(aComponent,"Xpos")}, {"Ypos", CairnUtils::getParam(aComponent,"Ypos")} });

    if (mCarrierType == "")
    {
        cCritical() << "<Type> attribute is void !! A Type among Fluid, Material, Thermal or Electrical should be given for EnergyVector " << (objectName());
        Cairn_Exception erreur("void <Type> attribute is not allowed, Type among Fluid, Material, Thermal or Electrical should be given for EnergyVector " + objectName(), -1);
        throw erreur;
    }
    if (!CairnUtils::contains(mCarrierType, { "Fluid",
        "Material",
        "Wood",
        "BioMass",
        "Electrical",
        "Thermal" }))
    {
        cCritical() << "<Type> attribute error " << (objectName());
        Cairn_Exception erreur("<Type> attribute must be one among Fluid*, BioMass, Wood, Material, Thermal or Electrical for EnergyVector " + objectName(), -1);
        throw erreur;
    }

    assert(mCarrierType != "");

    if (mFluxUnit == "")
    {
        if (CairnUtils::contains(mCarrierType, { "Fluid",
            "Material",
            "Wood",
            "BioMass" }))    mFluxUnit = mFlowrateUnit;
        if (CairnUtils::contains(mCarrierType, "Electrical"))    mFluxUnit = mPowerUnit;
        if (CairnUtils::contains(mCarrierType, "Thermal"))    mFluxUnit = mPowerUnit;
    }
    else
    {
        if (CairnUtils::contains(mCarrierType, { "Fluid",
            "Material",
            "Wood",
            "BioMass" }))    mFlowrateUnit = mFluxUnit;
        if (CairnUtils::contains(mCarrierType, "Electrical"))    mPowerUnit = mFluxUnit;
        if (CairnUtils::contains(mCarrierType, "Thermal"))    mPowerUnit = mFluxUnit;
    }

    if (mStorageUnit == "")
    {
        if (CairnUtils::contains(mCarrierType, { "Fluid",
            "Material",
            "Wood",
            "BioMass" }))    mStorageUnit = mMassUnit;
        if (CairnUtils::contains(mCarrierType, "Electrical"))    mStorageUnit = mEnergyUnit;
        if (CairnUtils::contains(mCarrierType, "Thermal"))       mStorageUnit = mEnergyUnit;
    }
    else
    {
        if (CairnUtils::contains(mCarrierType, { "Fluid",
            "Material",
            "Wood",
            "BioMass" }))      mMassUnit = mStorageUnit;
        if (CairnUtils::contains(mCarrierType, "Electrical"))   mEnergyUnit = mStorageUnit;
        if (CairnUtils::contains(mCarrierType, "Thermal"))      mEnergyUnit = mStorageUnit;
    }
    if (mFluxName == "")
    {
        if (CairnUtils::contains(mCarrierType, { "Fluid",
            "Material",
            "Wood",
            "BioMass" }))      mFluxName = mCarrierType + "Flowrate";
        if (CairnUtils::contains(mCarrierType, "Electrical"))   mFluxName = mCarrierType + "Power";
        if (CairnUtils::contains(mCarrierType, "Thermal"))      mFluxName = mCarrierType + "Power";
    }
    if (mStorageName == "")
    {
        if (CairnUtils::contains(mCarrierType, { "Fluid",
            "Material",
            "Wood",
            "BioMass" }))      mStorageName = mCarrierType + "Mass";
        if (CairnUtils::contains(mCarrierType, "Electrical"))   mStorageName = mCarrierType + "Energy";
        if (CairnUtils::contains(mCarrierType, "Thermal"))      mStorageName = mCarrierType + "Energy";
    }
    if (mPotentialName == "")
    {
        if (CairnUtils::contains(mCarrierType, "Fluid"))      mPotentialName = "Pressure";
        if (CairnUtils::contains(mCarrierType, { "Wood",
            "BioMass",
            "Material" }))   mPotentialName = "Xmassfract";
        if (CairnUtils::contains(mCarrierType, "Electrical")) mPotentialName = "Voltage";
        if (CairnUtils::contains(mCarrierType, "Thermal"))    mPotentialName = "Temperature"; // and Pressure ?? classes derivees a prevoir entre Thermal et Fluid
    }
    if (mPotentialUnit == "")
    {
        if (CairnUtils::contains(mCarrierType, "Fluid"))       mPotentialUnit = "bar";
        if (CairnUtils::contains(mCarrierType, { "Wood",
            "BioMass",
            "Material" }))    mPotentialUnit = "percent";
        if (CairnUtils::contains(mCarrierType, "Electrical"))  mPotentialUnit = "V";
        if (CairnUtils::contains(mCarrierType, "Thermal"))     mPotentialUnit = "degC";  // and Pressure ?? classes derivees a prevoir entre Thermal et Fluid
    }
    
    if (mPotentialUnit == "" || mFluxUnit == "" || mStorageUnit == "" || mPotentialUnit == "" || mFluxName == "" || mStorageName == "")
    {
        cCritical() << "Void unit/name detected for EnergyVector " << objectName();
        Cairn_Exception erreur("Void unit/name is not allowed - Please check/correct EnergyVector " + objectName(), -1);
        throw erreur;
    }

    if (CairnUtils::contains(mCarrierType, { "Fluid", "Material", "BioMass", "Wood" }))
    {
        mIsMassCarrier = true;
    }

    if (mIsMassCarrier && (CairnUtils::contains(mCarrierType, "Electrical") || CairnUtils::contains(mCarrierType, "Thermal")) )
    {
        cCritical() << "IsMassCarrier attribute error detected for EnergyVector " << (objectName());
        Cairn_Exception erreur("IsMassCarrier is not allowed for Electrical / Thermal energy types - Please check/correct EnergyVector " + objectName(), -1);
        throw erreur;
    }

    if (CairnUtils::contains(mCarrierType, "FluidH2O"))
    {
        mIsHeatCarrier = true;
    }

    if (mIsHeatCarrier && CairnUtils::contains(mCarrierType, "Electrical"))
    {
        cCritical() << "mIsHeatCarrier attribute error detected for EnergyVector " << (objectName());
        Cairn_Exception erreur("mIsHeatCarrier is not allowed for Electrical energy types - Please check/correct EnergyVector " + objectName(), -1);
        throw erreur;
    }
    
    if ((CairnUtils::contains(mCarrierType, "Fluid") && !CairnUtils::contains(mCarrierType, "FluidH2O") && !CairnUtils::contains(mCarrierType, "FluidCO2")) 
        || CairnUtils::contains(mCarrierType, "Wood") || CairnUtils::contains(mCarrierType, "BioMass"))
    {
        mIsFuelCarrier = true;
    }

    if (mIsFuelCarrier && CairnUtils::contains(mCarrierType, "Electrical"))
    {
        cCritical() << "IsFuelCarrier attribute error detected for EnergyVector " << (objectName());
        Cairn_Exception erreur("IsFuelCarrier is not allowed for Electrical energy types - Please check/correct EnergyVector " + objectName(), -1);
        throw erreur;
    }
    
    if (mIsFuelCarrier) {
        if (mLHV <=0.)  
        {
            double vConversion = UnitsConverter::Convert(1.0, mMassUnit, "kg") / UnitsConverter::Convert(1.0, mEnergyUnit, "MWh");
            mLHV = CarrierTypes::getCarrierProp(mCarrierType, "LHV"); // MWh/kg           
        }
        cInfo() << " Low Heat Value = " << mLHV;
    }
    else if (CairnUtils::contains(mCarrierType, "Thermal")) mLHV = 1.;
    else if (CairnUtils::contains(mCarrierType, "Electrical"))  mLHV = 1.;

    if (mLHV <= 0.)
    {
        cCritical() << "Negative or Zero LHV value detected for energy vector " << (objectName()) << mLHV;
        Cairn_Exception erreur("Negative LHV value is not allowed for energy carrier - Use 1. for pure energy carriers - Please check/correct EnergyVector " + objectName(), -1);
        throw erreur;
    }

    if (isHeatCarrier())
    {        
        if (mCP <= 0.)
        {
            mCP = CarrierTypes::getCarrierProp(mCarrierType, "CP"); // J/kg/K at 300K
        }
        cInfo() << objectName() << ".CP=" << mCP;
    }

    if (isMassCarrier())
    {
        if (mRHO <= 0.)
        {
            mRHO = CarrierTypes::getCarrierProp(mCarrierType, "RHO"); // MWh/kg
        }
    }

    return true;
}


void EnergyVector::jsonSaveGuiComponent(ojson &componentsArray)
{
    ojson compoObject;
    if (mEnergyColour == "") {
        mEnergyColour = getDefaultEnergyVectorColor();
    }    
    mGUIData->jsonSaveGUILine(compoObject);
    compoObject["energyTypeColor"] = mEnergyColour;
    compoObject["paramListJson"] = ojson::array();
    compoObject["optionListJson"] = ojson::array();
    compoObject["timeSeriesListJson"] = ojson::array();
   
    ojson& paramArray = compoObject["paramListJson"];
    mConfigParam->jsonSaveGUIInputParam(paramArray);
    mCompoInputSettings->jsonSaveGUIInputParam(paramArray);

    mCompoInputParam->jsonSaveGUIInputParam(compoObject["optionListJson"]);

    ojson& timeSeriesArray = compoObject["timeSeriesListJson"];
    mTimeSeriesParam->jsonSaveGUIInputParam(timeSeriesArray);
    mGridTimeSeries->jsonSaveGUIInputParam(timeSeriesArray);

    componentsArray.push_back(compoObject);
}

std::string EnergyVector::getDefaultEnergyVectorType() {
    std::string defaultType = "EnergyVector";
    if (CairnUtils::contains(mCarrierType, "THERM", true) || CairnUtils::contains(mCarrierType, "HEAT", true))
    {
        defaultType = "Heat";
    }
    else if (CairnUtils::contains(mCarrierType, "H2", true))
    {
        defaultType = "H2Vector";
    }
    else if (CairnUtils::contains(mCarrierType, "CH4", true))
    {
        defaultType = "CH4Vector";
    }
    else if (CairnUtils::contains(mCarrierType, "ELEC", true))
    {
        defaultType = "Electricity";
    }
    else if (CairnUtils::contains(mCarrierType, { "BIOMASS", "MATERIAL", "WOOD" }, true))
    {
        defaultType = "Biomass";
    }
    else {
        defaultType = "EnergyVector";
    }
    return defaultType;
}

std::string EnergyVector::getDefaultEnergyVectorColor()
{
    std::string defaultColor = "";
    if (CairnUtils::contains(mCarrierType, "THERM", true) || CairnUtils::contains(mCarrierType, "HEAT", true))
    {
        if (CairnUtils::contains(objectName(), "HOT", true)) defaultColor = "red";
        if (CairnUtils::contains(objectName(), "CHAUD", true)) defaultColor = "red";
        if (CairnUtils::contains(objectName(), "HEAT", true)) defaultColor = "red";
        if (CairnUtils::contains(objectName(), "COLD", true)) defaultColor = "blue";
        if (CairnUtils::contains(objectName(), "FROID", true)) defaultColor = "blue";
    }
    else if (CairnUtils::contains(mCarrierType, "H2", true))
    {
        defaultColor = "green";
    }
    else if (CairnUtils::contains(mCarrierType, "CH4", true))
    {
        defaultColor = "brown";
    }
    else if (CairnUtils::contains(mCarrierType, "CO2", true))
    {
        defaultColor = "grey";
    }
    else if (CairnUtils::contains(mCarrierType, "H2O", true))
    {
        defaultColor = "blue";
    }
    else if (CairnUtils::contains(mCarrierType, "ELEC", true))
    {
        defaultColor = "yellow";
    }
    else if (CairnUtils::contains(mCarrierType, "BIOMASS", true))
    {
        defaultColor = "darkgreen";
    }
    else if (CairnUtils::contains(mCarrierType, "WOOD", true))
    {
        defaultColor = "darkgreen";
    }
    else if (CairnUtils::contains(mCarrierType, "MATERIAL", true))
    {
        defaultColor = "maroon";
    }
    else if (CairnUtils::contains(mCarrierType, "FLUID", true))
    {
        defaultColor = "darkblue";
    }
    else {
        defaultColor = "black";
    }
    return defaultColor;
}

std::vector<InputParam*> EnergyVector::get_InputParams()
{
    std::vector<InputParam*> res = {  
        getConfigParam(), 
        getCompoInputParam(),
        getCompoInputSettings(),
        getTimeSeriesParam(),
        getGridTimeSeries()
    };
    // GUI data (optional)
    if (auto* gui = getGUIData()) {
        res.push_back(gui->getGuiInputParam());
    }
    return res;
}

std::vector<InputParam*> EnergyVector::get_ParamInputParams()
{
    std::vector<InputParam*> result;
    result.push_back(getConfigParam());
    result.push_back(getCompoInputSettings());
    return result;
}

std::vector<InputParam*> EnergyVector::get_OptionInputParams()
{
    std::vector<InputParam*> result;
    result.push_back(getCompoInputParam());
    return result;
}

std::vector<InputParam*> EnergyVector::get_TimeSeriesInputParams()
{
    std::vector<InputParam*> result;
    result.push_back(getTimeSeriesParam());
    result.push_back(getGridTimeSeries());
    return result;
}