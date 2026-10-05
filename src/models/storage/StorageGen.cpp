#include "StorageGen.h"
extern "C" MODELS_DECLSPEC CairnObject * createModel(CairnObject * aParent)
{
    return new StorageGen(aParent);
}

StorageGen::StorageGen(CairnObject* aParent) : StorageSubModel(aParent),
    mInitialSoe(0.),
    mInitialSoe_Def(0.),
    mInternalLosses(2,0.)
{
    mPossibleModelClasses = { "StorageGen", "StorageLinearBounds", "StorageThermal", "Battery_V1", "StorageSeasonal", "BatteryDetailledBeta" };
}

StorageGen::~StorageGen(){ }

void StorageGen::setTimeData() {
    StorageSubModel::setTimeData();
    mCapacityMultiplier.resize(mHorizon);
    mAllowCharge.resize(mHorizon);
    mAllowDischarge.resize(mHorizon);
    mFinalStorageValue.resize(mHorizon);
}

int StorageGen::checkConsistency()
{
    int ier = TechnicalSubModel::checkConsistency();

    if ((mInitSOC < 0. && mInitSOC != -1.) || mInitSOC > 1.) {
        cCritical() << "ERROR : Storage " << Name() << " expects an initial state of charge in the range [0,1] or -1 to use coupling within PEGASE. ";
        return -1;
    }

    // Charge/discharge flow must be strictly > 0 when states are enabled, 
    // and >= 0 when states are disabled.
    auto checkFlow = [&](double value, const std::string& label)
    {
        const bool ok = mAddChargeDischargeStates ? (value > kEpsilon)
            : (value >= 0.0);

        if (!ok) {
            const std::string msg = mAddChargeDischargeStates
                ? (label + " must be strictly greater than 0")
                : (label + " must be greater or equal to 0");

            cWarning() << Name() << " " << msg;
            return false;
        }
        return true;
    };

    if (!checkFlow(mMinFlowCharge, "MinFlowCharge"))    
        return -1;
    if (!checkFlow(mMinFlowDischarge, "MinFlowDischarge")) 
        return -1;

    return ier;
}

void StorageGen::computeInitialData()
{
    setMaxValue(mMaxEsto);
    setMinValue(mMinSize);

    /* Intial state used in Estock IO */
    mInitialSoe_Def = mInitSOC * getMaxBound();
    cDebug() << "StorageGen mInitialSoe_Def : " << mInitialSoe_Def;
}

void StorageGen::addChargeDischargeStates()
{
    if (!mAddChargeDischargeStates)
        return;

    addVariable(mCharging, "IsCharging", 0, 1, MIPModeler::MIP_INT);
    addVariable(mDischarging, "IsDischarging", 0, 1, MIPModeler::MIP_INT);

    fillExpression(mExpCharging, mCharging);
    fillExpression(mExpDischarging, mDischarging);

    for (uint64_t t = 0; t < mHorizon; ++t) {
        // Charging : IsCharging = 0 -> QC = 0 AND IsCharging = 1 -> MinChargeFlow <= QC <= MaxChargeFlow
        addConstraint(mExpFlowCharge[t] - mMaxFlowCharge * mExpCharging[t] <= 0, "Charging", t);
        addConstraint(mExpFlowCharge[t] - mMinFlowCharge * mExpCharging[t] >= 0, "NotCharging", t);

        // Discharging : IsDischarging = 0 -> QD = 0 AND IsDischarging = 1 -> MinDischargeFlow <= QD <= MaxDischargeFlow
        addConstraint(mExpFlowDischarge[t] - mMaxFlowDischarge * mExpDischarging[t] <= 0, "Discharging", t);
        addConstraint(mExpFlowDischarge[t] - mMinFlowDischarge * mExpDischarging[t] >= 0, "NotDischarging", t);

        // Charging OR Discharging
        if (mFlowDirection) {
            addConstraint(mExpCharging[t] + mExpDischarging[t] <= 1, "ChargingOrDischarging", t);
        }
    }
}

