/*
* \file		BusSubModel.cpp
* \brief	BusSubModel is a specialized SubModel implementation representing a bus.
* \version	1.0
* \author	Ali KASSEM
* \date		13/09/2024
*/

#include "BusSubModel.h"
#include "BusCompo.h"

// --- Construction -------------------------------------------------------------

BusSubModel::BusSubModel(CairnObject* aParent)
    : SubModel(aParent)
    , mObjectiveType({})
{
}

// --- SubModel interface -------------------------------------------------------------

int BusSubModel::checkPorts()
{
    if (LinkedPorts().empty()) {
        cError() << Name() << ": bus must have at least one link.";
        return -1;
    }

    if (!getMainCarrier())
        throw Cairn_Exception("Missing carrier for Bus " + Name(), -1);

    if (SubModel::checkPorts() < 0)
        return -1;

    if (checkConnections() < 0)
        return -1;

    return 0;
}

int BusSubModel::checkConsistency()
{
    return SubModel::checkConsistency();
}

void BusSubModel::buildModel()
{
    if (mAllocate)
        allocateExpressions();
    else
        closeExpressions();

    computeModelContribution(); // compute expressions, add variables and constraints
    mAllocate = false;
}

// --- Bus-specific -------------------------------------------------------------

std::vector<std::string> BusSubModel::getPossibleObjectiveTypes() const
{
    if (ModelClassName() == "ManualObjective")
        return { "Add", "Lexicographic", "" };
    return {};
}

// --- Linked ports -------------------------------------------------------------

void BusSubModel::addLink(MilpPort* linkedPort, BusCompo* parentBus)
{
    if (!linkedPort)
    {
        cWarning() << "addLink: null port passed to Bus" << Name();
        return;
    }
    mLinkedPorts.push_back(linkedPort);
    linkedPort->setLinkedBus(parentBus); // parnetBus == this->parent();
}

void BusSubModel::removeLink(MilpPort* linkedPort)
{
    if (!linkedPort)
    {
        cWarning() << "removeLink: null port passed to Bus" << Name();
        return;
    }

    const auto it = std::find(mLinkedPorts.cbegin(), mLinkedPorts.cend(), linkedPort);
    if (it == mLinkedPorts.cend())
    {
        cWarning() << "removeLink: port" << linkedPort->Name()
            << "not found in Bus" << Name();
        return;
    }

    mLinkedPorts.erase(it);
    linkedPort->setLinkedBus(nullptr);
}

int BusSubModel::checkConnections()
{
    const std::vector<MilpPort*>& linkedPorts = LinkedPorts();

    if (linkedPorts.empty()) {
        cError() << Name() << ": bus must have at least one connection.";
        return -1;
    }

    // Verify all connected ports share the same Flux unit
    auto effectiveDirection = [&](const MilpPort* port) -> std::string
        {
            const std::string& dir = port->Direction();
            const double coeff = port->VarCoeff();

            if (coeff >= 0.0)
                return dir;

            // Negative coefficient -> invert CONS <=> PROD
            if (dir == KCONS()) return KPROD();
            if (dir == KPROD()) return KCONS();

            return dir; // DATA or anything else stays unchanged
        };

    const std::string& busUnit = linkedPorts.front()->FluxUnit();
    int iIn = 0, iOut = 0, iData = 0;

    auto* comp = parentComponent();

    for (const MilpPort* port : linkedPorts)
    {
        if (comp && comp->Type() != "MultiObjCompo") {
            const std::string& portUnit = port->FluxUnit();
            if (portUnit != busUnit)
            {
                cError() << "Unit mismatch on Bus" << Name()
                    << "- port" << port->Name()
                    << "has Flux unit" << portUnit
                    << "but expected" << busUnit
                    << "(unit of first linked port)";
                return -1;
            }
        }

        const std::string effDir = effectiveDirection(port);

        if (effDir == KDATA()) {
            iData++;
        }
        else if (effDir == KCONS()) {
            iIn++;
        }
        else if (effDir == KPROD()) {
            iOut++;
        }
    }

    if (iData == 0 && (iIn == 0 || iOut == 0))
    {
        cWarning() << "Connection error on Bus " << Name()
            << "- input ports: " << iIn
            << ", output ports: " << iOut
            << ", data exchange ports: " << iData
            << "- expected at least one input and one output port, or one data exchange port";
    }

    return 0;
}

