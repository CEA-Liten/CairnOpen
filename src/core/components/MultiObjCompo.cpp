#include "GlobalSettings.h"
#include "MultiObjCompo.h"
#include <math.h>       /* fabs, log, pow */
#include <iostream>

using Eigen::Map;
using namespace GS ;

MultiObjCompo::MultiObjCompo(CairnObject *aParent, 
    const std::string& aName,
    const t_mapParamData& aComponent,
    const std::map < std::string, t_mapParamData>& aPorts,
    MilpData* aMilpData,
    TecEcoAnalysis* aTecEcoAnalysis, ModelFactory* aModelFactory) 
    : BusCompo(aParent, aName, aComponent, aPorts, aMilpData, aTecEcoAnalysis, aModelFactory)
{       
} 

//------------------------------------------------------------------------------
MultiObjCompo::~MultiObjCompo()
{
} 

void MultiObjCompo::declareCompoInputParam()
{
    BusCompo::declareCompoInputParam();
}


