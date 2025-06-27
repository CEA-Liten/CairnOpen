#ifndef JsonDescription_H
#define JsonDescription_H
class JsonDescription ;

#include <QXmlStreamReader>
#include <QSettings>

#include "MILPComponent_global.h"
#include "Cairn_Exception.h"

#define DECIMAL_PRECISION 9
#define SIGNIFICANT_DIGITS 12

static QString doubleToString(const double& value_d) {
    int precision = int(DECIMAL_PRECISION);
    int sig_digits = int(SIGNIFICANT_DIGITS);
    double value_f = QString::number(value_d, 'f', precision).toDouble();
    QString value_s = QString::number(value_f, 'g', sig_digits);
    return value_s;
}

static void upwardCompatibility(QMap<QString, QString>& component)
{
    //Attention the Option "Model" overwrites the component["Model"] == nodeType !!

    //Perform some conversions 'by hand' for upward compatibility
    if (component["Model"] == "Storage") {
        component["Model"] = "StorageGen";
    }

    if (component["Model"] == "Source" || component["Model"] == "Load"
        || component["Model"] == "PVSource" || component["Model"] == "WindSource"
        || component["Model"] == "CSPSource" || component["Model"] == "SolarHeater")
    {
        component["Model"] = "SourceLoad";
    }

    if (component["Model"] == "Grid") {
        component["Model"] = "GridFree";
    }

    if (component["ModelClass"] == "" && component["Model"] != "")
    {
        component["ModelClass"] = component["Model"];
    }
    qInfo() << "\t - Model \t\t" << component["Model"] << "\t - ModelClass \t\t" << component["ModelClass"];
}


static void parseJsonObject(QJsonObject& json, QString prefix, QMap<QString, QVariant>& map)
{
    QJsonValue value;
    QJsonObject obj;

    QStringList keys = json.keys();
    for (int i = 0; i < keys.size(); i++)
    {
        value = json.value(keys[i]);
        if (value.isObject())
        {
            obj = value.toObject();
            qDebug() << " parsing node " << prefix << keys[i];
            //parseJsonObject(obj, prefix + keys[i] + ".", map);
            if (keys[i].contains("Components"))
                parseJsonObject(obj, QString(), map);
            else if (keys[i].contains("paramListJson"))
                parseJsonObject(obj, prefix + keys[i] + ".", map);
        }
        else
        {
            if (keys[i].contains("nodeName"))
                map.insert(prefix + "." + keys[i], value.toVariant());
            qDebug() << " ending node " << prefix + keys[i];
        }
    }
}

static QJsonObject restoreJsonObject(QMap<QString, QVariant>& map)
{
    QJsonObject obj;
    QStringList keys = map.keys();

    for (int i = 0; i < keys.size(); i++)
    {
        QString key = keys.at(i);
        QVariant value = map.value(key);
        QStringList sections = key.split('/');
        if (sections.size() > 1)
        {
            continue;
        }
        else
        {
            map.remove(key);
            obj.insert(key, QJsonValue::fromVariant(value));
        }
    }

    QList<QMap<QString, QVariant>> subMaps;
    keys = map.keys();
    for (int i = 0; i < keys.size(); i++)
    {
        bool found = false;
        QString key = keys[i];

        for (int j = 0; j < subMaps.size(); j++)
        {
            QString subKey = subMaps[j].key(QString("__key__"));
            if (subKey.contains(key.section('/', 0, 0)))
            {
                subMaps[j].insert(key.section('/', 1), map.value(key));
                found = true;
                break;
            }
        }

        if (!found)
        {
            QMap<QString, QVariant> tmp;
            tmp.insert(key.section('/', 0, 0), QString("__key__"));
            tmp.insert(key.section('/', 1), map.value(key));
            subMaps.append(tmp);
        }
    }

    for (int i = 0; i < subMaps.size(); i++)
    {
        QString key = subMaps[i].key(QString("__key__"));
        subMaps[i].remove(key);

        QJsonObject tmp = restoreJsonObject(subMaps[i]);
        obj.insert(key, tmp);
    }
    return obj;
}
static void parseParamData(const QJsonArray& paramList, QMap<QString, QString>& aMap, QSettings::SettingsMap& aSettingsMap)
{//aMap["id"] + "." is added to filter by component in  QSettings. Not needed in JsonDescription::extractParamData
    foreach(const QJsonValue & p, paramList)
    {
        switch (p["value"].type())
        {
        case QJsonValue::Double:
            qInfo() << "\t\t - " << aMap["id"] << "." << p["key"].toString() << doubleToString(p["value"].toDouble());
            aSettingsMap.insert(aMap["id"] + "." + p["key"].toString(), doubleToString(p["value"].toDouble()));
            break;
        case QJsonValue::Bool:
            qInfo() << "\t\t - " << aMap["id"] << "." << p["key"].toString() << p["value"].toBool();
            aSettingsMap.insert(aMap["id"] + "." + p["key"].toString(), QString::number(p["value"].toBool()));
            break;
        case QJsonValue::String:
            qInfo() << "\t\t - " << aMap["id"] << "." << p["key"].toString() << p["value"].toString();
            aSettingsMap.insert(aMap["id"] + "." + p["key"].toString(), p["value"].toString());
            break;
        case QJsonValue::Array:
            qInfo() << "\t\t - " << p["key"].toString() << p["value"].toArray();
            aSettingsMap.insert(aMap["id"] + "." + p["key"].toString(), p["value"].toArray());
            break;
        case QJsonValue::Object:
        case QJsonValue::Undefined:
        case QJsonValue::Null:
        default:
            qWarning() << "\t\t !!!!!!!!!!!!!!!!! UNKOWN PARAMETER TYPE - PARAMETER IGNORED - " << p["key"].toString() << p["value"];
            break;
        }
    }
}
static void parseComponentData(const QJsonValue& comp, QSettings::SettingsMap &aSettingsMap)
{
    QMap<QString, QString> component;
    component["id"] = comp["nodeName"].toString();
    if (comp["nodeType"].toString() == "SimulationControl") {
        aSettingsMap.insert("SimulationControlName", component["id"]);
    }

    parseParamData(comp["paramListJson"].toArray(), component, aSettingsMap);
    parseParamData(comp["envImpactsListJson"].toArray(), component, aSettingsMap);
    parseParamData(comp["portImpactsListJson"].toArray(), component, aSettingsMap);

}

