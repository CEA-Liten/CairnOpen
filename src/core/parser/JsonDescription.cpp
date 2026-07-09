#include "JsonDescription.h"
#include "Cairn_Exception.h"
#include "GlobalSettings.h"
#include "CairnUtils.h"
#include <unordered_set>
#include <fstream>
#include <filesystem>
namespace fs = std::filesystem;

// ============================================================
// Constructor / Destructor
// ============================================================
JsonDescription::JsonDescription(const std::string& vJsonFile)
    : CairnObject(nullptr, "JsonDescription" + vJsonFile)
{
    cInfo() << "Read JSON input file: " + vJsonFile;
    try {
        extractJsonData(vJsonFile);
        extractCompoParamData();
    }
    catch (const Cairn_Exception& error) {
        throw error;
    }
}

JsonDescription::~JsonDescription() = default;

// =================================================================================
// extractJsonData - load file and populate mComponentsList, mLinksList, mGroupsList
// =================================================================================
void JsonDescription::extractJsonData(const std::string& aJsonFile)
{
    const json jsonData = readJSONFile(aJsonFile);

    // ---- Single-page format ----------------------------------------
    if (jsonData.contains("Components"))
    {
        const json& components = jsonData["Components"];
        if (components.is_array())
            mComponentsList = components; 

        if (jsonData.contains("Links"))
        {
            const json& links = jsonData["Links"];
            if (links.is_array())
                mLinksList = links;
        }

        if (jsonData.contains("Groups"))
        {
            const json& groups = jsonData["Groups"];
            if (groups.is_array())
                mGroupsList = groups;
        }
    }
    else
    {
        // ---- Multi-page format -----------------------------------------
        mComponentsList = json::array();
        mLinksList      = json::array();

        for (auto& [key, value] : jsonData.items())
        {
            if (!CairnUtils::contains(key, "Page") || key == "numberPages")
                continue;

            if (value.contains("Components"))
            {
                for (auto& component : value["Components"])
                    mComponentsList.push_back(std::move(component)); // move avoids deep copy
            }

            if (value.contains("Links"))
            {
                for (auto& link : value["Links"])
                    mLinksList.push_back(std::move(link));  
            }

            if (value.contains("Groups"))
            {
                for (auto& group : value["Groups"])
                    mGroupsList.push_back(std::move(group));
            }
        }
    }

    // ---- Dynamic indicators (page-independent) ----------------------------
    if (jsonData.contains("DynamicIndicators"))
    {
        const json& list = jsonData["DynamicIndicators"];
        mDynamicIndicators.reserve(mDynamicIndicators.size() + list.size());
        for (const auto& dynIndicator : list)
            extractUDIndicatorData(dynIndicator);
    }

    // ---- Group name (if applicable) ----------------------------
    if (jsonData.contains("groupName"))
    {
        mGroupName = jsonData["groupName"];
    }

    if (jsonData.contains("mainNodeName"))
    {
        mGroupMainNode = jsonData["mainNodeName"];
    }

    // Build component lookup indices now that the list is fully populated.
    buildComponentIndex();
}

