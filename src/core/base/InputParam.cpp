#include "GlobalSettings.h"
#include "InputParam.h"
#include "CairnUtils.h"

//#include "base/OptimProblem.h"
using namespace GS ;

InputParam::InputParam (QObject *aParent, QString aName): QObject(aParent)
{
    this->setObjectName(aName);
}

InputParam::~InputParam()
{
    for (auto & [iParName, val] : mMapParams) {
        if (val) {
            delete val;
            val = nullptr;
        }
    }         
    mMapParams.clear();
    for (auto& val : mIndicators) {
        if (val) {
            delete val;
            val = nullptr;
        }
    }
    mIndicators.clear();
}

QString InputParam::getParamQSValue(const QString &aParamName)
{ /** aParamName supposed to be existing in mMapParamQS */
    QString vValue = "";
    getParameterValue(aParamName, vValue, eString);
    return vValue;
}

//Model InputParameter Interface
void InputParam::checkConfigParameter (const QString &aParamName, const QList<QString> &aconfigList)
{
    if (GS::iVerbose >2)
    {
        QFile logFile (QDir::currentPath()+"/ConfigParam.log") ;
        logFile.open(QIODevice::Append | QIODevice::Text) ;
        QTextStream txtLogFile (&logFile) ;
        txtLogFile.setFieldAlignment(QTextStream::FieldAlignment::AlignLeft) ;
        foreach (QString config, aconfigList)
        {
            if (mConfigsList.indexOf(config) <= 0)
            {
                if (config != "" && config != "DONOTSHOW")
                {
                    txtLogFile << this->objectName() << aParamName << " is part of configuration set : " << config << " which has not been declared by addToConfigsList \n";
                }
                else if (config == "DONOTSHOW")
                {
                    txtLogFile << this->objectName() << aParamName << " will not be displayed as it is part of configuration set : " << config << " \n";
                }
                else
                {
                    txtLogFile << this->objectName() << aParamName << " will be part of every configuration set  as non configuration has been set for it \n";
                }
            }
        }
        logFile.close();
    }
}

void InputParam::removeParameter(const QString& aParamName) {
    if (mMapParams.find(aParamName) != mMapParams.end()) {
        delete mMapParams[aParamName];
        mMapParams.erase(aParamName);
    }
}

//-------------------------------------------- scalar param --------------------------------------------
void InputParam::addParameter(const QString& aParamName, const t_pvalue &aPtr, t_value aDefaultValue, t_flag aIsBlocking, t_flag aIsUsed, const QString& aDescription, const QString& aUnit, const QList<QString>& aconfigList)
{
    checkConfigParameter(aParamName, aconfigList);
    mMapParams[aParamName] = new ModelParam(aParamName, aPtr, aDefaultValue, aIsBlocking, aIsUsed, aDescription, aUnit);
}

void InputParam::addParameter(const QString &aParamName, bool* aBoolPtr, bool aDefaultValue, t_flag aIsBlocking, t_flag aIsUsed, const QString &aDescription, const QString &aUnit, const QList<QString> &aconfigList)
{   
    checkConfigParameter(aParamName, aconfigList) ;
    mMapParams[aParamName] = new ModelParam(aParamName, aBoolPtr, aDefaultValue, aIsBlocking, aIsUsed, aDescription);
}
void InputParam::addParameter(const QString &aParamName, QString* aQstring, QString aDefaultValue, t_flag IsBlocking, t_flag aIsUsed, const QString &aDescription, const QString &aUnit, const QList<QString> &aconfigList)
{    
    checkConfigParameter(aParamName, aconfigList) ;
    mMapParams[aParamName] = new ModelParam(aParamName, aQstring, aDefaultValue, IsBlocking, aIsUsed, aDescription, aUnit);
}
void InputParam::addParameter(const QString &aParamName, int* aIntPtr, int aDefaultValue, t_flag IsBlocking, t_flag aIsUsed, const QString &aDescription, const QString &aUnit, const QList<QString> &aconfigList)
{    
    checkConfigParameter(aParamName, aconfigList) ;
    mMapParams[aParamName] = new ModelParam(aParamName, aIntPtr, aDefaultValue, IsBlocking, aIsUsed, aDescription, aUnit);
}
void InputParam::addParameter(const QString &aParamName, double *aDblePtr, double aDefaultValue, t_flag IsBlocking, t_flag aIsUsed, const QString &aDescription, const QString &aUnit, const QList<QString> &aconfigList)
{      
    checkConfigParameter(aParamName, aconfigList) ;
    mMapParams[aParamName] = new ModelParam(aParamName, aDblePtr, aDefaultValue, IsBlocking, aIsUsed, aDescription, aUnit);
}
//-------------------------------------------- vector param --------------------------------------------
void InputParam::addParameter(const QString& aParamName, QStringList* aQSLPtr, t_flag IsBlocking, t_flag aIsUsed, const QString& aDescription, const QString& aUnit, const QList<QString>& aconfigList)
{
    checkConfigParameter(aParamName, aconfigList);
    mMapParams[aParamName] = new ModelParam(aParamName, aQSLPtr, IsBlocking, aIsUsed, aDescription);
}
void InputParam::addParameter(const QString &aParamName, std::vector<int>* aIntPtr, t_flag IsBlocking, t_flag aIsUsed, const QString &aDescription, const QString &aUnit, const QList<QString> &aconfigList)
{ 
    checkConfigParameter(aParamName, aconfigList) ;
    mMapParams[aParamName] = new ModelParam(aParamName, aIntPtr, IsBlocking, aIsUsed, aDescription, aUnit);
}
void InputParam::addParameter(const QString &aParamName, std::vector<double>* aDblePtr, t_flag IsBlocking, t_flag aIsUsed, const QString &aDescription, const QString &aUnit, const QList<QString> &aconfigList)
{
    checkConfigParameter(aParamName, aconfigList) ;
    mMapParams[aParamName] = new ModelParam(aParamName, aDblePtr, IsBlocking, aIsUsed, aDescription, aUnit);
}
void InputParam::addParameter(const QString& aParamName, std::vector<double>* aDblePtr, t_flag IsBlocking, t_flag aIsUsed, const QString& aDescription, const QList<QString>& aQuantities, const QList<QString>& aconfigList)
{
    checkConfigParameter(aParamName, aconfigList);
    mMapParams[aParamName] = new ModelParam(aParamName, aDblePtr, IsBlocking, aIsUsed, aDescription, aQuantities);
}

