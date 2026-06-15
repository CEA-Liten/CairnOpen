#include "GlobalSettings.h"
#include "CairnUtils.h"
#include "ModelParam.h"


ModelParam::ModelParam(const std::string& a_Name, t_flag a_IsBlocking, t_flag a_IsUsed, const std::string& a_Description,
    const t_unit& a_Unit, const std::string& a_ShowConfig)
{
    m_Type = EParamType::eUndefined;
    m_Name = a_Name;
    m_Description = a_Description;
    m_Unit.set_Value(a_Unit);
    m_IsBlocking.set_Value(a_IsBlocking);
    m_IsUsed.set_Value(a_IsUsed);
    m_default = std::nan("1");
    m_min = std::nan("1");
    m_max = std::nan("1");
    m_ShowConfig = a_ShowConfig;
    if (m_ShowConfig.empty())
        m_ShowConfig = "Base";

    if (IsBlocking() && !IsUsed()) {
        cWarning() << "Parameter " + a_Name + " is mandatory but marked as not used! It would be good to review the flags of this parameter!";
    }
}

// t_pvalue (scalar and vector of string parameters)
ModelParam::ModelParam(const std::string& a_Name, const t_pvalue& ap_Value, t_value a_defaultValue, t_flag a_IsBlocking, t_flag a_IsUsed,
    const std::string& a_Description, const t_unit& a_Unit, const std::string& a_ShowConfig)
    : ModelParam(a_Name, a_IsBlocking, a_IsUsed, a_Description, a_Unit, a_ShowConfig)
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
ModelParam::ModelParam(const std::string& a_Name, std::vector<double>* ap_Value, double a_default, double a_min, double a_max,
    t_flag a_IsBlocking, t_flag a_IsUsed, const std::string& a_Description, const t_unit& a_Unit, const std::string& a_ShowConfig)
    : ModelParam(a_Name, a_IsBlocking, a_IsUsed, a_Description, a_Unit, a_ShowConfig)
{
    m_Type = EParamType::eVectorDouble;
    p_Value = ap_Value;

    // in case of a performance parameter, the values always set to 1.0, std::nan("1"), std::nan("1")
    m_default = a_default;
    m_min = a_min;
    m_max = a_max;
}

/* special ModelParam constructor used to publish IO variables */
ModelParam::ModelParam(const std::string& a_Name, int aSize, double aDefault)
    : ModelParam(a_Name)
{
    m_Type = EParamType::eVectorEigen;
    p_Value = new Eigen::VectorXf(aSize);
    Eigen::VectorXf& vect = (Eigen::VectorXf&)(*std::get<eVectorEigen>(p_Value));
    for (size_t i = 0; i < aSize; i++) vect(i) = aDefault;
    m_create = true;
}


ModelParam::~ModelParam()
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

bool ModelParam::readParameter(const t_mapParamData& aSettings)
{
    if (isPValue()) {
        if (aSettings.find(m_Name) != aSettings.end()) {
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

bool ModelParam::IsBlocking()
{
    return m_IsBlocking.get_Value();
};

bool ModelParam::IsUsed()
{
    return m_IsUsed.get_Value();
};

bool ModelParam::isDependent()
{
    return !(m_IsBlocking.is_Scalar());
}

TriState ModelParam::isModified() {
    if (m_Type == eVectorDouble) {
        return Undefined;
    }
    else if (m_default == getValue()) {
        return False;
    }
    else {
        return True;
    }
}

bool ModelParam::isPValue() const
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

std::string ModelParam::getUnit() const
{
    return m_Unit.get_Value();
}

size_t  ModelParam::size()
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

t_value ModelParam::getValue() const
{
    switch (m_Type) {
    case eDouble:
        return *std::get<eDouble>(p_Value);

    case eInt:
        return *std::get<eInt>(p_Value);

    case eBool:
        return static_cast<int>(*std::get<eBool>(p_Value));  // Cast to int

    case eVectorDouble:
        return *std::get<eVectorDouble>(p_Value);

    case eString:
        return *std::get<eString>(p_Value);

    case eStringList:
        return *std::get<eStringList>(p_Value);

    default:
        return m_default;   
    }
}

bool ModelParam::getNumValue(double& a_Value)
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

// TODO: create a util method that converts a t_value to std::string !!
std::string ModelParam::toString()
{
    // TODO: use std::optional<std::string> ?!
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
        vRet = CairnUtils::joinStrings(*(std::vector<std::string>*)(std::get<eStringList>(p_Value)));
        break;
    }
    default:
        break;
    }
    return vRet;
}

std::optional<std::string> ModelParam::getStrDefaultValue() const
{
    return std::visit([](const auto& val) -> std::optional<std::string> {
        using T = std::decay_t<decltype(val)>;

        if constexpr (std::is_same_v<T, double>) {
            return std::to_string(val);
        }
        else if constexpr (std::is_same_v<T, int>) {
            return std::to_string(val);
        }
        else if constexpr (std::is_same_v<T, std::string>) {
            return val;
        }
        else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            // Concatenate strings with comma separator
            return CairnUtils::joinStrings(val, ",");
        }
        else {
            // vector<double> and vector<int>
            cWarning() << "Cannot convert numeric vector default value to string";
            return std::nullopt;
        }
        }, m_default);
}

