#ifndef EnvImpact_H
#define EnvImpact_H
class EnvImpact;
class TechnicalSubModel;

#include "CairnCore_global.h"
#include "InputParam.h"
#include "MilpPort.h"
#include <cmath>
#include <deque>

/**
 * \brief The EnvImpact class is the base class for each environmental impact
 */
class CAIRNCORESHARED_EXPORT EnvImpact 
{    
public:

    EnvImpact(TechnicalSubModel* aParent, std::string aName, std::string aUnit = "-", std::string aShortName = "");
    ~EnvImpact();

    void addConfigParameters(std::string aPortName, int j);
    void addGreyParameters();
    void addPortParameters(std::string aPortName, int j, EnergyVector* aCarrier);
    void addPerfParameters();
    void addIOExpressions();
    void addIndicators();

    void computeEmbodiedEnvImpactContribution(
        const MIPModeler::MIPExpression& aExpSizeMax,
        const MIPModeler::MIPExpression& aExpInstalled);

    void computeReplacementEnvImpactContribution(
        const MIPModeler::MIPExpression& aExpSizeMax,
        const MIPModeler::MIPExpression& aExpInstalled);

    void computeEnvImpactContribution(const size_t j, const MIPModeler::MIPExpression1D* aFlux,
        const MIPModeler::MIPExpression& aExpInstalled);

    void computeEnvImpactContributionCost();
    void evaluateEnvGreyImpact(const double* optSol);

    void setEnvImpactCost(double aEnvImpactCost) { mEnvImpactCost = aEnvImpactCost; }
    
    const std::string &Unit() const { return mImpactUnit; }
    const std::string &Name() const { return mName; }
    const std::string& ShortName() const { return mShortName; }

    InputParam* InputEnvImpacts();
    InputParam* InputPortImpacts();
    InputParam* InputPortImpactsTS();
    InputParam* InputIndicators();
    InputParam* InputPerfParam();

    const std::vector<double>& getTimeSteps() const;
    const double getLifeTime() const;

    MIPModeler::MIPExpression1D* getExpEnvOpCost() {return &mExpOpEnvImpactCost;}
    MIPModeler::MIPExpression1D* getExpEnvOp() { return &mExpOpEnvImpact; }
    MIPModeler::MIPExpression1D* getExpEnvFlow() { return &mExpFlowEnvImpact; }
    MIPModeler::MIPExpression* getExpEnvEmbodied() { return &mExpEmbodiedEnvImpact; }
    MIPModeler::MIPExpression* getExpEnvEmbodiedCost() { return &mExpEmbodiedEnvImpactCost; }
    MIPModeler::MIPExpression1D* getExpEnvReplacement() { return &mExpReplacementEnvImpact; }

    double* getEnvImpactPartPLAN() {return &mEnvImpactPart.at(0);}
    double* getEnvImpactPartHIST() { return &mEnvImpactPart.at(1); }
    double* getEnvImpactPartDiscountedPLAN() { return &mEnvImpactPartDiscounted.at(0); }
    double* getEnvImpactPartDiscountedHIST() { return &mEnvImpactPartDiscounted.at(1); }
    
    double* getEnvImpactMassPLAN() { return &mEnvImpactMass.at(0); }
    double* getEnvImpactMassHIST() { return &mEnvImpactMass.at(1); }
    double* getEnvImpactMassDiscountedPLAN() { return &mEnvImpactMassDiscounted.at(0); }
    double* getEnvImpactMassDiscountedHIST() { return &mEnvImpactMassDiscounted.at(1); }
    
    double* getEnvGreyImpactMass() { return &mEmbodiedEnvImpact.at(0); }
    double* getEnvGreyImpactPart() { return &mEmbodiedEnvImpactCost.at(0); }

    double* getEnvImpactReplacementPLAN() { return &mReplacementEnvImpact.at(0); }
    double* getEnvImpactReplacementHIST() { return &mReplacementEnvImpact.at(1); }

