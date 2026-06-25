#include "MaterialCarrier.h"
#include "CarrierTypes.h"
#include "OrUnitsConverter.h"

MaterialCarrier::MaterialCarrier(CairnObject* aParent, const std::string& aName, 
	const std::string& aTechnoType, const t_mapParamData aComponent)
	: EnergyVector(aParent, aName, "MaterialCarrier", aTechnoType, aComponent)
{
	declareConfigurationParameters();
	setConfigurationParameters(aComponent);
	declareCompoInputParam();
}

MaterialCarrier::~MaterialCarrier()
{
}

void MaterialCarrier::declareConfigurationParameters()
{
	mParamTS["Pressure"] = ParamCarrier("Pressure", &mPressureUnit, std::nan("1"));
	mParamTS["Temperature"] = ParamCarrier("Temperature", "degC", std::nan("1"));

	/** EnergyContent : Low Heat Value (PCI) in MWh/kg */
	mParamTS["LHV"] = ParamCarrier("Heat Value of fuel type carriers - Use 1. for pure energy model", 
		SFunctionUnit({eFTypeDivision, {&mEnergyUnit, &mMassUnit}}), 0.0);
	/** EnergyContent : Gross Heat Value (PCS) in MWh/kg */
	mParamTS["GHV"] = ParamCarrier("Gross Heat Value - Use 1. for pure energy model", SFunctionUnit({eFTypeDivision, {&mEnergyUnit, &mMassUnit}}), 0.0);
	/** Density in kg/m3 */
	mParamTS["RHO"] = ParamCarrier("Density of fluid type carriers", "kg/m3", 0.0);
	/** Heat capacity in J/kg/m3 */
	mParamTS["CP"] = ParamCarrier("Heat Capacity of heat carriers", "J/K/kg", 0.0);
			
	// Don't expose chemical compositions to the user, if they are fixed for the EnergyVector
	bool isUsed = true;
	const std::vector<std::string> supportedTechnoTypes = CarrierTypes::getCarrierTypes();
	auto it = std::find(supportedTechnoTypes.begin(), supportedTechnoTypes.end(), mCarrierTechnoType);
	if (it != supportedTechnoTypes.end()) {
		isUsed = false;
	}

	// TODO: ensure that the profile is [0, inf). The same for LHV ... ?!

	static const std::unordered_map<std::string, std::string> kElementDescriptions = {
		{ "C", "Atomic count of Carbon (C) in the molecular formula. "
			   "Defines the number of carbon atoms in the pseudo-molecule "
			   "(e.g. C = 1 in C_1H_1.5O_0.65)." },

		{ "H", "Atomic count of Hydrogen (H) in the molecular formula. "
			   "Defines the number of hydrogen atoms in the pseudo-molecule "
			   "(e.g. H = 1.5 in C_1H_1.5O_0.65)." },

		{ "O", "Atomic count of Oxygen (O) in the molecular formula. "
			   "Defines the number of oxygen atoms in the pseudo-molecule "
			   "(e.g. O = 0.65 in C_1H_1.5O_0.65)." },

		{ "N", "Atomic count of Nitrogen (N) in the molecular formula. "
			   "Defines the number of nitrogen atoms in the pseudo-molecule " 
		       "(e.g. N = 2 in CH_4N_2S)." },

		{ "S", "Atomic count of Sulfur (S) in the molecular formula. "
		       "Defines the number of sulfur atoms in the pseudo-molecule " 
		       "(e.g. S = 1 in CH_4N_2S)." }
	};

	std::vector<std::string> vChemicalCompositions = CarrierTypes::getChemicalCompositions();
	for (const auto& elem : vChemicalCompositions)
	{
		std::string description;

		auto it = kElementDescriptions.find(elem);
		if (it != kElementDescriptions.end()) {
			description = it->second;
		}
		else {
			description =
				"Atomic count in the molecular formula. "
				"Defines the number of atoms of this element in the pseudo-molecule.";
		}

		mParamTS[elem] = ParamCarrier(description, "", 0.0, isUsed, "Molecular Composition");
	}


	EnergyVector::declareConfigurationParameters();
}