// ================================================================
// extractCompoParamData - build list of component parameter maps
// ================================================================
void JsonDescription::extractCompoParamData()
{
    for (auto& comp : mComponentsList)
    {
        t_mapParamData component;

        const std::string componentType = extractComponentType(comp);
        const std::string nodeId = read(comp, "nodeId");
        const std::string nodeType = read(comp, "nodeType");
        const std::string nodeTechnoType = read(comp, "nodeTechnoType");
        const std::string componentCarrier = read(comp, "componentCarrier");
        const std::string xpos = read(comp, "x");
        const std::string ypos = read(comp, "y");
        const std::string nodeName = read(comp, "nodeName");
        const std::string energyColor = read(comp, "energyTypeColor");

        CairnUtils::setParamValue(component, "type", componentType);
        CairnUtils::setParamValue(component, "nodeId", nodeId);
        CairnUtils::setParamValue(component, "name", nodeName);
        CairnUtils::setParamValue(component, "ModelType", nodeType);
        CairnUtils::setParamValue(component, "ModelTechnoType", nodeTechnoType);
        CairnUtils::setParamValue(component, "componentCarrier", componentCarrier);
        CairnUtils::setParamValue(component, "Xpos", xpos);
        CairnUtils::setParamValue(component, "Ypos", ypos);

        if (CairnUtils::isEnergyVector(componentType)) {
            CairnUtils::setParamValue(component, "EnergyColor", energyColor);
        }

        cDebug() << "\n >>>>>>>>>>>> nodeName : " << nodeName;
        cDebug() << "\t - nodeId  \t\t" << nodeId;
        cDebug() << "\t - componentType \t\t" << componentType;
        cDebug() << "\t - nodeType \t\t" << nodeType;
        cDebug() << "\t - nodeTechnoType \t" << nodeTechnoType;
        cDebug() << "\t - componentCarrier \t" << componentCarrier;

        extractParamData(comp, "optionListJson", component, "", { "Xpos", "Ypos" });
        extractParamData(comp, "paramListJson", component);
        extractParamData(comp, "envImpactsListJson", component);
        extractParamData(comp, "portImpactsListJson", component);
        extractParamData(comp, "timeSeriesListJson", component);

        upwardCompatibility(component);

        // ---- Labels -------------------------------------------------------
         
        const bool skip = componentType == "SimulationControl" 
            || componentType == "Solver"
            || CairnUtils::isEnergyVector(componentType);

        if (componentType == "TecEcoAnalysis")
        {
            if (const auto it = comp.find("labelList"); it != comp.end() && it->is_array())
            {
                mLabelList.reserve(mLabelList.size() + it->size());
                for (const auto& lbl : *it)
                    if (lbl.is_string())
                        mLabelList.push_back(lbl.get<std::string>());
            }
        }
        else if (!skip)
        {
            if (comp.contains("labelListJson"))
            {
                t_mapLabels compoLabels;
                extractLabels(comp, compoLabels);
                mLabelMap.emplace(CairnUtils::getParamValue(component, "name"),
                    std::move(compoLabels));
            }
        }

        if (componentType == "TecEcoAnalysis")
            mTecEcoParamData = component;
        else if (componentType == "SimulationControl")
            mSimulationControlParamData = component;
        else if (componentType == "Solver")
            mSolverParamData = component;
        else if (CairnUtils::isEnergyVector(componentType))
            mCarrierParamDataList.push_back(std::move(component));
        else if (CairnUtils::isBus(componentType))
            mBusParamDataList.push_back(std::move(component));
        else
            mComponentParamDataList.push_back(std::move(component));
    }
}

