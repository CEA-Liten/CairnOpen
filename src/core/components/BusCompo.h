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
    virtual void initSubModelTopology() ; /**Send topology data to submodel : list of ports*/
    virtual void setBusFluxPortExpression(const double &aSignedCoefficient) ;/** Only deal with ports defined from .xml, not all Bus ports*/
    virtual void setBusSameValuePortExpression() ;
    void defineMainCarrier() {
        /*
        * Do nothing. The main carrier of a Bus component is set from its VectorName in OptimProblem::createPortsAndLinksToBus
        */
    }

    void declareCompoInputParam();
    void setCompoInputParam(const std::map<std::string, std::string> aComponent);

    void exportPortResults(t_mapExchange& a_Export, uint modinitTS);

    void jsonSaveGUIlistPortsData(ojson& nodePortArray, const std::string& aSide);
    std::vector<MilpPort*> listSidePorts(const std::string& aside);
    int NbPorts(const std::string& aDirection="");

    const std::string VectorName() {return mVectorName ;}
    const std::vector<MilpComponent*> &ListComponent() {return mListComponent ;}        /** get component List of Ports */

protected:  
    std::string mVectorName ;
    std::vector<MilpComponent*> mListComponent ;      /** List of connected MilpComponent onto Bus */

    void createPortsExportListVars(t_mapExchange& a_Exchange);

};

#endif // BusCompo_H
