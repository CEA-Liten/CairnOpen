#include "ElectricalCarrier.h"

ElectricalCarrier::ElectricalCarrier(CairnObject* aParent, const std::string& aName, 
	const std::string& aTechnoType, const t_mapParamData aComponent)
	: EnergyVector(aParent, aName, "ElectricalCarrier", aTechnoType, aComponent)
{
	declareConfigurationParameters();
	setConfigurationParameters(aComponent);
	declareCompoInputParam();
}

ElectricalCarrier::~ElectricalCarrier()
{
}

void ElectricalCarrier::declareConfigurationParameters()
{
	mParamTS["Voltage"] = ParamCarrier("Voltage", "V", 230.0);

	EnergyVector::declareConfigurationParameters();
}

void ElectricalCarrier::declareCompoInputParam()
{
	mCompoOptions->addParameter("PowerUnit", &mPowerUnit, "MW", false, true, "Unit to be used for power - default is MW", "-");

	EnergyVector::declareCompoInputParam();	
}

void ElectricalCarrier::initEnergyVector()
{
	mFluxName    = "ElectricalPower";
	mStorageName = "ElectricalEnergy";

	mFluxUnit    = mPowerUnit;
	mStorageUnit = mPowerUnit + "h";
	mEnergyUnit  = mPowerUnit + "h";
}