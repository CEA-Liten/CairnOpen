#ifndef MultiObjCompo_H
#define MultiObjCompo_H

class MultiObjCompo ;

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
    MultiObjCompo(CairnObject* aParent, 
        const std::string& aName,
        const t_mapParamData& aComponent,
        const std::map < std::string, t_mapParamData>& aPorts,
        MilpData* aMilpData, TecEcoAnalysis* aTecEcoAnalysis, ModelFactory* aModelFactory);

    ~MultiObjCompo();
    
    void declareCompoInputParam();    

protected:
};

#endif // MultiObjCompo_H
