#ifndef InputParam_H
#define InputParam_H
class InputParam ;

#include <QtCore>
#include <QDir>
#include "CairnCore_global.h"
#include <Eigen/SparseCore>
#include <Eigen/Dense>

#ifndef STRING
#define STRING
#include <string>
#endif
#include "CairnAPI.h"
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
    eVectorInt,
    eVectorEigen,
    eQVectorfloat
};
typedef std::variant<double*, int*, bool*, QString*, QStringList*, std::vector<double>*, std::vector<int>*, Eigen::VectorXf*, QVector<float>*> t_pvalue;

enum EFunctFlagType {
    eFTypeUndefined = -1,
    eFTypeNotAnd = 0,   // !Flags_i && !Flags_i ... && Flags2_j && Flags2_j ...
    eFTypeOrNot,         // Flags_i || Flags_i ... || !Flags2_j || !Flags2_j ...        
    eFTypeNotAndOr      // !Flags_i && !Flags_i ... && (Flags2_j || Flags2_j ...)
};
struct SExtFunctionFlag {
    bool (*pFunct)(class SubModel* ap_Model) { nullptr };
    class SubModel* pModel{ nullptr };
};
struct SFunctionFlag {
    EFunctFlagType Type;
    std::vector<bool*> Flags;
    std::vector<bool*> Flags2;  
    SExtFunctionFlag ExtFunct;
};

typedef std::variant<bool, bool*, SFunctionFlag, SExtFunctionFlag> t_flag;


/**
 * \brief The InputParam class defines MilpComponent & MilpModel Input Parameter variables
 * Provides functionnality to read MilpComponent parameter values from settings and to set these values to Submodel Input Parameters
 */

class CAIRNCORESHARED_EXPORT InputParam : public QObject
{
    Q_OBJECT
public:
    InputParam (QObject* aParent, QString aName="");
    virtual ~InputParam();

    void removeParameter(const QString& aParamName);

    /** @brief
    @param aParamName QString: param name
    @param aPtr t_pvalue: pointer on the param value (possible type: double*, int*, bool*, QString*, QStringList*, std::vector<double>*)
    @param aDfltValue t_value: default value
    @param isBlocking bool: is this parameter mandatory?
    @param isUsed bool: conditions for which the parameter is used
    @param aDescription QString&: comment to be displayed to help the user
    @param aUnit QString&: unit of the parameter
    @param aConfigList QList<QString>: to be used to gather parameters in the same filter
    */
    void addParameter(const QString& aParamName, const t_pvalue &aPtr, t_value aDefaultValue, t_flag aIsBlocking = true, t_flag aIsUsed = true, const QString& aDescription = "", const QString& aUnit = "", const QList<QString>& aconfigList = { "" });

