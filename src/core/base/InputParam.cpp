#include "GlobalSettings.h"
#include "InputParam.h"
#include "CairnUtils.h"
#include "SubModel.h"

#include <unordered_set>

//#include "base/OptimProblem.h"
using namespace GS ;

InputParam::InputParam (CairnObject *aParent, std::string aName): CairnObject(aParent)
{
    this->setObjectName(aName);
}

InputParam::~InputParam()
{
    removeParameters();
    removeIndicators();
}

void InputParam::removeIndicator(const std::string& indicatorName)
{
    auto it = std::remove_if(mIndicators.begin(), mIndicators.end(),
        [&](ModelIndicator* indicator) {
        if (indicator && indicator->getName() == indicatorName)
        {
            delete indicator;  // Free memory
            return true;      // Remove this element
        }
        return false;
    });

    mIndicators.erase(it, mIndicators.end());
}

void InputParam::removeIndicators()
{
    // Delete all indicators
    for (auto* indicator : mIndicators) {
        delete indicator;  
    }
    // Clear vector
    mIndicators.clear(); // Removes all elements
}

void InputParam::removeImpactIndicators(const std::string& impactName)
{
    for (auto it = mIndicators.begin(); it != mIndicators.end();)
    {
        ModelIndicator* indicator = *it;
        if (CairnUtils::contains(indicator->getName(), impactName)) {
            delete indicator;            // Free memory if you own it
            it = mIndicators.erase(it);  // Erase and move to next valid iterator
        }
        else {
            ++it;
        }
    }
}

//Model InputParameter Interface
void InputParam::removeParameter(const std::string& aParamName)
{
    auto it = mMapParams.find(aParamName);
    if (it != mMapParams.end()) {
        delete it->second;     // Free memory
        mMapParams.erase(it); // Erase safely
    }
}

void InputParam::removeParameters()
{
    // Delete all params
    for (auto& [name, param] : mMapParams) {
        delete param;  
    }
    // Clear map
    mMapParams.clear();
}

void InputParam::removeImpactParameters(const std::string& impactName)
{
    for (auto it = mMapParams.begin(); it != mMapParams.end();)
    {
        if (CairnUtils::contains(it->first, impactName))  {
            delete it->second;          // Free memory
            it = mMapParams.erase(it);  // Erase and move to next valid iterator
        }
        else {
            ++it;
        }
    }
}

void InputParam::removePortImpactParameters(const std::string& portName)
{
    for (auto it = mMapParams.begin(); it != mMapParams.end();)
    {
        if (CairnUtils::contains(it->first, portName)) {
            delete it->second;          // Free memory
            it = mMapParams.erase(it);  // Erase and move to next valid iterator
        }
        else {
            ++it;
        }
    }
}

void InputParam::addParameter(const std::string& aParamName, const t_pvalue &aPtr, t_value aDefaultValue, t_flag aIsBlocking, t_flag aIsUsed, 
    const std::string& aDescription, const t_unit& aUnit, const std::string aShowConfig)
{
    /* 
    * used to add a parameter. A parameter is either a scalar (bool, int, double, string) 
    * or a string vector (which is concatenated into a string at the end) 
    */
    addToShowConfigList(aShowConfig);
    if (mMapParams.find(aParamName) != mMapParams.end()) {
        delete mMapParams[aParamName];
    }
    mMapParams[aParamName] = new ModelParam(aParamName, aPtr, aDefaultValue, aIsBlocking, aIsUsed, aDescription, aUnit, aShowConfig);
}

