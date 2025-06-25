#include "GridCompo.h"
#include <QDebug>
#include <math.h>       /* fabs, log, pow */
#include <iostream>
#include "GlobalSettings.h"

using namespace GS ;

using Eigen::Map;

GridCompo::GridCompo (QObject *aParent,
    const QMap<QString, QString>& aComponent,
    const QMap < QString, QMap<QString, QString> >& aPorts,
    MilpData* aMilpData,
    TecEcoEnv& aTecEcoEnv,
    ModelFactory* aModelFactory) :
    MilpComponent(aParent, aComponent["id"], aMilpData, aTecEcoEnv, aComponent, aPorts, aModelFactory)
{      
}

//------------------------------------------------------------------------------
GridCompo::~GridCompo()
{
} 

void GridCompo::declareCompoInputParam()
{
    MilpComponent::declareCompoInputParam();
    mCompoInputParam->addParameter("Direction", &mDirection, "InjectToGrid", true, true, "Direction for gird injection or extraction - Equals to InjectToGrid or ExtractFromGrid");
}

void GridCompo::setCompoInputParam(const QMap<QString, QString> aComponent) {
    MilpComponent::setCompoInputParam(aComponent);

    if (mDirection == "InjectToGrid") {
        mSens = -1;
    }
    else if (mDirection == "ExtractFromGrid") {
        mSens = +1;
    }
    else
    {
        mSens = 0;
        qCritical() << "ERROR : Grid Direction /** ";
        Cairn_Exception erreur("Invald <Direction> attribute : InjectToGrid or ExtractFromGrid expected " + mName + " instead of " + mDirection + ".", -1);
        throw& erreur;
    }
}


int GridCompo::setParameters()
{
    if (fabs(mSens) < 1.e-20)
    {
        qCritical() << "ERROR :  GridCompo InjectToGrid or ExtractFromGrid is missing for component : " << (Name()) ;
        return -1 ;
    }

    // Prix elec
    MilpPort* lptrport = PortList().at(0) ;
    EnergyVector* lvect=lptrport->ptrEnergyVector();

    if (lvect == nullptr)
    {
        qCritical() << "ERROR : Grid first port must be of type BusFlowBalance and carry an energy vector " ;
        return -1 ;
    }

    

    if (mSens >0)  {
        //sens = " extracted " ;
        if (mEnergyPriceProfileName != "" )
        {
            if (lvect->BuyPrice() != 0.)
            {
                qWarning()  << "Grid flat buy price ignored as UseProfileBuyPrice was specified " << (lvect->Name()) ;
            }
        }
        else
        {
            m_timeSeries["UseProfileBuyPrice"].setDefault(lvect->BuyPrice());
            qInfo() << "INFO : Grid Injection / extraction : use of constant BuyPrice " << (lvect->Name()) << lvect->BuyPrice() ;
        }
        if (mEnergyPriceProfileNameSeasonal != "" )
        {
            if (lvect->BuyPriceSeasonal() != 0.)
            {
                qCritical() << "ERROR : Grid flat buy price should be 0 as UseProfileBuyPriceSeasonal was specified " << (lvect->Name()) ;
                return -1 ;
            }
        }
        else
        {
            m_timeSeries["UseProfileBuyPriceSeasonal"].setDefault(lvect->BuyPriceSeasonal());
            qInfo() << "INFO : Grid Injection / extraction : use of constant BuyPriceSeasonal " << (lvect->Name()) << lvect->BuyPriceSeasonal() ;
        }
    }
    else
    {
        //sens = " injected " ;
        if (mEnergyPriceProfileName != "")
        {
            if (lvect->SellPrice() != 0.)
            {
                qWarning() << "Grid flat sell price ignored as UseProfileSellPrice was specified " << (lvect->Name()) ;
            }
        }
        else
        {
            m_timeSeries["UseProfileSellPrice"].setDefault(lvect->SellPrice());
            qInfo() << "INFO : Grid Injection / extraction : use of constant SellPrice " << (lvect->Name()) << lvect->SellPrice() ;
        }
    }

    createHistFXLists();

    return 0 ;
}

void GridCompo::readTSVariablesFromModel()
{
    //Read time series
    MilpComponent::readTSVariablesFromModel();


    MilpPort* lptrport = PortList().at(0) ;
    EnergyVector* lvect=lptrport->ptrEnergyVector();
   
    if (mSens >0) {
        //sens = " extracted " ;
        mEnergyPriceProfileName = m_timeSeries["UseProfileBuyPrice"].getName();
        mEnergyPriceProfileNameSeasonal = m_timeSeries["UseProfileBuyPriceSeasonal"].getName();
        if (mEnergyPriceProfileName == "") {
            mEnergyPriceProfileName = lvect->UseProfileBuyPrice() ;
            m_timeSeries["UseProfileBuyPrice"].setName(lvect->UseProfileBuyPrice());
        }
	if (mEnergyPriceProfileNameSeasonal == "") {
            mEnergyPriceProfileNameSeasonal = lvect->UseProfileBuyPriceSeasonal() ;
            m_timeSeries["UseProfileBuyPriceSeasonal"].setName(lvect->UseProfileBuyPriceSeasonal());
        }		 
    }
    else  {
        //sens = " injected " ;
        mEnergyPriceProfileName = m_timeSeries["UseProfileSellPrice"].getName();
        if (mEnergyPriceProfileName == "") {
            mEnergyPriceProfileName = lvect->UseProfileSellPrice() ;
            m_timeSeries["UseProfileSellPrice"].setName(lvect->UseProfileSellPrice());
        }
    }
}

void GridCompo::setDefaultsResults()
{
    if (mSens > 0) {
        m_timeSeries["UseProfileBuyPrice"].set_Values(npdt(), 0.);
        m_timeSeries["UseProfileBuyPriceSeasonal"].set_Values(npdt(), 0.);
    }
    else {
        m_timeSeries["UseProfileSellPrice"].set_Values(npdt(), 0.);
    }
}
//------------------------------------------------------------------------------



int GridCompo::initProblem()
{
    //Always make this available to SubModel
     mCompoToModel->publishData("Direction", &mSens);
     return MilpComponent::initProblem();
}
