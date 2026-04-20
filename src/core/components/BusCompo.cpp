
#include "BusCompo.h"

#include <math.h>     
#include <iostream>

#include "GlobalSettings.h"
#include "CairnUtils.h"


using namespace GS;
using namespace CairnUtils;

using Eigen::Map;

BusCompo::BusCompo(CairnObject* aParent, const std::map<std::string, std::string>& aComponent,
    const std::map < std::string, std::map<std::string, std::string> >& aPorts, 
    MilpData* aMilpData, TecEcoAnalysis* aTecEcoAnalysis, ModelFactory* aModelFactory) :
    MilpComponent(aParent, CairnUtils::getParam(aComponent,"id"), aMilpData, aTecEcoAnalysis, aComponent, aPorts, aModelFactory)
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

void BusCompo::setCompoInputParam(const std::map<std::string, std::string> aComponent)
{
    MilpComponent::setCompoInputParam(aComponent);
}

std::string BusCompo::CarrierName() const {
    if (getMainCarrier()) {
        return getMainCarrier()->Name();
    }
    return "";
}

int BusCompo::checkConnections()
{
    const std::vector<MilpPort*>& linkedPorts = LinkedPorts();
    if (linkedPorts.empty())
        return 0;

    int iIn = 0, iOut = 0, iData = 0;
    const std::string& busUnit = (mType != "BusSameValue") ? linkedPorts.front()->FluxUnit() : "";

    for (const MilpPort* port : linkedPorts)
    {
        // Verify all connected ports share the same Flux unit
        if (mType != "BusSameValue")
        {
            const std::string& portUnit = port->FluxUnit();
            if (portUnit != busUnit)
            {
                cCritical() << "Unit mismatch on Bus" << Name()
                    << "- port" << port->Name()
                    << "has Flux unit" << portUnit
                    << "but expected" << busUnit;
                return -1;
            }
        }

        // Count port directions
        const std::string& dir = port->Direction();
        if (dir == KDATA()) iData++;
        else if (dir == KCONS()) iIn++;
        else if (dir == KPROD()) iOut++;
    }

    if (mCompoModelName == "BusFlowBalance" && iData == 0 && (iIn == 0 || iOut == 0))
    {
        cCritical() << "ERROR on component" << Name()
            << "- input ports:" << iIn
            << ", output ports:" << iOut
            << ", data exchange ports:" << iData
            << ". Expected at least one input and one output port, or a data exchange port!";
        return -1;
    }

    return 0;
}

int BusCompo::checkPorts()
{
    if (!getMainCarrier()) {
        Cairn_Exception error("Critical ERROR : Missing carrier for Bus " + Name(), -1);
        throw error;
    }

    // Check Bus own ports
    int ierr = MilpComponent::checkPorts();
    if (ierr < 0) return ierr;

    // Check conncetions
    ierr = checkConnections();
    if (ierr < 0) return ierr;

    return 0;
}

void BusCompo::createPortsExportListVars(t_mapExchange& a_Exchange) 
{
    /* Bus port variables should not be published because they are a copy of linked component variables */
}

std::string BusCompo::ObjectiveType() const {
    return mCompoModel->ObjectiveType(); // TODO: move ObjectiveType() from SubModel to BusSubModel
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
    if (linkedComponent) {
        std::vector<MilpComponent*>::iterator vIter = find(mListComponent.begin(), mListComponent.end(), linkedComponent);
        if (vIter != mListComponent.end()) {
            mListComponent.erase(vIter);
        }
    }

    busModel()->removeLink(linkedPort);
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