#ifndef BusSubModel_H
#define BusSubModel_H

#include "SubModel.h"

class CAIRNCORESHARED_EXPORT BusSubModel : public SubModel
{
public:
    BusSubModel(QObject* aParent=nullptr);
    ~BusSubModel();

    void computeDefaultIndicators(const double* optSol);
    virtual void computeAllContribution(); /** MILP Model description : objective contribution */

    //-------------------------------------------------------------------------------------------------------------------------
    // ---------------------------------------------------- Defaul Parameters --------------------------------------------------
    void declareDefaultModelConfigurationParameters() { 
        SubModel::declareDefaultModelConfigurationParameters();
    }
    void declareDefaultModelParameters() { }
    void declareDefaultModelInterface()
    {
        SubModel::declareDefaultModelInterface();
    }
    void declareDefaultModelIndicators() { }

    virtual void initDefaultPorts() { }; //Bus doesn't have default ports!

    //-------------------------------------------------------------------------------------

//protected:

};

#endif // BusSubModel_H