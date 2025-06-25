#include "base/EnergyVector.h"
#include "base/MilpComponent.h"
#include "GlobalSettings.h"
using namespace GS ;
using namespace EV ;
#include "qdebug.h"

EV::Struct_Universal_Parameters Universal_Parameters(1.38e-23,
6.02214179e23,
1.60217733e-19,
8.314472,
9.81);

EV::Fluid_Properties    const EnergyVector::H2 = Fill_Fluid_Properties(EV::Fluid_Type::H2);
EV::Fluid_Properties    const EnergyVector::O2 = Fill_Fluid_Properties(EV::Fluid_Type::O2);
EV::Fluid_Properties    const EnergyVector::Air = Fill_Fluid_Properties(EV::Fluid_Type::Air);
EV::Fluid_Properties    const EnergyVector::CH4 = Fill_Fluid_Properties(EV::Fluid_Type::CH4);
EV::Fluid_Properties    const EnergyVector::CH4_H2 = Fill_Fluid_Properties(EV::Fluid_Type::CH4_H2);
EV::Fluid_Properties    const EnergyVector::H2OLiq = Fill_Fluid_Properties(EV::Fluid_Type::H2OLiq);
EV::Fluid_Properties    const EnergyVector::Steam = Fill_Fluid_Properties(EV::Fluid_Type::Steam);
EV::Fluid_Properties    const EnergyVector::LOX = Fill_Fluid_Properties(EV::Fluid_Type::LOX);
EV::Fluid_Properties    const EnergyVector::LIN = Fill_Fluid_Properties(EV::Fluid_Type::LIN);
EV::Fluid_Properties    const EnergyVector::CO2 = Fill_Fluid_Properties(EV::Fluid_Type::CO2);
EV::Fluid_Properties    const EnergyVector::Fuel = Fill_Fluid_Properties(EV::Fluid_Type::Fuel);

double EnergyVector::Default_Pressure = 1.01325;
double EnergyVector::Default_Temperature = 20;

const QMap<QString, double> EnergyVector::mPowerToMW   = { {"GW" ,1.e3}, {"MW"  ,1.}, {"kW" ,1.e-3}, {"W" ,1.e-6} };
const QMap<QString, double> EnergyVector::mEnergyToMWh = { {"GWh",1.e3}, {"MWh" ,1.}, {"kWh",1.e-3}, {"Wh",1.e-6}, {"kJ", 1.e3/3600.}, {"MJ", 1.e6/3600.} };
const QMap<QString, double> EnergyVector::mMassToKg    = { {"t"  ,1.e3}, {"kg"  ,1.}, {"g"  ,1.e-3} };
const QMap<QString, double> EnergyVector::mFlowToKgPh  = { {"t/h",1.e3}, {"kg/h",1.} };

//TODO virtualize and create FluidVectors, ThermalVectors, ElectricalVectors
EnergyVector::EnergyVector(QObject* aParent, QString aName, const QMap<QString, QString> aComponent, const QSettings &aSettings): QObject(aParent),
    mEnergyColour(""),
    mBuyPriceSeasonal(0.0) //Is it needed ?
{
    this->setObjectName(aName);    
    declareCompoInputParam();
    setCompoInputParam(aComponent);
    InitEnergyVectorParam(aComponent, aSettings);

    qInfo() << " Energy Vector " << mName << " of type " << mType ;
    qInfo() << " Energy Vector " << mName << " use MassCarrier property " << isMassCarrier() << " RHO " << mRHO ;
    qInfo() << " Energy Vector " << mName << " use HeatCarrier property " << isHeatCarrier() << " CP  " << mCP  ;
    qInfo() << " Energy Vector " << mName << " use FuelCarrier property " << isFuelCarrier() << " LHV " << mLHV ;
}

EnergyVector::EnergyVector(QObject* aParent, 
    const QString &aName, const QString& aType):
    mEnergyColour(""),
    mBuyPriceSeasonal(0.0)
{
    this->setObjectName(aName);
    declareCompoInputParam();
    //should be set after declareCompoInputParam because it set params to their default values
    mName = aName;
    mType = aType;
    //
    InitEnergyVectorParam();
}

EnergyVector::~EnergyVector()
{
    if (mGUIData) delete mGUIData;
    if (mCompoInputParam) delete mCompoInputParam;
    if (mCompoInputSettings) delete mCompoInputSettings;
    if (mTimeSeriesParam) delete mTimeSeriesParam;
}

