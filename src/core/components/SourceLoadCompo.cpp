#include "SourceLoadCompo.h"
#include <math.h>       /* fabs, log, pow */
#include <iostream>
#include "GlobalSettings.h"

using namespace GS ;
using Eigen::Map;

SourceLoadCompo::SourceLoadCompo(CairnObject *aParent,
    const std::map<std::string, std::string>& aComponent,
    const std::map < std::string, std::map<std::string, std::string> >& aPorts,
    MilpData* aMilpData,
    TecEcoAnalysis* aTecEcoAnalysis,
    ModelFactory* aModelFactory) :
    MilpComponent(aParent, CairnUtils::getParam(aComponent,"id"), aMilpData, aTecEcoAnalysis, aComponent, aPorts, aModelFactory)
{
}

SourceLoadCompo::~SourceLoadCompo()
{
} 

void SourceLoadCompo::declareCompoInputParam()
{
    MilpComponent::declareCompoInputParam();
}

void SourceLoadCompo::setCompoInputParam(const std::map<std::string, std::string> aComponent) 
{
    MilpComponent::setCompoInputParam(aComponent);
}

