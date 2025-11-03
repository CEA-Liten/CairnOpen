#include "GridCompo.h"
#include <math.h>       /* fabs, log, pow */
#include <iostream>
#include "GlobalSettings.h"

using namespace GS ;

using Eigen::Map;

GridCompo::GridCompo (CairnObject *aParent,
    const std::map<std::string, std::string>& aComponent,
    const std::map < std::string, std::map<std::string, std::string> >& aPorts,
    MilpData* aMilpData,
    TecEcoEnv& aTecEcoEnv,
    ModelFactory* aModelFactory) :
    MilpComponent(aParent, CairnUtils::getParam(aComponent,"id"), aMilpData, aTecEcoEnv, aComponent, aPorts, aModelFactory)
{      
}

//------------------------------------------------------------------------------
GridCompo::~GridCompo()
{
} 

void GridCompo::declareCompoInputParam()
{
    MilpComponent::declareCompoInputParam();
}

void GridCompo::setCompoInputParam(const std::map<std::string, std::string> aComponent) {
    MilpComponent::setCompoInputParam(aComponent);
}

int GridCompo::setParameters()
{
    EnergyVector* lvect = mCompoModel->getMainCarrier();

    if (mCompoModel->Sens() > 0) {
        //sens = " extracted " ;
        if (mEnergyPriceProfileName != "" )
        {
            if (lvect->BuyPrice() != 0.)
            {
                cWarning()  << "Grid flat buy price ignored as UseProfileBuyPrice was specified " << lvect->Name() ;
            }
        }
        else
        {
            m_timeSeries["UseProfileBuyPrice"].setDefault(lvect->BuyPrice());
            cInfo() << "INFO : Grid Injection / extraction : use of constant BuyPrice " << lvect->Name() << lvect->BuyPrice() ;
        }
        if (mEnergyPriceProfileNameSeasonal != "" )
        {
            if (lvect->BuyPriceSeasonal() != 0.)
            {
                cCritical() << "ERROR : Grid flat buy price should be 0 as UseProfileBuyPriceSeasonal was specified " << lvect->Name();
                return -1 ;
            }
        }
        else
        {
            m_timeSeries["UseProfileBuyPriceSeasonal"].setDefault(lvect->BuyPriceSeasonal());
            cInfo() << "INFO : Grid Injection / extraction : use of constant BuyPriceSeasonal " << lvect->Name() << lvect->BuyPriceSeasonal() ;
        }
    }
    else
    {
        //sens = " injected " ;
        if (mEnergyPriceProfileName != "")
        {
            if (lvect->SellPrice() != 0.)
            {
                cWarning() << "Grid flat sell price ignored as UseProfileSellPrice was specified " << lvect->Name();
            }
        }
        else
        {
            m_timeSeries["UseProfileSellPrice"].setDefault(lvect->SellPrice());
            cInfo() << "INFO : Grid Injection / extraction : use of constant SellPrice " << lvect->Name() << lvect->SellPrice() ;
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
    EnergyVector* lvect=lptrport->getCarrier();
   
    if (mCompoModel->Sens() > 0) {
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
    if (mCompoModel->Sens() > 0) {
        m_timeSeries["UseProfileBuyPrice"].set_Values(npdt(), 0.);
        m_timeSeries["UseProfileBuyPriceSeasonal"].set_Values(npdt(), 0.);
    }
    else {
        m_timeSeries["UseProfileSellPrice"].set_Values(npdt(), 0.);
    }
}
//------------------------------------------------------------------------------

