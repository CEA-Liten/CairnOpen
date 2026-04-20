#ifndef SourceLoadCompo_H
#define SourceLoadCompo_H

class SourceLoadCompo ;

#include <Eigen/SparseCore>
#include <Eigen/Dense>
#include "MilpComponent.h"
#include "CairnCore_global.h"
#include "ModelFactory.h"
#include "SourceLoadSubModel.h"

/**
 * \brief The Load class provides functionnalities for IMPOSING time series energy flux load (sens =1) or time series energy flux source (sens=-1).\\
 * Energy fluxes may either be Fluid fluxes (in kg/h) or Electrical / Thermal fluxes (powers, in MW).\\
 * This class implements Milp model description for free injection or extraction:\\
 * \indent MILP Optimization on the weight of this component makes sense and yield relaxation of the problem !\\
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

class CAIRNCORESHARED_EXPORT SourceLoadCompo : public MilpComponent
{
public:
    SourceLoadCompo(CairnObject* aParent, const std::map<std::string, std::string>& aComponent, 
        const std::map<std::string, std::map<std::string, std::string>>& aPorts,
        MilpData* aMilpData, TecEcoAnalysis* aTecEcoAnalysis, ModelFactory* aModelFactory);

    virtual ~SourceLoadCompo();
  
    void declareCompoInputParam();
    void setCompoInputParam(const std::map<std::string, std::string> aComponent);
};

#endif // SourceLoadCompo_H