void EnergyVector::declareCompoInputParam()
{
    //------------------------------ options ------------------------------
    mCompoInputParam = new InputParam (this,"CompoInputParam"+ mName) ;
    mCompoInputParam->addToConfigList({"UnitNames"});
    //QString
    mCompoInputParam->addParameter("id", &mName, "EnergyVector", true, true, "Energy vector name", "", {"DONOTSHOW"});
    mCompoInputParam->addParameter("Type", &mType, "", false, true,"Energy vector type specifying the energy form among: Electrical - Thermal - Fluid - Material");
    mCompoInputParam->addParameter("Xpos", &mXpos, 0, false, true, "User interface positioning of the object", "", {"DONOTSHOW"});
    mCompoInputParam->addParameter("Ypos", &mYpos, 0, false, true, "User interface positioning of the object", "", {"DONOTSHOW"});
    mCompoInputParam->addParameter("MassUnit", &mMassUnit, "kg", true, true,"Unit to be used for mass - default is kg","-");
    mCompoInputParam->addParameter("EnergyUnit", &mEnergyUnit, "MWh", true, true,"Unit to be used for energy - default is MWh","-");
    mCompoInputParam->addParameter("PowerUnit", &mPowerUnit, "MW", true, true,"Unit to be used for power - default is MW","-");
    mCompoInputParam->addParameter("FlowrateUnit", &mFlowrateUnit, "kg/h", true, true,"Unit to be used for mass flow - default is kg/h","-");
    mCompoInputParam->addParameter("FluxUnit", &mFluxUnit, "", false, true,"Unit to be used for Flow","",{"NOTSHOWN"});
    mCompoInputParam->addParameter("FluxName", &mFluxName, "", false, true, "Name to be used for Flow", "", { "UnitNames" });
    mCompoInputParam->addParameter("StorageName", &mStorageName, "", false, true, "Name to be used for storage capacity", "", { "UnitNames" });
    mCompoInputParam->addParameter("StorageUnit", &mStorageUnit, "", false, true, "Unit to be used for storage capacity", "", { "NOTSHOWN" });
    mCompoInputParam->addParameter("PotentialName", &mPotentialName, "", false, true,"Name to be used for potential eg Pressure - Temperature...");
    mCompoInputParam->addParameter("PotentialUnit", &mPotentialUnit, "", false, true,"Unit to be used for constant potential","Bar");
    mCompoInputParam->addParameter("SurfaceUnit", &mSurfaceUnit, "m2", true, true, "Unit to be used for surface - default is m2", "-");
    mCompoInputParam->addParameter("PeakUnit", &mPeakUnit, "MWc", true, true, "Unit to be used for peak - default is Megawatt-crete ", "-");
    mCompoInputParam->addParameter("EnergyName", &mEnergyName, "Energy", false, true,"Name to be used for Energy","",{"UnitNames"});
    mCompoInputParam->addParameter("PowerName", &mPowerName, "Power", false, true,"Name to be used for Power","",{"UnitNames"});
    //bool
    mCompoInputParam->addParameter("IsMassCarrier",&mIsMassCarrier, false, false, true,"Option meaning mass is carried by energy vector - Default to false for Electrical and Thermal types - true for Fluids and Material");
    mCompoInputParam->addParameter("IsHeatCarrier",&mIsHeatCarrier, false, false, true,"Option giving heat capacity ability to energy vector - Default to true for Electrical and Thermal types - false for Fluids and Material");
    mCompoInputParam->addParameter("IsFuelCarrier",&mIsFuelCarrier, false, false, true,"Option giving heating vaue ability to energy vector - Default to false for Electrical and Thermal types or non fuel Fluids - true for Fluid and Material fuels");
    //------------------------------ ! timeseries Names ! ------------------------------
    mTimeSeriesParam = new InputParam(this, "TimeSeriesSettings" + mName);
    //QString
    mTimeSeriesParam->addParameter("UseProfileBuyPrice", &mUseProfileBuyPrice, "", false, true,"Optional buy price time profile","EUR/StorageUnit");
    mTimeSeriesParam->addParameter("UseProfileSellPrice", &mUseProfileSellPrice, "", false, true,"Optional sell price time profile","EUR/StorageUnit");
    mTimeSeriesParam->addParameter("UseProfileBuyPriceSeasonal", &mUseProfileBuyPriceSeasonal, "", false, true, "Optional buy price seasonal time profile", "EUR/StorageUnit");
    //------------------------------ parameters ------------------------------
    mCompoInputSettings = new InputParam (this,"CompoInputSettings"+ mName) ;
    //double
    mCompoInputSettings->addParameter("Potential", &mPotential, 0., true, true, "Voltage- Pressure- Temperature","V-Bar-degC");
    mCompoInputSettings->addParameter("LHV", &mLHV, 1., &mIsFuelCarrier, true, "Heat Value of fuel type carriers - Use 1. for pure energy model ","EnergyUnit/MassUnit");
    mCompoInputSettings->addParameter("CP", &mCP, 0., &mIsHeatCarrier, true, "Heat Capacity of heat carriers ","J/K/kg");
    mCompoInputSettings->addParameter("GHV", &mGHV, 0., &mIsFuelCarrier, true, "Gross Heat Value - Use 1. for pure energy model ","EnergyUnit/MassUnit",{"NOTUSED"});
    mCompoInputSettings->addParameter("RHO", &mRHO, 0., &mIsMassCarrier, true, "Density of fluid type carriers","kg/m3");
    mCompoInputSettings->addParameter("BuyPrice", &mBuyPrice, 0., true, true, "Constant BuyPrice per mass or energy units","EUR/StorageUnit");
    mCompoInputSettings->addParameter("SellPrice", &mSellPrice, 0., true, true, "Constant SellPrice per mass or energy units","EUR/StorageUnit");
}

void EnergyVector::setCompoInputParam(const QMap<QString, QString> aComponent) {
    mName = aComponent["id"];
    mType = aComponent["Type"];
    mXpos = aComponent["Xpos"].toInt();
    mYpos = aComponent["Ypos"].toInt();
    mEnergyColour = aComponent["EnergyColor"];
    mModel = aComponent["Model"];
    //units
    mFluxUnit = aComponent["FluxUnit"];
    mStorageUnit = aComponent["StorageUnit"];
    mPotentialUnit = aComponent["PotentialUnit"];
    if (aComponent["PowerUnit"] != "")  {
        mPowerUnit = aComponent["PowerUnit"];
    }
    if (aComponent["EnergyUnit"] != "") {
        mEnergyUnit = aComponent["EnergyUnit"];
    }
    if (aComponent["MassUnit"] != "")  {
        mMassUnit = aComponent["MassUnit"];
    }
    if (aComponent["FlowrateUnit"] != "") {
        mFlowrateUnit = aComponent["FlowrateUnit"];
    }
    setSurfaceUnit(aComponent["SurfaceUnit"]);
    setPeakUnit(aComponent["PeakUnit"]);
    //names 
    mFluxName = aComponent["FluxName"];
    mStorageName = aComponent["StorageName"];
    mPotentialName = aComponent["PotentialName"];
    if (aComponent["EnergyName"] != "") {
        QString energyName = aComponent["EnergyName"];
        setEnergyName(energyName);
    }
    if (aComponent["PowerName"] != "") {
        QString powerName = aComponent["PowerName"];
        setPowerName(powerName);
    }
    //names of price time series
    mUseProfileBuyPrice = aComponent["UseProfileBuyPrice"];
    mUseProfileBuyPriceSeasonal = aComponent["UseProfileBuyPriceSeasonal"];
    mUseProfileSellPrice = aComponent["UseProfileSellPrice"];
}

bool EnergyVector::InitEnergyVectorParam()
{
    QSettings vSettings;
    QMap<QString, QString> vComponent;
    return InitEnergyVectorParam(vComponent, vSettings);
}