//------------------------------ vector param (time series) --------------------------------------------
void InputParam::addTimeSeries(const QString& aParamName, std::vector<double>* aDblePtr, double a_default, t_flag aIsBlocking, t_flag aIsUsed, const QString& aDescription, const QString& aUnit, const QList<QString>& aconfigList, double a_min, double a_max)
{
    checkConfigParameter(aParamName, aconfigList);
    mMapParams[aParamName] = new ModelParam(aParamName, aDblePtr, a_default, a_min, a_max, aIsBlocking, aIsUsed, aDescription, aUnit);
}

//-------------------------------------------- used to transfer data from MilpComponent to SubModel --------------------------------------------
void InputParam::publishData(const QString& aParamName, bool* aBoolPtr)
{
    mMapParams[aParamName] = new ModelParam(aParamName, aBoolPtr);
}
void InputParam::publishData(const QString& aParamName, QString* aQstring)
{
    mMapParams[aParamName] = new ModelParam(aParamName, aQstring);
}
void InputParam::publishData(const QString& aParamName, int* aIntPtr)
{
    mMapParams[aParamName] = new ModelParam(aParamName, aIntPtr);
}
void InputParam::publishData(const QString& aParamName, double* aDblePtr)
{
    mMapParams[aParamName] = new ModelParam(aParamName, aDblePtr);
}
void InputParam::publishData(const QString& aParamName, const double* aDblePtr)
{
    mMapParams[aParamName] = new ModelParam(aParamName, const_cast<double*>(aDblePtr));
}
void InputParam::publishData(const QString& aParamName, Eigen::VectorXf* aVXfPtr)
{
    mMapParams[aParamName] = new ModelParam(aParamName, aVXfPtr);
}
void InputParam::publishData(const QString& aParamName, int aSize, double aDefault)
{
    mMapParams[aParamName] = new ModelParam(aParamName, aSize, aDefault);
}

/*
* Define an indicator
* arg1: Name that will be displayed in _PLAN & _HIST
* arg2: Pointer to value
* arg3: Pointer to boolean to choose whether the indicator is exported or not
* arg4: Comment that could be displayed as a tooltip 
* arg5: Unit of the indicator
* arg6: Shortname that will be displayed in UserDefinedIndicators
*/
void InputParam::addIndicator(const QString& aIndicatorName, std::vector<double>* aDblePtr, bool* aBoolPtr, const QString& aDesc, const QString& aUnit, const QString& aShortName)
{
    //Verify if indicator already exists. This is needed because mCompoModel->declareModelIndicators() maybe called several times when API is used.
    //Note, moving mCompoModel->declareModelIndicators() to initSubModelInput results in calling it every cycle !!
    for (auto const& indicator : mIndicators) {
        if (indicator->getName() == aIndicatorName) {
            //If indicator already exist, then updat it (this is necessary for EnvImpact indicators)
            auto it = find(mIndicators.begin(), mIndicators.end(), indicator);
            int iIndex = it - mIndicators.begin();
            mIndicators.at(iIndex) = new ModelIndicator(aIndicatorName, aDblePtr, aBoolPtr, aDesc, aUnit, aShortName);
            return; 
        }
    }
    mIndicators.push_back(new ModelIndicator(
        aIndicatorName, aDblePtr, aBoolPtr, aDesc, aUnit, aShortName));    
}

int InputParam::checkProfile (const QString aName, const Eigen::VectorXf &aProfile, const float aInf, const float aSup)
{
    float maxVal = aProfile.maxCoeff() ;
    float minVal = aProfile.minCoeff() ;
    if (maxVal < aInf || maxVal > aSup || minVal < aInf || minVal > aSup )
    {
        qCritical() << "ERROR in profile " << aName
                    << " values should be in the range ["<< aInf << ";" << aSup << "] "
                    << " instead of ["<< minVal << ";" << maxVal << "] " ;
        return -1 ;
    }
    return 0 ;
}

