#include "TEST_CairnCore.h"
#include <iostream>
#include <list>
#include <string>
#include <vector>
#include <utility> 
#include <fstream>
#include <sstream>
#include <cstdlib>
#include<cstdio>
#include <map>
#include <iomanip>
#include <cstring>
#include <ctime> 
#include <algorithm>
#include <iterator>
#include <math.h>   
#include <filesystem>
namespace fs = std::filesystem;

constexpr auto CSV_SEPARATOR = ';';

static bool starts_with(std::string_view str, std::string_view prefix)
{
    return str.size() >= prefix.size() && str.compare(0, prefix.size(), prefix) == 0;
}

enum ECodeError {
    noError = 0,
    errSize = 1,      // file Size Error(size not equal - empty)
    errCompare = 2,   // error in the compare Method - Not Equal
    noFindCmp = 3,    // composant non trouvé
    errSet = 4,
    errType = 5       // parameter type is not supported
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
    /* fail in case of error */ \
    try { \
        cout << "--- Test " << name << endl; \
        code; \
    } \
    catch (std::exception& error) { \
        cout << "Error test " << name << ", " << error.what() << endl; \
        return errSet;  \
    } \

#define TESTAPIFALSE(name, code) \
    /* fail in case of No error */ \
    try { \
        cout << "--- Test " << name << endl; \
        code; \
        cout << "Error test " << name << ", must be in error" << endl; \
        return errSet;  \
    } \
    catch (std::exception& error) {} \

#define TESTAPIBOOL(name, code) \
    /* fail if false */ \
    cout << "--- Test " << name << endl; \
	if (!(code)) { cout << "Error test " << name << endl; return errSet; }

#define TESTAPIBOOLFALSE(name, code) \
    /* fail if true */ \
    cout << "--- Test " << name << endl; \
	if (code) { cout << "Error test " << name << endl; return errSet; }

#define TESTVALUE(var, ref) \
		if (fabsf(var - ref)>1e-6) return 1; \

#define TESTVALUEFALSE(var, ref) \
		if (fabsf(var - ref)<1e-6) return 1; \

// Enum to represent log levels 
enum LogLevel { DEBUG, INFO, WARNING, ERROR, CRITICAL };

using namespace std;

class TestUtils {
public:
    // --- Comparison helpers (return bool) ---
    static bool compare_scalar(const t_value& val, const t_value& ref, EParamType type);
    static bool compare_lists(const t_list& inputList, const t_list& refList);
    static bool compare_dict(const t_dict& inputDict, const t_dict& refDict);
    static bool contains(const t_list& inputList, const std::string& val);
    static bool CreateReferenceList(const std::vector<t_list>& dataRef, t_list& outputSolverList);

    // --- Converters / formatters ---
    static std::string valueToString(const t_value& value);

    // --- CSV / file helpers ---
    static t_list readCSV(const std::string& filename);
    static t_list parseLineCSV(const std::string& a_line, char a_sep = CSV_SEPARATOR);
    static vector<t_list> ParserTxt(const string& cheminFichier);
    static bool ComparaisonCsvFile(const std::string& csvFilePath1, const std::string& csvFilePath2);

    // --- Display / utilities ---
    static void Display_list(const t_list& inputList, const std::string& title = "");
    static void Display_Vector(const std::vector<t_list>& inputVector);
    static void skipUTF8BOM(std::istream& in);
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



class CSVRow
{
public:
    double operator[](std::size_t index) const
    {
        size_t vIndex = (size_t)m_data[index] + 1;
        std::string str(&m_line[vIndex], (size_t)m_data[index + 1] - vIndex);
        return std::stof(str);
    }
    std::string operator()(std::size_t index) const
    {
        size_t vIndex = (size_t)m_data[index] + 1;
        std::string str(&m_line[vIndex], (size_t)m_data[index + 1] - vIndex);
        return str;
    }
    std::size_t size() const
    {
        return m_data.size() - 1;
    }
    void readNextRow(std::istream& str)
    {
        std::getline(str, m_line);

        m_data.clear();
        m_data.emplace_back(-1);
        std::string::size_type pos = 0;
        while ((pos = m_line.find(CSV_SEPARATOR, pos)) != std::string::npos)
        {
            m_data.emplace_back(pos);
            ++pos;
        }
        // This checks for a trailing comma with no data after it.
        pos = m_line.size();
        m_data.emplace_back(pos);
    }
private:
    std::string            m_line;
    std::vector<size_t>    m_data;
};

std::istream& operator>>(std::istream& str, CSVRow& data);

class CSVIterator
{
public:
    typedef std::input_iterator_tag     iterator_category;
    typedef CSVRow                      value_type;
    typedef std::size_t                 difference_type;
    typedef CSVRow* pointer;
    typedef CSVRow& reference;

    CSVIterator(std::istream& str) :m_str(str.good() ? &str : nullptr) { ++(*this); }
    CSVIterator() :m_str(nullptr) {}

    // Pre Increment
    CSVIterator& operator++() { if (m_str) { if (!((*m_str) >> m_row)) { m_str = nullptr; } }return *this; }
    // Post increment
    CSVIterator operator++(int) { CSVIterator    tmp(*this); ++(*this); return tmp; }
    CSVRow const& operator*()   const { return m_row; }
    CSVRow const* operator->()  const { return &m_row; }

    bool operator==(CSVIterator const& rhs) { return ((this == &rhs) || ((this->m_str == nullptr) && (rhs.m_str == nullptr))); }
    bool operator!=(CSVIterator const& rhs) { return !((*this) == rhs); }


private:
    std::istream* m_str;
    CSVRow              m_row;
};

class CSVRange
{
    std::istream& stream;
public:
    CSVRange(std::istream& str)
        : stream(str)
    {
    }
    CSVIterator begin() const { return CSVIterator{ stream }; }
    CSVIterator end()   const { return CSVIterator{}; }
};


std::ostream& operator<<(std::ostream& str, const std::vector<std::string>& data);
std::ostream& operator<<(std::ostream& str, const std::vector<double>& data);
std::ostream& operator<<(std::ostream& str, const std::vector<t_value>& data);