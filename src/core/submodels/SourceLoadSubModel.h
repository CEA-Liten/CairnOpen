/*
 * \file    SourceLoadSubModel.h
 * \brief   SourceLoadSubModel is a specialized TechnicalSubModel representing a SourceLoad.
 * \version 1.0
 * \author	Ali KASSEM
 * \date    13/09/2024
 */

#ifndef SOURCELOADSUBMODEL_H
#define SOURCELOADSUBMODEL_H

#include "TechnicalSubModel.h"

class CAIRNCORESHARED_EXPORT SourceLoadSubModel : public TechnicalSubModel
{
public:
    explicit SourceLoadSubModel(CairnObject* aParent);
    ~SourceLoadSubModel() override = default;

    // --- SubModel interface -------------------------------------------------

    void declareModelInterface()  override;
    void declareModelIndicators() override;

    void computeAllIndicators(const double* optSol) override;

    // --- SourceENR-specific Indicators -------------------------------------- 

    void declareSourceENRModelIndicators();
    void computeSourceENRModelIndicators(const double* optSol);

    // --- SourceLoad-specific ------------------------------------------------

    double      Sens()      const override; /** Returns +1.0 (source) or -1.0 (load) based on default port direction */
    std::string Direction() const;          /** Returns "source" or "load" */

protected:
    MilpPort* mSourceLoadDefaultPort = nullptr; /** Main default port - defines direction (source or load) */

private:
    // --- Indicator computation helpers -------------------------------------------
    void computeMaxRunningTime(const double* optSol);
    void computeRunningTime(const double* optSol);
    void computeProductionIndicators(const MilpPort* port, const MIPModeler::MIPExpression1D& exp1D, const double* optSol);
    void computeENRProductionIndicators(const MilpPort* port, const MIPModeler::MIPExpression1D& exp1D, const double* optSol);
    void computeConsumptionIndicators(const MilpPort* port, const MIPModeler::MIPExpression1D& exp1D, const double* optSol);
    void computeDataExchangeIndicators(const MilpPort* port, const MIPModeler::MIPExpression1D& exp1D, const double* optSol);
};

#endif // SOURCELOADSUBMODEL_H