// create and fill vector parameters from reading of parameter file - Caution : for the moment only use double values !
void InputParam::readVectorParameters (const QString &aName, const QString &aFileName, QList<QString> &aPerfParamNames)
{
    QFile dataFileName(aFileName);
    if (!dataFileName.exists()) return;
    
    QList<QStringList> data_Inputs = GS::readFromCsvFile (aFileName, ";");
    qInfo() << "Reading vector parameters from file " << aFileName ;
    // Loop on expected input parameters
    for (auto const& [iParName, val] : mMapParams) {
        if (val) {
            if (val->getType() == eVectorDouble) {
                QString varName = QString(aName + "." + iParName);
                for (int i = 0; i < (data_Inputs.at(0)).size(); ++i)
                {
                    QString colName = (data_Inputs.at(0)).at(i);
                    QString simplifiedColName = colName.simplified().replace(" ", "");
                    QString simplifiedVarName = varName.simplified().replace(" ", "");
                    if (simplifiedColName == simplifiedVarName || simplifiedColName.split("(")[0] == simplifiedVarName) // first line should include column description : element.arrayName
                    {
                        std::vector<double> data_vector = GS::getDataArray(data_Inputs, i, 1);
                        if (simplifiedColName.contains("(r=") || simplifiedColName.contains("(c=")) {
                            int n = simplifiedColName.split("=")[1].split(")")[0].toInt();
                            if (simplifiedColName.contains("(r=")) {//row
                                if (data_vector.size() % n != 0) {
                                    qCritical() << "ERROR: the data size in column " + colName + " is not a multiplication of " + QString::number(n);
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
                                    qCritical() << "ERROR: the data size in column " + colName + " is not a multiplication of " + QString::number(n);
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
                        qInfo() << "Found data for performance parameter: " << (aName + "." + iParName);
                        aPerfParamNames.removeAll(iParName);
                        break;
                    }
                }
            }
        }
    }
}

// fill in Vector Data by copying elements of aMapParamVXf map of input Eigen::VectorXf* in map aMapParamVD of SubModel input of std::vector<double> *
int InputParam::fillVectorData(const QString& aName, const InputParam& aSrc, const uint& aOffset)
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
                                    qCritical() << "ERROR fillVectorData: in SubModel, allocation of vector variable is missing for " << (aName + "." + key);
                                    return -1;
                                }
                                else if (vDestSize == 0)
                                {
                                    qWarning() << "Warning fillVectorData: in SubModel, allocation of vector variable is missing for " << (aName + "." + key);
                                    continue;
                                }                                
                                try
                                {
                                    val->copyValues(*vIter->second, vDestSize, aOffset);
                                    qInfo() << "- Read Vector Data Time Series : " << (aName + "." + key); //<< (*val)[0] ; // << " = " << (*versSubModel).at(0) << (*versSubModel).at((*versSubModel).size() - 1);
                                }
                                catch (const std::exception&e)
                                {
                                    qCritical() << "ERROR fillVectorData: in SubModel, variable size of " << (aName + "." + key) << vDestSize;
                                    qCritical() << e.what();
                                    return -1;
                                }                                                                
                            }
                        }
                    }
                    if (!vFindSrc) {
                        if (isBlocking)
                        {
                            qCritical() << "ERROR: nullptr pointer for component variable name " << (aName + "." + key);
                            return -1;
                        }
                        else
                        {
                            if (GS::iVerbose > 0) qWarning() << "Optionnal parameter not found in component - Hope will not be used by submodel ! " << (aName + "." + key) << endl << flush;
                            continue;
                        }
                    }
                }  
                else {
                    if (isBlocking)
                    {
                        qCritical() << "ERROR fillVectorData: nullptr pointer for component variable name " << (aName + "." + key);
                        return -1;
                    }
                    else
                    {
                        if (GS::iVerbose > 0) qWarning() << "WARNING fillVectorData: nullptr pointer for component variable name " << (aName + "." + key);
                        if (GS::iVerbose > 0) qWarning() << "SubModel vector variable will then NOT be initialized " << (aName + "." + key);
                    }
                }
            }
        }        
    }
    return 0;
}

int InputParam::fillData(const QString& aName, const InputParam& aSrc, const EParamType& aType)
{
    const t_mapParams& vSrcParams = aSrc.getMapParams();
    for (auto const& [key, val] : mMapParams) {
        if (val) {
            if (val->getType() == aType) {
                bool isBlocking = val->IsBlocking();
                if (val->isPValue()) {
                    t_mapParams::const_iterator vIter = vSrcParams.find(key);
                    bool vFindSrc = false;
                    if (vIter != vSrcParams.end()) {
                        if (vIter->second) {
                            if (vIter->second->getType() == aType && vIter->second->isPValue()) {
                                vFindSrc = true;
                                val->setValue(vIter->second->getValue());
                                qDebug() << "- Copy Scalar Data from Component to SubModel : " << (aName + "." + key) << " = " << val->toString();
                            }
                        }
                    }
                    if (!vFindSrc) {
                        if (isBlocking)
                        {
                            qCritical() << "Looking for variable defined in submodel = " << key;
                            for (auto const& [keySrc, valSrc] : vSrcParams) {
                                qInfo() << " found variable in component " << keySrc << " IsEqual to " << key << (key == keySrc);
                            }
                            qCritical() << "ERROR fillData: no corresponding component variable name " << (aName + "." + key);
                            qCritical() << "ERROR fillData: nullptr pointer for component variable name " << (aName + "." + key);
                            qCritical() << "This variable is required by component model as it has been declared in SubModel::declareModelParameters !";
                            qCritical() << "REMEDY : Create the corresponding variable in this component and publish it using component::mCompoToModel->publishData !";
                            return -1;
                        }                        
                    }
                }
            }
        }
    }
    return 0;
}