void MaterialCarrier::declareCompoInputParam()
{	
	mCompoOptions->addParameter("FluxType", &mFluxType, "Mass", false, true, "Mass/Energy", "-");
	mCompoOptions->addParameter("MassUnit", &mMassUnit, "kg", false, true, "Unit to be used for mass - default is kg", "-");
	mCompoOptions->addParameter("PowerUnit", &mPowerUnit, "MW", false, true, "Unit to be used for power - default is MW", "-");
	mCompoOptions->addParameter("PressureUnit", &mPressureUnit, "bar", false, true, "Unit to be used for pressure - default is bar", "-");

	mQuantities["MassUnit"] = &mMassUnit;
	mQuantities["FlowrateUnit"] = &mFlowrateUnit;
	mQuantities["PressureUnit"] = &mPressureUnit;

	// Compute Cp (used by the compressor model)
	mCp_ai.resize(5, 0.);
	for (size_t i = 0; i < mCp_ai.size(); i++) {
		mCompoParams->addParameter("a"+std::to_string(i+1), mCp_ai.data()+i, 0., false, true, "Compute Cp, a" + std::to_string(i+1) +" coefficient", "", "Advanced");
	}
	mCp_i.resize(6, 0.);
	for (size_t i = 0; i < mCp_i.size(); i++) {
		mCompoParams->addParameter("Cp_" + std::to_string(i), mCp_i.data() + i, 0., false, true, "Compute Cp, Cp_" + std::to_string(i) + " coefficient", "", "Advanced");
	}

	mCompoParams->addParameter("SpecificHeatRatio", &mSpecificHeatRatio, 0., false, true, "Specific Heat Ratio", "");

	EnergyVector::declareCompoInputParam();	
}

std::string MaterialCarrier::getDefaultColor()
{
	if (mCarrierTechnoType == "H2Vector")
		return "#6FBF73";
	else if (mCarrierTechnoType == "H2OVector")
		return "blue";
	else if (mCarrierTechnoType == "O2Vector")
		return "pink";
	else if (mCarrierTechnoType == "CO2Vector")
		return "grey";	
	else if (mCarrierTechnoType == "CH4Vector")
		return "#1F8A70";
	else if (mCarrierTechnoType == "NH3Vector")
		return "darkblue";
	else if (mCarrierTechnoType == "Biomass")
		return "darkgreen";
	else if (mCarrierTechnoType == "Heat")
		return "red";
	else if (mCarrierTechnoType == "Cold")
		return "lightblue";
	else if (mCarrierTechnoType == "Chemical")
		return "purple";

	return "maroon";
}

static bool ends_with(std::string_view str, std::string_view suffix)
{
	return str.size() >= suffix.size() && str.compare(str.size() - suffix.size(), suffix.size(), suffix) == 0;
}

void MaterialCarrier::initEnergyVector()
{
	if (mFluxType == "Mass") {
		// Mass
		mStorageName = "MaterialMass";
		mFluxName =  "Flowrate";
		if (ends_with(mMassUnit, "h"))
			mFlowrateUnit = mMassUnit.substr(0, mMassUnit.size()-1);
		else
			mFlowrateUnit = mMassUnit + "/h";
		mFluxUnit = mFlowrateUnit;
		mStorageUnit = mMassUnit;
	}
	else {
		// Energy
		mStorageName = "MaterialEnergy";
		mFluxName = "Power";
		mEnergyUnit = mPowerUnit + "h";
		mFluxUnit = mPowerUnit;
		mStorageUnit = mEnergyUnit;
	}
}

