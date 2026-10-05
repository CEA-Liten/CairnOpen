
#include "GlobalSettings.h"
#include "ManualConstraint.h"

extern "C" MODELS_DECLSPEC CairnObject * createModel(CairnObject * aParent)
{
    return new ManualConstraint(aParent);
}

ManualConstraint::ManualConstraint(CairnObject* aParent)
    : BusSubModel(aParent),
    mBusEnergyBalance(2,0.) 
{
}

ManualConstraint::~ManualConstraint()
{

}

void ManualConstraint::setTimeData()
{
    SubModel::setTimeData();
    mHistBusBalance.clear() ;
    mHistBusBalance.resize(mHorizon+mNpdtPast);
}

void ManualConstraint::computeInitialData()
{
    /* When UseExtrapolationFactor is true, then *BusValue is assumed to be over one year */
    
    // Retrieve extrapolation factor 
    auto* compo = parentComponent();
    const double factor = compo ? compo->ExtrapolationFactor() : 1.0;

    // Compute scaling (1.0 means "no scaling")
    const double scale = mUseExtrapolationFactor ? (1.0 / factor) : 1.0;

    mMinConstraintBusValue *= scale;
    mMaxConstraintBusValue *= scale;
    mStrictConstraintBusValue *= scale;
    mMinIntegrateConstraintBusValue *= scale;
    mMaxIntegrateConstraintBusValue *= scale;
    mStrictIntegrateConstraintBusValue *= scale;
    mMaxFlexIntegrateConstraintBusValue *= scale;
}

void ManualConstraint::computeBusBalance()
{
    mExpressions0D.clear();
    mExpressions1D.clear();

    for (const auto& port : mLinkedPorts) 
    {
        const double sign = (port->Direction() == GS::KCONS()) 
            ? -1. : 1.;

        mPortVarDirection.push_back(sign);
        mPortVarSet.push_back(port->GAMSVarName());
        mPortVarCoeff.push_back(port->VarCoeff());
        mPortVarOffSet.push_back(port->VarOffset());

        if (port->IsTimeDependant()) 
            mExpressions1D.push_back(&(port->Flux()));
        else 
            mExpressions0D.push_back(&(port->Flux0D()));
    }

    for (unsigned int t = 0; t < mHorizon; ++t)
    {
        for (const auto& expr0D : mExpressions0D)
            mBusBalance[t] += *expr0D;

        for (const auto& expr1D : mExpressions1D)
            mBusBalance[t] += (*expr1D)[t];
    }
}


void ManualConstraint::computeModelContribution()
{
    /** 
        Build balance constraint once component constraints have created their own expressions
        Constraint linked to mLinkedPorts (ports of the componenets connected to this Bus) 
    */

    computeBusBalance();

    // Add constraints

	if (mStrictConstraint) 
        addStrictConstraint();

	if (mMinConstraint) 
        addMinConstraint();

	if (mMaxConstraint) 
        addMaxConstraint();

	if (mStrictIntegrateConstraint) 
        addStrictIntegrateConstraint(); 

    if (mMinIntegrateConstraint) {
        if(mPeriodIntegrateConstraint == 0)
            addMinIntegrateConstraint() ;
        else
            addMinIntegrateConstraint(mPeriodIntegrateConstraint);
    }

    if (mMaxIntegrateConstraint) {
        if (mPeriodIntegrateConstraint == 0)
            addMaxIntegrateConstraint();
        else
            addMaxIntegrateConstraint(mPeriodIntegrateConstraint);
    }

    if (mMinIntegrateSeparateConstraint || mMaxIntegrateSeparateConstraint) 
        addIntegrateSeparateConstraint(mPeriodIntegrateConstraint, mIntervalBetweenIntegrals);

    if (mMaxFlexIntegrateConstraint) 
        addMaxFlexIntegrateConstraint();

    if (mMaxFlexIntegrateConstraint)
        mExpPenaltyConstraintCosts += mExpConstraintGap;
}


//----------------Parts of buildProblem--------------------------------------------------------------

bool ManualConstraint::isBalanceTimeIndependent() const
{
    return mExpressions1D.empty();
}

void ManualConstraint::addStrictConstraint()
{
    if (isBalanceTimeIndependent()) {
        addConstraint(mBusBalance[0] == mStrictConstraintBusValue, "S", 0);
    }
    else {
        for (unsigned int t = 0; t < mHorizon; ++t)
            addConstraint(mBusBalance[t] == mStrictConstraintBusValue, "S", t);
    }

    // Case of external modeler
    ModelerInterface* pExternalModeler = mModel->getExternalModeler();
    if (!pExternalModeler)
        return;

    std::string compoName = SubModel::parent()->objectName();
    std::string compoType = "StrictConstraint";

    pExternalModeler->addText("");
    pExternalModeler->addComment("+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++");
    pExternalModeler->addComment(" add new ManualConstraint Bus component");
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
    pExternalModeler->addModelFromFile("%gamslib%/Bus/ManualConstraint/ManualConstraint.gms", "%CompoName%", vOptions);
}

