#include "JsonDescription.h"
#include "Cairn_Exception.h"
#include "GlobalSettings.h"
#include <QFile>
#include <QDebug>

using namespace GS;

JsonDescription::JsonDescription(QObject *aParent, QString aName): QObject(aParent),
    mException(Cairn_Exception())
{
    this->setObjectName(aName);
}
JsonDescription::~JsonDescription()
{
} // ~JsonDescription()

QVector< QMap<QString,QString> > JsonDescription::parseJsonDescription(const QString &aDescFile)
{
    QJsonDocument jsonData=readJSONFile(aDescFile);
    extractDocumentData(jsonData);
    return mComponents ;
}

QJsonValue JsonDescription::getCompDataFromName(const QString &aCompName) const
{
    foreach (const QJsonValue & comp, mComponentsList)
    {
        if (comp["nodeName"].toString() == aCompName)
        {
            return comp ;
        }
    }

    return "N/A" ;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//Extract the data of ports for a given componenet 
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

QMap < QString, QMap<QString, QString> > JsonDescription::getCompoPortData(const QString compoName) const 
{
    QJsonValue comp; 
    QMap < QString, QMap<QString, QString> > ports; //contains the data of all ports of componenet compoId

    bool found = false;
    foreach(const QJsonValue & component, mComponentsList)
    {
        if (compoName == component["nodeName"].toString()) {
            comp = component;
            found = true;
            break;
        }
    }

    if (!found) {
        qWarning() << "getCompoPortData: component " + compoName + " has not been found!";
        return {}; 
    }

    qInfo() << compoName << " getCompoPortData :";

    const QJsonArray& posList = comp["nodePortsData"].toArray();
    uint i = 0;
    foreach(const QJsonValue & pos, posList)
    {
        bool isBus = false;
        if (comp["componentPERSEEType"].toString().contains("Bus") || comp["componentPERSEEType"].toString().contains("MultiObj")) {
            isBus = true;
        }
            
        QJsonArray portList = pos["ports"].toArray();
        foreach(const QJsonValue & port, portList)
        {
            i++;
            QString portname = port["name"].toString();
            QString portuse = port["direction"].toString();
            qInfo() << "\t - Insert port\t" << QString::number(i);
            qInfo() << "\t\t - name\t\t\t" << portname;
            qInfo() << "\t\t - type\t\t\t" << port["type"].toString() << port["carrier"].toString();
            qInfo() << "\t\t - use\t\t\t" << portuse;
            qInfo() << "\t\t - variable\t\t" << port["variable"].toString();
            qInfo() << "\t\t - coeff\t\t" << doubleToString(port["coeff"].toDouble());
            qInfo() << "\t\t - offset\t\t" << doubleToString(port["offset"].toDouble());
            qInfo() << "\t\t - checkunit\t\t" << port["checkunit"].toString();

            QMap<QString, QString> portMap; //contains the data of one port

            //Do not add Bus links
            bool insertPort = false;
            if (!isBus) {
                insertPort = true;
            }

            if (portuse == "") {
                qWarning() << "The direction of " + comp["nodeName"].toString() + " port " + portname + " (" + port["variable"].toString() + ")" + " is not set!";
                continue;
            }

            foreach(const QJsonValue & link, mLinksList)
            {
                // Add Port to map IF AND ONLY IF it is not a component-to-bus port that will be automatically built by PERSEE/core.
                QString identifier;
                if (link["tailNodeId"].toString() == "" || link["headNodeId"].toString() == "") {
                    //new format
                    identifier = "Name";
                }
                else {
                    //old format
                    identifier = "Id";

                }

                //supports two format : socketName and nodeName + "." + socketName
                QStringList headSocketList = (link["headSocket" + identifier].toString()).split(".");
                QStringList tailSocketList = (link["tailSocket" + identifier].toString()).split(".");

                QString headSocketName = headSocketList[headSocketList.size()-1]; 
                QString tailSocketName = tailSocketList[tailSocketList.size() - 1];

                //port is a link headSocket
                if (comp["node" + identifier].toString() == link["headNode" + identifier].toString() && portname == headSocketName)
                {
                    if(isBus && getcomponentCategoryFromId(link["tailNode" + identifier].toString()).contains("Bus")) {
                        //Bus-Bus link
                        Cairn_Exception cairn_error("Error: Bus-Bus links are not allowed : "+ link["headSocket" + identifier].toString() +" to "+ link["tailSocket" + identifier].toString(), -1);
                        throw cairn_error;
                    }
                        
                    if (insertPort)
                    {
                        portMap.insert("LinkedComponent", getNodeFromId(link["tailNode" + identifier].toString()));
                        portMap.insert("BusPortName", tailSocketName); //name of the linked bus port

                        qInfo() << "\t\t - " << comp["node" + identifier].toString() + "." + portname
                            << "==" << getNodeFromId(link["headNode" + identifier].toString()) + "." + portname
                            << " category " << getcomponentCategoryFromId(link["headNode" + identifier].toString())
                            << " connected to " << getNodeFromId(link["tailNode" + identifier].toString())
                            << " category " << getcomponentCategoryFromId(link["tailNode" + identifier].toString());

                        break;
                    }
                }

                //port is a link tailSocket
                if (comp["node" + identifier].toString() == link["tailNode" + identifier].toString() && portname == tailSocketName)
                {
                    if (isBus && getcomponentCategoryFromId(link["headNode" + identifier].toString()).contains("Bus")) {
                        //Bus-Bus link
                        Cairn_Exception cairn_error("Error: Bus-Bus links are not allowed: " + link["headSocket" + identifier].toString() +" to "+ link["tailSocket" + identifier].toString(), -1);
                        throw cairn_error;
                    }
                    
                    if (insertPort)
                    {
                        portMap.insert("LinkedComponent", getNodeFromId(link["headNode" + identifier].toString()));
                        portMap.insert("BusPortName", headSocketName); //name of the linked bus port

                        qInfo() << "\t\t - " << comp["node" + identifier].toString() + "." + portname
                            << "==" << getNodeFromId(link["headNode" + identifier].toString()) + "." + portname
                            << " category " << getcomponentCategoryFromId(link["headNode" + identifier].toString())
                            << " connected to " << getNodeFromId(link["tailNode" + identifier].toString())
                            << " category " << getcomponentCategoryFromId(link["tailNode" + identifier].toString());

                        break;
                    }
                }
            }

            if (insertPort)
            {
                QString portId = port["id"].toString();
                if (portId == "") {
                    portId = compoName + "." + portname;
                }

                QString position = port["position"].toString();
                if (position == "") {
                    position = pos["pos"].toString();
                }

                portMap.insert("CompoName", compoName);
                portMap.insert("Name", portname); 
                portMap.insert("Position", position);
                portMap.insert("IsDefaultPort", port["defaultport"].toString());
                portMap.insert("Enabled", port["enabled"].toString());
                portMap.insert("CarrierType", port["carrierType"].toString());
                portMap.insert("Carrier", port["carrier"].toString());
                portMap.insert("Direction", port["direction"].toString().toUpper());
                portMap.insert("Variable", port["variable"].toString());
                portMap.insert("Coeff", doubleToString(port["coeff"].toDouble()));
                portMap.insert("Offset", doubleToString(port["offset"].toDouble()));
                portMap.insert("CheckUnit", port["checkunit"].toString());

                //Add a port to the map
                ports.insert(portId, portMap);
            }
        }
    }
    return ports;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Loop on component array
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void JsonDescription::extractDocumentData(QJsonDocument& jsonData)
{
    QJsonObject object = jsonData.object();
    bool oldJsonFormat = false;

    //--------------------------------------------------------------------------//
    //For old study.json that has only one page 
    if (object.contains("Links")) {
        //~~~~~~~~~~~~ Get links array ~~~~~~~~~~~~~~~~~~~~
        QJsonValue links = object.value("Links");
        mLinksList = links.toArray();
    }

    if (object.contains("Components")) {
        //~~~~~~~~~~~~ component array ~~~~~~~~~~~~~~~~~~~~
        oldJsonFormat = true;
        QJsonValue components = object.value("Components");
        mComponentsList = components.toArray();
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
                //mLinksList = links.toArray();
                foreach(const QJsonValue & link, links.toArray()) {
                    mLinksList.append(link);
                }

                QJsonValue components = pageObject.value("Components");
                //mComponentsList = components.toArray();
                foreach(const QJsonValue& comp, components.toArray()) {
                    mComponentsList.append(comp);
                }
            }
        }
    }
    //--------------------------------------------------------------------------//

    foreach (const QJsonValue & comp, mComponentsList)
    {
        extractComponentData(comp);
    }

    //--------------------------------------------------------------------------//

    //DynamicIndicators are independent from pages
    if (object.contains("DynamicIndicators")) {
        //~~~~~~~~~~~~ dynamic indicators array ~~~~~~~~~~~~~~~~~~~~
        QJsonArray dynamicIndicatorsList = object.value("DynamicIndicators").toArray();
        foreach(const QJsonValue& dynIndicator, dynamicIndicatorsList)
        {
            extractIndicatorData(dynIndicator);
        }
    }

    return;
}

