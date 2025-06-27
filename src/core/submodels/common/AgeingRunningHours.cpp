/**
* \file		AgeingRunningHours.h
* \brief	AgeingRunningHours model that compute efficiency reduction from past use time - non constraints nor objective contribution
* \version	1.0
* \author	Alain Ruby
* \date		08/10/2020
*/

#include "AgeingRunningHours.h"
//-------------------------------------------------------------------------------------------
AgeingRunningHours::AgeingRunningHours(InputParam *aInputParam, InputParam *aInputData)
    : OperationSubModel(),
    mHistRunningTime_save(0.),
    mLastReplacementTime(0.),
    mEfficiencyAgeing(1.),
    mCapacityAgeing(1.)
{
    mInputData = aInputData ;
    mInputParam = aInputParam ;
}
//-------------------------------------------------------------------------------------------
AgeingRunningHours::~AgeingRunningHours()
{
    mInputParam = nullptr;
    mInputData = nullptr;
}
//-------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------
void AgeingRunningHours::buildModel()
{
    if (mHistRunningTime - mLastReplacementTime >= mEfficiencyMaxHours)
    {
        mLastReplacementTime += mEfficiencyMaxHours ;
        mHistRunningTime_save = mLastReplacementTime ;
        mEfficiencyAgeing = 1. ;
        mCapacityAgeing = 1.;
    }
    double runningTime = mHistRunningTime - mHistRunningTime_save ;

    qInfo() <<" Changing EfficiencyAgeing after HistRunningTime " << mHistRunningTime
            <<" \n Eff runningTime " << runningTime
            << " since last replacement at " << mLastReplacementTime
            << " \n old EfficiencyAgeing coeff " << mEfficiencyAgeing
            << " \n old CapacityAgeing coeff " << mCapacityAgeing;
    if (mGeometricalSequence){
        mEfficiencyAgeing = mEfficiencyAgeing * pow( 1. - mEfficiencyAgeingCoeff , runningTime) ;
        mCapacityAgeing = mCapacityAgeing * pow( 1. - mCapacityAgeingCoeff , runningTime) ;
    }
    else{
        mEfficiencyAgeing = mEfficiencyAgeing - runningTime * mEfficiencyAgeingCoeff;
        mCapacityAgeing = mCapacityAgeing - runningTime * mCapacityAgeingCoeff;
    }
    qInfo() << " \n new EfficiencyAgeing coeff " << mEfficiencyAgeing
            << " \n new CapacityAgeing coeff " << mCapacityAgeing;
    mHistRunningTime_save = mHistRunningTime ;
}
//-------------------------------------------------------------------------------------------