void ManualConstraint::addMaxConstraint()
{
    if (isBalanceTimeIndependent()) {
        addConstraint(mBusBalance[0] <= mMaxConstraintBusValue, "M", 0);
    }
    else {
        for (unsigned int t = 0; t < mHorizon; ++t)
            addConstraint(mBusBalance[t] <= mMaxConstraintBusValue, "M", t);
    }

    // Case of external modeler
    ModelerInterface* pExternalModeler = mModel->getExternalModeler();
    if (!pExternalModeler)
        return;

    std::string compoName = SubModel::parent()->objectName();
    std::string compoType = "MaxConstraint";
    pExternalModeler->addText("");
    pExternalModeler->addComment("+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++");
    pExternalModeler->addComment(" add new ManualConstraint Bus component");
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
    pExternalModeler->addModelFromFile("%gamslib%/Bus/ManualConstraint/ManualConstraint.gms", "%CompoName%", vOptions);
}

void ManualConstraint::addMinConstraint()
{
    if (isBalanceTimeIndependent()) {
        addConstraint(mBusBalance[0] >= mMinConstraintBusValue, "m", 0);
    }
    else {
        for (unsigned int t = 0; t < mHorizon; ++t)
            addConstraint(mBusBalance[t] >= mMinConstraintBusValue, "m", t);
    }

    // Case of external modeler
    ModelerInterface* pExternalModeler = mModel->getExternalModeler();
    if (!pExternalModeler)
        return;

    std::string compoName = SubModel::parent()->objectName();
    std::string compoType = "MinConstraint";
        
    pExternalModeler->addText("");
    pExternalModeler->addComment("+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++");
    pExternalModeler->addComment(" add new ManualConstraint Bus component");
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
    pExternalModeler->addModelFromFile("%gamslib%/Bus/ManualConstraint/ManualConstraint.gms", "%CompoName%", vOptions);
}

void ManualConstraint::addStrictIntegrateConstraint()
{
	MIPModeler::MIPExpression ExprIntegrate;

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

    // Case of external modeler
    ModelerInterface* pExternalModeler = mModel->getExternalModeler();
    if (!pExternalModeler)
        return;

    std::string compoName = SubModel::parent()->objectName();
    std::string compoType = "StrictIntegrateConstraint";

    pExternalModeler->addText("");
    pExternalModeler->addComment("+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++");
    pExternalModeler->addComment(" add new ManualConstraint Bus component");
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
    pExternalModeler->addModelFromFile("%gamslib%/Bus/ManualConstraint/ManualConstraint.gms", "%CompoName%", vOptions);
}

void ManualConstraint::addMaxIntegrateConstraint()
{
    MIPModeler::MIPExpression ExprIntegrate;
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

    // Case of external modeler
    ModelerInterface* pExternalModeler = mModel->getExternalModeler();
    if (!pExternalModeler)
        return;

    std::string compoName = SubModel::parent()->objectName();
    std::string compoType = "MaxIntegrateConstraint";

    pExternalModeler->addText("");
    pExternalModeler->addComment("+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++");
    pExternalModeler->addComment(" add new ManualConstraint Bus component");
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
    pExternalModeler->addModelFromFile("%gamslib%/Bus/ManualConstraint/ManualConstraint.gms", "%CompoName%", vOptions);
}

void ManualConstraint::addMinIntegrateConstraint()
{
    MIPModeler::MIPExpression ExprIntegrate;
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

    // Case of external modeler
    ModelerInterface* pExternalModeler = mModel->getExternalModeler();
    if (!pExternalModeler)
        return;

    std::string compoName = SubModel::parent()->objectName();
    std::string compoType = "MinIntegrateConstraint";

    pExternalModeler->addText("");
    pExternalModeler->addComment("+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++");
    pExternalModeler->addComment(" add new ManualConstraint Bus component");
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
    pExternalModeler->addModelFromFile("%gamslib%/Bus/ManualConstraint/ManualConstraint.gms", "%CompoName%", vOptions);
}

