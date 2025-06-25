#ifndef OptimProblem_H
#define OptimProblem_H

class OptimProblem;

#include <QtCore>
#include <qdom.h>
#include <QMap>
#include <QVector>
#include <QDataStream>
#include <QDebug>
#include <QDir>

#include <math.h>   
#include <iostream>
#include <fstream>

#include <Eigen/SparseCore>
#include <Eigen/Dense>

#include "CairnCore_global.h"
#include "GlobalSettings.h"

#include "TecEcoEnv.h"
#include "MilpData.h"
#include "MIPModeler.h"
#include "MilpComponent.h"
#include "ZEVariables.h"
#include "JsonDescription.h"

#include "GridCompo.h"
#include "SourceLoadCompo.h"
#include "BusCompo.h"
#include "MultiObjCompo.h"

#include "Solver.h"
#include "TecEcoAnalysis.h"
#include "EnergyVector.h"
#include "SimulationControl.h"

#include "ModelFactory.h"
#include "DynamicIndicator.h"
#include "StudyPathManager.h"
#include "ModelVar.h"
/**
 * \brief La classe OptimProblem permet de definir une collection de pb MILP OptimProblem
 */

static inline std::string printMat(MatrixXf *mat){
    std::stringstream ss;
    ss << *mat;
    return ss.str();
}


static inline double Energy(MatrixXf* positions, MatrixXf* distances){
    int n = distances->rows();
    MatrixXf D = MatrixXf::Constant(n,n,1);
    int maxD = distances->maxCoeff();
    double E = 0;
    MatrixXf distXY = MatrixXf::Zero(n,n);
    for (int i=0; i<n;i++){
        for (int j=0; j<n; j++){
            distXY(i,j) = sqrt(pow((*positions)(i,0)-(*positions)(j,0),2)
                               +pow((*positions)(i,1)-(*positions)(j,1),2));
            D(i,j) = (*distances)(i,j)/maxD; // a verifier, c'est bizarre..
        }
    }
    for (int i=0; i<n;i++){
        for (int j=0; j<n; j++){
            if(i!=j){
                E+=(1/sqrt(2) * pow(distXY(i,j)-(*distances)(i,j),2)/D(i,j));
            }
        }
    }
    return E;
}


static inline MatrixXf gradient(double (*Y_func)(MatrixXf*,MatrixXf*),MatrixXf *pos,MatrixXf *param,double* dx){
    int sizeP = pos->rows();
    MatrixXf Xdx;
    MatrixXf gradY(sizeP,2);
    for (int i = 0; i<sizeP; i++){
        for (int j = 0; j<2; j++){
            Xdx = pos->replicate(1,1);
            Xdx(i,j)=Xdx(i,j)+(*dx);
            QString strM = QString::fromStdString(printMat(&Xdx));
            gradY(i,j) = ((Y_func)(&Xdx,param) - (Y_func)(pos,param))/(*dx);
        }
    }
    return gradY;
}

struct GradDescResult {   // Declare PERSON struct type
    QList<MatrixXf> X;   // Declare member types
    QList<double> Y;
    double gap;
    int iter;
    bool cond;
} typedef GradDescResult;


static inline GradDescResult GradientDescent(double (*Y_func)(MatrixXf*,MatrixXf*),
                                     MatrixXf *init, MatrixXf *param,
                                     int nbMaxIterations, double gapStop, double dx,
                                     double alpha){
    //Preparation de X et recopie de Xinit
    QList<MatrixXf> X;
    QList<double> Y;
    X.append(*init);
    Y.append(Y_func(&X[0],param));
    int iter = 1;
    MatrixXf grad = gradient(Y_func,init,param,&dx);
    double gap = 10;
    bool cond = true;
    std::chrono::high_resolution_clock::time_point a= std::chrono::high_resolution_clock::now();
    std::chrono::high_resolution_clock::time_point b= std::chrono::high_resolution_clock::now();
    unsigned int time= std::chrono::duration_cast<std::chrono::seconds>(b - a).count();
    while(abs(gap)>gapStop && iter<nbMaxIterations && cond && time<120){
        b= std::chrono::high_resolution_clock::now();
        time= std::chrono::duration_cast<std::chrono::seconds>(b - a).count();
        X.append(X[iter-1] - (alpha)*grad);
        Y.append(Y_func(&X[iter],param));
        gap = Y[iter]-Y[iter-1];
        if (Y[iter]>Y[iter-1])
            cond=false;
        grad = gradient(Y_func,&X[iter],param,&dx);
        iter++;
    }
    qDebug()<<"Gradient stops:"<<gap<<"iter"<<iter<<"cond"<<cond;
    qDebug()<<"time:"<<time;
    GradDescResult res;
    res.X = X;
    res.Y = Y;
    res.gap = gap;
    res.iter = iter;
    res.cond = cond;
    return(res);
}

