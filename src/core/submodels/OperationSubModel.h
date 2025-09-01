#ifndef OperationSubModel_H
#define OperationSubModel_H

#include "SubModel.h"

class CAIRNCORESHARED_EXPORT OperationSubModel : public SubModel
{
public:
    OperationSubModel(QObject* aParent=nullptr);
    ~OperationSubModel();

    void buildModel();
    virtual void closeExpressions();
    void computeDefaultIndicators(const double* optSol);

    // --------------- Default Parameters (common to all operation models) ------------------ //
    void declareDefaultModelConfigurationParameters() { 
        SubModel::declareDefaultModelConfigurationParameters();  
        //bool
        addParameter("LPModelONLY", &mLPModelOnly, false, false, true, "Use LP Model - ie integer variables imposed or relaxed to real variables if true", "");          /** Use LP Model - ie binary variable imposed if true */
    }

    void declareDefaultModelParameters() { 
    }

    void declareDefaultModelInterface()
    {
        SubModel::declareDefaultModelInterface();
        addIO("VariableCosts", &mExpVariableCosts, true, mCurrency);    /** Computed variable costs resulting from ramp cost */
    }

    void declareDefaultModelIndicators() {
        QString currency = mParentCompo->Currency(); //for codeAnalyzer.py
        mInputIndicators->addIndicator("Opex part", &mVariableCosts, &mExportIndicators, "Total cost of operation constraint", currency, "Opex");
    }

protected:
    //indicators
    std::vector<double> mVariableCosts;

    MIPModeler::MIPExpression1D mExpVariableCosts;                       

    virtual bool defineDefaultVarNames(MilpPort* port);
};

#endif // OperationSubModel_H