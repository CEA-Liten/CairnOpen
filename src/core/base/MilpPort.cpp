
#include "MilpPort.h"
#include "BusCompo.h"
#include "MaterialCarrier.h"
#include "TechnicalSubModel.h"

MilpPort::MilpPort(CairnObject* aParent, const std::string& aID, const std::string& aName, 
    const t_mapParamData& aPort, EnergyVector * carrier)
    : CairnObject(aParent),
    mID(aID),
    mPosition(CairnUtils::getParamValue(aPort,"Position")),
    mCarrierType(CairnUtils::getParamValue(aPort,"CarrierType")),
    mIsDefaultPort(false), 
    mIsEnabled(CairnUtils::getParamValue(aPort,"Enabled")),
    mBusType(""), //BusFlowBalance, BusSameValue, or MultiObjCompo 
    mBusPortName(CairnUtils::getParamValue(aPort,"BusPortName")), //The name of the linked Bus port  
    mBusPortPosition(""), //The position of the linked Bus port  
    mFluxUnit(nullptr),
    mStorageUnit(nullptr),
    mPowerUnit(nullptr)
{

    if (!aParent) {
        throw Cairn_Exception("The parent of port " + aID + "(" + aName + ") is null", -1);
    }

    setName(aName);
    setObjectType("MilpPort");

    mAttributes = new InputParam(this, "Attributes" + Name());

    declareAttributes();
    setAttributes(aPort);
    
    setCarrier(carrier);
}

MilpPort::~MilpPort()
{
    mLinkedBus = nullptr;
    mCarrier = nullptr;
    mFluxUnit = nullptr;
    mStorageUnit = nullptr;
    mPowerUnit = nullptr;
    mFlux.clear();
    mPotential.clear();
    delete mAttributes;
    delete mInputParam;
}

std::string MilpPort::CarrierName()
{
    if (mCarrier) {
        return mCarrier->Name();
    }
    return "";
}


std::string MilpPort::LinkedBusName() 
{
    if (mLinkedBus) {
        return mLinkedBus->Name();
    }
    return "";
}


int MilpPort::initProblem(const uint aNpdtTot)
{
    mFlux.clear();
    mFlux.resize(aNpdtTot);

    mPotential.clear();
    mPotential.resize(aNpdtTot);

    return 0;
}

void MilpPort::declareAttributes() {
    mAttributes->addParameter("Direction", &mDirection, "", false, true, "Port direction");
    mAttributes->addParameter("Variable", &mVariable, "", true, true, "Port variable");
    mAttributes->addParameter("CheckUnit", &mVarCheckUnit, "Yes", false, true, "Port checkUnit");

    mAttributes->addParameter("Coeff", &mVarCoeff, 1., false, true, "Port coeff");
    mAttributes->addParameter("Offset", &mVarOffset, 0., false, true, "Port offset");
}

void MilpPort::declareParameters()
{
    // -----------------------------------------
    // Only MilpComponent ports declare parameters
    // -----------------------------------------
    auto* parentObj = parent();
    if (!parentObj) // || parentObj->objectType() != "MilpComponent")
        return;

    auto* component = dynamic_cast<MilpComponent*>(parentObj);
    if (!component)
        return;

    // Reset input parameters
    delete mInputParam;
    mInputParam = new InputParam(this, "InputParam" + Name());

    // -----------------------------------------
    // EcoInvest parameters: only for TechnicalSubModel
    // -----------------------------------------
    auto* model = dynamic_cast<TechnicalSubModel*>(component->compoModel());
    if (model) {
        mInputParam->addParameter( "VariableOpex", &mVariableOpex, 0.0,
            false, true,
            "Variable Opex",
            SFunctionUnit({ eFTypeDivision, { pCurrency(), pQuantity("EnergyUnit") } }),
            "EcoInvestModel"
        );
    }
}

void MilpPort::setAttributes(const t_mapParamData& portParams)
{
    mAttributes->readParameters(portParams);

    if (CairnUtils::getParamValue(portParams, "IsDefaultPort") == "Yes") {
        mIsDefaultPort = true;
    }
    
    // TODO: use bool for  mIsEnabled ?!
    if (mIsEnabled.empty()) { 
        mIsEnabled = "true";
    }

    if (mCarrierType != "Fluid"
        && mCarrierType != "FluidH2"
        && mCarrierType != "FluidCH4"
        && mCarrierType != "ANY_Fluid"
        && mCarrierType != "Thermal"
        && mCarrierType != "Electrical"
        && mCarrierType != "ThermalOrElectrical"
        && mCarrierType != "Material")
    {
        mCarrierType = "ANY_TYPE";
    }

    setPosition();
}

void MilpPort::setParameters(const t_mapParamData& portParams)
{
    if (!mInputParam) return;
    mInputParam->readParameters(portParams);
}

void MilpPort::completePortInfo(const t_mapParamData& portParams, EnergyVector* carrier)
{
    setAttributes(portParams);
    setCarrier(carrier);
    setName(CairnUtils::getParamValue(portParams, "Name"));

    mBusPortName = CairnUtils::getParamValue(portParams, "BusPortName");
    mPosition = CairnUtils::getParamValue(portParams, "Position");

    setPosition();
}

