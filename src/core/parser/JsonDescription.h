#ifndef JsonDescription_H
#define JsonDescription_H
class JsonDescription ;

#include <QXmlStreamReader>
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
