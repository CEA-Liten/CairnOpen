
#include "GUIData.h"
#include "GlobalSettings.h"

GUIData::GUIData(CairnObject *aParent) : CairnObject(aParent)
{
    mGuiNodeModelType = "" ;
    mGuiComponentCairnType = "componentPERSEEType" ;
    mId = GS::GenerateID() ;

    mGuiInputParam = new InputParam(this, "GuiInputParam " + Name());
    declareGuiInputParam();
}  

GUIData::~GUIData()
{
    if (mGuiInputParam) delete mGuiInputParam;
} 

void GUIData::doInit(const std::string aNodeType, const std::string aNodeTechnoType, const std::string aComponentCairnType, const std::map<std::string, std::string> paramMap)
{
    if (aNodeType == "ElectrolyzerDetailed") {
        mGuiNodeModelType = "Electrolyzer";
    }
    else if (aNodeType == "SourceLoadMinMax") {
        mGuiNodeModelType = "SourceLoad";
    }
    else if (aNodeType == "BuildingFlexibleBasic") {
        mGuiNodeModelType = "BuildingFlexible";
    }
    else if (aNodeType == "Battery_V1" 
        || aNodeType == "StorageLinearBounds" 
        || aNodeType == "StorageThermal") 
    {
        mGuiNodeModelType = "StorageGen";
    }
    else {
        mGuiNodeModelType = aNodeType;
    }

    if (aNodeTechnoType == "ElectrolyzerDetailed") {
        mGuiNodeTechnoType = "Electrolyzer";
    }
    else if (aNodeTechnoType == "SourceLoadMinMax") {
        mGuiNodeTechnoType = "SourceLoad";
    }
    else if (aNodeTechnoType == "BuildingFlexibleBasic") {
        mGuiNodeTechnoType = "BuildingFlexible";
    }
    else if (aNodeTechnoType == "Battery_V1" 
        || aNodeTechnoType == "StorageLinearBounds"
        || aNodeTechnoType == "StorageThermal") 
    {
        mGuiNodeTechnoType = "StorageGen";
    }
    else {
        mGuiNodeTechnoType = aNodeTechnoType;
    }

    mGuiComponentCairnType = aComponentCairnType;

    setGuiInputParam(paramMap);
}

void GUIData::declareGuiInputParam()
{
    mGuiInputParam->addParameter("Xpos", &mXpos, 0, false, true, "X position on planteditor", "", "DONOTSHOW");
    mGuiInputParam->addParameter("Ypos", &mYpos, 0, false, true, "Y position on planteditor", "", "DONOTSHOW");
}

void GUIData::setGuiInputParam(const std::map<std::string, std::string> paramMap)
{
    int ierr = mGuiInputParam->readParameters(paramMap);
    if (ierr < 0) {
        Cairn_Exception error("ERROR readParameters: missing value for a Gui parameter of component " + Name(), -1);
        throw& error;
    }

    if (mXpos == 0) {
        if (mGuiComponentCairnType == "SimulationControl") setXpos(50);
        else if (mGuiComponentCairnType == "TecEcoAnalysis") setXpos(150);
        else if (mGuiComponentCairnType == "Solver") setXpos(250);
        else if (mGuiComponentCairnType == "EnergyVector") setXpos(0.5 * mId);
    }
    if (mYpos == 0) {
        if (mGuiComponentCairnType == "SimulationControl"
            || mGuiComponentCairnType == "TecEcoAnalysis"
            || mGuiComponentCairnType == "Solver"
            || mGuiComponentCairnType == "EnergyVector")
            setXpos(10);
    }
}

void GUIData::jsonSaveGUILine(ojson& componentObject, const std::string& componentCarrier)
{ 
    std::string nodeID = std::to_string(mId+1);
    if (mGuiNodeTechnoType == "") mGuiNodeTechnoType = mGuiNodeModelType;
    componentObject = ojson{
        {"nodeId", nodeID},
        {"nodeName", Name()},
        {"componentPERSEEType", mGuiComponentCairnType},
        {"nodeType", mGuiNodeModelType},
        {"nodeTechnoType", mGuiNodeTechnoType},
        {"x", mXpos},
        {"y", mYpos}
    };
    if (componentCarrier != "") {
        componentObject["componentCarrier"] = componentCarrier;
    }   
}
