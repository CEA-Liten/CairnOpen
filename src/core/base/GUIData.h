#ifndef GUIData_H
#define GUIData_H
class GUIData ;

#include "CairnCore_global.h"
#include "InputParam.h"

const float MAX_X = 2000;
const float MAX_Y = 2000;

const int OFFSET_X = 100;
const int OFFSET_Y = 100;

class CAIRNCORESHARED_EXPORT GUIData : public CairnObject
{    
public:

    GUIData(CairnObject *aParent);
    ~GUIData();

    void doInit(const std::string aNodeType, const std::string aNodeTechnoType, 
        const std::string aComponentType, const std::map<std::string, std::string> paramMap = {}) ;

    int getXpos() const {return mXpos ;}
    int getYpos() const {return mYpos ;}

    std::string getGuiNodeClass() const {return mGuiNodeModelType ;}
    std::string getGuiNodeClassSubtype() const {return mGuiNodeTechnoType ;}
    std::string getGuiComponentType() const {return mGuiComponentType;}

    uint GetId() {return mId;}

    void setXpos (int val) { mXpos = fmax (0., fmin(val, MAX_X)); }
    void setYpos (int val) { mYpos = fmax (0., fmin(val, MAX_Y)); }

    std::string Name() { return std::string(this->parent()->objectName().c_str()); } /* component name */
    void setGuiNodeClass        (const std::string val) {mGuiNodeModelType = val;}
    void setGuiNodeClassSubtype (const std::string val) {mGuiNodeTechnoType = val;}
    void setGuiComponentType (const std::string val)      { mGuiComponentType = val;}

    void jsonSaveGUILine(ojson& componentObject, const std::string& componentCarrier="");

 	InputParam* getGuiInputParam() { return mGuiInputParam; }

    void declareGuiInputParam();
    void setGuiInputParam(const std::map<std::string, std::string> paramMap);protected:

    uint mId ;

    InputParam* mGuiInputParam{ nullptr };

    int mXpos ;         /** X position on planteditor */
    int mYpos ;         /** Y position on planteditor */

    std::string mGuiNodeModelType ;  /** GuiClass on planteditor */
    std::string mGuiNodeTechnoType ;  /** GuiType on planteditor - models of the same GuiClass */
    std::string mGuiComponentType ;  /** mGuiCategory on planteditor */
    std::string mGuiCarrier ;  /** mGuiCarrier name on planteditor */
};

#endif // GUIData_H
