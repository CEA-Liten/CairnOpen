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
extern "C" MODELS_DECLSPEC QObject * createModel(QObject * aParent)
{
    return new NodeLaw(aParent);
}


NodeLaw::NodeLaw(QObject* aParent)
    : BusSubModel(aParent),
    mBusEnergyBalance(2,0.) 
{
}

NodeLaw::~NodeLaw()
{

}

void NodeLaw::setParameters(double aMinConstraintBusValue, double aMaxConstraintBusValue, double aStrictConstraintBusValue, double aMinIntegrateConstraintBusValue, 
    double aMaxIntegrateConstraintBusValue, double aStrictIntegrateConstraintBusValue, double aMaxFlexIntegrateConstraintBusValue)
{
	mMinConstraintBusValue = aMinConstraintBusValue;
	mMaxConstraintBusValue = aMaxConstraintBusValue;
	mStrictConstraintBusValue = aStrictConstraintBusValue;
	mMinIntegrateConstraintBusValue = aMinIntegrateConstraintBusValue;
	mMaxIntegrateConstraintBusValue = aMaxIntegrateConstraintBusValue;
	mStrictIntegrateConstraintBusValue = aStrictIntegrateConstraintBusValue;
    mMaxFlexIntegrateConstraintBusValue = aMaxFlexIntegrateConstraintBusValue;
}

void NodeLaw::setTimeData()
{
    SubModel::setTimeData();
    mHistBusBalance.clear() ;
    mHistBusBalance.resize(mHorizon+mNpdtPast);
}

