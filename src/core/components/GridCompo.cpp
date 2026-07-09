#include "GridCompo.h"
#include "GlobalSettings.h"

#include <iostream>

using namespace GS;

// =============================================================================
// Construction / destruction
// =============================================================================

GridCompo::GridCompo(CairnObject* aParent,
              const std::string& aName,
              const t_mapParamData& aComponent,
              const std::map<std::string, t_mapParamData>& aPorts,
              MilpData* aMilpData,
              TecEcoAnalysis* aTecEcoAnalysis,
              ModelFactory* aModelFactory)
    : MilpComponent(aParent, aName, aMilpData, aTecEcoAnalysis,
                    aComponent, aPorts, aModelFactory)
{}

GridCompo::~GridCompo() = default;

// =============================================================================
// Public overrides
// =============================================================================

void GridCompo::readTSVariablesFromModel()
{
    MilpComponent::readTSVariablesFromModel();

    EnergyVector* carrier = getMainCarrier();

    if (mCompoModel->Sens() > 0)  
        resolveBuyPriceProfiles(carrier);  // extraction 
    else 
        resolveSellPriceProfile(carrier);  // injection 
}

// -----------------------------------------------------------------------------

int GridCompo::setTimeSeriesValues()
{
    EnergyVector* carrier = mCompoModel->getMainCarrier();

    const int rc = (mCompoModel->Sens() > 0)
                       ? initBuyPriceDefaults(carrier)        // extraction  
                       : (initSellPriceDefaults(carrier), 0); // injection 

    if (rc != 0)
        return rc;

    MilpComponent::setTimeSeriesValues();

    return 0;
}

// -----------------------------------------------------------------------------

void GridCompo::createImportListVars(t_mapExchange& a_Import)
{
    MilpComponent::createImportListVars(a_Import);

    if (mCompoModel->Sens() > 0)
    {
        m_timeSeries["UseProfileBuyPrice"].set_Values(npdt(), 0.);
        m_timeSeries["UseProfileBuyPriceSeasonal"].set_Values(npdt(), 0.);
    }
    else
    {
        m_timeSeries["UseProfileSellPrice"].set_Values(npdt(), 0.);
    }
}

// =============================================================================
// Private helpers
// =============================================================================

int GridCompo::initBuyPriceDefaults(EnergyVector* aCarrier)
{
    // -- Buy price -------------------------------------------------------
    if (!mEnergyPriceProfileName.empty())
    {
        if (aCarrier->BuyPrice() != 0.)
            cInfo() << "Grid flat buy price ignored because UseProfileBuyPrice"
                          " was specified for carrier" << aCarrier->Name();
    }
    else
    {
        m_timeSeries["UseProfileBuyPrice"].setDefault(aCarrier->BuyPrice());
        cInfo() << "Grid extraction: using constant BuyPrice from carrier '"
                << aCarrier->Name() << "' = " << aCarrier->BuyPrice();
    }

    // -- Seasonal buy price ---------------------------------------------------
    if (!mEnergyPriceProfileNameSeasonal.empty())
    {
        if (aCarrier->BuyPriceSeasonal() != 0.)
        {
            cCritical() << "ERROR: Grid flat BuyPriceSeasonal must be 0 when"
                           " UseProfileBuyPriceSeasonal is specified for carrier"
                        << aCarrier->Name();
            return -1;
        }
    }
    else
    {
        m_timeSeries["UseProfileBuyPriceSeasonal"].setDefault(aCarrier->BuyPriceSeasonal());
        cInfo() << "Grid extraction: using constant BuyPriceSeasonal from carrier '"
                << aCarrier->Name() << "' = " << aCarrier->BuyPriceSeasonal();
    }

    return 0;
}

// -----------------------------------------------------------------------------

void GridCompo::initSellPriceDefaults(EnergyVector* aCarrier)
{
    if (!mEnergyPriceProfileName.empty())
    {
        if (aCarrier->SellPrice() != 0.)
            cInfo() << "Grid flat sell price ignored because UseProfileSellPrice"
                          " was specified for carrier" << aCarrier->Name();
    }
    else
    {
        m_timeSeries["UseProfileSellPrice"].setDefault(aCarrier->SellPrice());
        cInfo() << "Grid injection: using constant SellPrice from carrier '"
                << aCarrier->Name() << "' = " << aCarrier->SellPrice();
    }
}

// -----------------------------------------------------------------------------

void GridCompo::resolveBuyPriceProfiles(EnergyVector* aCarrier)
{
    // Regular buy price
    mEnergyPriceProfileName = m_timeSeries["UseProfileBuyPrice"].getName();
    if (mEnergyPriceProfileName.empty())
    {
        mEnergyPriceProfileName = aCarrier->UseProfileBuyPrice();
        m_timeSeries["UseProfileBuyPrice"].setName(mEnergyPriceProfileName);
    }

    // Seasonal buy price
    mEnergyPriceProfileNameSeasonal = m_timeSeries["UseProfileBuyPriceSeasonal"].getName();
    if (mEnergyPriceProfileNameSeasonal.empty())
    {
        mEnergyPriceProfileNameSeasonal = aCarrier->UseProfileBuyPriceSeasonal();
        m_timeSeries["UseProfileBuyPriceSeasonal"].setName(mEnergyPriceProfileNameSeasonal);
    }
}

// -----------------------------------------------------------------------------

void GridCompo::resolveSellPriceProfile(EnergyVector* aCarrier)
{
    mEnergyPriceProfileName = m_timeSeries["UseProfileSellPrice"].getName();
    if (mEnergyPriceProfileName.empty())
    {
        mEnergyPriceProfileName = aCarrier->UseProfileSellPrice();
        m_timeSeries["UseProfileSellPrice"].setName(mEnergyPriceProfileName);
    }
}
