#include "SimulationControl.h"
#include "../base/MilpComponent.h"
#include "GlobalSettings.h"

using namespace GS ;

SimulationControl::SimulationControl(CairnObject* ap_Parent, const std::string& aSimulationControlName, const std::map<std::string, std::string> &aComponent):
    CairnObject(ap_Parent),
    mCompoInputParam(nullptr),
    mCompoInputSettings(nullptr),
    mGUIData(nullptr)
    //TODO: create mException
{
    setName(aSimulationControlName);
    doInit(aComponent);
}

SimulationControl::~SimulationControl()
{
    if (mGUIData) delete mGUIData;
    if (mCompoInputParam) delete mCompoInputParam;
    if (mCompoInputSettings) delete mCompoInputSettings;
}

void SimulationControl::declareCompoInputParam()
{
    mCompoInputParam = new InputParam(this, "CompoInputParam " + Name());
    //...

    mCompoInputSettings = new InputParam(this, "CompoInputSettings " + Name());

    //bool
    mCompoInputSettings->addParameter("RunUntilSimulationEnd", &mRunUntilSimulationEnd, false, false, true, "When true run until the end of all cycles", "-", "Global_Optim");
    mCompoInputSettings->addParameter("ExportResultsEveryCycle", &mExportResultsEveryCycle, false, false, true, "When true export a result file (PLAN and HIST) every cycle", "-", "Global_Optim");
    mCompoInputSettings->addParameter("UseExtrapolationFactor", &mUseExtrapolationFactor, true, false, true, "When true apply Extrapolation factor", "-", "Global_Optim");
    mCompoInputSettings->addParameter("ExportResults", &mExportResults, true, false, true, "When true export all results file", "-", "Global_Optim");
    mCompoInputSettings->addParameter("ShowIndicatorDescription", &mShowIndicatorDescription, false, false, true, "When true add a column for indicators Description in PLAN/HIST", "-", "Global_Optim");

    mCompoInputSettings->addParameter("ExportJson", &mExportJson, false, false, true, "When true export the study architecture as stuydName_self.json file after end of simulation", "-", "Global_Optim");
    mCompoInputSettings->addParameter("ExportParameters", &mExportParameters, false, false, true, "When true export the mandatory and modified parameters after end of simulation", "-", "Global_Optim");
    mCompoInputSettings->addParameter("CheckTimeSeriesUnits", &mCheckTimeSeriesUnits, true, false, true, "When true check if the timeseries units are correct", "-", "Global_Optim");
    //double 
    mCompoInputSettings->addParameter("TimeStep", &mTimeStep, 3600., true, true, "constant TimeStep of optimization - overwritten by studyName_ListeOfTimeSteps.csv file ", "s", "Global_Optim");
    //int
    mCompoInputSettings->addParameter("FutureSize", &mFutureSize, 8760, true, true, "Planning horizon in number of timesteps of constant value TimeStep ", "TimeStep", "Global_Optim");
    mCompoInputSettings->addParameter("NbCycle", &mNbCycle, 1, true, true, "Number of cycles ie rolling horizons to be computed", "-", "Global_Optim");
    mCompoInputSettings->addParameter("TimeShift", &mTimeShift, 1, true, true, "Rolling horizon shifting in number of timesteps", "TimeStep", "Global_Optim");
    mCompoInputSettings->addParameter("PastSize", &mPastSize, 1, true, true, "Past horizon in number of timesteps must be greater than or equal to timeshift", "TimeStep", "Global_Optim");
    mCompoInputSettings->addParameter("FutureVariableTimestep", &mFutureVariableTimestep, 8760, true, true, "Planning horizon in number of variable timesteps ie number of lines of the variable timestep file studyName_ListeOfTimeSteps.csv ", "-", "RH_MPC_Optim");
    //std::string
    mCompoInputSettings->addParameter("ReadingMode", &mReadingMode, "Average", false, true, "Reading mode used to read time series values", "-", "Global_Optim");
    mCompoInputSettings->addParameter("RollingMode", &mRollingMode, "Periodic", false, true, "Reading mode used for rolling horizons", "-", "Global_Optim");
}

void SimulationControl::setCompoInputParam(const std::map<std::string, std::string>& aComponent) 
{
    if (aComponent.size() != 0) {
        int ierr1 = mCompoInputParam->readParameters(aComponent);
        int ierr2 = mCompoInputSettings->readParameters(aComponent);
        if (ierr1 < 0 || ierr2 < 0) {
            Cairn_Exception error("Error while initializing SimulationControl " + Name() + ". A mandatory parameter is missing!", -1);
            throw& error;
        }
    }
}

void SimulationControl::doInit(const std::map<std::string, std::string>& aComponent)
{
    declareCompoInputParam();
    setCompoInputParam(aComponent);

    if (mTimeStep == 0) {
        mTimeStep = 3600;
        cInfo() << "Abnormal NULL timestep, use default 3600s timeStep instead " << mTimeStep;
    }

    if (mGUIData) delete mGUIData;
    mGUIData = new GUIData(this);
    mGUIData->doInit("SimulationControl", "SimulationControl", "SimulationControl", { {"Xpos", CairnUtils::getParam(aComponent,"Xpos")}, {"Ypos", CairnUtils::getParam(aComponent,"Ypos")} });
}

bool SimulationControl::isExportResults()
{
    return mExportResults;
}

bool SimulationControl::showIndicatorDescription()
{
    return mShowIndicatorDescription;
}

bool SimulationControl::isExportJson()
{
    return mExportJson;
}

bool SimulationControl::isExportParameters()
{
    return mExportParameters;
}

bool SimulationControl::isCheckTimeSeriesUnits()
{
    return mCheckTimeSeriesUnits;
}

void SimulationControl::jsonSaveGuiComponent(ojson &componentsArray)
{
    ojson compoObject;   
    mGUIData->jsonSaveGUILine(compoObject);
   
    compoObject["paramListJson"] = ojson::array();
    compoObject["optionListJson"] = ojson::array();
   
    mCompoInputSettings->jsonSaveGUIInputParam(compoObject["paramListJson"]);
    mCompoInputParam->jsonSaveGUIInputParam(compoObject["optionListJson"]);
    
    componentsArray.push_back(compoObject) ;
}

std::map<std::string, InputParam::ModelParam*> SimulationControl::getParameters()
{
    std::map<std::string, InputParam::ModelParam*> paramMap;

    paramMap.insert(getCompoInputParam()->getMapParams().begin(), getCompoInputParam()->getMapParams().end());
    paramMap.insert(getCompoInputSettings()->getMapParams().begin(), getCompoInputSettings()->getMapParams().end());

    return paramMap;
}
