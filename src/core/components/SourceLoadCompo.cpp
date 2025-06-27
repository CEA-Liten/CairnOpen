#include "SourceLoadCompo.h"
#include <QDebug>
#include <QDir>
#include <math.h>       /* fabs, log, pow */
#include <iostream>
#include "GlobalSettings.h"

using namespace GS ;
using Eigen::Map;

SourceLoadCompo::SourceLoadCompo(QObject *aParent,
    const QMap<QString, QString>& aComponent,
    const QMap < QString, QMap<QString, QString> >& aPorts,
    MilpData* aMilpData,
    TecEcoEnv& aTecEcoEnv,
    ModelFactory* aModelFactory) :
    MilpComponent(aParent, aComponent["id"], aMilpData, aTecEcoEnv, aComponent, aPorts, aModelFactory)
{
}

SourceLoadCompo::~SourceLoadCompo()
{
} 

void SourceLoadCompo::declareCompoInputParam()
{
    MilpComponent::declareCompoInputParam();
    mCompoInputParam->addParameter("Direction", &mDirection, "Sink", true, true, "Direction of the SourceLoad component - Sink means extraction of power or mass from the system - Source means injection in the system");    
}

void SourceLoadCompo::setCompoInputParam(const QMap<QString, QString> aComponent) 
{
    MilpComponent::setCompoInputParam(aComponent);

    if (mDirection == "Sink") {
        mSens = -1;

    }
    else if (mDirection == "Source") {
        mSens = +1;
    }
    else
    {
        mSens = 0;
        qCritical() << "ERROR : SourceLoad component /** ";
        Cairn_Exception erreur("Invald <Direction> attribute : Sink or Source expected " + mName + " instead of " + mDirection + ".", -1);
        throw& erreur;
    }
}

int SourceLoadCompo::initProblem()
{
    mCompoToModel->publishData("Direction", &mSens);    
    return MilpComponent::initProblem();
}
