#pragma once

#include "Constants.h"
using namespace CairnConstants;

class SubModel;

namespace SubModelComputation {

    inline double evalNonZero(const MIPModeler::MIPExpression& e, const double* sol) noexcept {
        const double v = e.evaluate(sol);
        return (std::fabs(v) > kEpsilon) ? v : 0.0;
    }

    inline double levelFactor(const SubModel* self, int year, bool isEnv) noexcept {
        return isEnv
            ? self->parentComponent()->ImpactLevelizationTable().at(year)
            : self->parentComponent()->LevelizationTable().at(year);
    }

    inline int resolveYear(uint t, double ts, uint histHours,
        int year, const std::vector<double>& table)
    {
        const uint t_hour = static_cast<uint>(t * ts + 0.999999) + histHours; // std::ceil(t * timeStep))
        while (year + 1 < static_cast<int>(table.size()) && t_hour > table[year])
            ++year;
        return year;
    }

    template<typename Accumulator>
    inline void runLoop(
        const SubModel* self,
        uint aNpdt,
        const MIPModeler::MIPExpression1D& exp,
        const double* optSol,
        Accumulator&& accum)
    {
        for (uint t = 0; t < aNpdt; ++t) {
            const double val = evalNonZero(exp.at(t), optSol);
            if (val != 0.0)
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
            if (val < -kEpsilon) charged += ts;
            };
    }

    inline auto makeProdAccumulator(const SubModel* self, double& ret, double factor, double coeff, 
        double offset, bool integrate)
    {
        return [self, &ret, factor, coeff, offset, integrate](uint t, double val) {
            const double base = coeff * val + offset;
            ret += integrate ? base * self->TimeStep(t) * factor : base;
            };
    }

    inline auto makeProdCDAccumulator(const SubModel* self, double& charged, double& discharged,
        double factor, double coeff, double offset)
    {
        return [self, &charged, &discharged, factor, coeff, offset](uint t, double val) {
            const double contrib = (coeff * val + offset) * self->TimeStep(t) * factor;
            if (val > kEpsilon) discharged += contrib;
            if (val < -kEpsilon) charged += contrib;
            };
    }

    inline auto makeLvlAccumulator(const SubModel* self, double& ret,
        double coeff, double offset,
        double factor, bool isEnv)
    {
        auto* compo = self->parentComponent();
        int year = 0;

        return [self, &ret, coeff, offset, factor, isEnv, compo, year](uint t, double val) mutable {
            year = resolveYear(t, self->TimeStep(t), compo->HistNbHours(), year, compo->TableYearsHours());
            const double lvl = levelFactor(self, year, isEnv);
            ret += (coeff * val + offset) * self->TimeStep(t) * lvl * factor;
            };
    }
}