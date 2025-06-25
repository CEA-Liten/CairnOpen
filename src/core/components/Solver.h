#ifndef SOLVERCOMPO_H
#define SOLVERCOMPO_H

#include <QtCore>
#include "InputParam.h"
#include "GUIData.h"

#include "MIPModeler.h"
#include "MIPSolverFactory.h"
#include "ModelerFactory.h"

#include "Cairn_Exception.h"

/**
 * \details
* This component defines the solver to be used. \\
* It is not mandatory. By default, Cbc solver will be used.
*/

class CAIRNCORESHARED_EXPORT Solver: public QObject
{
    Q_OBJECT
public:
    Solver(const QString& aSolverName, const QMap<QString, QString>& aComponent={});
    virtual ~Solver();
       
    void SolveProblem(MIPModeler::MIPModel* aModel, const QString &location, const int cycle=0, const QMap<QString, bool> paramMap = QMap<QString, bool>());
    QString getOptimisationStatus();
    int getNumberOfSolutions();
    double getSolverRunningTime();

    const double* getOptimalSolution(int aNsol, const std::string& varname="");
    bool getIsCheckConflicts();
    
    void jsonSaveGuiComponent(QJsonArray &componentsArray) ;

    double getTimeLimit() const {return mTimeLimit;}
    double getGap() const {return mGap;}
    int getThreads() const {return mThreads;}
    QString getModelType() const {return mModelType;};
    QString Name() const {return mSolverName;};
    void setStopSignal(int* stopSignal);
    ModelerInterface *getExternalModeler();
   
    InputParam* getCompoInputParam() { return mCompoInputParam; }  /** Get access to Model Parameters */
    InputParam* getCompoInputSettings() { return mCompoInputSettings; }  /** Get access to Model Parameters */

    std::map<QString, InputParam::ModelParam*> getParameters();

    Cairn_Exception getException() const { return mException; };

private:
    void doInit(const QMap<QString, QString>& aComponent);
    void declareCompoInputParam();
    bool setCompoInputParam(const QMap<QString, QString>& aComponent);
    void InitSolverParam();

    Cairn_Exception mException;

    ModelerFactory mModelers;
    ModelerInterface* mExternalModeler;
    ModelerResults mExternalResults;

    MIPSolverFactory mSolvers;
    MIPSolverResults mSolverResults;

    MIPModeler::MIPModel* mModel;
    GUIData* mGUIData;                 /** Pointer to GUI Data */

    InputParam* mCompoInputParam;      /** COMPONENT Input parameter List from XML File -> Options */
    InputParam* mCompoInputSettings;   /** COMPONENT Input parameter List from Settings File -> Params */

    QString mName;                                /** Component Name */
    QString mModelId;
    QString mComponentCairnType;
    QString mModelType;
    QString mSolverName;
    QString mProblemType;
    int mXpos;
    int mYpos;
    QString mWriteLp;
    QString mFileMipStart;
    QString mWriteMipStart;
    QString mReadParamFile;
    QString mWriteGMS;
    int mNbSolToKeep;
    double mGap;
    double mTimeLimit;
    int mThreads;
    int mTreeMemoryLimit; // MB

    int* mTerminateSignal;

    double mSolverRunningTime;
};

#endif // SOLVERCOMPO_H