void InputParam::addTimeSeries(const std::string& aParamName, std::vector<double>* aDblePtr, double a_default, t_flag aIsBlocking, t_flag aIsUsed,
    const std::string& aDescription, const t_unit& aUnit, const std::string& aShowConfig, double a_min, double a_max)
{
    /*
    * used to add a timeseries (a vector of double). 
    * The user has to specify the name of a timeseries (as a scalar parameter), e.g. "UseProfileLoadFlux" : "MyFluxTimeSeries", 
    * and provide the corresponding vector in an input .csv timeseries file (see TimeSeriesManager::importTS)
    * The names of the columns in the input timeseries file should be the names provided by the user (not aParamName), e.g. "MyFluxTimeSeries".
    */
    addToShowConfigList(aShowConfig);
    if (mMapParams.find(aParamName) != mMapParams.end()) {
        delete mMapParams[aParamName];
    }
    mMapParams[aParamName] = new ModelParam(aParamName, aDblePtr, a_default, a_min, a_max, aIsBlocking, aIsUsed, aDescription, aUnit, aShowConfig);
}

void InputParam::addPerfParam(const std::string& aParamName, std::vector<double>* aDblePtr, t_flag aIsBlocking, t_flag aIsUsed, 
    const std::string& aDescription, const t_unit& aUnit)
{
    /*
    * used to add a performance parameter (a vector of double). A performance parameters has a fixed hard-coded name.
    * The user has to provide the name of the corresponding input .csv file using parameter "DataFile" (MilpComponent::mDataFile)
    * The names of the columns in the input file should be the hard-coded names (i.e. aParamName).
    */
    if (mMapParams.find(aParamName) != mMapParams.end()) {
        delete mMapParams[aParamName];
    }
    mMapParams[aParamName] = new ModelParam(aParamName, aDblePtr, 1.0, std::nan("1"), std::nan("1"), aIsBlocking, aIsUsed, aDescription, aUnit);
}

void InputParam::publishData(const std::string& aVarName, int aSize, double aDefault)
{
    /* used to publish/export IO variables */
    if (mMapParams.find(aVarName) != mMapParams.end()) {
        delete mMapParams[aVarName];
    }
    mMapParams[aVarName] = new ModelParam(aVarName, aSize, aDefault);
}

void InputParam::addIndicator(const t_Name& aIndicatorName, std::vector<double>* aDblePtr, bool* aIsExported,
    const std::string& aDesc, const t_unit& aUnit, const t_Name& aShortName)
{
    /* add indicator */
    mIndicators.push_back(new ModelIndicator(this->parent(), aIndicatorName, aDblePtr, aIsExported, aDesc, aUnit, aShortName));
}

void InputParam::addToShowConfigList(const std::string& aConfig)
{
    // TODO: build list by iterating over all parameters ?! 
    if (aConfig == "DONOTSHOW") return;

    std::string config = aConfig;
    if (config.empty())
        config = "Base";

    if (find(mShowConfigList.begin(), mShowConfigList.end(), config) == mShowConfigList.end())
    {
        mShowConfigList.push_back(config);
    }
}

int InputParam::checkProfile (const std::string aName, const Eigen::VectorXf &aProfile, const float aInf, const float aSup)
{
    float maxVal = aProfile.maxCoeff() ;
    float minVal = aProfile.minCoeff() ;
    if (maxVal < aInf || maxVal > aSup || minVal < aInf || minVal > aSup )
    {
        cCritical() << "ERROR in profile " << aName
                    << " values should be in the range ["<< aInf << ";" << aSup << "] "
                    << " instead of ["<< minVal << ";" << maxVal << "] " ;
        return -1 ;
    }
    return 0 ;
}