void ModelParam::readParam(const std::string& aParamName, const t_mapParamData& a_Settings)
{
    m_Comment = CairnUtils::getParamComment(a_Settings, aParamName);

    const std::string raw = CairnUtils::getParamValue(a_Settings, aParamName);
    switch (m_Type)
    {
    case eDouble:
    {
        double* pValue = std::get<eDouble>(p_Value);
        try {
            *pValue = std::stod(raw);
        }
        catch (...) {
            *pValue = 0.0;
        }
        break;
    }

    case eInt:
    {
        int* pValue = std::get<eInt>(p_Value);
        try {
            *pValue = std::stoi(raw);
        }
        catch (...) {
            *pValue = 0;
        }
        break;
    }

    case eBool:
    {
        bool* pValue = std::get<eBool>(p_Value);

        const std::string s = CairnUtils::simplified(raw);
        if (s == "0" || s == "false" || s == "False" || s == "FALSE")
            *pValue = false;
        else if (s == "1" || s == "true" || s == "True" || s == "TRUE")
            *pValue = true;
        else
            *pValue = false; // default

        break;
    }

    case eString:
    {
        std::string* pValue = std::get<eString>(p_Value);
        *pValue = raw;   // raw is already a normalized string
        break;
    }

    case eStringList:
    {
        std::vector<std::string>* pValue = std::get<eStringList>(p_Value);
        *pValue = CairnUtils::toStringVector(raw);
        break;
    }

    default:
        break;
    }
}

