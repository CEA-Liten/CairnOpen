#ifndef TecEcoEnv_H
#define TecEcoEnv_H
class TecEcoEnv ;

#include <QtCore>
#include "CairnCore_global.h"
#include "GUIData.h"
#include <QtMath>

static inline int Newton(const double &aValue, const uint &aIarg, uint aOffset,const double aExtrapolateOverYear, double &X_Set, double (*Y_func)(const double&, const uint&, unsigned int,const double), bool (*Y_Test)(const double&, const double&))
{

    // Methode de Newton - secante (pas de calcul de derivee analytique disponible
    //  + convergence quadratique
    //  - 3 appels de la focntion a chaque iteration pour gerer les seuils min et max aux bords du domaine de definition

    //double Newton_Solve (unsigned int* p_Row_ID, double &X_Set, double (*Y_func)(unsigned int*, double), bool (*Y_Test)(unsigned int*))
    // Search for X_Set such that Y_Set(X_Set) < Epsilon using an approximated Newton scheme.
    // Input :
    // - Starting point of Newton is set to X_Set.
    // - *p_Row_Id current time step number
    // - Y_func function to solve
    // - Y_Test testing function to stop Newton iterative process
    // Output :
    // - Solution returned in X_Set.
    // - Returned values of function : 0 value if converged, negative value if not converged (equal to -iteration number after non convergence)

    bool iconv = false  ;
    int ier = 0 ;
    int iter = 0 ;
    double Y = 0.0, Delta_X = 0.0, epsX=1.e-2 ;
    double YPDYm = 0.0, YPDYp = 0.0, Yprim ;

    while ( iconv == false )
    {
        iter++;

        if (iter > 20)
        {
            qDebug() << "INFO convergence :  after " << iter << " iterations iconv " << iconv
                     << " Y = " << Y << " at X = " << X_Set << " Delta_X " << Delta_X << " Yprim- " << YPDYm << " Yprim+ " << YPDYp ;
            return -iter;
        }


        YPDYm = (*Y_func) (X_Set - epsX, aIarg, aOffset,aExtrapolateOverYear) ;
        YPDYp = (*Y_func) (X_Set + epsX, aIarg, aOffset,aExtrapolateOverYear) ;

        Yprim = ( YPDYp - YPDYm ) / (2. *epsX ) ;

        Y = (*Y_func) (X_Set, aIarg, aOffset,aExtrapolateOverYear) ;

        if (fabs(Yprim) > 1.e-20) {
            Delta_X = -(Y - aValue) / Yprim ;
        }
        else
        {
            Delta_X = 0. ;
        }

        iconv = (*Y_Test)(Y, aValue) ;

//        qDebug() << "INFO convergence :  " << iter << " iconv " << iconv << " target " << aValue
//                << " Y = " << Y << " at X = " << X_Set << " Delta_X " << Delta_X << " Yp- " << YPDYm << " Y+ " << YPDYp << " Yprim " << Yprim ;
        X_Set = X_Set + Delta_X ;

    }

    return ier ;
}
static inline bool levelization_test(const double  &aSum, const double &aValue)
{
    if ( abs(aSum - aValue) < 1.e-4 )
        return true;
    else
        return false;
}

static inline double levelization(const double  &aDiscountRate, const unsigned int &Nyear, unsigned int aOffset, const double aExtrapolateOverYear)
{
    double sum = 0. ;

    if (Nyear <= 1) return 1. ;

    for (unsigned int i=0+aOffset; i<Nyear+aOffset; i++)
    {
       sum += 1./pow((1.+aDiscountRate),i) ;
    }
//    qDebug() << "levelization factor simple " << sum ;
    return sum*aExtrapolateOverYear ;
}



static inline double discountRate(const double  &aDiscountFactor, const unsigned int &aNyear, unsigned int aOffset,const double aExtrapolateOverYear)
{
    double discountRate = 0.2 ;

    if (aDiscountFactor > 0.)
    {
        int iconv = Newton (aDiscountFactor, aNyear, aOffset,aExtrapolateOverYear, discountRate, &levelization, &levelization_test) ;

        if (iconv <0) qWarning ()<< "Non convergence in discountrate computation" << iconv ;
    }
    else
    {
        discountRate = 0. ;
    }

    return discountRate ;
}

