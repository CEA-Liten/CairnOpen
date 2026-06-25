
/**
 * \file    Profiler.cpp
 * \brief   Implementation of CairnProfiling framework
 * \version 1.0
 * \author  Ali KASSEM
 * \date    12/06/2026
 * 
 * ============
 * Implementation of CairnProfiling framework
 * C++17 required
 */

#include "Profiler.h"

#include <algorithm>
#include <cstring>
#include <ctime>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <numeric>
#include <sstream>
#include <filesystem>
namespace fs = std::filesystem;

// -----------------------------------------------------------------------------
//  Platform headers for memory sampling
// -----------------------------------------------------------------------------
#if defined(_WIN32) || defined(_WIN64)
#  define CAIRN_PLATFORM_WINDOWS 1
#  ifndef WIN32_LEAN_AND_MEAN
#    define WIN32_LEAN_AND_MEAN
#  endif
// NOMINMAX must be defined before windows.h to prevent it from injecting
// min/max macros that break std::max / std::min 
#  ifndef NOMINMAX
#    define NOMINMAX
#  endif
#  include <windows.h>
#  include <psapi.h>
#  pragma comment(lib, "Psapi.lib")
// Belt-and-suspenders: undef in case another TU already pulled in
// windows.h without NOMINMAX before our #include was reached.
#  ifdef min
#    undef min
#  endif
#  ifdef max
#    undef max
#  endif
#else
#  define CAIRN_PLATFORM_LINUX 1
#  include <fstream>
#endif

namespace CairnProfiling {

// -----------------------------------------------------------------------------
//  Internal helpers
// -----------------------------------------------------------------------------

static constexpr double kBytesToMB = 1.0 / (1024.0 * 1024.0);

// ISO-8601 timestamp for "now" using wall clock.
static std::string wallClockISO() noexcept
{
    try {
        auto now = std::chrono::system_clock::now();
        std::time_t t = std::chrono::system_clock::to_time_t(now);
        std::tm tm_buf{};
#if defined(_WIN32) || defined(_WIN64)
        localtime_s(&tm_buf, &t);
#else
        localtime_r(&t, &tm_buf);
#endif
        // Add milliseconds
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
            now.time_since_epoch()) % 1000;

        std::ostringstream oss;
        oss << std::put_time(&tm_buf, "%Y-%m-%d %H:%M:%S")
            << '.' << std::setw(3) << std::setfill('0') << ms.count();
        return oss.str();
    }
    catch (...) {
        return "unknown";
    }
}

// -----------------------------------------------------------------------------
//  MemorySampler
// -----------------------------------------------------------------------------

#if defined(CAIRN_PLATFORM_LINUX)

MemorySnapshot MemorySampler::sample() noexcept
{
    MemorySnapshot snap;
    try {
        // /proc/self/status gives VmRSS, VmSize, VmPeak, VmHWM in kB
        std::ifstream statusFile("/proc/self/status");
        if (!statusFile.is_open()) return snap;

        std::string line;
        while (std::getline(statusFile, line)) {
            auto parse = [&](const char* key, std::size_t& dst) {
                if (line.substr(0, strlen(key)) == key) {
                    std::istringstream iss(line.substr(strlen(key)));
                    std::size_t val = 0;
                    iss >> val;
                    dst = val * 1024;  // kB -> bytes
                }
            };

            parse("VmRSS:",  snap.rssBytes);
            parse("VmSize:", snap.vmBytes);
            parse("VmPeak:", snap.peakRssBytes);  // "peak virtual"
            parse("VmHWM:",  snap.peakRssBytes);  // "high watermark RSS" - overwrites, that's fine
        }
    }
    catch (...) {}
    return snap;
}

#elif defined(CAIRN_PLATFORM_WINDOWS)

MemorySnapshot MemorySampler::sample() noexcept
{
    MemorySnapshot snap;
    try {
        PROCESS_MEMORY_COUNTERS_EX pmc{};
        pmc.cb = sizeof(pmc);
        if (GetProcessMemoryInfo(GetCurrentProcess(),
                reinterpret_cast<PROCESS_MEMORY_COUNTERS*>(&pmc),
                sizeof(pmc)))
        {
            snap.rssBytes = pmc.WorkingSetSize;
            snap.vmBytes = static_cast<std::size_t>(pmc.PrivateUsage);
            snap.peakRssBytes = pmc.PeakWorkingSetSize;
        }
    }
    catch (...) {}
    return snap;
}

#else
// return zeroes
MemorySnapshot MemorySampler::sample() noexcept { return {}; }
#endif

// -----------------------------------------------------------------------------
//  ProfilerManager
// -----------------------------------------------------------------------------

ProfilerManager& ProfilerManager::instance()
{
    // Meyer's singleton 
    static ProfilerManager inst;
    return inst;
}

ProfilerManager::ProfilerManager()
    : mProgramStart(ScopedTimer::Clock::now()), 
      mProgramStartISO(wallClockISO())
{}

