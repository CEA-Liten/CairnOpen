#ifndef BusSubModel_H
#define BusSubModel_H

class BusCompo;

#include "SubModel.h"

class CAIRNCORESHARED_EXPORT BusSubModel : public SubModel
{
public:
    BusSubModel(CairnObject* aParent=nullptr);
    ~BusSubModel();

    void buildModel();
    void computeDefaultIndicators(const double* optSol);

    void declareDefaultModelConfigurationParameters() { 
        SubModel::declareDefaultModelConfigurationParameters();
    }
    void declareDefaultModelParameters() { }

    void declareDefaultModelInterface()
    {
        SubModel::declareDefaultModelInterface();
    }

    void declareDefaultModelIndicators() { }

    void initDefaultPorts() { }; //Bus doesn't have default ports!

    std::string ObjectiveType() { return mObjectiveType; };

    const std::vector<MilpPort*>& LinkedPorts() const { return mLinkedPorts; }
    void addLink(MilpPort* linkedPort, BusCompo* parnetBus);
    void removeLink(MilpPort* linkedPort);

    std::vector<std::string> getPossibleObjectiveTypes() const;

protected:
    std::string mObjectiveType;

    std::vector<MilpPort*> mLinkedPorts{};      /** List of other component ports that are linked to this bus */
};

#endif // BusSubModel_H