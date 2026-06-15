#ifndef TecEcoCompo_H
#define TecEcoCompo_H

class TecEcoCompo;

#include "MilpComponent.h"
#include "CairnCore_global.h"
#include "ModelFactory.h"
#include "TecEcoAnalysis.h"

class CAIRNCORESHARED_EXPORT TecEcoCompo : public MilpComponent
{
public:
    TecEcoCompo(CairnObject* aParent, 
        const std::string& aName,
        const t_mapParamData& aComponent,
        const std::map<std::string, t_mapParamData>& aPorts, 
        MilpData* aMilpData, ModelFactory* aModelFactory);

    ~TecEcoCompo();
  
    bool newCompoModel();

    void declareCompoInputParam() override;
    void setCompoInputParam(const t_mapParamData& aComponent) override;
    void redeclareEnvImpactParameters() override;

    std::vector<InputParam*> get_InputParams() override;
    std::vector<InputParam*> get_ParamInputParams() override;
    std::vector<InputParam*> get_OptionInputParams() override;
    std::vector<InputParam*> get_EnvImpactInputParams() override;

    std::string EnvImpactShortName(const std::string& name) const;
};

#endif // TecEcoCompo_H