typedef MilpComponent* (*f_MilpComponent)(QObject* aParent, QString aName, MilpData* aMilpData, TecEcoEnv &aTecEcoEnv,
                                          const QMap<QString, QString> &aComponent, const QMap < QString, QMap<QString, QString> >& aPorts);

class CAIRNCORESHARED_EXPORT OptimProblem : public MilpComponent
{
    Q_OBJECT
public:
    OptimProblem(QObject* aParent, QString aName, MilpData *aMilpData, TecEcoEnv &aTecEcoEnv, const QMap<QString, QString> &aComponent={});
    virtual ~OptimProblem();

    void doInit(const StudyPathManager& aStudy, bool aLoad);
    void doInit(const QString& aDescFile, const QString& aScenarioName);

    void setMIPModel(MIPModeler::MIPModel* aModel) ;
    void setObjective(MIPModeler::MIPExpression* aExpObjective) ;

    QString getOptimDirection() ;

    int initProblem();
    int initSubModelInput();
    int setParameters();
    int getNumberOfSolutions();

    void setExtrapolationFactor();

    void buildProblem();
    
    void readSolution(int n=0);
    void cleanExpressions();

    void writeSolution(int n, std::map<std::string, std::vector<double>>& resultats);    

    void exportEnvImpactMassIndicators(QString& aFileName = QString(""), QString& encoding = QString("UTF-8"));
    void exportEnvImpactParameters(QString& aFileName = QString(""), QString& encoding = QString("UTF-8"));
    void exportPortEnvImpactParameters(QString& aFileName = QString(""), QString& encoding = QString("UTF-8"));
    void exportParameters(QString& aFileName = QString(""), QString& encoding = QString("UTF-8")); 
    void exportParameters(QTextStream& out, QString& name, std::map<QString, InputParam::ModelParam*>& paramMap, EnergyVector* aEnergyVector=nullptr, QMap<QString, QString> aTimeSeriesNames={});

    void initSubModelTopology();
    void setStopSignal(int* stopSignal);
    void solveProblem(QString& optimLogFileName, const int cycle, const QMap<QString, bool> paramMap = QMap<QString, bool>(), const bool aExportResultsEveryCycle = false);

    void prepareOptim();
    void exportResults();
    void setDefaultsResults();
    void computeTecEcoEnvAnalysis(const int& aNsol, const int& istat);
    void exportMultiObjFile(QTextStream& out, int aNsol);
    void exportAllTecEcoEnvAnalysis(QString aResultFile, QString range, QString encoding = "UTF-8", const bool isRollingHorizon=false, const int aNsol=0);
    void computeHistState();
    void exportOptimaSizeAllCycles(const QString& aFileName, const int cycle, QString encoding = "UTF-8");

    double getSolverRunningTime() { return mSolver->getSolverRunningTime();  }

    void buildComponentConstraints();
    void buildManualObjectiveConstraints();
    void buildBusConstraints();

    void registerMilpComponent(QMap<QString,QString> component, MilpComponent* lptr);
    void registerEnergyVector(const QString &componentId, EnergyVector *lptr);

    void createTecEcoAnalysis();
    void createMilpComponentsFromParamMap();
    void createPortsAndLinksToBus();
    void createDynamicIndicators();
    void computeDynamicIndicators(const int& aNsol); //should be called after the end of the simulation

