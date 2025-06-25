#include "StorageGen.h"
extern "C" MODELS_DECLSPEC QObject * createModel(QObject * aParent)
{
    return new StorageGen(aParent);
}

StorageGen::StorageGen(QObject* aParent) : StorageSubModel(aParent),
    mInitialSoe(0.),
    mInitialSoe_Def(0.),
    mInternalLosses(2,0.)
{

}

StorageGen::~StorageGen()
{}

void StorageGen::setTimeData() {
    SubModel::setTimeData();
    mCapacityMultiplier.resize(mHorizon);
    mAllowCharge.resize(mHorizon);
    mAllowDischarge.resize(mHorizon);
    mAllowStorage.resize(mHorizon);
    mFinalStorageValue.resize(mHorizon);
}

int StorageGen::checkConsistency()
{
    if ((mInitSOC < 0. && mInitSOC != -1.) || mInitSOC > 1.) {
        qCritical() << "ERROR : Storage " << parent()->objectName() << " expects an initial state of charge in the range [0,1] or -1 to use coupling within PEGASE. ";
        return -1;
    }
    return 0;
}

void StorageGen::computeInitialData()
{
    setMaxBound(mMaxEsto);
    setMinBound(mMinEsto);

    mInitialSoe_Def = mInitSOC * fabs(getMaxBound()) ;

    qDebug() << "StorageGen mInitialSoe_Def : " << mInitialSoe_Def;
}

void StorageGen::buildModel() {
    QString aname = parent()->objectName();
    
    setExpSizeMax(mMaxEsto, "MaxESto");
    
    //variable
    if (mAllocate) {
        mVarStoIni        = MIPModeler::MIPVariable0D(0, fabs(getMaxBound()));
                          
        mVarEsto = MIPModeler::MIPVariable1D(mHorizon, getMinBound(), fabs(getMaxBound()));
        mVarFlowCharge = MIPModeler::MIPVariable1D(mHorizon, mMinFlowCharge, mMaxFlowCharge);
        mVarFlowDischarge = MIPModeler::MIPVariable1D(mHorizon, mMinFlowDischarge, mMaxFlowDischarge);

        mExpEsto = MIPModeler::MIPExpression1D(mHorizon);
        mExpFlowCharge = MIPModeler::MIPExpression1D(mHorizon);
        mExpFlowDischarge = MIPModeler::MIPExpression1D(mHorizon);
        mExpFlow = MIPModeler::MIPExpression1D(mHorizon);
        mExpEnergy = MIPModeler::MIPExpression1D(mHorizon);
        mExpLosses = MIPModeler::MIPExpression1D(mHorizon);

        if (mFlowDirection) {
            mVarFlowDirection = MIPModeler::MIPVariable1D(mHorizon, 0, 1, MIPModeler::MIP_INT);
            mExpFlowDirection = MIPModeler::MIPExpression1D(mHorizon);
        }
        mExpEstoBank = MIPModeler::MIPExpression1D(mHorizon);
        mExpFlowChargeBank = MIPModeler::MIPExpression1D(mHorizon);
        mExpFlowDischargeBank = MIPModeler::MIPExpression1D(mHorizon);
        mExpFlowBank = MIPModeler::MIPExpression1D(mHorizon);
        mExpEnergyBank = MIPModeler::MIPExpression1D(mHorizon);
    }
    else {
        closeExpression(mExpStoIni);
        closeExpression1D(mExpEsto);
        closeExpression1D(mExpFlowCharge);
        closeExpression1D(mExpFlowDischarge);
        closeExpression1D(mExpFlow);
        closeExpression1D(mExpEnergy);
        closeExpression1D(mExpLosses);

        if (mFlowDirection) {
            closeExpression1D(mExpFlowDirection);
        }

        closeExpression1D(mExpEstoBank);
        closeExpression1D(mExpFlowChargeBank);
        closeExpression1D(mExpFlowDischargeBank);
        closeExpression1D(mExpFlowBank);
        closeExpression1D(mExpEnergyBank);
    }

    // DO NOT FORGET TO ADD THEM !!!
    addVariable(mVarEsto,"E");
    addVariable(mVarFlowCharge,"QC");
    addVariable(mVarFlowDischarge,"QD");
    addVariable(mVarStoIni,"StoIni");

    //variables exprimed as expressions on horizon (optional)
    mExpStoIni += mVarStoIni;

    fillExpression(mExpEsto, mVarEsto);
    fillExpression(mExpFlowCharge, mVarFlowCharge);
    fillExpression(mExpFlowDischarge, mVarFlowDischarge);

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
    //    qInfo() << "StorageGen - initialSoe from past external value : " << mInitialSoe << parent()->objectName() ;
    //}
    //else if (mNpdtPast >0) { // Rolling Horizon getting initial from past internal value, or default behaviour with 1 past timestep
    //    mInitialSoe=mHistEstock[mNpdtPast-1];
    //    qInfo() << "StorageGen - initialSoe from past internal value  " << mInitialSoe << parent()->objectName() ;
    //}
    //else { // simple initial value from settings, with no past timestep
    //    mInitialSoe = mInitialSoe_Def;
    //    qInfo() << "StorageGen - initialSoe from settings = " << mInitialSoe << parent()->objectName() ;
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
        addConstraint(mExpEsto[t] - mExpSizeMax * mAllowStorage[t] <= 0,"M",t);

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
            addVariable(mVarFlowDirection,"sens");
        for (uint64_t t = 0; t < mHorizon ; ++t) {
            	mExpFlowDirection[t] += mVarFlowDirection(t);
                addConstraint(mExpFlowCharge[t] - mMaxFlowCharge* mExpFlowDirection[t] * mAllowCharge[t] <= 0,"MFC",t) ;
                addConstraint(mExpFlowDischarge[t] - mMaxFlowDischarge*(1- mExpFlowDirection[t]) * mAllowDischarge[t] <= 0,"MFD",t) ;
            }
        }


    //Muting on last time steps if pre-computed seasonalCosts model
    if (mSeasonalCosts) {
        //qInfo() << "StorageGen, mSeasonalCosts = " << mSeasonalCosts ;
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

    // possible minimum capacity for size optimization
    if (mAddMinimumCapacity) {
        double aMaxStoPrim = abs(getMaxBound());
        addMinimumCapacity(aMaxStoPrim);
    }

    /** Compute all expressions */
    computeAllContribution();

    mAllocate = false ;

    //GAMS
    ModelerInterface* pExternalModeler = mModel->getExternalModeler();
    if (pExternalModeler != nullptr) {
        std::string compoName = SubModel::parent()->objectName().toStdString();
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
    if (mAllocate) {
        mVarPressureIn = MIPModeler::MIPVariable1D(mHorizon, 0, mPressureMax);
        mExpPressure = MIPModeler::MIPExpression1D(mHorizon);
    }
    else {
        closeExpression1D(mExpPressure);
    }

    addVariable(mVarPressureIn, "Pin");

    if (getMaxBound() > 0.) {
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
    StorageSubModel::computeDefaultIndicators(optSol);

    computeProduction(true,mHorizon, mNpdtPast, mExpLosses, optSol, 1., 0., mInternalLosses.at(0));
    computeProduction(false, *mptrTimeshift, mNpdtPast, mExpLosses, optSol, 1., 0., mInternalLosses.at(1));
}

