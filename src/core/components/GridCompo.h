#ifndef GridCompo_H
#define GridCompo_H

class GridCompo ;

#include <QVector>
#include <QDataStream>
#include <Eigen/SparseCore>
#include <Eigen/Dense>
#include "MilpComponent.h"

#include "SubModel.h"
#include "CairnCore_global.h"
#include "ModelFactory.h"

/**
 * \brief The Grid class provides functionnalities for FREE injection of energy flux to grid (sens =1) or FREE extraction of energy flux from grid (sens=-1).\\
 * Energy fluxes may either be Fluid fluxes (in kg/h) or Electrical / Thermal fluxes (powers, in MW).\\
 * This class implements Milp model description for free injection or extraction:\\
 * \indent MILP Optimization on the weight of this component makes no sense and yield non linearity !\\
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

class CAIRNCORESHARED_EXPORT GridCompo : public MilpComponent
{
public:
    GridCompo(QObject* aParent, const QMap<QString, QString>& aComponent, const QMap<QString, QMap<QString, QString>>& aPorts,
        MilpData* aMilpData, TecEcoEnv& aTecEcoEnv, ModelFactory* aModelFactory);

    virtual ~GridCompo();
    
    void setDefaultsResults();
    void readTSVariablesFromModel();

    void declareCompoInputParam();
    void setCompoInputParam(const QMap<QString, QString> aComponent);

    int initProblem();
    int setParameters();
    

protected:
    // Model interface
    QString mEnergyPriceProfileName ;  /** Grid energy price profile name */
    QString mEnergyPriceProfileNameSeasonal ;  /** Grid energy price profile name */
};

#endif // GridCompo_H
