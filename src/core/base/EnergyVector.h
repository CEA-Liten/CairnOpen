#ifndef EnergyVector_H
#define EnergyVector_H
class EnergyVector ;

#include "CairnCore_global.h"
#include "GUIData.h"
#include "InputParam.h"
#include "CairnUtils.h"
/**
 * \details
* This component allows the definition of the quantities (mass of fluids, materials, or electricity and heat) managed by the energy system.
* These quantities will be denoted as energy vectors or energy carriers.\\
* The component allows the definition of units of stored quantities and their corresponding flows :\\
* \begin{itemize}
* \item Mass Flow    : kg/h
* \item Power (ie energy flow)   : MW
* \item Mass         : kg
* \item Energy       : MWh
* \item Time         : Hours
* \end{itemize}
*
* It is highly recommended to use the following, instead of the IS Units leading to "scaling" troubles during solving step.\\
*/
namespace EV
{
    enum Vector_Type
    {
        Electrical,
        Thermal,
        Material,
        Fluid,
        OtherType
    };
enum Fluid_Type
{
    H2,
    O2,
    CH4,
    CH4_H2,
    Air,
    H2OLiq,
    Steam,
    LOX,
    LIN,
    CO2,
    Fuel,
    UnknownFluid
};
enum Fluid_Phase
{
    Gas,
    Liquid
};

struct Fluid_Properties
{
    std::string     Name;

    Fluid_Phase Phase;

    Fluid_Type  Type;

    double      Molar_Mass,                         // kg/mol
                Critical_Temperature,               // K
                Critical_Pressure,                  // bar
                Critical_Density,                   // kg/m3
                Critical_Compressibility_Factor,    // -
                Acentric_Factor,                    // -
                Mass_Density_a,                     // kg/m3
                Mass_Density_b,
                Mass_Density_c,
                Mass_Density_d,
                DeltaH0,                            // J/mol
                DeltaG0,                            // J/mol
                S0,                                 // J/mol
                a1,                                 // Coeff NASA Technical Memorandum 4513 (1993)
                a2,                                 // Coeff NASA Technical Memorandum 4513 (1993)
                a3,                                 // Coeff NASA Technical Memorandum 4513 (1993)
                a4,                                 // Coeff NASA Technical Memorandum 4513 (1993)
                a5,                                 // Coeff NASA Technical Memorandum 4513 (1993)
                Cp_0,                               // heat capacity J/K/Kg
                Cp_1,
                Cp_2,
                Cp_3,
                Cp_4,
                Cp_5,
                LHV,                                // Lower Heating Value (or Net Heating Value) kWh/kg (1 kWh = 3.6 MJ)
                Specific_Heat_Ratio,                // gamma = cp / cv
                Gas_r ;                             // R / Molar_Mass ; J/K/Kg

    bool mixture;

    // beware of the use !
    // RHO and HHV must be set under same conditions representative of the "Normal" conditions !
    // CP can be set at real local conditions
    double 		Xvol,
                RHO_NormalConditions,    			// density at Normal conditions 0degC, 1.01325 Bar ; kg / Nm3
                HHV_NormalConditions;  				// Higher Heating Value at same conditions; MJ / Nm3
};
struct  Struct_Universal_Parameters
{
    Struct_Universal_Parameters(
            double Boltzman_Constant_Value,
            double Avogadro_Constant_Value,
            double Elementary_Charge_Value,
            double R_Value,
            double g_Value) :

        Boltzman_Constant(Boltzman_Constant_Value),
        Avogadro_Constant(Avogadro_Constant_Value),
        Elementary_Charge(Elementary_Charge_Value),
        R(R_Value),
        Faraday_Constant(Elementary_Charge_Value*Avogadro_Constant_Value),
        g(g_Value)
        {}

    const double    Boltzman_Constant,
                    Avogadro_Constant,                          // mol-1
                    Elementary_Charge,                          // C
                    R,                                          // J/mol/K
                    Faraday_Constant,                           //
                    g;                                          // 9.81
};

}

class CAIRNCORESHARED_EXPORT EnergyVector : public CairnObject
{    
public:
    EnergyVector(CairnObject* aParent, const std::string& aName, const std::string& aType, const std::map<std::string, std::string> aComponent);
    virtual ~EnergyVector();

    GUIData* getGUIData() { return mGUIData; }
    void jsonSaveGuiComponent(ojson& componentsArray);

    std::string Name() const { return std::string(this->objectName().c_str()); }
    void setName(const std::string& name) { this->setObjectName(name); }

    std::string Type() const { return mCarrierType; }

    std::vector<InputParam*> get_InputParams();

    bool isMassCarrier() const { return mIsMassCarrier; }
    bool isHeatCarrier() const { return mIsHeatCarrier; }
    bool isFuelCarrier() const { return mIsFuelCarrier; }

    bool convertStrToBool(const std::string& aCase) const { return (CairnUtils::toUpper(aCase) == "TRUE" || aCase == "1") ? true : false; }

