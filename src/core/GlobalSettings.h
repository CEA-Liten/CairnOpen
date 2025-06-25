#ifndef GLOBALSETTINGS_H
#define GLOBALSETTINGS_H

#if defined(WIN32) || defined(_WIN32)
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <Windows.h>
#endif

#include <QtCore>
#include <QtCore/qglobal.h>
#include <QSettings>
#include <QDebug>
#include <QFile>
#include <QVector>
#include <QDir>

#include "CairnVersion.h"
#include "Cairn_Exception.h"

#include <Eigen/SparseCore>
#include <Eigen/Dense>

using Eigen::Map;

// For mapping a vector subscribed in ZE to a VectorXf
#define ZE_IN(x) Map<VectorXf>(x.data(), x.size())
#define ZE_OUT(x,y) Map<VectorXf> y(x.data(), x.size());
#define ZEP_OUT(x,y) Map<VectorXf> y(x->data(), x->size());

namespace GS
{
    static QString Cairn_Release(PERSEE_VERSION) ;
    static uint IDCount ;
    static uint iVerbose ;

    static inline const QString GAMS() {return "GAMS";}              //keyword for GAMS modeler type
    static inline const QString MIPMODELER() {return "MIPModeler";}  //keyword for MIPModeler modeler type
    static inline const QString KDATA() {return "DATAEXCHANGE";}  //keyword for DATAEXCHANGE port type
    static inline const QString KPROD() {return "OUTPUT";}  //keyword for PRODuced matter
    static inline const QString KCONS() {return "INPUT";}   //keyword for CONSumed matter
    static inline const QString Left() { return "left"; }  
    static inline const QString Right() { return "right"; }   
    static inline const QString Bottom() { return "bottom"; }    
    static inline const QString Top() { return "top"; }
    static inline const QString Yes() { return "Yes"; }
    static inline const QString No() { return "No"; }

    static inline const QString ANY_TYPE() { return "ANY_TYPE"; }
    static inline const QString ANY_Fluid() { return "ANY_Fluid"; }
    static inline const QString Fluid() { return "Fluid"; }
    static inline const QString FluidH2() { return "FluidH2"; }
    static inline const QString FluidCH4() { return "FluidCH4"; }
    static inline const QString Thermal() { return "Thermal"; }
    static inline const QString Electrical() { return "Electrical"; }
    static inline const QString ThermalOrElectrical() { return "ThermalOrElectrical"; }
    static inline const QString Material() { return "Material"; }