static bool readSettingsJson(QIODevice& device, QSettings::SettingsMap& aSettingsMap)
{
    QJsonParseError jsonError;
    qInfo() << "Youpi readSettingsJson ! ";
    QJsonObject object = QJsonDocument::fromJson(device.readAll(), &jsonError).object();
    if (jsonError.error != QJsonParseError::NoError)
        return false;

    bool oldJsonFormat = false;
    QJsonArray LinksList;
    QJsonArray ComponentsList;

    //--------------------------------------------------------------------------//
    //For old study.json that has only one page
    if (object.contains("Links")) {
        //~~~~~~~~~~~~ Get links array ~~~~~~~~~~~~~~~~~~~~
        QJsonValue links = object.value("Links");
        LinksList = links.toArray();
    }
    if (object.contains("Components")) {
        oldJsonFormat = true;
        //~~~~~~~~~~~~ component array ~~~~~~~~~~~~~~~~~~~~
        QJsonValue components = object.value("Components");
        ComponentsList = components.toArray();
    }
    //--------------------------------------------------------------------------//
    // 
    //--------------------------------------------------------------------------//
    //For new study.json that may have multiple pages
    if (!oldJsonFormat) {
        //read pages
        foreach(const QString & key, object.keys()) {
            if (key.contains("Page") && key != "numberPages") {
                QJsonObject pageObject = object.value(key).toObject();

                QJsonValue links = pageObject.value("Links");
                //LinksList = links.toArray();
                foreach(const QJsonValue & link, links.toArray()) {
                    LinksList.append(link);
                }

                QJsonValue components = pageObject.value("Components");
                //ComponentsList = components.toArray();
                foreach(const QJsonValue & comp, components.toArray()) {
                    ComponentsList.append(comp);
                }
            }
        }
    }
    //--------------------------------------------------------------------------//

    foreach(const QJsonValue & comp, ComponentsList)
    {
        parseComponentData(comp, aSettingsMap);
    }
    return true;
}

static bool writeSettingsJson(QIODevice& device, const QMap<QString, QVariant>& map)
{
    QMap<QString, QVariant> tmp_map = map;
    QJsonObject buffer = restoreJsonObject(tmp_map);
    device.write(QJsonDocument(buffer).toJson());
    return true;
}

class CAIRNCORESHARED_EXPORT JsonDescription : public QObject
{
    Q_OBJECT
public:
    JsonDescription(QObject *aParent, QString aName);
    virtual ~JsonDescription();

    QVector< QMap<QString,QString> > parseJsonDescription(const QString &aDescFile) ;
    void extractComponentData(const QJsonValue& comp);
    void extractIndicatorData(const QJsonValue& indicatorJson);
    void extractDocumentData(QJsonDocument& jsonData) ;
    //void extractPortData(const QJsonArray &posList, const QJsonValue &comp, QMap<QString, QString> &aMap) ;
    void extractOptionData(const QJsonArray &optionList, QMap<QString, QString> &aMap) ;
    void extractParamData(const QJsonArray &paramList, QMap<QString, QString> &aMap) ;
    void extractTimeSeriesData(const QJsonArray &timeSeriesList, QMap<QString, QString> &aMap) ;

    QString getNodeFromId(const QString nodeId) const;
    QString getcomponentCategoryFromId(const QString nodeId) const;
    QJsonValue getCompDataFromName(const QString& aCompName) const;
    QMap < QString, QMap<QString, QString> > getCompoPortData(const QString compoName) const;
    const QVector< QMap<QString, QString> > dynamicIndicators() { return mDynamicIndicators; }

    QJsonDocument readJSONFile(QString aFileName) ;
    QVector< QMap<QString,QString> > parseXmlJsonDescription(QString aDescFile) ;

    Cairn_Exception  getException () const {return mException;}
    void  setException (const Cairn_Exception &aException) {mException = aException;}


protected:
    Cairn_Exception mException ;

    QJsonArray mComponentsList;
    QJsonArray mLinksList;

    QVector< QMap<QString,QString> > mComponents;
    QVector< QMap<QString, QString> > mDynamicIndicators;
};

#endif // JsonDescription_H
