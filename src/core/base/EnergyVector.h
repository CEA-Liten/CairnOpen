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
    EnergyVector(CairnObject* aParent, const std::string& aName, const std::string& aType, 
        const std::string& aTechnoType, const t_mapParamData aComponent);
    virtual ~EnergyVector();

    GUIData* getGUIData() { return mGUIData; }
    void jsonSaveGuiComponent(ojson& componentsArray);

    std::string Name() const { return std::string(this->objectName().c_str()); }
    void setName(const std::string& name) { this->setObjectName(name); }

    std::string Type() const { return mCarrierType; }
    std::string TechnoType() const { return mCarrierTechnoType; }

    bool convertStrToBool(const std::string& aCase) const { return (CairnUtils::toUpper(aCase) == "TRUE" || aCase == "1") ? true : false; }
     
    std::string FluxName() const { return mFluxName; }
    std::string StorageName() const { return mStorageName; }

    std::string FluxUnit() const { return mFluxUnit; }
    std::string StorageUnit() const { return mStorageUnit; }

    const std::string* pFluxUnit() const { return &mFluxUnit; }
    const std::string* pStorageUnit() const { return &mStorageUnit; }
    const std::string* pPowerUnit() const { return &mPowerUnit; }
    const std::string* pQuantity(const std::string& a_Quantity) const;
   
    bool isParamExist(const std::string& aName) const;
    bool useProfileParam(const std::string& aName) const;
    const double getParamCstValue(const std::string& aName) const;
    virtual const double getParamValue(const std::string& aName, const uint64_t t, 
        const MilpComponent* apComponent = nullptr) const;
    const double getMinParamValue(const std::string& aName, const MilpComponent* apComponent) const;
        
    virtual double MolarMass() const {
        return std::numeric_limits<double>::quiet_NaN();
    }
    virtual double MolarMass(uint64_t t, const class MilpComponent* apComponent = nullptr) const {
        return std::numeric_limits<double>::quiet_NaN();
    }

    std::string tsProfileID(const std::string& tsParamName) const {
        return Name() + "." + tsParamName;
    }

    int initProblem();
    bool updateCompoParamMap(const std::string& a_SettingName, 
        const std::string& a_AttributeName, const std::string& a_AttributeValue);

    InputParam* getTimeSeriesParam() { return mTimeSeriesParam; }  /** Get access to Model Parameters */

    std::vector<InputParam*> get_InputParams() override;
    std::vector<InputParam*> get_ParamInputParams() override;
    std::vector<InputParam*> get_OptionInputParams() override;
    std::vector<InputParam*> get_TimeSeriesInputParams() override;

    // Uses by GridCompo
    double SellPrice() const { return mParamGridTS.at("SellPrice").Value; }
    double BuyPrice() const { return mParamGridTS.at("BuyPrice").Value; }
    double BuyPriceSeasonal() const { return mParamGridTS.at("BuyPriceSeasonal").Value; }
    std::string UseProfileSellPrice() const { return mParamGridTS.at("SellPrice").Profile; }
    std::string UseProfileBuyPrice()  const { return mParamGridTS.at("BuyPrice").Profile; }
    std::string UseProfileBuyPriceSeasonal()  const { return mParamGridTS.at("BuyPriceSeasonal").Profile; }

    virtual void initEnergyVector() { };
    void initGuiData(const t_mapParamData& paramMap = {});

protected:
    void configTechnoType();

    virtual void declareConfigurationParameters();
    void setCustomConfigParams();
    int setConfigurationParameters(const t_mapParamData& aComponent);

    virtual void declareCompoInputParam();  
    void setCustomParams();
    int setCompoInputParam(const t_mapParamData& aComponent);

    virtual std::string getDefaultColor() { return "black"; };

    class ParamCarrier {
    public:
        double Value{ 0. };
        bool UseProfile{ false };
        std::string Profile;

        ParamCarrier() {};
        ParamCarrier(const std::string& aDescription, const t_unit& aUnit, 
            double aDefault = 0, bool aIsUsed = true, const std::string& aShowConfig = "Base");
        void addConfig(InputParam *aConfigParam, const std::string& aName);
        void addParameter(InputParam* aInputParam, InputParam* aTimeSeriesParam, const std::string& aName);

    protected:
        std::string mDescription;
        t_unit mUnit;
        double mDefault{ 0.0 };
        bool mUseProfileDefault{ false };
        bool mIsUsed{ false };  
        std::string mShowConfig{"Base"};

        bool mUseConfig{ false };
    };
   
    typedef std::map<std::string, ParamCarrier> t_mapParamCarrier;
    t_mapParamCarrier mParamTS;
    t_mapParamCarrier mParamGridTS;

    GUIData* mGUIData{ nullptr }; /** Pointer to GUI Data */
    std::string mModel; //The Model name appears on the GUI: Electricity, H2Vector, ..

    std::string mCarrierType;          /** Energy Vector Type - Electrical / Material */
    std::string mCarrierTechnoType;    /** EnergyVector TechnoType - H2Vector, H2OVector... / Heat */

    std::string mEnergyColour ;       /** Energy Vector associated Colour */

    std::string mCurrency = "EUR"; //TODO: get currency from TecEcoAnalysis

    std::string mStorageName;                        /** Mass or Energy */
    std::string mFluxName;                        /** Flux Name : Flowrate or Power */

    std::string mStorageUnit;                        /** kg or MWh by default */
    std::string mFluxUnit;                        /** Flux Unit : kg/h or MW by default */
    std::string mPowerUnit;                        /** MW by default */
    std::string mEnergyUnit;                        /** MWh by default */
     
    std::map<std::string, const std::string*> mQuantities;
    t_mapParamData mComponent;         /** Map of Topological data from .json */
  
    InputParam* mConfigParam;           /** Config parameters should be read first */
    InputParam* mCompoOptions;          /** COMPONENT Input parameter List from XML file -> Options */
    InputParam* mCompoParams;   /** COMPONENT Input parameter List from Settings File -> Params */
    InputParam* mGridTimeSeries;        /** Holders for Time Series names related to Grids */
    InputParam* mTimeSeriesParam;
};

#endif // EnergyVector_H
