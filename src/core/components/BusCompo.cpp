
#include "BusCompo.h"

#include <math.h>     
#include <iostream>

#include "GlobalSettings.h"
#include "CairnUtils.h"


using namespace GS;
using namespace CairnUtils;

using Eigen::Map;

BusCompo::BusCompo(CairnObject* aParent, 
    const std::string& aName,
    const t_mapParamData& aComponent,
    const std::map < std::string, t_mapParamData>& aPorts,
    MilpData* aMilpData, TecEcoAnalysis* aTecEcoAnalysis, ModelFactory* aModelFactory) 
    : MilpComponent(aParent, aName, aMilpData, aTecEcoAnalysis, aComponent, aPorts, aModelFactory)
{    
    setObjectType("BusCompo");
}

BusCompo::~BusCompo()
{
}

void BusCompo::declareCompoInputParam()
{
    MilpComponent::declareCompoInputParam(); //Common component input param
}

void BusCompo::setCompoInputParam(const t_mapParamData& aComponent)
{
    MilpComponent::setCompoInputParam(aComponent);
}

std::string BusCompo::CarrierName() const {
    if (getMainCarrier()) {
        return getMainCarrier()->Name();
    }
    return "";
}

void BusCompo::createPortsExportListVars(t_mapExchange& a_Exchange) 
{
    /* Bus port variables should not be published because they are a copy of linked component variables */
}

std::string BusCompo::ObjectiveType() const {
    return busModel()->ObjectiveType(); 
}

std::vector<std::string> BusCompo::getPossibleObjectiveTypes() const
{
    try {
        return busModel()->getPossibleObjectiveTypes();
    }
    catch (Cairn_Exception& cairn_error) {
        return {};
    }
}

std::vector<InputParam*> BusCompo::get_InputParams()
{
    std::vector<InputParam*> result;
    result.reserve(4);   // avoid reallocations

    // Always available
    result.push_back(getCompoInputParam());

    // Component model, if available
    if (auto* model = compoModel()) {
        result.push_back(model->getInputParam());
        //result.push_back(model->getInputTimeSeries());
    }

    // GUI data, if available
    if (auto* gui = getGUIData()) {
        result.push_back(gui->getGuiInputParam());
    }

    return result;
}

std::vector<InputParam*> BusCompo::get_TimeSeriesInputParams()
{
    return {};
}

std::vector<InputParam*> BusCompo::get_EnvImpactInputParams()
{
    return {};
}

std::vector<InputParam*> BusCompo::get_PortEnvImpactInputParams()
{
    return {};
}

BusSubModel* BusCompo::busModel() const {
    BusSubModel* busModel = dynamic_cast<BusSubModel*> (mCompoModel);
    if (!busModel) {
        throw Cairn_Exception("The model of Bus " + Name() + " is not defined!", -1);
    }
    return busModel;
}

const std::vector<MilpPort*>& BusCompo::LinkedPorts() const {
    return busModel()->LinkedPorts();
}

void BusCompo::addLink(MilpComponent* linkedComponent, MilpPort* linkedPort)
{
    // linkedComponent is the parent of linkedPort !
    mListComponent.push_back(linkedComponent);

    busModel()->addLink(linkedPort, this);
    linkedPort->setPortType(mType);
}

void BusCompo::removeLink(MilpComponent* linkedComponent, MilpPort* linkedPort)
{
    // Defensive: linkedPort must not be null 
    if (!linkedPort) {
        cError() << "BusCompo::removeLink called with null linkedPort.";
        return;
    }

    // Remove component from the list 
    if (!linkedComponent)
        return;

    auto it = std::find(mListComponent.begin(), mListComponent.end(), linkedComponent);
    if (it != mListComponent.end()) {
        mListComponent.erase(it);
    }

    // Defensive: busModel() must be valid 
    auto* model = busModel();
    if (!model) {
        cError() << "BusCompo::removeLink: busModel() is null.";
        return;
    }

    // Remove port link in the bus model 
    model->removeLink(linkedPort);
}

int BusCompo::NbPorts(const std::string& aDirection)
{
    if (aDirection != KDATA() && aDirection != KCONS() && aDirection != KPROD())
        return 0;

    if (aDirection == "") {
        return PortList().size() + LinkedPorts().size();
    }
    else {
        int num = 0;

        for (MilpPort* port : PortList()) {
            if (port->Direction() == aDirection) {
                num++;
            }
        }

        std::string busDirection = KDATA();
        if (aDirection == KCONS())
            busDirection = KPROD();
        else if (aDirection == KPROD())
            busDirection = KCONS();

        for (MilpPort* linkedPort : LinkedPorts()) {
            if (linkedPort->Direction() == busDirection) {
                num++;
            }
        }

        return num;
    }
}

vector<MilpPort*> BusCompo::listSidePorts(const std::string& aside)
{
    std::vector<MilpPort*> portList;

    // Own ports
    for (MilpPort* port : PortList())
    {
        const std::string portPosition = port->Position();
        if (portPosition == aside) {
            portList.push_back(port);
        }
        else if (portPosition.empty() && aside == Bottom()) {
            //add to bottom-side if position is not defined
            portList.push_back(port);
        }
    }

    // Ports used for connections
    for (MilpPort* linkedPort : LinkedPorts())
    {
        const std::string busPortPosition = linkedPort->BusPortPosition();
        if (busPortPosition == aside) {
            portList.push_back(linkedPort);
        }
        else if (busPortPosition.empty() && aside == Bottom()) {
            //add to bottom-side if position is not defined
            portList.push_back(linkedPort);
        }
    }

    return portList;
}

void BusCompo::jsonSaveGUIlistPortsData(ojson& nodePortArray, const std::string& aSide)
{
    // Own ports
    for (MilpPort* port : PortList()) {
        if (port->Position() == aSide) {
            port->jsonSaveGUIPortsData(nodePortArray);
        }
    }

    // Ports used for connections
    for (MilpPort* linkedPort : LinkedPorts()) {
        if (linkedPort->BusPortPosition() == aSide) {
            linkedPort->jsonSaveGUIPortsData(nodePortArray, true);
        }
    }
}