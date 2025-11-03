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
    TecEcoEnv& aTecEcoEnv, ModelFactory* aModelFactory) :
    BusCompo(aParent, aComponent, aPorts, aMilpData, aTecEcoEnv, aModelFactory)
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


