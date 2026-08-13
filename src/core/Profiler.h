#pragma once

/**
 * \file    Profiler.h
 * \brief   Implementation of CairnProfiling framework
 * \version 1.0
 * \author  Ali KASSEM
 * \date    12/06/2026
 * 
 * ==========
 * Lightweight, header-only-declarable profiling framework for Cairn.
 *
 * Usage pattern:
 *
 *   // Entire phase, RAII-based: 
 *   // a resource is acquired in a constructor and released in the destructor
 *   {
 *       ScopedProfiler p("buildProblem", iterationId);
 *       mProblem->buildProblem();
 *   }   // record written automatically on destruction
 *
 * Compile-time disable:
 *   #define ENABLE_PROFILING 0    ->  all macros expand to nothing
 *   #define ENABLE_PROFILING 1    ->  full instrumentation
 *
 * If ENABLE_PROFILING is not defined it defaults to 1.
 */

#include <string>
#include <vector>
#include <chrono>
#include <mutex>
#include <atomic>
#include <cstddef>
#include <memory>

#include "CairnCore_global.h"

namespace CairnProfiling {

// -----------------------------------------------------------------------------
//  Memory snapshot
// -----------------------------------------------------------------------------

/**
 * All values are in bytes; callers convert to MB at output time.
 */
struct MemorySnapshot
{
    std::size_t rssBytes = 0;      // Resident Set Size (currently in RAM)
    std::size_t vmBytes  = 0;      // Virtual Memory Size
    std::size_t peakRssBytes = 0;  // Peak RSS so far (from OS)
};

/**
 * MemorySampler
 * Platform abstraction: Windows calls GetProcessMemoryInfo(), 
 * Linux reads /proc/self/status.
 */
class MemorySampler
{
public:
    static MemorySnapshot sample() noexcept;
};

// -----------------------------------------------------------------------------
//  Profile record (one row in the CSV)
// -----------------------------------------------------------------------------

struct ProfileRecord
{
    std::string phaseName;
    int iterationId = -1;         // -1 = global (once) e.g. init phases

    double durationMs = 0.0;

    std::size_t rssBefore = 0;    // bytes
    std::size_t rssAfter = 0;     // bytes
    std::size_t peakRss = 0;      // bytes (max observed while scoped)

    std::string startTimestamp;
    std::string endTimestamp;
};

// -----------------------------------------------------------------------------
//  ProfilerManager  (singleton)
// -----------------------------------------------------------------------------

class ProfilerManager
{
public:
    // Returns the process-lifetime singleton.
    static ProfilerManager& instance();

    // Called once before the first doInit() to stamp program start time.
    //void notifyProgramStart() noexcept;

    // Append a completed record (thread-safe).
    void addRecord(ProfileRecord rec);

    // Clear all records, timestamps, ...
    void reset() noexcept;

    // Write profiling.csv and profiling_summary.txt.
    // Called automatically from destructor; may also be called manually.
    void flush(const std::string& outputDir = ".", const std::string& studyName = "");

    // Total iterations seen so far (incremented by ScopedProfiler when
    // phaseName == CAIRN_PROFILE_ITERATION_PHASE).
    int iterationCount() const noexcept { return mIterationCount.load(); }

    // Non-copyable / non-movable singleton
    ProfilerManager(const ProfilerManager&) = delete;
    ProfilerManager& operator=(const ProfilerManager&) = delete;

private:
    ProfilerManager();
    ~ProfilerManager();

    void writeCSV(const std::string& path) const;
    void writeSummary(const std::string& path) const;

    mutable std::mutex mMutex;
    std::vector<ProfileRecord> mRecords;

    std::chrono::steady_clock::time_point mProgramStart;
    std::string mProgramStartISO;

