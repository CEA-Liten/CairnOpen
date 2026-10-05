#ifndef CAIRNLOGGER_H
#define CAIRNLOGGER_H
#if defined(_MSC_VER)
#define EXPORT __declspec(dllexport)
#define IMPORT __declspec(dllimport)
#elif defined(__GNUC__)
//  GCC
#define EXPORT __attribute__((visibility("default")))
#define IMPORT
#else
//  do nothing and hope for the best?
#define EXPORT
#define IMPORT
#pragma warning Unknown dynamic link import/export semantics.
#endif

#if defined(CAIRNCORE_LIBRARY)
#  define CAIRNLOGGERSHARED_EXPORT EXPORT
#else
#  define CAIRNLOGGERSHARED_EXPORT IMPORT
#endif


#include <string>
#include <vector>
#include "spdlog/spdlog.h"
#include "spdlog/sinks/dist_sink.h"
#include "ErrorSink.h"
#include "ObjectLogScope.h"
#include "CairnAPI.h"

namespace CairnLogger {

	void CAIRNLOGGERSHARED_EXPORT CreateLogger();
	void CAIRNLOGGERSHARED_EXPORT CreateLogger(bool a_LogCons, const std::string& a_LogFile = "");
	void CAIRNLOGGERSHARED_EXPORT CreateLogger(const t_dict& a_DefLogs);
	std::shared_ptr<spdlog::logger> CAIRNLOGGERSHARED_EXPORT GetDefaultLogger();

	void CAIRNLOGGERSHARED_EXPORT ChangeFileLogger(const std::string& a_LogFile);
	void CAIRNLOGGERSHARED_EXPORT Flush();

	class CAIRNLOGGERSHARED_EXPORT MessageLogger {
	public:
		MessageLogger();
		~MessageLogger();
		void log(const std::string& a_msg);
		void flush();
		
		MessageLogger& debug();
		MessageLogger& info();
		MessageLogger& warning();
		MessageLogger& error();
		MessageLogger& critical();
	private:
		int m_level;
		std::string m_msg;		
	};	

	class CAIRNLOGGERSHARED_EXPORT LoggerFactory {
	public:
		LoggerFactory();
		void CreateLogger(const t_dict& a_DefLogs);
		void ChangeFileLogger(const std::string& a_LogPath);

	private:			
		std::string m_name;
		bool m_LogCons;
		bool m_LogFile;
		std::string m_LogPath;
		bool m_LogAuxFile;
		std::string m_LogAuxPath;
		spdlog::level::level_enum m_Level;
		spdlog::level::level_enum m_FlushLevel;

		std::shared_ptr<spdlog::sinks::dist_sink_mt> m_dist_sink;

		std::vector<spdlog::sink_ptr> create_sinks();
		
		
	};
	static LoggerFactory loggerFactory;
}

CAIRNLOGGERSHARED_EXPORT CairnLogger::MessageLogger&  operator<<(CairnLogger::MessageLogger& os, const std::string& msg);
CAIRNLOGGERSHARED_EXPORT CairnLogger::MessageLogger& operator<<(CairnLogger::MessageLogger& os, const double& msg);
CAIRNLOGGERSHARED_EXPORT CairnLogger::MessageLogger& operator<<(CairnLogger::MessageLogger& os, const std::vector<double>& msg);

#define cDebug CairnLogger::MessageLogger().debug
#define cInfo CairnLogger::MessageLogger().info
#define cWarning CairnLogger::MessageLogger().warning
#define cError CairnLogger::MessageLogger().error
#define cCritical CairnLogger::MessageLogger().critical
#endif