    //scalar: takes default value:
    /** @brief
    @param aParamName QString: param name
    @param aBoolPtr bool*: pointer on the bool value
    @param aDfltValue bool: default value
    @param isBlocking bool: is this parameter mandatory?
    @param isUsed bool: conditions for which the parameter is used
    @param aDescription QString&: comment to be displayed to help the user
    @param aUnit QString&: unit of the parameter
    @param aConfigList QList<QString>: to be used to gather parameters in the same filter
    */
    void addParameter(const QString &aParamName, bool* aBoolPtr, bool aDefaultValue, t_flag aIsBlocking=true, t_flag aIsUsed=true, const QString &aDescription="", const QString &aUnit="", const QList<QString> &aconfigList={""}) ;
    /** @brief
    @param aParamName QString: param name
    @param aQStringPtr QString*: pointer on the QString value
    @param aDfltValue QString: default value
    @param isBlocking bool: is this parameter mandatory?
    @param isUsed bool: conditions for which the parameter is used
    @param aDescription QString&: comment to be displayed to help the user
    @param aUnit QString&: unit of the parameter
    @param aConfigList QList<QString>: to be used to gather parameters in the same filter
    */
    void addParameter(const QString &aParamName, QString* aQstring, QString aDefaultValue, t_flag IsBlocking=true, t_flag aIsUsed = true, const QString &aDescription="", const QString &aUnit="", const QList<QString> &aconfigList={""}) ;
    /** @brief
    @param aParamName QString: param name
    @param aIntPtr int*: pointer on the int value
    @param aDfltValue int: default value
    @param isBlocking bool: is this parameter mandatory?
    @param isUsed bool: conditions for which the parameter is used
    @param aDescription QString&: comment to be displayed to help the user
    @param aUnit QString&: unit of the parameter
    @param aConfigList QList<QString>: to be used to gather parameters in the same filter
    */
    void addParameter(const QString &aParamName, int* aIntPtr, int aDefaultValue, t_flag IsBlocking=true, t_flag aIsUsed=true, const QString &aDescription="", const QString &aUnit="", const QList<QString> &aconfigList={""}) ;
    /** @brief
    @param aParamName QString: param name
    @param aDblePtr double*: pointer on the double value
    @param aDfltValue double: default value
    @param isBlocking bool: is this parameter mandatory?
    @param isUsed bool: conditions for which the parameter is used
    @param aDescription QString&: comment to be displayed to help the user
    @param aUnit QString&: unit of the parameter
    @param aConfigList QList<QString>: to be used to gather parameters in the same filter
    */
    void addParameter(const QString &aParamName, double* aDblePtr, double aDefaultValue, t_flag IsBlocking=true, t_flag aIsUsed = true, const QString &aDescription="", const QString &aUnit="", const QList<QString> &aconfigList={""}) ;
    
    //vector : doesn't take default value
    void addParameter(const QString& aParamName, QStringList* aQSLPtr, t_flag IsBlocking=true, t_flag aIsUsed = true, const QString& aDescription = "", const QString& aUnit = "", const QList<QString>& aconfigList = { "" });
    void addParameter(const QString &aParamName, std::vector<int>* aIntPtr, t_flag IsBlocking=true, t_flag aIsUsed = true, const QString &aDescription="", const QString &aUnit="", const QList<QString> &aconfigList={""}) ;
    void addParameter(const QString& aParamName, std::vector<double>* aDblePtr, t_flag IsBlocking=true, t_flag aIsUsed = true, const QString& aDescription="", const QString& aUnit = "", const QList<QString>& aconfigList = { "" });
    void addParameter(const QString& aParamName, std::vector<double>* aDblePtr, t_flag IsBlocking, t_flag aIsUsed, const QString& aDescription, const QList<QString>& aQuantities, const QList<QString>& aconfigList = {""});

    void addTimeSeries(const QString& aParamName, std::vector<double>* aDblePtr, 
        double a_default = 1.0,        
        t_flag IsBlocking = true, t_flag aIsUsed = true,
        const QString& aDescription = "", const QString& aUnit = "",
        const QList<QString>& aconfigList = { "" },
        double a_min = std::nan("1"), double a_max = std::nan("1"));


    //used to tranfer data from MilpComponent to SubModel
    void publishData(const QString& aParamName, bool* aBoolPtr);
    void publishData(const QString& aParamName, QString* aQstring);
    void publishData(const QString& aParamName, int* aIntPtr);
    void publishData(const QString& aParamName, double* aDblePtr);
    void publishData(const QString& aParamName, const double* aDblePtr);
    void publishData(const QString& aParamName, Eigen::VectorXf* aVXfPtr);
    void publishData(const QString& aParamName, int aSize, double aDefault);

    void addIndicator(const QString& aIndicatorName, std::vector<double>* aDblePtr, bool* aBoolPtr, const QString& aDescription = "", const QString& aUnit = "", const QString& aShortName = "");

    int readParameters(const QMap<QString, QString>& aSettings);
    int readParameters(const QString& aName, const QSettings& aSettings);
    void readVectorParameters (const QString &aName, const QString &aFileName, QList<QString>& aPerfParamNames) ;
    
    int fillVectorData(const QString& aName, const InputParam &aSrc, const uint& aOffset);
    int fillData(const QString& aName, const InputParam& aSrc, const EParamType& aType);