bool EnergyVector::InitEnergyVectorParam(const QMap<QString, QString> &aComponent, const QSettings& aSettings)
{
    if (mGUIData) delete mGUIData;
    mGUIData = new GUIData(parent(), mName);
    if (mModel == "") {
        mModel = getDefaultEnergyVectorType();
    }
    if (mXpos == 0) {
        mXpos = (0.5 * (mGUIData->GetId()));
    }
    if (mYpos == 0) {
        mYpos = 10;
    }
    mGUIData->doInit(mModel, mModel, "EnergyVector", mXpos, mYpos);

    if (mType == "")
    {
        qCritical() << "<Type> attribute is void !! A Type among Fluid, Material, Thermal or Electrical should be given for EnergyVector " << (mName);
        Cairn_Exception erreur("void <Type> attribute is not allowed, Type among Fluid, Material, Thermal or Electrical should be given for EnergyVector " + mName, -1);
        throw& erreur;
    }
    if (!(mType.contains("Fluid")
        || mType.contains("Material")
        || mType.contains("Wood")
        || mType.contains("BioMass")
        || mType.contains("Electrical")
        || mType.contains("Thermal")))
    {
        qCritical() << "<Type> attribute error " << (mName);
        Cairn_Exception erreur("<Type> attribute must be one among Fluid*, BioMass, Wood, Material, Thermal or Electrical for EnergyVector " + mName, -1);
        throw& erreur;
    }

    Q_ASSERT(mType != "");

    if (mFluxUnit == "")
    {
        if (mType.contains("Fluid")
            || mType.contains("Material")
            || mType.contains("Wood")
            || mType.contains("BioMass"))    mFluxUnit = mFlowrateUnit;
        if (mType.contains("Electrical"))    mFluxUnit = mPowerUnit;
        if (mType.contains("Thermal"))    mFluxUnit = mPowerUnit;
    }
    else
    {
        if (mType.contains("Fluid")
            || mType.contains("Material")
            || mType.contains("Wood")
            || mType.contains("BioMass"))    mFlowrateUnit = mFluxUnit;
        if (mType.contains("Electrical"))    mPowerUnit = mFluxUnit;
        if (mType.contains("Thermal"))    mPowerUnit = mFluxUnit;
    }

    if (mStorageUnit == "")
    {
        if (mType.contains("Fluid")
            || mType.contains("Material")
            || mType.contains("Wood")
            || mType.contains("BioMass"))    mStorageUnit = mMassUnit;
        if (mType.contains("Electrical"))    mStorageUnit = mEnergyUnit;
        if (mType.contains("Thermal"))       mStorageUnit = mEnergyUnit;
    }
    else
    {
        if (mType.contains("Fluid")
            || mType.contains("Material")
            || mType.contains("Wood")
            || mType.contains("BioMass"))      mMassUnit = mStorageUnit;
        if (mType.contains("Electrical"))   mEnergyUnit = mStorageUnit;
        if (mType.contains("Thermal"))      mEnergyUnit = mStorageUnit;
    }
    if (mFluxName == "")
    {
        if (mType.contains("Fluid")
            || mType.contains("Material")
            || mType.contains("Wood")
            || mType.contains("BioMass"))      mFluxName = mType + "Flowrate";
        if (mType.contains("Electrical"))   mFluxName = mType + "Power";
        if (mType.contains("Thermal"))      mFluxName = mType + "Power";
    }
    if (mStorageName == "")
    {
        if (mType.contains("Fluid")
            || mType.contains("Material")
            || mType.contains("Wood")
            || mType.contains("BioMass"))      mStorageName = mType + "Mass";
        if (mType.contains("Electrical"))   mStorageName = mType + "Energy";
        if (mType.contains("Thermal"))      mStorageName = mType + "Energy";
    }
    if (mPotentialName == "")
    {
        if (mType.contains("Fluid"))      mPotentialName = "Pressure";
        if (mType.contains("Wood")
            || mType.contains("BioMass")
            || mType.contains("Material"))   mPotentialName = "Xmassfract";
        if (mType.contains("Electrical")) mPotentialName = "Voltage";
        if (mType.contains("Thermal"))    mPotentialName = "Temperature"; // and Pressure ?? classes derivees a prevoir entre Thermal et Fluid
    }
    if (mPotentialUnit == "")
    {
        if (mType.contains("Fluid"))       mPotentialUnit = "bar";
        if (mType.contains("Wood")
            || mType.contains("BioMass")
            || mType.contains("Material"))    mPotentialUnit = "percent";
        if (mType.contains("Electrical"))  mPotentialUnit = "V";
        if (mType.contains("Thermal"))     mPotentialUnit = "degC";  // and Pressure ?? classes derivees a prevoir entre Thermal et Fluid
    }

    Q_ASSERT(mPotentialUnit != "");
    Q_ASSERT(mFluxUnit != "");
    Q_ASSERT(mStorageUnit != "");
    Q_ASSERT(mPotentialUnit != "");
    Q_ASSERT(mFluxName != "");
    Q_ASSERT(mStorageName != "");
    if (mPotentialUnit == "" || mFluxUnit == "" || mStorageUnit == "" || mPotentialUnit == "" || mFluxName == "" || mStorageName == "")
    {
        qCritical() << "Void unit/name detected for EnergyVector " << (mName);
        Cairn_Exception erreur("Void unit/name is not allowed - Please check/correct EnergyVector " + mName, -1);
        throw& erreur;
    }

    if (mType.contains("Fluid") || mType.contains("Material") || mType.contains("BioMass") || mType.contains("Wood"))
    {
        mIsMassCarrier = true;
    }
    QString massCarrier = aComponent["IsMassCarrier"];
    if (massCarrier != "") {
        mIsMassCarrier = convertStrToBool(massCarrier); 
        if (isMassCarrier() && (mType.contains("Electrical") || mType.contains("Thermal")))
        {
            qCritical() << "IsMassCarrier attribute error detected for EnergyVector " << (mName);
            Cairn_Exception erreur("IsMassCarrier is not allowed for Electrical / Thermal energy types - Please check/correct EnergyVector " + mName, -1);
            throw& erreur;
        }
    }

    if (mType.contains("FluidH2O"))
    {
        mIsHeatCarrier = true;
    }
    QString heatCarrier = aComponent["IsHeatCarrier"];
    if (heatCarrier != "") {
        mIsHeatCarrier = convertStrToBool(heatCarrier);
        if (isHeatCarrier() && mType.contains("Electrical"))
        {
            qCritical() << "mIsHeatCarrier attribute error detected for EnergyVector " << (mName);
            Cairn_Exception erreur("mIsHeatCarrier is not allowed for Electrical energy types - Please check/correct EnergyVector " + mName, -1);
            throw& erreur;
        }
    }
    
    if ((mType.contains("Fluid") && !mType.contains("FluidH2O") && !mType.contains("FluidCO2")) || mType.contains("Wood") || mType.contains("BioMass"))
    {
        mIsFuelCarrier = true;
    }
    QString fuelCarrier = aComponent["IsFuelCarrier"];
    if (fuelCarrier != "") {
        mIsFuelCarrier = convertStrToBool(fuelCarrier);
        if (isFuelCarrier() && mType.contains("Electrical"))
        {
            qCritical() << "IsFuelCarrier attribute error detected for EnergyVector " << (mName);
            Cairn_Exception erreur("IsFuelCarrier is not allowed for Electrical energy types - Please check/correct EnergyVector " + mName, -1);
            throw& erreur;
        }
    }
    
    if (isFuelCarrier())    {
        if (getVariantSetting(mName + ".LHV", aSettings).isValid()) {
            double lhv = getVariantSetting(mName + ".LHV", aSettings).toDouble();
            if (lhv > 0.) // si initialisé par Settings
            {
                mLHV = lhv;
            }
        }

        if (mLHV <=0.) // si initialisé avant
        {
            QString energyUnit = mCompoInputParam->getParamQSValue("EnergyUnit");
            if (energyUnit != "") setEnergyUnit(energyUnit);

            // Ok si pas de changement d'unite par defaut, ou EnergyUnit reprecisee
            if (EnergyToMWh(mEnergyUnit) > 0. && MassToKg(mMassUnit) > 0.)
            {
                // cf https://www.picbleu.fr/page/tableau-comparatif-pouvoir-calorique-inferieur-pci-des-energies
                if (getFluidTypeFromQString(mType) != UnknownFluid)
                    mLHV = 1.e-3 * Get_Pointer_To_Fluid_Properties(getFluidTypeFromQString(mType))->LHV * MassToKg(mMassUnit) / EnergyToMWh(mEnergyUnit); // MWh/kg
                else if (mType == "FluidFuel") mLHV = 11.86e-3 * MassToKg(mMassUnit) / EnergyToMWh(mEnergyUnit);
                else if (mType == "FluidH2O")  mLHV = 0.0;
                else if (mType == "DryWood")   mLHV = 4.e-3 * MassToKg(mMassUnit) / EnergyToMWh(mEnergyUnit);            //buches Hetres 20% humidite
                else if (mType == "WetWood")   mLHV = 2.e-3 * MassToKg(mMassUnit) / EnergyToMWh(mEnergyUnit);            //buches Hetres 40% humidite
                else if (mType == "BioMass")   mLHV = 6.e-3 * MassToKg(mMassUnit) / EnergyToMWh(mEnergyUnit);            // plutot bonne qualite
                else {
                    qWarning() << "Caution : Energy carrier type seems to be neither Fluid nor Electrical nor Thermal : " << (mName) << " of Type " << (mType);
                    qWarning() << "Your energy vector must be one among these Fluid*, Electrical or Thermal";
                }
            }
            else
            {
                qCritical() << "To use LHV predefined values, you must use consistent EnergyUnit or MassUnit for fluid vector " + mName + " !";
                qCritical() << "Instead, we found Energy Unit " << mEnergyUnit << " and Mass unit " << mMassUnit;
                Cairn_Exception erreur("Mass Unit must be multiple of kg or t, Energy Unit mut be multiple of MWh or J ", -1);
                throw& erreur;
            }
        }
        qInfo() << " Low Heat Value = " << mLHV;
    }
    else if (mType.contains("Thermal")) mLHV = 1.;
    else if (mType.contains("Electrical"))  mLHV = 1.;

    if (mLHV <= 0.)
    {
        qCritical() << "Negative or Zero LHV value detected for energy vector " << (mName) << mLHV;
        Cairn_Exception erreur("Negative LHV value is not allowed for energy carrier - Use 1. for pure energy carriers - Please check/correct EnergyVector " + mName, -1);
        throw& erreur;
    }

    if (isHeatCarrier())
    {        
        if (mCP <= 0.)
        {
            if (mType == "FluidH2O")  mCP = 4.186e3;// J/kg/K at 300K https://www.engineeringtoolbox.com/specific-heat-capacity-d_391.html
            else if (mType == "FluidCH4")  mCP = 2.226e3;// J/kg/K at 300 K https://www.engineeringtoolbox.com/methane-d_980.html
            else if (mType == "FluidFuel") mCP = 3.e3; // paraffin
            else if (mType == "FluidH2")   mCP = 14.3e3;      // J/kg/K at 300 K https://www.engineeringtoolbox.com/hydrogen-d_976.html
            else if (mType == "FluidAir")  mCP = 1.293;  // J/kg/K at 300K
            else if (mType == "DryWood")   mCP = 1.3e3;  // https://www.engineeringtoolbox.com/specific-heat-capacity-d_391.html
            else if (mType == "WetWood")   mCP = 2.4e3;  // https://www.engineeringtoolbox.com/specific-heat-capacity-d_391.html
            else if (mType == "BioMass")   mCP = 2.4e3;  //   https://www.engineeringtoolbox.com/specific-heat-capacity-d_391.html
            else if (mType.contains("Thermal")) mCP = 4.186e3; // J/kg/K at 300K https://www.engineeringtoolbox.com/specific-heat-capacity-d_391.html
        }
        qInfo() << mName << ".CP=" << mCP;
    }

    if (getVariantSetting(mName + ".Potential", aSettings).isValid())
        mPotential = getVariantSetting(mName + ".Potential", aSettings).toDouble();

    if (isHeatCarrier())
    {
        if (getVariantSetting(mName + ".CP", aSettings).isValid())
            mCP = getVariantSetting(mName + ".CP", aSettings).toDouble();
    }
    if (isFuelCarrier())
    {
        if (getVariantSetting(mName + ".GHV", aSettings).isValid())
            mGHV = getVariantSetting(mName + ".GHV", aSettings).toDouble();
    }

    if (isMassCarrier())
    {
        if (getVariantSetting(mName + ".RHO", aSettings).isValid())
            mRHO = getVariantSetting(mName + ".RHO", aSettings).toDouble();

        if (mRHO <= 0.)
        {
            if (getFluidTypeFromQString(mType) != UnknownFluid) mRHO = Get_Pointer_To_Fluid_Properties(getFluidTypeFromQString(mType))->RHO_NormalConditions; // MWh/kg
            if (mType == "FluidH2O") mRHO = 1.0e3;
        }
    }

    if (getVariantSetting(mName + ".BuyPrice", aSettings).isValid())
        mBuyPrice = getVariantSetting(mName + ".BuyPrice", aSettings).toDouble();

    if (getVariantSetting(mName + ".BuyPriceSeasonal", aSettings).isValid())
        mBuyPriceSeasonal = getVariantSetting(mName + ".BuyPriceSeasonal", aSettings).toDouble();

    if (getVariantSetting(mName + ".SellPrice", aSettings).isValid())
        mSellPrice = getVariantSetting(mName + ".SellPrice", aSettings).toDouble();

    return true;
}