bool ModelParam::setValue(const std::string& a_Value)
{
    bool vRet = false;

    switch (m_Type) {
    case eDouble:
    {
        if (double** ppValue = std::get_if<double*>(&p_Value)) {
            double* pValue = *ppValue;
            if (!pValue) {
                cWarning() << "No storage for parameter " << m_Name;
                break;
            }
            try {
                *pValue = std::stod(a_Value);
                vRet = true;
            }
            catch (const std::exception&) {
                cWarning() << "Conversion fails for " << m_Name << " : Value is not Double";
            }
        }
        else {
            cWarning() << "Parameter " << m_Name << " does not hold a double* in p_Value";
        }
        break;
    }

    case eInt:
    {
        if (int** ppValue = std::get_if<int*>(&p_Value)) {
            int* pValue = *ppValue;
            if (!pValue) {
                cWarning() << "No storage for parameter " << m_Name;
                break;
            }
            try {
                const std::string lower = CairnUtils::toLower(a_Value);
                if (lower == "false") {
                    *pValue = 0;
                }
                else if (lower == "true") {
                    *pValue = 1;
                }
                else {
                    *pValue = std::stoi(a_Value);
                }
                vRet = true;
            }
            catch (const std::exception&) {
                cWarning() << "Conversion fails for " << m_Name << " : Value is not Integer";
            }
        }
        else {
            cWarning() << "Parameter " << m_Name << " does not hold an int* in p_Value";
        }
        break;
    }

    case eBool:
    {
        if (bool** ppValue = std::get_if<bool*>(&p_Value)) {
            bool* pValue = *ppValue;
            if (!pValue) {
                cWarning() << "No storage for parameter " << m_Name;
                break;
            }
            std::string lower = CairnUtils::toLower(a_Value);
            *pValue = (a_Value == "0" || lower == "false") ? false : true;
            vRet = true;
        }
        else {
            cWarning() << "Parameter " << m_Name << " does not hold a bool* in p_Value";
        }
        break;
    }

    case eString:
    {
        if (std::string** ppValue = std::get_if<std::string*>(&p_Value)) {
            std::string* pValue = *ppValue;
            if (!pValue) {
                cWarning() << "No storage for parameter " << m_Name;
                break;
            }
            *pValue = a_Value;
            vRet = true;
        }
        else {
            cWarning() << "Parameter " << m_Name << " does not hold a std::string* in p_Value";
        }
        break;
    }

    case eStringList:
    {
        if (std::vector<std::string>** ppValue = std::get_if<std::vector<std::string>*>(&p_Value)) {
            std::vector<std::string>* pValue = *ppValue;
            if (!pValue) {
                cWarning() << "No storage for parameter " << m_Name;
                break;
            }
            // parse comma-separated strings into vector<string>
            *pValue = CairnUtils::toStringVector(a_Value);
            vRet = true;
        }
        else {
            cWarning() << "Parameter " << m_Name << " does not hold a std::vector<std::string>* in p_Value";
        }
        break;
    }

    case eVectorDouble:
    {
        if (std::vector<double>** ppValue = std::get_if<std::vector<double>*>(&p_Value)) {
            std::vector<double>* pValue = *ppValue;
            if (!pValue) {
                cWarning() << "No storage for parameter " << m_Name;
                break;
            }
            try {
                // parse comma-separated numbers into vector<double>
                std::vector<double> tmp;
                std::string s = a_Value;
                std::istringstream iss(s);
                std::string token;
                while (std::getline(iss, token, ',')) {
                    // trim whitespace
                    auto start = token.find_first_not_of(" \t\n\r");
                    auto end = token.find_last_not_of(" \t\n\r");
                    if (start == std::string::npos) continue;
                    std::string piece = token.substr(start, end - start + 1);
                    tmp.push_back(std::stod(piece));
                }
                *pValue = std::move(tmp);
                vRet = true;
            }
            catch (const std::exception&) {
                cWarning() << "Conversion fails for " << m_Name << " : Value is not VectorDouble";
            }
        }
        else {
            cWarning() << "Parameter " << m_Name << " does not hold a std::vector<double>* in p_Value";
        }
        break;
    }

    default:
        break;
    }

    return vRet;
}