    std::string FluxName() const { return mFluxName; }
    std::string StorageName() const { return mStorageName; }
    std::string PotentialName() const { return mPotentialName; }
    std::string EnergyName() const { return mEnergyName; }
    std::string PowerName() const { return mPowerName; }

    std::string FluxUnit() const { return mFluxUnit; }
    std::string StorageUnit() const { return mStorageUnit; }
    std::string MassUnit() const { return mMassUnit; }
    std::string FlowrateUnit()const { return mFlowrateUnit; }
    std::string PotentialUnit() const { return mPotentialUnit; }
    std::string EnergyUnit() const { return mEnergyUnit; }
    std::string PowerUnit() const { return mPowerUnit; }

    const std::string* pFluxUnit() const { return &mFluxUnit; }
    const std::string* pStorageUnit() const { return &mStorageUnit; }
    const std::string* pMassUnit() const { return &mMassUnit; }
    const std::string* pFlowrateUnit() const { return &mFlowrateUnit; }
    const std::string* pPotentialUnit() const { return &mPotentialUnit; }
    const std::string* pEnergyUnit() const { return &mEnergyUnit; }
    const std::string* pPowerUnit() const { return &mPowerUnit; }

    void setFluxUnit(std::string& aUnit) { if (aUnit != "") mFluxUnit = aUnit; }
    void setMassUnit(std::string& aUnit) { if (aUnit != "") mMassUnit = aUnit; }
    void setFlowrateUnit(std::string& aUnit) { if (aUnit != "") mFlowrateUnit = aUnit; }
    void setEnergyUnit(std::string& aUnit) { if (aUnit != "") mEnergyUnit = aUnit; }
    void setPowerUnit(std::string& aUnit) { if (aUnit != "") mPowerUnit = aUnit; }    
    void setEnergyName(std::string& aUnit) { if (aUnit != "") mEnergyName = aUnit; }
    void setPowerName(std::string& aUnit) { if (aUnit != "") mPowerName = aUnit; }
    
    double CP() { return mCP; }
    double LHV() { return mLHV; }
    double GHV() { return mGHV; }
    double RHO() { return mRHO; }
    double Potential()      const { return mPotential; }
    double* pPotential() { return &mPotential; }
    
    const double* pSpecificHeatRatio(std::string aVectorType);

    double SellPrice() { return mSellPrice; }
    double BuyPrice() { return mBuyPrice; }
    double BuyPriceSeasonal() { return mBuyPriceSeasonal; }
    std::string UseProfileSellPrice() const { return mUseProfileSellPrice; }
    std::string UseProfileBuyPrice()  const { return mUseProfileBuyPrice; }
    std::string UseProfileBuyPriceSeasonal()  const { return mUseProfileBuyPriceSeasonal; }

    void declareCompoInputParam(); //add parameters
    void setCompoInputParam(const std::map<std::string, std::string> &aComponent); 
    bool InitEnergyVectorParam(const std::map<std::string, std::string>& aComponent = {});

    InputParam* getCompoInputParam() { return mCompoInputParam; }  /** Get access to Model Parameters */
    InputParam* getCompoInputSettings() { return mCompoInputSettings; }  /** Get access to Model Parameters */
    InputParam* getTimeSeriesParam() { return mTimeSeriesParam; }  /** Get access to Model Parameters */

    std::string getDefaultEnergyVectorColor();
    std::string getDefaultEnergyVectorType();

    static EV::Fluid_Properties                         Fill_Fluid_Properties(EV::Fluid_Type Type);

    static EV::Fluid_Type                               getFluidTypeFromQString (const std::string FluidName) ;
    static const EV::Fluid_Properties*                  Get_Pointer_To_Fluid_Properties(EV::Fluid_Type Type);
    static double                                   Compute_Cp(double Temperature, const EV::Fluid_Properties* p_Fluid_Properties);
    static double                                   Compute_H(double Temperature, double Pressure, const EV::Fluid_Properties* p_Matter_Properties);
    static double                                   Compute_Density(double Temperature, const EV::Fluid_Properties* p_Matter_Properties);
    static double                                   Compute_S(double Temperature, double Pressure, const EV::Fluid_Properties *p_Matter_Properties);
    static double Bar2Pa(double P_Bar);
    static double Pa2Bar(double P_Pa);
    static double Deg2Kel(double T_Deg);
    static double Kel2Deg(double T_Kel);

    // convert mass fraction of fluid 1 in Fluid1+Fluid2 mixture into molar volume fraction of Fluid1.
    static double Mass2VolFrac(double Xmass_F1, EV::Fluid_Properties F1, EV::Fluid_Properties F2);
    static double Vol2MassFrac(double Xmolar_F1, EV::Fluid_Properties F1, EV::Fluid_Properties F2);

