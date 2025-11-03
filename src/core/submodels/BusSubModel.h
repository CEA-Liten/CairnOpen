#ifndef BusSubModel_H
#define BusSubModel_H

#include "SubModel.h"

class CAIRNCORESHARED_EXPORT BusSubModel : public SubModel
{
public:
    BusSubModel(CairnObject* aParent=nullptr);
    ~BusSubModel();

    void buildModel();
    void computeDefaultIndicators(const double* optSol);

    void declareDefaultModelConfigurationParameters() { 
        SubModel::declareDefaultModelConfigurationParameters();
    }
    void declareDefaultModelParameters() { }

    void declareDefaultModelInterface()
    {
        SubModel::declareDefaultModelInterface();
    }

    void declareDefaultModelIndicators() { }

    void initDefaultPorts() { }; //Bus doesn't have default ports!


//protected:

};

#endif // BusSubModel_H