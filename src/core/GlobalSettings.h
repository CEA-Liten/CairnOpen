#ifndef GLOBALSETTINGS_H
#define GLOBALSETTINGS_H

#if defined(WIN32) || defined(_WIN32)
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <Windows.h>
#endif

#include "CairnVersion.h"
#include "Cairn_Exception.h"
#include "CairnUtils.h"

#include <Eigen/SparseCore>
#include <Eigen/Dense>

using Eigen::Map;

// For mapping a vector subscribed in ZE to a VectorXf
#define ZE_IN(x) Map<VectorXf>(x.data(), x.size())
#define ZE_OUT(x,y) Map<VectorXf> y(x.data(), x.size());
#define ZEP_OUT(x,y) Map<VectorXf> y(x->data(), x->size());

namespace GS
{
    static std::string Cairn_Release(CAIRN_VERSION) ;
    static uint IDCount ;
    static uint iVerbose ;

    static inline const std::string GAMS() {return "GAMS";}              //keyword for GAMS modeler type
    static inline const std::string MIPMODELER() {return "MIPModeler";}  //keyword for MIPModeler modeler type
    static inline const std::string KDATA() {return "DATAEXCHANGE";}  //keyword for DATAEXCHANGE port type
    static inline const std::string KPROD() {return "OUTPUT";}  //keyword for PRODuced matter
    static inline const std::string KCONS() {return "INPUT";}   //keyword for CONSumed matter
    static inline const std::string Left() { return "left"; }  
    static inline const std::string Right() { return "right"; }   
    static inline const std::string Bottom() { return "bottom"; }    
    static inline const std::string Top() { return "top"; }
    static inline const std::string Yes() { return "Yes"; }
    static inline const std::string No() { return "No"; }

    static inline const std::string ANY_TYPE() { return "ANY_TYPE"; }
    static inline const std::string ANY_Fluid() { return "ANY_Fluid"; }
    static inline const std::string Fluid() { return "Fluid"; }
    static inline const std::string FluidH2() { return "FluidH2"; }
    static inline const std::string FluidCH4() { return "FluidCH4"; }
    static inline const std::string Thermal() { return "Thermal"; }
    static inline const std::string Electrical() { return "Electrical"; }
    static inline const std::string ThermalOrElectrical() { return "ThermalOrElectrical"; }
    static inline const std::string Material() { return "Material"; }

    static inline const std::string ProfileLHV() { return"ProfileLHV"; }
    static inline const std::string ProfileGHV() { return"ProfileGHV"; }