    static double PowerToMW (std::string& aUnit)   ;
    static double EnergyToMWh (std::string& aUnit) ;
    static double MassToKg (std::string& aUnit)    ;
    static double FlowToKgPh (std::string& aUnit)  ;

    // Static Properties
    static const EV::Fluid_Properties           H2;
    static const EV::Fluid_Properties           O2;
    static const EV::Fluid_Properties           Air;
    static const EV::Fluid_Properties           H2OLiq;
    static const EV::Fluid_Properties           Steam;
    static const EV::Fluid_Properties           CH4;
    static const EV::Fluid_Properties           CH4_H2;
    static const EV::Fluid_Properties           LOX;
    static const EV::Fluid_Properties           LIN;
    static const EV::Fluid_Properties           CO2;
    static const EV::Fluid_Properties           Fuel;
    static double                           Default_Pressure;
    static double                           Default_Temperature;

    static const std::map<std::string, double> mPowerToMW ;       /** Map Parameter to Value including following - non modifiable */
    static const std::map<std::string, double> mEnergyToMWh ;       /** Map Parameter to Value including following - non modifiable */

    static const std::map<std::string, double> mMassToKg    ;       /** Map Parameter to Value including following - non modifiable */
    static const std::map<std::string, double> mFlowToKgPh    ;     /** Map Parameter to Value including following - non modifiable */

private:

    GUIData* mGUIData{ nullptr }; /** Pointer to GUI Data */
    std::string mModel; //The Model name appears on the GUI: Electricity, H2Vector, ..

    std::string mCarrierType;                          /** Energy Vector Type - FluidH2, FluidCH4... / Electrical / Thermal */
    std::string mEnergyColour ;                 /** Energy Vector associated Colour */
    bool mIsHeatCarrier ;                  /** Energy vector is Heat carrier - Use for Fluid or materials having thermal capacity - Example hot water, cold water, wood, biomass etc ... */
    bool mIsMassCarrier ;                  /** Energy vector is Mass carrier - Use for Fluid or materials having mass transport capacity - Example water, H2, CH4, wood, biomass etc ... */
    bool mIsFuelCarrier ;                  /** Energy vector is Fuel carrier - Use for fluid or matierials having Heat Value capacities - Example H2, Methane CH4, Oil & Gas, wood, biomass etc ... */

    std::string mCurrency = "EUR"; //TODO: get currency from TecEcoAnalysis

    std::string mFluxUnit;                        /** Flux Unit : kg/h or MW by default */
    std::string mFluxName;                        /** Flux Name : Flowrate or Power */
    std::string mStorageUnit;                        /** kg or MWh by default */
    std::string mStorageName;                        /** Mass or Energy */
    std::string mPotentialUnit;                   /** Potential Unit : degC, Bar, V */
    std::string mPotentialName;                   /** Potential Name : Temperature, Pression, Voltage */

    std::string mEnergyUnit;                        /** MWh by default */
    std::string mPowerUnit;                        /** MW by default */
    std::string mEnergyName;                        /** energy by default */
    std::string mPowerName;                        /** power by default */
    std::string mMassUnit;                        /** kg by default */
    std::string mFlowrateUnit;               /** kg/h by default */

    double mPotential ;                         /** Energy Vector Potential Value carried : Temperature, Pression, Voltage */
    double mLHV ;                               /** EnergyContent : Low Heat Value (PCI) in MWh/kg */
    double mGHV ;                               /** EnergyContent : Gross Heat Value (PCS) in MWh/kg */
    double mRHO ;                               /** Density in kg/m3 */
    double mCP ;                               /** Heat capacity in J/kg/m3 */

    std::string mUseProfileSellPrice;  /** string indicating the sell price profile to import from PEGASE Exchange Zone 'ZE' <UseProfileBuyPrice>Elec_Grid.ElectricityPrice</UseProfileBuyPrice> */
    std::string mUseProfileBuyPrice ;  /** string indicating the buy price profile to import from PEGASE Exchange Zone 'ZE' <UseProfileSellPrice>Elec_Grid.ElectricityPrice</UseProfileSellPrice> */
    std::string mUseProfileBuyPriceSeasonal ;  /** string indicating the buy price profile to import from PEGASE Exchange Zone 'ZE' <UseProfileSellPriceSeasonal>Elec_Grid.ElectricityPrice</UseProfileSellPriceSeasonal> */
    double mSellPrice ;       /** Energy Vector selling price, per unit of storage (mass in kg, energy in MWh or MWhTh) */
    double mBuyPrice ;        /** Energy Vector buying price : Pressure, Voltage, Temperature */
    double mBuyPriceSeasonal ;/** Energy Vector buying price : Pressure, Voltage, Temperature */

    InputParam* mCompoInputParam ;   /** COMPONENT Input parameter List from XML file -> Options */
    InputParam* mCompoInputSettings ;   /** COMPONENT Input parameter List from Settings File -> Params */
    InputParam* mTimeSeriesParam;
};

#endif // EnergyVector_H
