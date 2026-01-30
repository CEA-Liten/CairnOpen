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
    TecEcoCompo(CairnObject* aParent, const std::map<std::string, std::string>& aComponent, const std::map<std::string, std::map<std::string, 
        std::string>>& aPorts, MilpData* aMilpData, TecEcoEnv& aTecEcoEnv, ModelFactory* aModelFactory);

    ~TecEcoCompo();
  
    bool newCompoModel();

    void declareCompoInputParam() override;
    void setCompoInputParam(const std::map<std::string, std::string> aComponent) override;
    void redeclareEnvImpactParameters() override;

    virtual std::vector<class InputParam*> get_InputParams();
};

#endif // TecEcoCompo_H
