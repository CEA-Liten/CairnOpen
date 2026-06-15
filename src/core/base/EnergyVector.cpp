#include "EnergyVector.h"
#include "MilpComponent.h"
#include "CarrierTypes.h"
#include "OrUnitsConverter.h"
#include "CairnAPIUtils.h"

EnergyVector::EnergyVector(CairnObject* aParent, const std::string& aName, const std::string& aType, 
    const std::string& aTechnoType, const t_mapParamData aComponent)
    : CairnObject(aParent, aName),
    mCarrierType(aType),
    mCarrierTechnoType(aTechnoType), 
    mComponent(aComponent),
    mEnergyColour("")
{
    setObjectType("EnergyVector");
    configTechnoType();

    mConfigParam = new InputParam(this, "ConfigParam" + Name());
    mCompoOptions = new InputParam(this, "CompoInputParam" + Name());
    mCompoParams = new InputParam(this, "CompoInputSettings" + Name());
    mTimeSeriesParam = new InputParam(this, "TimeSeriesSettings" + Name());    
    mGridTimeSeries = new InputParam(this, "GridTimeSeries" + Name());
}

EnergyVector::~EnergyVector()
{
    delete mGUIData;
    delete mConfigParam;
    delete mCompoOptions;
    delete mCompoParams;
    delete mTimeSeriesParam;
    delete mGridTimeSeries;
}

void EnergyVector::configTechnoType()
{
    const std::vector<std::string> supportedTechnoTypes = CarrierTypes::getCarrierTypes();
    auto it = std::find(supportedTechnoTypes.begin(), supportedTechnoTypes.end(), mCarrierTechnoType);
    if (it == supportedTechnoTypes.end() || mCarrierTechnoType.empty()) {
        if (mCarrierType == "ElectricalCarrier") /** componentPERSEEType */
            mCarrierTechnoType = "Electricity";  /** for the GUI : matches both nodeType and nodeTechnoType */
        else if (mCarrierType == "MaterialCarrier")
            mCarrierTechnoType = "Material";
        else
            cWarning() << "The TechnoType (" + mCarrierTechnoType + ") of EnergyVector " + Name() + " is not supported!";
    }
}

const std::string* EnergyVector::pQuantity(const std::string& a_Quantity)  const {
    if (mQuantities.find(a_Quantity) != mQuantities.end())
        return mQuantities.at(a_Quantity);
    else
        return nullptr;
}

void EnergyVector::declareConfigurationParameters()
{    
    for (auto& [key, param] : mParamTS) {
        param.addConfig(mConfigParam, key);
    }    

    setCustomConfigParams();
}

int EnergyVector::setConfigurationParameters(const t_mapParamData& aComponent)
{
    return mConfigParam->readParameters(aComponent); 
}

void EnergyVector::declareCompoInputParam()
{  
    //------------------------------ parameters + timeseries Names -----------------------
    for (auto& [key, param] : mParamTS) {
        param.addParameter(mCompoParams, mTimeSeriesParam, key);
    }
   
    mParamGridTS["BuyPrice"] = ParamCarrier("BuyPrice per mass or energy units", SFunctionUnit({ eFTypeDivision, { &mCurrency, &mStorageUnit } })) ;
    mParamGridTS["BuyPriceSeasonal"] = ParamCarrier("BuyPriceSeasonal per mass or energy units", SFunctionUnit({ eFTypeDivision, { &mCurrency, &mStorageUnit } }));
    mParamGridTS["SellPrice"] = ParamCarrier("SellPrice per mass or energy units", SFunctionUnit({ eFTypeDivision, { &mCurrency, &mStorageUnit } }));
    for (auto& [key, param] : mParamGridTS) {
        param.addParameter(mCompoParams, mGridTimeSeries, key);
    }     

    mQuantities["PowerUnit"]   = &mPowerUnit;
    mQuantities["EnergyUnit"]  = &mEnergyUnit;
    mQuantities["StorageUnit"] = &mStorageUnit;
    mQuantities["FluxUnit"]    = &mFluxUnit; 

    setCustomParams();
    initEnergyVector();
    initGuiData();
}

int EnergyVector::setCompoInputParam(const t_mapParamData& aComponent)
{
    if (mCompoOptions->readParameters(aComponent)      < 0
       || mCompoParams->readParameters(aComponent) < 0
       || mTimeSeriesParam->readParameters(aComponent)    < 0
       || mGridTimeSeries->readParameters(aComponent)     < 0)
    {
        return -1;
    }

    initEnergyVector();
    initGuiData(aComponent);

    return 0;
}

void EnergyVector::setCustomConfigParams() {
    const std::map<std::string, double> propMap =
        CarrierTypes::getCarrierProperties(mCarrierTechnoType);

    if (propMap.empty())
        return;

    t_dict settingDict;
    for (const auto& [key, value] : propMap) {
        settingDict.emplace(key, t_value{ value });
    }

    CairnAPIUtils::setParameters({ mConfigParam }, settingDict);
}

