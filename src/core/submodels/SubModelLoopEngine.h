#pragma once

#include "Constants.h"
using namespace CairnConstants;

class SubModel;

namespace SubModelComputation {

    inline int resolveYear(uint t, double timeStep, uint histHours,
        int year, const std::vector<double>& table)
    {
        const uint t_hour = static_cast<uint>(std::ceil(t * timeStep)) + histHours;

        while (
            year + 1 < static_cast<int>(table.size()) &&
            t_hour > table[year])
        {
            ++year;
        }

        return year;
    }

    template<typename Accumulator>
    inline void runLoop(
        uint Npdt,
        const MIPModeler::MIPExpression1D& exp,
        const double* optSol,
        Accumulator&& accum)
    {
        for (uint t = 0; t < Npdt; ++t) {
            const double val = exp[t].evaluate(optSol);

            if (std::fabs(val) > kEpsilon)
                accum(t, val);
        }
    }

    inline auto makeTimeAccumulator(const SubModel* self, double& ret, double factor)
    {
        return [self, &ret, factor](uint t, double /*val*/)
        {
            ret += self->TimeStep(t) * factor;
        };
    }

    inline auto makeTimeCDAccumulator(const SubModel* self, double& charged, double& discharged, double factor)
    {
        return [self, &charged, &discharged, factor](uint t, double val) {
            const double ts = self->TimeStep(t) * factor;
            if (val > kEpsilon) discharged += ts;
            else if (val < -kEpsilon) charged += ts;
        };
    }

    inline auto makeProdAccumulator(const SubModel* self, double& ret, double factor, double coeff,
        double offset, bool integrate)
    {
        return [self, &ret, factor, coeff, offset, integrate](uint t, double val) {
            const double base = coeff * val + offset;
            if (integrate)
                ret += base * self->TimeStep(t) * factor;
            else
                ret += base;
        };
    }

    inline auto makeProdCDAccumulator(const SubModel* self, double& charged, double& discharged,
        double factor, double coeff, double offset)
    {
        return [self, &charged, &discharged, factor, coeff, offset](uint t, double val) {
            const double contrib = (coeff * val + offset) * self->TimeStep(t) * factor;
            if (val > kEpsilon)
                discharged += contrib;
            else if (val < -kEpsilon)
                charged += contrib;
        };
    }

    inline auto makeLvlAccumulator(const SubModel* self, double& ret,
        double coeff, double offset,
        double factor, bool isEnv)
    {
        auto* const compo = self->parentComponent();
        const auto& levelTable = isEnv
            ? compo->ImpactLevelizationTable()
            : compo->LevelizationTable();

        const auto& tableYearsHours = compo->TableYearsHours();
        const uint histHours = compo->HistNbHours();

        int year = 0;

        return [self, &ret, coeff, offset, factor, isEnv,
            compo, &levelTable, &tableYearsHours, histHours, year](uint t, double val) mutable
        {
            const double ts = compo->TimeStep(t);
            year = resolveYear(t, ts, histHours, year, tableYearsHours);
            const double level = levelTable[year];

            ret += (coeff * val + offset) * ts * level * factor;
        };
    }

    inline auto makeLvlCDAccumulator(const SubModel* self,
        double& charged, double& discharged,
        double coeff, double offset, double factor)
    {
        auto* const compo = self->parentComponent();

        const auto& levelTable = compo->LevelizationTable();
        const auto& tableYearsHours = compo->TableYearsHours();
        const uint histHours = compo->HistNbHours();

        int year = 0;

        return[&charged, &discharged, coeff, offset, factor,
            compo, &levelTable, &tableYearsHours, histHours, year](uint t, double val) mutable
        {
            const double ts = compo->TimeStep(t);
            year = resolveYear(t, ts, histHours, year, tableYearsHours);
            const double level = levelTable[year];

            const double contribution = (coeff * val + offset) * ts * level * factor;

            if (val > kEpsilon)
            {
                discharged += contribution;
            }
            else if (val < -kEpsilon)
            {
                charged += contribution;
            }
        };
    }
}