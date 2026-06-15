
#include "Utils.h"
#include <math.h>

enum TypeIdentiferCode {
	NONE      = 0,
	BOOLEAN   = 1,
	DOUBLE    = 2,
	CHAINE    = 3,
	ARRAY     = 4,
	OBJET     = 5,
	UnKnown   = 7,
	UNDEFINED = 8
};



int TestUtils::Identify_Type(const std::string& str1)
{
	if (str1 == "double")
	{
		std::cout << "Double\n\r" << std::endl;
		return DOUBLE;
	}
	else if (str1 == "bool")
	{
		std::cout << "Bool\n\r" << std::endl;
		return BOOLEAN;
	}
	else if (str1 == "array")
	{
		std::cout << "Array\n\r" << std::endl;
		return ARRAY;
	}
	else if (str1 == "string")
	{
		std::cout << "String\n\r" << std::endl;
		return CHAINE;
	}
	else if (str1 == "Object")
	{
		std::cout << "Object\n\r" << std::endl;
		return OBJET;
	}
	else if (str1 == "Undefined")
	{
		std::cout << "Undefined\n\r" << std::endl;
		return UNDEFINED;
	}
	else if (str1 == "Null")
	{
		std::cout << "Null\n\r" << std::endl;
		return NONE;
	}
	else
	{
		//default case
		std::cout << "Unknown\n\r" << std::endl;
		return UnKnown;
	}
}

int TestUtils::CheckTestStatus(t_list gtl_TestStatusList)
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

int TestUtils::ComparePortValue(const string& ValueToBeCompared, const string& ComponentName, const string& PortName, const string& PortAttribut, vector<vector<string>>& PortValueReferenceList)
{
	int liRet = errSize;

	for (auto& ReferenceValue : PortValueReferenceList)
	{
		if ((ComponentName == ReferenceValue[0]) && (PortName == ReferenceValue[1]) && (PortAttribut == ReferenceValue[2]))
		{
			if (ValueToBeCompared == ReferenceValue[3])
			{
				liRet = noError;
				break;
			}
			else
			{
				liRet = errSize;
				break;
			}
		}
	}
	return liRet;
}

void TestUtils::Display_list(const t_list InputList, const string &a_Title)
{
	if (a_Title != "") {
		std::cout << a_Title << std::endl;
	}
	for (auto& vCmp : InputList)
	{
		std::cout << "component: " << vCmp << std::endl;
	}
}

void TestUtils::Display_Vector(vector<vector<string>> InputVector)
{
	for (const auto& ligne : InputVector)
	{
		for (const auto& cell : ligne)
		{
			std::cout << cell << ",";
		}

		cout << std::endl;
	}
}


vector<vector<string>> TestUtils::ParserTxt(const string& cheminFichier)
{
	// Variables
	vector<vector<string>> donneesCSV;
	string ligne;

	// Open File
	ifstream fichier(cheminFichier);

	// Verify file status - Open / Not Open
	if (!fichier.is_open())
	{
		cerr << "Erreur : Impossible d'ouvrir le fichier " << cheminFichier << endl;
		// Return Empty List
		return donneesCSV;
	}

	// Read File - string Format
	stringstream buffer;
	buffer << fichier.rdbuf();
	string contenuFichier = buffer.str();

	// Close File
	fichier.close();

	istringstream fluxFichier(contenuFichier);

	// Parse File - Line-Line
	while (getline(fluxFichier, ligne))
	{
		// Read file Vars
		ligne.erase(std::remove(ligne.begin(), ligne.end(), '\r'), ligne.end());
		istringstream fluxLigne(ligne);
		string cellule;
		vector<string> ligneCSV;

		// Parse and Seperator ','
		while (getline(fluxLigne, cellule, ','))
		{
			ligneCSV.push_back(cellule);
		}

		// Push-Back data to Vector
		donneesCSV.push_back(ligneCSV);
	}

	// Retourner la liste de données CSV
	return donneesCSV;
}


