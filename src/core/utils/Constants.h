// Constants.h
#pragma once
#include <limits>

namespace CairnConstants
{
    inline constexpr int    kResultCount = 2;         /** Number of result slots: 0 = PLAN, 1 = HIST */
    inline constexpr double kRunningEpsilon = 1.e-20; /** Minimum running time to compute mean values */
    inline constexpr double kEpsilon = 1.e-6;         /** Threshold below which a value is treated as zero */
    inline constexpr double kHoursPerYear = 8760.0;   /** Hours per year for replacement calculation */
    inline constexpr double kDoubleInit = std::numeric_limits<double>::quiet_NaN(); /** Sentinel for uninitialized double - default values are set by addParameter */

    constexpr char DOUBLE_FMT_C[] = "%.12g";          /** double precision */

    inline const std::string PARAM_SOLVER_NAME = "Solver";
    inline const std::string PARAM_AllowShutDown_NAME = "AllowShutDown";
    inline const std::string PARAM_MinPower_NAME = "MinPower";
}