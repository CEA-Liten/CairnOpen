#ifndef OperationSubModel_H
#define OperationSubModel_H

#include "SubModel.h"

class CAIRNCORESHARED_EXPORT OperationSubModel : public SubModel
{
public:
    OperationSubModel(CairnObject* aParent=nullptr);
    ~OperationSubModel();

    void buildModel();
    virtual void closeExpressions();
    void computeDefaultIndicators(const double* optSol);

    // --------------- Default Parameters (common to all operation models) ------------------ //
    void declareDefaultModelConfigurationParameters() { 
        SubModel::declareDefaultModelConfigurationParameters();  
        //bool
        addParameter("LPModelONLY", &mLPModelOnly, false, false, true, "Use LP Model - ie integer variables imposed or relaxed to real variables if true", "");  
    }

    void declareDefaultModelParameters() 
    { 
    }

    void declareDefaultModelInterface()
    {
        SubModel::declareDefaultModelInterface();
        addIO("VariableCosts", &mExpVariableCosts, true, pCurrency());    /** Computed variable costs resulting from ramp cost */
        addIO("State", &mExpState, &mAddStateVariable, "bool");  /** ON OFF state of the element connected to ramp */
        addControlIO("StartUp", &mExpStartUp, &mAddStartUpShutDownVariable, "bool", &mHistStartUp);
        addControlIO("ShutDown", &mExpShutDown, &mAddStartUpShutDownVariable, "bool", &mHistShutDown);
    }

    void declareDefaultModelIndicators() {
        mInputIndicators->addIndicator("Opex part", &mVariableCosts, &mExportIndicators, "Total cost of operation constraint", pCurrency(), "Opex");
    }

protected:
    //expressions
    MIPModeler::MIPExpression1D mExpVariableCosts;
    MIPModeler::MIPExpression1D mExpVariableOpex;

    //indicators
    std::vector<double> mVariableCosts;

    //methods
    virtual bool defineDefaultVarNames(MilpPort* port);
};

#endif // OperationSubModel_H