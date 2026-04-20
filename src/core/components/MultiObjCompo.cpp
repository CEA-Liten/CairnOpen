#include "GlobalSettings.h"
#include "MultiObjCompo.h"
#include <math.h>       /* fabs, log, pow */
#include <iostream>

using Eigen::Map;
using namespace GS ;

MultiObjCompo::MultiObjCompo(CairnObject *aParent,  
    const std::map<std::string, std::string>& aComponent,
    const std::map < std::string, std::map<std::string, std::string> >& aPorts,
    MilpData* aMilpData,
    TecEcoAnalysis* aTecEcoAnalysis, ModelFactory* aModelFactory) :
    BusCompo(aParent, aComponent, aPorts, aMilpData, aTecEcoAnalysis, aModelFactory)
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