ProfilerManager::~ProfilerManager()
{
    flush(mOutputDir, mStudyName);
}

//void ProfilerManager::notifyProgramStart() noexcept
//{
//    std::lock_guard<std::mutex> lk(mMutex);
//    mProgramStart = ScopedTimer::Clock::now();
//    mProgramStartISO = wallClockISO();
//}

void ProfilerManager::addRecord(ProfileRecord rec)
{
    // Count iterations
    if (rec.phaseName == CAIRN_PROFILE_ITERATION_PHASE) {
        mIterationCount.fetch_add(1, std::memory_order_relaxed);
    }

    std::lock_guard<std::mutex> lk(mMutex);
    mRecords.push_back(std::move(rec));
}

void ProfilerManager::reset() noexcept
{
    std::lock_guard<std::mutex> lk(mMutex);
    mRecords.clear();
    mFlushed = false;
    mIterationCount.store(0);
    mProgramStart = ScopedTimer::Clock::now();
    mProgramStartISO = wallClockISO();
}

// -----------------------------------------------------------------------------
//  flush() - writes CSV + summary
// -----------------------------------------------------------------------------

void ProfilerManager::flush(const std::string& outputDir, const std::string&  studyName)
{
    std::lock_guard<std::mutex> lk(mMutex);
    if (mFlushed || mRecords.empty()) return;
    mFlushed = true;
    mOutputDir = outputDir;
    mStudyName = studyName;

    const std::string prefix = studyName.empty() ? "" : studyName + "_";
    const std::string csvPath = (fs::path(outputDir) / (prefix + "profiling_results.csv")).string();
    const std::string summaryPath = (fs::path(outputDir) / (prefix + "profiling_summary.txt")).string();

    try { writeCSV(csvPath); }
    catch (const std::exception& e) {
        std::cerr << "[CairnProfiling] Failed to write CSV: " << e.what() << "\n";
    }
    try { writeSummary(summaryPath); }
    catch (const std::exception& e) {
        std::cerr << "[CairnProfiling] Failed to write summary: " << e.what() << "\n";
    }
}

// -----------------------------------------------------------------------------

static std::string makeRunId() noexcept
{
    // Simple: ISO timestamp with colons with dashes
    auto s = wallClockISO();
    for (char& c : s) if (c == ':') c = '-';
    return s;
}

void ProfilerManager::writeCSV(const std::string& path) const
{
    std::ofstream out(path);
    if (!out.is_open()) return;

    const std::string runId = makeRunId();

    // Header
    out << "RunId;Iteration;Phase;StartTime;EndTime;Duration(ms);"
           "MemoryBefore(MB);MemoryAfter(MB);MemoryDelta(MB);PeakMemory(MB)\n";

    for (const auto& r : mRecords) {
        const double beforeMB = r.rssBefore  * kBytesToMB;
        const double afterMB  = r.rssAfter   * kBytesToMB;
        const double deltaMB  = afterMB - beforeMB;
        const double peakMB   = r.peakRss    * kBytesToMB;

        out << runId             << ';'
            << r.iterationId     << ';'
            << r.phaseName       << ';'
            << r.startTimestamp  << ';'
            << r.endTimestamp    << ';'
            << std::fixed << std::setprecision(3) << r.durationMs << ';'
            << std::fixed << std::setprecision(3) << beforeMB     << ';'
            << std::fixed << std::setprecision(3) << afterMB      << ';'
            << std::fixed << std::setprecision(3) << deltaMB      << ';'
            << std::fixed << std::setprecision(3) << peakMB       << '\n';
    }
}

// -----------------------------------------------------------------------------

