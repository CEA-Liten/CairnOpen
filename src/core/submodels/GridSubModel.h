/*
 * \file    GridSubModel.h
 * \brief   GridSubModel is a specialized TechnicalSubModel representing a grid connection (injection or extraction).
 * \version 1.0
 * \author	Ali KASSEM
 * \date    13/09/2024
 */

#ifndef GRIDSUBMODEL_H
#define GRIDSUBMODEL_H

#include "TechnicalSubModel.h"
#include "Constants.h"

using namespace CairnConstants;

extern bool CAIRNCORESHARED_EXPORT isExtraction(class SubModel* ap_Model);
extern bool CAIRNCORESHARED_EXPORT isInjection(class SubModel* ap_Model);

class CAIRNCORESHARED_EXPORT GridSubModel : public TechnicalSubModel
{
public:
    explicit GridSubModel(CairnObject* aParent);
    ~GridSubModel() override = default;

    // --- SubModel interface -------------------------------------------------

    void defineMainCarrier()  override;
    void setTimeData()        override;
    void computeInitialData() override;

    void declareModelConfigurationParameters() override;
    void declareModelParameters()              override;
    void declareModelInterface()               override;
    void declareModelIndicators()              override;

    void computeAllIndicators(const double* optSol) override;

    // --- Grid-specific interface --------------------------------------------

    double      Sens()      const override; /** Returns +1.0 (extraction) or -1.0 (injection) based on default port direction */
    std::string Direction() const;          /** Returns "extraction" or "injection" */

protected:
    MilpPort* mPortGridFlow = nullptr; /** Main default port — defines grid direction (injection or extraction) */

    // --- MILP variables -----------------------------------------------------
    MIPModeler::MIPVariable1D   mVarFluxGrid;  /** Grid flow variable */

    // --- Output MIP expressions ---------------------------------------------
    MIPModeler::MIPExpression1D mExpFlux;      /** Grid flow expression */
    MIPModeler::MIPExpression1D mExpGridPrice; /** Grid price expression */

    // --- Input parameters ---------------------------------------------------
    bool   mAddVariableMaxFlow = false; /** If true: use time-variable maximum flow limitation */
    double mMaxFlux = CairnConstants::kDoubleInit;  /** Maximum allowed extraction or injection */
    double mMinFlux = CairnConstants::kDoubleInit;  /** Minimum allowed extraction or injection */

    // --- Time series --------------------------------------------------------
    std::vector<double> mEnergyPrice;         /** Energy price — equals SellPrice or BuyPrice based on Sens */
    std::vector<double> mSellPrice;           /** Grid sell price profile (injection) */
    std::vector<double> mBuyPrice;            /** Grid buy price profile (extraction) */
    std::vector<double> mBuyPriceSeasonal;    /** Seasonal buy price profile (extraction) */
    std::vector<double> mGridVariableMaxFlow; /** Time-variable maximum grid flow */

private:
    // --- Indicator computation helpers --------------------------------------------------
    void computeRunningTime(const MIPModeler::MIPExpression1D& exp1D, const double* optSol);
    void computeProductionIndicators(const MilpPort* port, const MIPModeler::MIPExpression1D& exp1D, const double* optSol);
    void computeConsumptionIndicators(const MilpPort* port, const MIPModeler::MIPExpression1D& exp1D, const double* optSol);
    void computeDataExchangeIndicators(const MilpPort* port, const MIPModeler::MIPExpression1D& exp1D, const double* optSol);
};

#endif // GRIDSUBMODEL_H