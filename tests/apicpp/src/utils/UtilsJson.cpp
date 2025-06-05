#include "UtilsJson.h"
#include <iomanip>

static vector<string> excludedElems = { "x", "y", "nodeId", "componentId", "tailNodeId", "tailSocketId", "headNodeId", "headSocketId"};

enum ECodeError {
	noError = 0,
	errSize = 1,  //File Size Error(size not equal - empty)
	errCompare = 2,  // Error in the compare Method - Not Equal
	noFindCmp = 3,  // composant non trouvé
};

int TestUtilsJson::CheckTestStatus(t_list gtl_TestStatusList)
{
	for (auto& vCmp : gtl_TestStatusList)
	{
		if (vCmp != "0")
		{
			return errSize;
		}
	}

	return noError;
}

bool TestUtilsJson::CompareJsonFiles(const std::string& file1, const std::string& file2)
{
	json jsonData1 = OpenJsonFile(file1);	
	if (jsonData1.is_null()) return false;
	json jsonData2 = OpenJsonFile(file2);
	if (jsonData2.is_null()) return false;

	// create the patch
	json patch = json::diff(jsonData1, jsonData2);
	
	// analyze patch
	json patchNew = json::array();
	for (auto& elem : patch) {
		if (elem["op"] == "replace") {
			if (!findInList(elem["path"], excludedElems)) {			
				// pas trouvé on l'ajoute
				patchNew.insert(patchNew.begin(), elem);
			}
		}
		else
			patchNew.insert(patchNew.begin(), elem);
	}
	std::cout << std::setw(4) << patchNew << std::endl;

	return (patchNew.size()==0);
}

json TestUtilsJson::OpenJsonFile(const std::string& filePath)
{
	json jsonData;
	// Open file
	std::ifstream inputFile(filePath);

	if (!inputFile.is_open())
	{
		std::cerr << "Open File Error " << filePath << std::endl;
		return {};
	}

	// Read file Data
	try
	{
		//creat json object
		inputFile >> jsonData;
	}
	catch (const std::exception& e)
	{
		std::cerr << "read json error: " << e.what() << std::endl;
		return {};
	}
	
	return jsonData;
}

bool TestUtilsJson::findInList(const string& a_str, const vector<string>& a_List)
{
	for (auto& vStr : a_List) {
		if (a_str.find(vStr) != string::npos) {
			// trouvé
			return true;
		}
	}
	return false;
}


void TestUtilsJson::Display_Json_Object(std::map<std::string, json> dict)
{
	std::map <std::string, json>::iterator itr;

	for (itr = dict.begin(); itr != dict.end(); ++itr)
	{
		std::cout << itr->first << ": " << itr->second << std::endl;
	}
}

void TestUtilsJson::Display_Json_File_Data(const std::vector<std::unordered_map<std::string, std::string>> parsedData)
{
	std::cout << "Data json file is :" << std::endl;
	for (const auto& it : parsedData)
	{
		for (const auto& data : it)
		{
			std::cout << data.first << data.second << std::endl;
		}
		std::cout << std::endl;
	}
}


bool TestUtilsJson::compareJSON(const json& json1, const json& json2)
{
	return json1 == json2;
}

std::map<std::string, json> TestUtilsJson::ParseandCompareJsonFromFile(const std::string& filePath)
{
	json jsonData;

	// Open file
	std::ifstream inputFile(filePath);

	if (!inputFile.is_open())
	{
		std::cerr << "Open File Error " << filePath << std::endl;
		return {};
	}

	// Read file Data

	try
	{
		//creat json object
		inputFile >> jsonData;

		json jf = json::parse(inputFile);
		std::string str(R"({"json": "beta"})");
		json js = json::parse(str);

	}
	catch (const std::exception& e)
	{
		std::cerr << "read json error: " << e.what() << std::endl;
		return {};
	}

	//return js
	return jsonData;
}

