#ifndef GUIData_H
#define GUIData_H
class GUIData ;

#include <QtCore>
#include "qdom.h"
#include "CairnCore_global.h"
#include <QObject>

const float MAX_X = 10000;
const float MAX_Y = 10000;

const int OFFSET_X = 100;
const int OFFSET_Y = 100;

class CAIRNCORESHARED_EXPORT GUIData : public QObject
{
    Q_OBJECT
public:

    GUIData(QObject *aParent, QString aName);

    virtual ~GUIData();
    void doInit(const QString aType, const QString aSubType, const QString aCategory, const int aXpos, const int aYpos) ;

    int getXpos() const {return mXpos ;}
    int getYpos() const {return mYpos ;}

    QString getGuiNodeClass() const {return mGuiNodeModelType ;}
    QString getGuiNodeClassSubtype() const {return mGuiNodeTechnoType ;}
    QString getGuiNodeName() const {return mGuiNodeName ;}
    QString getGuiComponentPERSEEType() const {return mGuiComponentCairnType ;}

    uint GetId() {return mId;}

    void setXpos (int val) { mXpos = fmax (0., fmin(val, MAX_X)); }
    void setYpos (int val) { mYpos = fmax (0., fmin(val, MAX_Y)); }

    void setGuiNodeClass        (const QString val) {mGuiNodeModelType = val;}
    void setGuiNodeClassSubtype (const QString val) {mGuiNodeTechnoType = val;}
    void setGuiNodeName         (const QString val) { mGuiNodeName = val;}
    void setGuiComponentPERSEEType (const QString val)      {mGuiComponentCairnType = val;}

    void jsonSaveGUILine(QJsonObject& componentObject, const QString& componentCarrier="");

protected:

    uint mId ;

    int mXpos ;         /** X position on planteditor */
    int mYpos ;         /** Y position on planteditor */

    QString mGuiNodeModelType ;  /** GuiClass on planteditor */
    QString mGuiNodeName ;  /** GuiName on planteditor */
    QString mGuiNodeTechnoType ;  /** GuiType on planteditor - models of the same GuiClass */
    QString mGuiComponentCairnType ;  /** mGuiCategory on planteditor */
    QString mGuiCarrier ;  /** mGuiCarrier name on planteditor */
};

#endif // GUIData_H