void EnergyVector::setCustomParams() 
{
    const std::map<std::string, double> propMap =
        CarrierTypes::getCarrierProperties(mCarrierTechnoType);

    if (propMap.empty())
        return;

    cDebug() << "CustomParams: " + Name() + " " + mCarrierTechnoType;

    t_dict settingDict;
    for (const auto& [key, value] : propMap) {
        settingDict.emplace(key, t_value{ value });
    }

    std::vector<InputParam*> inputParams = {
        mCompoOptions,
        mCompoParams,
        mTimeSeriesParam,
        mGridTimeSeries
    };

    CairnAPIUtils::setParameters(inputParams, settingDict);
}

int EnergyVector::initProblem()
{   
    if (setConfigurationParameters(mComponent) < 0)
        return -1;

    if (setCompoInputParam(mComponent) < 0)
        return -1;

    return 0;
}

bool EnergyVector::updateCompoParamMap(const std::string& a_SettingName, 
    const std::string& a_AttributeName, const std::string& a_AttributeValue) 
{
    if (a_AttributeName == "value") {
        CairnUtils::setParamValue(mComponent, a_SettingName, a_AttributeValue);
        if (a_SettingName == "FluxType")  /** To be relaxed when needed */
        { 
            initEnergyVector();
        }
        if (a_SettingName == "Xpos" || a_SettingName == "Ypos") { /** To be relaxed when needed */
            initGuiData(mComponent);
        }
    }
    else if (a_AttributeName == "comment") {
        CairnUtils::setParamComment(mComponent, a_SettingName, a_AttributeValue);
    }
    return true;
}

void EnergyVector::initGuiData(const t_mapParamData& paramMap)
{
    if (!mGUIData) {
        mGUIData = new GUIData(this);
    }

    t_mapParamData extractedParams = CairnUtils::extractGuiParams(paramMap);
    mGUIData->doInit(mCarrierTechnoType, mCarrierTechnoType, mCarrierType, extractedParams);
}

void EnergyVector::jsonSaveGuiComponent(ojson &componentsArray)
{
    ojson compoObject;
    if (mEnergyColour.empty()) {
        mEnergyColour = getDefaultColor();
    }    
    mGUIData->jsonSaveGUILine(compoObject);
    compoObject["energyTypeColor"] = mEnergyColour;
    compoObject["paramListJson"] = ojson::array();
    compoObject["optionListJson"] = ojson::array();
    compoObject["timeSeriesListJson"] = ojson::array();
   
    ojson& paramArray = compoObject["paramListJson"];
    mConfigParam->jsonSaveGUIInputParam(paramArray);
    mCompoParams->jsonSaveGUIInputParam(paramArray);

    mCompoOptions->jsonSaveGUIInputParam(compoObject["optionListJson"]);

    ojson& timeSeriesArray = compoObject["timeSeriesListJson"];
    mTimeSeriesParam->jsonSaveGUIInputParam(timeSeriesArray);
    mGridTimeSeries->jsonSaveGUIInputParam(timeSeriesArray);

    componentsArray.push_back(compoObject);
}

