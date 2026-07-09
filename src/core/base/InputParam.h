#ifndef InputParam_H
#define InputParam_H
class InputParam;

#include "CairnCore_global.h"
#include <Eigen/SparseCore>
#include <Eigen/Dense>

#ifndef STRING
#define STRING
#include <string>
#endif
#include "CairnAPI.h"


#include "IndicatorName.h"
#include "ModelParam.h"

#include <cmath>
#include <optional>

inline constexpr double INFINITY_VAL = 1.e12;

namespace IndicatorNames {
    inline const std::string IS_INSTALLED = "is installed";
    // ... other indicator names
}

/**
 * \brief The InputParam class defines MilpComponent & MilpModel Input Parameter variables
 * Provides functionnality to read MilpComponent parameter values from settings and to set these values to Submodel Input Parameters
 */

class CAIRNCORESHARED_EXPORT InputParam : public CairnObject
{
    
public:
    InputParam (CairnObject* aParent, std::string aName="");
    ~InputParam();

    void removeParameter(const std::string& aParamName);
    void removeParameters();
    void removeImpactParameters(const std::string& impactName);
    void removePortImpactParameters(const std::string& portName);

    void removeIndicator(const std::string& indicatorName);
    void removeIndicators();
    void removeImpactIndicators(const std::string& impactName);

    /** @brief
    @param aParamName std::string: param name
    @param aPtr t_pvalue: pointer on the param value (possible type: double*, int*, bool*, std::string*, std::vector<std::string>*, std::vector<double>*)
    @param aDfltValue t_value: default value
    @param aIsBlocking bool: is this parameter mandatory?
    @param aIsUsed bool: conditions for which the parameter is used
    @param aDescription std::string&: comment to be displayed to help the user
    @param aUnit std::string&: unit of the parameter
    @param aShowConfig std::vector<std::string>: to be used to gather parameters in the same filter
    */
    void addParameter(const std::string& aParamName, const t_pvalue &aPtr, t_value aDefaultValue, t_flag aIsBlocking = true, t_flag aIsUsed = true, 
        const std::string& aDescription = "", const t_unit& aUnit = "", const std::string aShowConfig = "Base");

    /** @brief
    @param aParamName std::string: param (timeseries) name
    @param aDblePtr double*: pointer on the double value
    @param a_default double: default value (the actual value is a vector of a_default)
    @param aIsBlocking bool: is this parameter mandatory?
    @param aIsUsed bool: conditions for which the parameter is used
    @param aDescription std::string&: comment to be displayed to help the user
    @param aUnit std::string&: unit of the parameter
    @param aShowConfig std::vector<std::string>: to be used to gather parameters in the same filter
    @param a_min double: minimum value allowed
    @param a_max double: maximum value allowed
    */
    void addTimeSeries(const std::string& aParamName, std::vector<double>* aDblePtr, 
        double a_default = 1.0,        
        t_flag aIsBlocking = true, t_flag aIsUsed = true,
        const std::string& aDescription = "", const t_unit& aUnit = "",
        const std::string& aShowConfig = "Base",
        double a_min = std::nan("1"), double a_max = std::nan("1"));

    /** @brief 
    @param aParamName std::string: param name
    @param aDblePtr double*: pointer on the double value
    * it doesn't take a default value
    @param aIsBlocking bool: is this parameter mandatory? 
    @param aIsUsed bool: conditions for which the parameter is used
    @param aDescription std::string&: comment to be displayed to help the user
    @param aUnit std::string&: unit of the parameter
    * it doesn't take and ShowConfig because  it is name is hard-coded
    * it is not shown to the user as the user doesn't have to provide a name.
    * it doesn't take min and max values
    */
    void addPerfParam(const std::string& aParamName, std::vector<double>* aDblePtr, t_flag aIsBlocking = true, t_flag aIsUsed = true, 
        const std::string& aDescription = "", const t_unit& aUnit = "");