bool ModelParam::setValue(const t_value& a_Value)
{
    bool vRet = false;
    switch (m_Type) {
    case eDouble:
    {
        if (double** ppValue = std::get_if<double*>(&p_Value)) {
            double* pValue = *ppValue;
            if (!pValue) {
                cWarning() << "No storage for parameter " << m_Name;
                break;
            }

            if (const double* pSrc = std::get_if<double>(&a_Value)) {
                *pValue = *pSrc;
                vRet = true;
            }
            else if (const int* pSrc = std::get_if<int>(&a_Value)) {
                *pValue = static_cast<double>(*pSrc);
                vRet = true;
            }
            else {
                cWarning() << "Conversion fails for " << m_Name << " : Value is not Double/Int";
            }
        }
        else {
            cWarning() << "Parameter " << m_Name << " does not hold a double* in p_Value";
        }
        break;
    }

    case eInt:
    {
        if (int** ppValue = std::get_if<int*>(&p_Value)) {
            int* pValue = *ppValue;
            if (!pValue) {
                cWarning() << "No storage for parameter " << m_Name;
                break;
            }

            if (const int* pSrc = std::get_if<int>(&a_Value)) {
                *pValue = *pSrc;
                vRet = true;
            }
            else if (const double* pSrc = std::get_if<double>(&a_Value)) {
                *pValue = static_cast<int>(*pSrc);
                vRet = true;
            }
            else {
                cWarning() << "Conversion fails for " << m_Name << " : Value is not Int/Double";
            }
        }
        else {
            cWarning() << "Parameter " << m_Name << " does not hold an int* in p_Value";
        }
        break;
    }

    case eBool:
    {
        if (bool** ppValue = std::get_if<bool*>(&p_Value)) {
            bool* pValue = *ppValue;
            if (!pValue) {
                cWarning() << "No storage for parameter " << m_Name;
                break;
            }

            if (const double* pSrc = std::get_if<double>(&a_Value)) {
                *pValue = (*pSrc != 0.0);
                vRet = true;
            }
            else if (const int* pSrc = std::get_if<int>(&a_Value)) {
                *pValue = (*pSrc != 0);
                vRet = true;
            }
            else if (const std::string* pSrc = std::get_if<std::string>(&a_Value)) {
                std::string s = CairnUtils::toLower(*pSrc);
                *pValue = (s == "0" || s == "false") ? false : true;
                vRet = true;
            }
            else {
                cWarning() << "Conversion fails for " << m_Name << " : Value is not Bool-compatible";
            }
        }
        else {
            cWarning() << "Parameter " << m_Name << " does not hold a bool* in p_Value";
        }
        break;
    }

    case eString:
    {
        if (std::string** ppValue = std::get_if<std::string*>(&p_Value)) {
            std::string* pValue = *ppValue;
            if (!pValue) {
                cWarning() << "No storage for parameter " << m_Name;
                break;
            }

            if (const std::string* pSrc = std::get_if<std::string>(&a_Value)) {
                *pValue = *pSrc;
                vRet = true;
            }
            else {
                cWarning() << "Conversion fails for " << m_Name << " : Value is not String";
            }
        }
        else {
            cWarning() << "Parameter " << m_Name << " does not hold a std::string* in p_Value";
        }
        break;
    }

    case eStringList:
    {
        if (std::vector<std::string>** ppValue = std::get_if<std::vector<std::string>*>(&p_Value)) {
            std::vector<std::string>* pValue = *ppValue;
            if (!pValue) {
                cWarning() << "No storage for parameter " << m_Name;
                break;
            }

            if (const std::vector<std::string>* pSrc = std::get_if<std::vector<std::string>>(&a_Value)) {
                *pValue = *pSrc;
                vRet = true;
            }
            else if (const std::string* pSrc = std::get_if<std::string>(&a_Value)) {
                *pValue = CairnUtils::toStringVector(*pSrc);
                vRet = true;
            }
            else {
                cWarning() << "Conversion fails for " << m_Name << " : Value is not a StringList";
            }
        }
        else {
            cWarning() << "Parameter " << m_Name << " does not hold a std::vector<std::string>* in p_Value";
        }
        break;
    }

    case eVectorDouble:
    {
        if (std::vector<double>** ppValue = std::get_if<std::vector<double>*>(&p_Value)) {
            std::vector<double>* pValue = *ppValue;
            if (!pValue) {
                cWarning() << "No storage for parameter " << m_Name;
                break;
            }

            if (const std::vector<double>* pSrc = std::get_if<std::vector<double>>(&a_Value)) {
                *pValue = *pSrc;
                vRet = true;
            }
            else {
                cWarning() << "Conversion fails for " << m_Name << " : Value is not VectorDouble";
            }
        }
        else {
            cWarning() << "Parameter " << m_Name << " does not hold a std::vector<double>* in p_Value";
        }
        break;
    }

    default:
        cWarning() << "Unsupported parameter type for " << m_Name;
        break;
    }

    return vRet;
}

bool ModelParam::copyValues(const ModelParam& aSrc, size_t aOffset)
{
    if (m_Type != eVectorDouble || aSrc.getType() != eVectorEigen) {
        return false;
    }

    std::vector<double>* pValue = (std::vector<double>*)std::get<eVectorDouble>(p_Value);
    Eigen::VectorXf* pSrc = (Eigen::VectorXf*)std::get<eVectorEigen>(aSrc.getPtr());

    bool vRet = false;
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
    
    return vRet;
}

bool ModelParam::copyValues(const std::vector<double>& aSrc, size_t aOffset)
{
    if (m_Type != eVectorDouble) {
        return false;
    }

    std::vector<double>* pValue = (std::vector<double>*)std::get<eVectorDouble>(p_Value);

    bool vRet = false;
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
    
    return vRet;
}

bool ModelParam::setValues(const double& aValue, size_t aSize)
{
    bool vRet = false;
    if (m_Type == eVectorDouble) {
        std::vector<double>* pValue = (std::vector<double>*)std::get<eVectorDouble>(p_Value);
        pValue->assign(aSize, aValue);
        vRet = true;
    }
    return vRet;
}

t_value ModelParam::operator[](size_t i)
{
    t_value vRet = m_default;
    if (i >= 0 && i < size()) {
        switch (m_Type) {
        case eVectorDouble:
            vRet = (std::vector<double>)(*std::get<eVectorDouble>(p_Value))[i];
            break;
        case eVectorEigen:
        {
            Eigen::VectorXf& vect = (Eigen::VectorXf&)(*std::get<eVectorEigen>(p_Value));
            vRet = vect[i];
            break;
        }
        }
    }
    return vRet;
}