// create and fill vector parameters from reading of parameter file - Caution : for the moment only use double values !
void InputParam::readVectorParameters (const std::string &aName, const std::string &aFileName, std::vector<std::string> &aPerfParamNames)
{
    fs::path vPath(aFileName);    
    if (!fs::exists(vPath)) {
        cError() << "ERROR DataFile " + aFileName + " doesn't exist for component " + aName;
    }
    
    std::vector<std::vector<std::string>> data_Inputs = CairnUtils::readFromCsvFile(aFileName, ";");
    cInfo() << "Reading vector parameters from file " << aFileName;
    // Loop on expected input parameters
    for (auto const& [iParName, val] : mMapParams) {
        if (val) {
            if (val->getType() == eVectorDouble) {
                std::string varName = std::string(aName + "." + iParName);
                for (int i = 0; i < (data_Inputs.at(0)).size(); ++i)
                {
                    std::string colName = std::string((data_Inputs.at(0)).at(i).c_str());
                    std::string simplifiedColName = CairnUtils::replace(colName, " ", "");
                    std::string simplifiedVarName = CairnUtils::replace(varName, " ", "");
                    if (simplifiedColName == simplifiedVarName || CairnUtils::split(simplifiedColName, '(')[0] == simplifiedVarName) // first line should include column description : element.arrayName
                    {
                        std::vector<double> data_vector = CairnUtils::getDataArray(data_Inputs, i, 1);
                        if (CairnUtils::contains(simplifiedColName, "(r=") || CairnUtils::contains(simplifiedColName, "(c=")) {
                            int n = std::stoi(CairnUtils::split(CairnUtils::split(simplifiedColName, '=')[1], ')')[0]);
                            if (CairnUtils::contains(simplifiedColName, "(r=")) {//row
                                if (data_vector.size() % n != 0) {
                                    cError() << "ERROR: the data size in column " << colName << " is not a multiplication of " << n;
                                    return;
                                }
                                else {
                                    int k = 0;
                                    int inc = int(data_vector.size() / n);
                                    std::vector<double> filtered_data;
                                    while (k < data_vector.size()) {
                                        filtered_data.push_back(data_vector[k]);
                                        k += inc;
                                    }
                                    val->setValue(filtered_data);                                    
                                }
                            }
                            else {//column
                                if (data_vector.size() % n != 0) {
                                    cError() << "ERROR: the data size in column " << colName << " is not a multiplication of " << n;
                                    return;
                                }
                                else {
                                    std::vector<double> filtered_data;
                                    for (int k = 0; k < n; k++) {
                                        filtered_data.push_back(data_vector[k]);
                                    }
                                    val->setValue(filtered_data);
                                }
                            }
                        }
                        else {
                            val->setValue(data_vector);
                        }
                        cInfo() << "Found data for performance parameter: " << (aName + "." + iParName);
                        std::vector<std::string>::iterator vIter = find(aPerfParamNames.begin(), aPerfParamNames.end(), iParName);
                        if (vIter != aPerfParamNames.end())
                            aPerfParamNames.erase(vIter);
                        break;
                    }
                }
            }
        }
    }
}

// fill in Vector Data by copying elements of aMapParamVXf map of input Eigen::VectorXf* in map aMapParamVD of SubModel input of std::vector<double> *
int InputParam::fillVectorData(const std::string& aName, const InputParam& aSrc, const uint& aOffset)
{
    const t_mapParams& vSrcParams = aSrc.getMapParams();
    for (auto const& [key, val] : mMapParams) {
        if (val) {
            if (val->getType() == eVectorDouble) {
                bool isBlocking = val->IsBlocking();
                if (val->isPValue()) {
                    t_mapParams::const_iterator vIter = vSrcParams.find(key);
                    bool vFindSrc = false;
                    if (vIter != vSrcParams.end()) {
                        if (vIter->second) {
                            if (vIter->second->getType() == eVectorEigen && vIter->second->isPValue()) {
                                vFindSrc = true;             
                                size_t vDestSize = val->size();                                
                                if (vDestSize == 0 && isBlocking)
                                {
                                    cError() << "fillVectorData: allocation of vector variable is missing for " << (aName + "." + key);
                                    return -1;
                                }
                                else if (vDestSize == 0)
                                {
                                    cWarning() << "fillVectorData: allocation of vector variable is missing for " << (aName + "." + key) << ". Skip!";
                                    continue;
                                }                                
                                try
                                {
                                    val->copyValues(*vIter->second, aOffset);
                                    cDebug() << "Read Vector Data Time Series : " << (aName + "." + key); //<< (*val)[0] ; // << " = " << (*versSubModel).at(0) << (*versSubModel).at((*versSubModel).size() - 1);
                                }
                                catch (const std::exception&e)
                                {
                                    cError() << "ERROR fillVectorData: variable size of " 
                                        << (aName + "." + key) << vDestSize 
                                        << e.what();
                                    return -1;
                                }                                                                
                            }
                        }
                    }
                    if (!vFindSrc) {
                        if (isBlocking)
                        {
                            cError() << "ERROR: nullptr pointer for component variable name " << (aName + "." + key);
                            return -1;
                        }
                        else
                        {
                            if (GS::iVerbose > 0) 
                                cWarning() << "Optionnal parameter not found in component: " << (aName + "." + key);
                            continue;
                        }
                    }
                }  
                else {
                    if (isBlocking)
                    {
                        cError() << "ERROR fillVectorData: nullptr pointer for component variable name " << (aName + "." + key);
                        return -1;
                    }
                    else
                    {
                        if (GS::iVerbose > 0) 
                            cWarning() << "fillVectorData: nullptr pointer for component variable name " << (aName + "." + key) 
                            << ". Will then NOT be initialized!";
                    }
                }
            }
        }        
    }
    return 0;
}