void InputParam::jsonSaveGUIInputParam(QJsonArray& paramArray)
{
    QJsonObject paramObject;
    for (auto const& [key, val] : mMapParams) {
        if (val) {
            paramObject.insert("key", QJsonValue::fromVariant(key));
            switch (val->getType()) {
                case eDouble:
                    paramObject.insert("value", QJsonValue::fromVariant(*std::get<eDouble>(val->getPtr())));
                    break;
                case eInt:
                    paramObject.insert("value", QJsonValue::fromVariant(*std::get<eInt>(val->getPtr())));
                    break;
                case eBool:
                    if (*std::get<eBool>(val->getPtr()))
                        paramObject.insert("value", QJsonValue::fromVariant(true));
                    else
                        paramObject.insert("value", QJsonValue::fromVariant(false));
                    break;
                case eString:
                    paramObject.insert("value", QJsonValue::fromVariant(*std::get<eString>(val->getPtr())));
                    break;
                case eStringList: {
                    paramObject.insert("value", QJsonValue::fromVariant(*(QStringList*)(std::get<eStringList>(val->getPtr()))));
                    break;
                }
                default:
                    paramObject.insert("value", QJsonValue::fromVariant(val->toString()));
                    break;
            }
            paramArray.push_back(paramObject);
        }
    }
}

void InputParam::getParameters(QList<QString>& a_List, const EParamType& a_Type)
{
    for (auto const& [key, val] : mMapParams) {
        if (val) {
            if (val->getType() == a_Type)
                a_List.push_back(key);
        }
    }
}

void InputParam::getParameters(QList<ModelParam*>& a_List, const EParamType& a_Type)
{
    for (auto const& [key, val] : mMapParams) {
        if (val) {
            if (val->getType() == a_Type)
                a_List.push_back(val);
        }
    }
}

InputParam::ModelParam* InputParam::getParameter(const QString& aName)
{
    ModelParam* vRet = nullptr;
    t_mapParams::iterator vIter = mMapParams.find(aName);
    if (vIter != mMapParams.end()) {
        return vIter->second;        
    }
    return vRet;
}

void InputParam::getParameters(QList<QString> &a_List, CairnAPI::ESettingsLimited a_setLimited)
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

bool InputParam::getParameterValue(const QString& a_SettingsName, QString &a_Value, const EParamType& a_Type)
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

bool InputParam::getParameterValue(const QString& a_SettingsName, t_value &a_Value)
{    
    t_mapParams::iterator vIter = mMapParams.find(a_SettingsName);
    if (vIter != mMapParams.end()) {
        if (vIter->second) {
            a_Value = vIter->second->getValue();
            return true;
        }
    }    
    return false;
}

bool InputParam::setParameterValue(const QString& a_SettingsName, const QString& a_SettingsValue)
{    
    t_mapParams::iterator vIter = mMapParams.find(a_SettingsName);
    if (vIter != mMapParams.end()) {
        if (vIter->second) {
            return vIter->second->setValue(a_SettingsValue);            
        }
    }
    return false;
}

bool InputParam::setParameterValue(const QString& a_SettingsName, const t_value& a_SettingsValue)
{
    t_mapParams::iterator vIter = mMapParams.find(a_SettingsName);
    if (vIter != mMapParams.end()) {
        if (vIter->second) {
            return vIter->second->setValue(a_SettingsValue);
        }
    }
    return false;
}

int InputParam::readParameters(const QMap<QString, QString>& aSettings)
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

int InputParam::readParameters(const QString& aName, const QSettings& aSettings)
{
    for (auto const& [key, val] : mMapParams) {
        if (val) {
            if (!val->readParameter(aName, aSettings))
                return -1;
        }
        else
            return -1;
    }
    return 0;
}


/*****************************************************************************************************/

InputParam::ModelParam::ModelParam(const QString& a_Name, t_flag a_IsBlocking, t_flag a_IsUsed, const QString& a_Comment, const QString& a_Unit)
{
    m_Type = EParamType::eUndefined;
    m_Name = a_Name;
    m_Comment = a_Comment;
    m_Quantities.push_back(a_Unit);
    m_IsBlocking.set_Value(a_IsBlocking);
    m_IsUsed.set_Value(a_IsUsed);        
    m_default = std::nan("1");
    m_min = std::nan("1");
    m_max = std::nan("1");

    if (IsBlocking() && !IsUsed()) {
        qWarning() << "Parameter " + a_Name + " is mandatory but marked as not used! It would be good to review the flags of this parameter!";
    }

    CairnUtils::printInfoParam(m_Name, IsBlocking(), a_Unit, m_Comment);
}