    QString getParamQSValue(const QString& aParamName);
    void getParameters(QList<QString>& a_List, const EParamType& a_Type);
    void getParameters(QList<QString>& a_List, CairnAPI::ESettingsLimited a_setLimited = CairnAPI::all);
    bool getParameterValue(const QString& a_SettingsName, QString& a_Value, const EParamType& a_Type = eUndefined);
    bool getParameterValue(const QString& a_SettingsName, t_value& a_Value);
    bool setParameterValue(const QString& a_SettingsName, const QString& a_SettingsValue);
    bool setParameterValue(const QString& a_SettingsName, const t_value& a_SettingsValue);

    void addToConfigList(const QList<QString> aConfigsList) { mConfigsList = aConfigsList; }
    void checkConfigParameter (const QString &aParamName, const QList<QString> &aconfigList) ;
    static int checkProfile(const QString aName, const Eigen::VectorXf& aProfile, const float aInf, const float aSup);

    void jsonSaveGUIInputParam(QJsonArray& paramArray);

    class ModelParam
    {
    public:
        // default
        ModelParam(const QString& a_Name = "",
            t_flag a_IsBlocking = false,
            t_flag a_IsUsed = true,
            const QString& a_Comment = "",
            const QString& a_Unit = "");        
        ModelParam(const QString& a_Name,
            const t_pvalue &ap_Value,
            t_value a_defaultValue,
            t_flag a_IsBlocking = false,
            t_flag a_IsUsed = true,
            const QString& a_Comment = "",
            const QString& a_Unit = "");
        // bool
        ModelParam(const QString& a_Name, bool* ap_Value);
        ModelParam(const QString& a_Name,
            bool* ap_Value,
            bool a_defaultValue, 
            t_flag a_IsBlocking = false,
            t_flag a_IsUsed = true,
            const QString& a_Comment = "");
        // string
        ModelParam(const QString& a_Name, QString* ap_Value);
        ModelParam(const QString& a_Name,
            QString* ap_Value,
            QString a_defaultValue,
            t_flag a_IsBlocking = false,
            t_flag a_IsUsed = true,
            const QString& a_Comment = "",
            const QString& a_Unit = "");
        // double
        ModelParam(const QString& a_Name, double* ap_Value);
        ModelParam(const QString& a_Name,
            double* ap_Value,
            double a_defaultValue,
            t_flag a_IsBlocking,
            t_flag a_IsUsed,
            const QString& a_Comment = "",
            const QString& a_Unit = "",
            double a_min = std::nan("1"),
            double a_max = std::nan("1"));        
        // int
        ModelParam(const QString& a_Name, int* ap_Value);
        ModelParam(const QString& a_Name,
            int* ap_Value,
            int a_defaultValue,
            t_flag a_IsBlocking = false,
            t_flag a_IsUsed = true,
            const QString& a_Comment = "",
            const QString& a_Unit = "",
            int a_min = std::numeric_limits<int>::max(),
            int a_max = std::numeric_limits<int>::max());
        // string list
        ModelParam(const QString& a_Name,
            QStringList* ap_Value,
            t_flag a_IsBlocking = false,
            t_flag a_IsUsed = true,
            const QString& a_Comment = "");
        // vector double
        ModelParam(const QString& a_Name,
            std::vector<double>* ap_Value,
            t_flag a_IsBlocking = false,
            t_flag a_IsUsed = true,
            const QString& a_Comment = "",
            const QString& a_Unit = "");
        ModelParam(const QString& a_Name,
            std::vector<double>* ap_Value,
            t_flag a_IsBlocking = false,
            t_flag a_IsUsed = true,
            const QString& a_Comment = "",
            const QList<QString>& a_Quantities = { "" });
        ModelParam(const QString& a_Name,
            std::vector<double>* ap_Value,
            double a_default = 1.0,
            double a_min = std::nan("1"),
            double a_max = std::nan("1"),
            t_flag a_IsBlocking = false,
            t_flag a_IsUsed = true,
            const QString& a_Comment = "",
            const QString& a_Unit = "");
        // vector int
        ModelParam(const QString& a_Name,
            std::vector<int>* ap_Value,
            t_flag a_IsBlocking = false,
            t_flag a_IsUsed = true,
            const QString& a_Comment = "",
            const QString& a_Unit = "");
        // vector eigen
        ModelParam(const QString& a_Name, Eigen::VectorXf* ap_Value);
        ModelParam(const QString& a_Name, int aSize, double aDefault);
        ~ModelParam();

