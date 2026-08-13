/*
* \file		BusSubModel.h
* \brief	BusSubModel is a specialized SubModel implementation representing a bus. 
* \version	1.0
* \author	Ali KASSEM
* \date		13/09/2024
*/

#ifndef BUSSUBMODEL_H
#define BUSSUBMODEL_H

#include <string>
#include <vector>

#include "SubModel.h"

class BusCompo; 
class MilpPort;

class CAIRNCORESHARED_EXPORT BusSubModel : public SubModel
{
public:
    explicit BusSubModel(CairnObject* aParent);

    // --- SubModel interface -------------------------------------------------
    int  checkPortCount()   override { return 0; }
    int  checkPorts()       override;
    int  checkConsistency() override;
    void buildModel()       override final; /** Sealed - models must not override buildModel() */

    void initDefaultPorts() override {}     /** Bus has no default ports */

    // --- Bus-specific interface -------------------------------------------------
    std::string ObjectiveType() const { return mObjectiveType; }
    std::vector<std::string> getPossibleObjectiveTypes() const;

    // --- Linked ports - components connected to this bus ------------------------ 
    const std::vector<MilpPort*>& LinkedPorts() const { return mLinkedPorts; }
    void addLink(MilpPort* linkedPort, BusCompo* parentBus);  
    void removeLink(MilpPort* linkedPort);
    int  checkConnections();

protected:
    std::string mObjectiveType;
    std::vector<MilpPort*> mLinkedPorts{}; /** Non-owning: ports from components linked to this bus (represent links) */
};

#endif // BUSSUBMODEL_H