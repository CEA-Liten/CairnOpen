#include "BusCompo.h"
#include "TechnicalSubModel.h"
#include <QDebug>
#include <QDir>
#include <math.h>       /* fabs, log, pow */
#include <iostream>
#include "GlobalSettings.h"
#include "CairnUtils.h"
using namespace CairnUtils;

using namespace GS ;

using Eigen::Map;

BusCompo::BusCompo(QObject* aParent, const QMap<QString, QString>& aComponent,
    const QMap < QString, QMap<QString, QString> >& aPorts, 
    MilpData* aMilpData, TecEcoEnv &aTecEcoEnv, ModelFactory* aModelFactory) :
    MilpComponent(aParent, aComponent["id"], aMilpData, aTecEcoEnv, aComponent, aPorts, aModelFactory)
{    
}

BusCompo::~BusCompo()
{
}

void BusCompo::declareCompoInputParam()
{
    MilpComponent::declareCompoInputParam(); //Common component input param
    mCompoInputParam->addParameter("VectorName", &mVectorName, "", true, true, "VectorName", "string", {"DO NOT SHOW"});
}

void BusCompo::setCompoInputParam(const QMap<QString, QString> aComponent)
{
    MilpComponent::setCompoInputParam(aComponent);

    if (mVectorName == "") {
        qCritical() << "Critical ERROR : Missing carried VectorName specified for Bus " << (mCompoInputParam->getParamQSValue("id"));
        Cairn_Exception erreur("Invalid void <Vector> name :  " + mVectorName + " for " + mName, -1);
        throw& erreur;
    }
    Q_ASSERT(mVectorName != "");
}

void BusCompo::DeleteBusPort(MilpPort* lptrport)
{
    if (lptrport != nullptr) {
        mCompoModel->removeBusPort(lptrport);
    }
}

void BusCompo::addPort(MilpPort* lptrport)
{
    MilpComponent::addPort(lptrport); /** Add self-defined port from component connections onto Bus */
    lptrport->setPortType(mType);
}

int BusCompo::initPorts()
{   
    /** Initialize Bus ports of mListPort then of mBusOwnPorts list */
    int ierr = 0 ;
    int iIn = 0 ;
    int iOut = 0 ;
    int iData = 0 ;
    int numport = 0 ;

    MilpPort* port ;
    QListIterator<MilpPort*> iport(PortList());
    while (iport.hasNext())
    {
       port = iport.next() ;

       ierr = port->initProblem(npdt()) ;
       if (ierr <0) return ierr ;

       if (port->Direction() == KPROD()) iOut++ ;
       if (port->Direction() == KCONS()) iIn++ ;
       if (port->Direction() == KDATA()) iData++ ;

       numport++ ;
    }

    mNbInputPorts = iIn ;
    mNbOutputPorts = iOut ;
    mNbDataPorts = iData ;

    if (mCompoModelName == "BusFlowBalance" && mNbDataPorts == 0 && (mNbOutputPorts == 0 || mNbInputPorts == 0))
    {
       qCritical() << " ERROR on component " << Name() ;
       qCritical() << " Found consumer Ports : " << mNbInputPorts ;
       qCritical() << " Found producer Ports : " << mNbOutputPorts ;
       qCritical() << " Found data exchange Ports : " << mNbDataPorts ;
       qCritical() << " You should have at least one consumer and one producer ! " ;
       return -1 ;
     }

    return ierr ;
}

int BusCompo::checkPorts()
{
    int ierr = 0 ;
    if (mType != "BusSameValue") {
        /** Verify that all the connected ports have the same Unit */
        QString busUnit = "none";        
        MilpPort* port;
        QListIterator<MilpPort*> iport(PortList());
        while (iport.hasNext())
        {
            port = iport.next();
            QString varFluxUnit = port->getFluxUnit();
            if (busUnit == "none") {
                busUnit = varFluxUnit;
            }
            else if (busUnit != varFluxUnit) {
                qCritical() << ("ERROR at port " + port->Name() + " of Bus " + Name() + ". The port Flux unit is " + varFluxUnit);
                qCritical() << ("But, another port of the same Bus is using Flux unit " + busUnit);
                return -1;
            }
        }
    }
    return ierr ;
}

void BusCompo::setBusFluxPortExpression(const double &aSignedCoefficient)
{
    // do nothing
}
void BusCompo::setBusSameValuePortExpression()
{
    // do nothing
}

void BusCompo::initSubModelTopology()
{
    //mCompoModel->setTopo(mListPort) ;
    mCompoModel->setParentCompo(this) ;
}

void BusCompo::addComponent(MilpComponent* lptr)
{
    mListComponent.push_back(lptr);         /** Add component connected onto Bus */
}

void BusCompo::RemoveLinkComponent(MilpComponent* lptr)
{
    if (lptr != nullptr)
    {
        mListComponent.removeOne(lptr);
    }
}

void BusCompo::exportPortResults(t_mapExchange& a_Export, uint modinitTS) {

}

void BusCompo::createPortsExportListVars(t_mapExchange& a_Exchange) {

}

int BusCompo::NbPorts(const QString& aDirection)
{
    if (aDirection == "") {
        return PortList().size();
    }
    else {
        int num = 0;
        foreach(MilpPort * lptrport, PortList())
        {
            if (lptrport->Direction() == aDirection) {
                num++;
            }
        }
        return num;
    }
}

QList<MilpPort*> BusCompo::listSidePorts(const QString& aside)
{
    QList<MilpPort*> ptrlist;
    foreach(MilpPort * lptrport, PortList())
    {
        if (lptrport->BusPortPosition() == aside) {
            ptrlist.push_back(lptrport);
        }
        if (lptrport->BusPortPosition() == "" && aside == Bottom()) {
            //add to bottom-side if position is not defined
            ptrlist.push_back(lptrport);
        }
    }
    return ptrlist;
}

void BusCompo::jsonSaveGUIlistPortsData(QJsonArray& nodePortArray, const QString& aSide)
{
    MilpPort* port;
    QListIterator<MilpPort*> iport(PortList());
    while (iport.hasNext())
    {
        port = iport.next();
        if (port->BusPortPosition() == aSide) {
            port->jsonSaveGUIPortsData(nodePortArray, true);
        }
    }
}
