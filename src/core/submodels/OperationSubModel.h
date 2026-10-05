/*
 * \file    OperationSubModel.h
 * \brief   OperationSubModel is a specialized SubModel implementation representing operational models
 * \version 1.0
 * \author	Ali KASSEM
 * \date    13/09/2024
 */

#ifndef OPERATIONSUBMODEL_H
#define OPERATIONSUBMODEL_H

#include "SubModel.h"

class CAIRNCORESHARED_EXPORT OperationSubModel : public SubModel
{
public:
    explicit OperationSubModel(CairnObject* aParent);
    ~OperationSubModel() override = default;

    // --- SubModel interface -------------------------------------------------

    void closeExpressions()  override;

    void declareModelConfigurationParameters() override;
    void declareModelInterface()               override;
    void declareModelIndicators()              override;

    void buildModel() override final; /** Sealed - subclasses must not override buildModel() */

    void computeAllIndicators(const double* optSol) override;

protected:
    // --- MIP expressions ----------------------------------------------------
    MIPModeler::MIPExpression1D mExpVariableCosts; /** Computed variable costs resulting from operation constraints */

    // --- Indicators ---------------------------------------------------------
    std::vector<double> mVariableCosts; /** Variable costs: [0] = PLAN (extrapolated), [1] = HIST (cumulated) */
};

#endif // OPERATIONSUBMODEL_H