InputParam::ModelParam::ModelParam(const QString& a_Name, const t_pvalue &ap_Value, t_value a_defaultValue, t_flag a_IsBlocking, t_flag a_IsUsed, const QString& a_Comment, const QString& a_Unit)
    : ModelParam(a_Name, a_IsBlocking, a_IsUsed, a_Comment, a_Unit)
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
    else if (std::get_if<QString*>(&ap_Value)) {
        m_Type = EParamType::eString;              
    }
    else if (std::get_if<QStringList*>(&ap_Value)) {
        m_Type = EParamType::eStringList;
    }
    else if (std::get_if<std::vector<double>*>(&ap_Value)) {
        m_Type = EParamType::eVectorDouble;
    }
    else {
        // erreur?
        qCritical() << "Bad type for the parameter " << m_Name;
    }
    setValue(m_default);  
}

InputParam::ModelParam::ModelParam(const QString& a_Name, std::vector<double>* ap_Value, double a_default, double a_min, double a_max, t_flag a_IsBlocking, t_flag a_IsUsed, const QString& a_Comment, const QString& a_Unit)
    : ModelParam(a_Name, a_IsBlocking, a_IsUsed, a_Comment, a_Unit)
{
    m_Type = EParamType::eVectorDouble;
    p_Value = ap_Value;

    // cas particulier des timeseries, paramètres default, min, max de type double
    m_default = a_default;
    m_min = a_min;
    m_max = a_max;
}


//scalars
InputParam::ModelParam::ModelParam(const QString& a_Name, bool* ap_Value, bool a_defaultValue, t_flag a_IsBlocking, t_flag a_IsUsed, const QString& a_Comment)
    : ModelParam(a_Name, a_IsBlocking, a_IsUsed, a_Comment)
{
    m_Type = EParamType::eBool;
    p_Value = ap_Value;
    m_default = (a_defaultValue) ? 1 : 0; //t_value cannot be bool ! use int
    setValue(QString::number(a_defaultValue));
}

InputParam::ModelParam::ModelParam(const QString& a_Name, QString* ap_Value, QString a_defaultValue, t_flag a_IsBlocking, t_flag a_IsUsed,
    const QString& a_Comment, const QString& a_Unit)
    : ModelParam(a_Name, a_IsBlocking, a_IsUsed, a_Comment)
{
    m_Type = EParamType::eString;
    p_Value = ap_Value;
    m_default = a_defaultValue.toStdString();
    setValue(a_defaultValue);
}

InputParam::ModelParam::ModelParam(const QString& a_Name, double* ap_Value, double a_defaultValue,
    t_flag a_IsBlocking, t_flag a_IsUsed,
    const QString& a_Comment, const QString& a_Unit, double a_min, double a_max)
    : ModelParam(a_Name, a_IsBlocking, a_IsUsed, a_Comment, a_Unit)
{
    m_Type = EParamType::eDouble;
    m_default = a_defaultValue;
    m_max = a_max;
    m_min = a_min;
    p_Value = ap_Value;
    setValue(QString::number(a_defaultValue));
}

InputParam::ModelParam::ModelParam(const QString& a_Name, int* ap_Value, int a_defaultValue, t_flag a_IsBlocking, t_flag a_IsUsed,
    const QString& a_Comment, const QString& a_Unit, int a_min, int a_max)
    : ModelParam(a_Name, a_IsBlocking, a_IsUsed, a_Comment, a_Unit)
{
    m_Type = EParamType::eInt;
    m_default = a_defaultValue;
    m_max = a_max;
    m_min = a_min;
    p_Value = ap_Value;
    setValue(QString::number(a_defaultValue));
}
//vectors
InputParam::ModelParam::ModelParam(const QString& a_Name, QStringList* ap_Value, t_flag a_IsBlocking, t_flag a_IsUsed, const QString& a_Comment)
    : ModelParam(a_Name, a_IsBlocking, a_IsUsed, a_Comment)
{
    m_Type = EParamType::eStringList;
    p_Value = ap_Value;
    m_default = std::vector<std::string> {}; 
}

InputParam::ModelParam::ModelParam(const QString& a_Name, std::vector<double>* ap_Value, t_flag a_IsBlocking, t_flag a_IsUsed, const QString& a_Comment, const QString& a_Unit)
    : ModelParam(a_Name, a_IsBlocking, a_IsUsed, a_Comment, a_Unit)
{
    m_Type = EParamType::eVectorDouble;    
    p_Value = ap_Value;
}

InputParam::ModelParam::ModelParam(const QString& a_Name, std::vector<double>* ap_Value, t_flag a_IsBlocking, t_flag a_IsUsed, const QString& a_Comment, const QList<QString>& a_Quantities)
    : ModelParam(a_Name, a_IsBlocking, a_IsUsed, a_Comment)
{
    m_Type = EParamType::eVectorDouble;
    p_Value = ap_Value;
    m_Quantities.clear();
    for (auto& vQuantity : a_Quantities) m_Quantities.push_back(vQuantity);
}

