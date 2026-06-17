#ifndef MaterialCarrier_H
#define MaterialCarrier_H

#include "EnergyVector.h"

class CAIRNCORESHARED_EXPORT MaterialCarrier : public EnergyVector
{
public:
    MaterialCarrier(CairnObject* aParent, const std::string& aName, 
        const std::string& aTechnoType = "Material", t_mapParamData aComponent = {});
    ~MaterialCarrier();

    void initEnergyVector() override;

    std::string FluxType() const { return mFluxType; }

    const std::string* pFlowrateUnit() const { return &mFlowrateUnit; }
    const std::string* pPressureUnit() const { return &mPressureUnit; }

    double SpecificHeatRatio() const { return mSpecificHeatRatio; }
    double computeCp(double a_temp_C);

    double MolarMass() const override;
    double MolarMass(uint64_t t, const class MilpComponent* apComponent = nullptr) const override;

    bool verifyCstCompositions(double C = 0.0, double O = 0.0, double H = 0.0, double N = 0.0, double S = 0.0);

protected:
    void declareConfigurationParameters() override;;
    void declareCompoInputParam() override;;
    std::string getDefaultColor() override;;

    std::string mFluxType;       /** Mass, Energy */

    std::string mMassUnit;       /** kg by default */
    std::string mPressureUnit;                  
    std::string mFlowrateUnit;   /** kg/h by default */

    // Compute Cp
    std::vector<double> mCp_ai;
    std::vector<double> mCp_i;

    double mSpecificHeatRatio;
};


#endif