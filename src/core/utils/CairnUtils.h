#ifndef CAIRNUTILS_H
#define CAIRNUTILS_H

#include <QDebug>
#include <QString>
#include <QFile>
#include <QFileInfo>
#include <QDir>
#include <QIODevice>
#include <QTextStream>

#include <fstream>
#include <filesystem>
namespace fs = std::filesystem;

namespace CairnUtils {
	//Generate a timestamp.
	int generateTimeStamp(); 

	//Add a timestamp to aFileName. 
	std::string addTimeStampToFileName(std::string aFileName); 

	//Open aFileOut for writing. If aFileOut cannot be accessed, add a timestamp to the file name.
	bool openFileForWriting(std::fstream& aFileOut, const std::string &a_FileName, std::ios::openmode a_openMode = std::ios::out);

	//write a line in PLAN or HIST file
	//scalar 
	void outputIndicator(std::fstream& out, const QString compoName, const QString indicatorName, const double value, const QString unit, const QString alias);
	//list
	void outputIndicator(std::fstream& out, const QString compoName, const QString indicatorName, const QVector<double> list_values, const QString unit, const QString alias);

	void printInfoParam(const QString& aParamName, const bool IsBlocking, const QString& aUnit, const QString& aDescription);
	void printMissingParam(const QString& aParamName, const QString& aValue);
	void resetInfoParam();
	void resetMissingParam();

	//void jsonSaveGUIModelParams(QJsonArray& paramArray, const QMap <std::string, std::string>& paramMapAll, const QList<std::string>& paramNamesList);


	bool contains(const std::vector< std::string>& a_List, const std::string& a_Find);

	bool contains(const std::string &a_string, const std::string& a_Find, bool a_toUpper = false);
	bool contains(const std::string &a_string, const std::vector<std::string>& a_FindOneInList, bool a_toUpper = false);
	std::string join(const std::vector< std::string>& a_List);

	std::string replace(std::string& a_string, const std::string& a_find, const std::string& a_replace, bool a_toUpper = false);
	std::string toUpper(const std::string& a_string);

	std::vector<std::string> split(const std::string& a_string, const char& a_separator = ',');



	std::string BuildFileName(const std::string &aFileName);
	std::vector<std::vector<std::string>> readFromCsvFile(const std::string& aFileName, const std::string& sep = ";");
	std::vector<std::vector<std::string>> readToList(const std::string& Full_File_Name, const std::string& Separator);
	std::vector<double> getDataArray(const std::vector<std::vector<std::string>>& data_Inputs, int aCol, int iskipHead);

}

#endif //CAIRNUTILS_H