// Parse Json files Using Standard Lib
std::vector<std::unordered_map<std::string, std::string>> TestUtilsJson::ParseJsonFile(const std::string& filePath)
{
	std::vector<std::unordered_map<std::string, std::string>> data;

	// Ouverture du fichier JSON
	std::ifstream file(filePath);
	if (!file.is_open())
	{
		std::cerr << "Error Read file." << std::endl;
		return data;
	}

	try {
		// Read json file
		std::stringstream buffer;
		buffer << file.rdbuf();
		std::string jsonContent = buffer.str();

		// Parse  JSON
		size_t startPos = jsonContent.find_first_of("{[");
		size_t endPos = jsonContent.find_last_of("]}");
		std::string jsonBody = jsonContent.substr(startPos, endPos - startPos + 1);

		// Read json file
		std::stringstream ss(jsonBody);
		std::string line;
		while (std::getline(ss, line, '{'))
		{
			if (line.empty()) continue; // Ignor empty lines
			std::stringstream obj(line);
			std::string item;
			std::unordered_map<std::string, std::string> objData;
			while (std::getline(obj, item, ','))
			{
				if (item.empty()) continue; // Ignor empty lines
				size_t colonPos = item.find_first_of(":");
				std::string key = item.substr(0, colonPos);
				std::string value = item.substr(colonPos + 1);

				// Supprimer les guillemets autour des valeurs
				value.erase(0, value.find_first_not_of("\""));
				value.erase(value.find_last_not_of("\"") + 1);

				// Verify Var Type
				if (value.front() == '[' && value.back() == ']')
				{
					//array
					for (auto& vCmp : value)
					{
						objData[key] = vCmp;
					}

				}
				else if (value.front() == '{' && value.back() == '}')
				{
					// dictionnaire
					for (auto& vCmp : value)
					{
						objData[key] = vCmp;
					}
				}
				else
				{
					// simple
					objData[key] = value;
				}
				//objData[key] = value;
			}
			data.push_back(objData);
		}
	}
	catch (const std::exception& e)
	{
		std::cerr << "Error parsing file : " << e.what() << std::endl;
	}

	// close file
	file.close();

	return data;
}

//This function return all params of a setting type of a specific type name
t_list  TestUtilsJson::GetAllPossibleModelsNameFromJsonFiles(const std::string& InputFilejsonPath, const std::string& JsComponentName, const std::string& JsSettingsType)
{
	// Variables
	t_list result;
	std::ifstream inputFile(InputFilejsonPath);
	nlohmann::json j = nlohmann::json::parse(inputFile);

	if (j.is_null())
	{
		cout << "Error file json - File is empty" << endl;
		return result;
	}

	for (auto& value : j)
	{
		if (value.find("type") != value.end())
		{
			if (value["type"] == JsComponentName)
			{
				if (value.find(JsSettingsType) != value.end())
				{
					for (const auto& ligne : value[JsSettingsType])
					{
						if (ligne.find("name") != ligne.end())
						{
							result.push_back(ligne["name"]);
						}
					}
				}
				return result;
			}
		}
		else
		{
			cout << JsSettingsType << " of the Component " << JsComponentName << " does not exist" << endl;
		}
	}

	return result;
}


// This function return all type names in a json file
t_list  TestUtilsJson::GetAllPossibleModelsTypeNameFromJsonFiles(const std::string& InputFilejsonPath)
{
	//Variables
	t_list result;
	std::ifstream inputFile(InputFilejsonPath);
	nlohmann::json j = nlohmann::json::parse(inputFile);

	if (j.is_null())
	{
		cout << "Error file json - File is empty" << endl;
		return result;
	}

	
	cout << "These are the Full List of Type Names in the json File : " << endl;
	
	for (auto& value : j)
	{
		if (value.find("type") != value.end())
		{
			std::cout << value["type"] << std::endl;
			result.push_back(value["type"]);
		}
		else
		{
			cout <<"the Type Names does not exist in this File" << endl;
		}
	}

	return result;
}


// Read and convert file json to json object
json  TestUtilsJson::readJSON(const std::string& filePath)
{
	std::ifstream file(filePath);
	if (!file.is_open())
	{
		std::cerr << "Error open file : " << filePath << std::endl;
		return json();
	}

	json j;
	file >> j;
	file.close();
	return j;
}

// Recursive Method to find diff between json objects
void  TestUtilsJson::findDifferences(const json& json1, const json& json2, const std::string& path, std::vector<Difference>& differences)
{
	if (json1.type() != json2.type())
	{
		differences.push_back({ path, json1, json2 });
		return;
	}

	if (json1.is_object())
	{
		for (json::const_iterator it = json1.begin(); it != json1.end(); ++it)
		{
			std::string newPath = path + "/" + it.key();
			if (json2.contains(it.key()))
			{
				findDifferences(it.value(), json2.at(it.key()), newPath, differences);
			}
			else
			{
				differences.push_back({ newPath, it.value(), nullptr });
			}
		}
		for (json::const_iterator it = json2.begin(); it != json2.end(); ++it)
		{
			std::string newPath = path + "/" + it.key();
			if (!json1.contains(it.key()))
			{
				differences.push_back({ newPath, nullptr, it.value() });
			}
		}
	}
	else if (json1.is_array())
	{
		size_t maxSize = std::max(json1.size(), json2.size());
		for (size_t i = 0; i < maxSize; ++i)
		{
			std::string newPath = path + "/" + std::to_string(i);
			if (i < json1.size() && i < json2.size())
			{
				findDifferences(json1[i], json2[i], newPath, differences);
			}
			else if (i < json1.size())
			{
				differences.push_back({ newPath, json1[i], nullptr });
			}
			else
			{
				differences.push_back({ newPath, nullptr, json2[i] });
			}
		}
	}
	else if (json1 != json2)
	{
		differences.push_back({ path, json1, json2 });
	}
}

