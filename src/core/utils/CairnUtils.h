#ifndef CAIRNUTILS_H
#define CAIRNUTILS_H

#include <math.h>   
#include <iostream>
#include <fstream>
#include <filesystem>
#include <algorithm>
#include <charconv>
#include <codecvt>
#include <locale>

#include <Eigen/SparseCore>
#include <Eigen/Dense>

#include "CairnCore_global.h"
#include "ModelParam.h"

#include "Cairn_Exception.h"

namespace fs = std::filesystem;
typedef std::map<std::string, std::string> t_mapParams;

using Eigen::VectorXf;
using Eigen::MatrixXf;

struct ParameterRow {
	std::string component;
	std::string parameter;
	std::string value;
	std::string unit;
	std::string description;
	bool mandatory;
	std::map<std::string, std::string> labels;
	std::map<std::string, std::string> extraData;
};

struct ExportParameterData {
	std::vector<std::string> labelHeaders;
	std::vector<std::string> extraHeaders;
	std::vector<ParameterRow> rows;
};

struct GradDescResult {    
    std::vector<MatrixXf> X;  
    std::vector<double> Y;
    double gap;
    int iteration;
    bool condition;
} typedef GradDescResult;

namespace  CairnUtils {
	//Generate a timestamp.
	int generateTimeStamp(); 

	//Add a timestamp to aFileName. 
	std::string addTimeStampToFileName(std::string aFullFileName); 

	//Open aFileOut for writing. If aFileOut cannot be accessed, add a timestamp to the file name.
	bool openFileForWriting(std::fstream& aFileOut, const std::string &a_FileName, std::ios::openmode a_openMode = std::ios::out);

	// Parse indicator name
	std::string parseIndicatorName(const std::string& indicatorName, const bool isSizeOptimized = false);
	
	//write a line in PLAN or HIST file	
	void outputIndicator(std::fstream& out, const std::string compoName, const std::string indicatorName, const double value, 
		const std::string unit, const std::string alias, const std::string Description = "N/A", const std::vector<std::string>& labels = {});

	std::string toLower(const std::string& a_string);
	std::string toUpper(const std::string& a_string);
	std::string remove_spaces(const std::string& a_string);
	std::string trim(const std::string& a_string);
	std::string CAIRNCORESHARED_EXPORT simplified(const std::string& a_string);
	std::string to_string_trim(const double& num, int sig = 15);
	std::string replace(std::string& a_string, const std::string& a_find, const std::string& a_replace, bool a_toUpper = false);
	std::string joinStrings(const std::vector< std::string>& a_List, const std::string& a_separator = ",");

	bool contains(const std::string& a_string, const std::vector<std::string>& a_FindOneInList, bool a_toUpper = false);
	bool CAIRNCORESHARED_EXPORT contains(const std::vector< std::string>& a_List, const std::string& a_Find);
	bool CAIRNCORESHARED_EXPORT contains(const std::string &a_string, const std::string& a_Find, bool a_toUpper = false);

	void CAIRNCORESHARED_EXPORT removeMatchingSubstring(std::vector<std::string>& list, const std::string& substring);

	std::vector<std::string> CAIRNCORESHARED_EXPORT split(const std::string& a_string, const char& a_separator = ',');
	std::vector<std::string> CAIRNCORESHARED_EXPORT split(const std::string& a_string, const std::string & a_separator);

	std::string BuildFileName(const std::string& aFileName);
	std::string BuildFileName_W(const std::wstring &aFileName);

	std::vector<std::vector<std::string>> CAIRNCORESHARED_EXPORT readFromCsvFile(const std::string& aFileName, const std::string& sep = ";");
	std::vector<std::vector<std::string>> CAIRNCORESHARED_EXPORT readFromCsvFile_W(const std::wstring& aFileName, const std::string& sep = ";");

	std::vector<std::vector<std::string>> readToList(const std::string& Full_File_Name, const std::string& Separator);
	std::vector<double> getDataArray(const std::vector<std::vector<std::string>>& data_Inputs, int aCol, int iskipHead);

	inline std::wstring toWString(const std::string& s)
	{
#pragma warning(push)
#pragma warning(disable: 4996)
		std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
#pragma warning(pop)
		return converter.from_bytes(s);
	}

