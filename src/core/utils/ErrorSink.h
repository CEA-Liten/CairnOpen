/**
* \file		ErrorSink.h
* \brief    A sink to collect warnings, errors, and criticals
* \version	1.0
* \author	Ali Kassem 
* \date		23/06/2026
*/

#pragma once
#include "spdlog/sinks/base_sink.h"
#include <vector>
#include <string>
#include <mutex>

namespace CairnLogger
{
    struct ErrorEntry
    {
        std::string               message;
        spdlog::level::level_enum level;

        bool isWarning()  const { return level == spdlog::level::warn; }
        bool isError()    const { return level == spdlog::level::err; }
        bool isCritical() const { return level == spdlog::level::critical; }

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

    class ErrorSink : public spdlog::sinks::base_sink<std::mutex>
    {
    public:
        // One collector for the whole app
        static std::shared_ptr<ErrorSink> instance()
        {
            static auto sink = std::make_shared<ErrorSink>();
            return sink;
        }

        // Get and clear the collected errors
        std::vector<ErrorEntry> getAndClear()
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            return std::exchange(m_errors, {});
        }

        bool hasErrors() const
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            return !m_errors.empty();
        }

        int count() const
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            return static_cast<int>(m_errors.size());
        }

        void clear()
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            m_errors.clear();
        }

    protected:
        // Called by spdlog on every log message
        void sink_it_(const spdlog::details::log_msg& msg) override
        {
            // Collect warnings, errors, criticals 
            if (msg.level < spdlog::level::warn)
                return;

            std::lock_guard<std::mutex> lock(m_mutex);
            m_errors.push_back(
                { std::string(msg.payload.data(), msg.payload.size()), msg.level }
            );
        }

        void flush_() override {}

    private:
        std::vector<ErrorEntry> m_errors;
        mutable std::mutex m_mutex; 
    };
}