    inline Eigen::VectorXf uAverageQVector2Vecxf(QVector<float> vFineIn, double aTimeStepIn, std::vector<double> aTimeStepsOut, uint aNpdtPast)
    {
      const uint64_t aSizeFine = vFineIn.size() ; //pastSize+futurSize
      const uint64_t aSizeCoarse = aTimeStepsOut.size() + aNpdtPast ; //pastSize+ComputationfuturSize

      std::vector<double> TimeStepsIn(aSizeFine);
      std::vector<double> localIn(aSizeFine);
      double tmpFine=0.;
      double tmpCoarse=0.;

      if (aSizeFine < aSizeCoarse)
      {
          qCritical() << "aSizeFine = " << aSizeFine ;
          qCritical() << "aSizeCoarse = " << aSizeCoarse ;
          qCritical() << "ANOMALIE ! " << aTimeStepIn << aNpdtPast ;
      }

      Eigen::VectorXf vCoarseOut (Eigen::VectorXf::Constant(aSizeCoarse,0.)) ;
      if (aSizeFine == 0)
      {
          qCritical() << "Abnormal missing allocation aSizeFine = " << aSizeFine ;
          return vCoarseOut ;
      }
      TimeStepsIn.assign(aSizeFine,aTimeStepIn) ;

      uint64_t icoarse = 0 ;
      double dt = 0. ;

      //initialize past.
      for (uint64_t ifine=0; ifine<aNpdtPast; ifine++)
      {
              vCoarseOut[icoarse] += vFineIn[ifine] ;
              localIn[ifine] =  vFineIn[ifine] ;
              icoarse++ ;
      }
      for (uint64_t ifine=aNpdtPast; ifine<aSizeFine; ifine++)
      {
          if (dt <= aTimeStepsOut[icoarse-aNpdtPast])  // dt est la periode de moyenne, composee d'un nombre entier de pas de temps
          {
              dt += TimeStepsIn[ifine] ;
              vCoarseOut[icoarse] += vFineIn[ifine] * TimeStepsIn[ifine] ;
              localIn[ifine] =  vFineIn[ifine] ;
              tmpFine += TimeStepsIn[ifine] ;
              //qDebug() << "vFineIn ifine " << ifine << aSizeFine << vFineIn[ifine];
          }
          if (dt >= aTimeStepsOut[icoarse-aNpdtPast] )
          {
              vCoarseOut[icoarse] = vCoarseOut[icoarse] / dt ;
              //qDebug() << "vCoarseOut icoarse " << icoarse << aSizeCoarse << vCoarseOut[icoarse];
              tmpCoarse += aTimeStepsOut[icoarse-aNpdtPast] ;
              dt = 0. ;
              icoarse++ ;
          }
      }
      if (tmpFine != tmpCoarse || icoarse != aSizeCoarse)
      {
          qCritical() << "Bad timesteps definition : the sum of coarse timesteps "<< tmpCoarse <<" must be equal to the sum of constant fine timestep " << tmpFine ;
          qCritical() << "Bad timesteps definition : icoarse "<< icoarse <<" must be equal to aSizeCoarse " << aSizeCoarse ;

          vCoarseOut[aSizeCoarse-1] = vCoarseOut[aSizeCoarse-1] / aTimeStepsOut[aTimeStepsOut.size()-1] ;
          qCritical() << "Final value of coarse timeseries will be biased : " << aSizeCoarse << vCoarseOut[aSizeCoarse-1] ;
      }
      return vCoarseOut ;
    }

    inline void uExpandVecxf2QVector(QVector<float>* vFineOut, const uint64_t aSizeFineOut,
                                     Eigen::VectorXf vCoarseIn, double aTimeStepOut,
                                     std::vector<double> aTimeStepsIn,
                                     uint aNpdtPast, double aCoeff)
    {
      const uint64_t aSizeFine = aSizeFineOut ; //pastSize+futurSize
      const uint64_t aSizeCoarse = vCoarseIn.size() ; //pastSize+computationFuturSize

      Q_ASSERT ( aSizeFine >= aSizeCoarse );

      std::vector<double> localOut(aSizeFine);

      uint64_t icoarse = 0 ;
      double dt = 0. ;
      //publish Past, Present and Future.
      //past
      for (uint64_t ifine=0; ifine<aNpdtPast; ifine++)
      {
              (*vFineOut)[ifine] = aCoeff * vCoarseIn[icoarse] ;
              localOut[ifine] = (*vFineOut)[ifine] ;
              icoarse++ ;
      }
      icoarse = aNpdtPast ;
      for (uint64_t ifine=aNpdtPast; ifine<aSizeFine; ifine++)
      {
          if (dt <= aTimeStepsIn[icoarse-aNpdtPast])  // dt est la periode de moyenne, composee d'un nombre entier de pas de temps
          {
              (*vFineOut)[ifine] = aCoeff * vCoarseIn[icoarse] ;
              localOut[ifine] = (*vFineOut)[ifine] ;
              dt +=aTimeStepOut ;
              //qDebug() << "vFineOut ifine " << ifine << vCoarseIn[icoarse];
          }
          if (dt >= aTimeStepsIn[icoarse-aNpdtPast])
          {
              dt = 0. ;
              icoarse++ ;
          }
      }
    }
    inline QVariant getVariantSetting (const QString aString, const QSettings &aSettings)
    {
        QString qstringName = (aString) ;

        //qDebug() << "getVariantSetting reading:" << qstringName << aSettings.value(qstringName) ;

           return aSettings.value(qstringName) ;

        }

