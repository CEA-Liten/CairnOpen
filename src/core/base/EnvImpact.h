#ifndef ENVIMPACT_H
#define ENVIMPACT_H

#include "CairnCore_global.h"
#include "InputParam.h"
#include "MilpPort.h"
#include <cmath>
#include <deque>
#include <string>
#include <vector>

class TechnicalSubModel;

/**
 * \brief EnvImpact encapsulates all parameter registration, expression building,
 *        and indicator declaration for one environmental impact attached to a TechnicalSubModel.
 *
 * Design note: the InputParam containers are owned by the parent SubModel 
 * but are exclusively written by EnvImpact. They are injected at construction 
 * so EnvImpact never needs to call back into SubModel for them. 
 */

class CAIRNCORESHARED_EXPORT EnvImpact 
{    
public:

    EnvImpact(TechnicalSubModel* parent, const std::string& name, 
        const std::string& unit = "-", const std::string& shortName = "");

    ~EnvImpact() = default;

    // Non-copyable 
    EnvImpact(const EnvImpact&) = delete;
    EnvImpact& operator=(const EnvImpact&) = delete;

    // --- Parameter & IO registration ----------------------------------------

    void addConfigParameters(const std::string& portName, int j);
    void addGreyParameters();
    void addPortParameters(const std::string& portName, int j, EnergyVector* carrier);
    void addPerfParameters();
    void addIOExpressions();
    void addIndicators();

    /** Resize per-port coefficient vectors when a new port is added */
    void initPortCoefficients(size_t portCount);

    // --- Expression lifecycle -----------------------------------------------

    void allocateExpressions(int horizon);
    void closeExpressions();

    // --- Contribution computation -------------------------------------------

    void computeEmbodiedEnvImpactContribution(
        const MIPModeler::MIPExpression& expSizeMax,
        const MIPModeler::MIPExpression& expInstalled);

    void computeReplacementEnvImpactContribution(
        const MIPModeler::MIPExpression& expSizeMax,
        const MIPModeler::MIPExpression& expInstalled);

    void computeEnvImpactContribution(const size_t j, 
        const MIPModeler::MIPExpression1D* flux,
        const MIPModeler::MIPExpression& expInstalled);

    void computeEnvImpactContributionCost();
    void evaluateEmbodiedImpact(const double* optSol);

    // --- Accessors ----------------------------------------------------------

    void setEnvImpactCost(double aEnvImpactCost) { mEnvImpactCost = aEnvImpactCost; }
    
    const std::string& Unit() const { return mImpactUnit; }
    const std::string& Name() const { return mName; }
    const std::string& ShortName() const { return mShortName; }

    const std::vector<double>& getTimeSteps() const;
    double getLifeTime() const;

    bool PiecewiseEnvGreyContentCoeff()     const { return mPiecewiseEmbodiedCoeff; }
    bool TryRelaxationEnvGreyContentCoeff() const { return mTryRelaxationEmbodiedCoeff; }

    MIPModeler::MIPData1D CapacitySetPoint() const { return mImpactCapacitySetPoint; }
    MIPModeler::MIPData1D SetPoint() const { return mImpactSetPoint; }

    double EnvGreyContentOffset() const { return mEmbodiedOffset; }

    bool isNewlySelected() const { return mNewlySelected; }
    void markAsOld() { mNewlySelected = false; }

    // --- Expression pointers (for TechnicalSubModel to integrate into model) -

    MIPModeler::MIPExpression1D* getExpEnvOpCost() {return &mExpOpEnvImpactCost;}
    MIPModeler::MIPExpression1D* getExpEnvOp()     { return &mExpOpEnvImpact; }
    MIPModeler::MIPExpression1D* getExpEnvFlow()   { return &mExpFlowEnvImpact; }
    MIPModeler::MIPExpression*   getExpEnvEmbodied()     { return &mExpEmbodiedEnvImpact; }
    MIPModeler::MIPExpression*   getExpEnvEmbodiedCost() { return &mExpEmbodiedEnvImpactCost; }
    MIPModeler::MIPExpression1D* getExpEnvReplacement()  { return &mExpReplacementEnvImpact; }