double EnergyVector::PowerToMW (QString& aUnit)
{
    if (EnergyVector::mPowerToMW.find(aUnit)   == EnergyVector::mPowerToMW.end()  )
    {
        qCritical() <<" Unable to perform conversion with unknown unit ! " << aUnit ;
        return -1 ;
    }
    return EnergyVector::mPowerToMW[aUnit] ;
}
double EnergyVector::EnergyToMWh (QString& aUnit)
{
    if (EnergyVector::mEnergyToMWh.find(aUnit) == EnergyVector::mEnergyToMWh.end())
    {
        qCritical() <<" Unable to perform conversion with unknown unit ! " << aUnit ;
        return -1 ;
    }
    return EnergyVector::mEnergyToMWh[aUnit] ;
}
double EnergyVector::MassToKg (QString& aUnit)
{
    if (EnergyVector::mMassToKg.find(aUnit)    == EnergyVector::mMassToKg.end()   )
    {
        qCritical() <<" Unable to perform conversion with unknown unit ! " << aUnit ;
        return -1 ;
    }
    return EnergyVector::mMassToKg[aUnit] ;
}
double EnergyVector::FlowToKgPh (QString& aUnit)
{
    if (EnergyVector::mFlowToKgPh.find(aUnit)  == EnergyVector::mFlowToKgPh.end() )
    {
        qCritical() <<" Unable to perform conversion with unknown unit ! " << aUnit ;
        return -1 ;
    }
    return EnergyVector::mFlowToKgPh[aUnit] ;
}

void EnergyVector::jsonSaveGuiComponent(QJsonArray &componentsArray)
{
    QJsonArray paramArray;
    QJsonArray optionArray;
    QJsonArray timeSeriesArray;

    mCompoInputSettings->jsonSaveGUIInputParam(paramArray);
    mCompoInputParam->jsonSaveGUIInputParam(optionArray);
    mTimeSeriesParam->jsonSaveGUIInputParam(timeSeriesArray);
    
    if (mEnergyColour == "") {
        mEnergyColour = getDefaultEnergyVectorColor();
    }

    QJsonObject compoObject;
    mGUIData->jsonSaveGUILine(compoObject);
    compoObject.insert("energyTypeColor", QJsonValue::fromVariant(mEnergyColour));
    compoObject.insert("paramListJson", paramArray);
    compoObject.insert("optionListJson", optionArray);
    compoObject.insert("timeSeriesListJson", timeSeriesArray);

    componentsArray.push_back(compoObject);
}