void JsonDescription::extractIndicatorData(const QJsonValue& indicatorJson) {
    QMap<QString, QString> indicator;
    indicator["name"] = indicatorJson["name"].toString();
    indicator["formula"] = indicatorJson["formula"].toString();
    mDynamicIndicators.push_back(indicator);
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Extract array items by key and print value
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void JsonDescription::extractComponentData(const QJsonValue &comp)
{
    QMap<QString, QString> component;

    component["type"] = comp["componentPERSEEType"].toString();
    component["id"] = comp["nodeName"].toString();
    component["nodeId"] = comp["nodeId"].toString();
    component["Model"] = comp["nodeType"].toString();
    component["ModelTechnoType"] = comp["nodeTechnoType"].toString();  //for EnergyVector
    component["componentCarrier"] = comp["componentCarrier"].toString();

    component["Xpos"] = QString::number(comp["x"].toInt());
    component["Ypos"] = QString::number(comp["y"].toInt());

    if (comp["nodeType"].toString() == "SimulationControl") {
        component["type"] = "SimulationControl";
    }

    qInfo() << "\n >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> nodeName : " << comp["nodeName"].toString();
    qInfo() << "\t - nodeType \t\t" << comp["nodeType"].toString();
    qInfo() << "\t - nodeId  \t\t" << comp["nodeId"].toString();
    qInfo() << "\t - nodeTechnoType \t\t" << comp["nodeTechnoType"].toString();
    qInfo() << "\t - type \t\t" << comp["componentPERSEEType"].toString();
    qInfo() << "\t - componentCarrier \t\t" << comp["componentCarrier"].toString();

    if (component["type"] == "SimulationControl") {
        component["Model"] = comp["nodeTechnoType"].toString();
        component["id"] = "Cairn";
    }
    if (component["type"] == "EnergyVector") {
        component["EnergyColor"] = comp["energyTypeColor"].toString();
    }

    component["VectorName"] = comp["componentCarrier"].toString(); //only needed for Bus components

    extractOptionData(comp["optionListJson"].toArray(), component);
    extractParamData(comp["paramListJson"].toArray(), component);
    extractParamData(comp["envImpactsListJson"].toArray(), component);
    extractParamData(comp["portImpactsListJson"].toArray(), component);
    extractTimeSeriesData(comp["timeSeriesListJson"].toArray(), component);

    upwardCompatibility(component);

    mComponents.push_back(component);
}


QString JsonDescription::getNodeFromId(const QString nodeId) const
{
    foreach (const QJsonValue & comp, mComponentsList)
    {
        if (comp["nodeId"] == nodeId) return comp["nodeName"].toString() ;
        if (comp["nodeName"] == nodeId) return comp["nodeName"].toString();

    }
    return QString("nodeId_Not_Found_")+nodeId;
}
QString JsonDescription::getcomponentCategoryFromId(const QString nodeId) const
{
    foreach (const QJsonValue & comp, mComponentsList)
    {
        if (comp["nodeId"] == nodeId || comp["nodeName"] == nodeId)
        {
            if (comp["componentPERSEEType"].toString().contains("MultiObjCompo")) 
                return "BusMultiObjCompo"; // to comply with JsonDescription treatment 
            else
                return comp["componentPERSEEType"].toString();
        }
    }
    return QString("nodeId_Not_Found_")+nodeId;
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// First loop on options array
// Extract array items and print key + value
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void JsonDescription::extractOptionData(const QJsonArray &optionList, QMap<QString, QString>& aMap)
{
    qInfo() << "\t - Options :";
    foreach (const QJsonValue & o, optionList)
    {
        if (o["key"].toString() == "Xpos" || o["key"].toString() == "Ypos")
            continue;
        //if (o["key"].toString() == "Model") //take "Model" fron "nodeType"
        //    continue; //Needed for Solver !!!
        switch (o["value"].type())
        {
        case QJsonValue::Double :
            qInfo() << "\t\t - " << o["key"].toString() << doubleToString(o["value"].toDouble());
            aMap.insert(o["key"].toString(), doubleToString(o["value"].toDouble()));
            break;
        case QJsonValue::Bool :
            qInfo() << "\t\t - " << o["key"].toString() << o["value"].toBool();
            aMap.insert(o["key"].toString(), QString::number(o["value"].toBool()));
            break;
        case QJsonValue::String :
            qInfo() << "\t\t - " << o["key"].toString() << o["value"].toString();
            aMap.insert(o["key"].toString(), o["value"].toString());			
            break;
        case QJsonValue::Array :
        case QJsonValue::Object :
        case QJsonValue::Undefined :
        case QJsonValue::Null :
        default :
            qWarning() << "\t\t !!!!!!!!!!!!!!!!! UNKOWN OPTION TYPE - OPTION IGNORED - " << o["key"].toString() << o["value"];
            break;
        }
    }
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// First loop on parameters array
// Extract array items and print key + value
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void JsonDescription::extractParamData(const QJsonArray &paramList, QMap<QString, QString>& aMap)
{
    qInfo() << "\t - Parameters :";
    foreach (const QJsonValue & p, paramList)
    {
        QJsonDocument doc;
        QString val; 
        switch (p["value"].type())
        {
        case QJsonValue::Double :
            qInfo() << "\t\t - " << p["key"].toString() << doubleToString(p["value"].toDouble());
            aMap.insert(p["key"].toString(), doubleToString(p["value"].toDouble()));
            break;
        case QJsonValue::Bool :
            qInfo() << "\t\t - " << p["key"].toString() << p["value"].toBool();
            aMap.insert(p["key"].toString(), QString::number(p["value"].toBool()));
            break;
        case QJsonValue::String :
            qInfo() << "\t\t - " << p["key"].toString() << p["value"].toString();
            aMap.insert(p["key"].toString(), p["value"].toString());
            break;
        case QJsonValue::Array :
            doc.setArray(p["value"].toArray());
            val = QString(doc.toJson()).simplified().remove('"').remove('[').remove(']');
            qInfo() << "\t\t - " << p["key"].toString() << QString(doc.toJson());
            aMap.insert(p["key"].toString(), val);
            break;
        case QJsonValue::Object :
        case QJsonValue::Undefined :
        case QJsonValue::Null :
        default :
            qWarning() << "\t\t !!!!!!!!!!!!!!!!! UNKOWN PARAMETER TYPE - PARAMETER IGNORED - " << p["key"].toString() << p["value"];
            break;
        }
    }
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// First loop on timeSeries array
// Extract array items and print key + value
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void JsonDescription::extractTimeSeriesData(const QJsonArray &timeSeriesList, QMap<QString, QString>& aMap)
{
qInfo() << "\t - Time Series :";
foreach (const QJsonValue & p, timeSeriesList)
{
    switch (p["value"].type())
    {
    //Can a timeseries be a double or bool?! :)
    case QJsonValue::Double :
        qInfo() << "\t\t - " << p["key"].toString() << doubleToString(p["value"].toDouble());
        aMap.insert(p["key"].toString(), doubleToString(p["value"].toDouble()));
        break;
    case QJsonValue::Bool :
        qInfo() << "\t\t - " << p["key"].toString() << p["value"].toBool();
        aMap.insert(p["key"].toString(), QString::number(p["value"].toBool()));
        break;
    case QJsonValue::String :
        qInfo() << "\t\t - " << p["key"].toString() << p["value"].toString();
        aMap.insert(p["key"].toString(), p["value"].toString());
        break;
    case QJsonValue::Array :
    case QJsonValue::Object :
    case QJsonValue::Undefined :
    case QJsonValue::Null :
    default :
        qWarning() << "\t\t !!!!!!!!!!!!!!!!! UNKOWN Time Series TYPE - PARAMETER IGNORED - " << p["key"].toString() << p["value"];
        break;
    }
}
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Read the json formatted file
// return the JsonDocument
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
QJsonDocument JsonDescription::readJSONFile(QString aFileName)
{
QFile loadFile(aFileName);
if (! loadFile.open(QIODevice::ReadOnly)) {
    QString ErrorMSG("Couldn't open read file: "+aFileName);
    ErrorMSG.append(loadFile.fileName());
    ErrorMSG.append(".\n");
    qCritical("%s",ErrorMSG.toStdString().c_str());
}

QByteArray readData = loadFile.readAll();
QJsonParseError aParseError;

/// If parsing fails, the returned document will be null,
/// and the optional error variable will contain further details about the error.
QJsonDocument document(QJsonDocument::fromJson(readData, &aParseError));

if (document.isNull()){

    QString aFaultyString;
    // To print 14 characters centered on the aParseError.offset
    int aDeb(aParseError.offset-7), aFin(aParseError.offset+7);

    aDeb = (aDeb < 0) ? 0 : aDeb;
    aFin = (aFin > readData.size() - 1) ? readData.size() - 1 : aFin;
    for (int i = aDeb; i < aFin; i++){
        aFaultyString.append(readData.at(i));
    }

    QString ErrorMSG("The ");
    ErrorMSG.append(loadFile.fileName());
    ErrorMSG.append(" cannot be parsed without errors.\n");
    ErrorMSG.append("The generated error is: ");
    ErrorMSG.append(aParseError.errorString());
    ErrorMSG.append(".\n");
    ErrorMSG.append("The error occured at offset: ");
    ErrorMSG.append(QString("%1").arg(aParseError.offset));
    ErrorMSG.append(".\n");
    ErrorMSG.append("The faulty string is: ");
    ErrorMSG.append(aFaultyString);
    qCritical("%s",ErrorMSG.toStdString().c_str());
}

loadFile.close();
return document;
}