std::vector<InputParam*> EnergyVector::get_InputParams()
{
    std::vector<InputParam*> res = {  
        mConfigParam, 
        mCompoOptions,
        mCompoParams,
        mTimeSeriesParam,
        mGridTimeSeries
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
    result.push_back(mConfigParam);
    result.push_back(mCompoParams);
    return result;
}

std::vector<InputParam*> EnergyVector::get_OptionInputParams()
{
    std::vector<InputParam*> result;
    result.push_back(mCompoOptions);
    return result;
}

std::vector<InputParam*> EnergyVector::get_TimeSeriesInputParams()
{
    std::vector<InputParam*> result;
    result.push_back(mTimeSeriesParam);
    result.push_back(mGridTimeSeries);
    return result;
}

//------------------------------------------------------------
EnergyVector::ParamCarrier::ParamCarrier(const std::string& aDescription,
    const t_unit& aUnit, double aDefault, bool aIsUsed, const std::string& aShowConfig)
{    
    mDescription = aDescription;
    mUnit = aUnit;
    mDefault = aDefault;
    mIsUsed = aIsUsed;
    mShowConfig = aShowConfig;

    mUseConfig = false;
}

void EnergyVector::ParamCarrier::addConfig(InputParam* aConfigParam, const std::string& aName)
{
    if (aConfigParam) {
        /** isMandatory is false because UseProfile are missing in old studies */
        aConfigParam->addParameter("UseProfile" + aName, &UseProfile, false, false, mIsUsed, "If true use timeseries, otherwise use scalar value.", "bool", mShowConfig);
        mUseConfig = true;
    }
}

void EnergyVector::ParamCarrier::addParameter(InputParam* aInputParam, InputParam* aTimeSeriesParam, const std::string& aName)
{
    if (aInputParam && aTimeSeriesParam) {
        if (mUseConfig) {
            // mParamTS;
            
            //TODO : aInputParam->addParameter(aName, &Value, mDefault, SFunctionFlag({ eFTypeNotAnd, {&UseProfile}, {&mIsUsed} }), SFunctionFlag({ eFTypeNotAnd, {&UseProfile}, {&mIsUsed} }), mDescription, mUnit);
           
            aInputParam->addParameter(aName, &Value, mDefault, false, SFunctionFlag({ eFTypeNotAnd, {&UseProfile}, {&mIsUsed} }), mDescription, mUnit, mShowConfig);
            aTimeSeriesParam->addParameter("Profile" + aName, &Profile, "", SFunctionFlag({ eFTypeNotAnd, {}, {&UseProfile, &mIsUsed} }), SFunctionFlag({ eFTypeNotAnd, {}, {&UseProfile, &mIsUsed} }), mDescription + " time profile", mUnit, mShowConfig);
        }
        else {
            // mParamGridTS
            aInputParam->addParameter(aName, &Value, mDefault, false, true, mDescription, mUnit);
            aTimeSeriesParam->addParameter("UseProfile" + aName, &Profile, "", false, true, mDescription + " time profile", mUnit);
        }        
    }
}

//------------------------------------------------------------
bool EnergyVector::isParamExist(const std::string& aName) const
{
    t_mapParamCarrier::const_iterator vIter = mParamTS.find(aName);
    return (vIter != mParamTS.end());
}

bool EnergyVector::useProfileParam(const std::string& aName) const
{
    t_mapParamCarrier::const_iterator vIter = mParamTS.find(aName);
    if (vIter != mParamTS.end()) {
        return vIter->second.UseProfile;
    }
    else
        cError() << "Parameter " + aName + " does not exist in carrier " + Name();
    return false;
}

const double EnergyVector::getParamCstValue(const std::string& aName) const
{
    t_mapParamCarrier::const_iterator vIter = mParamTS.find(aName);
    if (vIter != mParamTS.end()) {
        return (vIter->second.Value);
    }
    else
        cError() << "Parameter " + aName + " does not exist in carrier " + Name();
    return std::numeric_limits<double>::quiet_NaN();
}

const double EnergyVector::getParamValue(const std::string& aName, uint64_t t, 
    const MilpComponent* apComponent) const
{
    auto it = mParamTS.find(aName);
    if (it == mParamTS.end()) {
        cError() << "Parameter \"" << aName
            << "\" does not exist in carrier " << Name();
        return std::numeric_limits<double>::quiet_NaN();
    }

    const ParamCarrier& param = it->second;

    // Constant parameter (no profile)
    if (!param.UseProfile)
        return param.Value;

    // Profile parameter but no component
    if (!apComponent) {
        cError() << "No component provided for time-series parameter \""
            << aName << "\" in carrier " << Name();
        return std::numeric_limits<double>::quiet_NaN();
    }

    // TODO: let EnergyVector reads its timeseries instead of using MilpComponent

    // Retrieve time series
    const std::string tsName = tsProfileID("Profile" + aName);
    const std::vector<double>* ts = apComponent->getTimeSeries(tsName);
    if (!ts) {
        cError() << "Time-series \"" << tsName
            << "\" does not exist in carrier " << Name();
        return std::numeric_limits<double>::quiet_NaN();
    }

    // Bounds check 
    if (t >= ts->size()) {
        cError() << "Time index " << t << " out of range for time-series \""
            << tsName << "\" in carrier " << Name()
            << " (size = " << ts->size() << ")";
        return std::numeric_limits<double>::quiet_NaN();
    }

    return (*ts)[t];
}

const double EnergyVector::getMinParamValue(const std::string& aName, const MilpComponent* apComponent) const
{
    t_mapParamCarrier::const_iterator vIter = mParamTS.find(aName);
    if (vIter != mParamTS.end()) {
        if (vIter->second.UseProfile) {
            if (apComponent) {
                std::string vTSname = tsProfileID("Profile" + vIter->first);
                const std::vector<double>* vTS = apComponent->getTimeSeries(vTSname);
                if (vTS) 
                    return *std::min_element(vTS->begin(), vTS->end());
                else
                    cError() << "Time series " + vTSname + " does not exist in carrier " + Name();
            }
            else
                cError() << "No component for the time series parameter " + aName + " in carrier " + Name();
        }
        else
            return (vIter->second.Value);
    }
    else
        cError() << "Parameter " + aName + " does not exist in carrier " + Name();
    return std::numeric_limits<double>::quiet_NaN();
}
