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
    if (aConfig == "DONOTSHOW") return;
    if (find(mShowConfigList.begin(), mShowConfigList.end(), aConfig) == mShowConfigList.end())
    {
        mShowConfigList.push_back(aConfig);
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
        Cairn_Exception error("ERROR DataFile " + aFileName + " doesn't exist for component " + aName, -1);
        throw& error;
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
                                    cCritical() << "ERROR: the data size in column " << colName << " is not a multiplication of " << n;
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
                                    cCritical() << "ERROR: the data size in column " << colName << " is not a multiplication of " << n;
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
                                    cCritical() << "ERROR fillVectorData: in SubModel, allocation of vector variable is missing for " << (aName + "." + key);
                                    return -1;
                                }
                                else if (vDestSize == 0)
                                {
                                    cWarning() << "Warning fillVectorData: in SubModel, allocation of vector variable is missing for " << (aName + "." + key);
                                    continue;
                                }                                
                                try
                                {
                                    val->copyValues(*vIter->second, vDestSize, aOffset);
                                    cInfo() << "- Read Vector Data Time Series : " << (aName + "." + key); //<< (*val)[0] ; // << " = " << (*versSubModel).at(0) << (*versSubModel).at((*versSubModel).size() - 1);
                                }
                                catch (const std::exception&e)
                                {
                                    cCritical() << "ERROR fillVectorData: in SubModel, variable size of " << (aName + "." + key) << vDestSize;
                                    cCritical() << e.what();
                                    return -1;
                                }                                                                
                            }
                        }
                    }
                    if (!vFindSrc) {
                        if (isBlocking)
                        {
                            cCritical() << "ERROR: nullptr pointer for component variable name " << (aName + "." + key);
                            return -1;
                        }
                        else
                        {
                            if (GS::iVerbose > 0) cWarning() << "Optionnal parameter not found in component - Hope will not be used by submodel ! " << (aName + "." + key);
                            continue;
                        }
                    }
                }  
                else {
                    if (isBlocking)
                    {
                        cCritical() << "ERROR fillVectorData: nullptr pointer for component variable name " << (aName + "." + key);
                        return -1;
                    }
                    else
                    {
                        if (GS::iVerbose > 0) cWarning() << "WARNING fillVectorData: nullptr pointer for component variable name " << (aName + "." + key);
                        if (GS::iVerbose > 0) cWarning() << "SubModel vector variable will then NOT be initialized " << (aName + "." + key);
                    }
                }
            }
        }        
    }
    return 0;
}

