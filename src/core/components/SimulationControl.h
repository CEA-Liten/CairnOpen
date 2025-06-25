#ifndef SimulationControlCOMPO_H
#define SimulationControlCOMPO_H

#include <QtCore>
#include "InputParam.h"
#include "GUIData.h"
#include "MilpData.h"

/**
 * \details
* This component defines the SimulationControl to be used. \\
* It is mandatory. .
*/

class CAIRNCORESHARED_EXPORT SimulationControl: public QObject
{
    Q_OBJECT
public:
    SimulationControl(const QString& aSimulationControlName="SimulationControl", const QMap<QString, QString>& aComponent={});
    virtual ~SimulationControl();

    void jsonSaveGuiComponent(QJsonArray &componentsArray) ;

    int getTimeStep() const {return mTimeStep;}
    int getPastSize() const {return mPastSize;}
    int getFutureSize() const {return mFutureSize;}
    int getTimeShift() const {return mTimeShift;}
    int getNbCycle() const {return mNbCycle;}
    int getFutureVariableTimestep() const {return mFutureVariableTimestep;}
    QString Name() const { return mSimulationControlName; }
    QString getModeObjective() const {return mModeObjective;}
    
    InputParam* getCompoInputParam() { return mCompoInputParam; }  /** Get access to Model Parameters */
    InputParam* getCompoInputSettings() { return mCompoInputSettings; }  /** Get access to Model Parameters */

    bool isExportResults();
    bool isExportJson();
    bool isExportParameters();
    bool isCheckTimeSeriesUnits();

    std::map<QString, InputParam::ModelParam*> getParameters();

private:
    void declareCompoInputParam();
    void setCompoInputParam(const QMap<QString, QString>& aComponent);
    void doInit(const QMap<QString, QString>& aComponent);

    GUIData* mGUIData;                  /** Pointer to GUI Data */
    InputParam* mCompoInputParam;      /** COMPONENT Input parameter List from XML File -> Options */
    InputParam* mCompoInputSettings;   /** COMPONENT Input parameter List from Settings File -> Params */

    QString mSimulationControlName ;
    QString mtype ;
    QString mModeObjective;
    
    int mXpos;
    int mYpos;

    int mTimeStep ;
    int mPastSize ;
    int mFutureSize ;
    int mTimeShift ;
    int mFutureVariableTimestep ;
    int mNbCycle ;
    QString mRollingMode;
    QString mReadingMode;
    QString mUseTypicalPeriodsFile;
    QString mUseVariableTimeStepsFile;
    bool mRunUntilSimulationEnd;  
    bool mExportResultsEveryCycle;
    bool mUseExtrapolationFactor;
    bool mExportResults;
    bool mExportJson;
    bool mExportParameters;
    bool mCheckTimeSeriesUnits;
};

#endif // SimulationControlCOMPO_H