// ============================================================
// extractPortParamData
// ============================================================
std::map<std::string, t_mapParamData>
JsonDescription::extractPortParamData(const std::string& compoName) const
{
    // O(1) lookup via index instead of O(n) linear scan.
    const json* compPtr = nullptr;
    if (const auto it = mComponentIndexByName.find(compoName);
        it != mComponentIndexByName.end())
    {
        compPtr = &mComponentsList[it->second];
    }

    if (!compPtr) {
        cDebug() << "extractPortParamData: component " + compoName + " has not been found!";
        return {};
    }

    const json& comp = *compPtr;
    cDebug() << compoName << " extractPortParamData:";

    const std::string type = extractComponentType(comp);
    const bool isBus = CairnUtils::contains(type, "Bus") ||
        CairnUtils::contains(type, "MultiObj");

    // Build link index 
    buildLinkIndex();

    if (!comp.contains("nodePortsData")) {
        cDebug() << "extractPortParamData: component " << compoName << " has no nodePortsData!";
        return {};
    }

    std::map<std::string, t_mapParamData> ports;

    const json& posList = comp["nodePortsData"];
    uint portIdx = 0;

    for (const auto& pos : posList)
    {
        for (const auto& port : pos["ports"])
        {
            ++portIdx;

            const std::string portName = read(port, "name", "");
            const std::string portID = read(port, "id", compoName + "." + portName);
            const std::string portDirection = read(port, "direction", "DATAEXCHANGE");
            const std::string portVariable = read(port, "variable", "");
            const std::string portCarrier = read(port, "carrier");

            cDebug() << "\t - Port\t" << portIdx;
            cDebug() << "\t\t - id\t\t\t" << portID;
            cDebug() << "\t\t - name\t\t\t" << portName;
            cDebug() << "\t\t - carrier\t\t\t" << portCarrier;
            cDebug() << "\t\t - direction\t\t\t" << portDirection;
            cDebug() << "\t\t - variable\t\t" << portVariable;
            cDebug() << "\t\t - coeff\t\t" << read(port, "coeff");
            cDebug() << "\t\t - offset\t\t" << read(port, "offset");
            cDebug() << "\t\t - checkunit\t\t" << read(port, "checkunit");

            const std::string portLabel = compoName + " port " + portName + " (" + portID + ")";

            if (portVariable.empty()) {
                if (!isBus)
                    cDebug() << "Variable not defined for" << portLabel;
                continue;
            }
            if (portDirection.empty()) {
                cDebug() << "Direction not defined for" << portLabel;
                continue;
            }
            if (portCarrier.empty() || portCarrier == "NO_CARRIER") {
                cDebug() << "Carrier not defined for" << portLabel;
                continue;
            }

            bool insertPort = true;
            t_mapParamData portMap;

            // --- Use the link index: only iterate links that are relevant to this node ---
            // Determine which identifier format this file uses
            const std::string compNodeId = read(comp, "nodeId");
            const std::string compNodeName = read(comp, "nodeName");

            // Gather candidate link indices (nodeId key or nodeName key)
            auto gatherLinks = [&](const std::string& key) -> const std::vector<std::size_t>*
            {
                const auto it = mLinkIndexByNode.find(key);
                return (it != mLinkIndexByNode.end()) ? &it->second : nullptr;
            };

            // Process links from either id-based or name-based bucket
            auto processLinkBucket = [&](const std::vector<std::size_t>* bucket) -> bool  
            {
                if (!bucket) return false;
                for (std::size_t li : *bucket)
                {
                    const json& link = mLinksList[li];

                    const std::string identifier =
                        (read(link, "tailNodeId") == "" || read(link, "headNodeId") == "")
                        ? "Name" : "Id"; // In old json format, nodeId was used in links definition

                    const std::string headSocket = read(link, "headSocket" + identifier);
                    const std::string tailSocket = read(link, "tailSocket" + identifier);

                    const std::vector<std::string> headSocketList = CairnUtils::split(headSocket, '.');
                    const std::vector<std::string> tailSocketList = CairnUtils::split(tailSocket, '.');

                    const std::string headSocketName = headSocketList.back();
                    const std::string tailSocketName = tailSocketList.back();

                    const std::string compNode = read(comp, "node" + identifier);
                    const std::string headNode = read(link, "headNode" + identifier);
                    const std::string tailNode = read(link, "tailNode" + identifier);

                    const bool isHeadSocket = (compNode == headNode && portName == headSocketName);
                    const bool isTailSocket = (compNode == tailNode && portName == tailSocketName);

                    if (!isHeadSocket && !isTailSocket)
                        continue;

                    if (isBus)
                    {
                        const std::string& otherNode = isHeadSocket ? tailNode : headNode;
                        const std::string& otherSocketName = isHeadSocket ? tailSocketName : headSocketName;

                        const std::string portVar = getPortVariable(otherNode, otherSocketName);
                        if (!portVar.empty()) {
                            if (CairnUtils::contains(getcomponentCategoryFromId(otherNode), "Bus")) {
                                cWarning() << "Only one Bus should have a variable in case of a Bus-Bus link: " << compNode << otherNode;
                                // throw error ?!
                            }
                            insertPort = false;
                            return true; // break outer
                        }
                    }

                    const std::string headNodeName = getNodeFromId(headNode);
                    const std::string tailNodeName = getNodeFromId(tailNode);

                    CairnUtils::setParamValue(portMap, "LinkedComponent", "");

                    if (isHeadSocket)
                    {
                        CairnUtils::setParamValue(portMap, "LinkedComponent", tailNodeName);
                        CairnUtils::setParamValue(portMap, "BusPortName", tailSocketName);
                        cDebug() << "\t\t - " << compNode << "." << portName
                            << " ==" << tailNodeName << "." << portName
                            << " category " << getcomponentCategoryFromId(headNode)
                            << " connected to " << tailNodeName
                            << " category " << getcomponentCategoryFromId(tailNode);
                    }
                    else
                    {
                        CairnUtils::setParamValue(portMap, "LinkedComponent", headNodeName);
                        CairnUtils::setParamValue(portMap, "BusPortName", headSocketName);
                        cDebug() << "\t\t - " << compNode << "." << portName
                            << " ==" << headNodeName << "." << portName
                            << " category " << getcomponentCategoryFromId(headNode)
                            << " connected to " << headNodeName
                            << " category " << getcomponentCategoryFromId(tailNode);
                    }
                    return true; // found – stop searching
                }
                return false;
            };

            // Try id-based bucket first, then name-based.
            if (!processLinkBucket(gatherLinks(compNodeId)))
                processLinkBucket(gatherLinks(compNodeName));

            if (!insertPort)
                continue;

            std::string position = read(port, "position");
            if (position.empty())
                position = read(pos, "pos");

            const std::string isDefaultPort = read(port, "defaultport");
            const std::string enabled = read(port, "enabled");
            const std::string carrierType = read(port, "carrierType");
            const std::string carrier = read(port, "carrier");
            const std::string direction = CairnUtils::toUpper(read(port, "direction"));
            const std::string coeff = read(port, "coeff");
            const std::string offset = read(port, "offset");
            const std::string checkUnit = read(port, "checkunit");

            CairnUtils::setParamValue(portMap, "CompoName", compoName);
            CairnUtils::setParamValue(portMap, "Name", portName);
            CairnUtils::setParamValue(portMap, "Position", position);
            CairnUtils::setParamValue(portMap, "IsDefaultPort", isDefaultPort);
            CairnUtils::setParamValue(portMap, "Enabled", enabled);
            CairnUtils::setParamValue(portMap, "CarrierType", carrierType);
            CairnUtils::setParamValue(portMap, "Carrier", carrier);
            CairnUtils::setParamValue(portMap, "Direction", direction);
            CairnUtils::setParamValue(portMap, "Variable", portVariable);
            CairnUtils::setParamValue(portMap, "Coeff", coeff);
            CairnUtils::setParamValue(portMap, "Offset", offset);
            CairnUtils::setParamValue(portMap, "CheckUnit", checkUnit);

            extractParamData(port, "params", portMap, portName);

            ports.emplace(portID, std::move(portMap));
        }
    }

    return ports;
}

