#ifndef GRIDCOMPO_H
#define GRIDCOMPO_H

#include <string>

#include "MilpComponent.h"
#include "GridSubModel.h"
#include "CairnCore_global.h"
#include "ModelFactory.h"

/**
 * @brief Models a grid connection for free energy injection or extraction.
 *
 * - Sens > 0 : energy is extracted from the grid (buy price applies).
 * - Sens < 0 : energy is injected into the grid  (sell price applies).
 *
 * Energy fluxes can be fluid (kg/h) or electrical/thermal (MW).
 *
 * @note MILP weight optimisation on this component is non-linear and should
 *       not be used; the weight parameter may still be set externally.
 *
 */
class CAIRNCORESHARED_EXPORT GridCompo : public MilpComponent
{
public:
    GridCompo(CairnObject* aParent,
              const std::string& aName,
              const t_mapParamData& aComponent,
              const std::map<std::string, t_mapParamData>& aPorts,
              MilpData* aMilpData,
              TecEcoAnalysis* aTecEcoAnalysis,
              ModelFactory* aModelFactory);

    ~GridCompo();

    // MilpComponent overrides
    void readTSVariablesFromModel() override;
    void createImportListVars(t_mapExchange& a_Import) override;
    int  setTimeSeriesValues() override;
    virtual void setDefaultsResults() {};
    virtual void exportRHVariableInModel() {};

protected:
    std::string mEnergyPriceProfileName;          /* Grid energy price profile name */
    std::string mEnergyPriceProfileNameSeasonal;  /* Grid energy seasonal price profile name */

private:
    // ---------- helpers to reduce duplication between the two public methods --

    /**
     * @brief Initialise buy-price time-series defaults (extraction mode, Sens > 0).
     * @param aCarrier: Main energy carrier of this component.
     * @return 0 on success, -1 on configuration error.
     */
    int  initBuyPriceDefaults(EnergyVector* aCarrier);

    /**
     * @brief Initialise sell-price time-series defaults (injection mode, Sens < 0).
     * @param aCarrier: Main energy carrier of this component.
     */
    void initSellPriceDefaults(EnergyVector* aCarrier);

    /**
     * @brief Resolve buy-price profile names from model / carrier (extraction mode).
     * @param aCarrier: Main energy carrier of this component.
     */
    void resolveBuyPriceProfiles(EnergyVector* aCarrier);

    /**
     * @brief Resolve sell-price profile name from model / carrier (injection mode).
     * @param aCarrier:Main energy carrier of this component.
     */
    void resolveSellPriceProfile(EnergyVector* aCarrier);
};

#endif // GRIDCOMPO_H