static inline double levelization(const double  aDiscountRate, const unsigned int Nyear, const unsigned int aAbsoluteCurrentTimeStep,
                                  const unsigned int aNbYearInput, const unsigned int aLeapYearPos, unsigned int aOffset, const double aExtrapolateOverYear)
{
    double extra;
    if (Nyear <= 1) return 1. ;
    if (aNbYearInput>1){
        extra = 1.;
    }
    else{
        extra = aExtrapolateOverYear;
    }

    QVector<double> yearsHours;
    yearsHours.resize(aNbYearInput);

    int hours = 0;
    for (unsigned int i = 0; i < aNbYearInput; i++) {
      if (i == (aLeapYearPos - 1))
      {
          hours += 8784;
      }
      else {
          hours += 8760;
      }
      yearsHours[i] = hours;
    }

    int pos = 0;
    bool notFound = true;
    while(pos < yearsHours.size() && notFound)
    {
        if (aAbsoluteCurrentTimeStep < yearsHours[pos])
        {
            notFound = false;
        }
        else {
            pos += 1;
//            qDebug() << "POS +=1 !! " << " absolutecurrent time step " << aAbsoluteCurrentTimeStep << yearsHours.size() ;
        }
    }

    unsigned int NbYearSimu = pos ;

    QVector<double> levelizedFactors;
    levelizedFactors.resize(aNbYearInput+1);

    for (int j = 0; j < levelizedFactors.size(); j++) {
        double factor = 0.;
        for (int i = 0+aOffset; i < qCeil((Nyear+aOffset)/aNbYearInput); ++i) {
            factor += 1./pow((1.+aDiscountRate),aNbYearInput*i+j) ;
        }
        levelizedFactors[j]=factor;
    }

//    qDebug() << "levelization factor " << levelizedFactors[NbYearSimu] << " nbyear " << NbYearSimu << " absolutecurrent time step " << aAbsoluteCurrentTimeStep ;
    return levelizedFactors[NbYearSimu] * extra;
}

static inline QVector<double> levelizationTable(const double  aDiscountRate, const unsigned int Nyear,
                                  const unsigned int aNbYearInput, const unsigned int aLeapYearPos, unsigned int aOffset,
                                                const double aExtrapolateOverYear)
{
    double extra;
    QVector<double> vide(1,1.);
    if (Nyear <= 1) return vide ;
    if (aNbYearInput>1){
        extra = 1.;
    }
    else{
        extra = aExtrapolateOverYear;
    }
    int leapyear = aLeapYearPos;
    QVector<double> yearsHours;
    yearsHours.resize(aNbYearInput);

    int hours = 0;
    for (unsigned int i = 0; i < aNbYearInput; i++) {
      if (i == (leapyear - 1))
      {
          leapyear = leapyear + 4;
          hours += 8784;
      }
      else {
          hours += 8760;
      }
      yearsHours[i] = hours;
    }

    QVector<double> levelizedFactors;
    levelizedFactors.resize(aNbYearInput);
    int i = 0;
    for (int j = 0; j < levelizedFactors.size(); j++) {
        double factor = 0.;
        int ceil = qCeil((Nyear + aOffset) / aNbYearInput);
        i = 0;
        while ((aNbYearInput * i + j) < Nyear) {
            factor += 1. / pow((1. + aDiscountRate), aNbYearInput * i + j);
            i += 1;
        }
        levelizedFactors[j]=factor * extra;
    }

    //qDebug() << "levelization factor " << levelizedFactors;
    return levelizedFactors;
}
static inline QVector<double> yearHourTable(const unsigned int aNbYearInput, const unsigned int aLeapYearPos){
    QVector<double> yearsHours;
    yearsHours.resize(aNbYearInput);

    int hours = 0;
    for (unsigned int i = 0; i < aNbYearInput; i++) {
      if (i == (aLeapYearPos - 1))
      {
          hours += 8784;
      }
      else {
          hours += 8760;
      }
      yearsHours[i] = hours;
    }
    return yearsHours;
}
/**
 * \brief The TecEcoEnv class implements Techno Economic analysis functionnalities for MilpComponent assessment :
 * yields information on OptimalObjective contribution, Optimal nominal power or storage capacity, Running time, produced mass or energy...
 */
class CAIRNCORESHARED_EXPORT TecEcoEnv: public QObject
{
    Q_OBJECT
public:
    TecEcoEnv(QObject *aParent=nullptr, QString aName="",
              const double aDiscountRate=0.07,
              const double aImpactDiscountRate = 0.,
              const unsigned int aNbYear=20,
              const unsigned int aNbYearInput=1,
              const unsigned int aLeapYearPos=0,
              const double aExtrapolationFactor=1.,
              QString aRange="PLAN"
              );
    virtual ~TecEcoEnv();

    virtual void computeTecEcoEnvAnalysis() ;

