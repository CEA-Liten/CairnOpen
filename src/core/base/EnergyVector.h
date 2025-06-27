#ifndef EnergyVector_H
#define EnergyVector_H
class EnergyVector ;

#include <QtCore>
#include <QObject>

#include <QMap>
#include <string.h>
#include <QSettings>

#include "CairnCore_global.h"
#include "GUIData.h"
#include "InputParam.h"

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
    QString     Name;

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

class CAIRNCORESHARED_EXPORT EnergyVector : public QObject
{
    Q_OBJECT
public:
    EnergyVector(QObject* aParent, QString aName, const QMap<QString, QString> aComponent, const QSettings& aSettings);
    EnergyVector(QObject* aParent, const QString& aName, const QString& aType);
    virtual ~EnergyVector();

    GUIData* getGUIData() { return mGUIData; }
    void jsonSaveGuiComponent(QJsonArray& componentsArray);

    QString Name() const { return mName; }
    QString Type() const { return mType; }

    bool isMassCarrier() const { return mIsMassCarrier; }
    bool isHeatCarrier() const { return mIsHeatCarrier; }
    bool isFuelCarrier() const { return mIsFuelCarrier; }

    bool convertStrToBool(const QString& aCase) const { return (aCase.toUpper() == "TRUE" || aCase == "1") ? true : false; }

    QString FluxName() const { return mFluxName; }
    QString StorageName() const { return mStorageName; }
    QString PotentialName() const { return mPotentialName; }
    QString EnergyName() const { return mEnergyName; }
    QString PowerName() const { return mPowerName; }

    QString FluxUnit() const { return mFluxUnit; }
    QString StorageUnit() const { return mStorageUnit; }
    QString MassUnit() const { return mMassUnit; }
    QString FlowrateUnit()const { return mFlowrateUnit; }
    QString PotentialUnit() const { return mPotentialUnit; }
    QString EnergyUnit() const { return mEnergyUnit; }
    QString PowerUnit() const { return mPowerUnit; }
    QString SurfaceUnit() const { return mSurfaceUnit; }
    QString PeakUnit() const { return mPeakUnit; }

    const QString* pFluxUnit() const  { return &mFluxUnit; }
    const QString* pStorageUnit() const  { return &mStorageUnit; }
    const QString* pMassUnit() const { return &mMassUnit; }
    const QString* pFlowrateUnit() const { return &mFlowrateUnit; }
    const QString* pPotentialUnit() const { return &mPotentialUnit; }
    const QString* pEnergyUnit() const { return &mEnergyUnit; }
    const QString* pPowerUnit() const { return &mPowerUnit; }
    const QString* pSurfaceUnit() const { return &mSurfaceUnit; }
    const QString* pPeakUnit() const { return &mPeakUnit; }

    void setFluxUnit(QString& aUnit) { if (aUnit != "") mFluxUnit = aUnit; }
    void setMassUnit(QString& aUnit) { if (aUnit != "") mMassUnit = aUnit; }
    void setFlowrateUnit(QString& aUnit) { if (aUnit != "") mFlowrateUnit = aUnit; }
    void setEnergyUnit(QString& aUnit) { if (aUnit != "") mEnergyUnit = aUnit; }
    void setPowerUnit(QString& aUnit) { if (aUnit != "") mPowerUnit = aUnit; }    
    void setEnergyName(QString& aUnit) { if (aUnit != "") mEnergyName = aUnit; }
    void setPowerName(QString& aUnit) { if (aUnit != "") mPowerName = aUnit; }
    void setSurfaceUnit(const QString& aUnit) { if (aUnit != "") mSurfaceUnit = aUnit; }
    void setPeakUnit(const QString& aUnit) { if (aUnit != "") mPeakUnit = aUnit; }
    
    double CP() { return mCP; }
    double LHV() { return mLHV; }
    double GHV() { return mGHV; }
    double RHO() { return mRHO; }
    double Potential()      const { return mPotential; }
    double* pPotential() { return &mPotential; }
    
    const double* pRgas(QString aVectorType) { return &Get_Pointer_To_Fluid_Properties(getFluidTypeFromQString(aVectorType))->Gas_r; }
    const double* pLHV(QString aVectorType) { return &Get_Pointer_To_Fluid_Properties(getFluidTypeFromQString(aVectorType))->LHV; }
    const double* pSpecificHeatRatio(QString aVectorType) { return &Get_Pointer_To_Fluid_Properties(getFluidTypeFromQString(aVectorType))->Specific_Heat_Ratio; }

    double SellPrice() { return mSellPrice; }
    double BuyPrice() { return mBuyPrice; }
    double BuyPriceSeasonal() { return mBuyPriceSeasonal; }
    QString UseProfileSellPrice() const { return mUseProfileSellPrice; }
    QString UseProfileBuyPrice()  const { return mUseProfileBuyPrice; }
    QString UseProfileBuyPriceSeasonal()  const { return mUseProfileBuyPriceSeasonal; }

    void declareCompoInputParam(); //add parameters
    void setCompoInputParam(const QMap<QString, QString> aComponent);  //set (read from .json) parameter values from paramsMap: mComponent
    bool InitEnergyVectorParam(); //set params based on EnergyVector type and from QSetting