    // --- Indicator value pointers (written during computeAllIndicators) ------

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

protected:
    // --- Identity -----------------------------------------------------------
    std::string mName;       /** Full impact name */
    std::string mShortName;  /** Short name used in indicator keys */
    std::string mImpactUnit;

    bool mNewlySelected; /** True until markAsOld() is called after first initialization */

    // --- Parent & injected InputParam containers (non-owning) ---------------
    TechnicalSubModel* mParentModel;     /** Owning submodel - never null after construction */
    InputParam* mInputConfigEnvImpacts;  /** SubModel::mInputConfigEnvImpacts */
    InputParam* mInputEnvImpacts;        /** SubModel::mInputEnvImpacts */
    InputParam* mInputConfigPortImpacts; /** SubModel::mInputConfigPortImpacts */
    InputParam* mInputPortImpacts;       /** SubModel::mInputPortImpacts */
    InputParam* mInputPortImpactsTS;     /** SubModel::mTSInputPortImpacts */
    InputParam* mInputPerfParam;         /** SubModel::mInputPerfParam */
    InputParam* mInputIndicators;        /** SubModel::mInputIndicators */

    // --- Grey impact (embodied / piecewise) parameters ----------------------
    bool mPiecewiseEmbodiedCoeff;       /** If true use piecewise linearization for grey impacts */
    bool mTryRelaxationEmbodiedCoeff;   /** If true relax piecewise linearization variables */

    MIPModeler::MIPData1D mImpactCapacitySetPoint; /** Capacity set points for piecewise grey impact */
    MIPModeler::MIPData1D mImpactSetPoint;         /** Impact set points for piecewise grey impact */

    double mEmbodiedCoefficient;       /** Grey impact: A in A*X + B (per unit of size) */
    double mEmbodiedOffset;            /** Grey impact: B in A*X + B (constant part) */
    double mEmbodiedReplacement;       /** Replacement impact coefficient per unit of size */
    double mEmbodiedReplacementOffset; /** Replacement impact constant offset */

    // --- Per-port operational impact coefficients ---------------------------
    std::vector<double>  mEnvContentCoefficients;  /** A coefficient per port (impact per flow unit) */
    std::vector<double>  mEnvContentOffsets;       /** B offset per port (impact per time) */
    std::deque<bool>     mUseTSEnvContentCoeff;    /** Use time-series coefficient for port j
                                                        std::deque used because std::vector<bool>
                                                        does not provide real bool& references */
    MIPModeler::MIPData2D mTSEnvContentCoeff;      /** Time-series coefficient profiles per port */

    // --- MIP Expressions ---------------------------------------------------
    MIPModeler::MIPExpression1D mExpOpEnvImpactCost;       /** Operational impact cost per timestep */
    MIPModeler::MIPExpression1D mExpOpEnvImpact;           /** Operational impact mass per timestep */
    MIPModeler::MIPExpression1D mExpFlowEnvImpact;         /** Operational impact flow rate per timestep */
    MIPModeler::MIPExpression   mExpEmbodiedEnvImpactCost; /** Embodied impact cost (0D) */
    MIPModeler::MIPExpression   mExpEmbodiedEnvImpact;     /** Embodied impact mass (0D) */
    MIPModeler::MIPExpression1D mExpReplacementEnvImpact;  /** Replacement impact per timestep */

    // --- Indicator result vectors [0]=PLAN [1]=HIST -------------------------
    std::vector<double> mEnvImpactPart;            /** Operational impact cost contributions */
    std::vector<double> mEnvImpactMass;            /** Operational impact mass contributions */
    std::vector<double> mEnvImpactPartDiscounted;  /** Discounted operational impact cost */
    std::vector<double> mEnvImpactMassDiscounted;  /** Discounted operational impact mass */
    std::vector<double> mEmbodiedEnvImpactCost;    /** Embodied impact cost contributions */
    std::vector<double> mEmbodiedEnvImpact;        /** Embodied impact mass contributions */
    std::vector<double> mReplacementEnvImpact;     /** Replacement impact contributions */

    double mEnvImpactCost; /** Cost per unit of impact */
};
#endif // ENVIMPACT_H