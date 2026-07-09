/* --------------------------------------------------------------------------
 * File: AddNode.cpp
 * version 1.0
 * Author: Alain Ruby
 * Date 23/07/2019
 *---------------------------------------------------------------------------
 * Description: Model imposing Node addition node constraint on Bus Component
 * --------------------------------------------------------------------------
 */

# include "GlobalSettings.h"
#include "NodeLaw.h"

extern "C" MODELS_DECLSPEC CairnObject * createModel(CairnObject * aParent)
{
    return new NodeLaw(aParent);
}

NodeLaw::NodeLaw(CairnObject* aParent)
    : BusSubModel(aParent),
    mBusEnergyBalance(2,0.) 
{
}

NodeLaw::~NodeLaw()
{

}

void NodeLaw::setTimeData()
{
    SubModel::setTimeData();
    mHistBusBalance.clear() ;
    mHistBusBalance.resize(mHorizon+mNpdtPast);
}

void NodeLaw::computeModelContribution()
{
    /** Build balance constraint once component constraints have created their own expressions
        Constraint linked to mLinkedPorts (ports of the componenets connected to this Bus) */
    for (auto& port : mLinkedPorts) {
        double aSign = (port->Direction() == GS::KCONS())? -1.: 1.;
        double portVarTimeDepend = (port->FluxDim()==1) ? 1 : 0;
        mPortVarSet.push_back(port->GAMSVarName());
        mPortVarCoeff.push_back(port->VarCoeff());
        mPortVarOffSet.push_back(port->VarOffset());
        mPortVarDirection.push_back(aSign);
        mPortVarTimeDepend.push_back(portVarTimeDepend);

        if(port->FluxDim()==1.){
            addExpressionToBalance(port->Flux()) ;
        }
        else{
            addExpressionToBalance(port->Flux0D()) ;
        }
	}
    
    addStrictConstraint();
}

void NodeLaw::computeInitialData() 
{
    /* When UseExtrapolationFactor is true, then *BusValue is assumed to be over one year */

    const double factor = mParentCompo->ExtrapolationFactor();

    const double scale = mUseExtrapolationFactor ? (1.0 / factor) : 1.0;

    mStrictConstraintBusValue *= scale;
}

void NodeLaw::addExpressionToBalance(MIPModeler::MIPExpression1D& aFluxExpression)
{
	for (unsigned int t = 0; t < mHorizon ; ++t)
	{
	   mBusBalance[t] += aFluxExpression[t] ;
	}
}

void NodeLaw::addExpressionToBalance(MIPModeler::MIPExpression &aFluxExpression)
{
    for (unsigned int t = 0; t < mHorizon ; ++t)
    {
       mBusBalance[t] += aFluxExpression ;
    }
}

void NodeLaw::addStrictConstraint()
{
	for (unsigned int t = 0; t < mHorizon ; ++t)
       addConstraint(mBusBalance[t] == mStrictConstraintBusValue, "S", t) ;

    ModelerInterface* pExternalModeler = mModel->getExternalModeler();
    if (pExternalModeler != nullptr) {
        std::string compoName = SubModel::parent()->objectName();
        std::string compoType = "StrictConstraint";

        pExternalModeler->addText("");
        pExternalModeler->addComment("+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++");
        pExternalModeler->addComment(" add new NodeLaw Bus component");
        pExternalModeler->addComment("+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++");

        ModelerParams vParams;
        vParams.addParam(compoName + "_set_PortName", mPortVarSet);
        vParams.addParam(compoName + "_p_ConstraintValue", mStrictConstraintBusValue);
        vParams.addParam(compoName + "_p_VarCoeff", compoName + "_set_PortName", mPortVarCoeff);
        vParams.addParam(compoName + "_p_VarOffSet", compoName + "_set_PortName", mPortVarOffSet);
        vParams.addParam(compoName + "_p_FlowDirection", compoName + "_set_PortName", mPortVarDirection);
        pExternalModeler->setModelData(vParams);

        pExternalModeler->addText("$\t setLocal CompoName " + compoName);
        pExternalModeler->addText("$\t setLocal compoType " + compoType);
        pExternalModeler->addComment("");

        std::string args;
        for (size_t ii = 0; ii < mPortVarSet.size(); ii++) {
            pExternalModeler->addText("$\t setLocal PortVar" + std::to_string(ii) + "  " + mPortVarSet[ii]);
            args += "%PortVar" + std::to_string(ii) + "% ";
        }

        pExternalModeler->addText("");
        ModelerParams vOptions;
        vOptions.addParam("compoType", "%compoType%");
        vOptions.addParam("args", args);
        pExternalModeler->addModelFromFile("%gamslib%/Bus/NodeLaw/NodeLaw.gms", "%CompoName%", vOptions);
    }
}

void NodeLaw::computeAllIndicators(const double* optSol)
{
    BusSubModel::computeDefaultIndicators(optSol);
    computeProduction(true, mHorizon, mNpdtPast, mBusBalance, optSol, 1., 0., mBusEnergyBalance.at(0), true);
    computeProduction(false, *mptrTimeshift, mNpdtPast, mBusBalance, optSol, 1., 0., mBusEnergyBalance.at(1), true);
}