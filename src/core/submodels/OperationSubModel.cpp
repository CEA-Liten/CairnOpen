/*
 * \file    OperationSubModel.cpp
 * \brief   OperationSubModel is a specialized SubModel implementation representing operational models
 * \version 1.0
 * \author	Ali KASSEM
 * \date    13/09/2024
 */

#include "OperationSubModel.h"

// --- Construction -----------------------------------------------------------

OperationSubModel::OperationSubModel(CairnObject* aParent)
    : SubModel(aParent)
    , mVariableCosts(kResultCount, 0.)
{
}

// --- SubModel interface -----------------------------------------------------

void OperationSubModel::closeExpressions()
{
    SubModel::closeExpressions();
    closeExpression1D(mExpVariableCosts);
}

void OperationSubModel::declareModelConfigurationParameters()
{
    SubModel::declareModelConfigurationParameters();

    addConfigParameter("LPModelONLY",
        &mLPModelOnly, false, 
        false, true,
        "If true use LP model - integer variables imposed or relaxed to real variables",
        "");
}

void OperationSubModel::declareModelInterface()
{
    SubModel::declareModelInterface();

    addIO("VariableCosts", 
        &mExpVariableCosts,
        true,
        pCurrency(), 
        "Variable costs from operation constraints"); 

    addIO("State", 
        &mExpState,
        &mAddStateVariable,
        "bool", 
        "ON/OFF state of the element");

    addControlIO("StartUp", 
        &mExpStartUp,
        &mAddStartUpShutDownVariable,
        "bool",
        &mHistStartUp);                     /** Startup event variable */

    addControlIO("ShutDown", 
        &mExpShutDown,
        &mAddStartUpShutDownVariable,
        "bool",
        &mHistShutDown);                    /** Shutdown event variable */
}

void OperationSubModel::declareModelIndicators()
{
    SubModel::declareModelIndicators();

    mInputIndicators->addIndicator("Opex part", 
        &mVariableCosts, 
        &mExportIndicators,
        "Total cost of operation constraints",
        pCurrency(),
        "Opex");
}

void OperationSubModel::buildModel()
{
    if (mAllocate)
        allocateExpressions();
    else
        closeExpressions();

    setExpSizeMax(); /** Set SizeMax expression and add SizeMax constraints */

    if (mAddStateVariable)
        addStateConstraints(varMilpHorizon());

    if (mAddStartUpShutDownVariable)
        addStartUpShutDown(varMilpHorizon());

    computeModelContribution(); /** Compute expressions, add variables and constraints */
    mAllocate = false;
}

void OperationSubModel::computeAllIndicators(const double* optSol)
{
    SubModel::computeAllIndicators(optSol);

    auto* compo = parentComponent();
    const double extrapolationFactor = compo ? compo->ExtrapolationFactor() : 1.0;

    mVariableCosts.at(0) = 0.;

    for (uint64_t t = 0; t < mHorizon; ++t)
        mVariableCosts.at(0) += mExpVariableCosts.at(t).evaluate(optSol) * extrapolationFactor; // PLAN

    for (uint64_t t = 0; t < *mptrTimeshift; ++t)
        mVariableCosts.at(1) += mExpVariableCosts.at(t).evaluate(optSol);                       // HIST
}