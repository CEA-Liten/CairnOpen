/**
* \file		AgeingRunningHours.h
* \brief	AgeingRunningHours model that forces energy system to take into account
*           limitation constrains between two consecutive times
* \version	1.0
* \author	Yacine Gaoua
* \date		23/07/2019
*/

#ifndef AgeingRunningHours_H
#define AgeingRunningHours_H

//#include "MIPModeler.h"
#include "OperationSubModel.h"

/** ---------------------------------------------------
 * \details:  * This component will be used in other component submodels to describe AgeingRunningHourss efficiency reduction due to running time ageing.
*/
class CAIRNCORESHARED_EXPORT AgeingRunningHours : public OperationSubModel {
public:
//----------------------------------------------------------------------------
    AgeingRunningHours(InputParam* aInputParam, InputParam* aInputData);
    ~AgeingRunningHours();
//----------------------------------------------------------------------------------------------------
    void buildModel();
//----------------------------------------------------------------------------------------------------
    void computeAllContribution(){}
    void computeAllIndicators(const double* optSol) {}
//----------------------------------------------------------------------------------------------------
    int checkUnit(MilpPort* aPort) {return 0 ;}
    void declareModelInterface() 
    {
        // no model interface - keep this line to enable correct documentation generation
    }
    void declareModelConfigurationParameters() 
    {
        // no configuration parameters - keep this line to enable correct documentation generation
    }
    void declareModelIndicators() {
        // no indicators - keep this line to enable correct documentation generation
    }
//----------------------------------------------------------------------------------------------------
    void declareModelParameters()
    {
        //bool
        addParameter("GeometricalSequence", &mGeometricalSequence, true, false, true, "If the efficiency decreases following a geometrical set true. Else it will evolve artimtetically", "", { "Ageing" });
        //double
        addConfig("HistRunningTime", &mHistRunningTime, 0., &mUseAgeing, true, "","",{"Ageing"}); /** Number of past running hours for efficiency reduction by ageing*/
        addParameter("EfficiencyAgeingCoeff", &mEfficiencyAgeingCoeff, 0., &mUseAgeing, true,"","",{"Ageing"}) ; /** Coefficient of efficiency reduction, in proportion of current efficiency per running time hours */
        addParameter("EfficiencyMaxHours", &mEfficiencyMaxHours, 4.e4, &mUseAgeing, true,"","",{"Ageing"}) ; /** Maximum number of hours before assuming replacement and resetting Efficiency to initial efficiency. */
        addParameter("CapacityAgeingCoeff", &mCapacityAgeingCoeff, 0., &mUseAgeing, true, "", "", {"Ageing"});/** Coefficient of capacity reduction, in proportion of current capacity per running time hours */
    }

//----------------------------------------------------------------------------
    double EfficiencyAgeing() const {return mEfficiencyAgeing ;}
    double CapacityAgeing() const {return mCapacityAgeing ;}

    void initDefaultPorts() { };

protected:
    /** Ageing model input data */
    double mHistRunningTime ;
    double mHistRunningTime_save ;
    double mLastReplacementTime ;
    double mEfficiencyAgeing ;  /** Computed Efficiency reduction by ageing. */
    double mEfficiencyAgeingCoeff ;  /** Efficiency reduction coefficient by ageing, per running Time hour */
    double mEfficiencyMaxHours ; /** Maximum efficiency loss */
    double mCapacityAgeing ; /** Computed capacity reduction by ageing. */
    double mCapacityAgeingCoeff ;  /** capacity reduction coefficient by ageing, per running Time hour */
    bool mGeometricalSequence;
};

#endif // AgeingRunningHours_H
