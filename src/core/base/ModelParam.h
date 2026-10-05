#ifndef ModelParam_H
#define ModelParam_H
#include "CairnCore_global.h"
#include "CairnAPI.h"
#include "FlagParam.h"
#include "UnitParam.h"
#include <optional>

/* Param Raw Data: value, comment */
struct ParamData {
    std::string value;
    std::string comment;
};

typedef std::map<std::string, ParamData> t_mapParamData;

enum TriState {
    False = 0,
    True = 1,
    Undefined = 2
};

enum EParamType {
    eUndefined = -1,
    eDouble = 0,
    eInt,
    eBool,
    eString,
    eStringList,
    eVectorDouble,
    //eVectorInt,
    eVectorEigen
};
typedef std::variant<double*, int*, bool*, std::string*, std::vector<std::string>*, std::vector<double>*, Eigen::VectorXf*> t_pvalue;

class ModelParam
{
public:
    // default contractor  
    ModelParam(const std::string& a_Name = "",
        t_flag a_IsBlocking = false,
        t_flag a_IsUsed = true,
        const std::string& a_Description = "",
        const t_unit& aUnit = "",
        const std::string& a_ShowConfig = "");

    // t_pvalue (scalar and vector of string parameters) 
    ModelParam(const std::string& a_Name,
        const t_pvalue& ap_Value,
        t_value a_defaultValue,
        t_flag a_IsBlocking = false,
        t_flag a_IsUsed = true,
        const std::string& a_Description = "",
        const t_unit& aUnit = "",
        const std::string& a_ShowConfig = "");

    // vector double (used for timeseries and performance parameters)
    ModelParam(const std::string& a_Name,
        std::vector<double>* ap_Value,
        double a_default = 1.0,
        double a_min = std::nan("1"),
        double a_max = std::nan("1"),
        t_flag a_IsBlocking = false,
        t_flag a_IsUsed = true,
        const std::string& a_Description = "",
        const t_unit& aUnit = "",
        const std::string& a_ShowConfig = "");

    // publish IO var name
    ModelParam(const std::string& a_Name, int aSize, double aDefault);

    ~ModelParam();

    virtual std::string toString();
    virtual bool setValue(const std::string& a_Value);
    virtual bool setValue(const t_value& a_Value);
    virtual t_value getValue() const;
    bool getNumValue(double& a_Value) const; // return if possible a double value
    bool copyValues(const ModelParam& aSrc, size_t aOffset = 0);
    bool copyValues(const std::vector<double>& aSrc, size_t aOffset = 0);
    bool setValues(const double& aValue, size_t aSize);

    bool readParameter(const t_mapParamData& aSettings);
    bool IsBlocking();
    bool IsUsed();
    const FlagParam* pIsUsed() const { return &m_IsUsed; }; /* dynamically pass isUsede.g. from a timeseries ModelParam to the corresponding ModelTS */
    bool isDependent(); /* whether m_IsBlocking is a scalr or depends on other parameetrs */
    TriState isModified();

    const std::string& getName() const { return m_Name; };
    const std::string& getDescription() const { return m_Description; };
    const std::string& getShowConfig() const { return m_ShowConfig; };
    const EParamType& getType() const { return m_Type; };
    std::string getUnit() const;
    const UnitParam* pUnitParam() const { return &m_Unit; }; /* dynamically pass the unit e.g. from a timeseries ModelParam to the corresponding ModelTS */

    const std::string& getComment() const { return m_Comment; };
    void setComment(const std::string& a_Comment)  { m_Comment = a_Comment; };
    
    const t_value& getDefault() const { return m_default; };
    std::optional<std::string> getStrDefaultValue() const;
    const t_value& getMin() const { return m_min; };
    const t_value& getMax() const { return m_max; };

    bool isPValue() const;
    const t_pvalue& getPtr() const { return p_Value; };
    size_t  size();
    t_value operator[](size_t i);

protected:
    std::string m_Name;
    EParamType m_Type;
    std::string m_Description; // Description/Definition of the parameter
    UnitParam m_Unit;
    std::string m_ShowConfig; // In GUI, the parameter is displayed only if m_ShowConfig is selected from DataFilter

    std::string m_Comment{};    // Optional user note (not initialized by the constructor)

    t_pvalue p_Value;
    bool m_create{ false };

    t_value m_default; // cas particulier pour vector<double>, le type peut �tre double
    t_value m_min;
    t_value m_max;

    FlagParam m_IsBlocking;
    FlagParam m_IsUsed;

    virtual void readParam(const std::string& aParamName, const t_mapParamData& a_Settings);
};


#endif