    /** @brief
    * Method publishData is used to publish/export IO variables
    @param aVarName std::string: IO variable name
    @param aSize int: the size of the corresponding 1D-Expression
    @param aDfltValue double: default value
    */
    void publishData(const std::string& aVarName, int aSize, double aDefault);

    /** @brief
    @param aIndicatorName std::string: indicator name
    @param aDblePtr double*: pointer on the double value
    @param aIsExported bool*: whether to export the indicator or not (into the result file)
    @param aDescription std::string&: comment to be displayed to help the user
    @param aUnit std::string&: unit of the parameter
    @param aShortName std::string: indicator short name (alias)
    */
    void addIndicator(const t_Name& aIndicatorName, std::vector<double>* aDblePtr, bool* aIsExported, const std::string& aDescription = "",
        const t_unit& aUnit = "", const t_Name& aShortName = "");
    
   
    /* ------------------------------------------------------------------------------------------------------------------------------- */

    int readParameters(const t_mapParamData& aSettings);
    void readVectorParameters (const std::string &aName, const std::string &aFileName, std::vector<std::string>& aPerfParamNames) ;
    
    int fillVectorData(const std::string& aName, const InputParam &aSrc, const uint& aOffset);

    void getParameters(std::vector<std::string>& a_List, const EParamType& a_Type);
    void getParameters(std::vector<std::string>& a_List, CairnAPI::ESettingsLimited a_setLimited = CairnAPI::all);
    bool getParameterValue(const std::string& a_SettingsName, std::string& a_Value, const EParamType& a_Type = eUndefined);
    bool getParameterValue(const std::string& a_SettingsName, t_value& a_Value);
    bool setParameterValue(const std::string& a_SettingsName, const std::string& a_SettingsValue);
    bool setParameterValue(const std::string& a_SettingsName, const t_value& a_SettingsValue);

    void addToShowConfigList(const std::string& aConfig);
    static int checkProfile(const std::string aName, const Eigen::VectorXf& aProfile, const float aInf, const float aSup);

    void jsonSaveGUIInputParam(ojson& paramArray);

    
    class ModelIndicator {
    public:
        ModelIndicator(CairnObject* aParent = nullptr,
            const t_Name& aIndicatorName = "",
            std::vector<double>* aDblePtr = nullptr, 
            bool* aBoolPtr = nullptr, 
            const std::string& aDesc = "", 
            const t_unit& aUnit = "",
            const t_Name& aShortName = "");

        std::string getName() const;
        std::string getShortName() const;
        std::string getUnit() const;
        bool IsExported() const;
        void Export(std::fstream& out, const std::string& aComponentName, const std::string& range, bool aForceExport, 
            bool showDescription, const std::vector<std::string>& labels = {}) const;
        void Export(std::fstream& out, const std::string& aComponentName, const std::string& range, bool aForceExport, bool aIsSizeOptimized, 
            bool aIsPriceOptimized, bool isRollingHorizon, const std::vector<double> &aOptimalSizeAllCycles, const bool showDescription, 
            const std::vector<std::string>& labels = {}) const;
        double getValue(size_t aIndex=0) const;
        void resetValue(); //reset indicator value to 0
    protected:
        SubModel* pModel;
        IndicatorName m_Name;
        IndicatorName m_ShortName;
        std::string m_Comment;
        UnitParam m_Unit;
        bool *p_IsExported;
        std::vector<double>  *p_Value;
    };
    
    typedef std::vector<ModelIndicator*> t_Indicators;
    typedef std::map<std::string, ModelParam*> t_mapParams;
    
    const t_mapParams& getMapParams() const { return mMapParams; };    

    void getParameters(std::vector<ModelParam*>& a_List, const EParamType& a_Type);
    ModelParam* getParameter(const std::string &aName);

    const std::vector <std::string>& getShowConfigList() const { return mShowConfigList; };

    const t_Indicators& getIndicators() const { return mIndicators; };

private:    
    std::vector <std::string> mShowConfigList = {"Base"};  /** List of possible parameter set configurations */
    t_mapParams mMapParams = {};
    t_Indicators mIndicators = {};
};

#endif // InputParam_H

