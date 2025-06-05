#include "..\TEST_PerseeLib.h"
#include "json.hpp"
#include <string>
#include <iostream>
#include <fstream>
#include <list>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <cstdlib>
#include<cstdio>
#include <map>
#include <unordered_map>

using json = nlohmann::json;

using namespace std;

// Structure for the Diff between Json Files
struct Difference {
	std::string path;
	json value1;
	json value2;
};

class TestUtilsJson {
public:
	static bool CompareJsonFiles(const std::string& file1, const std::string& file2);

	static json OpenJsonFile(const std::string& filePath);

	static std::vector<std::unordered_map<std::string, std::string>> TestUtilsJson::ParseJsonFile(const std::string& filePath);
	static std::map<std::string, json> TestUtilsJson::ParseandCompareJsonFromFile(const std::string& filePath);
	static bool TestUtilsJson::compareJSON(const json& json1, const json& json2);
	static void TestUtilsJson::Display_Json_File_Data(const std::vector<std::unordered_map<std::string, std::string>> parsedData);
	static void TestUtilsJson::Display_Json_Object(std::map<std::string, json> dict);
	static t_list  TestUtilsJson::GetAllPossibleModelsNameFromJsonFiles(const std::string& InputFilejsonPath, const std::string& JsComponentName, const std::string& JsSettingsType);
	static t_list  TestUtilsJson::GetAllPossibleModelsTypeNameFromJsonFiles(const std::string& InputFilejsonPath);
	static json  TestUtilsJson::readJSON(const std::string& filePath);
	static void  TestUtilsJson::findDifferences(const json& json1, const json& json2, const std::string& path, std::vector<Difference>& differences);
	
	
	
	static int TestUtilsJson::CheckTestStatus(t_list gtl_TestStatusList);
protected:
	static bool findInList(const string& a_str, const vector<string>& a_List);
};