    std::atomic<int> mIterationCount{0};
    bool mFlushed{false};
    std::string mOutputDir{"."};
    std::string mStudyName{};
};

// -----------------------------------------------------------------------------
//  ScopedTimer
// -----------------------------------------------------------------------------

class ScopedTimer
{
public:
    using Clock = std::chrono::steady_clock;
    using TimePoint = Clock::time_point;

    ScopedTimer() noexcept : mStart(Clock::now()) {}

    double elapsedMs() const noexcept
    {
        auto end = Clock::now();
        return std::chrono::duration<double, std::milli>(end - mStart).count();
    }

    TimePoint startPoint() const noexcept { return mStart; }

private:
    TimePoint mStart;
};

// -----------------------------------------------------------------------------
//  ScopedProfiler
// -----------------------------------------------------------------------------

/**
 * RAII helper. Construct at the top of a scope; the destructor records
 * timing + memory and deposits the ProfileRecord into ProfilerManager.
 */
class CAIRNCORESHARED_EXPORT ScopedProfiler
{
public:
    explicit ScopedProfiler(std::string phaseName, int iterationId = -1) noexcept;
    ~ScopedProfiler() noexcept;

    // Non-copyable
    ScopedProfiler(const ScopedProfiler&) = delete;
    ScopedProfiler& operator=(const ScopedProfiler&) = delete;

private:
#if ENABLE_PROFILING
    std::string mPhaseName;
    int mIterationId{ -1 };
    ScopedTimer mTimer;
    MemorySnapshot mBefore;
    std::string mStartISO;
#endif
};

// ----------------------------------------------------------------------------
//  Iteration phase name
//
//  Single source of truth for the top-level per-iteration scope name.
//  Used by both the call site (CAIRN_PROFILE_ITERATION) and
//  ProfilerManager::writeSummary() for its special-case iteration logic.
//  To rename it for a different application, change only this one line.
// ----------------------------------------------------------------------------

#define CAIRN_PROFILE_ITERATION_PHASE "doStep"

// -----------------------------------------------------------------------------
//  Convenience macros
// -----------------------------------------------------------------------------

#if ENABLE_PROFILING

// Use at the top of a function body that IS the phase.
#  define CAIRN_PROFILE_FUNCTION(iter)   \
    CairnProfiling::ScopedProfiler _cairn_profiler_(__func__, (iter))

// Use when you need an explicit phase name (e.g. wrapping a call site).
#  define CAIRN_PROFILE_SCOPE(name, iter) \
    CairnProfiling::ScopedProfiler _cairn_profiler_((name), (iter))

// Use for the top-level iteration scope (rolling-horizon loop body).
// Automatically uses CAIRN_PROFILE_ITERATION_PHASE as the phase name.
#  define CAIRN_PROFILE_ITERATION(iter) \
    CairnProfiling::ScopedProfiler _cairn_profiler_(CAIRN_PROFILE_ITERATION_PHASE, (iter))

// Stamp program start (call once, before doInit).
//#  define CAIRN_PROFILE_PROGRAM_START() \
//    CairnProfiling::ProfilerManager::instance().notifyProgramStart()

#  define CAIRN_PROFILE_PROGRAM_START() \
    CairnProfiling::ProfilerManager::instance().reset()

// Force early flush (optional; also happens at program exit in the destructor).
#  define CAIRN_PROFILE_FLUSH(dir, studyName) \
    CairnProfiling::ProfilerManager::instance().flush(dir, studyName)

#else  // ENABLE_PROFILING == 0 -> zero overhead

#  define CAIRN_PROFILE_FUNCTION(iter)     do {} while(false)
#  define CAIRN_PROFILE_SCOPE(name, iter)  do {} while(false)
#  define CAIRN_PROFILE_ITERATION(iter)    do {} while(false)
#  define CAIRN_PROFILE_PROGRAM_START()    do {} while(false)
#  define CAIRN_PROFILE_FLUSH(dir, studyName)         do {} while(false)

#endif  // ENABLE_PROFILING

}  // namespace CairnProfiling