    bool createComponent(const QMap<QString, QString> &component, const QMap < QString, QMap<QString, QString> >& ports={});
    void createPortsAndLinksToBus(MilpComponent *aComponent);
    void deleteComponent(MilpComponent* lptrComponent);

    void createZEVariablesList() ;
    const t_mapExchange &ListSubscribedVariables() { return mListSubscribedVars; }
    const t_mapExchange &ListPublishedVariables() { return mListPublishedVars; }        /** define OUTPUT Variable of component */

    int SaveFullArchitecture(QString& filename=QString(""), QString& posAlgorithm = QString(""));
    void jsonSaveDocument (QFile* jsonOutputFile, QString encoding="UTF-8");
    void jsonSaveGuiComponents(QJsonArray &componentsArray);
    void jsonSaveGuiLinks(QJsonArray &linksArray);
    void jsonSaveGuiLinkNodes(QJsonArray& linksArray, const QString& compoName, const QString& compoPortName, const QString& busName, const QString& busPortName,
        const int& compoX, const int& compoY, const int& busX, const int& busY); 
    TecEcoEnv* getTecEcoEnv(){return mTecEcoEnv;}

    bool createEnergyVector(const QString& aName, const QString &aType, const QMap<QString, QString> paramMap={}, const QSettings& aSettings=QSettings());
    EnergyVector* getEnergyVector(const QString& aName);
    QList<EnergyVector*> getEnergyVectorList();
    void deleteEnergyVector(const QString& aName);

    bool createSolver(const QString& aSolverName = "Cbc", const QMap<QString, QString>& paramMap = {});
    Solver* getSolver() { return mSolver; }

    bool createSimulationControl(const QString& mSimulationControlName = "SimulationControl", const QMap<QString, QString>& paramMap={});
    SimulationControl* getSimulationControl() { return mSimulationControl; };
    
    TecEcoAnalysis* getTecEcoAnalysis() { return mTecEcoAnalysis; };

    f_MilpComponent LoadDllMilpComponent(QString Filename, QString ModuleName) ;

    QString getOptimisationStatus() ;
    int getInterpretedOptimStatus();
    bool getIsCheckConflicts();

    void computeGUICompoAndBusLocations();

    void computeGUIBusLocations();
    void computeGUIComponentLocations();

    static QString getRelease() {return GS::Cairn_Release;}

    void setStdAloneMode (const bool & abool) {mStdAloneMode = abool ;}

    void initOptimProblemFromTecEcoAnalysis();
    void setCompoTecEcoEnvSettings(TecEcoEnv& aTecEcoEnv);

    void computeObjectiveFunction(MIPModeler::MIPExpression& objective);
    void resetFlags();
    
private:    
    TecEcoEnv* mTecEcoEnv ;
    ModelFactory* mModelFactory;
    JsonDescription* mJsonDescription ;
    Solver* mSolver;
    TecEcoAnalysis* mTecEcoAnalysis; //== mCompoModel
    SimulationControl* mSimulationControl;
    
    const StudyPathManager* mStudyFile{ nullptr };   // the architecture of study file

    bool mStdAloneMode;  // true indicates no link with Pegase
    bool mForceExportAllIndicators;

    MIPModeler::MIPExpression* mExpObjective;
    int mOptimStatus;

    QVector< QMap<QString,QString> > mMilpComponents ;      // Listes cle / valeur de description des composants MILPs a partir du fichier de description
    QMap<QString,MilpComponent*>   mMapMilpComponents ;   // map nom / pointeurs des composants MILPs du probleme
    QMap<QString,EnergyVector*>   mMapEnergyVectors ;

    t_mapExchange mListPublishedVars;  // export
    t_mapExchange mListSubscribedVars; // import

    //------------------------------ Dynamic Indicators ----------------------------------------------------------------- 
    QVector< QMap<QString, QString> > mDynamicIndicatorsData{}; // List of dynamic indicators data in the form "key: value"  
    QVector<DynamicIndicator*> mDynamicIndicators{}; // List of pointers to DynamicIndicator objects  

    bool mExportIndicators;

    bool newCompoModel();    
};

#endif // OptimProblem_H