void InputParam::jsonSaveGUIInputParam(ojson& paramArray)
{    
    for (auto const& [key, param] : mMapParams) {
        if (param) {
            ojson paramObject;
            paramObject["key"] = key;
            switch (param->getType()) {
                case eDouble:
                    paramObject["value"] = *std::get<eDouble>(param->getPtr());
                    break;
                case eInt:
                    paramObject["value"] = *std::get<eInt>(param->getPtr());
                    break;
                case eBool:
                    if (*std::get<eBool>(param->getPtr()))
                        paramObject["value"] = true;
                    else
                        paramObject["value"] = false;
                    break;
                case eString:
                    paramObject["value"] = *std::get<eString>(param->getPtr());
                    break;
                case eStringList: {
                    paramObject["value"] = ojson::array();
                    ojson& vList = paramObject["value"];
                    std::vector<std::string> &values = *(std::vector<std::string>*)(std::get<eStringList>(param->getPtr()));
                    for (auto& value : values) {
                        vList.push_back(value);
                    }                    
                    break;
                }
                default:
                    paramObject["value"] = param->toString();
                    break;
            }
            const std::string comment = param->getComment();
            if (!comment.empty()) {
                paramObject["comment"] = comment;
            }
            paramArray.push_back(paramObject);
        }
    }
}

void InputParam::getParameters(std::vector<std::string>& a_List, const EParamType& a_Type)
{
    for (auto const& [key, val] : mMapParams) {
        if (val) {
            if (val->getType() == a_Type)
                a_List.push_back(key);
        }
    }
}

void InputParam::getParameters(std::vector<ModelParam*>& a_List, const EParamType& a_Type)
{
    for (auto const& [key, val] : mMapParams) {
        if (val) {
            if (val->getType() == a_Type)
                a_List.push_back(val);
        }
    }
}

ModelParam* InputParam::getParameter(const std::string& aName)
{
    ModelParam* vRet = nullptr;
    t_mapParams::iterator vIter = mMapParams.find(aName);
    if (vIter != mMapParams.end()) {
        return vIter->second;        
    }
    return vRet;
}

void InputParam::getParameters(std::vector<std::string> &a_List, CairnAPI::ESettingsLimited a_setLimited)
{  
    if (a_setLimited == CairnAPI::all) {
        for (auto const& [key, val] : mMapParams) {
            if (val) {
                a_List.push_back(key);
            }
        }        
    }
    else {
        for (auto const& [key, val] : mMapParams) {
            if (val) {
                if ((val->IsBlocking() && a_setLimited == CairnAPI::mandatory)
                    || (!val->IsBlocking() && a_setLimited == CairnAPI::optional)
                    || (val->IsUsed() && a_setLimited == CairnAPI::used))
                    a_List.push_back(key);
            }
        }        
    }
}

