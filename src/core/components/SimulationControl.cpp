#include "SimulationControl.h"
#include "../base/MilpComponent.h"
#include "GlobalSettings.h"

using namespace GS ;

SimulationControl::SimulationControl(const QString& aSimulationControlName, const QMap<QString, QString> &aComponent):
    mCompoInputParam(nullptr),
    mCompoInputSettings(nullptr),
    mGUIData(nullptr),
    mSimulationControlName(aSimulationControlName)
    //TODO: create mException
{
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
    mCompoInputParam = new InputParam(this, "CompoInputParam" + mSimulationControlName);
    
    //QString
    mCompoInputParam->addParameter("type", &mtype,"SimulationControl", true, true, "Component type", "", {"DONOTSHOW"});
    mCompoInputParam->addParameter("id", &mSimulationControlName, "Cairn", true, true, "Component name", "", {"DONOTSHOW"});
    mCompoInputParam->addParameter("Xpos", &mXpos, 25, false, true, "X position on planteditor", "", {"DONOTSHOW"});
    mCompoInputParam->addParameter("Ypos", &mYpos, 10, false, true, "Y position on planteditor", "", {"DONOTSHOW"});
    
    mCompoInputSettings = new InputParam(this, "CompoInputSettings" + mSimulationControlName);

    //bool
    mCompoInputSettings->addParameter("RunUntilSimulationEnd", &mRunUntilSimulationEnd, false, false, true, "When true run until the end of all cycles", "-", { "Global_Optim", "RH_MPC_Optim" });
    mCompoInputSettings->addParameter("ExportResultsEveryCycle", &mExportResultsEveryCycle, false, false, true, "When true export a result file (PLAN and HIST) every cycle", "-", { "Global_Optim", "RH_MPC_Optim" });
    mCompoInputSettings->addParameter("UseExtrapolationFactor", &mUseExtrapolationFactor, true, false, true, "When true apply Extrapolation factor", "-", { "Global_Optim", "RH_MPC_Optim" });
    mCompoInputSettings->addParameter("ExportResults", &mExportResults, true, false, true, "When true export all results file", "-", { "Global_Optim", "RH_MPC_Optim" });
    mCompoInputSettings->addParameter("ExportJson", &mExportJson, false, false, true, "When true export the study architecture as stuydName_self.json file after end of simulation", "-", { "Global_Optim", "RH_MPC_Optim" });
    mCompoInputSettings->addParameter("ExportParameters", &mExportParameters, false, false, true, "When true export the mandatory and modified parameters after end of simulation", "-", { "Global_Optim", "RH_MPC_Optim" });
    mCompoInputSettings->addParameter("CheckTimeSeriesUnits", &mCheckTimeSeriesUnits, true, false, true, "When true check if the timeseries units are correct", "-", { "Global_Optim", "RH_MPC_Optim" });
    //int
    mCompoInputSettings->addParameter("TimeStep", &mTimeStep, 3600, true, true, "constant TimeStep of optimization - overwritten by studyName_ListeOfTimeSteps.csv file ", "s", { "Global_Optim", "RH_MPC_Optim" });
    mCompoInputSettings->addParameter("FutureSize", &mFutureSize, 8760, true, true, "Planning horizon in number of timesteps of constant value TimeStep ", "TimeStep", { "Global_Optim", "RH_MPC_Optim" });
    mCompoInputSettings->addParameter("NbCycle", &mNbCycle, 1, true, true, "Number of cycles ie rolling horizons to be computed", "-", { "Global_Optim", "RH_MPC_Optim" });
    mCompoInputSettings->addParameter("TimeShift", &mTimeShift, 1, true, true, "Rolling horizon shifting in number of timesteps", "TimeStep", { "Global_Optim", "RH_MPC_Optim" });
    mCompoInputSettings->addParameter("PastSize", &mPastSize, 1, true, true, "Past horizon in number of timesteps must be greater than or equal to timeshift", "TimeStep", { "Global_Optim", "RH_MPC_Optim" });
    mCompoInputSettings->addParameter("FutureVariableTimestep", &mFutureVariableTimestep, 8760, true, true, "Planning horizon in number of variable timesteps ie number of lines of the variable timestep file studyName_ListeOfTimeSteps.csv ", "-", { "RH_MPC_Optim" });
    //QString
    mCompoInputSettings->addParameter("ModeObjective", &mModeObjective, "Mono", true, true, "Optimization types are: - Mono by default keeps the traditional mono objective of NPV without taking other into account - Blend to add several objectives to the main one with coefficients - Alphanumeric TODO requires an order between the objectives", "String", { "Global_Optim", "RH_MPC_Optim" });
    mCompoInputSettings->addParameter("ReadingMode", &mReadingMode, "Average", false, true, "Reading mode used to read time series values", "-", { "Global_Optim", "RH_MPC_Optim" });
    mCompoInputSettings->addParameter("RollingMode", &mRollingMode, "Periodic", false, true, "Reading mode used for rolling horizons", "-", { "Global_Optim", "RH_MPC_Optim" });
}

void SimulationControl::setCompoInputParam(const QMap<QString, QString>& aComponent) 
{
    if (aComponent.size() != 0) {
        int ierr1 = mCompoInputParam->readParameters(aComponent);
        int ierr2 = mCompoInputSettings->readParameters(aComponent);
        //if (ierr1 < 0 || ierr2 < 0) { return false; } //Error
    }
}

void SimulationControl::doInit(const QMap<QString, QString>& aComponent)
{
    declareCompoInputParam();
    setCompoInputParam(aComponent);

    if (mTimeStep == 0) {
        mTimeStep = 3600;
        qInfo() << "Abnormal NULL timestep, use default 3600s timeStep instead " << mTimeStep;
    }

    if (mGUIData) delete mGUIData;
    mGUIData = new GUIData(this, "Cairn");
    mGUIData->doInit("SimulationControl", "SimulationControl", "SimulationControl", mXpos, mYpos);
}

bool SimulationControl::isExportResults()
{
    return mExportResults;
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

void SimulationControl::jsonSaveGuiComponent(QJsonArray &componentsArray)
{
    QJsonArray paramArray;
    QJsonArray optionArray;

    mCompoInputSettings->jsonSaveGUIInputParam(paramArray);
    mCompoInputParam->jsonSaveGUIInputParam(optionArray);
    
    QJsonObject compoObject;
    mGUIData->jsonSaveGUILine(compoObject);
    compoObject.insert("paramListJson", paramArray);
    compoObject.insert("optionListJson", optionArray);

    componentsArray.push_back(compoObject) ;
}

std::map<QString, InputParam::ModelParam*> SimulationControl::getParameters()
{
    std::map<QString, InputParam::ModelParam*> paramMap;

    paramMap.insert(getCompoInputParam()->getMapParams().begin(), getCompoInputParam()->getMapParams().end());
    paramMap.insert(getCompoInputSettings()->getMapParams().begin(), getCompoInputSettings()->getMapParams().end());

    return paramMap;
}
