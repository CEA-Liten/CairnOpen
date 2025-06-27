#include "GlobalSettings.h"
#include "MultiObjCompo.h"
#include <QDebug>
#include <QDir>
#include <math.h>       /* fabs, log, pow */
#include <iostream>

using Eigen::Map;
using namespace GS ;

MultiObjCompo::MultiObjCompo(QObject *aParent,  
    const QMap<QString, QString>& aComponent,
    const QMap < QString, QMap<QString, QString> >& aPorts,
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
    mCompoInputParam->addParameter("ObjectiveType", &mObjectiveType, "", true, true, "available: Blended = part of the NPV or Add = simply added to objective or Lexicographic = ordered objectives managed by Cplex (see doc)");
}

int MultiObjCompo::initProblem()
{    
    mCompoToModel->publishData("ObjectiveType", &mObjectiveType);    
    return MilpComponent::initProblem();
}

