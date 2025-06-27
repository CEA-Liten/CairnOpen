#ifndef MILPPORT_H
#define MILPPORT_H
class MilpPort ;

#include "MIPModeler.h"

#include <string.h>
using namespace std ;

#include "CairnCore_global.h"
#include "MilpComponent.h"
#include "EnergyVector.h"

/**
 * \brief The MilpPort class defines MilpComponent ports used to exchange MilpExpression with agregator (bus components)
 * Expression may be Flow (for balance) or Potential (simple value)
 */
class CAIRNCORESHARED_EXPORT MilpPort : public QObject
{
    Q_OBJECT
public:
    MilpPort(QObject *aParent, QString aID, QString aName, const QMap<QString, QString> aComponent);
    virtual ~MilpPort();

    virtual int initProblem(const uint aNpdtTot);
    InputParam* getInputParam() { return mInputParam; }
    void declareParameters();
    void setParameters(const QMap<QString, QString>& portParams);
    void completePortInfo(QMap<QString, QString>& portParams);

    QString ID() const { return mID; }
    QString Name() const { return mName; }
    QString Position() const { return mPosition; }
    QString Carrier() const { return mCarrierName; }
    QString CarrierType() const { return mCarrierType; }
    bool IsDefaultPort() const { return mIsDefaultPort; } 
    void setIsDefaultPort(const bool& isDefault) { mIsDefaultPort = isDefault; }

    QString Variable() const { return mVariable; }
    QString VarType() const { return mVarType; }
    QString Direction() const { return mDirection; }
    QString VarCheckUnit() const { return mVarCheckUnit; }
    double  VarCoeff() const { return mVarCoeff; }
    double  VarOffset() const { return mVarOffset; }

    QString CompoName() const { return mCompoName; }
    QString LinkedComponent() const {return mLinkedComponent ;}
    void setLinkedComponent(QString& aLinkedComponent) { mLinkedComponent = aLinkedComponent ;  };
    QString PortType() const { return mBusType; } //BusFlowBalance, BusSameValue, or MultiObjCompo 
    QString BusPortName() const { return mBusPortName; }
    void setBusPortName(const QString& aBusPortName)  { mBusPortName = aBusPortName; }
    QString BusPortPosition() const { return mBusPortPosition; }
    void setBusPortPosition(const QString& aBusPortPosition) { mBusPortPosition = aBusPortPosition; }

    QString VectorName() const {return mEnergyVector->Name() ;}
    std::string GAMSVarName();

    void setPosition();

    double  FluxDim() const {return mTimeDependant;}
    MIPModeler::MIPExpression1D & Flux(){return mFlux ;}
    MIPModeler::MIPExpression & Flux0D(){return mFlux0D ;}
    MIPModeler::MIPExpression1D & ExpPotential(){return mPotential ;}

    void setName(const QString &aName) {mName = aName;}
    void setPortType(QString aPortType);
    void setVarType(QString aVarType) {mVarType = aVarType;}
    void setVariable(QString aVariable) { mVariable = aVariable;}
    void setDirection(QString aDirection) { mDirection = aDirection;}
    void setVarCheckUnit(QString aVarCheckUnit) {mVarCheckUnit = aVarCheckUnit;}

    void setVarCoeff(double aVarCoeff) { mVarCoeff =aVarCoeff;}
    void setVarOffset(double aVarOffset) { mVarOffset = aVarOffset;}

    void setFlux(const unsigned int &aTime, const double &aSignedCoeff, MIPModeler::MIPExpression &aFluxExpression) ;
    void setFlux0D(const double &aSignedCoeff, MIPModeler::MIPExpression& aFluxExpression) ;
    void setPotential(const unsigned int &aTime, MIPModeler::MIPExpression &aPotentialExpression) ;

    EnergyVector* ptrEnergyVector() { return mEnergyVector; }
    void setEnergyVector(EnergyVector* aptrEnergyVector); 

    MilpComponent* ptrLinkedComponent() { return mptrLinkedComponent; }
    void setptrLinkedComponent(MilpComponent* aptrLinkedComponent);
    void DeleteptrLinkedComponent();

    const QString PotentialName() { 
        if(mEnergyVector) return mEnergyVector->PotentialName(); 
        return "";
    }

    const QString getFluxName() {
        if (mEnergyVector) return mEnergyVector->FluxName();
        return "";
    }

    const QString getStorageName() {
        if (mEnergyVector) return mEnergyVector->StorageName();
        return "";
    }

    const QString* pFluxUnit() const { return mFluxUnit; }
    const QString* pStorageUnit() const { return mStorageUnit; }
    const QString* pPowerUnit() const { return mPowerUnit; }
    const QString* pMassUnit() const { return mMassUnit; }
    const QString* pPotentialUnit() const { return mPotentialUnit; }

    QString getFluxUnit() const;
    QString getStorageUnit() const;
    QString getPotentialUnit() const;

    void jsonSaveGUIPortsData(QJsonArray& nodePortArray, const bool& isBusPort=false);

private:
    //Units and Phy. Names
    const QString* mFluxUnit;
    const QString* mStorageUnit;
    const QString* mPowerUnit;
    const QString* mMassUnit;
    const QString* mPotentialUnit;

    //Attributes
    QString mID;              /** Port unique Id */
    QString mName ;           /** Port Name */
    QString mPosition;       /** Used for port position on GUI */
    QString mCarrierName;   /** Name of Carrier (EnergyVector mEnergyVector) */
    QString mCarrierType;   /** Possible Carrier Type */
    QString mIsEnabled;     /** The port is enabled in the GUI only when the value is true */
    bool mIsDefaultPort;

    //Parameters
    QString mVariable ;          /** Expression Name to be used by the port*/
    QString mDirection;          /** Expression use case : indicates whether flow is a consumption INPUT or a production OUTPUT flow - relevent for converters only ! */
    QString mVarCheckUnit ;      /** QString telling if unit should be checked "Yes" or "No" */
    double mVarCoeff;            /** Multiplying coefficient to be applied to VarName Expression */
    double mVarOffset;           /** Offset coefficient to be applied to VarName Expression */
    
    //Associated 
    QString mCompoName;            /** Component Name of this port */
    QString mLinkedComponent;     /** Connected Bus Name */
    QString mBusType;            /** Bus Type (BusSameValue or BusFlowBalance) */
    QString mBusPortName;       /** The port name of the linked Bus - it is used to maintain the same name of Bus ports on the GUI*/
    QString mBusPortPosition;  /** The port position of the linked Bus - it is used to maintain the same position of Bus ports on the GUI*/

    QString mVarType;           /** Expression Type Scalar or Vector to be carried */
    MIPModeler::MIPExpression1D mFlux;       /** MIP expression of flux based on Variable mFluxVarName */
    MIPModeler::MIPExpression mFlux0D;       /** MIP expression of flux based on Variable mFluxVarName */
    MIPModeler::MIPExpression1D mPotential;  /** MIP expression of potential based on Variable mPotentialVarName */
    double mTimeDependant;               /** 0 if not timeDependant, 1 else*/

    EnergyVector* mEnergyVector{ nullptr }; //EnergyVector is set from the linked bus componenet (which has an Option "VectorName")
    MilpComponent* mptrLinkedComponent{ nullptr };
    InputParam* mInputParam{ nullptr };
};

#endif // MILPPORT_H
