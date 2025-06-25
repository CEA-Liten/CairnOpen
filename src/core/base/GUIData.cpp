#include <QtCore>
#include "qdom.h"
#include "base/GUIData.h"
#include "GlobalSettings.h"
#include <QDebug>

GUIData::GUIData(QObject *aParent, QString aName) : QObject(aParent),
    mGuiNodeName(aName),
    mXpos(0),
    mYpos(0)
{
    this->setObjectName(aName);
    mGuiNodeModelType = "" ;
    mGuiComponentCairnType = "componentPERSEEType" ;
    mId = GS::GenerateID() ;
}  

GUIData::~GUIData()
{
} 

void GUIData::doInit(const QString aType, const QString aSubType, const QString aCategory, const int aXpos, const int aYpos)
{
    mGuiNodeModelType = aType ;
    mGuiNodeTechnoType = aSubType ;
    mGuiComponentCairnType = aCategory ;

    setXpos(aXpos);
    setYpos(aYpos);
}
void GUIData::jsonSaveGUILine(QJsonObject& componentObject, const QString& componentCarrier)
{
    QString nodeID = QString::number(mId+1);
    if (mGuiNodeTechnoType == "") mGuiNodeTechnoType = mGuiNodeModelType;

    componentObject.insert("nodeId", QJsonValue::fromVariant(nodeID));
    componentObject.insert("nodeName", QJsonValue::fromVariant(mGuiNodeName));
    componentObject.insert("componentPERSEEType", QJsonValue::fromVariant(mGuiComponentCairnType));
    componentObject.insert("nodeType", QJsonValue::fromVariant(mGuiNodeModelType));
    componentObject.insert("nodeTechnoType", QJsonValue::fromVariant(mGuiNodeTechnoType));
    if (componentCarrier != "") {
        componentObject.insert("componentCarrier", QJsonValue::fromVariant(componentCarrier));
    }
    componentObject.insert("x", QJsonValue::fromVariant(mXpos));
    componentObject.insert("y", QJsonValue::fromVariant(mYpos));
}
