#ifndef SourceLoadCompo_H
#define SourceLoadCompo_H

class SourceLoadCompo ;

#include <QVector>
#include <QDataStream>
#include <Eigen/SparseCore>
#include <Eigen/Dense>
#include "MilpComponent.h"
#include "CairnCore_global.h"
#include "ModelFactory.h"

/**
 * \brief The Load class provides functionnalities for IMPOSING time series energy flux load (sens =1) or time series energy flux source (sens=-1).\\
 * Energy fluxes may either be Fluid fluxes (in kg/h) or Electrical / Thermal fluxes (powers, in MW).\\
 * This class implements Milp model description for free injection or extraction:\\
 * \indent MILP Optimization on the weight of this component makes sense and yield relaxation of the problem !\\
 * \indent Weight parameter can still be set by outside optimization of the design
 */
/** Use specific createZEVariablesList         for interface definition with PEGASE GUI        */
/** Use specific setParameters                 for interface with User Data                    */
/** Use specific prepareOptim                  for variable initialization from PEGASE coupling*/
/** Use specific initProblem                   for MIP problem init, model creation & checkings*/
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
    SourceLoadCompo(QObject* aParent, const QMap<QString, QString>& aComponent, const QMap<QString, QMap<QString, QString>>& aPorts,
        MilpData* aMilpData, TecEcoEnv& aTecEcoEnv, ModelFactory* aModelFactory);

    virtual ~SourceLoadCompo();

    
    int initProblem();
  
    
    void declareCompoInputParam();
    void setCompoInputParam(const QMap<QString, QString> aComponent);

};

#endif // SourceLoadCompo_H
