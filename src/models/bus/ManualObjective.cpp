/* --------------------------------------------------------------------------
 * File: AddNode.cpp
 * version 1.0
 * Author: Alain Ruby
 * Date 23/07/2019
 *---------------------------------------------------------------------------
 * Description: Model to add manually objective to the problem.
 * --------------------------------------------------------------------------
 */

#include "ManualObjective.h"
extern "C" MODELS_DECLSPEC CairnObject * createModel(CairnObject * aParent)
{
    return new ManualObjective(aParent);
}

ManualObjective::ManualObjective(CairnObject* aParent)
    : BusSubModel(aParent),
    mBusEnergyBalance(2, 0.) 
{
}

ManualObjective::~ManualObjective()
{

}

void ManualObjective::setParameters(double aMinConstraintBusValue, double aMaxConstraintBusValue, double aStrictConstraintBusValue){
	mMinConstraintBusValue = aMinConstraintBusValue;
	mMaxConstraintBusValue = aMaxConstraintBusValue;
	mStrictConstraintBusValue = aStrictConstraintBusValue;
}

void ManualObjective::setTimeData()
{
    SubModel::setTimeData();
    mObjectiveCoeffTS.resize(mHorizon);
}

void ManualObjective::closeExpressions()
{
    SubModel::closeExpressions();

    closeExpression(mBusBalance);
    closeExpression(mSubObjective);
    closeExpression(mExpCommonMinVariable);
    closeExpression(mExpCommonMaxVariable);

    closeExpression1D(mBusBalance0D);
    closeExpression1D(mBusBalance1D);
}

void ManualObjective::buildModel()
{
    // le bus est une contrainte systeme sous forme d'une expression a laquelle chaque composant contribue directement
    if (mAllocate)
    {
        mBusBalance1D = MIPModeler::MIPExpression1D(mHorizon);
        mBusBalance0D = MIPModeler::MIPExpression1D(mHorizon);
    }
    else
    {
        closeExpressions();
    }

	/** Build balance constraint once component constraints have created their own expressions */
    addVariable(mCommonMinVariable, "CommonMin");
    addVariable(mCommonMaxVariable, "CommonMax");

	initBalance() ;

    mExpCommonMaxVariable = mCommonMaxVariable;
    mExpCommonMinVariable = mCommonMinVariable;

    if (mUseCommonMaxVariable) {
        for (auto& port : mListPort) {        
            if (port->FluxDim() == 1.) {
                for (unsigned int t = 0; t < mHorizon; ++t)
                {
                    addConstraint(mExpCommonMaxVariable >= port->Flux()[t], "MaxConstraint");
                }
            }
            else {
                addConstraint(mExpCommonMaxVariable >= port->Flux0D(), "MaxConstraint");
            }
        }
        mSubObjective += mObjectiveCoefficient * mExpCommonMaxVariable;
    }

    else if (mUseCommonMinVariable) {
        for (auto& port : mListPort) {        
            if (port->FluxDim() == 1.) {
                for (unsigned int t = 0; t < mHorizon; ++t)
                {
                    addConstraint(mExpCommonMinVariable <= port->Flux()[t], "MinConstraint");
                }
            }
            else {
                addConstraint(mExpCommonMinVariable <= port->Flux0D(), "MinConstraint");
            }
        }
        mSubObjective += mObjectiveCoefficient * mExpCommonMinVariable;
    }
    else {
        /** Constraint linked to mListPort connected ports */
        for (auto& port : mListPort) {        
            if (port->FluxDim() == 1.) {
                addExpressionToBalance(port->Flux());
            }
            else {
                addExpressionToBalance(port->Flux0D());
            }
        }

        for (unsigned int t = 0; t < mHorizon; ++t)
        {
            mBusBalance0D[t] += mBusBalance;
        }
        computeSubObjectiveContribution();
    }
    /** Objective contribution expressed as the sum of Capex and Opex so as to be able to account for actualization rate on Opex part only*/
    

	if (mStrictConstraint) addStrictConstraint() ;
	if (mMinConstraint) addMinConstraint() ;
	if (mMaxConstraint) addMaxConstraint() ;

    /** Compute all expressions */
    computeAllContribution() ;

    mAllocate = false ;
}
//------------------------------------------------------------------------------
void ManualObjective::finalizeModelData() {
    double extrapolationFactor = mParentCompo->ExtrapolationFactor();
    //Divide by UseExtrapolationFactor; assuming that the *BusValue are over one year
    if (mUseExtrapolationFactor) {
        setParameters(mMinConstraintBusValue / extrapolationFactor, mMaxConstraintBusValue / extrapolationFactor, mStrictConstraintBusValue / extrapolationFactor);
    }
}
//------------------------------------------------------------------------------
void ManualObjective::computeEconomicalContribution()
{
    cInfo()<<"model type"<<mObjectiveType;

    // Note: The "Add" case is handled directly at the Optim Problem level
    if (mObjectiveType == "Lexicographic"){
        cInfo() << "adding the objective " << parent()->objectName();
        std::string name = parent()->objectName();
        MIPModeler::MIPSubobjective subObjective(name);
        subObjective.setSubObjective(mSubObjective,1,mObjectiveLevel,mAbsTol,mRelTol);
        mModel->addSubobjective(subObjective);
    }
}

void ManualObjective::computeSubObjectiveContribution()
{
    for (unsigned int t = 0; t < mHorizon ; ++t){
        if (mTimeIntegration) {
            mSubObjective += TimeStep(t) * mBusBalance1D[t] * mObjectiveCoefficient;
        } else {
            mSubObjective += mBusBalance1D[t] * mObjectiveCoefficient;
        }
    }
    mSubObjective += mObjectiveCoefficient*mBusBalance;
}
//----------------Parts of buildProblem--------------------------------------------------------------

void ManualObjective::initBalance()
{
}

void ManualObjective::addExpressionToBalance(MIPModeler::MIPExpression1D &aFluxExpression)
{
	for (unsigned int t = 0; t < mHorizon ; ++t)
	{
       mBusBalance1D[t] += aFluxExpression[t] * mObjectiveCoeffTS[t];
	}
}
void ManualObjective::addExpressionToBalance(MIPModeler::MIPExpression &aFluxExpression)
{
    mBusBalance += aFluxExpression ;
}

void ManualObjective::addStrictConstraint()
{
    addConstraint(mSubObjective== mStrictConstraintBusValue,"SMO") ;
}

void ManualObjective::addMaxConstraint()
{
    addConstraint(mSubObjective <= mMaxConstraintBusValue,"M") ;
}

void ManualObjective::addMinConstraint()
{
    addConstraint(mSubObjective >= mMinConstraintBusValue,"m") ;
}
//-------------------------------------------------------
void ManualObjective::computeAllIndicators(const double* optSol)
{
    BusSubModel::computeDefaultIndicators(optSol);
    mBusEnergyBalance.at(0) = 0.;
    mBusEnergyBalance.at(1) = 0.;
}