QString EnergyVector::getDefaultEnergyVectorType() {
    QString defaultType = "EnergyVector";
    if (mType.toUpper().contains("THERM") || mType.toUpper().contains("HEAT"))
    {
        defaultType = "Heat";
    }
    else if (mType.toUpper().contains("H2"))
    {
        defaultType = "H2Vector";
    }
    else if (mType.toUpper().contains("CH4"))
    {
        defaultType = "CH4Vector";
    }
    else if (mType.toUpper().contains("ELEC"))
    {
        defaultType = "Electricity";
    }
    else if (mType.toUpper().contains("BIOMASS") || mType.toUpper().contains("MATERIAL") || mType.toUpper().contains("WOOD"))
    {
        defaultType = "Biomass";
    }
    else {
        defaultType = "EnergyVector";
    }
    return defaultType;
}

QString EnergyVector::getDefaultEnergyVectorColor()
{
    QString defaultColor = "";
    if (mType.toUpper().contains("THERM") || mType.toUpper().contains("HEAT"))
    {
        if (mName.toUpper().contains("HOT")) defaultColor = "red";
        if (mName.toUpper().contains("CHAUD")) defaultColor = "red";
        if (mName.toUpper().contains("HEAT")) defaultColor = "red";
        if (mName.toUpper().contains("COLD")) defaultColor = "blue";
        if (mName.toUpper().contains("FROID")) defaultColor = "blue";
    }
    else if (mType.toUpper().contains("H2"))
    {
        defaultColor = "green";
    }
    else if (mType.toUpper().contains("CH4"))
    {
        defaultColor = "brown";
    }
    else if (mType.toUpper().contains("CO2"))
    {
        defaultColor = "grey";
    }
    else if (mType.toUpper().contains("H2O"))
    {
        defaultColor = "blue";
    }
    else if (mType.toUpper().contains("ELEC"))
    {
        defaultColor = "yellow";
    }
    else if (mType.toUpper().contains("BIOMASS"))
    {
        defaultColor = "darkgreen";
    }
    else if (mType.toUpper().contains("WOOD"))
    {
        defaultColor = "darkgreen";
    }
    else if (mType.toUpper().contains("MATERIAL"))
    {
        defaultColor = "maroon";
    }
    else if (mType.toUpper().contains("FLUID"))
    {
        defaultColor = "darkblue";
    }
    else {
        defaultColor = "black";
    }
    return defaultColor;
}

