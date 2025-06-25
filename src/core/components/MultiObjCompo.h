#ifndef MultiObjCompo_H
#define MultiObjCompo_H

class MultiObjCompo ;

#include <QVector>
#include <QDataStream>
#include <Eigen/SparseCore>
#include <Eigen/Dense>
#include "MilpComponent.h"
#include "MIPModeler.h"
#include "BusCompo.h"

#include "CairnCore_global.h"

/**
 * \brief The MultiObjCompo class implements the Milp Model for define manually objectives.\\
 * Ports connected to it are automatically MultiObjCompo typed.\\
 * Balance is the sum of all flow contributions : SumOnPorts(Port->Flux(t)) == 0
 */
class CAIRNCORESHARED_EXPORT MultiObjCompo : public BusCompo
{
public:
    MultiObjCompo(QObject* aParent, const QMap<QString, QString>& aComponent, const QMap < QString, QMap<QString, QString> >& aPorts,
        MilpData* aMilpData, TecEcoEnv& aTecEcoEnv, ModelFactory* aModelFactory);

    virtual ~MultiObjCompo();
    
    int initProblem();
    void declareCompoInputParam();    
    QString ObjectiveType() {return mObjectiveType;}


protected:
    QString mObjectiveType;
};

#endif // MultiObjCompo_H
