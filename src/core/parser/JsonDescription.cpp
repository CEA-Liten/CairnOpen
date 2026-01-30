#include "JsonDescription.h"
#include "Cairn_Exception.h"
#include "GlobalSettings.h"
#include "CairnUtils.h"
#include <fstream>
#include <filesystem>
namespace fs = std::filesystem;


JsonDescription::JsonDescription(CairnObject *aParent, std::string aName)
    : CairnObject(aParent, aName),
    mException(Cairn_Exception())
{
}
JsonDescription::~JsonDescription()
{
} // ~JsonDescription()

std::vector< t_mapParams > JsonDescription::parseJsonDescription(const std::string &aDescFile)
{    
    extractDocumentData(readJSONFile(aDescFile));
    return mComponents ;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//Extract the data of ports for a given componenet 
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

std::map < std::string, std::map<std::string, std::string> > JsonDescription::getCompoPortData(const std::string &compoName) const
{
    json comp; 
    std::map < std::string, std::map<std::string, std::string> > ports; //contains the data of all ports of componenet compoId

    bool found = false;
    for(auto & component : mComponentsList)
    {
        if (compoName == (std::string)component["nodeName"]) {
            comp = component;
            found = true;
            break;
        }
    }

    if (!found) {
        cWarning() << "getCompoPortData: component " + compoName + " has not been found!";
        return {}; 
    }

    cInfo() << compoName << " getCompoPortData :";

    const json& posList = comp["nodePortsData"];
    uint i = 0;
    for (auto & pos : posList)
    {
        bool isBus = false;
        if (CairnUtils::contains((std::string)comp["componentPERSEEType"], "Bus") || CairnUtils::contains((std::string)comp["componentPERSEEType"], "MultiObj")) {
            isBus = true;
        }
            
        const json &portList = pos["ports"];
        for(auto & port : portList)
        {
            i++;
            std::string portname = port["name"];
            std::string portuse = read(port, "direction", "DATAEXCHANGE");
            cInfo() << "\t - Insert port\t" << std::to_string(i);
            cInfo() << "\t\t - name\t\t\t" << portname;
            cInfo() << "\t\t - type\t\t\t" << read(port, "type") << read(port, "carrier");
            cInfo() << "\t\t - use\t\t\t" << portuse;
            cInfo() << "\t\t - variable\t\t" << read(port, "variable");
            cInfo() << "\t\t - coeff\t\t" << read(port, "coeff");
            cInfo() << "\t\t - offset\t\t" << read(port, "offset");
            cInfo() << "\t\t - checkunit\t\t" << read(port, "checkunit");
            
            //Do not add Bus links
            bool insertPort = false;
            if (!isBus) {
                insertPort = true;
            }

            if (portuse == "") {
                cWarning() << "The direction of " + (std::string)comp["nodeName"] + " port " + portname + " (" + (std::string)port["variable"] + ")" + " is not set!";
                continue;
            }

            t_mapParams portMap; //contains the data of one port
            for(auto & link : mLinksList)
            {
                // Add Port to map IF AND ONLY IF it is not a component-to-bus port that will be automatically built by PERSEE/core.
                std::string identifier;
                if (read(link, "tailNodeId") == "" || read(link,"headNodeId") == "") {
                    //new format
                    identifier = "Name";
                }
                else {
                    //old format
                    identifier = "Id";

                }

                //supports two format : socketName and nodeName + "." + socketName
                std::vector<std::string> headSocketList = CairnUtils::split(read(link, "headSocket" + identifier), '.');
                std::vector<std::string> tailSocketList = CairnUtils::split(read(link, "tailSocket" + identifier), '.');
                                
                std::string headSocketName = headSocketList[headSocketList.size()-1]; 
                std::string tailSocketName = tailSocketList[tailSocketList.size() - 1];

                //port is a link headSocket
                if (read(comp, "node" + identifier) == read(link, "headNode" + identifier) && portname == headSocketName)
                {
                    if(isBus && CairnUtils::contains(getcomponentCategoryFromId(read(link,"tailNode" + identifier)), "Bus")) {
                        //Bus-Bus link
                        Cairn_Exception cairn_error("Error: Bus-Bus links are not allowed : "+ read(link, "headSocket" + identifier) +" to "+ read(link, "tailSocket" + identifier), -1);
                        throw cairn_error;
                    }
                        
                    if (insertPort)
                    {
                        portMap["LinkedComponent"] = getNodeFromId(read(link, "tailNode" + identifier));
                        portMap["BusPortName"] = tailSocketName; //name of the linked bus port

                        cInfo() << "\t\t - " << read(comp, "node" + identifier) + "." + portname
                            << "==" << getNodeFromId(read(link, "headNode" + identifier)) + "." + portname
                            << " category " << getcomponentCategoryFromId(read(link, "headNode" + identifier))
                            << " connected to " << getNodeFromId(read(link, "tailNode" + identifier))
                            << " category " << getcomponentCategoryFromId(read(link, "tailNode" + identifier));

                        break;
                    }
                }

                //port is a link tailSocket
                if (read(comp, "node" + identifier) == read(link, "tailNode" + identifier) && portname == tailSocketName)
                {
                    if (isBus && CairnUtils::contains(getcomponentCategoryFromId(read(link, "headNode" + identifier)), "Bus")) {
                        //Bus-Bus link
                        Cairn_Exception cairn_error("Error: Bus-Bus links are not allowed: " + read(link, "headSocket" + identifier) +" to "+ read(link, "tailSocket" + identifier), -1);
                        throw cairn_error;
                    }
                    
                    if (insertPort)
                    {
                        portMap["LinkedComponent"] = getNodeFromId(read(link, "headNode" + identifier));
                        portMap["BusPortName"] =  headSocketName; //name of the linked bus port

                        cInfo() << "\t\t - " << read(comp, "node" + identifier) + "." + portname
                            << "==" << getNodeFromId(read(link, "headNode" + identifier)) + "." + portname
                            << " category " << getcomponentCategoryFromId((std::string)link["headNode" + identifier])
                            << " connected to " << getNodeFromId(read(link, "tailNode" + identifier))
                            << " category " << getcomponentCategoryFromId(read(link, "tailNode" + identifier));

                        break;
                    }
                }
            }

            if (insertPort)
            {
                std::string portId = read(port, "id");
                if (portId == "") {
                    portId = compoName + "." + portname;
                }

                std::string position = read(port, "position");
                if (position == "") {
                    position = read(pos, "pos");
                }

                portMap["CompoName"] = compoName;
                portMap["Name"] = portname; 
                portMap["Position"] = position;
                portMap["IsDefaultPort"] = read(port, "defaultport");
                portMap["Enabled"] = read(port, "enabled");
                portMap["CarrierType"] = read(port, "carrierType");
                portMap["Carrier"] = read(port, "carrier");
                portMap["Direction"] =  CairnUtils::toUpper(read(port, "direction"));
                portMap["Variable"] = read(port, "variable");
                portMap["Coeff"] = read(port,"coeff") ;
                portMap["Offset"] = read(port, "offset");
                portMap["CheckUnit"] = read(port, "checkunit");

                //Add a port to the map
                ports[portId] = portMap;
            }
        }
    }
    return ports;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Loop on component array
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void JsonDescription::extractDocumentData(const json& jsonData)
{    
    bool oldJsonFormat = false;

    //--------------------------------------------------------------------------//
    //For old study.json that has only one page 
    if (jsonData.contains("Links")) {
        //~~~~~~~~~~~~ Get links array ~~~~~~~~~~~~~~~~~~~~
        json links = jsonData["Links"];
        if (links.is_array())
            mLinksList = links;
    }

    if (jsonData.contains("Components")) {
        //~~~~~~~~~~~~ component array ~~~~~~~~~~~~~~~~~~~~
        oldJsonFormat = true;
        json components = jsonData["Components"];
        if (components.is_array())
            mComponentsList = components;
    }

    //--------------------------------------------------------------------------//
    // 
    //--------------------------------------------------------------------------//
    //For new study.json that may have multiple pages
    if (!oldJsonFormat) {
        //read pages
        for (auto& [key, value] : jsonData.items()) {
            if (CairnUtils::contains(key, "Page") && key != "numberPages") {
                if (value.contains("Links")) {
                    json links = value["Links"];
                    for (auto& link : links) {
                        mLinksList.push_back(link);
                    }
                }
                if (value.contains("Components")) {
                    json components = value["Components"];
                    for (auto& component : components) {
                        mComponentsList.push_back(component);
                    }
                }
            }
        }        
    }
    //--------------------------------------------------------------------------//

    for (auto &component : mComponentsList)
    {
        extractComponentData(component);
    }

    //--------------------------------------------------------------------------//

    //DynamicIndicators are independent from pages
    if (jsonData.contains("DynamicIndicators")) {
        //~~~~~~~~~~~~ dynamic indicators array ~~~~~~~~~~~~~~~~~~~~
        json dynamicIndicatorsList = jsonData["DynamicIndicators"];
        for(auto & dynIndicator : dynamicIndicatorsList)
        {
            extractUDIndicatorData(dynIndicator);
        }
    }

    return;
}

void JsonDescription::extractUDIndicatorData(const json& indicatorJson) {
    t_mapParams indicator;
    indicator["name"] = indicatorJson["name"];
    indicator["formula"] = indicatorJson["formula"];
    mDynamicIndicators.push_back(indicator);
}

std::string JsonDescription::read(const json& in, const std::string& id, const std::string& defaultValue) const
{
    std::string vRet = defaultValue;
    if (in.contains(id)) {
        const json &value = in[id];
        if (value.is_string())
            vRet = value;
        else if (value.is_number_unsigned())
            vRet = std::to_string(value.get<uint64_t>());
        else if (value.is_number_integer())
            vRet = std::to_string(value.get<int64_t>());       
        else if (value.is_number_float())
            vRet = doubleToString(value.get<double>());
        else if (value.is_boolean())
            vRet = std::to_string(value.get<bool>());
        else if (value.is_array()) {                       
            vRet = "";
            std::string vSep = "";
            for (size_t vIdx = 0; vIdx < value.size(); vIdx++) {
                if (value[vIdx].is_string()) {
                    vRet += vSep + value[vIdx].get<std::string>();
                    vSep = ",";
                }
            }             
        }
    }        
    return vRet;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Extract array items by key and print value
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void JsonDescription::extractComponentData(const json &comp)
{
    t_mapParams component;
    t_mapParams compoLabels;

    component["type"] = read(comp, "componentPERSEEType");
    component["nodeId"] = comp["nodeId"];
    component["Model"] = comp["nodeType"]; // TecEcoAnalysis and Solver has an option named "Model" which will overwrite the value
    component["ModelTechnoType"] = comp["nodeTechnoType"];   
    component["componentCarrier"] = read(comp, "componentCarrier"); //only needed for Bus components

    component["Xpos"] = read(comp, "x");
    component["Ypos"] = read(comp, "y");

    if (comp["nodeType"] == "SimulationControl") {
        component["type"] = "SimulationControl";
    }

    cInfo() << "\n >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> nodeName : " << comp.value("nodeName", "");
    cInfo() << "\t - nodeType \t\t" << comp.value("nodeType", "");
    cInfo() << "\t - nodeId  \t\t" << comp.value("nodeId", "");
    cInfo() << "\t - nodeTechnoType \t\t" << comp.value("nodeTechnoType", "");
    cInfo() << "\t - type \t\t" << comp.value("componentPERSEEType", "");
    cInfo() << "\t - componentCarrier \t\t" << comp.value("componentCarrier", "");

    if (component["type"] == "SimulationControl") {
        component["Model"] = comp["nodeTechnoType"];
        //component["id"] = "Cairn";
    }
    if (component["type"] == "EnergyVector") {
        component["EnergyColor"] = comp["energyTypeColor"];
    }

    extractParamData(comp, "optionListJson", component, {"Xpos", "Ypos"});
    extractParamData(comp, "paramListJson", component);
    extractParamData(comp, "envImpactsListJson", component);
    extractParamData(comp, "portImpactsListJson", component);
    extractParamData(comp, "timeSeriesListJson", component);

    /* set name after extract options to override option "id" */
    component["id"] = comp["nodeName"];

    upwardCompatibility(component);

    mComponents.push_back(component);

    //----------- Labels ------------------//
    if (component["type"] == "TecEcoAnalysis") {
        //list of user-defined labels
        if (comp.contains("labelList")) {
            json labelList = comp["labelList"];
            if (labelList.is_array()) {
                for (size_t vIdx = 0; vIdx < labelList.size(); vIdx++) {
                    if (labelList[vIdx].is_string()) {
                        mLabelList.push_back(labelList[vIdx].get<std::string>());
                    }
                }
            }
        }
    }
    else if ( component["type"] != "SimulationControl"
        && component["type"] != "Solver"
        && component["type"] != "EnergyVector"
        ) {
        //mLabelMap keys should be the same as mLabelList 
        //if it is not the case, the labels are filtered later on in OptimProblem
        if (comp.contains("labelListJson")) {
            extractParamData(comp, "labelListJson", compoLabels);
            mLabelMap[component["id"]] = compoLabels;
        }
    }
}


std::string JsonDescription::getNodeFromId(const std::string& nodeId) const
{
    for (auto & comp : mComponentsList)
    {
        if ((std::string)comp["nodeId"] == nodeId) return (std::string)comp["nodeName"] ;
        if ((std::string)comp["nodeName"] == nodeId) return (std::string)comp["nodeName"];

    }
    return "nodeId_Not_Found_"+nodeId;
}
std::string JsonDescription::getcomponentCategoryFromId(const std::string & nodeId) const
{
    for (auto & comp : mComponentsList)
    {
        if ((std::string)comp["nodeId"] == nodeId || (std::string)comp["nodeName"] == nodeId)
        {
            if (CairnUtils::contains((std::string)comp["componentPERSEEType"], "MultiObjCompo"))
                return "BusMultiObjCompo"; // to comply with JsonDescription treatment 
            else
                return (std::string)comp["componentPERSEEType"];
        }
    }
    return "nodeId_Not_Found_"+nodeId;
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// First loop on parameters array
// Extract array items and print key + value
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void JsonDescription::extractParamData(const json& comp, const std::string& a_key, t_mapParams& aMap, const std::vector<std::string>& a_excludedKeys)
{
    if (comp.contains(a_key)) {
        json paramList = comp[a_key];
        cInfo() << "\t - " << a_key << " :";
        for (auto& p : paramList)
        {
            std::vector<std::string>::const_iterator vIter = find(a_excludedKeys.begin(), a_excludedKeys.end(), p["key"]);
            if (vIter != a_excludedKeys.end())
                continue;

           
            switch (p["value"].type())
            {
            case nlohmann::detail::value_t::number_float:  
            case nlohmann::detail::value_t::number_integer:
            case nlohmann::detail::value_t::number_unsigned:  
            case nlohmann::detail::value_t::boolean:
            case nlohmann::detail::value_t::string:
            case nlohmann::detail::value_t::array:
            {
                std::string vValue = read(p, "value");
                cInfo() << "\t\t - " << (std::string)p["key"] << " = " << vValue;
                aMap[p["key"]] = vValue;
            }                
                break;                      
            default:
                cWarning() << "\t\t !!!!!!!!!!!!!!!!! UNKOWN " << a_key << " TYPE - PARAMETER IGNORED - " << (std::string)p["key"];
                break;
            }
        }
    }
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Read the json formatted file
// return the JsonDocument
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
json JsonDescription::readJSONFile(const std::string& aFileName)
{
    json vRet;
    fs::path vPath(aFileName);
    if (!fs::exists(vPath)) {
        cCritical() << "Couldn't open read file: " << aFileName;
    }
    std::ifstream file(aFileName);
    if (file.is_open()) {
        try
        {
            vRet = json::parse(file);            
        }
        catch (const std::exception& e)
        {
            cCritical() << e.what();
        }
    }
    return vRet;
}