void StorageGen::computeModelContribution()
{   
    addVariable(mVarEsto, "E", 0., fabs(getMaxBound()));
    addVariable(mVarFlowCharge, "QC", mMinFlowCharge, mMaxFlowCharge);
    addVariable(mVarFlowDischarge, "QD", mMinFlowDischarge, mMaxFlowDischarge);
    addVariable(mVarStoIni, "StoIni", 0., fabs(getMaxBound()));

    //variables exprimed as expressions on horizon (optional)
    mExpStoIni += mVarStoIni;

    fillExpression(mExpEsto, mVarEsto);
    fillExpression(mExpFlowCharge, mVarFlowCharge);
    fillExpression(mExpFlowDischarge, mVarFlowDischarge);

    // Add charge discharge states, if needed
    addChargeDischargeStates();

    for (uint64_t t = 0; t < mHorizon; ++t) {
        mExpFlow[t] += mExpFlowDischarge[t] - mExpFlowCharge[t];
        mExpEnergy[t] += ((1./mEta) * mExpFlowDischarge[t] - mEta * mExpFlowCharge[t]) * TimeStep(t) ;// Charged power into storage has Eta efficiency
    }

    if (mAddSocConstraints) {
        for (uint64_t t = 0; t < mHorizon; ++t) {
            addConstraint(mExpEsto[t] >= mMinSoc * mExpSizeMax, "MinSOC", t);
            addConstraint(mExpEsto[t] <= mMaxSoc * mExpSizeMax, "MaxSOC", t);
        }
    }

    //if (mNpdtPast > 0 && mControl == "MPC") { // MPC getting intial from past external value
    //    mInitialSoe = mStateOfCharge[mNpdtPast - 1];
    //    cInfo() << "StorageGen - initialSoe from past external value : " << mInitialSoe << parent()->objectName() ;
    //}
    //else if (mNpdtPast >0) { // Rolling Horizon getting initial from past internal value, or default behaviour with 1 past timestep
    //    mInitialSoe=mHistEstock[mNpdtPast-1];
    //    cInfo() << "StorageGen - initialSoe from past internal value  " << mInitialSoe << parent()->objectName() ;
    //}
    //else { // simple initial value from settings, with no past timestep
    //    mInitialSoe = mInitialSoe_Def;
    //    cInfo() << "StorageGen - initialSoe from settings = " << mInitialSoe << parent()->objectName() ;
    //}

    // avoid possible residual out of range values of initialSoe due to solving gaps in MPC or RH modes
    mInitialSoe = min(getMaxBound(), mInitialSoe) ; 
    mInitialSoe = max(getMinBound(), mInitialSoe) ; 

    if (mControl=="") {
            if(mInitSOC >= 0) {
                addConstraint(mExpStoIni == mInitSOC * mExpSizeMax, "StoIni3", 0);
            }
            else {
                addConstraint(mExpStoIni <= fabs(mInitSOC) * mExpSizeMax, "StoIni4", 0);
            }
    }
    else {
        addConstraint(mExpStoIni == mInitialSoe, "StoIni0", 0);
    }
    for (uint64_t t = 0; t < mHorizon ; ++t) {
        addConstraint(mExpEsto[t] - mExpSizeMax * mComponentAvailabilityTS[t] <= 0,"M",t);

        // M(ti)-M(ti-1) - (eta * MFRcharge - MFRdecharge / eta - Kpertes*Esto) * dt = 0
        // sauf au premier pas de temps ou le membre de droite vaut M(t-1) (issue de l'etat du plantModel) ou M(t=0) si optim sans retroaction

        // State Of Energy variation
        mExpLosses[t] += mKlosses * TimeStep(t) * mExpEsto[t];
        if (t == 0) {
            addConstraint(mExpEsto[t] - mExpStoIni + mExpEnergy[t] + mExpLosses[t] == 0,"Sto",t);
        }
        else {
                addConstraint(mExpEsto[t] - mExpEsto[t-1] + mExpEnergy[t] + mExpLosses[t] == 0,"Sto",t);
        }

        // valid positive finalSOC value : set optional constraint to get back to initial state of charge at last time step of planning horizon
        // undefined finalSOC : free final SOC of storage (no constraint on value)

        if (mFinalSoc >0 && t==mHorizon-1) {
            if (mImposeStrictFinalSOC) {
                addConstraint(mExpEsto[t] == mFinalSoc * mExpStoIni, "StrictStoFin", t);
            }
            else {
                addConstraint(mExpEsto[t] >= mFinalSoc * mExpStoIni, "MinStoFin", t);
            }
        }
    }
    if (mAddPressureModel) {
        addPressureModel();
    }

    //Adding flow direction constraints
    if (mFlowDirection) {
        addVariable(mVarFlowDirection,"sens", 0, 1, MIPModeler::MIP_INT);
        for (uint64_t t = 0; t < mHorizon ; ++t) {
            mExpFlowDirection[t] += mVarFlowDirection(t);
            addConstraint(mExpFlowCharge[t] - mMaxFlowCharge* mExpFlowDirection[t] * mAllowCharge[t] <= 0,"MFC",t) ;
            addConstraint(mExpFlowDischarge[t] - mMaxFlowDischarge*(1- mExpFlowDirection[t]) * mAllowDischarge[t] <= 0,"MFD",t) ;
        }
    }


    //Muting on last time steps if pre-computed seasonalCosts model
    if (mSeasonalCosts) {
        //cInfo() << "StorageGen, mSeasonalCosts = " << mSeasonalCosts ;
        for (uint64_t t = mMilpNpdt; t < mHorizon; t++) {
            addConstraint(mExpEsto[t] == 0 , "seasonalMute",t);
            addConstraint(mExpFlowCharge[t] == 0 , "seasonalMute",t);
            addConstraint(mExpFlowDischarge[t] == 0 , "seasonalMute",t);
        }
    }

    for (uint64_t t = 0; t < mHorizon; ++t) {
        mExpEstoBank[t] += mExpEsto[t] * mCapacityMultiplier[t];
        mExpFlowChargeBank[t] += mExpFlowCharge[t] * mCapacityMultiplier[t]  ;
        mExpFlowDischargeBank[t] += mExpFlowDischarge[t] * mCapacityMultiplier[t] ;
        mExpFlowBank[t] += mExpFlowDischargeBank[t] - mExpFlowChargeBank[t]  ; // Discharged power onto bus has Eta efficiency
        mExpEnergyBank[t] += ((1./mEta) * mExpFlowDischargeBank[t] - mEta * mExpFlowChargeBank[t]) * TimeStep(t) ;// Charged power into storage has Eta efficiency
    }


    //GAMS
    ModelerInterface* pExternalModeler = mModel->getExternalModeler();
    if (pExternalModeler != nullptr) {
        std::string compoName = SubModel::parent()->objectName();
        bool designModel = (getMaxBound() < 0.) ? true : false;

        pExternalModeler->addText("");
        pExternalModeler->addComment("+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++");
        pExternalModeler->addComment(" add new StorageGen component");
        pExternalModeler->addComment("+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++");

        ModelerParams vParams;
        vParams.addParam(compoName + "_p_Eta", mEta);
        vParams.addParam(compoName + "_p_KLoss", mKlosses);
        vParams.addParam(compoName + "_p_MaxChargeFlow", mMaxFlowCharge);
        vParams.addParam(compoName + "_p_MinChargeFlow", mMinFlowCharge);
        vParams.addParam(compoName + "_p_MaxDischargeFlow", mMaxFlowDischarge);
        vParams.addParam(compoName + "_p_MinDischargeFlow", mMinFlowDischarge);
        vParams.addParam(compoName + "_p_MaxEstock", fabs(getMaxBound()));
        vParams.addParam(compoName + "_p_MinEstock", getMinBound());
        vParams.addParam(compoName + "_p_InitSOC", mInitSOC);
        vParams.addParam(compoName + "_p_FinalSOC", mFinalSoc);
        vParams.addParam(compoName + "_p_UseFinalEstock", (mFinalSoc > 0));
        vParams.addParam(compoName + "_p_RelaxedFlowDirection", !mFlowDirection);
        pExternalModeler->setModelData(vParams);

        pExternalModeler->addText("$\t setLocal CompoName   " + compoName);
        pExternalModeler->addText("$\t setLocal DesignModel " + std::to_string(designModel));
        pExternalModeler->addText("");
        ModelerParams vOptions;
        vOptions.addParam("DesignModel", "%DesignModel%");
        pExternalModeler->addModelFromFile("%gamslib%/StorageGen/StorageGen.gms", "%CompoName%", vOptions);
    }

}