//typedef std::variant<double, int, bool, std::string, std::vector<double>, std::vector<int>> t_value;
int TestUtils::compare_scalar(const t_value& val, const t_value& ref, const EParamType& type) {
	switch (type) {
	case eDouble:
		if (std::get<double>(val) == std::get<double>(ref)) {
			return noError;
		}
		else {
			return errCompare;
		}
		break;
	case eInt:
		if (std::get<int>(val) == std::get<int>(ref)) {
			return noError;
		}
		else {
			return errCompare;
		}
		break;
	case eBool:
		{
			bool b1 = std::get<int>(val);
			bool b2 = std::get<int>(ref);
			if (b1 == b2) {
				return noError;
			}
			else {
				return errCompare;
			}
			break;
		}
	case eString:
		if (std::get<std::string>(val) == std::get<std::string>(ref)) {
			return noError;
		}
		else {
			return errCompare;
		}
		break;
	case eStringList:
		{
			std::vector<std::string> val_list = std::get<std::vector<std::string>>(val);
			std::vector<std::string> ref_list = std::get<std::vector<std::string>>(ref);
			if (std::equal(val_list.begin(), val_list.end(), ref_list.begin())) {
				return noError;
			}
			else {
				return errCompare;
			}
			break;
		}
	default:
		return errType;
		break;
	}
}

int TestUtils::compare_lists(const t_list &InputList, const t_list &RefList)
{
	bool lbFound = false;
	int liDiffSize = 0;

	if (InputList.size() == RefList.size())
	{
		for (auto& Input : InputList)
		{
			lbFound = false;

			for( auto& Ref : RefList)
			{
				if (Input == Ref)
				{
					lbFound = true;
					break;
				}
			}
			if (lbFound != true)
			{
				//Test Not Ok - the list of settings doesn't conforms to the expected list
				cout << "Error in the compare - the Dict of component doesn't conforms to the expected list" << endl;
				cout << "Please check the reference List file - the difference between the two list is in this element : " << endl;
				cout << "This Input element not found in the reference list : " << Input << endl;
				return errCompare;
			}
		}

		//Test Ok - the list of settings conforms to the expected list
		return noError;
	}
	else
	{
		//Test Not Ok - the Size of list of settings does not conforms to the expected list
		liDiffSize = InputList.size() - RefList.size();
		cout << "There are " << abs(liDiffSize) << " elements are ";
		if (InputList.size() > RefList.size())
		{
			cout << "missing in the reference list" << endl;
		}
		else if (InputList.size() < RefList.size())
		{
			cout << "more in the reference list" << endl;
		}
		return errSize;
	}
}

int TestUtils::contains(const t_list& InputList, const std::string& val)
{
	/* Check if a list contains a given val */
	if (std::find(InputList.begin(), InputList.end(), val) != InputList.end()) {
		return noError;
	}
	return errCompare;
}

int TestUtils::CreateRefrenceList(const vector<vector<string>>& DataRef, t_list& OutputSolverList)
{
	for (const auto& ligne : DataRef)
	{
		for (const auto& cell : ligne)
		{
			OutputSolverList.push_back(cell);
		}
	}

	if (std::empty(OutputSolverList))
	{
		return errSize;
	}

	return noError;
}

void TestUtils::Display_Dict(std::map<std::string, std::string>& dict)
{
	std::map <std::string, std::string>::iterator itr;

	for (itr = dict.begin(); itr != dict.end(); ++itr)
	{
		std::cout << itr->first << ": " << itr->second << std::endl;
	}
}

int TestUtils::compare_dict(const t_dict& inputDict, const t_dict& refDict)
{
	// Check size compatibility
	if (inputDict.size() != refDict.size()) {
		int diffSize = static_cast<int>(inputDict.size()) - static_cast<int>(refDict.size());
		std::cout << "There are " << std::abs(diffSize) << " elements different between the two dicts" << std::endl;
		if (inputDict.size() > refDict.size()) {
			std::cout << "missing in the reference dict" << std::endl;
		}
		else {
			std::cout << "more in the reference dict" << std::endl;
		}
		return errSize;
	}

	// Compare each key-value pair
	for (const auto& [key, inputValue] : inputDict)
	{
		// Check if key exists in reference dict
		auto refIt = refDict.find(key);
		if (refIt == refDict.end()) {
			std::cout << "Error in the compare - the Dict of component doesn't conform to the expected dict" << std::endl;
			std::cout << "Please check the reference dict file:" << std::endl;
			std::cout << "This Input element: " << key << " is not found in the reference dict" << std::endl;
			return errCompare;
		}

		// Compare values
		const t_value& refValue = refIt->second;
		if (inputValue != refValue) {
			std::cout << "Error in the compare - the Dict of component doesn't conform to the expected dict" << std::endl;
			std::cout << "Please check the reference dict file - the two dicts have the same key but their values are different:" << std::endl;
			std::cout << "Key: " << key << std::endl;
			std::cout << "Input value: " << valueToString(inputValue) << std::endl;
			std::cout << "Reference value: " << valueToString(refValue) << std::endl;
			return errCompare;
		}
	}

	// All checks passed
	return noError;
}

