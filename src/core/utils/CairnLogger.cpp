#include "CairnLogger.h"

#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/sinks/basic_file_sink.h"
#include "MIPLogger.h"
#include "OrJsonUtils.h"

#include <chrono>
#include <ctime>
#include <fstream>
#include <filesystem>
namespace fs = std::filesystem;
#include "json.hpp"
using json = nlohmann::json;

namespace CairnLogger {
    static const std::string logger_name = "cairn";

	void CAIRNLOGGERSHARED_EXPORT CreateLogger(bool a_LogCons, const std::string& a_LogFile)
	{
        // default config
        std::string vName = logger_name;
        bool vLogCons = a_LogCons;
        bool vLogFile = (a_LogFile != "");
        bool vLogAuxFile = false;
        std::string vLogAuxPath = "";

        spdlog::level::level_enum vLevel = spdlog::level::debug;
        spdlog::level::level_enum vFlushLevel = spdlog::level::debug;

        std::string prefFileName = std::getenv("CAIRN_BIN") + (std::string)"/../resources/Prefs.json";
        // load preferences
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
                        if (vStr != "") vLevel = spdlog::level::from_str(vStr);
                    }
                    std::string vStr2;
                    if (orjson::from_json(vLogsCfg, "flushlevel", vStr2)) {
                        if (vStr2 != "") vFlushLevel = spdlog::level::from_str(vStr2);
                    }

                    orjson::from_json(vLogsCfg, "console", vLogCons);
                    orjson::from_json(vLogsCfg, "file", vLogFile);
                    orjson::from_json(vLogsCfg, "auxfile", vLogAuxFile);
                    orjson::from_json(vLogsCfg, "auxpath", vLogAuxPath);
                }
            }
            catch (const std::exception& e)
            {
                //std::cout << e.what();
            }
        }


        std::vector<spdlog::sink_ptr> sinks;
        if (vLogCons) {
            auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
            console_sink->set_level(vLevel);
            console_sink->set_pattern("[%c] [%^%l%$] %v");
            sinks.push_back(console_sink);
        }
        if (vLogFile && a_LogFile != "") {
            auto file_sink =
                std::make_shared<spdlog::sinks::basic_file_sink_mt>(a_LogFile, true);
            file_sink->set_level(vLevel);
            file_sink->set_pattern("[%c %l] %v");
            sinks.push_back(file_sink);           
            fs::path vPath(a_LogFile);
            vName = vPath.stem().string();                       
        }   

        if (vLogAuxFile && vLogAuxPath !="") {
            fs::path vLogFileSrv = vLogAuxPath;
            std::time_t time = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
            char timeString[std::size("yyyy-mm-dd hh-mm-ss")];
            std::strftime(std::data(timeString), std::size(timeString),
                "%F %H-%M-%S", std::localtime(&time));
            std::string vAuxName = " " + (std::string)timeString;
            vLogFileSrv /= vName + vAuxName + ".log";
            auto fileSrv_sink =
                std::make_shared<spdlog::sinks::basic_file_sink_mt>(vLogFileSrv.string(), true);
            fileSrv_sink->set_level(vLevel);
            fileSrv_sink->set_pattern("[%c %l] %v");
            sinks.push_back(fileSrv_sink);
        }

        auto logger = std::make_shared<spdlog::logger>(vName, sinks.begin(), sinks.end());   
        logger->set_level(vLevel);
        logger->flush_on(vFlushLevel);
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