EV::Fluid_Properties                                EnergyVector::Fill_Fluid_Properties(EV::Fluid_Type Type)
{
    EV::Fluid_Properties Properties;

    // avoid multiple runtime computations
    Properties.mixture = false ;
    Properties.Xvol = 1. ;   // molar fraction of pure gas
    Properties.LHV = 0. ;    // Lower Heating Value kWh/kg
    Properties.HHV_NormalConditions = 0. ;    // Higher Heating Value at normal conditions (1.01325 Bar, 0 degC) MJ/Nm3
    Properties.RHO_NormalConditions = 0. ;    // density at normal conditions (1.01325 Bar, 0 degC)kg/Nm3

    switch (Type)
    {
      case EV::Fluid_Type::H2:

        Properties.Name = "H2";
        Properties.Phase = EV::Fluid_Phase::Gas;
        Properties.Type = EV::Fluid_Type::H2;
        Properties.Critical_Temperature = 33.18;
        Properties.Critical_Pressure = 13.13;
        Properties.Critical_Density = 31.4;
        Properties.Critical_Compressibility_Factor = 0.305;
        Properties.Acentric_Factor = -0.220;
        Properties.Molar_Mass = 0.0020159;
        Properties.Mass_Density_a = 0.89;
        Properties.Mass_Density_b = 0;
        Properties.Mass_Density_c = 0;
        Properties.Mass_Density_d = 0;
        Properties.DeltaH0 = 0;
        Properties.DeltaG0 = 0;
        Properties.S0 = 130.44944;

        Properties.a1 = 2.93286579;
        Properties.a2 = 8.26607967e-4;
        Properties.a3 = -1.46402335e-7;
        Properties.a4 = 1.54100359e-11;
        Properties.a5 = -6.88804432e-16;

        Properties.Cp_0 = 27.6716;
        Properties.Cp_1 = 3.3858 * 1e-3;
        Properties.Cp_2 = 0;
        Properties.Cp_3 = 0;
        Properties.Cp_4 = 0;
        Properties.Cp_5 = 0;

        Properties.Specific_Heat_Ratio = 1.41;
        Properties.LHV = 33.32;
        Properties.HHV_NormalConditions = 12.75 ;
        Properties.RHO_NormalConditions = 8.99e-2 ;

        break ;

      case EV::Fluid_Type::O2:

        Properties.Name = "O2";
        Properties.Phase = EV::Fluid_Phase::Gas;
        Properties.Type = EV::Fluid_Type::O2;
        Properties.Critical_Temperature = 154.55;
        Properties.Critical_Pressure = 50.43;
        Properties.Critical_Density = 436.1;
        Properties.Critical_Compressibility_Factor = 0;
        Properties.Acentric_Factor = 0.022;
        Properties.Molar_Mass = 0.0319989;
        Properties.Mass_Density_a = 1.427;
        Properties.Mass_Density_b = 0;
        Properties.Mass_Density_c = 0;
        Properties.Mass_Density_d = 0;
        Properties.DeltaH0 = 0;
        Properties.DeltaG0 = 0;
        Properties.S0 = 204.83254;

        Properties.a1 = 3.66096083;
        Properties.a2 = 6.56365523E-04;
        Properties.a3 = -1.41149485E-07;
        Properties.a4 = 2.05797658E-11;
        Properties.a5 = -1.29913248E-15;

        Properties.Cp_0 = 25.9996;
        Properties.Cp_1 = 11.3278 * 1e-3;
        Properties.Cp_2 = -1.5466 * 1e-6;
        Properties.Cp_3 = -0.9196 * 1e-9;
        Properties.Cp_4 = 0;
        Properties.Cp_5 = 0;

        Properties.Specific_Heat_Ratio = 1.40;

        Properties.RHO_NormalConditions = 1.4269 ;                // kg/Nm3 1bar, 15�C
        break ;

    case EV::Fluid_Type::Air:

        Properties.Name = "Air";
        Properties.Phase = EV::Fluid_Phase::Gas;
        Properties.Type = EV::Fluid_Type::Air;
        Properties.Critical_Temperature = 0;
        Properties.Critical_Pressure = 0;
        Properties.Critical_Density = 0;
        Properties.Critical_Compressibility_Factor = 0;
        Properties.Acentric_Factor = 0;
        Properties.Molar_Mass = 0.02897;
        Properties.Mass_Density_a = 1.294;
        Properties.Mass_Density_b = 0;
        Properties.Mass_Density_c = 0;
        Properties.Mass_Density_d = 0;
        Properties.DeltaH0 = 0;
        Properties.DeltaG0 = 0;
        Properties.S0 = 0;

        Properties.a1 = 0.0;
        Properties.a2 = 0.0;
        Properties.a3 = 0.0;
        Properties.a4 = 0.0;
        Properties.a5 = 0.0;

        Properties.Cp_0 = 29.116;
        Properties.Cp_1 = 0;
        Properties.Cp_2 = 0;
        Properties.Cp_3 = 0;
        Properties.Cp_4 = 0;
        Properties.Cp_5 = 0;
        Properties.Specific_Heat_Ratio = 1.40;

        break ;

    case EV::Fluid_Type::CH4:

        // Ref Air Liquide
        Properties.Name = "CH4";
        Properties.Phase = EV::Fluid_Phase::Gas;
        Properties.Type = EV::Fluid_Type::CH4;
        Properties.Critical_Temperature = 190.56;
        Properties.Critical_Pressure = 45.99;
        Properties.Critical_Density = 162.7;
        Properties.Critical_Compressibility_Factor = 0;
        Properties.Acentric_Factor = 0.0115;                // Ref http://pubs.usgs.gov/of/2005/1451/equation.html
        Properties.Molar_Mass = 0.01604;
        Properties.Mass_Density_a = 0;
        Properties.Mass_Density_b = 0;
        Properties.Mass_Density_c = 0;
        Properties.Mass_Density_d = 0;
        Properties.DeltaH0 = 0;
        Properties.DeltaG0 = 0;
        Properties.S0 = 0;

        Properties.a1 = 1.63552643;
        Properties.a2 = 1.00842795e-2;
        Properties.a3 = -3.36916254e-6;
        Properties.a4 = 5.34958667e-10;
        Properties.a5 = -3.15518833e-14;

        Properties.Cp_0 = 0;
        Properties.Cp_1 = 0;
        Properties.Cp_2 = 0;
        Properties.Cp_3 = 0;
        Properties.Cp_4 = 0;
        Properties.Cp_5 = 0;
        Properties.Specific_Heat_Ratio = 1.304;

        Properties.mixture = true ;            // possible mixture
        Properties.Xvol = 1. ;                 // pure gas by default
        Properties.LHV = 13.8486948056;                     // Ref (http://www.engineeringtoolbox.com/)
        Properties.HHV_NormalConditions = 39.72 ;                // MJ/Nm3 mean value, dry basis http://en.citizendium.org/wiki/Heat_of_combustion
        Properties.RHO_NormalConditions = 0.71568 ;                // kg/Nm3 1.01325 bar, 0�C

        break ;

    case EV::Fluid_Type::CH4_H2:

        // Ref Air Liquide
        Properties.Name = "CH4-H2";
        Properties.Phase = EV::Fluid_Phase::Gas;
        Properties.Type = EV::Fluid_Type::CH4_H2;
        Properties.Critical_Temperature = 0;
        Properties.Critical_Pressure = 0;
        Properties.Critical_Density = 0;
        Properties.Critical_Compressibility_Factor = 0;
        Properties.Acentric_Factor = 0;
        Properties.Molar_Mass = 0;
        Properties.Mass_Density_a = 0;
        Properties.Mass_Density_b = 0;
        Properties.Mass_Density_c = 0;
        Properties.Mass_Density_d = 0;
        Properties.DeltaH0 = 0;
        Properties.DeltaG0 = 0;
        Properties.S0 = 0;

        Properties.a1 = 0.0;
        Properties.a2 = 0.0;
        Properties.a3 = 0.0;
        Properties.a4 = 0.0;
        Properties.a5 = 0.0;

        Properties.Cp_0 = 0;
        Properties.Cp_1 = 0;
        Properties.Cp_2 = 0;
        Properties.Cp_3 = 0;
        Properties.Cp_4 = 0;
        Properties.Cp_5 = 0;
        Properties.Specific_Heat_Ratio = 0;

        break ;

    case EV::Fluid_Type::H2OLiq:

        Properties.Name = "H2OLiq";
        Properties.Phase = EV::Fluid_Phase::Liquid;
        Properties.Type = EV::Fluid_Type::H2OLiq;
        Properties.Critical_Temperature = 0;
        Properties.Critical_Pressure = 0;
        Properties.Critical_Density = 0;
        Properties.Critical_Compressibility_Factor = 0;
        Properties.Acentric_Factor = 0;
        Properties.Molar_Mass = 0.0180153;
        Properties.Mass_Density_a = 1000.9971;
        Properties.Mass_Density_b = -0.0825;
        Properties.Mass_Density_c = -0.0038;
        Properties.Mass_Density_d = 4.3121e-6;
        Properties.DeltaH0 = -285.5567e3;
        Properties.DeltaG0 = -236.95166e3;
        Properties.S0 = 69.8478;

        Properties.a1 = 0.0;
        Properties.a2 = 0.0;
        Properties.a3 = 0.0;
        Properties.a4 = 0.0;
        Properties.a5 = 0.0;

        Properties.Cp_0 = 3.0034E+03;                   // Source ThermExcel + (See "Fluid_Properties.xlsx")
        Properties.Cp_1 = - 4.3170E+01;
        Properties.Cp_2 = 2.5440E-01;
        Properties.Cp_3 = - 7.4892E-04;
        Properties.Cp_4 = 1.1010E-06;
        Properties.Cp_5 = -6.4633E-10;
        Properties.Specific_Heat_Ratio = 1.33;

        break ;

    case EV::Fluid_Type::Steam:

        // Ref Air Liquide
        Properties.Name = "Steam";
        Properties.Phase = EV::Fluid_Phase::Gas;
        Properties.Type = EV::Fluid_Type::Steam;
        Properties.Critical_Temperature = 0;
        Properties.Critical_Pressure = 0;
        Properties.Critical_Density = 0;
        Properties.Critical_Compressibility_Factor = 0;
        Properties.Acentric_Factor = 0;
        Properties.Molar_Mass = 0.0180153;
        Properties.Mass_Density_a = 1000.9971;
        Properties.Mass_Density_b = -0.0825;
        Properties.Mass_Density_c = -0.0038;
        Properties.Mass_Density_d = 4.3121e-6;
        Properties.DeltaH0 = -241.814e3;                      // Source Prosim+
        Properties.DeltaG0 = -236.95166e3;
        Properties.S0 = 69.8478;

        Properties.a1 = 2.67703787;
        Properties.a2 = 2.97318329e-3;
        Properties.a3 = -7.73769690e-7;
        Properties.a4 = 9.44336689e-11;
        Properties.a5 = -4.26900959e-15;

        Properties.Cp_0 = 3.1760E+01;                         // Source http://www.engineeringtoolbox.com/water-vapor-d_979.html (see Fluid_Properties.xlsx)
        Properties.Cp_1 = 3.8273E-03;
        Properties.Cp_2 = 9.6739E-06;
        Properties.Cp_3 = -4.9411E-09;
        Properties.Cp_4 = 8.9524E-13;
        Properties.Cp_5 = -5.6056E-17;
        Properties.Specific_Heat_Ratio = 1.33;

        break ;

    case EV::Fluid_Type::LOX:

        Properties.Name = "LOX";
        Properties.Phase = EV::Fluid_Phase::Liquid;
        Properties.Type = EV::Fluid_Type::LOX;
        Properties.Critical_Temperature = 154.58;   //https://encyclopedia.airliquide.com/fr/oxygene
        Properties.Critical_Pressure = 50.43;
        Properties.Critical_Density = 436.14;
        Properties.Critical_Compressibility_Factor = 0;
        Properties.Acentric_Factor = 0;
        Properties.Molar_Mass = 31.999e-3;
        Properties.Mass_Density_a = 1141.2;  // degC, 1.013e5Pa   https://technifab.com/cryogenic-resource-library/cryogenic-fluids/liquid-oxygen/
        Properties.Mass_Density_b = 0;
        Properties.Mass_Density_c = 0;
        Properties.Mass_Density_d = 0;
        Properties.DeltaH0 = 0;
        Properties.DeltaG0 = 0;
        Properties.S0 = 0;

        Properties.a1 = 0.0;
        Properties.a2 = 0.0;
        Properties.a3 = 0.0;
        Properties.a4 = 0.0;
        Properties.a5 = 0.0;

        Properties.Cp_0 = 1.70e3;  // 0 degC, 1.013e5Pa   https://technifab.com/cryogenic-resource-library/cryogenic-fluids/liquid-oxygen/
        Properties.Cp_1 = 0;
        Properties.Cp_2 = 0;
        Properties.Cp_3 = 0;
        Properties.Cp_4 = 0;
        Properties.Cp_5 = 0;
        Properties.Specific_Heat_Ratio = 0;
        Properties.LHV = 212.9e3;  // 0 degC, 1.013e5Pa   https://technifab.com/cryogenic-resource-library/cryogenic-fluids/liquid-oxygen/

        Properties.RHO_NormalConditions = 1.4269 ;                // kg/Nm3 1bar, 15�C
        break ;

    case EV::Fluid_Type::LIN:

        Properties.Name = "LIN";
        Properties.Phase = EV::Fluid_Phase::Liquid;
        Properties.Type = EV::Fluid_Type::LIN;
        Properties.Critical_Temperature = 126.19;
        Properties.Critical_Pressure = 33.96;
        Properties.Critical_Density = 313.3;
        Properties.Critical_Compressibility_Factor = 0;
        Properties.Acentric_Factor = 0;
        Properties.Molar_Mass = 28.013e-3;
        Properties.Mass_Density_a = 808.6;  // 0 degC, 1.013e5Pa   https://technifab.com/cryogenic-resource-library/cryogenic-fluids/liquid-oxygen/
        Properties.Mass_Density_b = 0;
        Properties.Mass_Density_c = 0;
        Properties.Mass_Density_d = 0;
        Properties.DeltaH0 = 0;
        Properties.DeltaG0 = 0;
        Properties.S0 = 0;

        Properties.a1 = 0.0;
        Properties.a2 = 0.0;
        Properties.a3 = 0.0;
        Properties.a4 = 0.0;
        Properties.a5 = 0.0;

        Properties.Cp_0 = 2.04e3;    // 0 degC, 1.013e5Pa   https://technifab.com/cryogenic-resource-library/cryogenic-fluids/liquid-oxygen/
        Properties.Cp_1 = 0;
        Properties.Cp_2 = 0;
        Properties.Cp_3 = 0;
        Properties.Cp_4 = 0;
        Properties.Cp_5 = 0;
        Properties.Specific_Heat_Ratio = 0;
        Properties.LHV = 198.3e3;    // 0 degC, 1.013e5Pa   https://technifab.com/cryogenic-resource-library/cryogenic-fluids/liquid-oxygen/

        Properties.RHO_NormalConditions = 1.25125 ;                // kg/Nm3 1bar, 15�C
        break ;

    case EV::Fluid_Type::CO2:

        Properties.Name = "CO2";
        Properties.Phase = EV::Fluid_Phase::Gas;
        Properties.Type = EV::Fluid_Type::CO2;
        Properties.Critical_Temperature = 30.98;     // https://encyclopedia.airliquide.com/carbon-dioxide
        Properties.Critical_Pressure = 73.773;
        Properties.Critical_Density = 467.6;
        Properties.Critical_Compressibility_Factor = 9.9326e-1;
        Properties.Acentric_Factor = -1.e99 ;    // unknown
        Properties.Molar_Mass = 44.01e-3;
        Properties.Mass_Density_a = 1.9763;  // 0 degC, 1.013e5Pa
        Properties.Mass_Density_b = 0;
        Properties.Mass_Density_c = 0;
        Properties.Mass_Density_d = 0;
        Properties.DeltaH0 = 0;
        Properties.DeltaG0 = 0;
        Properties.S0 = 0;

        Properties.a1 = 0.0;
        Properties.a2 = 0.0;
        Properties.a3 = 0.0;
        Properties.a4 = 0.0;
        Properties.a5 = 0.0;

        Properties.Cp_0 = 8.2684e2;    // J/kg 0 degC, 1.013e5Pa
        Properties.Cp_1 = 0;
        Properties.Cp_2 = 0;
        Properties.Cp_3 = 0;
        Properties.Cp_4 = 0;
        Properties.Cp_5 = 0;
        Properties.Specific_Heat_Ratio = 1.3083 ;

        break ;

      case EV::Fluid_Type::Fuel:
      case EV::Fluid_Type::UnknownFluid:
      default:

//        qWarning() << "Fill_Fluid_Properties : user defined Fluid - no default properties will be set " << Type ;
        break ;

    }

    // avoid multiple runtime computations
    Properties.Gas_r = Universal_Parameters.R / Properties.Molar_Mass ;

    return Properties;
}
EV::Fluid_Type EnergyVector::getFluidTypeFromQString(const QString FluidName)
{
   if (FluidName == "FluidH2")
       return EV::Fluid_Type::H2 ;
   else if  (FluidName == "FluidFuel")
       return EV::Fluid_Type::Fuel ;
   else if  (FluidName == "FluidCH4")
       return EV::Fluid_Type::CH4 ;
   else if  (FluidName == "FluidCH4_H2")
       return EV::Fluid_Type::CH4_H2 ;
   else if  (FluidName == "FluidCO2")
       return EV::Fluid_Type::CO2 ;
   else if  (FluidName == "FluidO2")
       return EV::Fluid_Type::CO2 ;
   else if  (FluidName == "FluidAir")
       return EV::Fluid_Type::Air ;
   else if  (FluidName == "FluidH2O")
       return EV::Fluid_Type::H2OLiq ;
   else if  (FluidName == "FluidSteam")
       return EV::Fluid_Type::Steam ;
   else if  (FluidName == "FluidLOX")
       return EV::Fluid_Type::LOX ;
   else if  (FluidName == "FluidLIN")
       return EV::Fluid_Type::LIN ;
   else
       qWarning() << " User defined Fluid - no default properties will be set for " << FluidName ;
       return EV::Fluid_Type::UnknownFluid ;
}
const EV::Fluid_Properties *EnergyVector::Get_Pointer_To_Fluid_Properties(EV::Fluid_Type Type)
{
    switch (Type)
    {
      case EV::Fluid_Type::H2:

        return &H2;

      case EV::Fluid_Type::O2:

        return &O2;

    case EV::Fluid_Type::Air:

        return &Air;

    case EV::Fluid_Type::CH4:

        return &CH4;

    case EV::Fluid_Type::CH4_H2:

        return &CH4_H2;

    case EV::Fluid_Type::H2OLiq:

        return &H2OLiq;

    case EV::Fluid_Type::Steam:

        return &Steam;

    case EV::Fluid_Type::LOX:

        return &LOX;

    case EV::Fluid_Type::LIN:

        return &LIN;

    case EV::Fluid_Type::CO2:

        return &CO2;

    case EV::Fluid_Type::Fuel:

        return &Fuel;

    case EV::Fluid_Type::UnknownFluid:
    default:

        qWarning () << "ERROR: Get_Pointer_To_Fluid_Properties - Fluid_Type unknown" ;
        return nullptr;
    }
}
double                                          EnergyVector::Compute_H(double Temperature, double Pressure, const EV::Fluid_Properties *p_Matter_Properties)
{
    // Compute H in J/mol
    double  Temp_K = Deg2Kel(Temperature),
            H = p_Matter_Properties->DeltaH0
                + p_Matter_Properties->Cp_0 * Temp_K
                + p_Matter_Properties->Cp_1 / 2 * pow(Temp_K, 2)
                + p_Matter_Properties->Cp_2 / 3 * pow(Temp_K, 3)
                + p_Matter_Properties->Cp_3 / 4 * pow(Temp_K, 4)
                + p_Matter_Properties->Cp_4 / 5 * pow(Temp_K, 5)
                + p_Matter_Properties->Cp_5 / 6 * pow(Temp_K, 6)
                -
                (+ p_Matter_Properties->Cp_0 * 298.15
                + p_Matter_Properties->Cp_1 / 2 * pow(298.15, 2)
                + p_Matter_Properties->Cp_2 / 3 * pow(298.15, 3)
                + p_Matter_Properties->Cp_3 / 4 * pow(298.15, 4)
                + p_Matter_Properties->Cp_4 / 5 * pow(298.15, 5)
                + p_Matter_Properties->Cp_5 / 6 * pow(298.15, 6));

    if (p_Matter_Properties->Phase == Liquid)
    {
        H += (Bar2Pa(Pressure) - 1e5)
           * p_Matter_Properties->Molar_Mass
           / Compute_Density(Temperature, p_Matter_Properties);
    }

    return H;
}
double                                          EnergyVector::Compute_Density(double Temperature, const EV::Fluid_Properties *p_Matter_Properties)
{
    // Compute density in kg/m3
    double  Temp_K = Deg2Kel(Temperature),
            Density = p_Matter_Properties->Mass_Density_a
                    + p_Matter_Properties->Mass_Density_b * Temp_K
                    + p_Matter_Properties->Mass_Density_c * pow(Temp_K, 2)
                    + p_Matter_Properties->Mass_Density_d * pow(Temp_K, 3);

    return Density;
}
double                                          EnergyVector::Compute_S(double Temperature, double Pressure, const EV::Fluid_Properties *p_Matter_Properties)
{
    // Compute S in J/DegC/mol
    double  Temp_K = Deg2Kel(Temperature),

            S   = p_Matter_Properties->S0
                + p_Matter_Properties->Cp_0 * log(Temp_K)
                + p_Matter_Properties->Cp_1 * Temp_K
                + p_Matter_Properties->Cp_2 / 2 * pow(Temp_K, 2)
                + p_Matter_Properties->Cp_3 / 3 * pow(Temp_K, 3)
                + p_Matter_Properties->Cp_4 / 4 * pow(Temp_K, 4)
                + p_Matter_Properties->Cp_5 / 5 * pow(Temp_K, 5)
                -
                (+ p_Matter_Properties->Cp_0 * log(298.15)
                + p_Matter_Properties->Cp_1 * 298.15
                + p_Matter_Properties->Cp_2 / 2 * pow(298.15, 2)
                + p_Matter_Properties->Cp_3 / 3 * pow(298.15, 3)
                + p_Matter_Properties->Cp_4 / 4 * pow(298.15, 4)
                + p_Matter_Properties->Cp_5 / 5 * pow(298.15, 5));

    if (p_Matter_Properties->Phase == Gas)
    {
        S -= (Universal_Parameters.R * log(Pressure / 1));
    }

    return S;
}
double                                          EnergyVector::Compute_Cp(double Temperature, const EV::Fluid_Properties *p_Fluid_Properties)
{
    // Compute Cp in J/DegC/kg from Temperature in DegC
    double  Temp_K = Deg2Kel(Temperature),
            Cp = 0.0;

    if (p_Fluid_Properties->a1 != 0.0)
    {
        // Use of NASA equation and coefficients
        Cp  = (Universal_Parameters.R * (p_Fluid_Properties->a1
                                                        + p_Fluid_Properties->a2 * Temp_K
                                                        + p_Fluid_Properties->a3 * pow(Temp_K, 2.0)
                                                        + p_Fluid_Properties->a4 * pow(Temp_K, 3.0)
                                                        + p_Fluid_Properties->a5 * pow(Temp_K, 4.0)))
              / p_Fluid_Properties->Molar_Mass;
    }
    else
    {
        // Use of previous method
            Cp  = (p_Fluid_Properties->Cp_0
                + p_Fluid_Properties->Cp_1 * Temp_K
                + p_Fluid_Properties->Cp_2 * pow(Temp_K, 2)
                + p_Fluid_Properties->Cp_3 * pow(Temp_K, 3)
                + p_Fluid_Properties->Cp_4 * pow(Temp_K, 4)
                + p_Fluid_Properties->Cp_5 * pow(Temp_K, 5))
                / p_Fluid_Properties->Molar_Mass;
    }

    return Cp;
}
double EnergyVector::Bar2Pa(double P_Bar)
{
    return (P_Bar * 1e5);
}
double EnergyVector::Pa2Bar(double P_Pa)
{
    return (P_Pa / 1e5);
}
double EnergyVector::Deg2Kel(double T_Deg)
{
    return (T_Deg + 273.15);
}
double EnergyVector::Kel2Deg(double T_Kel)
{
    return (T_Kel - 273.15);
}

// convert mass fraction of fluid 1 in Fluid1+Fluid2 mixture into molar volume fraction of Fluid1.
double EnergyVector::Mass2VolFrac(double Xmass_F1, EV::Fluid_Properties F1, EV::Fluid_Properties F2)
{
    return (Xmass_F1 / F1.Molar_Mass) / (Xmass_F1 / F1.Molar_Mass + (1-Xmass_F1) / F2.Molar_Mass) ;
}

double EnergyVector::Vol2MassFrac(double Xmolar_F1, EV::Fluid_Properties F1, EV::Fluid_Properties F2)
{
    return Xmolar_F1 * F1.Molar_Mass / (Xmolar_F1 * F1.Molar_Mass + (1-Xmolar_F1)*F2.Molar_Mass);
}