void StorageGen::addPressureModel()
{
    addVariable(mVarPressureIn, "Pin", 0, mPressureMax);

    if (getMaxBound() > 0.) { // getMaxBound() is always > 0 !!
        fillExpression(mExpPressure, mVarPressureIn);
        for (uint64_t t = 0; t < mHorizon; ++t) {
            addConstraint(mExpPressure[t] - mExpEsto[t] * mPressureMax / getMaxBound() == 0, "pressureInStorage", t);
        }
    }
}

void StorageGen::computeEconomicalContribution()
{
    TechnicalSubModel::computeEconomicalContribution()  ;

    for (uint64_t t = 0; t < mHorizon ; ++t) {
        mExpVariableCosts[t] += mStoragePrice * TimeStep(t) * mExpFlowCharge[t] ;
    }

    if (mAddFinalStorageValue) {
        mExpVariableCosts[mMilpNpdt-1] +=  - mFinalStorageValue[0] * (mExpEsto[mMilpNpdt-1]-mInitialSoe);
    }
}

void StorageGen::computeAllIndicators(const double* optSol)
{
    StorageSubModel::computeAllIndicators(optSol);

    computeProduction(true, mHorizon, mExpLosses, optSol, 1., 0., mInternalLosses.at(0));
    computeProduction(false, *mptrTimeshift, mExpLosses, optSol, 1., 0., mInternalLosses.at(1));
}

