/*
* \file		IndicatorUtils.cpp
* \brief	A method to dynamically generate an indicator name on demand
* \version	1.0
* \author	Ali KASSEM
* \date		18/12/2025
*/

#include "GridSubModel.h"
#include "SourceLoadSubModel.h"
#include "StorageSubModel.h"
#include "ConverterSubModel.h"

#include "MilpPort.h"
#include "CairnUtils.h"

using namespace CairnUtils;

std::string CAIRNCORESHARED_EXPORT indicatorName(SubModel* ap_Model, MilpPort* ap_Port, const std::vector<std::string>& a_NameParts)
{
    auto* grid = (ap_Model ? dynamic_cast<GridSubModel*>(ap_Model) : nullptr);
    auto* sourceLoad = (grid ? nullptr : (ap_Model ? dynamic_cast<SourceLoadSubModel*>(ap_Model) : nullptr));
    auto* storage    = (grid || sourceLoad ? nullptr : (ap_Model ? dynamic_cast<StorageSubModel*>(ap_Model) : nullptr));
    auto* converter  = (grid || sourceLoad || storage ? nullptr : (ap_Model ? dynamic_cast<ConverterSubModel*>(ap_Model) : nullptr));

    const std::string direction = [&]()->std::string {
        if (grid)        return grid->Direction();
        if (sourceLoad)  return sourceLoad->Direction();
        return {};
    }();

    auto* carrier = (ap_Port ? ap_Port->getCarrier() : nullptr);
    const bool isHeatCarrier = (carrier && carrier->isHeatCarrier());
    const std::string heatSuffix = isHeatCarrier && ( grid || storage || (converter && converter->ModelClassName() == "PowerToFluidT") )
        ? (" at " + CairnUtils::to_string_trim(carrier->Potential()) + " degC")
        : std::string{};

    const std::unordered_map<std::string, std::function<std::string()>> tokenMap{
        {VARIABLE,     [&] { return ap_Port ? ap_Port->Variable() : std::string{}; }},
        {STORAGE_NAME, [&] { return carrier ? carrier->StorageName() + (isHeatCarrier ? heatSuffix : std::string{}) : std::string{}; }},
        {FLUX_NAME,    [&] { return carrier ? carrier->FluxName() + (isHeatCarrier ? heatSuffix : std::string{}) : std::string{}; }},
        {DIRECTION,    [&] { return direction; }},
    };

    const std::string identifier =
        (ap_Model && ap_Port && !ap_Model->isPortIndicatorNameUnique(ap_Port))
        ? " (" + ap_Port->Name() + ")" //use port ID ?!
        : std::string{};


    std::string name{};
    for (std::size_t i = 0; i < a_NameParts.size(); ++i) {
        const auto& part = a_NameParts[i];
        if (auto it = tokenMap.find(part); it != tokenMap.end()) {
            const std::string value = it->second();
            name += value.empty() ? part : value; //if value is empty, use the original part
        }
        else {
            name += part;
        }
        if (i + 1 < a_NameParts.size()) {
            name += ' '; // add space except after the last element
        }
    }
    name += identifier;

    return name;
}