double MaterialCarrier::computeCp(double a_temp_C)
{
	/* TODO: define a time - dependant computeCp ?! */
	
	// Validate techno type
	static const std::vector<std::string> supported = CarrierTypes::getCarrierTypes();
	if (std::find(supported.cbegin(), supported.cend(), mCarrierTechnoType) == supported.cend())
		throw Cairn_Exception("The molar mass of a MaterialCarrier may be time-dependent. "
			"computeCp(temp) cannot be used. Define computeCp(t, temp) instead.", -1);

	const double molarMass = MolarMass();
	if (molarMass == 0.0)
	{
		cCritical() << "Molar Mass is zero in MaterialCarrier" << Name();
		return 0.0;
	}

	const double Temp_K = UnitsConverter::Convert(a_temp_C, "DegC", "K");
	double Cp = 0.0;

	if (mCp_ai[0] != 0.0)
	{
		// Use of NASA equation and coefficients
		for (size_t i = 0; i < mCp_ai.size(); i++)
			Cp += mCp_ai[i] * pow(Temp_K, i);
		Cp = 8.314472 * Cp / molarMass;
	}
	else
	{
		// Use of previous method
		for (size_t i = 0; i < mCp_i.size(); i++)
			Cp += mCp_i[i] * pow(Temp_K, i);
		Cp /= molarMass;
	}

	return Cp;
}

double MaterialCarrier::MolarMass() const
{
	// Constant MolarMass : case of pre-defined techno-types e.g. H2Vector, CO2Vector, etc.
	const std::vector<std::string> supportedTechnoTypes = CarrierTypes::getCarrierTypes();
	auto it = std::find(supportedTechnoTypes.begin(), supportedTechnoTypes.end(), mCarrierTechnoType);
	if (it != supportedTechnoTypes.end()) {
		return MolarMass(0);  // Assumes that the chemical compositions are fixed (not profile)
	}

	// A general material carrier may have time-dependent molar mass : technoType == "Material"
	throw Cairn_Exception(
		Name() + ": MolarMass() without time index is invalid for a MaterialCarrier." 
		"Use MolarMass(t) instead because the value might be time-dependent.",
		-1
	);
}

double MaterialCarrier::MolarMass(uint64_t t, const MilpComponent* apComponent) const
{
	// Cache chemical compositions once (on the first call at t = 0)
	static const std::vector<std::string> compositions =
		CarrierTypes::getChemicalCompositions();

	// Cache composition properties once 
	static std::vector<double> compProps = [] {
		std::vector<double> v;
		v.reserve(compositions.size());
		for (const auto& c : compositions)
			v.push_back(CarrierTypes::getChemicalCompProp(c));
		return v;
	}();

	double result = 0.0;

	for (std::size_t i = 0; i < compositions.size(); ++i) {
		double coeff = getParamValue(compositions[i], t, apComponent);
		if (coeff != 0.0) {
			result += coeff * compProps[i];
		}
	}

	return result;
}

bool MaterialCarrier::verifyCstCompositions(double C, double O, double H, double N, double S)
{
	// Cache chemical compositions once
	static const std::vector<std::string> compositions =
		CarrierTypes::getChemicalCompositions();

	// Ensure no composition uses a time profile
	for (const auto& comp : compositions) {
		if (EnergyVector::useProfileParam(comp)) {
			cCritical() << "Composition parameter \"" << comp
				<< "\" uses a profile — fixed compositions required.";
			return false;
		}
	}

	// Check all required atomic fractions
	auto check = [&](const std::string& name, double expected) -> bool {
		double value = EnergyVector::getParamCstValue(name);

		if (!std::isfinite(value) || value < 0.0) {
			cCritical() << "Composition parameter \"" << name
				<< "\" is invalid (negative or NaN).";
			return false;
		}

		if (std::abs(value - expected) > 1e-6) {
			cCritical() << "Composition mismatch for \"" << name
				<< "\": expected " << expected
				<< ", got " << value;
			return false;
		}

		return true;
	};

	if (!check("H", H)) return false;
	if (!check("O", O)) return false;
	if (!check("C", C)) return false;
	if (!check("N", N)) return false;
	if (!check("S", S)) return false;

	return true;
}

