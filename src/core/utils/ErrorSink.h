/**
* \file		ErrorSink.h
* \brief    A sink to collect warnings, errors, and criticals
* \version	1.0
* \author	Ali Kassem 
* \date		23/06/2026
*/

#pragma once
#include "spdlog/sinks/base_sink.h"
#include <mutex>
#include <vector>
#include <string>
#include <iterator>

namespace CairnLogger
{
    // --- Object context ----------------------------------------
    // Set by ObjectLogScope (RAII) to tag log entries with the calling object.
    // Returns empty string when no context is active.
        const std::string& currentObjectContext();
        void pushObjectContext(const std::string& name);
        void popObjectContext();

    // --- Log entry ----------------------------------------------------------

    struct ErrorEntry
    {
        std::string message;
        std::string objectName; /** CairnObject::Name() at call site - empty if not set */
        spdlog::level::level_enum level;

        bool isWarning() const
        {
            return level == spdlog::level::warn;
        }

        bool isError() const
        {
            return level == spdlog::level::err ||
                level == spdlog::level::critical;
        }

        std::string levelStr() const
        {
            switch (level)
            {
            case spdlog::level::warn:     return "WARNING";
            case spdlog::level::err:      return "ERROR";
            case spdlog::level::critical: return "CRITICAL";
            default:                      return "UNKNOWN";
            }
        }
    };

    // --- Sink ---------------------------------------------------------------

    class ErrorSink : public spdlog::sinks::base_sink<std::mutex>
    {
    public:
        // One collector for the whole app
        static std::shared_ptr<ErrorSink> instance()
        {
            static auto sink = std::make_shared<ErrorSink>();
            return sink;
        }

        // Get and clear the collected warnings and errors
        std::vector<ErrorEntry> getAndClear()
        {
            std::lock_guard<std::mutex> lock(m_mutex);

            std::vector<ErrorEntry> result;
            result.reserve(m_errors.size() + m_warnings.size());

            // Move contents into result
            std::move(m_errors.begin(), m_errors.end(), std::back_inserter(result));
            std::move(m_warnings.begin(), m_warnings.end(), std::back_inserter(result));

            // Clear internal storage
            m_errors.clear();
            m_warnings.clear();

            return result;
        }

        bool hasWarnings() const
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            return !m_warnings.empty();
        }

        bool hasErrors() const
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            return !m_errors.empty();
        }

        int warningCount() const
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            return static_cast<int>(m_warnings.size());
        }

        int errorCount() const
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            return static_cast<int>(m_errors.size());
        }

        int count() const
        {
            return warningCount() + errorCount();
        }

        void clear()
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            m_warnings.clear();
            m_errors.clear();
        }

    protected:
        void flush_() override {}

        // Called by spdlog on every log message
        void sink_it_(const spdlog::details::log_msg& msg) override
        {
            // Only collect warnings, errors, criticals
            if (msg.level < spdlog::level::warn)
                return;

            ErrorEntry entry{
                std::string(msg.payload.data(), msg.payload.size()),
                currentObjectContext(),
                msg.level
            };

            std::lock_guard<std::mutex> lock(m_mutex);

            if (msg.level == spdlog::level::warn) {
                if(!warningExists(entry))
                    m_warnings.push_back(std::move(entry));
            }
            else {
                m_errors.push_back(std::move(entry));
            }
        }

        bool warningExists(const ErrorEntry& entry)
        {
            const auto it = std::find_if(
                m_warnings.begin(),
                m_warnings.end(),
                [&](const ErrorEntry& e)
                {
                    return e.objectName == entry.objectName &&
                           e.message == entry.message &&
                           e.level == entry.level;
                });

            return (it != m_warnings.end());
        }

    private:
        std::vector<ErrorEntry> m_warnings;
        std::vector<ErrorEntry> m_errors;
        mutable std::mutex m_mutex; 
    };
}