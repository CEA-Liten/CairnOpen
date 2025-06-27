#ifndef EnvImpact_H
#define EnvImpact_H
class EnvImpact;

#include "CairnCore_global.h"
#include "InputParam.h"
#include "MilpPort.h"
#include <cmath>

/**
 * \brief The EnvImpact class is the base class for each environmental impact
 */
class CAIRNCORESHARED_EXPORT EnvImpact 
{    
public:

    EnvImpact(QObject* aParent = nullptr, QString aName = "", QString aShortName = "");
    EnvImpact(const QString &aName, const QString &aShortName, const QString& aUnit);
    ~EnvImpact();

    void computeEnvGreyImpactContribution(MIPModeler::MIPExpression aExpSizeMax);
    void computeEnvGreyImpactReplacement(MIPModeler::MIPExpression aExpSizeMax);
    void computeEnvImpactContribution(const int j, const MIPModeler::MIPExpression1D* aFlux);
    void computeEnvImpactContributionCost();
    
    void setTimesteps(std::vector<double> aTimesteps) { mTimeSteps = aTimesteps; }
    void setCapex(double aCapex) { mCapex = aCapex; }
    void setLifeTime(double aLifeTime) { mLifeTime = aLifeTime; }
    void setOptimalSizeUnit(QString aOptimalSizeUnit) { mOptimalSizeUnit = aOptimalSizeUnit; }
    
    void setEnvImpactCost(double aEnvImpactCost) { mEnvImpactCost = aEnvImpactCost; }
    void setEnvImpactUnit(QString aEnvImpactUnit) { mImpactUnit = aEnvImpactUnit; }
    
    const QString &Unit() const { return mImpactUnit; }
    const QString &Name() const { return mName; }
    const QString& ShortName() const { return mShortName; }
    QString getOptimalSizeUnit() { return mOptimalSizeUnit; }

    void addModelConfigParameters(InputParam* aInputEnvImpacts, InputParam* aInputPortImpacts, bool* aEnvModelAddr, QString aPortName, int j)
    {
        //bool
        aInputEnvImpacts->addParameter(mName + " PiecewiseEnvGreyContentCoeff_A", &mPiecewiseEnvGreyContentCoeff, false, false, aEnvModelAddr, "If true use piecewise linear functionality for Grey environmental impacts", "", { "EnvironmentModel" });
        aInputPortImpacts->addParameter(aPortName + "." + mName + " UseEnvContentTS_A", &mUseTSEnvContentCoeff[j], false, false, aEnvModelAddr, "Comment", "", { "EnvironmentModel" });
    }

    void addGreyParameters(InputParam* aInputEnvImpacts, bool* aEnvModelAddr)
    {  
        //bool
        aInputEnvImpacts->addParameter(mName + " TryRelaxationEnvGreyContentCoeff_A", &mTryRelaxationEnvGreyContentCoeff, true, &mPiecewiseEnvGreyContentCoeff, &mPiecewiseEnvGreyContentCoeff, "If the EnvGreyContentCoefficient is a convex function of size the linearization variables will be continuous", "", { "EnvironmentModel" });
        //double
        aInputEnvImpacts->addParameter(mName + " EnvGreyContentCoefficient_A", &mEnvGreyContentCoefficient, 0., aEnvModelAddr, aEnvModelAddr, "Environmental impact linked to manufacturing - Coefficient A. Grey impact calculation: A*X+B with X the size and A = Multiplying coefficient - Default value is 1 - B = Offset - Default value is 0", mImpactUnit + "/Unit", { "EnvironmentModel" });  /** Always optional : Grey Environmental Emission Mass in kg per unit of input flux (power, flowrate) - Default value is 0 */
        aInputEnvImpacts->addParameter(mName + " EnvGreyContentOffset_B", &mEnvGreyContentOffset, 0., aEnvModelAddr, aEnvModelAddr, "Environmental impact linked to manufacturing - Offset B. Grey impact calculation: A*X+B with X the size and A = Multiplying coefficient - Default value is 1 - B = Offset - Default value is 0", mImpactUnit, { "EnvironmentModel" });  /** Always optional : Grey Environmental Emission Mass in kg per unit of input flux (power, flowrate) - Default value is 0 */
        aInputEnvImpacts->addParameter(mName + " EnvGreyReplacement", &mEnvGreyReplacement, 0., false, aEnvModelAddr, "Environmental impact linked to future manufacturing for replacement - Replacement env impact contribution ", mImpactUnit + "/"+getOptimalSizeUnit()+"/Replacement", {"EnvironmentModel"});
        aInputEnvImpacts->addParameter(mName + " EnvGreyReplacementConstant", &mEnvGreyReplacementConstant, 0., false, aEnvModelAddr, "The constant part of replacement env impact contribution ", mImpactUnit + "/Replacement", { "EnvironmentModel" });
    }