InputParam::ModelParam::ModelParam(const QString& a_Name, std::vector<int>* ap_Value, t_flag a_IsBlocking, t_flag a_IsUsed, const QString& a_Comment, const QString& a_Unit)
    : ModelParam(a_Name, a_IsBlocking, a_IsUsed, a_Comment, a_Unit)
{
    m_Type = EParamType::eVectorInt;
    p_Value = ap_Value;
}


/*****************************************************************************************************/
//special params used to tranfer data from MilpComponent to SubModel
InputParam::ModelParam::ModelParam(const QString& a_Name, bool* ap_Value)
    : ModelParam(a_Name)
{
    m_Type = EParamType::eBool;
    p_Value = ap_Value;
}
InputParam::ModelParam::ModelParam(const QString& a_Name, QString* ap_Value)
    : ModelParam(a_Name)
{
    m_Type = EParamType::eString;
    p_Value = ap_Value;
}
InputParam::ModelParam::ModelParam(const QString& a_Name, int* ap_Value)
    : ModelParam(a_Name)
{
    m_Type = EParamType::eInt;
    p_Value = ap_Value;
}
InputParam::ModelParam::ModelParam(const QString& a_Name, double* ap_Value)
    : ModelParam(a_Name)
{
    m_Type = EParamType::eDouble;
    p_Value = ap_Value;
}
InputParam::ModelParam::ModelParam(const QString& a_Name, Eigen::VectorXf* ap_Value)
    : ModelParam(a_Name)
{
    m_Type = EParamType::eVectorEigen;   
    p_Value = ap_Value;
}

InputParam::ModelParam::ModelParam(const QString& a_Name, int aSize, double aDefault)
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

bool InputParam::ModelParam::readParameter(const QString& aName, const QSettings& aSettings)
{
    if (isPValue()) {
        QVariant lu = GS::getVariantSetting(aName + "." + m_Name, aSettings);
        if (lu != QVariant()) {
            readParam(aName + "." + m_Name, lu);
            qDebug().noquote() << "- Read parameter : " << (aName + "." + m_Name) << " = " << toString();
        }        
        else {
            if (IsBlocking()) {
                qCritical() << "ERROR readParameters: missing value for parameter " << (m_Name);
                return false;
            }
            else {
                CairnUtils::printMissingParam(aName+"."+m_Name, toString());
            }
        }
    }
    else {
        qCritical() << "ERROR readParameters: nullptr pointer for component variable name " << (m_Name);
        return false;
    }
    return true;
}

bool InputParam::ModelParam::readParameter(const QMap<QString, QString>& aSettings) 
{   
    if (isPValue()) {
        if (aSettings.contains(m_Name)) {
            readParam(m_Name, aSettings);            
        }
        else {
            if (IsBlocking()) {
                qCritical() << "ERROR readParameters: missing value for parameter " << (m_Name);
                return false;
            }
            else {
                CairnUtils::printMissingParam(m_Name, toString());
            }
        }
    }
    else {
        qCritical() << "ERROR readParameters: nullptr pointer for component variable name " << (m_Name);
        return false;
    }
    return true;
}

InputParam::ModelParam::FlagParam::FlagParam()
{
    m_Value = false;
    p_Value = nullptr;
    m_Function.Type = eFTypeUndefined;
}

void InputParam::ModelParam::FlagParam::set_Value(t_flag a_Flag)
{    
    if (const bool* pval = std::get_if<bool>(&a_Flag)) {
        m_Value = *pval;
    }
    else if (bool** pval = std::get_if<bool*>(&a_Flag)) {
        p_Value = *pval;
    }
    else if (SFunctionFlag* pval = std::get_if<SFunctionFlag>(&a_Flag)) {
        m_Function.Type = pval->Type;
        m_Function.Flags.assign(pval->Flags.begin(), pval->Flags.end());
        m_Function.Flags2.assign(pval->Flags2.begin(), pval->Flags2.end());
        m_Function.ExtFunct = pval->ExtFunct;
    }
    else if (SExtFunctionFlag* pval = std::get_if<SExtFunctionFlag>(&a_Flag)) {
        m_ExtFunct.pFunct = pval->pFunct;
        m_ExtFunct.pModel = pval->pModel;
    }
}

bool InputParam::ModelParam::FlagParam::get_Value()
{
    if (m_ExtFunct.pModel && m_ExtFunct.pFunct) {
        return (*m_ExtFunct.pFunct)(m_ExtFunct.pModel);
    }
    else if (m_Function.Type == eFTypeUndefined) {
        if (p_Value)
            return *p_Value;
        else
            return m_Value;
    }
    else {
        bool vRet = false;
        switch (m_Function.Type)
        {
        case eFTypeOrNot:
            for (auto& vFlag : m_Function.Flags) {
                vRet |= *vFlag;
            }
            for (auto& vFlag : m_Function.Flags2) {
                vRet |= !*vFlag;
            }            
            break;
        case eFTypeNotAnd:
            vRet = true;
            for (auto& vFlag : m_Function.Flags) {
                vRet &= !*vFlag;
            }
            for (auto& vFlag : m_Function.Flags2) {
                vRet &= *vFlag;
            }            
            break;    
        case eFTypeNotAndOr:            
            for (auto& vFlag : m_Function.Flags2) {
                vRet |= *vFlag;
            }        
            for (auto& vFlag : m_Function.Flags) {
                vRet &= !*vFlag;
            }   
            break;
        default:
            break;
        }
        if (m_Function.ExtFunct.pFunct && m_Function.ExtFunct.pModel) {
            vRet &= (*m_Function.ExtFunct.pFunct)(m_Function.ExtFunct.pModel);
        }
        return vRet;
    }
}

