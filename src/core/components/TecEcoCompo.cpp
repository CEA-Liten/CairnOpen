#include "TecEcoCompo.h"
#include "GlobalSettings.h"

using namespace GS ;
using Eigen::Map;

TecEcoCompo::TecEcoCompo(CairnObject *aParent,
    const std::map<std::string, std::string>& aComponent,
    const std::map < std::string, std::map<std::string, std::string> >& aPorts,
    MilpData* aMilpData,
    ModelFactory* aModelFactory) :
    MilpComponent(aParent, CairnUtils::getParam(aComponent,"id"), aMilpData, nullptr, aComponent, aPorts, aModelFactory)
{
    setObjectType("TecEcoCompo");
    if (CairnUtils::getParam(aComponent, "id").empty()) {
        setObjectName("TecEco");
    }
    mType = "TecEcoAnalysis";
    mCompoModelName = "TecEcoAnalysis";
    mCompoTechnoType = "TecEcoAnalysis";
    mCompoModelClassName = "TecEcoAnalysis";
}

TecEcoCompo::~TecEcoCompo()
{
} 

bool TecEcoCompo::newCompoModel()
{
    mCompoModel = new TecEcoAnalysis(this, mComponent);
    return (mCompoModel != nullptr);
}

void TecEcoCompo::declareCompoInputParam()
{
}

void TecEcoCompo::setCompoInputParam(const std::map<std::string, std::string> aComponent)
{
}

void TecEcoCompo::redeclareEnvImpactParameters()
{

}

std::vector<InputParam*> TecEcoCompo::get_InputParams()
{
    std::vector<InputParam*> result;
    result.reserve(5);   // avoid reallocations

    TecEcoAnalysis* vTecEcoAnalysis = (TecEcoAnalysis*)mCompoModel;

    if (vTecEcoAnalysis) {
        result.push_back(vTecEcoAnalysis->getConfigParam());
        result.push_back(vTecEcoAnalysis->getCompoInputParam());
        result.push_back(vTecEcoAnalysis->getCompoInputSettings());
        result.push_back(vTecEcoAnalysis->getCompoEnvImpactsParam());
    }

    // Add GUI parameters if available
    if (auto* gui = getGUIData()) {
        result.push_back(gui->getGuiInputParam());
    }

    return result;
}

std::vector<InputParam*> TecEcoCompo::get_ParamInputParams()
{
    std::vector<InputParam*> result;
    TecEcoAnalysis* vTecEcoAnalysis = (TecEcoAnalysis*)mCompoModel;
    if (vTecEcoAnalysis) {
        result.push_back(vTecEcoAnalysis->getConfigParam());
        result.push_back(vTecEcoAnalysis->getCompoInputSettings());
    }
    return result;
}

std::vector<InputParam*> TecEcoCompo::get_OptionInputParams()
{
    std::vector<InputParam*> result;
    TecEcoAnalysis* vTecEcoAnalysis = (TecEcoAnalysis*)mCompoModel;
    if (vTecEcoAnalysis) {
        result.push_back(vTecEcoAnalysis->getCompoInputParam());
    }
    return result;
}

std::vector<InputParam*> TecEcoCompo::get_EnvImpactInputParams()
{
    std::vector<InputParam*> result;
    TecEcoAnalysis* vTecEcoAnalysis = (TecEcoAnalysis*)mCompoModel;
    if (vTecEcoAnalysis) {
        result.push_back(vTecEcoAnalysis->getCompoEnvImpactsParam());
    }
    return result;
}

std::string TecEcoCompo::EnvImpactShortName(const std::string& name) const
{
    TecEcoAnalysis* vTecEcoAnalysis = (TecEcoAnalysis*)mCompoModel;
    if (vTecEcoAnalysis) {
        return vTecEcoAnalysis->EnvImpactShortName(name);
    }
    return name;
}