void NodeLaw::buildModel()
{
        // le bus est une contrainte systeme sous forme d'une expression a laquelle chaque composant contribue directement
        if (mAllocate)
        {
            mBusBalance = MIPModeler::MIPExpression1D(mHorizon);
        }
        else
        {
            closeExpression1D(mBusBalance) ;
        }

        /** Build balance constraint once component constraints have created their own expressions
            Constraint linked to mListPort connected ports */
	    MilpPort* port ;
	    QListIterator<MilpPort*> iport (mListPort);

        

	    while (iport.hasNext())
	    {
	       port = iport.next() ;

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

	    if (mStrictConstraint) addStrictConstraint() ;
	    if (mMinConstraint) addMinConstraint() ;
	    if (mMaxConstraint) addMaxConstraint() ;
	    if (mStrictIntegrateConstraint) addStrictIntegrateConstraint(); 

        if (mMinIntegrateConstraint) {
            if(mPeriodIntegrateConstraint==0){
                addMinIntegrateConstraint() ;
            }
            else{
                addMinIntegrateConstraint(mPeriodIntegrateConstraint);
            }
        }

        if (mMaxIntegrateConstraint) {
            if (mPeriodIntegrateConstraint == 0){
                addMaxIntegrateConstraint() ;
            }
            else{
                addMaxIntegrateConstraint(mPeriodIntegrateConstraint);
            }
        }

        if (mMinIntegrateSeparateConstraint || mMaxIntegrateSeparateConstraint) {
            addIntegrateSeparateConstraint(mPeriodIntegrateConstraint, mIntervalBetweenIntegrals);
        }

        if (mMaxFlexIntegrateConstraint) addMaxFlexIntegrateConstraint() ;

	    /** Objective contribution expressed as the sum of Capex and Opex so as to be able to account for actualization rate on Opex part only*/
        computeAllContribution() ;

        mAllocate = false;
}

//------------------------------------------------------------------------------
void NodeLaw::finalizeModelData() {
    double extrapolationFactor  = mParentCompo->ExtrapolationFactor();
    //Divide by UseExtrapolationFactor; assuming that the *BusValue are over one year
    if (mUseExtrapolationFactor) {
        setParameters(mMinConstraintBusValue / extrapolationFactor, mMaxConstraintBusValue / extrapolationFactor, mStrictConstraintBusValue / extrapolationFactor,
            mMinIntegrateConstraintBusValue / extrapolationFactor, mMaxIntegrateConstraintBusValue / extrapolationFactor,
            mStrictIntegrateConstraintBusValue / extrapolationFactor, mMaxFlexIntegrateConstraintBusValue / extrapolationFactor);
    }
}

//------------------------------------------------------------------------------
void NodeLaw::computeAllContribution()
{
    BusSubModel::computeAllContribution() ;

    if (mMaxFlexIntegrateConstraint)
    {
        if (mAllocate)
            mExpPenaltyConstraintCosts = MIPModeler::MIPExpression();
        else
            closeExpression(mExpPenaltyConstraintCosts);

        mExpPenaltyConstraintCosts += mExpConstraintGap ;
    }
}
//----------------Parts of buildProblem--------------------------------------------------------------

void NodeLaw::initBalance()
{
    closeExpression1D(mBusBalance);
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
       addConstraint(mBusBalance[t] == mStrictConstraintBusValue,"S",t) ;

    ModelerInterface* pExternalModeler = mModel->getExternalModeler();
    if (pExternalModeler != nullptr) {
        std::string compoName = SubModel::parent()->objectName().toStdString();
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

void NodeLaw::addMaxConstraint()
{
	for (unsigned int t = 0; t < mHorizon ; ++t)
       addConstraint(mBusBalance[t] <= mMaxConstraintBusValue,"M",t) ;

    ModelerInterface* pExternalModeler = mModel->getExternalModeler();
    if (pExternalModeler != nullptr) {
        std::string compoName = SubModel::parent()->objectName().toStdString();
        std::string compoType = "MaxConstraint";
        pExternalModeler->addText("");
        pExternalModeler->addComment("+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++");
        pExternalModeler->addComment(" add new NodeLaw Bus component");
        pExternalModeler->addComment("+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++");

        ModelerParams vParams;
        vParams.addParam(compoName + "_set_PortName", mPortVarSet);
        vParams.addParam(compoName + "_p_ConstraintValue", mMaxConstraintBusValue);
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

void NodeLaw::addMinConstraint()
{
	for (unsigned int t = 0; t < mHorizon ; ++t)
       addConstraint(mBusBalance[t] >= mMinConstraintBusValue,"m",t) ;

    ModelerInterface* pExternalModeler = mModel->getExternalModeler();
    if (pExternalModeler != nullptr) {
        std::string compoName = SubModel::parent()->objectName().toStdString();
        std::string compoType = "MinConstraint";
        
        pExternalModeler->addText("");
        pExternalModeler->addComment("+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++");
        pExternalModeler->addComment(" add new NodeLaw Bus component");
        pExternalModeler->addComment("+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++");

        ModelerParams vParams;
        vParams.addParam(compoName + "_set_PortName", mPortVarSet);
        vParams.addParam(compoName + "_p_ConstraintValue", mMinConstraintBusValue);
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

void NodeLaw::addStrictIntegrateConstraint()
{
	MIPModeler::MIPExpression ExprIntegrate ;

    if (mTimeIntegration)
    {
        for (unsigned int t = 0; t < mHorizon; ++t)
            ExprIntegrate += mBusBalance[t] * TimeStep(t);
    }
    else
    {
        for (unsigned int t = 0; t < mHorizon; ++t)
            ExprIntegrate += mBusBalance[t] ;
    }
    addConstraint(ExprIntegrate == mStrictIntegrateConstraintBusValue,"SI") ;

    ModelerInterface* pExternalModeler = mModel->getExternalModeler();
    if (pExternalModeler != nullptr) {
        std::string compoName = SubModel::parent()->objectName().toStdString();
        std::string compoType = "StrictIntegrateConstraint";

        pExternalModeler->addText("");
        pExternalModeler->addComment("+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++");
        pExternalModeler->addComment(" add new NodeLaw Bus component");
        pExternalModeler->addComment("+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++");

        ModelerParams vParams;
        vParams.addParam(compoName + "_set_PortName", mPortVarSet);
        vParams.addParam(compoName + "_p_ConstraintValue", mStrictIntegrateConstraintBusValue);
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

void NodeLaw::addMaxIntegrateConstraint()
{
    MIPModeler::MIPExpression ExprIntegrate ;
    if (mAllocate)
        mExprIntegrate = MIPModeler::MIPExpression1D(mHorizon);
    else
        closeExpression1D(mExprIntegrate);

    if (mTimeIntegration)
    {
        for (unsigned int t = 0; t < mHorizon; ++t)
        {
            ExprIntegrate += mBusBalance[t] * TimeStep(t);
            mExprIntegrate[t] += mBusBalance[t] * TimeStep(t);
        }
    }
    else
    {
        for (unsigned int t = 0; t < mHorizon; ++t)
        {
            ExprIntegrate += mBusBalance[t];
            mExprIntegrate[t] += mBusBalance[t];
        }
    }
    addConstraint(ExprIntegrate <= mMaxIntegrateConstraintBusValue,"MI",0) ;

    ModelerInterface* pExternalModeler = mModel->getExternalModeler();
    if (pExternalModeler != nullptr) {
        std::string compoName = SubModel::parent()->objectName().toStdString();
        std::string compoType = "MaxIntegrateConstraint";

        pExternalModeler->addText("");
        pExternalModeler->addComment("+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++");
        pExternalModeler->addComment(" add new NodeLaw Bus component");
        pExternalModeler->addComment("+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++");

        ModelerParams vParams;
        vParams.addParam(compoName + "_set_PortName", mPortVarSet);
        vParams.addParam(compoName + "_p_ConstraintValue", mMaxIntegrateConstraintBusValue);
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

void NodeLaw::addMinIntegrateConstraint()
{
    MIPModeler::MIPExpression ExprIntegrate ;
    if (mAllocate)
        mExprIntegrate = MIPModeler::MIPExpression1D(mHorizon);
    else
        closeExpression1D(mExprIntegrate);

    if (mTimeIntegration)
    {
        for (unsigned int t = 0; t < mHorizon; ++t)
            ExprIntegrate += mBusBalance[t] * TimeStep(t);
    }
    else
    {
        for (unsigned int t = 0; t < mHorizon; ++t)
            ExprIntegrate += mBusBalance[t] ;
    }
    addConstraint(ExprIntegrate >= mMinIntegrateConstraintBusValue,"mI",0) ;

    ModelerInterface* pExternalModeler = mModel->getExternalModeler();
    if (pExternalModeler != nullptr) {
        std::string compoName = SubModel::parent()->objectName().toStdString();
        std::string compoType = "MinIntegrateConstraint";

        pExternalModeler->addText("");
        pExternalModeler->addComment("+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++");
        pExternalModeler->addComment(" add new NodeLaw Bus component");
        pExternalModeler->addComment("+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++");

        ModelerParams vParams;
        vParams.addParam(compoName + "_set_PortName", mPortVarSet);
        vParams.addParam(compoName + "_p_ConstraintValue", mMinIntegrateConstraintBusValue);
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

void NodeLaw::addMaxIntegrateConstraint(int period)
{

    if(period>(int)mHorizon + (int)mNpdtPast)
    {
        qCritical() << "ERROR : the interval of integration is greater than (futursize + pastsize)= "<< mHorizon + mNpdtPast<<". The constraint can't be computed ! " ;
    }
    else
    {
        if (mAllocate)
            mExprIntegrate = MIPModeler::MIPExpression1D(mHorizon);
        else
            closeExpression1D(mExprIntegrate);

        for (int t = 0; t < (int)mHorizon ; ++t)
        {
            for (int k = 0; k < period; ++k)
            {
                if((t-k)< 0)
                {
                    int an = mNpdtPast+(t-k);
                    if (an>=0)
                    {
                        if (!isnan(mHistBusBalance[mNpdtPast+(t-k)]))
                        {
                            if (mTimeIntegration)
                            {
                                mExprIntegrate[t] += mHistBusBalance[mNpdtPast + (t - k)] * TimeStep(0);
                            }
                            else
                            {
                                mExprIntegrate[t] += mHistBusBalance[mNpdtPast + (t - k)] ;
                            }
                        }
                    }
                }
                else if ((t-k)>=0)
                {
                    if (mTimeIntegration)
                    {
                        mExprIntegrate[t] += mBusBalance[t - k] * TimeStep(t - k);
                    }
                    else
                    {
                        mExprIntegrate[t] += mBusBalance[t - k] ;
                    }
                }
            }
            addConstraint(mExprIntegrate[t] <= mMaxIntegrateConstraintBusValue,"MIperiod",t) ;
        }

        ModelerInterface* pExternalModeler = mModel->getExternalModeler();
        if (pExternalModeler != nullptr) {
            std::string compoName = SubModel::parent()->objectName().toStdString();
            std::string compoType = "RollingMaxIntegrateConstraint";

            pExternalModeler->addText("");
            pExternalModeler->addComment("+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++");
            pExternalModeler->addComment(" add new NodeLaw Bus component");
            pExternalModeler->addComment("+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++");

            ModelerParams vParams;
            std::vector<std::string> kpastSet(mHistBusBalance.size());
            for (uint64_t i = 0; i < mHistBusBalance.size(); i++)
                kpastSet[i] = std::to_string(i);

            vParams.addParam(compoName + "_set_kpast", kpastSet);
            vParams.addParam(compoName + "_set_PortName", mPortVarSet);
            vParams.addParam(compoName + "_p_ConstraintValue", mMaxIntegrateConstraintBusValue);
            vParams.addParam(compoName + "_p_RollingPeriod", (double)period);
            vParams.addParam(compoName + "_p_VarCoeff", compoName + "_set_PortName", mPortVarCoeff);
            vParams.addParam(compoName + "_p_VarOffSet", compoName + "_set_PortName", mPortVarOffSet);
            vParams.addParam(compoName + "_p_FlowDirection", compoName + "_set_PortName", mPortVarDirection);
            vParams.addParam(compoName + "_p_HistoricalValue", compoName + "_set_kpast", mHistBusBalance);
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
}

void NodeLaw::addMaxFlexIntegrateConstraint()
{
    if (mAllocate)
    {
        mVarConstraintGap = MIPModeler::MIPVariable0D( 0.f, 1.e12);
        mExpConstraintGap = MIPModeler::MIPExpression () ;
    }
    else
    {
        closeExpression(mExpConstraintGap);
    }

    addVariable(mVarConstraintGap,"Gap");

    mExpConstraintGap += mVarConstraintGap ;

    MIPModeler::MIPExpression ExprIntegrate ;
    if (mTimeIntegration)
    {
        for (unsigned int t = 0; t < mHorizon; ++t)
            ExprIntegrate += mBusBalance[t] * TimeStep(t);
    }
    else
    {
        for (unsigned int t = 0; t < mHorizon; ++t)
            ExprIntegrate += mBusBalance[t] ;
    }
    addConstraint(mPenaltyCost * ( ExprIntegrate - mMaxFlexIntegrateConstraintBusValue ) <= mExpConstraintGap, "MFI") ;
    addConstraint(0. <= mExpConstraintGap,"MFI") ;
}

void NodeLaw::addMinIntegrateConstraint(int period)
{

    if(period>(int)mHorizon){
        qWarning() << "ERROR : the period has to be smaller than the timeshift. MaxIntegrateConstraint to be checked ! " ;
        addMinIntegrateConstraint();
    }
    else{
        if (mAllocate)
            mExprIntegrate = MIPModeler::MIPExpression1D(mHorizon);
        else
            closeExpression1D(mExprIntegrate);

        for (int t = period; t < (int)mHorizon ; ++t)
        {
            for (int k = 0; k < period; ++k)
            {
                if((t-k)< 0)
                {
                    int an = mNpdtPast+(t-k);
                    if (an>=0)
                    {
                        //int a = mHistBusBalance[mNpdtPast+(t-k)];
                        if (!isnan(mHistBusBalance[mNpdtPast + (t - k)]))
                        {
                            if (mTimeIntegration)
                            {
                                mExprIntegrate[t] += mHistBusBalance[mNpdtPast + (t - k)] * TimeStep(0);
                            }
                            else
                            {
                                mExprIntegrate[t] += mHistBusBalance[mNpdtPast + (t - k)] ;
                            }
                        }
                    }
                }
                else if ((t-k)>=0)
                {
                    if (mTimeIntegration)
                    {
                        mExprIntegrate[t] += mBusBalance[t - k] * TimeStep(t - k);
                    }
                    else
                    {
                        mExprIntegrate[t] += mBusBalance[t - k];
                    }
                }
            }
            addConstraint(mExprIntegrate[t] >= mMinIntegrateConstraintBusValue,"mIperiod",t) ;
        }

        ModelerInterface* pExternalModeler = mModel->getExternalModeler();
        if (pExternalModeler != nullptr) {
            std::string compoName = SubModel::parent()->objectName().toStdString();
            std::string compoType = "RollingMinIntegrateConstraint";


            pExternalModeler->addText("");
            pExternalModeler->addComment("+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++");
            pExternalModeler->addComment(" add new NodeLaw Bus component");
            pExternalModeler->addComment("+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++");

            ModelerParams vParams;
            std::vector<std::string> kpastSet(mHistBusBalance.size());
            for (uint64_t i = 0; i < mHistBusBalance.size(); i++)
                kpastSet[i] = std::to_string(i);

            vParams.addParam(compoName + "_set_kpast", kpastSet);
            vParams.addParam(compoName + "_set_PortName", mPortVarSet);
            vParams.addParam(compoName + "_p_ConstraintValue", mMinIntegrateConstraintBusValue);
            vParams.addParam(compoName + "_p_RollingPeriod", (double)period);
            vParams.addParam(compoName + "_p_VarCoeff", compoName + "_set_PortName", mPortVarCoeff);
            vParams.addParam(compoName + "_p_VarOffSet", compoName + "_set_PortName", mPortVarOffSet);
            vParams.addParam(compoName + "_p_FlowDirection", compoName + "_set_PortName", mPortVarDirection);
            vParams.addParam(compoName + "_p_HistoricalValue", compoName + "_set_kpast", mHistBusBalance);
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
}

void NodeLaw::addIntegrateSeparateConstraint(int period, int intervalBetween){
    if (mAllocate)
        mExprIntegrate = MIPModeler::MIPExpression1D(mHorizon);
    else
        closeExpression1D(mExprIntegrate);

    if (intervalBetween==0)
    {
        intervalBetween = period;
    }
    if ((mHorizon-period) % intervalBetween != 0)
    {
        qCritical() << "ERROR: to use separate constraint, the period"<<intervalBetween<<" should be a divisor of (futuresize - interval between integrate constraints) "<<(mHorizon-intervalBetween)<<"!";
    }
    if ( (*mptrTimeshift) % intervalBetween != 0)
    {
        qCritical() << "ERROR: to use separate constraint, interval between integrates constraints "<<intervalBetween<<" should be a divisor of timeshift "<<*mptrTimeshift<<"!";
    }
    if ( period < intervalBetween)
    {
        qCritical() << "ERROR: to use separate constraint, period of integration "<<(period)<<"should be > than interval between 2 periods"<<intervalBetween <<"!";
    }
    else
    {
        int intervalSize = (mHorizon - period) / intervalBetween;
        if (intervalSize < mExprIntegrate.size() - 1) intervalSize += 1;
        if (mTimeIntegration)
        {
            for (int u = 0; u < intervalSize; ++u)
            {
                for (int i = 0; i < period; ++i)
                {
                    mExprIntegrate[u * intervalBetween] += mBusBalance[u * intervalBetween + i] * TimeStep(u * intervalBetween + i);
                }

                if (mMinIntegrateSeparateConstraint) {
                    addConstraint(mExprIntegrate[u * intervalBetween] >= mMinIntegrateConstraintBusValue, "cIntegrateSeparate", u);
                }
                else if (mMaxIntegrateSeparateConstraint) {
                    addConstraint(mExprIntegrate[u * intervalBetween] <= mMaxIntegrateConstraintBusValue, "cIntegrateSeparate", u);
                }
            }
        }
        else
        {
            for (int u = 0; u < intervalSize; ++u)
            {
                for (int i = 0; i < period; ++i)
                {
                    mExprIntegrate[u * intervalBetween] += mBusBalance[u * intervalBetween + i] ;
                }

                if (mMinIntegrateSeparateConstraint) {
                    addConstraint(mExprIntegrate[u * intervalBetween] >= mMinIntegrateConstraintBusValue, "IntegrateSeparate", u);
                }
                else if (mMaxIntegrateSeparateConstraint) {
                    addConstraint(mExprIntegrate[u * intervalBetween] <= mMaxIntegrateConstraintBusValue, "IntegrateSeparate", u);
                }
            }
        }
    }
}
//-------------------------------------------------------
void NodeLaw::computeAllIndicators(const double* optSol)
{
    BusSubModel::computeDefaultIndicators(optSol);
    computeProduction(true, mHorizon, mNpdtPast, mBusBalance, optSol, 1., 0., mBusEnergyBalance.at(0),mTimeIntegration);
    computeProduction(false, *mptrTimeshift, mNpdtPast, mBusBalance, optSol, 1., 0., mBusEnergyBalance.at(1), mTimeIntegration);
}