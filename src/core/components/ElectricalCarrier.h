#ifndef ElectricalCarrier_H
#define ElectricalCarrier_H

#include "EnergyVector.h"

class CAIRNCORESHARED_EXPORT ElectricalCarrier : public EnergyVector
{
public:
    ElectricalCarrier(CairnObject* aParent, const std::string& aName, 
        const std::string& aTechnoType = "Electricity", const t_mapParamData aComponent = {});
    ~ElectricalCarrier();

    void initEnergyVector() override;

protected:
    void declareConfigurationParameters() override;;
    void declareCompoInputParam() override;
    std::string getDefaultColor() override { return "#E8A317"; }; /** yellow */
};


#endif