        virtual QString toString();
        virtual bool setValue(const QString& a_Value);
        virtual bool setValue(const t_value& a_Value);
        virtual t_value getValue();
        bool getNumValue(double &a_Value); // return if possible a double value
        bool copyValues(const ModelParam& aSrc, size_t aSize, size_t aOffset = 0);
        bool copyValues(const QVector<float> &aSrc, size_t aOffset = 0);
        bool setValues(const double& aValue, size_t aSize);

        bool readParameter(const QString& a_Name, const QSettings& aSettings);
        bool readParameter(const QMap<QString, QString>& aSettings);//aName is only needed in case of QSettings to filter by componenet
        bool IsBlocking();
        bool IsUsed() ;
        TriState isModified();

        const QString& getName() const { return m_Name; };
        const QString& getDescription() const { return m_Comment; };
        //const QString& getUnit() const { return m_Quantities[0]; };
        const QList<QString>& getQuantities() const { return m_Quantities; };
        const EParamType& getType() const { return m_Type; };
        bool isPValue() const;
        const t_pvalue& getPtr() const { return p_Value; };
        size_t  size();
        t_value operator[](size_t i);

        const t_value& getDefault() const { return m_default; };
        const t_value& getMin() const { return m_min; };
        const t_value& getMax() const { return m_max; };

        class FlagParam
        {
        public:
            FlagParam();
            void set_Value(t_flag a_Flag);
            bool get_Value();
        private:
            bool m_Value;
            bool* p_Value;
            SFunctionFlag m_Function;
            SExtFunctionFlag m_ExtFunct;
        };
    protected:
        QString m_Name;
        EParamType m_Type;
        QString m_Comment;
        QList<QString> m_Quantities;
        
        t_pvalue p_Value;
        bool m_create{ false }; 

        t_value m_default; // cas particulier pour vector<double>, le type peut être double
        t_value m_min;
        t_value m_max;
       
        FlagParam m_IsBlocking;
        FlagParam m_IsUsed;
        
        virtual void readParam(const QString& aParamName, const QMap<QString, QString>& a_Settings);
        virtual void readParam(const QString& aParamName, const QVariant& a_Setting);
    };
    class ModelIndicator {
    public:
        ModelIndicator(const QString& aIndicatorName = "", 
            std::vector<double>* aDblePtr = nullptr, 
            bool* aBoolPtr = nullptr, 
            const QString& aDesc = "", 
            const QString& aUnit = "", 
            const QString& aShortName = "");

        const QString& getName() const { return m_Name; };
        const QString& getShortName() const { return m_ShortName; };
        const QString& getUnit() const { return m_Unit; };
        bool IsExported();
        void Export(QTextStream& out, const QString& aComponentName, const QString &range, bool aForceExport);
        void Export(QTextStream& out, const QString& aComponentName, const QString& range, bool aForceExport, 
            bool aIsSizeOptimized, bool aIsPriceOptimized, bool isRollingHorizon, const std::vector<double> &aOptimalSizeAllCycles);
        double getValue(size_t aIndex=0);
    protected:
        QString m_Name;
        QString m_ShortName;
        QString m_Comment;
        QString m_Unit;
        bool *p_IsExported;
        std::vector<double>  *p_Value;
    };
    
    typedef std::vector<ModelIndicator*> t_Indicators;
    typedef std::map<QString, ModelParam*> t_mapParams;
    
    const t_mapParams& getMapParams() const { return mMapParams; };    

    void getParameters(QList<ModelParam*>& a_List, const EParamType& a_Type);
    ModelParam* getParameter(const QString &aName);

    const t_Indicators& getIndicators() const { return mIndicators; };

private:    
    QList<QString> mConfigsList ;  /** List of possible parameter set configurations */
    
    t_mapParams mMapParams;  
    t_Indicators mIndicators;
};

#endif // InputParam_H