    void addParameters(InputParam* aInputPortImpacts, InputParam* aInputPortImpactsTS, bool* aEnvModelAddr, QString aPortName, int j)
    {   
        //vector (time series)
        aInputPortImpactsTS->addParameter(aPortName + "." + mName + " EnvContentTS_A", &mTSEnvContentCoeff[j], SFunctionFlag({ eFTypeNotAnd, {}, { aEnvModelAddr , &mUseTSEnvContentCoeff[j]} }), SFunctionFlag({ eFTypeNotAnd, {}, { aEnvModelAddr , &mUseTSEnvContentCoeff[j]} }), "Environmental impact linked to usage - Time profile of coefficient A. Impact calculation: A*x+B with x the flow and A = Multiplying coefficient", mImpactUnit + "/StorageUnit", { "EnvironmentModel" });
        //double
        aInputPortImpacts->addParameter(aPortName + "." + mName + " EnvContentCoefficient_A", &mEnvContentCoefficients[j], 0., SFunctionFlag({ eFTypeNotAnd, {&mUseTSEnvContentCoeff[j]}, { aEnvModelAddr } }), SFunctionFlag({ eFTypeNotAnd, {&mUseTSEnvContentCoeff[j]}, { aEnvModelAddr } }), "Environmental impact linked to usage - Coefficient A. Impact calculation: A*x+B with x the flow and A = Multiplying coefficient - Default value is 1 - B = Offset - Default value is 0", mImpactUnit + "/StorageUnit", { "EnvironmentModel" });
        aInputPortImpacts->addParameter(aPortName + "." + mName + " EnvContentOffset_B", &mEnvContentOffsets[j], 0., aEnvModelAddr, aEnvModelAddr, "Environmental impact linked to usage - Offset B. Impact calculation: A*x+B with x the flow and A = Multiplying coefficient - Default value is 1 - B = Offset - Default value is 0", mImpactUnit, { "EnvironmentModel" });
    }

    void addPerfParam(InputParam* aInputPerfParam) {
        //vector
        aInputPerfParam->addParameter(mName + " CapacitySetPoint", &mImpactCapacitySetPoint, &mPiecewiseEnvGreyContentCoeff, &mPiecewiseEnvGreyContentCoeff, "name of vector impact capacity that will be defined from DataFile specification by the User", "", { "EnvironmentModel" });
        aInputPerfParam->addParameter(mName + " SetPoint", &mImpactSetPoint, &mPiecewiseEnvGreyContentCoeff, &mPiecewiseEnvGreyContentCoeff, "name of vector impact SetPoint that will be defined from DataFile specification by the User", "", { "EnvironmentModel" });
    }

   void addIndicators(InputParam* aInputIndicators, bool* aBoolPtr, QString aCurrency)
    {
        aInputIndicators->addIndicator(mName + " Env impact penalty part", &mEnvImpactPart, aBoolPtr, "Penalty contribution due to direct environmental impact linked to usage ", aCurrency, mShortName + "DirectCost");  /**  */
        aInputIndicators->addIndicator(mName + " Env impact mass", &mEnvImpactMass, aBoolPtr, "Mass of the direct environmental impact linked to usage ", mImpactUnit, mShortName + "DirectMass");  /**  */
        aInputIndicators->addIndicator(mName + " EnvGrey impact cost", &mEnvGreyImpactCost, aBoolPtr, "Contribution due to the grey emissions linked to manufacturing ", aCurrency, mShortName + "GreyCost");					/**  */
        aInputIndicators->addIndicator(mName + " EnvGrey impact mass", &mEnvGreyImpactMass, aBoolPtr, "Mass of the grey emissions linked to manufacturing", mImpactUnit, mShortName + "GreyMass");              /**  */
        aInputIndicators->addIndicator(mName + " Env grey impact replacement part", &mEnvImpactReplacement, aBoolPtr, "Env grey impact replacement part linked to future manufacturing for replacement", aCurrency, mShortName + "ReplacementCost");              /**  */
    }

   void addIOs(class SubModel* aSubModel);
    

    void evaluateEnvGreyImpact(const double* optSol)
    {
        mEnvGreyImpactMass.at(0) = mEnvGreyImpactMass.at(1) = mExpEnvGreyImpactMass.evaluate(optSol);
        mEnvGreyImpactCost.at(0) = mEnvGreyImpactCost.at(1) = mExpEnvGreyImpactCost.evaluate(optSol);
    }

    MIPModeler::MIPExpression1D* getExpEnvCost() {return &mExpEnvImpactCost;}
    MIPModeler::MIPExpression1D* getExpEnvMass() { return &mExpEnvImpactMass; }
    MIPModeler::MIPExpression1D* getExpEnvFlow() { return &mExpEnvImpactFlow; }

    double* getEnvImpactPartPLAN() {return &mEnvImpactPart.at(0);}
    double* getEnvImpactPartHIST() { return &mEnvImpactPart.at(1); }
    double* getEnvImpactPartDiscountedPLAN() { return &mEnvImpactPartDiscounted.at(0); }
    double* getEnvImpactPartDiscountedHIST() { return &mEnvImpactPartDiscounted.at(1); }
    
