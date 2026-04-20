#include "CairnLogger.h"

#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/sinks/basic_file_sink.h"
#include "MIPLogger.h"
#include "OrJsonUtils.h"
#include "CairnAPIUtils.h"

#include <chrono>
#include <ctime>
#include <fstream>
#include <filesystem>
namespace fs = std::filesystem;
#include "json.hpp"
using json = nlohmann::json;

namespace CairnLogger {
    static const std::string logger_name = "cairn";

    void CAIRNLOGGERSHARED_EXPORT CreateLogger(const t_dict& a_DefLog)
    {
        loggerFactory.CreateLogger(a_DefLog);
    }


	void CAIRNLOGGERSHARED_EXPORT CreateLogger(bool a_LogCons, const std::string& a_LogFile)
	{
        t_dict vOptions = { {"console", a_LogCons}, {"path", a_LogFile } };
        loggerFactory.CreateLogger(vOptions);
	}

    void CAIRNLOGGERSHARED_EXPORT CreateLogger()
    {
        t_dict vOptions = {};
        loggerFactory.CreateLogger(vOptions);
    }

    std::shared_ptr<spdlog::logger> GetDefaultLogger()
    {
        return spdlog::default_logger();
    }

    void ChangeFileLogger(const std::string& a_LogFile)
    {
        loggerFactory.ChangeFileLogger(a_LogFile);
    }

    void CAIRNLOGGERSHARED_EXPORT Flush()
    {
        return spdlog::default_logger()->flush();
    }


    //-------------------------------------------------------
    LoggerFactory::LoggerFactory()
    {
        // default config
        m_name = logger_name;
        m_LogCons = true;
        m_LogFile = false;
        m_LogPath = "";
        m_LogAuxFile = false;
        m_LogAuxPath = "";
        m_Level = spdlog::level::debug;
        m_FlushLevel = spdlog::level::debug;
    }

    void LoggerFactory::CreateLogger(const t_dict& a_DefLogs)
    {   
        // default config
        m_name = logger_name;
        m_LogCons = true;
        m_LogFile = false;
        m_LogPath = "";
        m_LogAuxFile = false;
        m_LogAuxPath = "";
        m_Level = spdlog::level::debug;
        m_FlushLevel = spdlog::level::debug;

        std::string prefFileName = std::getenv("CAIRN_BIN") + (std::string)"/../resources/Prefs.json";
        // 1) load preferences
        json input;
        std::ifstream file(prefFileName);
        if (file.is_open()) {
            try
            {
                file >> input;
                if (input.contains("logs")) {
                    const json& vLogsCfg = input["logs"];
                    std::string vStr;
                    if (orjson::from_json(vLogsCfg, "level", vStr)) {
                        if (vStr != "") m_Level = spdlog::level::from_str(vStr);
                    }
                    std::string vStr2;
                    if (orjson::from_json(vLogsCfg, "flushlevel", vStr2)) {
                        if (vStr2 != "") m_FlushLevel = spdlog::level::from_str(vStr2);
                    }

                    orjson::from_json(vLogsCfg, "console", m_LogCons);
                    orjson::from_json(vLogsCfg, "file", m_LogFile);
                    orjson::from_json(vLogsCfg, "path", m_LogPath);
                    orjson::from_json(vLogsCfg, "auxfile", m_LogAuxFile);
                    orjson::from_json(vLogsCfg, "auxpath", m_LogAuxPath);
                }
            }
            catch (const std::exception& e)
            {
                //std::cout << e.what();
            }
        }

        // 2) User config
        for (auto& vParam : a_DefLogs) {
            if (vParam.first == "console") {
                m_LogCons = CairnAPIUtils::get_tvalueOr<int>(vParam.second, m_LogCons);
            }
            else if (vParam.first == "file") {
                m_LogFile = CairnAPIUtils::get_tvalueOr<int>(vParam.second, m_LogFile);
            }
            else if (vParam.first == "path") {
                m_LogPath = CairnAPIUtils::get_tvalueOr<std::string>(vParam.second, m_LogPath);
            }
            else if (vParam.first == "auxfile") {
                m_LogAuxFile = CairnAPIUtils::get_tvalueOr<int>(vParam.second, m_LogAuxFile);
            }
            else if (vParam.first == "auxpath") {
                m_LogAuxPath = CairnAPIUtils::get_tvalueOr<std::string>(vParam.second, m_LogAuxPath);
            }
            else if (vParam.first == "level") {
                std::string vStrLevel = CairnAPIUtils::get_tvalueOr<std::string>(vParam.second, "");
                if (vStrLevel != "") m_Level = spdlog::level::from_str(vStrLevel);
            }
            else if (vParam.first == "flushlevel") {
                std::string vStrLevel = CairnAPIUtils::get_tvalueOr<std::string>(vParam.second, "");
                if (vStrLevel != "") m_FlushLevel = spdlog::level::from_str(vStrLevel);
            }
        }

        m_dist_sink = std::make_shared<spdlog::sinks::dist_sink_mt>(create_sinks());

        auto logger = std::make_shared<spdlog::logger>(m_name, m_dist_sink);
        logger->set_level(m_Level);
        logger->flush_on(m_FlushLevel);
        spdlog::set_default_logger(logger);
        MIPModeler::InitLogger(logger);
    }

    std::vector<spdlog::sink_ptr> LoggerFactory::create_sinks()
    {
        // Apply parameters      
        std::vector<spdlog::sink_ptr> sinks;
        if (m_LogCons) {
            auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
            console_sink->set_level(m_Level);
            console_sink->set_pattern("[%c] [%^%l%$] %v");
            sinks.push_back(console_sink);
        }
        if (m_LogFile && m_LogPath != "") {
            auto file_sink =
                std::make_shared<spdlog::sinks::basic_file_sink_mt>(m_LogPath, true);
            file_sink->set_level(m_Level);
            file_sink->set_pattern("[%c %l] %v");
            sinks.push_back(file_sink);
            fs::path vPath(m_LogPath);
            m_name = vPath.stem().string();
        }

        if (m_LogAuxFile && m_LogAuxPath != "") {
            fs::path vLogFileSrv = m_LogAuxPath;
            std::time_t time = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
            char timeString[std::size("yyyy-mm-dd hh-mm-ss")];
            std::strftime(std::data(timeString), std::size(timeString),
                "%F %H-%M-%S", std::localtime(&time));
            std::string vAuxName = " " + (std::string)timeString;
            vLogFileSrv /= m_name + vAuxName + ".log";
            auto fileSrv_sink =
                std::make_shared<spdlog::sinks::basic_file_sink_mt>(vLogFileSrv.string(), true);
            fileSrv_sink->set_level(m_Level);
            fileSrv_sink->set_pattern("[%c %l] %v");
            sinks.push_back(fileSrv_sink);
        }
        return sinks;
    }

    void LoggerFactory::ChangeFileLogger(const std::string& a_LogPath)
    {
        if (m_LogFile) {
            m_LogPath = a_LogPath;
            m_dist_sink->set_sinks(create_sinks());
        }        
    }


    //-------------------------------------------------------
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