bool InputParam::getParameterValue(const std::string& a_SettingsName, std::string &a_Value, const EParamType& a_Type)
{        
    t_mapParams::iterator vIter = mMapParams.find(a_SettingsName);
    if (vIter != mMapParams.end()) {
        if (vIter->second) {
            if (a_Type == eUndefined) {
                // no type
                a_Value = vIter->second->toString();
                return true;
            }
            else {
                if (vIter->second->getType() == a_Type) {
                    a_Value = vIter->second->toString();
                    return true;
                }
            }            
        }
    }
    return false;    
}

bool InputParam::getParameterValue(const std::string& a_SettingsName, t_value &a_Value)
{    
    t_mapParams::iterator vIter = mMapParams.find(a_SettingsName);
    if (vIter != mMapParams.end()) {
        bool b = vIter->second->IsUsed();
        if (vIter->second) {
            a_Value = vIter->second->getValue();
            return true;
        }
    }    
    return false;
}

bool InputParam::setParameterValue(const std::string& a_SettingsName, const std::string& a_SettingsValue)
{    
    t_mapParams::iterator vIter = mMapParams.find(a_SettingsName);
    if (vIter != mMapParams.end()) {
        if (vIter->second) {
            return vIter->second->setValue(a_SettingsValue);            
        }
    }
    return false;
}

bool InputParam::setParameterValue(const std::string& a_SettingsName, const t_value& a_SettingsValue)
{
    t_mapParams::iterator vIter = mMapParams.find(a_SettingsName);
    if (vIter != mMapParams.end()) {
        if (vIter->second) {
            return vIter->second->setValue(a_SettingsValue);
        }
    }
    return false;
}

int InputParam::readParameters(const t_mapParamData& aSettings)
{
    if (mMapParams.empty())
        return 0;

    for (auto const& [key, val] : mMapParams) {
        if (val) {
            if (!val->readParameter(aSettings))
                return -1;
        }
        else
            return -1;
    }   
    return 0;
}


/*****************************************************************************************************/

InputParam::ModelIndicator::ModelIndicator(CairnObject* aParent, 
    const t_Name& aIndicatorName, std::vector<double>* aDblePtr, bool* aIsExported,
    const std::string& aDesc, const t_unit& aUnit, const t_Name& aShortName)
{
    pModel = dynamic_cast<SubModel*> (aParent);
    m_Name.set_Value(aIndicatorName);
    m_ShortName.set_Value(aShortName); 
    m_Unit.set_Value(aUnit);
    m_Comment = aDesc;

    p_Value = aDblePtr;
    p_IsExported = aIsExported;
}

bool InputParam::ModelIndicator::IsExported() const
{
    bool vRet = true;
    if (p_IsExported)
        vRet = *p_IsExported;
    return vRet;
}

double InputParam::ModelIndicator::getValue(size_t aIndex) const
{
    double vRet = std::nan("1");
    if (p_Value) {
        if (aIndex >= 0 && aIndex < p_Value->size()) {
            vRet = (*p_Value)[aIndex];
        }
    }
    return vRet;
}

void InputParam::ModelIndicator::resetValue()
{
    if (p_Value->size() >= 2)
    {
        (*p_Value)[0] = 0.;
        (*p_Value)[1] = 0.;
    }
}

std::string InputParam::ModelIndicator::getName() const 
{
    bool isSizeOptimized = pModel ? pModel->isSizeOptimized() : false;
    return CairnUtils::parseIndicatorName(m_Name.get_Value(), isSizeOptimized);
}

std::string InputParam::ModelIndicator::getShortName() const
{
    return CairnUtils::remove_spaces(m_ShortName.get_Value());
}

