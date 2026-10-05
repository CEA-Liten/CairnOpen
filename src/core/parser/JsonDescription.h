#ifndef JsonDescription_H
#define JsonDescription_H
class JsonDescription;

#include "CairnCore_global.h"
#include "Cairn_Exception.h"
#include "CairnUtils.h"

#include <sstream>
#include <cstdint>
#include <charconv>         // std::to_chars (fast number formatting)
#include <unordered_map>    // O(1) component/link lookups

#define DECIMAL_PRECISION 9
#define SIGNIFICANT_DIGITS 12

typedef std::map<std::string, std::string> t_mapUserIndicator;
typedef std::map<std::string, std::string> t_mapLabels;
typedef std::map<std::string, std::string> t_mapGroups;

// ------------------------------------------------------------------------
// Fast double to string conversion: uses std::to_chars (no locale, no heap)
// ------------------------------------------------------------------------
static std::string doubleToString(const double value_d)
{
    // Buffer large enough for any finite double at 9 decimal places
    char buf[32];
    // to_chars with fixed notation and DECIMAL_PRECISION decimal digits
    auto [ptr, ec] = std::to_chars(buf, buf + sizeof(buf), value_d,
                                   std::chars_format::fixed, DECIMAL_PRECISION);
    if (ec == std::errc{})
        return std::string(buf, ptr);

    // Fallback (should never be reached for normal doubles)
    std::ostringstream out;
    out.precision(DECIMAL_PRECISION);
    out << std::fixed << value_d;
    return out.str();
}

static void upwardCompatibility(t_mapParamData& component)
{
    const std::string model = CairnUtils::getParamValue(component, "ModelType");

    if (model == "Storage") {
        CairnUtils::setParamValue(component, "ModelType", "StorageGen");
    }
    else if (model == "Source"    || model == "Load"        ||
             model == "PVSource"  || model == "WindSource"  ||
             model == "CSPSource" || model == "SolarHeater")
    {
        CairnUtils::setParamValue(component, "ModelType", "SourceLoad");
    }
    else if (model == "Grid") {
        CairnUtils::setParamValue(component, "ModelType", "GridFree");
    }

    const std::string modelClass = CairnUtils::getParamValue(component, "ModelClass");
    const std::string newModel   = CairnUtils::getParamValue(component, "ModelType");

    if (modelClass.empty() && !newModel.empty()) {
        CairnUtils::setParamValue(component, "ModelClass", newModel);
    }

    cDebug() << "\t - Model \t\t" << newModel
            << "\t - ModelClass \t\t" << CairnUtils::getParamValue(component, "ModelClass");
}


class CAIRNCORESHARED_EXPORT JsonDescription : public CairnObject
{
public:
    JsonDescription(const std::string& vJsonFile);
    ~JsonDescription();

    const std::vector<t_mapUserIndicator>& dynamicIndicators() const { return mDynamicIndicators; }
    const std::vector<std::string>& LabelList() const { return mLabelList; }
    const std::map<std::string, t_mapLabels>& LabelMap()  const { return mLabelMap; }
    const std::string& groupName() const { return mGroupName; }
    const std::string& groupMainNode() const { return mGroupMainNode; }

    const t_mapParamData& TecEcoParamData() const { return mTecEcoParamData; };
    const t_mapParamData& SimulationControlParamData() const { return mSimulationControlParamData; };
    const t_mapParamData& SolverParamData() const { return mSolverParamData; };

    std::vector<t_mapParamData>& CarrierParamDataList() { return mCarrierParamDataList; };
    std::vector<t_mapParamData>& BusParamDataList() { return mBusParamDataList; };
    std::vector<t_mapParamData>& ComponentParamDataList() { return mComponentParamDataList; };

    std::map<std::string, t_mapParamData> extractPortParamData(const std::string& compoName) const;
    std::vector<t_mapGroups> extractGroupData() const;

    std::string CairnVersionJson() const { return mCairnVersionJson; }

protected:
    json mComponentsList;   // full component array (nlohmann::json)
    json mLinksList;        // full link array
    json mGroupsList;       // full group array

    t_mapParamData mTecEcoParamData{};
    t_mapParamData mSimulationControlParamData{};
    t_mapParamData mSolverParamData{}; 

    std::vector<t_mapParamData> mCarrierParamDataList{};
    std::vector<t_mapParamData> mBusParamDataList{};
    std::vector<t_mapParamData> mComponentParamDataList{};

    std::vector<std::string> mLabelList; // reference labels from TecEcoAnalysis
    std::map<std::string, t_mapLabels> mLabelMap;  // component name -> label map

    std::vector<t_mapUserIndicator> mDynamicIndicators;

    std::string mCairnVersionJson{};

    std::string mGroupName{};     // Only case of group
    std::string mGroupMainNode{}; // Only case of group

    // ------------------------------------------------------------------
    // Lookup indices built once in extractJsonData / extractCompoParamData
    // Key = nodeId OR nodeName, Value = index into mComponentsList
    // ------------------------------------------------------------------
    std::unordered_map<std::string, std::size_t> mComponentIndexById;   // nodeId -> index
    std::unordered_map<std::string, std::size_t> mComponentIndexByName; // nodeName -> index

    // Link index: nodeId/nodeName -> list of relevent link indices 
    // Built lazily in extractPortParamData (first call).
    mutable bool mLinkIndexBuilt = false;
    mutable std::unordered_map<std::string, std::vector<std::size_t>> mLinkIndexByNode;

    // ------------------------------------------------------------------
    // Internal helpers
    // ------------------------------------------------------------------

    void extractJsonData(const std::string& aJsonFile);

    void extractCompoParamData();

    void extractUDIndicatorData(const json& indicatorJson);

    void extractParamData(const json& comp, const std::string& a_key,
        t_mapParamData& aMap,
        const std::string& portName = "",
        const std::vector<std::string>& a_excludedKeys = {}) const;

    bool extractParam(const json& p, t_mapParamData& outMap,
                      const std::string& portName = "",
                      const std::vector<std::string>& excludedKeys = {}) const;

    void extractLabels(const json& comp, t_mapLabels& aMap);

    std::string extractComponentType(const json& comp) const;

    std::string getPortVariable(const std::string& compoName,
        const std::string& portName) const;

    std::string getNodeFromId(const std::string& nodeId) const;
    std::string getcomponentCategoryFromId(const std::string& nodeId) const;

    json readJSONFile(const std::string& aFileName);

    std::string read(const json& in, const std::string& id,
                     const std::string& defaultValue = "") const;

    void buildComponentIndex();
    void buildLinkIndex() const;
};

#endif // JsonDescription_H