// ============================================================
// extractUDIndicatorData
// ============================================================
void JsonDescription::extractUDIndicatorData(const json& indicatorJson)
{
    mDynamicIndicators.push_back({
        {"name",    indicatorJson.value("name",    std::string{})},
        {"formula", indicatorJson.value("formula", std::string{})}
    });
}

// ============================================================
// extractParamData
// ============================================================
void JsonDescription::extractParamData(const json& comp, const std::string& a_key,
    t_mapParamData& aMap, const std::string& portName,
    const std::vector<std::string>& a_excludedKeys) const
{
    const auto it = comp.find(a_key);
    if (it == comp.end())
        return;

    cDebug() << "\t - " << a_key << " :";
    for (const auto& param : *it)
        extractParam(param, aMap, portName, a_excludedKeys);
}

// ============================================================
// extractParam
// ============================================================
bool JsonDescription::extractParam(const json& param, t_mapParamData& outMap,
    const std::string& portName,
    const std::vector<std::string>& excludedKeys) const
{
    if (!param.contains("key") || !param.contains("value"))
        return false;

    std::string key = param["key"].get<std::string>();

    if (!portName.empty())
    {
        const std::string suffix = portName + ".";
        if (auto pos = key.find(suffix); pos != std::string::npos)
            key.erase(pos, suffix.size());
    }

    // excludedKeys is a small vector so std::find is fine
    if (std::find(excludedKeys.begin(), excludedKeys.end(), key) != excludedKeys.end())
        return false;

    const auto type = param["value"].type();
    const bool supported =
        type == nlohmann::detail::value_t::number_float ||
        type == nlohmann::detail::value_t::number_integer ||
        type == nlohmann::detail::value_t::number_unsigned ||
        type == nlohmann::detail::value_t::boolean ||
        type == nlohmann::detail::value_t::string ||
        type == nlohmann::detail::value_t::array;

    if (!supported) {
        cWarning() << "\t\t !!!! UNKNOWN TYPE - PARAMETER IGNORED - " << key;
        return false;
    }

    std::string value = read(param, "value");
    std::string comment = read(param, "comment", "");
    cDebug() << "\t\t - " << key << " = " << value;

    CairnUtils::setParamValue(outMap, key, std::move(value));
    CairnUtils::setParamComment(outMap, key, std::move(comment));

    return true;
}