std::string InputParam::ModelIndicator::getUnit() const
{
    return m_Unit.get_Value();
}

void InputParam::ModelIndicator::Export(std::fstream& out, const std::string &aComponentName, const std::string& range, 
    bool aForceExport, bool showDescription, const std::vector<std::string>& labels) const
{
    if (p_Value && (IsExported() || aForceExport))
    {
        //Published (declared) indicators        
        if (range == "PLAN") {     
            if(showDescription)
                CairnUtils::outputIndicator(out, aComponentName, getName(), (*p_Value)[0], getUnit(), getShortName(), m_Comment, labels);
            else 
                CairnUtils::outputIndicator(out, aComponentName, getName(), (*p_Value)[0], getUnit(), getShortName(), "N/A", labels);
        }
        else if (range == "HIST") {  
            if (showDescription)
                CairnUtils::outputIndicator(out, aComponentName, getName(), (*p_Value)[1], getUnit(), getShortName(), m_Comment, labels);
            else 
                CairnUtils::outputIndicator(out, aComponentName, getName(), (*p_Value)[1], getUnit(), getShortName(), "N/A", labels);
        }        
    }
}

void InputParam::ModelIndicator::Export(std::fstream& out, const std::string& aComponentName, const std::string& range,
    bool aForceExport, bool aIsSizeOptimized, bool aIsPriceOptimized, bool isRollingHorizon, const std::vector<double>& aOptimalSizeAllCycles, 
    const bool showDescription, const std::vector<std::string>& labels) const
{
    if (p_Value && (IsExported() || aForceExport))    
    {
        bool isOptimalSize = false;
        const std::string indicatorName = getName();

        //Manage optim/fixed sizing  
        static const std::unordered_set<std::string> optimizedNames = {
            "Installed Size",     "Installed Optimal Size",
            "Component Weight",   "Component Optimal Weight",
            "Storage Capacity",   "Storage Optimal Capacity"
        };

        auto it = optimizedNames.find(indicatorName);
        if (it != optimizedNames.end()) {
            isOptimalSize = true;
        }

        if (aIsPriceOptimized && indicatorName == "Component Optimal Price") {
            isOptimalSize = true;
        }

        //0 -> _PLAN, 1 -> _HIST
        if (range == "PLAN") {
            //Assuming that when isRollingHorizon==false, the export is to the main _PLAN.csv (it is re-written every cycle), 
            //and when isRollingHorizon==true, the export is to _PLAN_RH_cycle.csv (a file per cycle)
            if (!isRollingHorizon && isOptimalSize) {
                //Take the maximum values from all cycles
                auto it_maxValue = std::max_element(aOptimalSizeAllCycles.begin(), aOptimalSizeAllCycles.end());
                if (it_maxValue != aOptimalSizeAllCycles.end()) {
                    if(showDescription)
                        CairnUtils::outputIndicator(out, aComponentName, indicatorName, *it_maxValue, getUnit(), getShortName(), m_Comment, labels);
                    else
                        CairnUtils::outputIndicator(out, aComponentName, indicatorName, *it_maxValue, getUnit(), getShortName(), "N/A", labels);
                }
                isOptimalSize = false;
            }
            else {
                if (showDescription)
                    CairnUtils::outputIndicator(out, aComponentName, indicatorName, (*p_Value)[0], getUnit(), getShortName(), m_Comment, labels);
                else
                    CairnUtils::outputIndicator(out, aComponentName, indicatorName, (*p_Value)[0], getUnit(), getShortName(), "N/A", labels);

            }
        }
        else if (range == "HIST") {
            if (showDescription)
                CairnUtils::outputIndicator(out, aComponentName, indicatorName, (*p_Value)[1], getUnit(), getShortName(), m_Comment, labels);
            else 
                CairnUtils::outputIndicator(out, aComponentName, indicatorName, (*p_Value)[1], getUnit(), getShortName(), "N/A", labels);
        }
    }
}