	inline std::vector<std::wstring> toWStringList(const std::vector<std::string>& list)
	{
		std::vector<std::wstring> wlist;
		wlist.reserve(list.size());
		for (const std::string& s : list) {
			wlist.push_back(toWString(s));
		}
		return wlist;
	}

	inline std::string toUTF8String(const std::wstring& ws)
	{
#pragma warning(push)
#pragma warning(disable: 4996)
		std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
#pragma warning(pop)
		return converter.to_bytes(ws);
	}

	static inline std::string getParam(const t_mapParams& a_Params, const std::string& a_Name) {
		try
		{
			return a_Params.at(a_Name);
		}
		catch (const std::exception&)
		{
			return "";
		}
	}

	inline void ltrim(std::string& s) {
		s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) {
			return !std::isspace(ch);
			}));
	}

	inline void rtrim(std::string& s) {
		s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) {
			return !std::isspace(ch);
			}).base(), s.end());
	}

    /* Methods related to the levelization and discount factor computations */
    int Newton(const double& aValue, const uint& aIarg, uint aOffset,
        const double aExtrapolateOverYear, double& X_Set,
        double (*Y_func)(const double&, const uint&, unsigned int, const double),
        bool (*Y_Test)(const double&, const double&));

    bool levelization_test(const double& aSum, const double& aValue);

    double levelization(const double& aDiscountRate, const unsigned int& Nyear, 
        unsigned int aOffset, const double aExtrapolateOverYear);
    
    double discountRate(const double& aDiscountFactor, const unsigned int& aNyear,
        unsigned int aOffset, const double aExtrapolateOverYear);
    
    double levelization(const double  aDiscountRate, const unsigned int Nyear,
        const unsigned int aAbsoluteCurrentTimeStep, const unsigned int aNbYearInput,
        const unsigned int aLeapYearPos, unsigned int aOffset, const double aExtrapolateOverYear);
    
    std::vector<double> levelizationTable(const double  aDiscountRate, const unsigned int Nyear,
        const unsigned int aNbYearInput, const unsigned int aLeapYearPos, unsigned int aOffset,
        const double aExtrapolateOverYear);

    std::vector<double> yearHourTable(unsigned int aNbYearInput, unsigned int aLeapYearPos);

    /* Methods related to gradient matrix computation */
    double Energy(MatrixXf* positions, MatrixXf* distances);
    
    MatrixXf gradient(double (*Y_func)(MatrixXf*, MatrixXf*), MatrixXf* pos, MatrixXf* param, double* dx);

	GradDescResult GradientDescent(double (*Y_func)(MatrixXf*, MatrixXf*),
		MatrixXf* init, MatrixXf* param, int nbMaxIterations, double gapStop, 
		double dx, double alpha);

    /* ------------------------------------------------------ */

	inline void writeUTF8BOM(std::ostream& out)
	{
		static const unsigned char UTF8_BOM[] = { 0xEF, 0xBB, 0xBF };
		out.write(reinterpret_cast<const char*>(UTF8_BOM), sizeof(UTF8_BOM));
	}

	void collectParameters(
		std::vector<ParameterRow>& rows,
		const std::string& componentName,
		const std::map<std::string, ModelParam*>& paramMap,
		const std::map<std::string, bool>& optionsMap = {},
		const std::map<std::string, std::string>& timeSeriesNames = {},
		const std::vector<std::string>& labelList = {},
		const std::map<std::string, std::string>& labelValueMap = {});

	void writeParameterDataToCSV(
		std::ostream& out,
		const ExportParameterData& data,
		const std::map<std::string, bool>& optionsMap);

	static std::string detectBOM(std::ifstream& file)
	{
		char bom[3] = { 0 };
		file.read(bom, 3);

		if (bom[0] == '\xEF' && bom[1] == '\xBB' && bom[2] == '\xBF')
		{
			return "UTF-8 BOM"; // skip BOM — file position already past it
		}
		else if (bom[0] == '\xFF' && bom[1] == '\xFE')
		{
			file.seekg(2); // UTF-16 LE BOM is 2 bytes
			return "UTF-16 LE";
		}
		else if (bom[0] == '\xFE' && bom[1] == '\xFF')
		{
			file.seekg(2); // UTF-16 BE BOM is 2 bytes
			return "UTF-16 BE";
		}
		else
		{
			file.seekg(0); // no BOM — rewind to start
			return "no BOM detected (assuming Windows-1252)";
		}
	}
	// Windows-1252 characters in the 0x80-0x9F range that differ from ISO-8859-1
	static const std::unordered_map<unsigned char, std::string> windows1252ExtraChars = {
		{0x80, "\xE2\x82\xAC"}, // €
		{0x82, "\xE2\x80\x9A"}, // ‚
		{0x83, "\xC6\x92"},     // ƒ
		{0x84, "\xE2\x80\x9E"}, // „
		{0x85, "\xE2\x80\xA6"}, // …
		{0x86, "\xE2\x80\xA0"}, // †
		{0x87, "\xE2\x80\xA1"}, // ‡
		{0x88, "\xCB\x86"},     // ˆ
		{0x89, "\xE2\x80\xB0"}, // ‰
		{0x8A, "\xC5\xA0"},     // Š
		{0x8B, "\xE2\x80\xB9"}, // ‹
		{0x8C, "\xC5\x92"},     // Œ
		{0x8E, "\xC5\xBD"},     // Ž
		{0x91, "\xE2\x80\x98"}, // '
		{0x92, "\xE2\x80\x99"}, // '
		{0x93, "\xE2\x80\x9C"}, // "
		{0x94, "\xE2\x80\x9D"}, // "
		{0x95, "\xE2\x80\xA2"}, // •
		{0x96, "\xE2\x80\x93"}, // –
		{0x97, "\xE2\x80\x94"}, // —
		{0x98, "\xCB\x9C"},     // ˜
		{0x99, "\xE2\x84\xA2"}, // ™
		{0x9A, "\xC5\xA1"},     // š
		{0x9B, "\xE2\x80\xBA"}, // ›
		{0x9C, "\xC5\x93"},     // œ
		{0x9E, "\xC5\xBE"},     // ž
		{0x9F, "\xC5\xB8"},     // Ÿ
	};

	// Convert Windows-1252 encoded string to UTF-8
	static std::string windows1252ToUtf8(const std::string& input)
	{
		std::string result;
		result.reserve(input.size()); // reserve at least input size

		for (unsigned char c : input)
		{
			if (c < 0x80)
			{
				// ASCII range — identical in all encodings
				result += static_cast<char>(c);
			}
			else if (c >= 0xA0)
			{
				// Latin-1 supplement (0xA0-0xFF) — same in Windows-1252 and ISO-8859-1
				// Convert to UTF-8: two bytes 0xC2/0xC3 + continuation byte
				result += static_cast<char>(0xC0 | (c >> 6));
				result += static_cast<char>(0x80 | (c & 0x3F));
			}
			else
			{
				// Windows-1252 special range (0x80-0x9F)
				const auto it = windows1252ExtraChars.find(c);
				if (it != windows1252ExtraChars.end())
					result += it->second;
				// else: undefined in Windows-1252, skip
			}
		}
		return result;
	}

	// Check if a string is valid UTF-8
	static bool isUtf8(const std::string& input)
	{
		int continuation = 0;
		for (unsigned char c : input)
		{
			if (continuation > 0)
			{
				if ((c & 0xC0) != 0x80)
					return false; // expected continuation byte
				--continuation;
			}
			else if (c < 0x80)  continuation = 0; // ASCII
			else if (c < 0xC0)  return false;      // unexpected continuation byte
			else if (c < 0xE0)  continuation = 1; // 2-byte sequence
			else if (c < 0xF0)  continuation = 2; // 3-byte sequence
			else if (c < 0xF8)  continuation = 3; // 4-byte sequence
			else                return false;      // invalid
		}
		return continuation == 0;
	}

	// Normalize to UTF-8 — detects encoding and converts if needed
	static std::string normalizeToUtf8(const std::string& input)
	{
		if (isUtf8(input))
			return input;
		return windows1252ToUtf8(input); // fallback to Windows-1252
	}

	// Compare two strings regardless of encoding
	static bool compareStrings(const std::string& s1, const std::string& s2)
	{
		if (s1 == s2) {
			return true;
		}
		return normalizeToUtf8(s1) == normalizeToUtf8(s2);
	}
	/* ------------------------------------------------------ */

	inline static void checkRead(int err, const std::string& context)
	{
		if (err < 0) {
			throw Cairn_Exception(
				"Error while initializing " + context +
				". A mandatory parameter is missing!",
				-1
			);
		}
	}
}

#endif //CAIRNUTILS_H