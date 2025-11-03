#include "Cairn_Exception.h"
#include "MilpPort.h"
#include "GlobalSettings.h"

using namespace GS;

MilpPort::MilpPort(CairnObject *aParent, std::string aID, std::string aName, const std::map<std::string, std::string> aPort): CairnObject(aParent),
    mID(aID),
    mPosition(CairnUtils::getParam(aPort,"Position")),
    mCarrierType(CairnUtils::getParam(aPort,"CarrierType")),
    mIsDefaultPort(false), 
    mIsEnabled(CairnUtils::getParam(aPort,"Enabled")),
    mBusType(""), //BusFlowBalance, BusSameValue, or MultiObjCompo 
    mBusPortName(CairnUtils::getParam(aPort,"BusPortName")), //The name of the linked Bus port  
    mBusPortPosition(""), //The position of the linked Bus port  
    mFluxUnit(nullptr),
    mStorageUnit(nullptr),
    mPowerUnit(nullptr),
    mMassUnit(nullptr),
    mPotentialUnit(nullptr)
{
    setName(aName);
    setObjectType("MilpPort");
    setCompoName(CairnUtils::getParam(aPort, "CompoName"));
    mInputParam = new InputParam(this, "InputParam" + Name());
    declareParameters();
    setParameters(aPort);
}

MilpPort::~MilpPort()
{
    mLinkedBus = nullptr;
    mCarrier = nullptr;
    mFluxUnit = nullptr;
    mStorageUnit = nullptr;
    mPowerUnit = nullptr;
    mMassUnit = nullptr;
    mPotentialUnit = nullptr;
    mFlux.clear();
    mPotential.clear();
    if (mInputParam) delete mInputParam;
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

void MilpPort::declareParameters() {
    //std::string
    mInputParam->addParameter("Direction", &mDirection, "", false, true, "Port direction");
    mInputParam->addParameter("Variable", &mVariable, "", true, true, "Port variable");
    mInputParam->addParameter("CheckUnit", &mVarCheckUnit, "Yes", false, true, "Port checkUnit");
    //double
    mInputParam->addParameter("Coeff", &mVarCoeff, 1., false, true, "Port coeff");
    mInputParam->addParameter("Offset", &mVarOffset, 0., false, true, "Port offset");
}

void MilpPort::setParameters(const std::map<std::string, std::string>& portParams) 
{
    mInputParam->readParameters(portParams);

    if (CairnUtils::getParam(portParams, "IsDefaultPort") == "Yes") {
        mIsDefaultPort = true;
    }
    
    if (mIsEnabled == "") {
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

void MilpPort::completePortInfo(std::map<std::string, std::string>& portParams) {
    //portParams.remove("Direction");
    //portParams.remove("Variable");
    mInputParam->readParameters(portParams);
    setName(portParams["Name"]);
    setCompoName(portParams["CompoName"]);
    mBusPortName = portParams["BusPortName"];
    mPosition = portParams["Position"];
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

void MilpPort::setPortType(std::string aBusType)
{
    mBusType=aBusType;
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
std::string MilpPort::PotentialUnit() const 
{
    if (mPotentialUnit) return *mPotentialUnit;
    return "";
}

void MilpPort::setCarrier(EnergyVector* aptrEnergyVector)
{ 
    mCarrier = aptrEnergyVector; 
    if (mCarrier) {
        mFluxUnit = mCarrier->pFluxUnit(); 
        mStorageUnit = mCarrier->pStorageUnit();
        mPowerUnit = mCarrier->pPowerUnit();
        mMassUnit = mCarrier->pMassUnit();
        mPotentialUnit = mCarrier->pPotentialUnit();
        //if (mIsDefaultPort) { //What about old studies?!
        //    MilpComponent* lptrCompo = (MilpComponent*)this->parent();
        //    if (lptrCompo) {
        //        lptrCompo->declareIOVariables();
        //    }
        //}
    }
}

void MilpPort::setLinkedBus(MilpComponent* aLinkedBus) {
    mLinkedBus = aLinkedBus;
}

void MilpPort::DeleteLinkedBus()
{    
    if (mLinkedBus) {
        mLinkedBus = nullptr;
    }
}

void MilpPort::setFlux(const unsigned int &aTime, const double &aSignedCoeff, MIPModeler::MIPExpression &aFluxExpression)
{
    mFlux[aTime] = mVarCoeff * aSignedCoeff * aFluxExpression + mVarOffset ;
    mTimeDependant = 1;
}
void MilpPort::setFlux0D(const double &aSignedCoeff, MIPModeler::MIPExpression &aFluxExpression)
{
    mFlux0D = mVarCoeff * aSignedCoeff * aFluxExpression;
    mTimeDependant = 0;
}

void MilpPort::setPotential(const unsigned int &aTime, MIPModeler::MIPExpression &aFluxExpression)
{
    mPotential[aTime] = mVarCoeff * aFluxExpression + mVarOffset ;
}

void MilpPort::jsonSaveGUIPortsData(ojson &nodePortArray, const bool& isBusPort)
{  
    std::string portId = mID;
    std::string portName = Name();
    std::string position = mPosition;
    std::string defaultport = No();
    if (mIsDefaultPort) defaultport = Yes();
    if (isBusPort) {
        /**
        * Bus port is a copy of the linked component port
        * Only change the port ID and port Name, 
        * and always define it as a non-default port
        */
        portId = "port" + std::to_string(nodePortArray.size() + 1);
        portName = mBusPortName;
        position = mBusPortPosition;
        defaultport = No();
    }
    ojson nodePort = ojson{
            {"id", portId},
            {"name", portName},
            {"position", position},
            {"carrier", CarrierName()},
            {"carrierType", mCarrierType},
            {"direction", mDirection},
            {"variable", mVariable},
            {"coeff", mVarCoeff},
            {"offset",mVarOffset},
            {"checkunit",mVarCheckUnit},
            {"defaultport", defaultport},
            {"enabled", mIsEnabled}
    };
    nodePortArray.push_back(nodePort);
}


std::string MilpPort::GAMSVarName()
{
    std::string aGAMSVarName = CompoName() + "_v_" + mVariable;
    return aGAMSVarName;
}