    InputParam* getCompoInputParam() { return mCompoInputParam; }  /** Get access to Model Parameters */
    InputParam* getCompoInputSettings() { return mCompoInputSettings; }  /** Get access to Model Parameters */
    InputParam* getTimeSeriesParam() { return mTimeSeriesParam; }  /** Get access to Model Parameters */

    QString getDefaultEnergyVectorColor();
    QString getDefaultEnergyVectorType();

    static EV::Fluid_Properties                         Fill_Fluid_Properties(EV::Fluid_Type Type);

    static EV::Fluid_Type                               getFluidTypeFromQString (const QString FluidName) ;
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

    static double PowerToMW (QString& aUnit)   ;
    static double EnergyToMWh (QString& aUnit) ;
    static double MassToKg (QString& aUnit)    ;
    static double FlowToKgPh (QString& aUnit)  ;

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

    static const QMap<QString, double> mPowerToMW ;       /** Map Parameter to Value including following - non modifiable */
    static const QMap<QString, double> mEnergyToMWh ;       /** Map Parameter to Value including following - non modifiable */

    static const QMap<QString, double> mMassToKg    ;       /** Map Parameter to Value including following - non modifiable */
    static const QMap<QString, double> mFlowToKgPh    ;     /** Map Parameter to Value including following - non modifiable */

private:

    GUIData* mGUIData{ nullptr }; /** Pointer to GUI Data */
    QString mModel; //The Model name appears on the GUI: Electricity, H2Vector, ..

    QString mName ;                           /** Energy Vector Name - just an id */
    QString mType ;                           /** Energy Vector Type - FluidH2, FluidCH4... / Electrical / Thermal */
    QString mEnergyColour ;                      /** Energy Vector associated Colour */
    bool mIsHeatCarrier ;                  /** Energy vector is Heat carrier - Use for Fluid or materials having thermal capacity - Example hot water, cold water, wood, biomass etc ... */
    bool mIsMassCarrier ;                  /** Energy vector is Mass carrier - Use for Fluid or materials having mass transport capacity - Example water, H2, CH4, wood, biomass etc ... */
    bool mIsFuelCarrier ;                  /** Energy vector is Fuel carrier - Use for fluid or matierials having Heat Value capacities - Example H2, Methane CH4, Oil & Gas, wood, biomass etc ... */
    int mXpos;
    int mYpos;

    QString mFluxUnit;                        /** Flux Unit : kg/h or MW by default */
    QString mFluxName;                        /** Flux Name : Flowrate or Power */
    QString mStorageUnit;                        /** kg or MWh by default */
    QString mStorageName;                        /** Mass or Energy */
    QString mPotentialUnit;                   /** Potential Unit : degC, Bar, V */
    QString mPotentialName;                   /** Potential Name : Temperature, Pression, Voltage */

    QString mEnergyUnit;                        /** MWh by default */
    QString mPowerUnit;                        /** MW by default */
    QString mEnergyName;                        /** energy by default */
    QString mPowerName;                        /** power by default */
    QString mMassUnit;                        /** kg by default */
    QString mFlowrateUnit;               /** kg/h by default */
    QString mSurfaceUnit;                 /** m2 by default */
    QString mPeakUnit;                 /** MWc by default */

    double mPotential ;                         /** Energy Vector Potential Value carried : Temperature, Pression, Voltage */
    double mLHV ;                               /** EnergyContent : Low Heat Value (PCI) in MWh/kg */
    double mGHV ;                               /** EnergyContent : Gross Heat Value (PCS) in MWh/kg */
    double mRHO ;                               /** Density in kg/m3 */
    double mCP ;                               /** Heat capacity in J/kg/m3 */

    QString mUseProfileSellPrice;  /** string indicating the sell price profile to import from PEGASE Exchange Zone 'ZE' <UseProfileBuyPrice>Elec_Grid.ElectricityPrice</UseProfileBuyPrice> */
    QString mUseProfileBuyPrice ;  /** string indicating the buy price profile to import from PEGASE Exchange Zone 'ZE' <UseProfileSellPrice>Elec_Grid.ElectricityPrice</UseProfileSellPrice> */
    QString mUseProfileBuyPriceSeasonal ;  /** string indicating the buy price profile to import from PEGASE Exchange Zone 'ZE' <UseProfileSellPriceSeasonal>Elec_Grid.ElectricityPrice</UseProfileSellPriceSeasonal> */
    double mSellPrice ;       /** Energy Vector selling price, per unit of storage (mass in kg, energy in MWh or MWhTh) */
    double mBuyPrice ;        /** Energy Vector buying price : Pressure, Voltage, Temperature */
    double mBuyPriceSeasonal ;/** Energy Vector buying price : Pressure, Voltage, Temperature */

    InputParam* mCompoInputParam ;   /** COMPONENT Input parameter List from XML file -> Options */
    InputParam* mCompoInputSettings ;   /** COMPONENT Input parameter List from Settings File -> Params */
    InputParam* mTimeSeriesParam;

    bool InitEnergyVectorParam(const QMap<QString, QString> &aComponent, const QSettings& aSettings);
};

#endif // EnergyVector_H
