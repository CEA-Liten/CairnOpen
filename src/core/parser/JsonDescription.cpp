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

std::string JsonDescription::extractComponentType(const json& comp) const
{
    std::string type;
    if (comp.contains("componentPERSEEType")) {
        type = (std::string)comp["componentPERSEEType"];
    }
    else {
        type = (std::string)comp["componentType"];

    }
    return type;
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

    std::string type = extractComponentType(comp);
    const bool isBus = (CairnUtils::contains(type, "Bus") || CairnUtils::contains(type, "MultiObj")) ? true : false;

    const json& posList = comp["nodePortsData"];
    uint i = 0;
    for (auto & pos : posList)
    {
        const json &portList = pos["ports"];
        for(auto & port : portList)
        {
            i++;

            const std::string portName = read(port, "name", "");
            const std::string portID = read(port, "id", compoName + "." + portName);
            const std::string portDirection = read(port, "direction", "DATAEXCHANGE");
            const std::string portVariable = read(port, "variable", "");
            const std::string portCarrier = read(port, "carrier");

            cInfo() << "\t - Port\t" << std::to_string(i);
            cInfo() << "\t\t - id\t\t\t" << portID;
            cInfo() << "\t\t - name\t\t\t" << portName;
            cInfo() << "\t\t - carrier\t\t\t" << portCarrier;
            cInfo() << "\t\t - direction\t\t\t" << portDirection;
            cInfo() << "\t\t - variable\t\t" << portVariable;
            cInfo() << "\t\t - coeff\t\t" << read(port, "coeff");
            cInfo() << "\t\t - offset\t\t" << read(port, "offset");
            cInfo() << "\t\t - checkunit\t\t" << read(port, "checkunit");
            
            const std::string portLabel = compoName + " port " + portName + " (" + portID + ")";

            if (portVariable.empty()) //Only add ports that have variables
            {
                if (!isBus) { //A Bus port has a variable only if it is the master port in the case of a Bus-Bus link
                    cWarning() << "Variable not defined for" << portLabel;
                }
                continue;
            }

            if (portDirection.empty()) {
                cWarning() << "Direction not defined for" << portLabel;
                continue;
            }

            if (portCarrier.empty() || portCarrier == "NO_CARRIER") {
                cWarning() << "Carrier not defined for" << portLabel;
                continue;
            }

            bool insertPort = true;  // insert port even if it is not connected

            // Add port to map  
            t_mapParams portMap; //contains the data of one port
            for(auto & link : mLinksList)
            {
                const std::string identifier = (read(link, "tailNodeId") == "" || read(link, "headNodeId") == "")
                    ? "Name" : "Id"; // new format : old format

                //supports two format : socketName and nodeName + "." + socketName
                const std::string headSocket = read(link, "headSocket" + identifier);
                const std::string tailSocket = read(link, "tailSocket" + identifier);
                std::vector<std::string> headSocketList = CairnUtils::split(headSocket, '.');
                std::vector<std::string> tailSocketList = CairnUtils::split(tailSocket, '.');
                                
                std::string headSocketName = headSocketList[headSocketList.size()-1]; 
                std::string tailSocketName = tailSocketList[tailSocketList.size() - 1];

                const std::string compNode = read(comp, "node" + identifier); //nodeId or nodeName
                const std::string headNode = read(link, "headNode" + identifier);
                const std::string tailNode = read(link, "tailNode" + identifier);

                const bool isHeadSocket = (compNode == headNode && portName == headSocketName);
                const bool isTailSocket = (compNode == tailNode && portName == tailSocketName);

                if (!isHeadSocket && !isTailSocket) {
                    // A link that is not related to the current port!
                    continue;
                }

                // Filter Bus ports that have variables in case of Bus-Componenet links (old studies)
                if (isBus)
                {
                    const std::string& otherNode = isHeadSocket ? tailNode : headNode;
                    const std::string& otherSocketName = isHeadSocket ? tailSocketName : headSocketName;

                    if (getPortVariable(otherNode, otherSocketName) != "")
                    {
                        if (CairnUtils::contains(getcomponentCategoryFromId(otherNode), "Bus")) {
                            throw Cairn_Exception("Only one Bus should have a variable in case of a Bus-Bus link!", -1);
                        }
                        // else: Bus-Component link => Bus port which has a variable in an old study

                        insertPort = false; // don't insert port!
                        break; 
                    }
                }

                const std::string headNodeName = getNodeFromId(headNode);
                const std::string tailNodeName = getNodeFromId(tailNode);
                if (isHeadSocket)
                {
                    portMap["LinkedComponent"] = tailNodeName;
                    portMap["BusPortName"] = tailSocketName;
                    cInfo() << "\t\t - " << compNode << "." << portName
                        << " ==" << tailNodeName << "." << portName
                        << " category " << getcomponentCategoryFromId(headNode)
                        << " connected to " << tailNodeName
                        << " category " << getcomponentCategoryFromId(tailNode);
                }
                else // isTailSocket
                {
                    portMap["LinkedComponent"] = headNodeName;
                    portMap["BusPortName"] = headSocketName;
                    cInfo() << "\t\t - " << compNode << "." << portName
                        << " ==" << headNodeName << "." << portName
                        << " category " << getcomponentCategoryFromId(headNode)
                        << " connected to " << headNodeName
                        << " category " << getcomponentCategoryFromId(tailNode);
                }

                break; // found!  
            }

            if (insertPort)
            {
                std::string position = read(port, "position");
                if (position == "") {
                    position = read(pos, "pos");
                }

                portMap["CompoName"] = compoName;
                portMap["Name"] = portName;
                portMap["Position"] = position;
                portMap["IsDefaultPort"] = read(port, "defaultport");
                portMap["Enabled"] = read(port, "enabled");
                portMap["CarrierType"] = read(port, "carrierType");
                portMap["Carrier"] = read(port, "carrier");
                portMap["Direction"] =  CairnUtils::toUpper(read(port, "direction"));
                portMap["Variable"] = portVariable;
                portMap["Coeff"] = read(port,"coeff") ;
                portMap["Offset"] = read(port, "offset");
                portMap["CheckUnit"] = read(port, "checkunit");

                //Add a port to the map
                ports[portID] = portMap;
            }
        }
    }
    return ports;
}