bool InputParam::ModelParam::IsBlocking() 
{ 
    return m_IsBlocking.get_Value(); 
};

bool InputParam::ModelParam::IsUsed() 
{ 
    return m_IsUsed.get_Value(); 
};

TriState InputParam::ModelParam::isModified() {
    if (m_Type == eVectorDouble || m_Type == eVectorInt) {
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
    case eVectorInt:
        vRet = (std::get<eVectorInt>(p_Value) != nullptr);
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
        case eVectorInt:
        {
            std::vector<int>* pValue = (std::vector<int>*)(std::get<eVectorInt>(p_Value));
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
    case eVectorInt:
        vRet = (std::vector<int>)(*std::get<eVectorInt>(p_Value));
        break;        
    case eString: 
        {
            QString* pValue = (QString*)(std::get<eString>(p_Value));
            vRet = pValue->toStdString();
            break;
        } 
    case eStringList:
        {
            QStringList* pValue = (QStringList*)(std::get<eStringList>(p_Value));
            std::vector<std::string> pValue_list;
            for (auto const& val : *pValue) {
                pValue_list.push_back(val.toStdString());
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

QString InputParam::ModelParam::toString()
{
    QString vRet = "";
    switch (m_Type) {
    case eDouble:
        vRet = QString::number(*std::get<eDouble>(p_Value));
        break;
    case eInt:
        vRet = QString::number(*std::get<eInt>(p_Value));
        break;
    case eBool:
        if (*std::get<eBool>(p_Value))
            vRet = "true";
        else
            vRet = "false";
        break;
    case eString:
    {
        QString* pValue = (QString*)(std::get<eString>(p_Value));
        vRet = *pValue;
        break;
    }
    case eStringList: {
        vRet = (*(QStringList*)(std::get<eStringList>(p_Value))).join(",");
        break;
    }
    default:
        break;
    }
    return vRet;
}

void InputParam::ModelParam::readParam(const QString& aParamName, const QMap<QString, QString>& a_Settings)
{    
    switch (m_Type) {
    case eDouble:
    {
        double* pValue = (double*)std::get<eDouble>(p_Value);
        *pValue = a_Settings[aParamName].toDouble();
        break;
    }              
    case eInt:
    {
        int* pValue = (int*)std::get<eInt>(p_Value);
        *pValue = a_Settings[aParamName].toInt();
        break;
    }
    case eBool:
    {
        bool* pValue = (bool*)std::get<eBool>(p_Value);
        *pValue = (a_Settings[aParamName] == "0") ? false : true;        
        break;
    }
    case eString: 
        {
            QString* pValue = std::get<eString>(p_Value);
            *pValue = a_Settings[aParamName];
            break;
        }
    case eStringList: {
        QStringList* pValue = std::get<eStringList>(p_Value);
        *pValue = QStringList({});
        bool isImpactSelected = false; //at least one impact is selected
        if (a_Settings[aParamName].simplified() != "")
        {
            QStringList stringList = a_Settings[aParamName].split(",", QString::SkipEmptyParts);
            for (size_t i = 0; i < stringList.size(); i++) {
                if (stringList[i].simplified() != "") {
                    stringList[i] = stringList[i].simplified();
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

void InputParam::ModelParam::readParam(const QString & aParamName, const QVariant & a_Setting)
{
    switch (m_Type) {
    case eDouble:
    {
        double* pValue = (double*)std::get<eDouble>(p_Value);
        *pValue = a_Setting.toDouble();
        break;
    }
    case eInt:
    {
        int* pValue = (int*)std::get<eInt>(p_Value);
        *pValue = a_Setting.toInt();
        break;
    }
    case eBool:
    {
        bool* pValue = (bool*)std::get<eBool>(p_Value);                
        *pValue = a_Setting.toBool();
        break;
    }
    case eString: 
        {
            QString* pValue = (QString*)std::get<eString>(p_Value);
            *pValue = a_Setting.toString();
            break;
        }
    case eStringList:
    {
        QStringList* pValue = (QStringList*)std::get<eStringList>(p_Value);
        pValue->clear();
        QList<QVariant> ql = a_Setting.toList();
        QListIterator<QVariant> iQL(ql);
        while (iQL.hasNext())
        {
            pValue->append(iQL.next().toString());
        }
        break;
    }             
    default:
        break;
    }
}

bool InputParam::ModelParam::setValue(const QString& a_Value)
{
    bool vRet = false;
    switch (m_Type) {
    case eDouble:
    {
        double* pValue = (double*)std::get<eDouble>(p_Value);
        bool ok = false;
        *pValue = a_Value.toDouble(&ok);
        if (!ok) {
            qWarning() << "Conversion fails for the" << m_Name << " Value is not Double" << endl;            
        }
        else {
            // in the bound ? clamp ? 
            vRet = true;
        }        
        break;        
    }
    case eInt:
    {
        int* pValue = (int*)std::get<eInt>(p_Value);
        bool ok = false;
        *pValue = a_Value.toInt(&ok);
        if (!ok) {
            qWarning() << "Conversion fails for the" << m_Name << " Value is not Integer" << endl;            
        }
        else {
            vRet = true;
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
        QString* pValue = std::get<eString>(p_Value);
        *pValue = a_Value.toUtf8().constData();
        vRet = true;
        break;
    }
    case eStringList:
    {//assumes that a_Value is a string with list elements sperated with comma
        QStringList* pValue = std::get<eStringList>(p_Value);
        QString value = a_Value.toUtf8().constData();
        *pValue = value.split(",");
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
            qWarning() << "Conversion fails for the" << m_Name << " Value is not Double" << endl;
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
            qWarning() << "Conversion fails for the" << m_Name << " Value is not Double" << endl;
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
            qWarning() << "Conversion fails for the" << m_Name << " Value is not Bool" << endl;
        }   
        break;
    }
    case eString:
    {
        QString* pValue = std::get<eString>(p_Value);
        if (const std::string* pSrc = std::get_if<std::string>(&a_Value)) {
            *pValue = QString(pSrc->c_str());
            vRet = true;
        }
        else {
            // TODO: conversion ?
            qWarning() << "Conversion fails for the" << m_Name << " Value is not Bool" << endl;
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
            qWarning() << "Conversion fails for the" << m_Name << " Value is not VectorDouble" << endl;
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

bool InputParam::ModelParam::copyValues(const QVector<float>& aSrc, size_t aOffset)
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
                (*pValue)[i] = double(aSrc[i + aOffset]);
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
            case eVectorInt:
                vRet = (std::vector<int>)(*std::get<eVectorInt>(p_Value))[i];
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

InputParam::ModelIndicator::ModelIndicator(const QString& aIndicatorName, 
    std::vector<double>* aDblePtr, bool* aBoolPtr, 
    const QString& aDesc, const QString& aUnit, const QString& aShortName)
{
    m_Name = aIndicatorName;
    m_ShortName = aShortName;
    m_Unit = aUnit;
    m_Comment = aDesc;

    p_Value = aDblePtr;
    p_IsExported = aBoolPtr;
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

void InputParam::ModelIndicator::Export(QTextStream& out, const QString &aComponentName, 
    const QString& range, bool aForceExport)
{
    if (p_Value && (IsExported() || aForceExport))
    {
        //Unit
        QString unit = QString("-");
        if (m_Unit != "") {
            unit = m_Unit;
        }
        //Published (declared) indicators        
        if (range == "PLAN") {            
            CairnUtils::outputIndicator(out, aComponentName, m_Name, (*p_Value)[0], unit, m_ShortName);
        }
        else if (range == "HIST") {            
            CairnUtils::outputIndicator(out, aComponentName, m_Name, (*p_Value)[1], unit, m_ShortName);
        }        
    }
}

void InputParam::ModelIndicator::Export(QTextStream& out, const QString& aComponentName, const QString& range, bool aForceExport, 
    bool aIsSizeOptimized, bool aIsPriceOptimized, bool isRollingHorizon, const std::vector<double>& aOptimalSizeAllCycles)
{
    if (p_Value && (IsExported() || aForceExport))    {
        //Unit
        QString unit = QString("-");
        if (m_Unit != "") {
            unit = m_Unit;
        }
        bool isOptimalSize = false;
        QString varName = m_Name;
        //Manage optim/fixed sizing        
        if (m_Name == "Installed Size") {
            if (aIsSizeOptimized) varName = "Installed Optimal Size";
            isOptimalSize = true;
        }
        if (m_Name == "Component Weight") {
            if (aIsSizeOptimized) varName = "Component Optimal Weight";
            isOptimalSize = true;
        }
        if (m_Name == "Storage Capacity") {
            if (aIsSizeOptimized) varName = "Storage Optimal Capacity";
            isOptimalSize = true;
        }
        if (aIsPriceOptimized && m_Name == "Component Optimal Price")
            isOptimalSize = true;


        //0 -> _PLAN, 1 -> _HIST
        if (range == "PLAN") {
            //Assuming that when isRollingHorizon==false, the export is to the main _PLAN.csv (it is re-written every cycle), 
            //and when isRollingHorizon==true, the export is to _PLAN_RH_cycle.csv (a file per cycle)
            if (!isRollingHorizon && isOptimalSize) {
                //Take the maximum values from all cycles
                auto it_maxValue = std::max_element(aOptimalSizeAllCycles.begin(), aOptimalSizeAllCycles.end());
                if (it_maxValue != aOptimalSizeAllCycles.end()) {
                    CairnUtils::outputIndicator(out, aComponentName, varName, *it_maxValue, unit, m_ShortName);
                }
                isOptimalSize = false;
            }
            else {
                CairnUtils::outputIndicator(out, aComponentName, varName, (*p_Value)[0], unit, m_ShortName);
            }
        }
        else if (range == "HIST") {
            CairnUtils::outputIndicator(out, aComponentName, varName, (*p_Value)[1], unit, m_ShortName);
        }
    }
}