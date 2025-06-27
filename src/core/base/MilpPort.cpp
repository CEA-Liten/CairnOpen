#include "Cairn_Exception.h"
#include "base/MilpPort.h"
#include "GlobalSettings.h"

using namespace GS;

MilpPort::MilpPort(QObject *aParent, QString aID, QString aName, const QMap<QString, QString> aPort): QObject(aParent),
    mID(aID),
    mName(aName),
    mPosition(aPort["Position"]),
    mCarrierName(aPort["Carrier"]),
    mCarrierType(aPort["CarrierType"]),
    mIsDefaultPort(false), 
    mIsEnabled(aPort["Enabled"]),
    mCompoName(aPort["CompoName"]),
    mLinkedComponent(aPort["LinkedComponent"]), //Linked Bus name
    mBusType(""), //BusFlowBalance, BusSameValue, or MultiObjCompo 
    mBusPortName(aPort["BusPortName"]), //The name of the linked Bus port  
    mBusPortPosition(""), //The position of the linked Bus port  
    mFluxUnit(nullptr),
    mStorageUnit(nullptr),
    mPowerUnit(nullptr),
    mMassUnit(nullptr),
    mPotentialUnit(nullptr)
{
      this->setObjectName(aName);

      mInputParam = new InputParam(this, "InputParam" + mName);
      declareParameters();
      setParameters(aPort);
}

MilpPort::~MilpPort()
{
    mptrLinkedComponent = nullptr;
    mEnergyVector = nullptr;
    mFluxUnit = nullptr;
    mStorageUnit = nullptr;
    mPowerUnit = nullptr;
    mMassUnit = nullptr;
    mPotentialUnit = nullptr;
    mFlux.clear();
    mPotential.clear();
    if (mInputParam) delete mInputParam;
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
    //QString
    mInputParam->addParameter("Name", &mName, "", false, "Port name");
    mInputParam->addParameter("Direction", &mDirection, "", false, "Port direction");
    mInputParam->addParameter("Variable", &mVariable, "", true, "Port variable");
    mInputParam->addParameter("CheckUnit", &mVarCheckUnit, "Yes", false, "Port checkUnit");
    //double
    mInputParam->addParameter("Coeff", &mVarCoeff, 1., false, "Port coeff");
    mInputParam->addParameter("Offset", &mVarOffset, 0., false, "Port offset");
}

void MilpPort::setParameters(const QMap<QString, QString>& portParams) 
{
    QString previousName = mName;
    mInputParam->readParameters(portParams);
    if (mName == "") {
        mName = previousName;
    }

    if (portParams["IsDefaultPort"] == "Yes") {
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

void MilpPort::completePortInfo(QMap<QString, QString>& portParams) {
    //portParams.remove("Direction");
    //portParams.remove("Variable");
    mInputParam->readParameters(portParams);
    mCarrierName = portParams["Carrier"];
    mCompoName = portParams["CompoName"];
    mLinkedComponent = portParams["LinkedComponent"];
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

void MilpPort::setPortType(QString aBusType)
{
    mBusType=aBusType;
}

QString MilpPort::getFluxUnit() const 
{
    if (mFluxUnit) return *mFluxUnit;
    return "";
}
QString MilpPort::getStorageUnit() const 
{
    if (mStorageUnit) return *mStorageUnit;
    return "";
}
QString MilpPort::getPotentialUnit() const 
{
    if (mPotentialUnit) return *mPotentialUnit;
    return "";
}

void MilpPort::setEnergyVector(EnergyVector* aptrEnergyVector) 
{ 
    mEnergyVector = aptrEnergyVector; 
    mFluxUnit = mEnergyVector->pFluxUnit(); 
    mStorageUnit = mEnergyVector->pStorageUnit(); 
    mPowerUnit = mEnergyVector->pPowerUnit(); 
    mMassUnit = mEnergyVector->pMassUnit(); 
    mPotentialUnit = mEnergyVector->pPotentialUnit(); 
}


void MilpPort::setptrLinkedComponent(MilpComponent* aptrLinkedComponent) {
    mptrLinkedComponent = aptrLinkedComponent;
    if (mptrLinkedComponent) {
        mLinkedComponent = mptrLinkedComponent->Name();
    }
}

void MilpPort::DeleteptrLinkedComponent()
{    
    if (mptrLinkedComponent) {
        mptrLinkedComponent = nullptr;
        mLinkedComponent = QString("");
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

void MilpPort::jsonSaveGUIPortsData(QJsonArray &nodePortArray, const bool& isBusPort)
{
    QJsonObject nodePort;
    QString portId = mID;
    QString portName = mName;
    QString position = mPosition;
    QString defaultport = No();
    if (mIsDefaultPort) defaultport = Yes();
    if (isBusPort) {
        /**
        * Bus port is a copy of the linked component port
        * Only change the port ID and port Name, 
        * and always define it as a non-default port
        */
        portId = "port" + QString::number(nodePortArray.count() + 1);
        portName = mBusPortName;
        position = mBusPortPosition;
        defaultport = No();
    }
    nodePort.insert("id", portId);
    nodePort.insert("name", portName);
    nodePort.insert("position", position);
    nodePort.insert("carrier", mEnergyVector->Name()); //==mCarrierName
    nodePort.insert("carrierType", mCarrierType);
    nodePort.insert("direction", mDirection);
    nodePort.insert("variable", mVariable);
    nodePort.insert("coeff", mVarCoeff);
    nodePort.insert("offset",mVarOffset) ;
    nodePort.insert("checkunit",mVarCheckUnit);
    nodePort.insert("defaultport", defaultport);
    nodePort.insert("enabled", mIsEnabled);
    nodePortArray.push_back(nodePort);
}


std::string MilpPort::GAMSVarName()
{
    QString aGAMSVarName = mCompoName + "_v_" + mVariable;
    return aGAMSVarName.toStdString ();
}
