#ifndef BusCompo_H
#define BusCompo_H

#include <Eigen/SparseCore>
#include <Eigen/Dense>
#include "MilpComponent.h"
#include "MilpPort.h"
#include "MIPModeler.h"

#include "CairnCore_global.h"
/**
 * \brief BusCompo is the virtual class for definition of bus connecting constraints (rules)
 */

class CAIRNCORESHARED_EXPORT BusCompo : public MilpComponent
{
public:
    BusCompo (CairnObject* aParent, const std::map<std::string, std::string>& aComponent, const std::map < std::string, std::map<std::string, std::string> >& aPorts, 
        MilpData *aMilpData, TecEcoEnv &aTecEcoEnv, ModelFactory* aModelFactory) ;

    virtual ~BusCompo();

    void addPort(MilpPort* lptrport);
    int initPorts() ;
    virtual int checkPorts();
    void DeleteBusPort(MilpPort* lptrport);
    void RemoveLinkComponent(MilpComponent* lptr);/* Remove Component connected */
    virtual void addComponent(MilpComponent* lptr) ;  /** Add Component connected */
    void setBusFluxPortExpression() override;/** Only deal with ports defined from .xml, not all Bus ports*/
    void setBusSameValuePortExpression() ;
    void defineMainCarrier() {
        /*
        * Do nothing. The main carrier of a Bus component is set in OptimProblem::createPortsAndLinksToBus
        */
    }

    void declareCompoInputParam();
    void setCompoInputParam(const std::map<std::string, std::string> aComponent);

    std::string ObjectiveType() const; /* case of MultiObjective */

    void exportPortResults(t_mapExchange& a_Export, uint modinitTS);

    void jsonSaveGUIlistPortsData(ojson& nodePortArray, const std::string& aSide);
    std::vector<MilpPort*> listSidePorts(const std::string& aside);
    int NbPorts(const std::string& aDirection="");

    std::string CarrierName() const;

    const std::vector<MilpComponent*> &ListComponent() {return mListComponent ;}        /** get component List of Ports */

    std::vector<InputParam*> get_InputParams();

protected:  
    std::vector<MilpComponent*> mListComponent ;      /** List of connected MilpComponent onto Bus */

    void createPortsExportListVars(t_mapExchange& a_Exchange);

};

#endif // BusCompo_H
