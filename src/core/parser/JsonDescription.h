#ifndef JsonDescription_H
#define JsonDescription_H
class JsonDescription ;

#include "CairnCore_global.h"
#include "Cairn_Exception.h"
#include "CairnUtils.h"

#include <sstream>
#include <cstdint>

#define DECIMAL_PRECISION 9
#define SIGNIFICANT_DIGITS 12

static std::string doubleToString(const double& value_d) {
    std::ostringstream out;
    out.precision(DECIMAL_PRECISION);
    out << std::fixed << value_d;
    return std::move(out).str();

    //
    //int precision = int(DECIMAL_PRECISION);
    //int sig_digits = int(SIGNIFICANT_DIGITS);
    //double value_f = std::to_string(value_d, 'f', precision).toDouble();
    //std::string value_s = std::to_string(value_f, 'g', sig_digits);
    //return value_s;
}

static void upwardCompatibility(t_mapParams& component)
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
    cInfo() << "\t - Model \t\t" << component["Model"] << "\t - ModelClass \t\t" << component["ModelClass"];
}



class CAIRNCORESHARED_EXPORT JsonDescription : public CairnObject
{

public:
    JsonDescription(CairnObject* aParent, std::string aName);
    virtual ~JsonDescription();

    std::vector< t_mapParams > parseJsonDescription(const std::string& aDescFile);
    const std::vector< t_mapParams >& dynamicIndicators() { return mDynamicIndicators; }

    std::map < std::string, std::map<std::string, std::string> > getCompoPortData(const std::string& compoName) const;

    Cairn_Exception  getException() const { return mException; }
    void  setException(const Cairn_Exception& aException) { mException = aException; }

    const std::vector< std::string >& LabelList() const { return mLabelList; };
    const std::map<std::string, t_mapParams >& LabelMap() const { return mLabelMap; };

protected:
    Cairn_Exception mException ;

    json mComponentsList;
    json mLinksList;

    std::vector< t_mapParams > mComponents = {};
    std::vector< t_mapParams > mDynamicIndicators = {};

    std::vector< std::string > mLabelList = {}; /* existing labels as specified by TecEcoAnalysis (acts as a reference) */
    std::map<std::string, t_mapParams > mLabelMap = {}; /* key is the componenet name, and t_mapParams is labelName: labelValue (only labels from mLabelNames are accepted at the end) */

    void extractDocumentData(const json& jsonData);

    void extractComponentData(const json& comp);
    void extractUDIndicatorData(const json& indicatorJson);
        
    void extractParamData(const json& comp, const std::string& a_key, t_mapParams& aMap, const std::vector<std::string>& a_excludedKeys = {});    

    std::string getNodeFromId(const std::string&nodeId) const;
    std::string getcomponentCategoryFromId(const std::string &nodeId) const;
    
    json readJSONFile(const std::string& aFileName);

    std::string read(const json& in, const std::string& id, const std::string& defaultValue = "") const;
};

#endif // JsonDescription_H