    inline Eigen::VectorXf uAverageQVector2Vecxf(std::vector<double> vFineIn, double aTimeStepIn, std::vector<double> aTimeStepsOut, uint aNpdtPast)
    {
      const uint64_t aSizeFine = vFineIn.size() ; //pastSize+futurSize
      const uint64_t aSizeCoarse = aTimeStepsOut.size() + aNpdtPast ; //pastSize+ComputationfuturSize

      std::vector<double> TimeStepsIn(aSizeFine);
      std::vector<double> localIn(aSizeFine);
      double tmpFine=0.;
      double tmpCoarse=0.;

      if (aSizeFine < aSizeCoarse)
      {
          cCritical() << "aSizeFine = " << aSizeFine ;
          cCritical() << "aSizeCoarse = " << aSizeCoarse ;
          cCritical() << "ANOMALIE ! " << aTimeStepIn << aNpdtPast ;
      }

      Eigen::VectorXf vCoarseOut (Eigen::VectorXf::Constant(aSizeCoarse,0.)) ;
      if (aSizeFine == 0)
      {
          cCritical() << "Abnormal missing allocation aSizeFine = " << aSizeFine ;
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
              //cDebug() << "vFineIn ifine " << ifine << aSizeFine << vFineIn[ifine];
          }
          if (dt >= aTimeStepsOut[icoarse-aNpdtPast] )
          {
              vCoarseOut[icoarse] = vCoarseOut[icoarse] / dt ;
              //cDebug() << "vCoarseOut icoarse " << icoarse << aSizeCoarse << vCoarseOut[icoarse];
              tmpCoarse += aTimeStepsOut[icoarse-aNpdtPast] ;
              dt = 0. ;
              icoarse++ ;
          }
      }
      if (tmpFine != tmpCoarse || icoarse != aSizeCoarse)
      {
          cCritical() << "Bad timesteps definition : the sum of coarse timesteps "<< tmpCoarse <<" must be equal to the sum of constant fine timestep " << tmpFine ;
          cCritical() << "Bad timesteps definition : icoarse "<< icoarse <<" must be equal to aSizeCoarse " << aSizeCoarse ;

          vCoarseOut[aSizeCoarse-1] = vCoarseOut[aSizeCoarse-1] / aTimeStepsOut[aTimeStepsOut.size()-1] ;
          cCritical() << "Final value of coarse timeseries will be biased : " << aSizeCoarse << vCoarseOut[aSizeCoarse-1] ;
      }
      return vCoarseOut ;
    }

    inline void uExpandVecxf2QVector(std::vector<double>* vFineOut, const uint64_t aSizeFineOut,
                                     Eigen::VectorXf vCoarseIn, double aTimeStepOut,
                                     std::vector<double> aTimeStepsIn,
                                     uint aNpdtPast, double aCoeff)
    {
      const uint64_t aSizeFine = aSizeFineOut ; //pastSize+futurSize
      const uint64_t aSizeCoarse = vCoarseIn.size() ; //pastSize+computationFuturSize

      assert ( aSizeFine >= aSizeCoarse );

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
              //cDebug() << "vFineOut ifine " << ifine << vCoarseIn[icoarse];
          }
          if (dt >= aTimeStepsIn[icoarse-aNpdtPast])
          {
              dt = 0. ;
              icoarse++ ;
          }
      }
    }

    inline std::vector<std::vector<double>> getDataMatrix(const std::vector<std::vector<std::string>> &data_Inputs, int iskipHead)
    {
        /**
         * @brief Cette fonction renvoie une matrice de nombres en virgule flottante ? partir d'une liste de cha?nes de caract?res,
         * en ignorant un certain nombre de lignes au d?but.
         *
         * @param data_Inputs : une liste de cha?nes de caract?res (std::vector<std::vector<std::string>>) o? chaque ?l?ment de la liste est une
         * autre liste de cha?nes de caract?res, repr?sentant une ligne dans un fichier de donn?es.
         *
         * @param iskipHead : un entier (int) repr?sentant le nombre de lignes ? ignorer au d?but du fichier de donn?es.
         *
         * @return std::vector<std::vector<double>> : une matrice de nombres en virgule flottante (std::vector<std::vector<double>>)
         * contenant les donn?es de toutes les colonnes et de toutes les lignes (? l'exception des iskipHead premi?res lignes).
         * Si une cha?ne vide est rencontr?e dans une ligne, la lecture de cette ligne est arr?t?e et la ligne suivante est trait?e.
         * Si aucune donn?e n'est trouv?e, la fonction renvoie une matrice vide.
         */
        std::vector<std::vector<double>> lu;
        std::string value;

        for (int i = iskipHead; i < data_Inputs.size(); ++i)
        {
            std::vector<double> row;

            for (int j = 0; j < data_Inputs[i].size(); ++j)
            {
                value = data_Inputs[i][j];

                if (value == "")
                {
                    //cDebug() << " End of vector found ";
                    break;
                }
                else
                {
                    //cDebug() << "Read value " << i << value;
                    row.push_back(std::stod(value));
                }
            }

            if (!row.empty())
            {
                lu.push_back(row);
            }
        }

        //cDebug() << "Return matrix of size " << lu.size() << "x" << lu[0].size();
        return lu;
    }

    inline std::vector<double> getDataArray(const std::vector<std::vector<std::string>> &data_Inputs, int aCol, int iskipHead)
    {
        std::vector<double> lu ;
        std::string value ;

        for ( int i = iskipHead; i < data_Inputs.size(); ++i )
        {
            value=(data_Inputs.at(i)).at(aCol);

            if (value == "")
            {
                //cDebug() << " End of vector found " ;
                break ;
            }
            else
            {
                //cDebug() << "Read value " << i << value;
                lu.push_back(std::stod(value));
            }

        }

        //cDebug() << "Return vector of size " << lu.size() ;
        return lu ;
    }



    inline std::vector<int> getIntDataArray(const std::vector<std::vector<std::string>> &data_Inputs, int aCol, int iskipHead)
    {
        std::vector<int> lu ;
        std::string value ;

        for ( int i = iskipHead; i < data_Inputs.size(); ++i )
        {
            value=(data_Inputs.at(i)).at(aCol);

            if (value == "")
            {
                //cDebug() << " End of vector found " ;
                break ;
            }
            else
            {
                //cDebug() << "Read value " << i << value;
                lu.push_back(std::stoi(value));
            }

        }

        //cDebug() << "Return vector of size " << lu.size() ;
        return lu ;
    }


    inline std::vector <double> uVecxf2Double(Eigen::VectorXf vIn, uint aSizeOut, const uint aOffset)
    {
      std::vector <double> vOut (aSizeOut) ;
      assert(aSizeOut+aOffset == vIn.size()) ;
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


   

    static inline uint GenerateID()
    {
        IDCount += 100 ;
        return IDCount;
    }

}
#endif // GLOBALSETTINGS_H
