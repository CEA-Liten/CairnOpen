#include "SourceLoadCompo.h"
#include <math.h>       /* fabs, log, pow */
#include <iostream>
#include "GlobalSettings.h"

using namespace GS ;
using Eigen::Map;

SourceLoadCompo::SourceLoadCompo(CairnObject *aParent,
    const std::string& aName,
    const t_mapParamData& aComponent,
    const std::map < std::string, t_mapParamData>& aPorts,
    MilpData* aMilpData,
    TecEcoAnalysis* aTecEcoAnalysis,
    ModelFactory* aModelFactory) 
    : MilpComponent(aParent, aName, aMilpData, aTecEcoAnalysis, aComponent, aPorts, aModelFactory)
{
}

SourceLoadCompo::~SourceLoadCompo()
{
} 

void SourceLoadCompo::declareCompoInputParam()
{
    MilpComponent::declareCompoInputParam();
}

void SourceLoadCompo::setCompoInputParam(const t_mapParamData& aComponent)
{
    MilpComponent::setCompoInputParam(aComponent);
}

