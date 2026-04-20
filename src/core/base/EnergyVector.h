#ifndef EnergyVector_H
#define EnergyVector_H
class EnergyVector ;

#include <unordered_map>

#include "CairnCore_global.h"
#include "GUIData.h"
#include "InputParam.h"
#include "CairnUtils.h"
#include "GlobalSettings.h"

using namespace GS;

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

    bool useProfileLHV() const { return mUseProfileLHV; }
    bool useProfileGHV() const { return mUseProfileGHV; }

    std::string LHVProfileName() const { return mProfileLHV; };
    std::string GHVProfileName() const { return mProfileGHV; };
    
    std::string tsProfileID(const std::string& tsParamName) const {
        return Name() + "." + tsParamName;
    }

    std::string LHVProfileID() const { return tsProfileID(ProfileLHV()); };
    std::string GHVProfileID() const { return tsProfileID(ProfileGHV()); };

    double SellPrice() { return mSellPrice; }
    double BuyPrice() { return mBuyPrice; }
    double BuyPriceSeasonal() { return mBuyPriceSeasonal; }
    std::string UseProfileSellPrice() const { return mUseProfileSellPrice; }
    std::string UseProfileBuyPrice()  const { return mUseProfileBuyPrice; }
    std::string UseProfileBuyPriceSeasonal()  const { return mUseProfileBuyPriceSeasonal; }

    void declareConfigurationParameters();
    void setConfigurationParameters(const std::map<std::string, std::string>& aComponent);

    void declareCompoInputParam(); //add parameters
    void setCompoInputParam(const std::map<std::string, std::string> &aComponent); 
    bool InitEnergyVectorParam(const std::map<std::string, std::string>& aComponent = {});

    InputParam* getConfigParam() { return mConfigParam;  }
    InputParam* getCompoInputParam() { return mCompoInputParam; }  /** Get access to Model Parameters */
    InputParam* getCompoInputSettings() { return mCompoInputSettings; }  /** Get access to Model Parameters */
    InputParam* getTimeSeriesParam() { return mTimeSeriesParam; }  /** Get access to Model Parameters */
    InputParam* getGridTimeSeries() { return mGridTimeSeries; }

    std::string getDefaultEnergyVectorColor();
    std::string getDefaultEnergyVectorType();

    std::vector<InputParam*> get_InputParams() override;

    std::vector<InputParam*> get_ParamInputParams() override;
    std::vector<InputParam*> get_OptionInputParams() override;
    std::vector<InputParam*> get_TimeSeriesInputParams() override;
    
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

    bool mUseProfileLHV;
    bool mUseProfileGHV;

    std::string mProfileLHV;
    std::string mProfileGHV;

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

    InputParam* mConfigParam;          /** Config parameters should be read first */
    InputParam* mCompoInputParam ;   /** COMPONENT Input parameter List from XML file -> Options */
    InputParam* mCompoInputSettings ;   /** COMPONENT Input parameter List from Settings File -> Params */
    InputParam* mGridTimeSeries;   /** Holders for Time Series names related to Grids */
    InputParam* mTimeSeriesParam;
};

#endif // EnergyVector_H
