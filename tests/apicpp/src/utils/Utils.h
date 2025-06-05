#include "..\TEST_PerseeLib.h"
#include <iostream>
#include <list>
#include <string>
#include <vector>
#include <utility> // for std::pair
#include <fstream>
#include <sstream>
#include <vector>
#include <cstdlib>
#include<cstdio>
#include <map>
#include <iomanip>

#include <ctime> 

#include <filesystem>
namespace fs = std::filesystem;
enum ECodeError {
    noError = 0,
    errSize = 1,  //File Size Error(size not equal - empty)
    errCompare = 2,  // Error in the compare Method - Not Equal
    noFindCmp = 3,  // composant non trouvé
    errSet = 4,
    errType = 5 //parameter type is not supported
};

enum EParamType {
    eDouble = 0,
    eInt,
    eBool,
    eString,
    eStringList
};

//TODO: find a better way to compare two csv files
struct tokens : std::ctype<char>
{
    tokens() : std::ctype<char>(get_table()) {}

    static std::ctype_base::mask const* get_table()
    {
        typedef std::ctype<char> cctype;
        static const cctype::mask* const_rc = cctype::classic_table();

        static cctype::mask rc[cctype::table_size];
        std::memcpy(rc, const_rc, cctype::table_size * sizeof(cctype::mask));

        rc[';'] = std::ctype_base::space;
        rc[','] = std::ctype_base::space;
        return &rc[0];
    }
};

#define TESTAPI(name, code) \
try { \
    code; \
} \
catch (std::exception& error) { \
    cout << "Error method " << name << ", " << error.what() << endl; \
    return errSet;  \
} \

#define TESTAPIFALSE(name, code) \
try { \
    code; \
    cout << "Error method " << name << ", must be in error" << endl; \
    return errSet;  \
} \
catch (std::exception& error) {} \

#define TESTAPI2(name, code) \
	if (code) { cout << "Error method " << name << endl; return errSet; }

// Enum to represent log levels 
enum LogLevel { DEBUG, INFO, WARNING, ERROR, CRITICAL };

using namespace std;

class TestUtils {
public:
	static void Display_list(const t_list InputList, const string& a_Title = "");
	static void Display_Vector(vector<vector<string>> InputVector);
	static vector<vector<string>> ParserTxt(const string& cheminFichier);
    static int compare_scalar(const t_value& val, const t_value& ref, const EParamType& type);
	static int compare_lists(t_list InputList, t_list RefList);
	static int CreateRefrenceList(const vector<vector<string>>& DataRef, t_list& OutputSolverList);
	static map<string, string> ParseDictionaryFile(const string& cheminFichier);
	static string SearchValueInDict(const string& filePath, const string& PortName);
	static int ComparePortValue(const std::string& a_ComponentName, const std::string& InputPortValue);
	static void Display_Dict(std::map<std::string, std::string>& dict);
	static int compare_dict(std::map<std::string, std::string> Inputdict, std::map<std::string, std::string> Refdict);
	static int ReadNameParamCsvFile(const std::string filename, t_list csvData);
	static int Identify_Type(const std::string& str1);
	static map<string, string> ParsingDictionaryFromFile(const string& cheminFichierDict);
	static int SearchAndDeleteFromDict(t_dict& TargetDict, const string& TargetKeyToBeDeleted);
	static void updateElementInDict(map <string, string>& givenMap, const string givenKey, const string newValue);
	static int CheckTestStatus(t_list gtl_TestStatusList);
	static int ComparePortValue(const string& ValueToBeCompared, const string& ComponentName, const string& PortName, const string& PortAttribut, vector<vector<string>>& PortValueReferenceList);
    static std::vector<std::string> readCSV(const std::string& filename);
    static int ComparaisonCsvFile(string const CsvFilePath1, string const CsvFilePath2);
    static std::vector<std::string> str_to_vector(std::string& str);
    static map<std::string, std::map<std::string, std::string>> ReadDataFileWithIndex(const std::string& filePath);
    static string GetDataWithIndex(const std::map<std::string, std::map<std::string, std::string>>& data, const std::string& section, const std::string& key);
    static t_dict PreparePortSettings(const std::map<std::string, std::map<std::string, std::string>>& data, const std::string& section);
};

class Logger 
{
public:
    // Constructor: Opens the log file in append mode 
    Logger(const string& filename)
    {
        StoreLogFilePath(filename);

        logFile.open(filename, ios::app);
        if (!logFile.is_open()) {
            cerr << "Error opening log file." << endl;
        }
    }

    // Destructor: Closes the log file 
    ~Logger() { logFile.close(); }

    // Logs a message with a given log level 
    void log(LogLevel level, const string& message)
    {
        //Get the LogFilePath
        const string LogFilePath = getLogFilePath();

        // Get current timestamp 
        time_t now = time(0);
        tm* timeinfo = localtime(&now);
        char timestamp[20];
        strftime(timestamp, sizeof(timestamp),
            "%Y-%m-%d %H:%M:%S", timeinfo);

        // Create log entry 
        ostringstream logEntry;
        logEntry << "[" << timestamp << "] "
            << levelToString(level) << ": " << message
            << endl;

        // Output to console 
        cout << logEntry.str();

        // Output to log file 
        if (logFile.is_open())
        {
            logFile << logEntry.str();
            logFile.flush(); // Ensure immediate write to file 
        }
    }

    void StoreLogFilePath(const string& filename)
    {
        LogFilePath = filename;
    }

    string getLogFilePath()
    {
        return LogFilePath;
    }
private:
    ofstream logFile; // File stream for the log file 

    string LogFilePath;

    // Converts log level to a string for output 
    string levelToString(LogLevel level)
    {
        switch (level) {
        case DEBUG:
            return "DEBUG";
        case INFO:
            return "INFO";
        case WARNING:
            return "WARNING";
        case ERROR:
            return "ERROR";
        case CRITICAL:
            return "CRITICAL";
        default:
            return "UNKNOWN";
        }
    }
};