// ============================================================
// extractLabels
// ============================================================
void JsonDescription::extractLabels(const json& comp, t_mapLabels& aMap)
{
    static constexpr const char* a_key = "labelListJson";

    const auto listIt = comp.find(a_key);
    if (listIt == comp.end())
        return;

    cDebug() << "\t - " << a_key << " :";

    for (const auto& p : *listIt)
    {
        const auto& val = p["value"];
        switch (val.type())
        {
        case nlohmann::detail::value_t::number_float:
        case nlohmann::detail::value_t::number_integer:
        case nlohmann::detail::value_t::number_unsigned:
        case nlohmann::detail::value_t::boolean:
        case nlohmann::detail::value_t::string:
        case nlohmann::detail::value_t::array:
        {
            std::string vValue = read(p, "value");
            const std::string key = p["key"].get<std::string>();
            cDebug() << "\t\t - " << key << " = " << vValue;
            aMap.emplace(std::move(key), std::move(vValue));
            break;
        }
        default:
            cWarning() << "\t\t !!!!!!!! UNKNOWN labelListJson TYPE - PARAMETER IGNORED - "
                << p["key"].get<std::string>();
            break;
        }
    }
}

// ============================================================
// extractGroups
// ============================================================
std::vector<t_mapGroups> JsonDescription::extractGroupData() const
{
    std::vector<t_mapGroups> groups;
    groups.reserve(mGroupsList.size());

    for (const auto& grp : mGroupsList)
    {
        t_mapGroups group;

        // Basic fields
        const std::string groupId = read(grp, "groupId");
        const std::string groupName = read(grp, "groupName");
        const std::string mainNodeName = read(grp, "mainNodeName");
        const std::string minimized = read(grp, "minimized");
        const std::string borderColor = read(grp, "borderColor");

        group.emplace("groupId", groupId);
        group.emplace("groupName", groupName);
        group.emplace("mainNodeName", mainNodeName);
        group.emplace("minimized", minimized);
        group.emplace("borderColor", borderColor);

        // listNodeName[] -> store as comma-separated 
        if (grp.contains("listNodeName") && grp["listNodeName"].is_array())
        {
            std::string str;

            for (const auto& node : grp["listNodeName"])
            {
                if (node.is_string()) 
                {
                    if (!str.empty())
                        str += ",";

                    str += node.get<std::string>();
                }
            }

            group.emplace("listNodeName", str);
        }

        groups.push_back(std::move(group));
    }

    return groups;
}

// ============================================================
// extractComponentType
// ============================================================
std::string JsonDescription::extractComponentType(const json& comp) const
{
    // For backward compatibility
    if (const auto it = comp.find("componentPERSEEType"); it != comp.end())
        return it->get<std::string>();
    return comp.value("componentType", std::string{});
}

// ============================================================
// getPortVariable (O(1) via index)
// ============================================================
std::string JsonDescription::getPortVariable(const std::string& compoName,
                                              const std::string& portName) const
{
    // O(1) lookup using the pre-built index.
    const json* compPtr = nullptr;
    if (const auto it = mComponentIndexByName.find(compoName);
        it != mComponentIndexByName.end())
        compPtr = &mComponentsList[it->second];
    else if (const auto it2 = mComponentIndexById.find(compoName);
             it2 != mComponentIndexById.end())
        compPtr = &mComponentsList[it2->second];

    if (!compPtr) {
        cWarning() << "Component not found:" << compoName;
        return "";
    }

    const json& posList = (*compPtr)["nodePortsData"];
    for (const auto& pos : posList)
    {
        for (const auto& port : pos["ports"])
        {
            if (read(port, "name", "") == portName)
                return read(port, "variable", "");
        }
    }

    cWarning() << "Port not found:" << portName << "in component:" << compoName;
    return "";
}

// ============================================================
// getNodeFromId  (O(1) via index)
// ============================================================
std::string JsonDescription::getNodeFromId(const std::string& nodeId) const
{
    if (const auto it = mComponentIndexByName.find(nodeId);
        it != mComponentIndexByName.end())
        return nodeId; // argument was already a name

    if (const auto it = mComponentIndexById.find(nodeId);
        it != mComponentIndexById.end())
        return mComponentsList[it->second].value("nodeName", std::string{});

    return "nodeId_Not_Found_" + nodeId;
}