void ProfilerManager::writeSummary(const std::string& path) const
{
    std::ofstream out(path);
    if (!out.is_open()) return;

    // --- Formatting helpers --------------------------------------------------
    auto w = [&](const char* label, double value, const char* unit = "") {
        out << std::left << std::setw(38) << label
            << std::fixed << std::setprecision(3) << value << ' ' << unit << '\n';
    };
    auto ws = [&](const char* label, const std::string& value) {
        out << std::left << std::setw(38) << label << value << '\n';
    };

    // --- Total runtime --------------------------------------------------------
    auto now = ScopedTimer::Clock::now();
    double totalMs = std::chrono::duration<double, std::milli>(
        now - mProgramStart).count();

    // --- Collect the ordered list of unique phase names ------------------------
    //   Insertion order is preserved so the summary reflects the execution order.
    //   CAIRN_PROFILE_ITERATION_PHASE is excluded here - it gets its own dedicated section below.
    std::vector<std::string> phaseOrder;
    for (const auto& r : mRecords) {
        if (r.phaseName == CAIRN_PROFILE_ITERATION_PHASE) continue;
        if (std::find(phaseOrder.begin(), phaseOrder.end(), r.phaseName) == phaseOrder.end())
            phaseOrder.push_back(r.phaseName);
    }

    // --- Generic per-phase stats (works for any phase name) ---------------------
    struct PhaseStats {
        double avg = 0.0, min = 0.0, max = 0.0;
        int    count = 0;
    };

    auto statsFor = [&](const std::string& phase) -> PhaseStats {
        std::vector<double> v;
        for (const auto& r : mRecords)
            if (r.phaseName == phase)
                v.push_back(r.durationMs);
        if (v.empty()) return {};
        double sum = std::accumulate(v.begin(), v.end(), 0.0);
        return { sum / v.size(),
                 *std::min_element(v.begin(), v.end()),
                 *std::max_element(v.begin(), v.end()),
                 int(v.size()) };
    };

    // --- CAIRN_PROFILE_ITERATION_PHASE stats (iteration-level special case) -------------------------
    PhaseStats stepStats = statsFor(CAIRN_PROFILE_ITERATION_PHASE);

    // --- Peak memory across all records --------------------------------------
    std::size_t peakBytes = 0;
    for (const auto& r : mRecords)
        peakBytes = std::max(peakBytes, r.peakRss);

    // --- Memory growth per iteration -----------------------------------------
    //   (rssAfter of last CAIRN_PROFILE_ITERATION_PHASE - rssAfter of first CAIRN_PROFILE_ITERATION_PHASE) / (n-1)
    std::vector<double> stepRssAfter;
    for (const auto& r : mRecords)
        if (r.phaseName == CAIRN_PROFILE_ITERATION_PHASE)
            stepRssAfter.push_back(double(r.rssAfter));

    double memGrowthPerIterMB = 0.0;
    if (stepRssAfter.size() >= 2) {
        double growthBytes = stepRssAfter.back() - stepRssAfter.front();
        memGrowthPerIterMB = (growthBytes * kBytesToMB)
            / double(stepRssAfter.size() - 1);
    }

    // --- Write ----------------------------------------------------------------
    out << "======================================================\n";
    out << "  Cairn Profiling Summary\n";
    out << "  Run started : " << mProgramStartISO << "\n";
    out << "======================================================\n\n";

    out << "[Global]\n";
    w("  Total runtime", totalMs, "ms");
    w("  Total runtime", totalMs / 1000.0, "s");
    ws("  Total iterations", std::to_string(stepStats.count));
    out << "\n";

    // --- CAIRN_PROFILE_ITERATION_PHASE: one record per iteration, report avg/min/max ------------------
    out << "[Phase: " << CAIRN_PROFILE_ITERATION_PHASE << " (full iteration)]\n";
    if (stepStats.count == 1) {
        w("  Duration", stepStats.avg, "ms");
    }
    else {
        w("  Avg duration", stepStats.avg, "ms");
        w("  Min duration", stepStats.min, "ms");
        w("  Max duration", stepStats.max, "ms");
    }
    out << "\n";

    // --- All other phases: discovered automatically from mRecords --------------
    for (const auto& phase : phaseOrder) {
        PhaseStats s = statsFor(phase);
        out << "[Phase: " << phase << "]\n";
        if (s.count == 1) {
            // Single-shot phases (doInit, saveStudy): duration only
            w("  Duration", s.avg, "ms");
        }
        else {
            if (stepStats.count == 1) {
                w("  Duration", s.avg, "ms");
            }
            else {
                w("  Avg duration", s.avg, "ms");
                w("  Min duration", s.min, "ms");
                w("  Max duration", s.max, "ms");
            }
        }
        out << "\n";
    }

    out << "[Memory]\n";
    w("  Peak RSS", peakBytes * kBytesToMB, "MB");
    w("  Mem growth / iteration", memGrowthPerIterMB, "MB");
    out << "\n";

    out << "======================================================\n";
    out << "  Full data: profiling_results.csv\n";
    out << "======================================================\n";
}

// -----------------------------------------------------------------------------
//  ScopedProfiler
// -----------------------------------------------------------------------------

ScopedProfiler::ScopedProfiler(std::string phaseName, int iterationId) noexcept
    : mPhaseName(std::move(phaseName)), 
      mIterationId(iterationId), 
      mTimer(), 
      mBefore(MemorySampler::sample()), 
      mStartISO(wallClockISO())
{}

ScopedProfiler::~ScopedProfiler() noexcept
{
    try {
        MemorySnapshot after = MemorySampler::sample();

        ProfileRecord rec;
        rec.phaseName      = mPhaseName;
        rec.iterationId    = mIterationId;
        rec.durationMs     = mTimer.elapsedMs();
        rec.rssBefore      = mBefore.rssBytes;
        rec.rssAfter       = after.rssBytes;
        // Peak: take the highest watermark from before/after
        rec.peakRss        = std::max(mBefore.peakRssBytes, after.peakRssBytes);
        rec.startTimestamp = mStartISO;
        rec.endTimestamp   = wallClockISO();

        ProfilerManager::instance().addRecord(std::move(rec));
    }
    catch (...) {
        // Never throw from destructor
    }
}

}  // namespace CairnProfiling