void ManualConstraint::addMaxIntegrateConstraint(int period)
{
    const int fullHorizon = (int)mHorizon + (int)mNpdtPast;
    if (period > fullHorizon) {
        cError() << "ERROR : the interval of integration is greater than (futursize + pastsize)= " << fullHorizon 
            << ". The constraint can't be computed!";
        return;
    }
    
    for (int t = 0; t < mHorizon ; ++t)
    {
        for (int k = 0; k < period; ++k)
        {
            const int ts = t - k;
            if(ts < 0)
            {
                const int an = mNpdtPast + ts;
                if (an >= 0)
                {
                    if (!isnan(mHistBusBalance[mNpdtPast + ts]))
                    {
                        if (mTimeIntegration)
                            mExprIntegrate[t] += mHistBusBalance[mNpdtPast + ts] * TimeStep(0);
                        else
                            mExprIntegrate[t] += mHistBusBalance[mNpdtPast + ts] ;
                    }
                }
            }
            else if (ts >= 0)
            {
                if (mTimeIntegration)
                    mExprIntegrate[t] += mBusBalance[ts] * TimeStep(ts);
                else
                    mExprIntegrate[t] += mBusBalance[ts] ;
            }
        }

        // Add only one constraint if 
        addConstraint(mExprIntegrate[t] <= mMaxIntegrateConstraintBusValue, "MIperiod", t) ;
    }

    // Case of external modeler
    ModelerInterface* pExternalModeler = mModel->getExternalModeler();
    if (!pExternalModeler)
        return;

    std::string compoName = SubModel::parent()->objectName();
    std::string compoType = "RollingMaxIntegrateConstraint";

    pExternalModeler->addText("");
    pExternalModeler->addComment("+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++");
    pExternalModeler->addComment(" add new ManualConstraint Bus component");
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
    pExternalModeler->addModelFromFile("%gamslib%/Bus/ManualConstraint/ManualConstraint.gms", "%CompoName%", vOptions);
}

void ManualConstraint::addMaxFlexIntegrateConstraint()
{
    addVariable(mVarConstraintGap,"Gap", 0.f, 1.e12);

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
    addConstraint(0. <= mExpConstraintGap, "MFI");
}

void ManualConstraint::addMinIntegrateConstraint(int period)
{
    if(period > mHorizon){
        cWarning() << "The period has to be smaller than the timeshift. MaxIntegrateConstraint to be checked!" ;
        addMinIntegrateConstraint();
        return;
    }

    for (int t = period; t < mHorizon ; ++t)
    {
        for (int k = 0; k < period; ++k)
        {
            const int ts = t - k;
            if(ts < 0)
            {
                const int an = mNpdtPast + ts;
                if (an >= 0)
                {
                    if (!isnan(mHistBusBalance[mNpdtPast + ts]))
                    {
                        if (mTimeIntegration)
                        {
                            mExprIntegrate[t] += mHistBusBalance[mNpdtPast + ts] * TimeStep(0);
                        }
                        else
                        {
                            mExprIntegrate[t] += mHistBusBalance[mNpdtPast + ts] ;
                        }
                    }
                }
            }
            else if (ts  >= 0)
            {
                if (mTimeIntegration)
                {
                    mExprIntegrate[t] += mBusBalance[ts] * TimeStep(ts);
                }
                else
                {
                    mExprIntegrate[t] += mBusBalance[ts];
                }
            }
        }
        addConstraint(mExprIntegrate[t] >= mMinIntegrateConstraintBusValue, "mIperiod", t) ;
    }

    // Case of external modeler
    ModelerInterface* pExternalModeler = mModel->getExternalModeler();
    if (!pExternalModeler)
        return;

    std::string compoName = SubModel::parent()->objectName();
    std::string compoType = "RollingMinIntegrateConstraint";


    pExternalModeler->addText("");
    pExternalModeler->addComment("+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++");
    pExternalModeler->addComment(" add new ManualConstraint Bus component");
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
    pExternalModeler->addModelFromFile("%gamslib%/Bus/ManualConstraint/ManualConstraint.gms", "%CompoName%", vOptions);
}

void ManualConstraint::addIntegrateSeparateConstraint(int period, int intervalBetween)
{
    if (intervalBetween==0)
    {
        intervalBetween = period;
    }
    if ((mHorizon-period) % intervalBetween != 0)
    {
        cCritical() << "ERROR: to use separate constraint, the period"<<intervalBetween<<" should be a divisor of (futuresize - interval between integrate constraints) "<<(mHorizon-intervalBetween)<<"!";
    }
    if ( (*mptrTimeshift) % intervalBetween != 0)
    {
        cCritical() << "ERROR: to use separate constraint, interval between integrates constraints "<<intervalBetween<<" should be a divisor of timeshift "<<*mptrTimeshift<<"!";
    }
    if ( period < intervalBetween)
    {
        cCritical() << "ERROR: to use separate constraint, period of integration "<<(period)<<"should be > than interval between 2 periods"<<intervalBetween <<"!";
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
void ManualConstraint::computeAllIndicators(const double* optSol)
{
    BusSubModel::computeAllIndicators(optSol);
    computeProduction(true, mHorizon, mBusBalance, optSol, 1., 0., mBusEnergyBalance.at(0),mTimeIntegration);
    computeProduction(false, *mptrTimeshift, mBusBalance, optSol, 1., 0., mBusEnergyBalance.at(1), mTimeIntegration);
}