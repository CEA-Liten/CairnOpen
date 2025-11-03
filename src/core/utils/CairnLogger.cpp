#include "CairnLogger.h"

#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/sinks/basic_file_sink.h"
#include "MIPLogger.h"
#include <filesystem>
namespace fs = std::filesystem;

namespace CairnLogger {
    static const std::string logger_name = "cairn";

	void CAIRNLOGGERSHARED_EXPORT CreateLogger(bool a_LogCons, const std::string& a_LogFile)
	{
        std::string vName = logger_name;
        std::vector<spdlog::sink_ptr> sinks;
        if (a_LogCons) {
            auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
            console_sink->set_level(spdlog::level::info);
            console_sink->set_pattern("[%c] [%^%l%$] %v");
            sinks.push_back(console_sink);
        }
        if (a_LogFile != "") {
            auto file_sink =
                std::make_shared<spdlog::sinks::basic_file_sink_mt>(a_LogFile, true);
            file_sink->set_level(spdlog::level::debug);
            file_sink->set_pattern("[%c %l] %v");
            sinks.push_back(file_sink);
            fs::path vPath(a_LogFile);
            vName = vPath.stem().string();

        }                                
        auto logger = std::make_shared<spdlog::logger>(vName, sinks.begin(), sinks.end());   
        logger->set_level(spdlog::level::debug);
        spdlog::set_default_logger(logger);
        MIPModeler::InitLogger(logger);
	}

    std::shared_ptr<spdlog::logger> GetDefaultLogger()
    {
        return spdlog::default_logger();
    }

    void CAIRNLOGGERSHARED_EXPORT Flush()
    {
        return spdlog::default_logger()->flush();
    }

    MessageLogger::MessageLogger()
    {
        m_level = spdlog::level::info;
    }

    MessageLogger::~MessageLogger()
    {
        flush();
    }

    void MessageLogger::log(const std::string& a_msg)
    {       
        m_msg += a_msg;        
    }

    void MessageLogger::flush()
    {
        if (m_msg != "") {
            spdlog::log((spdlog::level::level_enum)m_level, m_msg);
            m_msg = "";
        }
    }

    MessageLogger& MessageLogger::debug()
    {
        m_level = spdlog::level::debug;
        return *this;
    }

    MessageLogger& MessageLogger::info()
    {    
        m_level = spdlog::level::info;
        return *this;
    }

    MessageLogger& MessageLogger::warning()
    {
        m_level = spdlog::level::warn;
        return *this;
    }

    MessageLogger& MessageLogger::error()
    {
        m_level = spdlog::level::err;
        return *this;
    }

    MessageLogger& MessageLogger::critical()
    {
        m_level = spdlog::level::critical;
        return *this;
    }

}

CAIRNLOGGERSHARED_EXPORT CairnLogger::MessageLogger&  operator<<(CairnLogger::MessageLogger& os, const std::string& msg)
{
    os.log(msg);
    return os;
}

CAIRNLOGGERSHARED_EXPORT CairnLogger::MessageLogger& operator<<(CairnLogger::MessageLogger& os, const double &msg)
{
    os.log(std::to_string(msg));
    return os;
}


CAIRNLOGGERSHARED_EXPORT CairnLogger::MessageLogger& operator<<(CairnLogger::MessageLogger& os, const std::vector<double>& msg)
{
    std::string vLog = "";
    std::string vSep = "[";
    for (auto& val : msg) {
        vLog += vSep + std::to_string(val);
        vSep = ",";
    }    
    os.log(vLog+"]");
    return os;
}