// Helper function to convert t_value to string 
std::string TestUtils::valueToString(const t_value& value)
{
	return std::visit([](const auto& val) -> std::string {
		using T = std::decay_t<decltype(val)>;

		if constexpr (std::is_same_v<T, double>) {
			return std::to_string(val);
		}
		else if constexpr (std::is_same_v<T, int>) {
			return std::to_string(val);
		}
		else if constexpr (std::is_same_v<T, std::string>) {
			return val;
		}
		else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
			std::string result = "[";
			for (size_t i = 0; i < val.size(); ++i) {
				result += val[i];
				if (i < val.size() - 1) result += ", ";
			}
			result += "]";
			return result;
		}
		else if constexpr (std::is_same_v<T, std::vector<double>>) {
			std::string result = "[";
			for (size_t i = 0; i < val.size(); ++i) {
				result += std::to_string(val[i]);
				if (i < val.size() - 1) result += ", ";
			}
			result += "]";
			return result;
		}
		else if constexpr (std::is_same_v<T, std::vector<int>>) {
			std::string result = "[";
			for (size_t i = 0; i < val.size(); ++i) {
				result += std::to_string(val[i]);
				if (i < val.size() - 1) result += ", ";
			}
			result += "]";
			return result;
		}
		return "unknown";
		}, value);
}

int TestUtils::SearchAndDeleteFromDict(t_dict& TargetDict, const string& TargetKeyToBeDeleted)
{
	/*map<string, string>::iterator iter = TargetDict.find(TargetKeyToBeDeleted);

	if (iter != TargetDict.end())
	{
		TargetDict.erase(iter);
	}
	else
	{
		cout << "Key Not found in the Target Dict" << endl;
	}*/

	return 0;
}


void TestUtils::updateElementInDict(map <string, string>& givenMap, const string givenKey, const string newValue)
{
	map <string, string>::iterator itr;
	itr = givenMap.find(givenKey);
	if (itr != givenMap.end())
	{
		// when item has found
		itr->second = newValue;
	}
}


map<string, string> TestUtils::ParseDictionaryFile(const string& cheminFichier)
{
	map<string, string> dictionary;

	ifstream inputFileStream(cheminFichier);

	if (!inputFileStream.is_open())
	{
		cerr << "Erreur lors de la lecture du fichier " + cheminFichier << endl;
	}
	else
	{
		string line;
		while (getline(inputFileStream, line))
		{
			// search the first occ of "=" in the line
			size_t pos = line.find_first_of("=");

			// Extract key and Value
			if (pos != string::npos)
			{
				string key = line.substr(0, pos);
				string value = line.substr(pos + 1);

				// Insert key and value in the dict
				dictionary.insert(make_pair(key, value));
			}
		}
	}

	inputFileStream.close();
	return dictionary;
}

map<string, string> TestUtils::ParsingDictionaryFromFile(const string& cheminFichierDict)
{
	std::map<std::string, std::string> dictionary;

	// Open File
	std::ifstream file(cheminFichierDict);
	if (!file.is_open()) 
	{
		std::cerr << "Erreur lors de l'ouverture du fichier " << cheminFichierDict << std::endl;
		return dictionary; // Error - Return empty Dict
	}

	std::string line;

	// Read File Lines
	while (std::getline(file, line)) 
	{
		// Ignore empty lines or lines with '#'
		if (line.empty() || line[0] == '#') 
		{
			continue;
		}

		// Delete comments in the Line
		size_t commentPos = line.find('#');
		if (commentPos != std::string::npos) 
		{
			line.erase(commentPos);
		}

		std::istringstream iss(line);
		std::string key, value;

		// line :  Key = Value
		if (std::getline(iss, key, '=') && std::getline(iss, value))
		{
			// Delete space  " " from Key and Vale
			key.erase(std::remove_if(key.begin(), key.end(), ::isspace), key.end());
			value.erase(std::remove_if(value.begin(), value.end(), ::isspace), value.end());
			//Add key Value
			dictionary[key] = value; 
		}
	}

	return dictionary;
}

