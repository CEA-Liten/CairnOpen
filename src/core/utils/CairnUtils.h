#ifndef CAIRNUTILS_H
#define CAIRNUTILS_H

#include "CairnCore_global.h"
#include <fstream>
#include <filesystem>
namespace fs = std::filesystem;
typedef std::map<std::string, std::string> t_mapParams;

namespace  CairnUtils {
	//Generate a timestamp.
	int generateTimeStamp(); 

	//Add a timestamp to aFileName. 
	std::string addTimeStampToFileName(std::string aFullFileName); 

	//Open aFileOut for writing. If aFileOut cannot be accessed, add a timestamp to the file name.
	bool openFileForWriting(std::fstream& aFileOut, const std::string &a_FileName, std::ios::openmode a_openMode = std::ios::out);

	//write a line in PLAN or HIST file	
	void outputIndicator(std::fstream& out, const std::string compoName, const std::string indicatorName, const double value, 
		const std::string unit, const std::string alias, const std::string Description = "N/A", const std::vector<std::string>& labels = {});


	bool CAIRNCORESHARED_EXPORT contains(const std::vector< std::string>& a_List, const std::string& a_Find);

	bool CAIRNCORESHARED_EXPORT contains(const std::string &a_string, const std::string& a_Find, bool a_toUpper = false);
	bool contains(const std::string &a_string, const std::vector<std::string>& a_FindOneInList, bool a_toUpper = false);
	std::string join(const std::vector< std::string>& a_List);

	std::string replace(std::string& a_string, const std::string& a_find, const std::string& a_replace, bool a_toUpper = false);
	std::string toUpper(const std::string& a_string);
	std::string CAIRNCORESHARED_EXPORT simplified(const std::string& a_string);

	std::vector<std::string> CAIRNCORESHARED_EXPORT split(const std::string& a_string, const char& a_separator = ',');
	std::vector<std::string> CAIRNCORESHARED_EXPORT split(const std::string& a_string, const std::string & a_separator);


	std::string BuildFileName(const std::string &aFileName);
	std::vector<std::vector<std::string>> CAIRNCORESHARED_EXPORT readFromCsvFile(const std::string& aFileName, const std::string& sep = ";");
	std::vector<std::vector<std::string>> readToList(const std::string& Full_File_Name, const std::string& Separator);
	std::vector<double> getDataArray(const std::vector<std::vector<std::string>>& data_Inputs, int aCol, int iskipHead);

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
	std::string upperCase(const std::string& str);

	

}

#endif //CAIRNUTILS_H