    int NbYear() { return mNbYear ; }
    int NbYearOffset() { return mNbYearOffset ; }
    int NbYearInput() { return mNbYearInput ; }
    int LeapYearPos() { return mLeapYearPos ; }
    double DiscountRate() { return mDiscountRate ; }
    double ImpactDiscountRate() { return mImpactDiscountRate; }
    double ExtrapolationFactor() {return mExtrapolationFactor;}
    double InternalRateOfReturn() { return mInternalRateOfReturn ; }
    double RateOfReturnDiscountFactor() { return mRateOfReturnDiscountFactor; }
    QStringList EnvImpactsList() { return mEnvImpactsList; }
    QStringList EnvImpactsShortNamesList() { return mEnvImpactsShortNamesList; }
    QStringList EnvImpactUnitsList() { return mEnvImpactUnitsList; }
    std::vector<double> EnvImpactCosts() { return mEnvImpactCosts; }
    QString Range() { return mRange ; }
    QString Currency() { return mCurrency ; }
    QString ObjectiveUnit() { return mObjectiveUnit; }

    double ProductionContribution () const { return mProductionContribution ; }          /** Planned Production to be accounted for unit cost computation */
    double HistProductionContribution ()  const { return mHistProductionContribution ; }       /** Historical Production to be accounted for unit cost computation */

    void setCurrency (const QString aCurrency) ;
    void setRange (const QString aRange) ;
    void setNbYear (const int aNbYear) ;
    void setNbYearOffset (const int aNbYearOffset);
    void setNbYearInput (const int aNbYearInput) ;
    void setLeapYearPos (const int aLeapYearPos) ;
    void setDiscountRate (const double aDiscountRate) ;
    void setImpactDiscountRate(const double aImpactDiscountRate);
    void setInternalRateOfReturn (const QString aInternalRateOfReturn) ;
    void setInternalRateOfReturn (const double aInternalRateOfReturn) ;
    void setEnvImpactsList(const QStringList aEnvImpactsList);
    void setEnvImpactsShortNamesList(const QStringList aEnvImpactsShortNamesList);
    void setEnvImpactUnitsList(const QStringList aEnvImpactUnitsList);
    void setEnvImpactCosts(const std::vector<double> aEnvImpactCosts);

    void setHistNbHours(const double &aHistTime) {mHistNbHours += aHistTime;};
    double HistNbHours() { return mHistNbHours; }

    void setLevelizationTable();
    QVector<double> LevelizationTable() {return mLevelizationTable;}
    void setImpactLevelizationTable();
    QVector<double> ImpactLevelizationTable() { return mImpactLevelizationTable; }
    void setTableYearsHours();
    QVector<double> TableYearsHours() { return mTableYearsHours; }
    void setExtrapolationFactor(const double aExtrapolationFactor);
    void setOptimalSize(double aOptimalSize) { mOptimalSize = aOptimalSize ;}

protected:
    GUIData* mGUIData{ nullptr };     /** Pointer to GUI Data */

    QString mCurrency ; /** Currency unit - default to EUR */
    QString mObjectiveUnit;
    QString mRange ;                 /** Evaluation range used for TecEco analysis : HIST = past operation, PLAN = planned operation */
    int mNbYear ;               /** Number of year for economic data extrapolation */
    int mNbYearOffset ;         /** Offset of nb of year for discount cost computation */
    int mNbYearInput ;          /** Number of years in the input time series */
    int mLeapYearPos ;          /** Position of the leap in the input time series if there is one (0 if not, 1 if it is the first year, ...) */
    double mDiscountRate ;               /** Discount Rate */
    double mImpactDiscountRate;               /** Discount Rate for env impacts */
    double mExtrapolationFactor; //Should only be in OptimProblem!
    double mInternalRateOfReturn ;          /** Target Internal Rate of Return chosen by the user*/
    double mRateOfReturnDiscountFactor ;           /** Average Rate of Return */

    QStringList mEnvImpactsList;                /** List of the environmental impacts the user wants to consider */
    QStringList mEnvImpactsShortNamesList{};                /** List of the environmental impacts short names the user wants to consider */
    QStringList mEnvImpactUnitsList;                /** List of the units of the environmental impacts the user wants to consider */
    std::vector<double> mEnvImpactCosts;                   /** Costs of environmental impacts, in CURRENCY / kg */

    double mOptimalSize{ 0 };                      /** Resulting optimal size of component, if optimized - One per component - eg Nominal power, storage capacity...*/

    double mProductionContribution ;          /** Planned Production to be accounted for unit cost computation */
    double mHistProductionContribution ;      /** Historical Production to be accounted for unit cost computation */