void InputParam::jsonSaveGUIInputParam(ojson& paramArray)
{    
    for (auto const& [key, val] : mMapParams) {
        if (val) {
            ojson paramObject;
            paramObject["key"] = key;
            switch (val->getType()) {
                case eDouble:
                    paramObject["value"] = *std::get<eDouble>(val->getPtr());
                    break;
                case eInt:
                    paramObject["value"] = *std::get<eInt>(val->getPtr());
                    break;
                case eBool:
                    if (*std::get<eBool>(val->getPtr()))
                        paramObject["value"] = true;
                    else
                        paramObject["value"] = false;
                    break;
                case eString:
                    paramObject["value"] = *std::get<eString>(val->getPtr());
                    break;
                case eStringList: {
                    paramObject["value"] = ojson::array();
                    ojson& vList = paramObject["value"];
                    std::vector<std::string> &values = *(std::vector<std::string>*)(std::get<eStringList>(val->getPtr()));
                    for (auto& value : values) {
                        vList.push_back(value);
                    }                    
                    break;
                }
                default:
                    paramObject["value"] = val->toString();
                    break;
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

InputParam::ModelParam* InputParam::getParameter(const std::string& aName)
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

int InputParam::readParameters(const std::map<std::string, std::string>& aSettings)
{
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

InputParam::ModelParam::ModelParam(const std::string& a_Name, t_flag a_IsBlocking, t_flag a_IsUsed, const std::string& a_Comment, 
    const t_unit& a_Unit, const std::string& a_ShowConfig)
{
    m_Type = EParamType::eUndefined;
    m_Name = a_Name;
    m_Comment = a_Comment;
    m_Unit.set_Value(a_Unit);
    m_IsBlocking.set_Value(a_IsBlocking);
    m_IsUsed.set_Value(a_IsUsed);        
    m_default = std::nan("1");
    m_min = std::nan("1");
    m_max = std::nan("1");
    m_ShowConfig = a_ShowConfig;

    if (IsBlocking() && !IsUsed()) {
        cWarning() << "Parameter " + a_Name + " is mandatory but marked as not used! It would be good to review the flags of this parameter!";
    }
}

// t_pvalue (scalar and vector of string parameters)
InputParam::ModelParam::ModelParam(const std::string& a_Name, const t_pvalue &ap_Value, t_value a_defaultValue, t_flag a_IsBlocking, t_flag a_IsUsed, 
    const std::string& a_Comment, const t_unit& a_Unit, const std::string& a_ShowConfig)
    : ModelParam(a_Name, a_IsBlocking, a_IsUsed, a_Comment, a_Unit, a_ShowConfig)
{
    p_Value = ap_Value;
    m_default = a_defaultValue;
    if (std::get_if<bool*>(&ap_Value)) {
        m_Type = EParamType::eBool;        
    }
    else if (std::get_if<double*>(&ap_Value)) {
        m_Type = EParamType::eDouble;        
    }
    else if (std::get_if<int*>(&ap_Value)) {
        m_Type = EParamType::eInt;
    }
    else if (std::get_if<std::string*>(&ap_Value)) {
        m_Type = EParamType::eString;              
    }
    else if (std::get_if<std::vector<std::string>*>(&ap_Value)) {
        m_Type = EParamType::eStringList;
    }
    else if (std::get_if<std::vector<double>*>(&ap_Value)) {
        m_Type = EParamType::eVectorDouble;
    }
    else {
        // erreur?
        cCritical() << "Bad type for the parameter " << m_Name;
    }
    setValue(m_default);  
}

// vector double (used for timeseries and performance parameters)
InputParam::ModelParam::ModelParam(const std::string& a_Name, std::vector<double>* ap_Value, double a_default, double a_min, double a_max, 
    t_flag a_IsBlocking, t_flag a_IsUsed, const std::string& a_Comment, const t_unit& a_Unit, const std::string& a_ShowConfig)
    : ModelParam(a_Name, a_IsBlocking, a_IsUsed, a_Comment, a_Unit, a_ShowConfig)
{
    m_Type = EParamType::eVectorDouble;
    p_Value = ap_Value;

    // in case of a performance parameter, the values always set to 1.0, std::nan("1"), std::nan("1")
    m_default = a_default;
    m_min = a_min;
    m_max = a_max;
}

/* special ModelParam constructor used to publish IO variables */
InputParam::ModelParam::ModelParam(const std::string& a_Name, int aSize, double aDefault)
    : ModelParam(a_Name)
{
    m_Type = EParamType::eVectorEigen;
    p_Value = new Eigen::VectorXf(aSize);
    Eigen::VectorXf& vect = (Eigen::VectorXf&)(*std::get<eVectorEigen>(p_Value));
    for (size_t i = 0; i < aSize; i++) vect(i) = aDefault;
    m_create = true;
}


InputParam::ModelParam::~ModelParam()
{
    if (m_create) {
        if (m_Type = EParamType::eVectorEigen) {
            Eigen::VectorXf* pValue = (Eigen::VectorXf*)(std::get<eVectorEigen>(p_Value));
            delete pValue;
            pValue = nullptr;
        }
    }        
}


/*****************************************************************************************************/

bool InputParam::ModelParam::readParameter(const std::map<std::string, std::string>& aSettings) 
{   
    if (isPValue()) {
        if (aSettings.find(m_Name)!=aSettings.end()) {
            readParam(m_Name, aSettings);            
        }
        else {
            if (IsBlocking()) {
                cCritical() << "ERROR readParameters: missing value for parameter " << (m_Name);
                return false;
            }          
        }
    }
    else {
        cCritical() << "ERROR readParameters: nullptr pointer for component variable name " << (m_Name);
        return false;
    }
    return true;
}

bool InputParam::ModelParam::IsBlocking() 
{ 
    return m_IsBlocking.get_Value(); 
};

bool InputParam::ModelParam::IsUsed() 
{ 
    return m_IsUsed.get_Value(); 
};

bool InputParam::ModelParam::isDependent()
{
    return !(m_IsBlocking.is_Scalar());
}

TriState InputParam::ModelParam::isModified() {
    if (m_Type == eVectorDouble){
        return Undefined;
    }
    else if (m_default == getValue()) {
        return False;
    }
    else {
        return True;
    }
}

bool InputParam::ModelParam::isPValue() const
{
    bool vRet = false;
    switch (m_Type) {
    case eDouble:
        vRet = (std::get<eDouble>(p_Value) != nullptr);
        break;
    case eInt:
        vRet = (std::get<eInt>(p_Value) != nullptr);
        break;
    case eBool:
        vRet = (std::get<eBool>(p_Value) != nullptr);
        break;
    case eString:
        vRet = (std::get<eString>(p_Value) != nullptr);
        break;
    case eStringList:
        vRet = (std::get<eStringList>(p_Value) != nullptr);
        break;
    case eVectorDouble:
        vRet = (std::get<eVectorDouble>(p_Value) != nullptr);
        break;
    case eVectorEigen:
        vRet = (std::get<eVectorEigen>(p_Value) != nullptr);
        break;
    default:
        vRet = false;
        break;
    }
    return vRet;
}

std::string InputParam::ModelParam::getUnit() const
{
    return m_Unit.get_Value();
}

size_t  InputParam::ModelParam::size()
{
    size_t vRet = 0;    
    switch (m_Type) {
        case eVectorDouble:
        {
            std::vector<double>* pValue = (std::vector<double>*)(std::get<eVectorDouble>(p_Value));
            if (pValue)
                vRet = pValue->size();
            break;
        }
        case eVectorEigen:
        {
            Eigen::VectorXf* pValue = (Eigen::VectorXf*)(std::get<eVectorEigen>(p_Value));
            if (pValue)
                vRet = pValue->size();
            break;
        }
    }
    return vRet;
}

t_value InputParam::ModelParam::getValue()
{
    t_value vRet = m_default;
    switch (m_Type) {
    case eDouble:
        vRet = (double)(*std::get<eDouble>(p_Value));
        break;
    case eInt:
        vRet = (int)(*std::get<eInt>(p_Value));
        break;
    case eBool:
        vRet = (int)(*std::get<eBool>(p_Value)); //cast as int (t_value doesn't support bool)
        break;
    case eVectorDouble:
        vRet = (std::vector<double>)(*std::get<eVectorDouble>(p_Value));
        break;   
    case eString: 
        {
            std::string* pValue = (std::string*)(std::get<eString>(p_Value));
            vRet = *pValue;
            break;
        } 
    case eStringList:
        {
            std::vector<std::string>* pValue = (std::vector<std::string>*)(std::get<eStringList>(p_Value));
            std::vector<std::string> pValue_list;
            for (auto const& val : *pValue) {
                pValue_list.push_back(val);
            }
            vRet = pValue_list;
            break;
        }
    }
    return vRet;   
}

bool InputParam::ModelParam::getNumValue(double& a_Value)
{
    bool vRet = false;
    switch (m_Type) {
    case eDouble:
        a_Value = (double)(*std::get<eDouble>(p_Value));
        vRet = true;
        break;
    case eInt:
        a_Value = (double)(*std::get<eInt>(p_Value));
        vRet = true;
        break;
    case eBool:
        a_Value = (double)(*std::get<eBool>(p_Value));
        vRet = true;
        break;    
    }
    return vRet;
}

std::string InputParam::ModelParam::toString()
{
    std::string vRet = "";
    switch (m_Type) {
    case eDouble:
        vRet = std::to_string(*std::get<eDouble>(p_Value));
        break;
    case eInt:
        vRet = std::to_string(*std::get<eInt>(p_Value));
        break;
    case eBool:
        if (*std::get<eBool>(p_Value))
            vRet = "true";
        else
            vRet = "false";
        break;
    case eString:
    {
        std::string* pValue = (std::string*)(std::get<eString>(p_Value));
        vRet = *pValue;
        break;
    }
    case eStringList: {
        vRet = CairnUtils::join (*(std::vector<std::string>*)(std::get<eStringList>(p_Value)));
        break;
    }
    default:
        break;
    }
    return vRet;
}

void InputParam::ModelParam::readParam(const std::string& aParamName, const std::map<std::string, std::string>& a_Settings)
{    
    switch (m_Type) {
    case eDouble:
    {
        double* pValue = (double*)std::get<eDouble>(p_Value);
        try
        {
            *pValue = std::stod(a_Settings.at(aParamName));
        }
        catch (const std::exception&)
        {
            *pValue = 0.0;
        }        
        break;
    }              
    case eInt:
    {
        int* pValue = (int*)std::get<eInt>(p_Value);
        try
        {
            *pValue = std::stoi(a_Settings.at(aParamName));
        }
        catch (const std::exception&)
        {
            *pValue = 0;
        }        
        break;
    }
    case eBool:
    {
        bool* pValue = (bool*)std::get<eBool>(p_Value);
        try
        {
            *pValue = (a_Settings.at(aParamName) == "0") ? false : true;
        }
        catch (const std::exception&)
        {
            *pValue = false;
        }        
        break;
    }
    case eString: 
        {
        std::string* pValue = std::get<eString>(p_Value);
        try
        {
            *pValue = a_Settings.at(aParamName);
        }
        catch (const std::exception&)
        {
            *pValue = "";
        }                       
            break;
        }
    case eStringList: {
        std::vector<std::string>* pValue = std::get<eStringList>(p_Value);
        *pValue = std::vector<std::string>({});
        bool isImpactSelected = false; //at least one impact is selected
        if (CairnUtils::simplified(a_Settings.at(aParamName)) != "")
        {
            std::vector<std::string> stringList = CairnUtils::split(a_Settings.at(aParamName), ',');
            for (size_t i = 0; i < stringList.size(); i++) {
                if (CairnUtils::simplified(stringList[i]) != "") {
                    stringList[i] = CairnUtils::simplified(stringList[i]);
                    isImpactSelected = true;
                }
            }
            if (isImpactSelected) {
                *pValue = stringList;
            }
        }
        break;
    }
    default:
        break;
    }
    
}

bool InputParam::ModelParam::setValue(const std::string& a_Value)
{
    bool vRet = false;
    switch (m_Type) {
    case eDouble:
    {
        double* pValue = (double*)std::get<eDouble>(p_Value);
        try
        {
            *pValue = std::stod(a_Value);
            vRet = true;
        }
        catch (const std::exception&)
        {
            cWarning() << "Conversion fails for the" << m_Name << " Value is not Double";
        }                
        break;        
    }
    case eInt:
    {
        int* pValue = (int*)std::get<eInt>(p_Value);
        try
        {
            *pValue = std::stoi(a_Value);
            vRet = true;
        }
        catch (const std::exception&)
        {
            cWarning() << "Conversion fails for the" << m_Name << " Value is not Integer";
        }        
        break;
    }
    case eBool:
    {
        bool* pValue = (bool*)std::get<eBool>(p_Value);        
        *pValue = (a_Value == "0") ? false : true;
        vRet = true;
        break;
    }
    case eString:
    {
        std::string* pValue = std::get<eString>(p_Value);
        *pValue = a_Value;
        vRet = true;
        break;
    }
    case eStringList:
    {   //assumes that a_Value is a string with list elements sperated with comma
        auto* pValue = std::get<eStringList>(p_Value);
        const std::string value = CairnUtils::simplified(a_Value);
        if (!value.empty()) {
            pValue->clear(); 
            for (const auto& s : CairnUtils::split(value)) {
                pValue->push_back(CairnUtils::trim(s));
            }
        }
        else {
            pValue->clear();
        }
        vRet = true;
        break;
    }
    default:
        break;
    }
  
    return vRet;
}

bool InputParam::ModelParam::setValue(const t_value& a_Value)
{
    bool vRet = false;
    switch (m_Type) {
    case eDouble:
    {
        double* pValue = (double*)std::get<eDouble>(p_Value);
        if (const double* pSrc = std::get_if<double>(&a_Value)) {
            *pValue = *pSrc;
            // in the bound ? clamp ? 
            vRet = true;
        }
        else if (const int* pSrc = std::get_if<int>(&a_Value)) {
            *pValue = (double)*pSrc;
            // in the bound ? clamp ? 
            vRet = true;
        }
        else {
            cWarning() << "Conversion fails for the" << m_Name << " Value is not Double";
        }        
        break;
    }
    case eInt:
    {
        int* pValue = (int*)std::get<eInt>(p_Value);
        if (const int* pSrc = std::get_if<int>(&a_Value)) {
            *pValue = *pSrc;
            // in the bound ? clamp ? 
            vRet = true;
        }
        else {
            cWarning() << "Conversion fails for the" << m_Name << " Value is not Double";
        }
        break;
    }
    case eBool:
    {
        bool* pValue = (bool*)std::get<eBool>(p_Value);
        if (const double* pSrc = std::get_if<double>(&a_Value)) {
            *pValue = (*pSrc!=0);            
            vRet = true;
        }
        else if (const int* pSrc = std::get_if<int>(&a_Value)) {
            *pValue = (*pSrc != 0);
            vRet = true;
        } 
        else if (const std::string* pSrc = std::get_if<std::string>(&a_Value)) {
            *pValue = (*pSrc == "0") ? false : true;
            vRet = true;
        }
        else
        {
            // TODO: conversion ?
            cWarning() << "Conversion fails for the" << m_Name << " Value is not Bool";
        }   
        break;
    }
    case eString:
    {
        std::string* pValue = std::get<eString>(p_Value);
        if (const std::string* pSrc = std::get_if<std::string>(&a_Value)) {
            *pValue = std::string(pSrc->c_str());
            vRet = true;
        }
        else {
            // TODO: conversion ?
            cWarning() << "Conversion fails for the" << m_Name << " Value is not Bool";
        }        
        break;
    }
    case eVectorDouble:
    {
        std::vector<double>* pValue = (std::vector<double>*)std::get<eVectorDouble>(p_Value);
        if (const std::vector<double>* pSrc = std::get_if<std::vector<double>>(&a_Value)) {
            *pValue = *pSrc;
            vRet = true;
        }
        else {
            // TODO: conversion ?
            cWarning() << "Conversion fails for the" << m_Name << " Value is not VectorDouble";
        }
        break;
    }        
    default:
        break;
    }

    return vRet;
}

bool InputParam::ModelParam::copyValues(const ModelParam& aSrc, size_t aSize, size_t aOffset)
{   
    bool vRet = false;
    if (m_Type == eVectorDouble && aSrc.getType() == eVectorEigen) {
        std::vector<double> *pValue = (std::vector<double>*)std::get<eVectorDouble>(p_Value);
        Eigen::VectorXf *pSrc = (Eigen::VectorXf*)std::get<eVectorEigen>(aSrc.getPtr());
        if (pValue->size() + aOffset != (long unsigned)pSrc->size()) {            
            int vSize = pSrc->size() - aOffset;
            throw std::range_error("it should be equal to Component, variable futursize " + std::to_string(vSize));
        }
        else {                        
            for (uint i = 0; i < pValue->size(); i++) {
                (*pValue)[i] = double((*pSrc)[i + aOffset]);
            }         
            vRet = true;
        }
    }    
    return vRet;
}

bool InputParam::ModelParam::copyValues(const std::vector<double>& aSrc, size_t aOffset)
{
    bool vRet = false;
    if (m_Type == eVectorDouble) {
        std::vector<double>* pValue = (std::vector<double>*)std::get<eVectorDouble>(p_Value);        
        if (pValue->size() + aOffset != (long unsigned)aSrc.size()) {
            int vSize = aSrc.size() - aOffset;
            throw std::range_error("it should be equal to Component, variable futursize " + std::to_string(vSize));
        }
        else {
            for (uint i = 0; i < pValue->size(); i++) {
                (*pValue)[i] = aSrc[i + aOffset];
            }
            vRet = true;
        }
    }
    return vRet;
}

bool InputParam::ModelParam::setValues(const double& aValue, size_t aSize)
{
    bool vRet = false;
    if (m_Type == eVectorDouble) {
        std::vector<double>* pValue = (std::vector<double>*)std::get<eVectorDouble>(p_Value);
        pValue->assign(aSize, aValue);
        vRet = true;        
    }
    return vRet;
}

t_value InputParam::ModelParam::operator[](size_t i)
{
    t_value vRet = m_default;
    if (i >= 0 && i < size()) {
        switch (m_Type) {
            case eVectorDouble:
                vRet = (std::vector<double>)(*std::get<eVectorDouble>(p_Value))[i];
                break;
            case eVectorEigen:
            {
                Eigen::VectorXf &vect = (Eigen::VectorXf&)(*std::get<eVectorEigen>(p_Value));
                vRet = vect[i];
                break;
            }
        }
    }
    return vRet;
}

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

bool InputParam::ModelIndicator::IsExported()
{
    bool vRet = true;
    if (p_IsExported)
        vRet = *p_IsExported;
    return vRet;
}

double InputParam::ModelIndicator::getValue(size_t aIndex)
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
    bool aForceExport, const bool showDescription, const std::vector<std::string>& labels)
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
    const bool showDescription, const std::vector<std::string>& labels)
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