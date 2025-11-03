#ifndef GridCompo_H
#define GridCompo_H

class GridCompo ;

#include <Eigen/SparseCore>
#include <Eigen/Dense>
#include "MilpComponent.h"

#include "GridSubModel.h"
#include "CairnCore_global.h"
#include "ModelFactory.h"

/**
 * \brief The Grid class provides functionnalities for FREE injection of energy flux to grid (sens =1) or FREE extraction of energy flux from grid (sens=-1).\\
 * Energy fluxes may either be Fluid fluxes (in kg/h) or Electrical / Thermal fluxes (powers, in MW).\\
 * This class implements Milp model description for free injection or extraction:\\
 * \indent MILP Optimization on the weight of this component makes no sense and yield non linearity !\\
 * \indent Weight parameter can still be set by outside optimization of the design
 */
/** Use specific setParameters                 for interface with User Data                    */
/** Use specific prepareOptim                  for variable initialization from PEGASE coupling*/
/** Use generic MilpComponent::buildProblem()  for optimal problem building                    */
/** - Build Model component behaviour : refer to model/buildModel()                            */
/** - define behaviour model and associated Variables                                          */
/** - make IO expression available to Component                                                */
/** - Build Objective contribution                                                             */
/** - send flux expressions to FlowBalanceBus                                                  */
/** - publish expression to SameValueBus                                                       */
/** Use specific exportResults                for PEGASE GUI interface filling                 */
/** Use specific setDefaultsResults           for PEGASE GUI interface filling                 */
/** Use specific computeTecEcoEnvAnalysis     for TecEco analysis computation                  */

class CAIRNCORESHARED_EXPORT GridCompo : public MilpComponent
{
public:
    GridCompo(CairnObject* aParent, const std::map<std::string, std::string>& aComponent, const std::map<std::string, std::map<std::string, std::string>>& aPorts,
        MilpData* aMilpData, TecEcoEnv& aTecEcoEnv, ModelFactory* aModelFactory);

    virtual ~GridCompo();
    
    void setDefaultsResults();
    void readTSVariablesFromModel();

    void declareCompoInputParam();
    void setCompoInputParam(const std::map<std::string, std::string> aComponent);

    int setParameters();
    
protected:
    // Model interface
    std::string mEnergyPriceProfileName ;  /** Grid energy price profile name */
    std::string mEnergyPriceProfileNameSeasonal ;  /** Grid energy price profile name */
};

#endif // GridCompo_H