    void closeExpressions()
    {
        for (int i = 0; i < (int)mExpOpEnvImpact.size(); i++) {
            mExpOpEnvImpact.at(i).close();
            mExpOpEnvImpactCost.at(i).close();
        }   
        for (int i = 0; i < (int)mExpFlowEnvImpact.size(); i++)
            mExpFlowEnvImpact.at(i).close();
        for (int i = 0; i < (int)mExpReplacementEnvImpact.size(); i++) {
            mExpReplacementEnvImpact.at(i).close();
        }
        mExpEmbodiedEnvImpactCost.close();
        mExpEmbodiedEnvImpact.close();
    }

    void allocateExpressions(int aSizeTimestep)
    {
        mExpOpEnvImpactCost = MIPModeler::MIPExpression1D(aSizeTimestep);
        mExpOpEnvImpact = MIPModeler::MIPExpression1D(aSizeTimestep);
        mExpFlowEnvImpact = MIPModeler::MIPExpression1D(aSizeTimestep);
        mExpReplacementEnvImpact = MIPModeler::MIPExpression1D(aSizeTimestep);

    }

    void resizeCoeffs(const size_t vecSize)  
    {
        mEnvContentCoefficients.resize(vecSize);
        mEnvContentOffsets.resize(vecSize);
        mUseTSEnvContentCoeff.resize(vecSize);
        mTSEnvContentCoeff.resize(vecSize);

        const size_t horizon = getTimeSteps().size();
        for (auto& coeffVec : mTSEnvContentCoeff) {   
            coeffVec.resize(horizon);
        }
    }

    bool PiecewiseEnvGreyContentCoeff() const { return mPiecewiseEnvGreyContentCoeff; }
    bool TryRelaxationEnvGreyContentCoeff() const { return mTryRelaxationEnvGreyContentCoeff; }

    MIPModeler::MIPData1D CapacitySetPoint() const { return mImpactCapacitySetPoint; }
    MIPModeler::MIPData1D SetPoint() const { return mImpactSetPoint; }

    double EnvGreyContentOffset() const { return mEnvGreyContentOffset; }

    bool isNewlySelected() const { return mNewlySelected; }
    void markAsOld() { mNewlySelected = false; }

protected:
    bool mNewlySelected; // A flag to mark if the impact is selected after the first initialization of mParentModel

    std::string mName; //Impact name
    std::string mShortName; //Impact short name
    std::string mImpactUnit;

    TechnicalSubModel* mParentModel;

    bool mPiecewiseEnvGreyContentCoeff;
    bool mTryRelaxationEnvGreyContentCoeff;
    MIPModeler::MIPData1D mImpactCapacitySetPoint;
    MIPModeler::MIPData1D mImpactSetPoint;

    double mEnvGreyContentCoefficient; /** multiplying coefficient **/
    double mEnvGreyContentOffset;      /** offset coefficient **/
    double mEnvGreyReplacement;
    double mEnvGreyReplacementConstant;

    std::vector<double> mEnvContentCoefficients;           /** Environmental Emission in kg per input flow unit : vector as there can be several ports **/
    std::vector<double> mEnvContentOffsets;                /** Environmental Emission in kg per time **/
    std::deque<bool> mUseTSEnvContentCoeff;     /** use std::deque because std::vector doens't provide a referance &mUseTSEnvContentCoeff[j] */
    MIPModeler::MIPData2D mTSEnvContentCoeff;              /** Environmental Emission in kg per input flow unit per timestep */

    MIPModeler::MIPExpression1D mExpOpEnvImpactCost;
    MIPModeler::MIPExpression1D mExpOpEnvImpact;
    MIPModeler::MIPExpression1D mExpFlowEnvImpact;
    MIPModeler::MIPExpression mExpEmbodiedEnvImpactCost;
    MIPModeler::MIPExpression mExpEmbodiedEnvImpact;
    MIPModeler::MIPExpression1D mExpReplacementEnvImpact;

    std::vector<double> mEnvImpactPart;
    std::vector<double> mEnvImpactMass;
    std::vector<double> mEnvImpactPartDiscounted;
    std::vector<double> mEnvImpactMassDiscounted;

    std::vector<double> mEmbodiedEnvImpactCost;
    std::vector<double> mEmbodiedEnvImpact;
    std::vector<double> mReplacementEnvImpact;

    double mEnvImpactCost;
};
#endif // EnvImpact_H