void MilpPort::setPosition()
{
    if (mPosition == "") {
        if (mDirection == KCONS()) {
            mPosition = Left();
        }
        else if (mDirection == KPROD()) {
            mPosition = Right();
        }
        else if (mDirection == KDATA()) {
            mPosition = Bottom();
        }
        else {
            mPosition = Top();
        }
    }
}

void MilpPort::setVariable(std::string aVariable) {
    mVariable = aVariable; 
}

void MilpPort::setPortType(std::string aBusType)
{
    mBusType = aBusType;
}

std::string MilpPort::FluxUnit() const 
{
    if (mFluxUnit) return *mFluxUnit;
    return "";
}
std::string MilpPort::StorageUnit() const 
{
    if (mStorageUnit) return *mStorageUnit;
    return "";
}

double MilpPort::getParamValue(const std::string& a_ParamName, uint64_t t) const
{
    return mCarrier ?
        mCarrier->getParamValue(a_ParamName, t, dynamic_cast<const MilpComponent*>(parent()))
        : std::numeric_limits<double>::quiet_NaN();
}

double MilpPort::MolarMass(uint64_t t) const
{
    return mCarrier ?
        mCarrier->MolarMass(t, dynamic_cast<const MilpComponent*>(parent()))
        : std::numeric_limits<double>::quiet_NaN();
}

double MilpPort::MolarMass() const
{
    return mCarrier ?
        mCarrier->MolarMass()
        : std::numeric_limits<double>::quiet_NaN();
}

bool MilpPort::useLHV() const
{
    return mCarrier
        ? mCarrier->isParamExist("LHV")
        : false;
}

bool MilpPort::useProfileLHV() const
{
    if (!mCarrier) {
        throw Cairn_Exception("useProfileLHV called on port (" + ID() + ", " + Name() + ") with no carrier", -1);
    }
    return mCarrier->useProfileParam("LHV");
}

double MilpPort::LHV() const
{
    return mCarrier 
        ? mCarrier->getParamCstValue("LHV")
        : std::numeric_limits<double>::quiet_NaN();
}

const double MilpPort::LHV(const uint64_t t) const
{
    return mCarrier ?
        mCarrier->getParamValue("LHV", t, dynamic_cast<const MilpComponent*>(parent()) )      
        : std::numeric_limits<double>::quiet_NaN();
}

const double MilpPort::minLHV() const
{
    return mCarrier ?
        mCarrier->getMinParamValue("LHV", dynamic_cast<const MilpComponent*>(parent()))
        : std::numeric_limits<double>::quiet_NaN();   
}

bool MilpPort::useProfileGHV() const
{
    if (!mCarrier) {
        throw Cairn_Exception("useProfileGHV called on port (" + ID() + ", " + Name() + ") with no carrier", -1);
    }
    return mCarrier->useProfileParam("GHV");
}

double MilpPort::GHV() const 
{
    return mCarrier
        ? mCarrier->getParamCstValue("GHV")
        : std::numeric_limits<double>::quiet_NaN();
}

const std::vector<double>* MilpPort::getTimeSeries(const std::string& tsName) const
{
    if (!mCarrier)
        return nullptr;

    const MilpComponent* pParent = dynamic_cast<const MilpComponent*>(parent());
    if (!pParent)
        return nullptr;

    return pParent->getTimeSeries(tsName);
}

void MilpPort::setCarrier(EnergyVector* aptrEnergyVector)
{ 
    if (mCarrier == aptrEnergyVector)
        return;

    mCarrier = aptrEnergyVector; 

    if (mCarrier) {
        MilpComponent* pParent = dynamic_cast<MilpComponent*>(parent()); 

        updateUnits(pParent);

        //if (mIsDefaultPort) { //What about old studies?!
        //    MilpComponent* lptrCompo = (MilpComponent*)this->parent();
        //    if (lptrCompo) {
        //        lptrCompo->declareIOVariables();
        //    }
        //}

        declareParameters();

        if (pParent) {
            const auto portParams = pParent->portData(ID(), Name());
            setParameters(portParams);
        }
    }
}

void MilpPort::updateUnits(const MilpComponent* pParent) 
{
    mFluxUnit = mCarrier->pFluxUnit();
    mStorageUnit = mCarrier->pStorageUnit();  
    mPowerUnit = mCarrier->pPowerUnit();

    if (pParent && pParent->compoModel()) {
        mCurrency = pParent->compoModel()->pCurrency();
    }
}

void MilpPort::setLinkedBus(BusCompo* linkedBus) {
    mLinkedBus = linkedBus;
}

void MilpPort::setFlux(const unsigned int &aTime, const double &aSignedCoeff, MIPModeler::MIPExpression &aFluxExpression)
{
    mFlux[aTime] = aSignedCoeff * (mVarCoeff * aFluxExpression + mVarOffset);
    mTimeDependant = 1;
}
void MilpPort::setFlux0D(const double &aSignedCoeff, MIPModeler::MIPExpression &aFluxExpression)
{
    mFlux0D = aSignedCoeff * mVarCoeff * aFluxExpression;
    mTimeDependant = 0;
}

