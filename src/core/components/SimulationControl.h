#ifndef SimulationControlCOMPO_H
#define SimulationControlCOMPO_H

#include "InputParam.h"
#include "GUIData.h"
#include "MilpData.h"

/**
 * \details
* This component defines the SimulationControl to be used. \\
* It is mandatory. .
*/

class CAIRNCORESHARED_EXPORT SimulationControl: public CairnObject
{
    
public:
    SimulationControl(CairnObject *ap_Parent, 
        const std::string& aSimulationControlName = "SimulationControl", 
        const t_mapParamData& aComponent = {});
    
    ~SimulationControl();

    void jsonSaveGuiComponent(ojson &componentsArray) ;

    int getStartTime() const { return mStartTime; }
    int getEndTime() const { return mEndTime; }
    int getTimeStep() const {return mTimeStep;}
    int getPastSize() const {return mPastSize;}
    int getFutureSize() const {return mFutureSize;}
    int getTimeShift() const {return mTimeShift;}
    int getNbCycle() const {return mNbCycle;}
    int getFutureVariableTimestep() const {return mFutureVariableTimestep;}

    std::string Name() const { return std::string(this->objectName().c_str()); }
    void setName(const std::string& name) { this->setObjectName(name); }

    InputParam* getCompoInputParam() { return mCompoInputParam; }  /** Get access to Model Parameters */
    InputParam* getCompoInputSettings() { return mCompoInputSettings; }  /** Get access to Model Parameters */

    bool isExportResults();
    bool showIndicatorDescription();
    bool isExportJson();
    bool isExportParameters();
    bool isCheckTimeSeriesUnits();

    std::map<std::string, ModelParam*> getParameters();
    GUIData* getGUIData() { return mGUIData; }
    std::vector<InputParam*> get_InputParams();

    std::vector<InputParam*> get_ParamInputParams();
    std::vector<InputParam*> get_OptionInputParams();

    std::vector<std::string> getReadingModes() const {
        return std::vector<std::string>{ "Average", "Interpolation" };
    };
        
    std::vector<std::string> getRollingModes() const {
        return std::vector<std::string>{ "Periodic", "Persistent", "Stop" };
    };

private:
    void declareCompoInputParam();
    void setCompoInputParam(const t_mapParamData& aComponent);
    void doInit(const t_mapParamData& aComponent);

    GUIData* mGUIData;                  /** Pointer to GUI Data */
    InputParam* mCompoInputParam;      /** COMPONENT Input parameter List from XML File -> Options */
    InputParam* mCompoInputSettings;   /** COMPONENT Input parameter List from Settings File -> Params */
    
    int mStartTime; // First time in TimeSetp to consider from the input timeseries
    int mEndTime;   // Last time in TimeSetp to consider from the input timeseries
    double mTimeStep; /** TimeStep is double, not int */
    int mPastSize;
    int mFutureSize;
    int mTimeShift;
    int mFutureVariableTimestep;
    int mNbCycle;
    std::string mRollingMode;
    std::string mReadingMode;
    std::string mUseTypicalPeriodsFile;
    std::string mUseVariableTimeStepsFile;
    bool mRunUntilSimulationEnd;  
    bool mExportResultsEveryCycle;
    bool mUseExtrapolationFactor;
    bool mExportResults;
    bool mShowIndicatorDescription;
    bool mExportJson;
    bool mExportParameters;
    bool mCheckTimeSeriesUnits;
};

#endif // SimulationControlCOMPO_H