    double mBusEnergyBalance ;                 /** Resulting cumulated energy balance */
    //Storage analysis over time horizon (no projection)
    double mChargingTime ;                      /** Resulting Charging time of storage */
    double mDischargingTime ;                   /** Resulting Discharging time of storage */
    double mChargedEnergy ;                     /** Resulting Cumulated charging energy of storage */
    double mDischargedEnergy ;                  /** Resulting Cumulated discharging energy of storage */
    double mNLevChargedEnergy ;                     /** Non Levelized Resulting Cumulated charging energy of storage */
    double mNLevDischargedEnergy ;                  /** Non Levelized Resulting Cumulated discharging energy of storage */
    //Converter analysis over time horizon (no projection)
    double mRunnningTime ;                      /** Resulting Running time of converter */
    double mConsumption ;                       /** Resulting Cumulated generated energy from conversion */
    double mProduction ;                        /** Resulting Cumulated used energy during conversion */
    double mNLevConsumption ;                    /** Non Levelized Resulting Cumulated generated energy from conversion */
    double mNLevProduction ;                     /** Non Levelized Resulting Cumulated used energy during conversion */
    QMap <QString, double> mConsumptionMap ;                       /** Resulting Cumulated levelized generated energy from conversion */
    QMap <QString, double> mProductionMap ;                        /** Resulting Cumulated levelized used energy during conversion */
    QMap <QString, double> mNLevConsumptionMap ;                    /** Non Levelized Resulting Cumulated generated energy from conversion */
    QMap <QString, double> mNLevProductionMap ;                     /** Non Levelized Resulting Cumulated used energy during conversion */
    //Contractual Load/Grid analysis over time horizon (no projection)
    double mContractualEnergy;                  /** Resulting Cumulated contractual injected/extracted energy or mass */
    double mNLevContractualEnergy;              /** Non Levelized Resulting Cumulated contractual injected/extracted energy or mass */
    //Contractual Load/Grid analysis over time horizon (no projection)
    double mFreeEnergy;                         /** Resulting Cumulated free injected/extracted energy or mass */
    double mNLevFreeEnergy;                     /** Non Levelized Resulting Cumulated free injected/extracted energy or mass */

    double mHistBusEnergyBalance ;                 /** History Cumulated Resulting  energy balance */
    //Storage analysis over time horizon (no projection)
    double mHistChargingTime ;                      /** History Cumulated Resulting Charging time of storage */
    double mHistDischargingTime ;                   /** History Cumulated Resulting Discharging time of storage */
    double mHistChargedEnergy ;                     /** History Cumulated Resulting charging energy of storage */
    double mHistDischargedEnergy ;                  /** History Cumulated Resulting discharging energy of storage */
    double mHistNLevChargedEnergy ;                 /** Non Levelized History Cumulated Resulting charging energy of storage */
    double mHistNLevDischargedEnergy ;              /** Non Levelized History Cumulated Resulting discharging energy of storage */
    //Converter analysis over time horizon (no projection)
    double mHistRunningTime ;                      /** History Cumulated Resulting Running time of converter */
    double mHistConsumption ;                    /** History Cumulated Resulting generated energy from conversion */
    double mHistProduction ;                        /** History Cumulated Resulting used energy during conversion */
    double mHistNLevConsumption ;                    /** History Cumulated Resulting generated energy from conversion */
    double mHistNLevProduction ;                        /** History Cumulated Resulting used energy during conversion */
    QMap <QString, double> mHistConsumptionMap ;                       /** Resulting History Cumulated generated energy from conversion */
    QMap <QString, double> mHistProductionMap ;                        /** Resulting History Cumulated used energy during conversion */
    QMap <QString, double> mHistNLevConsumptionMap ;                    /** Non Levelized Resulting History Cumulated generated energy from conversion */
    QMap <QString, double> mHistNLevProductionMap ;                     /** Non Levelized Resulting History Cumulated used energy during conversion */
    //Contractual Load/Grid analysis over time horizon (no projection)
    double mHistContractualEnergy;                  /** History Cumulated Resulting contractual injected/extracted energy or mass */
    double mHistContractualEnergyOptimal ;
    double mHistNLevContractualEnergy;              /** Non Levelized  History Cumulated Resulting contractual injected/extracted energy or mass */
    //Contractual Load/Grid analysis over time horizon (no projection)
    double mHistFreeEnergy;                         /** History Cumulated Resulting free injected/extracted energy or mass */
    double mHistNLevFreeEnergy;                     /** Non Levelized History Cumulated Resulting free injected/extracted energy or mass */
    QVector<double> mLevelizationTable;                /** tableau de levelization factor par année calculée */
    QVector<double> mImpactLevelizationTable;          /** tableau de levelization factor par année calculée pour env impact*/
    QVector<double> mTableYearsHours;               /** tableau des nombre de pas de temps par année cumulés */
    int mHistNbHours; /** nombre d'heures déjà décompté par le passé (permet de faire la bonne actualisation) */
};

#endif // TecEcoEnv_H