std::string JsonDescription::getPortVariable(const std::string& compoName, const std::string& portName) const
{
    // Find the component by name
    const auto compIt = std::find_if(mComponentsList.cbegin(), mComponentsList.cend(),
        [&compoName](const json& component)
        {
            return (std::string)component["nodeName"] == compoName || (std::string)component["nodeId"] == compoName;
        });

    if (compIt == mComponentsList.cend())
    {
        cWarning() << "Component not found:" << compoName;
        return "";
    }

    // Search for the port in all port groups
    const json& posList = (*compIt)["nodePortsData"];
    for (const auto& pos : posList)
    {
        const json& portList = pos["ports"];
        for (const auto& port : portList)
        {
            if (read(port, "name", "") == portName)
            {
                return read(port, "variable", "");
            }
        }
    }

    cWarning() << "Port not found:" << portName
        << "in component:" << compoName;
    return "";
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
            vRet = CairnUtils::trim(value.get<std::string>()); // explicit UTF-8 string
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

    std::string type = extractComponentType(comp);
    component["type"] = type;
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
    cInfo() << "\t - type \t\t" << type;
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
            const std::string type = extractComponentType(comp);
            if (CairnUtils::contains(type, "MultiObjCompo")) 
                return "BusMultiObjCompo"; // to comply with JsonDescription treatment 
            else 
                return type;
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
    // Check file existence before opening
    if (!fs::exists(aFileName)) {
        throw Cairn_Exception("JSON file not found: " + aFileName, -1);
    }

    // Open in binary mode to preserve UTF-8 encoding
    std::ifstream file(aFileName, std::ios::binary);
    if (!file.is_open()) {
        throw Cairn_Exception("JSON file could not be opened: " + aFileName, -1);
    }

    try
    {
        return json::parse(file);
    }
    catch (const json::parse_error& e)
    {
        throw Cairn_Exception("JSON parse error in file: " + aFileName
            + "\n  at byte " + std::to_string(e.byte)
            + "\n  " + e.what(), -1);
    }
    catch (const std::exception& e)
    {
        throw Cairn_Exception("Error reading JSON file: " + aFileName
            + "\n  " + e.what(), -1);
    }
}