string TestUtils::SearchValueInDict(const string& filePath, const string& PortName)
{
	string lsNone = "";

	map<string, string> dictionary = ParseDictionaryFile(filePath);

	for (const auto& pair : dictionary)
	{
		//cout << "Clé : " << pair.first << " Valeur : " << pair.second << endl;
		if (pair.first == PortName)
		{
			return pair.second;
		}
	}
	return lsNone;
}


map<std::string, std::map<std::string, std::string>> TestUtils::ReadDataFileWithIndex(const std::string& filePath)
{
	std::map<std::string, std::map<std::string, std::string>> data;
	std::ifstream file(filePath);

	if (!file.is_open())
	{
		std::cerr << "Error in open File : " << filePath << std::endl;
		return data;
	}

	std::string line;
	std::string currentSection;
	while (std::getline(file, line))
	{
		if (line.empty() || line[0] == '#')
		{
			// Ignore the empty line and comments
			if (!line.empty() && line[1] != '-')
			{
				currentSection = line.substr(1); // Section Name
				data[currentSection] = std::map<std::string, std::string>();
			}
			continue;
		}

		size_t pos = line.find('=');
		if (pos != std::string::npos)
		{
			std::string key = line.substr(0, pos);
			std::string value = line.substr(pos + 1);
			data[currentSection][key] = value;
		}
	}

	file.close();
	return data;
}

// Method to find a specific a Data from Dictionnary
string TestUtils::GetDataWithIndex(const std::map<std::string, std::map<std::string, std::string>>& data,
	const std::string& section, const std::string& key)
{
	auto sectionIt = data.find(section);
	if (sectionIt != data.end())
	{
		auto keyIt = sectionIt->second.find(key);
		if (keyIt != sectionIt->second.end())
		{
			return keyIt->second;
		}
	}
	return "Key Not Found";
}

t_dict TestUtils::PreparePortSettings(const std::map<std::string, std::map<std::string, std::string>>& data, const std::string& section)
{
	t_dict ldSettingsPort;

	for (auto& element : data)
	{
		if (element.first == section)
		{
			for (auto& str : element.second)
			{
				ldSettingsPort.insert({ str.first, str.second });
			}
		}
	}
	return ldSettingsPort;
}

int TestUtils::ComparePortValue(const std::string& a_ComponentName, const std::string& InputPortValue)
{

	return 0;
}


int TestUtils::ReadNameParamCsvFile(const std::string filename, t_list csvData)
{
	std::ifstream file;
	std::string input;

	file.open(filename);

	if (file.fail())
	{
		cout << "file didn't open" << endl;
		file.close();
		return 1;
	}

	while (file.peek() != EOF)
	{
		std::string records;

		std::getline(file, records, ',');
		csvData.push_back(records);
	}

	if (csvData.size() > 0)
	{
		file.close();
		cout << "file is opned and parsed" << endl;
		return noError;
	}
	else
	{
		file.close();
		cout << "file is empty" << endl;
		return errCompare;
	}
}

// Read and Save Csv File 
std::vector<std::string> TestUtils::readCSV(const std::string& filename)
{
	std::vector<std::string> lines;
	std::ifstream file(filename, std::ios::binary);
	skipUTF8BOM(file);

	if (!file.is_open()) {
		std::cerr << "Erreur Open file : " << filename << std::endl;
		return lines;
	}

	std::string line;
	while (std::getline(file, line)) {
		line.erase(std::remove(line.begin(), line.end(), '\r'), line.end());
		lines.push_back(line);
	}

	file.close();
	return lines;
}

std::vector<std::string> TestUtils::parseLineCSV(const std::string& a_line, const char& a_sep)
{
	std::vector<std::string> elems;
	std::istringstream iLineStream(a_line);
	std::string cell;
	while (std::getline(iLineStream, cell, a_sep))
	{
		elems.push_back(cell);
	}
	return elems;
}

