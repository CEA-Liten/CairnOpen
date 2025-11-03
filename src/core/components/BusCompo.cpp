#include "BusCompo.h"
#include "TechnicalSubModel.h"
#include <math.h>       /* fabs, log, pow */
#include <iostream>
#include "GlobalSettings.h"
#include "CairnUtils.h"
using namespace CairnUtils;

using namespace GS ;

using Eigen::Map;

BusCompo::BusCompo(CairnObject* aParent, const std::map<std::string, std::string>& aComponent,
    const std::map < std::string, std::map<std::string, std::string> >& aPorts, 
    MilpData* aMilpData, TecEcoEnv &aTecEcoEnv, ModelFactory* aModelFactory) :
    MilpComponent(aParent, CairnUtils::getParam(aComponent,"id"), aMilpData, aTecEcoEnv, aComponent, aPorts, aModelFactory)
{    
    setObjectType("BusCompo");
}

BusCompo::~BusCompo()
{
}

void BusCompo::declareCompoInputParam()
{
    MilpComponent::declareCompoInputParam(); //Common component input param
    mCompoInputParam->addParameter("VectorName", &mVectorName, "", true, true, "VectorName", "string", "DONOTSHOW");
}

void BusCompo::setCompoInputParam(const std::map<std::string, std::string> aComponent)
{
    MilpComponent::setCompoInputParam(aComponent);

    if (mVectorName == "") {
        cCritical() << "Critical ERROR : Missing carried VectorName specified for Bus " << Name();
        Cairn_Exception erreur("Invalid void <Vector> name :  " + mVectorName + " for " + Name(), -1);
        throw& erreur;
    }
    assert(mVectorName != "");
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

    for (MilpPort* port : PortList()) {    
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
       cCritical() << " ERROR on component " << Name();
       cCritical() << " Found consumer Ports : " << mNbInputPorts ;
       cCritical() << " Found producer Ports : " << mNbOutputPorts ;
       cCritical() << " Found data exchange Ports : " << mNbDataPorts ;
       cCritical() << " You should have at least one consumer and one producer ! " ;
       return -1 ;
     }

    return ierr ;
}

int BusCompo::checkPorts()
{
    int ierr = 0 ;
    if (mType != "BusSameValue") {
        /** Verify that all the connected ports have the same Unit */
        std::string busUnit = "none";        
        for (MilpPort* port : PortList()) {
            std::string varFluxUnit = port->FluxUnit();
            if (busUnit == "none") {
                busUnit = varFluxUnit;
            }
            else if (busUnit != varFluxUnit) {
                cCritical() << ("ERROR at port " + port->Name() + " of Bus " + Name() + ". The port Flux unit is " + varFluxUnit);
                cCritical() << ("But, another port of the same Bus is using Flux unit " + busUnit);
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
        std::vector<MilpComponent*>::iterator vIter = find(mListComponent.begin(), mListComponent.end(), lptr);
        if (vIter != mListComponent.end()) {
            mListComponent.erase(vIter);
        }        
    }
}

void BusCompo::exportPortResults(t_mapExchange& a_Export, uint modinitTS) {

}

void BusCompo::createPortsExportListVars(t_mapExchange& a_Exchange) 
{
    /* Bus port variables should not be published because they are a copy of linked component variables */
}

int BusCompo::NbPorts(const std::string& aDirection)
{
    if (aDirection == "") {
        return PortList().size();
    }
    else {
        int num = 0;
        for (MilpPort* lptrport : PortList()) {        
            if (lptrport->Direction() == aDirection) {
                num++;
            }
        }
        return num;
    }
}

vector<MilpPort*> BusCompo::listSidePorts(const std::string& aside)
{
    std::vector<MilpPort*> ptrlist;
    for(MilpPort * lptrport : PortList())
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

void BusCompo::jsonSaveGUIlistPortsData(ojson& nodePortArray, const std::string& aSide)
{
    for (MilpPort* port : PortList()) {    
        if (port->BusPortPosition() == aSide) {
            port->jsonSaveGUIPortsData(nodePortArray, true);
        }
    }
}
