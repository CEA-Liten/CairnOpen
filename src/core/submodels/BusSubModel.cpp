
#include "BusSubModel.h"

BusSubModel::BusSubModel(CairnObject* aParent) :
    SubModel(aParent),
    mObjectiveType("")
{
}

BusSubModel::~BusSubModel(){
}

void BusSubModel::computeDefaultIndicators(const double* optSol)
{
}

void BusSubModel::buildModel()
{
    if (mAllocate) {
        allocateExpressions();
    }
    else {
        closeExpressions();
    }

    /** compute expressions, add variables and add constraints */
    computeModelContribution();

    mAllocate = false;
}

void BusSubModel::addLink(MilpPort* linkedPort, BusCompo* parnetBus)
{
    if (!linkedPort) return;

    mLinkedPorts.push_back(linkedPort);

    linkedPort->setLinkedBus(parnetBus); // parnetBus == this->parent();
}

void BusSubModel::removeLink(MilpPort* linkedPort)
{
    if (!linkedPort) return;

    std::vector<MilpPort*>::iterator vIter = find(mLinkedPorts.begin(), mLinkedPorts.end(), linkedPort);
    if (vIter != mLinkedPorts.end()) {
        mLinkedPorts.erase(vIter);
    }

    linkedPort->setLinkedBus(nullptr);
}

std::vector<std::string> BusSubModel::getPossibleObjectiveTypes() const {
    if (ModelClassName() == "ManualObjective")
        return { "Add", "Lexicographic", ""};
    else
        return {};
};
