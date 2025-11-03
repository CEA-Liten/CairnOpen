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
#include "FlagParam.h"
#include "UnitParam.h"
#include <cmath>

const double INFINITY_VAL = 1.e12;

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
    void addIndicator(const std::string& aIndicatorName, std::vector<double>* aDblePtr, bool* aIsExported, const std::string& aDescription = "",
        const t_unit& aUnit = "", const std::string& aShortName = "");
    
   
    /* ------------------------------------------------------------------------------------------------------------------------------- */

    int readParameters(const std::map<std::string, std::string>& aSettings);
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

    class ModelParam
    {
    public:
        // default contractor  
        ModelParam(const std::string& a_Name = "",
            t_flag a_IsBlocking = false,
            t_flag a_IsUsed = true,
            const std::string& a_Comment = "",
            const t_unit& aUnit = "",
            const std::string& a_ShowConfig = "");

        // t_pvalue (scalar and vector of string parameters) 
        ModelParam(const std::string& a_Name,
            const t_pvalue &ap_Value,
            t_value a_defaultValue,
            t_flag a_IsBlocking = false,
            t_flag a_IsUsed = true,
            const std::string& a_Comment = "",
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
            const std::string& a_Comment = "",
            const t_unit& aUnit = "",
            const std::string& a_ShowConfig = "");

        // publish IO var name
        ModelParam(const std::string& a_Name, int aSize, double aDefault);

        ~ModelParam();

        virtual std::string toString();
        virtual bool setValue(const std::string& a_Value);
        virtual bool setValue(const t_value& a_Value);
        virtual t_value getValue();
        bool getNumValue(double &a_Value); // return if possible a double value
        bool copyValues(const ModelParam& aSrc, size_t aSize, size_t aOffset = 0);
        bool copyValues(const std::vector<double> &aSrc, size_t aOffset = 0);
        bool setValues(const double& aValue, size_t aSize);

        bool readParameter(const std::map<std::string, std::string>& aSettings); 
        bool IsBlocking();
        bool IsUsed();
        bool isDependent(); /* whether m_IsBlocking is a scalr or depends on other parameetrs */
        TriState isModified();

        const std::string& getName() const { return m_Name; };
        const std::string& getDescription() const { return m_Comment; };
        const std::string& getShowConfig() const { return m_ShowConfig; };
        const EParamType& getType() const { return m_Type; };
        std::string getUnit() const;
        const UnitParam* pUnitParam() const { return &m_Unit; }; /* Used to dynamically pass the unit e.g. from a timeseries ModelParam to the corresponding ModelTS */

        const t_value& getDefault() const { return m_default; };
        const t_value& getMin() const { return m_min; };
        const t_value& getMax() const { return m_max; };

        bool isPValue() const;
        const t_pvalue& getPtr() const { return p_Value; };
        size_t  size();
        t_value operator[](size_t i);

    protected:
        std::string m_Name;
        EParamType m_Type;
        std::string m_Comment;
        UnitParam m_Unit; 
        std::string m_ShowConfig; // In GUI, the parameter is displayed only if m_ShowConfig is selected from DataFilter

        t_pvalue p_Value;
        bool m_create{ false }; 

        t_value m_default; // cas particulier pour vector<double>, le type peut �tre double
        t_value m_min;
        t_value m_max;
       
        FlagParam m_IsBlocking;
        FlagParam m_IsUsed;
        
        virtual void readParam(const std::string& aParamName, const std::map<std::string, std::string>& a_Settings);
    };
    class ModelIndicator {
    public:
        ModelIndicator(const std::string& aIndicatorName = "", 
            std::vector<double>* aDblePtr = nullptr, 
            bool* aBoolPtr = nullptr, 
            const std::string& aDesc = "", 
            const t_unit& aUnit = "",
            const std::string& aShortName = "");

        const std::string& getName() const { return m_Name; };
        const std::string& getShortName() const { return m_ShortName; };
        std::string getUnit() const;
        bool IsExported();
        void Export(std::fstream& out, const std::string& aComponentName, const std::string&range, bool aForceExport, 
            const bool showDescription, const std::vector<std::string>& labels = {});
        void Export(std::fstream& out, const std::string& aComponentName, const std::string& range, bool aForceExport, bool aIsSizeOptimized, 
            bool aIsPriceOptimized, bool isRollingHorizon, const std::vector<double> &aOptimalSizeAllCycles, const bool showDescription, 
            const std::vector<std::string>& labels = {});
        double getValue(size_t aIndex=0);
        void resetValue(); //reset indicator value to 0
    protected:
        std::string m_Name;
        std::string m_ShortName;
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
    std::vector <std::string> mShowConfigList = {};  /** List of possible parameter set configurations */
    t_mapParams mMapParams = {};
    t_Indicators mIndicators = {};
};

#endif // InputParam_H

