#ifndef BusCompo_H
#define BusCompo_H

#include <Eigen/SparseCore>
#include <Eigen/Dense>

#include "MilpComponent.h"
#include "BusSubModel.h"
#include "MilpPort.h"
#include "MIPModeler.h"

#include "CairnCore_global.h"
/**
 * \brief BusCompo is the virtual class for definition of bus connecting constraints (rules)
 */

class CAIRNCORESHARED_EXPORT BusCompo : public MilpComponent
{
public:
    BusCompo (CairnObject* aParent, const std::map<std::string, std::string>& aComponent, 
        const std::map < std::string, std::map<std::string, std::string> >& aPorts, 
        MilpData *aMilpData, TecEcoAnalysis* aTecEcoAnalysis, ModelFactory* aModelFactory) ;

    virtual ~BusCompo();

    int checkPorts() override;
    int checkConnections();

    void defineMainCarrier() {
        /*
        * Do nothing. The main carrier of a Bus component is set in OptimProblem::createPortsAndLinksToBus
        */
    }

    void declareCompoInputParam();
    void setCompoInputParam(const std::map<std::string, std::string> aComponent);

    std::string ObjectiveType() const; /* case of MultiObjective */
    std::vector<std::string> getPossibleObjectiveTypes() const;

    std::string CarrierName() const;

    const std::vector<MilpComponent*> &ListComponent() {return mListComponent ;}        /** get component List of Ports */

    std::vector<InputParam*> get_InputParams();

    std::vector<InputParam*> get_TimeSeriesInputParams() override;
    std::vector<InputParam*> get_EnvImpactInputParams() override;
    std::vector<InputParam*> get_PortEnvImpactInputParams() override;

    BusSubModel* busModel() const;

    /** Mangmenet of the ports related to the componenets that are linked to this Bus */
    const std::vector<MilpPort*>& LinkedPorts() const;
    void addLink(MilpComponent* linkedComponent, MilpPort* linkedPort);
    void removeLink(MilpComponent* linkedComponent, MilpPort* linkedPort);
    
    /** Save as json file */
    int NbPorts(const std::string& aDirection = "");
    std::vector<MilpPort*> listSidePorts(const std::string& aside);
    void jsonSaveGUIlistPortsData(ojson& nodePortArray, const std::string& aSide);

protected:  
    // Iterate on LinkedPorts and obtain their parents?!
    std::vector<MilpComponent*> mListComponent ;      /** List of connected MilpComponent onto Bus */

    void createPortsExportListVars(t_mapExchange& a_Exchange);
};
#endif // BusCompo_H