// ============================================================
// getcomponentCategoryFromId  (O(1) via index)
// ============================================================
std::string JsonDescription::getcomponentCategoryFromId(const std::string& nodeId) const
{
    const json* compPtr = nullptr;

    if (const auto it = mComponentIndexById.find(nodeId);
        it != mComponentIndexById.end())
        compPtr = &mComponentsList[it->second];
    else if (const auto it2 = mComponentIndexByName.find(nodeId);
             it2 != mComponentIndexByName.end())
        compPtr = &mComponentsList[it2->second];

    if (compPtr)
    {
        const std::string type = extractComponentType(*compPtr);
        return CairnUtils::contains(type, "MultiObjCompo")
               ? "BusMultiObjCompo"
               : type;
    }

    return "nodeId_Not_Found_" + nodeId;
}

// ============================================================
// readJSONFile
// ============================================================
json JsonDescription::readJSONFile(const std::string& aFileName)
{
    if (!fs::exists(aFileName))
        throw Cairn_Exception("JSON file not found: " + aFileName, -1);

    // Open in binary mode to preserve UTF-8 encoding.
    std::ifstream file(aFileName, std::ios::binary);
    if (!file.is_open())
        throw Cairn_Exception("JSON file could not be opened: " + aFileName, -1);

    try {
        return json::parse(file);
    }
    catch (const json::parse_error& e) {
        throw Cairn_Exception("JSON parse error in file: " + aFileName
            + "\n  at byte " + std::to_string(e.byte)
            + "\n  " + e.what(), -1);
    }
    catch (const std::exception& e) {
        throw Cairn_Exception("Error reading JSON file: " + aFileName
            + "\n  " + e.what(), -1);
    }
}


// ============================================================
// read – extract a JSON field as std::string
// ============================================================
std::string JsonDescription::read(const json& in, const std::string& id,
    const std::string& defaultValue) const
{
    // Use find() to avoid double lookup (contains + operator[]).
    const auto it = in.find(id);
    if (it == in.end())
        return defaultValue;

    const json& value = *it;

    if (value.is_string())
        return CairnUtils::trim(value.get<std::string>());
    if (value.is_number_unsigned())
        return std::to_string(value.get<uint64_t>());
    if (value.is_number_integer())
        return std::to_string(value.get<int64_t>());
    if (value.is_number_float())
        return doubleToString(value.get<double>());
    if (value.is_boolean())
        return std::to_string(value.get<bool>());

    if (value.is_array())
    {
        std::string result;
        // Pre-scan for string elements to avoid repeated reallocations.
        for (const auto& elem : value)
        {
            if (!elem.is_string()) continue;
            if (!result.empty()) result += ',';
            result += elem.get<std::string>();
        }
        return result;
    }

    return defaultValue;
}

// ============================================================
// Index builders
// ============================================================

// Build O(1) lookup maps from mComponentsList once, after loading.
void JsonDescription::buildComponentIndex()
{
    const std::size_t n = mComponentsList.size();
    mComponentIndexById.reserve(n);
    mComponentIndexByName.reserve(n);

    for (std::size_t i = 0; i < n; ++i)
    {
        const json& c = mComponentsList[i];
        if (c.contains("nodeId"))
            mComponentIndexById.try_emplace(c["nodeId"].get<std::string>(), i);
        if (c.contains("nodeName"))
            mComponentIndexByName.try_emplace(c["nodeName"].get<std::string>(), i);
    }
}

// Build per-node link index (first call of extractPortParamData).
void JsonDescription::buildLinkIndex() const
{
    if (mLinkIndexBuilt) return;

    const std::size_t nLinks = mLinksList.size();
    // Each link has at most 2 nodes; reserve generously.
    mLinkIndexByNode.reserve(nLinks * 2);

    for (std::size_t i = 0; i < nLinks; ++i)
    {
        const json& link = mLinksList[i];
        // New format uses Id fields; old format uses Name fields.
        for (const char* field : { "headNodeId", "tailNodeId", "headNodeName", "tailNodeName" })
        {
            if (link.contains(field))
            {
                const std::string node = link[field].get<std::string>();
                if (!node.empty())
                    mLinkIndexByNode[node].push_back(i);
            }
        }
    }
    mLinkIndexBuilt = true;
}