    double* getEnvImpactMassPLAN() { return &mEnvImpactMass.at(0); }
    double* getEnvImpactMassHIST() { return &mEnvImpactMass.at(1); }
    double* getEnvImpactMassDiscountedPLAN() { return &mEnvImpactMassDiscounted.at(0); }
    double* getEnvImpactMassDiscountedHIST() { return &mEnvImpactMassDiscounted.at(1); }
    
    double* getEnvGreyImpactMass() { return &mEnvGreyImpactMass.at(0); }
    double* getEnvGreyImpactPart() { return &mEnvGreyImpactCost.at(0); }

    MIPModeler::MIPExpression* getExpEnvGreyMass() { return &mExpEnvGreyImpactMass; }

    MIPModeler::MIPExpression1D* getExpEnvReplacement() { return &mExpEnvGreyImpactReplacement; }
    double* getEnvImpactReplacementPLAN() { return &mEnvImpactReplacement.at(0); }
    double* getEnvImpactReplacementHIST() { return &mEnvImpactReplacement.at(1); }

    void closeExpressions()
    {
        for (int i = 0; i < (int)mExpEnvImpactCost.size(); i++)
            mExpEnvImpactCost.at(i).close();
        for (int i = 0; i < (int)mExpEnvImpactMass.size(); i++)
            mExpEnvImpactMass.at(i).close();
        for (int i = 0; i < (int)mExpEnvImpactFlow.size(); i++)
            mExpEnvImpactFlow.at(i).close();
        for (int i = 0; i < (int)mExpEnvGreyImpactReplacement.size(); i++)
            mExpEnvGreyImpactReplacement.at(i).close();
        mExpEnvGreyImpactCost.close();
        mExpEnvGreyImpactMass.close();
    }

    void allocateExpressions(int aSizeTimestep)
    {
        mExpEnvImpactCost = MIPModeler::MIPExpression1D(aSizeTimestep);
        mExpEnvImpactMass = MIPModeler::MIPExpression1D(aSizeTimestep);
        mExpEnvImpactFlow = MIPModeler::MIPExpression1D(aSizeTimestep);
        mExpEnvGreyImpactReplacement = MIPModeler::MIPExpression1D(aSizeTimestep);

    }

    void resizeCoeffs(const int aSize)
    {
        mEnvContentCoefficients.resize(aSize);
        mUseTSEnvContentCoeff.resize(aSize);
        mTSEnvContentCoeff.resize(aSize);
        for (int i = 0; i < aSize; i++) {
            mTSEnvContentCoeff[i].resize(mTimeSteps.size());
        }
        mEnvContentOffsets.resize(aSize);
    }

    bool PiecewiseEnvGreyContentCoeff() const { return mPiecewiseEnvGreyContentCoeff; }
    bool TryRelaxationEnvGreyContentCoeff() const { return mTryRelaxationEnvGreyContentCoeff; }

    MIPModeler::MIPData1D CapacitySetPoint() const { return mImpactCapacitySetPoint; }
    MIPModeler::MIPData1D SetPoint() const { return mImpactSetPoint; }

    double EnvGreyContentOffset() const { return mEnvGreyContentOffset; }

protected:
    Cairn_Exception mException;

    QString mName; //Impact name
    QString mShortName; //Impact short name
    QString mImpactUnit;
    std::vector<double> mTimeSteps;
    double mCapex;                        
    double mLifeTime;   
    QString mOptimalSizeUnit;

    bool mPiecewiseEnvGreyContentCoeff;
    bool mTryRelaxationEnvGreyContentCoeff;
    MIPModeler::MIPData1D mImpactCapacitySetPoint;
    MIPModeler::MIPData1D mImpactSetPoint;

    double mEnvGreyContentCoefficient; /** multiplying coefficient **/
    double mEnvGreyContentOffset;      /** offset coefficient **/
    double mEnvGreyReplacement;
    double mEnvGreyReplacementConstant;

    std::vector<double> mEnvContentCoefficients;                /** Environmental Emission in kg per input flow unit : vector as there can be several ports **/
    std::vector<double> mEnvContentOffsets;                /** Environmental Emission in kg per time **/
    QVector<bool> mUseTSEnvContentCoeff;
    MIPModeler::MIPData2D mTSEnvContentCoeff;              /** Environmental Emission in kg per input flow unit per timestep */

    MIPModeler::MIPExpression1D mExpEnvImpactCost;
    MIPModeler::MIPExpression1D mExpEnvImpactMass;
    MIPModeler::MIPExpression1D mExpEnvImpactFlow;
    MIPModeler::MIPExpression mExpEnvGreyImpactCost;
    MIPModeler::MIPExpression mExpEnvGreyImpactMass;
    MIPModeler::MIPExpression1D mExpEnvGreyImpactReplacement;

    std::vector<double> mEnvImpactPart;
    std::vector<double> mEnvImpactMass;
    std::vector<double> mEnvImpactPartDiscounted;
    std::vector<double> mEnvImpactMassDiscounted;

    std::vector<double> mEnvGreyImpactCost;
    std::vector<double> mEnvGreyImpactMass;
    std::vector<double> mEnvImpactReplacement;

    double mEnvImpactCost;
};
#endif // EnvImpact_H