int TestUtils::ComparaisonCsvFile(string const CsvFilePath1, string const CsvFilePath2)
{
	bool areEqual = true;

	std::vector<std::string> lines1 = TestUtils::readCSV(CsvFilePath1);
	std::vector<std::string> lines2 = TestUtils::readCSV(CsvFilePath2);

	size_t maxLines = std::max(lines1.size(), lines2.size());

	//Comparaison des fichiers résultats
	for (size_t i = 0; i < maxLines; ++i)//outerLoop
	{
		if (i >= lines1.size())
		{
			cout << "Line supp in " << CsvFilePath1 << ": " << lines2[i] << endl;
			areEqual = false;
		}
		else if (i >= lines2.size())
		{
			std::cout << "Line supp in " << CsvFilePath2 << ": " << lines1[i] << endl;
			areEqual = false;
		}
		else 
		{
			//header
			if (i == 0) {
				std::vector<std::string> values1 = parseLineCSV(lines1[i]);
				std::vector<std::string> values2 = parseLineCSV(lines2[i]);

				if (values1.size() != values2.size()) {
					areEqual = false;
				}
				else {
					for (size_t j = 0; j < values1.size(); ++j) {//innerLoop
						if (values1[j] != values2[j]) {
							areEqual = false;
							break;
						}
					}
				}				
			}
			else {
				std::vector<std::string> values1 = parseLineCSV(lines1[i]);
				std::vector<std::string> values2 = parseLineCSV(lines2[i]);

				if (values1.size() != values2.size()) {
					areEqual = false;
				}
				else {
					for (size_t j = 0; j < values1.size(); ++j) {//innerLoop
						double val1 = atof(values1[j].c_str());
						double val2 = atof(values2[j].c_str());
						if (fabs(val2) < 10e-10) {
							if (fabs(val1 - val2) > 10e-6) {
								areEqual = false;//innerLoop
								break;
							}
						}
						else {
							if (fabs(val1 - val2) > 0.05*fabs(val2)) {
								areEqual = false;//innerLoop
								break;
							}
						}
						
					}
				}
			}
			if (!areEqual) {
				cout << "Difference in the line " << i + 1 << ":" << endl;
				cout << CsvFilePath1 << ": " << lines1[i] << endl;
				cout << CsvFilePath2 << ": " << lines2[i] << endl;
				break; //outerLoop
			}
		}
	}

	if (areEqual)
	{
		cout << "Test Ok - Files are identical." << endl;
		return noError;
	}
	else
	{
		cout << "Error Compare - Test Not Ok - Files are different." << endl;
		return errCompare;
	}
}

std::vector<std::string> TestUtils::str_to_vector(const std::string& str)
{
	std::stringstream ss(str);
	ss.imbue(std::locale(std::locale(), new tokens()));
	std::istream_iterator<std::string> begin(ss);
	std::istream_iterator<std::string> end;
	std::vector<std::string> vec(begin, end);
	std::copy(vec.begin(), vec.end(), std::ostream_iterator<std::string>(std::cout, ";"));
	return vec;
}

void TestUtils::skipUTF8BOM(std::istream& in)
{
	unsigned char bom[3];
	std::streampos start = in.tellg();

	in.read(reinterpret_cast<char*>(bom), 3);

	if (in.gcount() == 3 &&
		bom[0] == 0xEF && bom[1] == 0xBB && bom[2] == 0xBF)
	{
		return; // BOM consumed
	}

	// No BOM => rewind to start
	in.clear();
	in.seekg(start);
}



std::istream& operator>>(std::istream& str, CSVRow& data)
{
	data.readNextRow(str);
	return str;
}


std::ostream& operator<<(std::ostream& str, const std::vector<std::string>& data)
{
	if (data.size()) {
		str << data[0];
		for (size_t i = 1; i < data.size(); i++) {
			str << CSV_SEPARATOR << data[i];
		}
		str << std::endl;
	}
	return str;
}

std::ostream& operator<<(std::ostream& str, const std::vector<double>& data)
{
	if (data.size()) {
		str << data[0];
		for (size_t i = 1; i < data.size(); i++) {
			str << CSV_SEPARATOR << data[i];
		}
		str << std::endl;
	}
	return str;
}

std::ostream& operator<<(std::ostream& str, const std::vector<t_value>& data)
{
	if (data.size()) {
		if (const double* pSrc = std::get_if<double>(&data[0])) {
			str << *pSrc;
		}		
		else if (const std::string* pSrc = std::get_if<std::string>(&data[0])) {
			str << *pSrc;
		}		
		for (size_t i = 1; i < data.size(); i++) {
			if (const double* pSrc = std::get_if<double>(&data[i])) {
				str << CSV_SEPARATOR << *pSrc;				
			}
			else if (const std::string* pSrc = std::get_if<std::string>(&data[i])) {
				str << CSV_SEPARATOR << *pSrc;
			}			
		}
		str << std::endl;
	}
	return str;
}