void MilpPort::setPotential(const unsigned int &aTime, MIPModeler::MIPExpression &aFluxExpression)
{
    mPotential[aTime] = mVarCoeff * aFluxExpression + mVarOffset ;
}

void MilpPort::jsonSaveGUIPortsData(ojson &nodePortArray, const bool& isBusLinkedPort)
{  
    std::string portId = mID;
    std::string portName = Name();
    std::string position = mPosition;
    std::string variable = mVariable;
    std::string defaultport = No();
    if (mIsDefaultPort) defaultport = Yes();

    if (isBusLinkedPort) {
        /**
        * A Bus linked port is a copy of the linked component port
        * => - change the port ID and port Name, 
        *    - set variable to empty
        *    - define it as a non-default port
        */
        portId = "port" + std::to_string(nodePortArray.size() + 1);
        portName = mBusPortName;
        position = mBusPortPosition;
        variable = "";
        defaultport = No();
    }

    // Attributes
    ojson nodePort = ojson{
            {"id", portId},
            {"name", portName},
            {"position", position},
            {"carrier", CarrierName()},
            {"carrierType", mCarrierType},
            {"direction", mDirection},
            {"variable", variable},
            {"coeff", mVarCoeff},
            {"offset",mVarOffset},
            {"checkunit",mVarCheckUnit},
            {"defaultport", defaultport},
            {"enabled", mIsEnabled}
    };

    // Parameters
    if (mInputParam) {
        nodePort["params"] = ojson::array();
        mInputParam->jsonSaveGUIInputParam(nodePort["params"]);
    }

    nodePortArray.push_back(nodePort);
}

bool MilpPort::checkCarrierType(const std::string& expectedTechnoType) const
{
    const EnergyVector* carrier = getCarrier();
    if (!carrier) {
        cCritical() << "Port " << ID()
            << " has no carrier assigned (expected " << expectedTechnoType << ")";
        return false;
    }

    const std::string actualTechnoType = carrier->TechnoType();
    if (actualTechnoType != expectedTechnoType) {
        cCritical() << "Port " << ID()
            << " has carrier type '" << actualTechnoType
            << "' but expected '" << expectedTechnoType << "'";
        return false;
    }

    return true;
}

bool MilpPort::checkCarrierType(const std::vector<std::string>& allowedTypes) const
{
    const EnergyVector* carrier = getCarrier();
    if (!carrier) {
        cCritical() << "Port " << ID()
            << " has no carrier assigned (expected one of: " << CairnUtils::joinStrings(allowedTypes) << ")";
        return false;
    }

    const std::string actual = carrier->TechnoType();

    // Check if actual type is in the allowed list
    const bool match = std::find(allowedTypes.begin(),
        allowedTypes.end(),
        actual) != allowedTypes.end();

    if (!match) {
        cCritical() << "Port " << ID()
            << " has carrier type '" << actual
            << "' but expected one of: " << CairnUtils::joinStrings(allowedTypes);
        return false;
    }

    return true;
}


std::string MilpPort::GAMSVarName()
{
    std::string aGAMSVarName = CompoName() + "_v_" + mVariable;
    return aGAMSVarName;
}


std::vector<InputParam*> MilpPort::get_InputParams()
{
   return { 
       getAttributes(), 
       getInputParam() 
   };    
}

double MilpPort::getCarrierTemperature(bool* ap_Ok)
{
    if (mCarrier != nullptr) {
        if (mCarrier->Type() == "MaterialCarrier") {
            MaterialCarrier* pCarrier = (MaterialCarrier*)mCarrier;
            if (ap_Ok) *ap_Ok = true;
            double vRet = pCarrier->getParamCstValue("Temperature");
            if (std::isnan(vRet)) {
                if (ap_Ok) *ap_Ok = false;
            }
            return vRet;
        }
    }
    if (ap_Ok) *ap_Ok = false;
    return 0.0;
}

double MilpPort::getCarrierPressure(bool* ap_Ok)
{
    if (mCarrier != nullptr) {
        if (mCarrier->Type() == "MaterialCarrier") {
            MaterialCarrier* pCarrier = (MaterialCarrier*)mCarrier;
            if (ap_Ok) *ap_Ok = true;
            return pCarrier->getParamCstValue("Pressure");
        }
    }
    if (ap_Ok) *ap_Ok = false;
    return 0.0;
}

const std::string* MilpPort::pQuantity(const std::string& a_Quantity) const {
    if (mCarrier != nullptr) {
        return mCarrier->pQuantity(a_Quantity);
    }
    return nullptr;
}
std::vector<InputParam*> MilpPort::get_AttributeInputParams()
{
    return {
        getAttributes()
    };
}

std::vector<InputParam*> MilpPort::get_ParamInputParams()
{
    return {
        getInputParam()
    };
}