    inline std::vector<std::vector<double>> getDataMatrix(QList<QStringList> data_Inputs, int iskipHead)
    {
        /**
         * @brief Cette fonction renvoie une matrice de nombres en virgule flottante à partir d'une liste de chaînes de caractères,
         * en ignorant un certain nombre de lignes au début.
         *
         * @param data_Inputs : une liste de chaînes de caractères (QList<QStringList>) où chaque élément de la liste est une
         * autre liste de chaînes de caractères, représentant une ligne dans un fichier de données.
         *
         * @param iskipHead : un entier (int) représentant le nombre de lignes à ignorer au début du fichier de données.
         *
         * @return std::vector<std::vector<double>> : une matrice de nombres en virgule flottante (std::vector<std::vector<double>>)
         * contenant les données de toutes les colonnes et de toutes les lignes (à l'exception des iskipHead premières lignes).
         * Si une chaîne vide est rencontrée dans une ligne, la lecture de cette ligne est arrêtée et la ligne suivante est traitée.
         * Si aucune donnée n'est trouvée, la fonction renvoie une matrice vide.
         */
        std::vector<std::vector<double>> lu;
        QString value;

        for (int i = iskipHead; i < data_Inputs.size(); ++i)
        {
            std::vector<double> row;

            for (int j = 0; j < data_Inputs[i].size(); ++j)
            {
                value = data_Inputs[i][j];

                if (value == "")
                {
                    //qDebug() << " End of vector found ";
                    break;
                }
                else
                {
                    //qDebug() << "Read value " << i << value;
                    row.push_back(value.toDouble());
                }
            }

            if (!row.empty())
            {
                lu.push_back(row);
            }
        }

        //qDebug() << "Return matrix of size " << lu.size() << "x" << lu[0].size();
        return lu;
    }

    inline std::vector<double> getDataArray(QList<QStringList> data_Inputs, int aCol, int iskipHead)
    {
        std::vector<double> lu ;
        QString value ;

        for ( int i = iskipHead; i < data_Inputs.size(); ++i )
        {
            value=(data_Inputs.at(i)).at(aCol);

            if (value == "")
            {
                //qDebug() << " End of vector found " ;
                break ;
            }
            else
            {
                //qDebug() << "Read value " << i << value;
                lu.push_back(value.toDouble());
            }

        }

        //qDebug() << "Return vector of size " << lu.size() ;
        return lu ;
    }



    inline std::vector<int> getIntDataArray(QList<QStringList> data_Inputs, int aCol, int iskipHead)
    {
        std::vector<int> lu ;
        QString value ;

        for ( int i = iskipHead; i < data_Inputs.size(); ++i )
        {
            value=(data_Inputs.at(i)).at(aCol);

            if (value == "")
            {
                //qDebug() << " End of vector found " ;
                break ;
            }
            else
            {
                //qDebug() << "Read value " << i << value;
                lu.push_back(value.toInt());
            }

        }

        //qDebug() << "Return vector of size " << lu.size() ;
        return lu ;
    }

    inline QVector<double> getQVArray(QList<QStringList> data_Inputs, int aCol, int iskipHead)
    {
        QVector<double> lu;
        QString value;

        for (int i = iskipHead; i < data_Inputs.size(); ++i)
        {
            value = (data_Inputs.at(i)).at(aCol);

            if (value == "")
            {
                //qDebug() << " End of vector found " ;
                break;
            }
            else if(std::isnan(value.toDouble())) {
                qDebug() << "A NAN value is found at row" << i+1 << ", column" << aCol << "while reading the input time series.";
                break;
            }
            else
            {
                //qDebug() << "Read value " << i << value;
                lu.push_back(value.toDouble());
            }

        }

        //qDebug() << "Return vector of size " << lu.size() ;
        return lu;
    }
    static inline bool NotExist(QString filename)
    {
        //check that file exists and is readable
        QFile FileIn (filename);
        if (!FileIn.open(QIODevice::ReadOnly | QIODevice::Text)) {
                FileIn.close();
                return true ;
         }
        //close first and read using Qtcsv (Qtcsv will open it by itself)
        FileIn.close();
        return false ;
    }
    static inline QString BuildFileName (QString aFileName)
    {
      QString filename  = (aFileName) ;
      if ( !filename.contains(":/") && !filename.contains(":\\") && !filename.startsWith("//") )
      {
            filename=QDir::currentPath()+"/"+filename ;
      }
      return filename;
    }

