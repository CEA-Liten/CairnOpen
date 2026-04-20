#ifndef MILPPORT_H
#define MILPPORT_H

class BusCompo;

#include "MIPModeler.h"
#include "CairnCore_global.h"

#include "EnergyVector.h"
#include "GlobalSettings.h"

using namespace std;
using namespace GS;

/**
 * \brief The MilpPort class defines MilpComponent ports used to exchange MilpExpression with agregator (bus components)
 * Expression may be Flow (for balance) or Potential (simple value)
 */
class CAIRNCORESHARED_EXPORT MilpPort : public CairnObject
{
    
public:
    MilpPort(CairnObject *aParent, std::string aID, std::string aName, const std::map<std::string, std::string> aComponent);
    virtual ~MilpPort();

    virtual int initProblem(const uint aNpdtTot);
    InputParam* getInputParam() { return mInputParam; }
    void declareParameters();
    void setParameters(const std::map<std::string, std::string>& portParams);
    void completePortInfo(std::map<std::string, std::string>& portParams);

    std::string ID() const { return mID; }
    std::string Name() const { return this->objectName(); }
    std::string Position() const { return mPosition; }
    std::string CarrierName();
    std::string CarrierType() const { return mCarrierType; }
    bool IsDefaultPort() const { return mIsDefaultPort; } 
    void setIsDefaultPort(const bool& isDefault) { mIsDefaultPort = isDefault; }

    std::string Variable() const { return mVariable; }
    std::string Direction() const { return mDirection; }
    std::string VarCheckUnit() const { return mVarCheckUnit; }
    double  VarCoeff() const { return mVarCoeff; }
    double  VarOffset() const { return mVarOffset; }

    std::string CompoName() { return std::string(this->parent()->objectName().c_str()); }
    std::string LinkedBusName();
    std::string PortType() const { return mBusType; } //BusFlowBalance, BusSameValue, or MultiObjCompo 
    std::string BusPortName() const { return mBusPortName; }
    void setBusPortName(const std::string& aBusPortName)  { mBusPortName = aBusPortName; }
    std::string BusPortPosition() const { return mBusPortPosition; }
    void setBusPortPosition(const std::string& aBusPortPosition) { mBusPortPosition = aBusPortPosition; }

    std::string GAMSVarName();

    void setPosition();

    double  FluxDim() const {return mTimeDependant;}
    MIPModeler::MIPExpression1D & Flux(){return mFlux ;}
    MIPModeler::MIPExpression & Flux0D(){return mFlux0D ;}
    MIPModeler::MIPExpression1D & ExpPotential(){return mPotential ;}

    void setName(const std::string& name) { this->setObjectName(name); }
    void setCompoName(const std::string& name) { this->parent()->setObjectName(name); }
    void setPortType(std::string aPortType);
    void setVariable(std::string aVariable);
    void setDirection(std::string aDirection) { mDirection = aDirection;}
    void setVarCheckUnit(std::string aVarCheckUnit) {mVarCheckUnit = aVarCheckUnit;}

    void setVarCoeff(double aVarCoeff) { mVarCoeff =aVarCoeff;}
    void setVarOffset(double aVarOffset) { mVarOffset = aVarOffset;}

    void setFlux(const unsigned int &aTime, const double &aSignedCoeff, MIPModeler::MIPExpression &aFluxExpression) ;
    void setFlux0D(const double &aSignedCoeff, MIPModeler::MIPExpression& aFluxExpression) ;
    void setPotential(const unsigned int &aTime, MIPModeler::MIPExpression &aPotentialExpression) ;

    EnergyVector* getCarrier() { return mCarrier; }
    void setCarrier(EnergyVector* aptrEnergyVector);

    BusCompo* getLinkedBus() { return mLinkedBus; }
    void setLinkedBus(BusCompo* linkedBus);

    const std::string PotentialName() { 
        if(mCarrier) return mCarrier->PotentialName();
        return "";
    }

    const std::string getFluxName() {
        if (mCarrier) return mCarrier->FluxName();
        return "";
    }

    const std::string getStorageName() {
        if (mCarrier) return mCarrier->StorageName();
        return "";
    }

    const std::string* pFluxUnit() const { return mFluxUnit; }
    const std::string* pStorageUnit() const { return mStorageUnit; }
    const std::string* pPowerUnit() const { return mPowerUnit; }
    const std::string* pMassUnit() const { return mMassUnit; }
    const std::string* pPotentialUnit() const { return mPotentialUnit; }

    std::string FluxUnit() const;
    std::string StorageUnit() const;
    std::string PotentialUnit() const;

    bool useProfileLHV() const;
    bool useProfileGHV() const;

    double LHV() const;
    const double LHV(const uint64_t t) const;
    const double minLHV() const;

    double GHV() const;

    const std::vector<double>* LHVProfile() const;
    const std::vector<double>* GHVProfile() const;

    void jsonSaveGUIPortsData(ojson& nodePortArray, const bool& isBusLinkedPort = false);

    std::vector<InputParam*> get_InputParams();

private:
    InputParam* mInputParam{ nullptr };

    EnergyVector* mCarrier{ nullptr };  
    BusCompo* mLinkedBus{ nullptr };

    //Units and Phy. Names
    const std::string* mFluxUnit;
    const std::string* mStorageUnit;
    const std::string* mPowerUnit;
    const std::string* mMassUnit;
    const std::string* mPotentialUnit;

    //Attributes
    std::string mID;              /** Port unique Id */
    std::string mPosition;       /** Used for port position on GUI */
    std::string mCarrierType;   /** Possible Carrier Type */
    std::string mIsEnabled;     /** The port is enabled in the GUI only when the value is true */
    bool mIsDefaultPort;

    //Parameters
    std::string mVariable ;          /** Expression Name to be used by the port*/
    std::string mDirection;          /** Expression use case : indicates whether flow is a consumption INPUT or a production OUTPUT flow - relevent for converters only ! */
    std::string mVarCheckUnit ;      /** std::string telling if unit should be checked "Yes" or "No" */
    double mVarCoeff;            /** Multiplying coefficient to be applied to VarName Expression */
    double mVarOffset;           /** Offset coefficient to be applied to VarName Expression */
    
    //Associated 
    std::string mBusType;            /** Bus Type (BusSameValue or BusFlowBalance) */
    std::string mBusPortName;       /** The port name of the linked Bus - it is used to maintain the same name of Bus ports on the GUI*/
    std::string mBusPortPosition;  /** The port position of the linked Bus - it is used to maintain the same position of Bus ports on the GUI*/

    MIPModeler::MIPExpression1D mFlux;       /** MIP expression of flux based on Variable mFluxVarName */
    MIPModeler::MIPExpression mFlux0D;       /** MIP expression of flux based on Variable mFluxVarName */
    MIPModeler::MIPExpression1D mPotential;  /** MIP expression of potential based on Variable mPotentialVarName */
    double mTimeDependant;               /** 0 if not timeDependant, 1 else*/

    const std::vector<double>* getTimeSeries(const std::string& tsName) const;
};

#endif // MILPPORT_H