    inline QList<QStringList> readToList(QString Full_File_Name, QString Separator)
    {
        QList<QStringList> data_input ;
        QStringList fields ;
        QFile File (Full_File_Name) ;
        QTextStream FileStream_In (&File);

        if(!File.open(QIODevice::ReadOnly))
        {
            //qInfo() << "Error CSV File could not be opened for reading " ;
            //return {{"ERROR"}};
            Cairn_Exception error("Error CSV File could not be opened for reading: " + Full_File_Name, -1);
            throw error;
        }

        int k = 1;
        while ( ! FileStream_In.atEnd())
        {
            QString line = FileStream_In.readLine();
            fields = line.split(Separator);

            if ( fields.contains(QString("Error\n")) )
            {
              qInfo() << "Error reading line " << line ;
            }
            if (k > 4) {//data lines 
                //Verify if the used separator is correct and that comma is not used for decimals.
                for (int i = 0; i < fields.size(); i++) {
                    if (fields[i].contains(",") || fields[i].contains(";")) {
                        QString errorMessage = QString("Error while importing input time series: " + Full_File_Name + "\nPlease, verify that the correct separator(" + Separator + ") is used and that comma is not used for decimal values.");
                        if (i == 0) {
                            errorMessage += " Line: " + fields[0];
                        }
                        else {
                            errorMessage += " Value: " + fields[i];
                        }
                        Cairn_Exception error(errorMessage, -1);
                        throw error;
                    }
                }
            }
            data_input.append(fields) ;
            k++;
        }
        return data_input ;
    }


    inline QList<QStringList> readFromCsvFile(QString aFileName, QString sep)
    {
        QList<QStringList>  data_Inputs ;

        QString filename = BuildFileName(aFileName) ;

        if (NotExist(filename))
        {
                return data_Inputs;
        }
        else
        {
           qInfo() << " Reading csv file " << filename ;
        }
        data_Inputs = readToList(filename, ";");
        return data_Inputs ;
    }

    inline std::vector <double> uVecxf2Double(Eigen::VectorXf vIn, uint aSizeOut, const uint aOffset)
    {
      std::vector <double> vOut (aSizeOut) ;
      Q_ASSERT(aSizeOut+aOffset == vIn.size()) ;
      for (uint i=0; i<aSizeOut; i++) {
          vOut[i] = double(vIn[i+aOffset]) ;
      }
      return vOut ;
    }

#if defined(WIN32) || defined(_WIN32)
    inline LPWSTR ConvertToLPWSTR( const std::string &s )
    {
      LPWSTR ws = new wchar_t[s.size()+1]; // +1 for zero at the end
      copy( s.begin(), s.end(), ws );
      ws[s.size()] = 0; // zero at the end
      return ws;
    }
#else
    inline wchar_t* ConvertToLPWSTR( const std::string &s )
    {
      wchar_t* ws = new wchar_t[s.size()+1]; // +1 for zero at the end
      copy( s.begin(), s.end(), ws );
      ws[s.size()] = 0; // zero at the end
      return ws;
    }
#endif


    template <class T>
    inline void extendMap1WithMap2(QMap<QString, T*> &aListDest, QMap<QString, T*> aListSource)
    {
        QMapIterator<QString, T*> ivar(aListSource) ;
        while (ivar.hasNext())
        {
            ivar.next() ;
            aListDest[ivar.key()] = ivar.value() ;
        }
        return ;
    }

    static inline uint GenerateID()
    {
        IDCount += 100 ;
        return IDCount;
    }

}
#endif // GLOBALSETTINGS_H
