
#include "Utils.h"
#include <math.h>
#include <unordered_set>

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


void TestUtils::Display_list(const t_list& inputList, const std::string& title)
{
	if (title != "") {
		std::cout << title << std::endl;
	}
	for (auto& vCmp : inputList)
	{
		std::cout << "component: " << vCmp << std::endl;
	}
}

void TestUtils::Display_Vector(const std::vector<t_list>& inputVector)
{
	for (const auto& ligne : inputVector)
	{
		for (const auto& cell : ligne)
		{
			std::cout << cell << ",";
		}

		cout << std::endl;
	}
}

vector<t_list> TestUtils::ParserTxt(const string& cheminFichier)
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
bool TestUtils::compare_scalar(const t_value& val, const t_value& ref, EParamType type)
{
	switch (type) {
	case eDouble: {
		const double a = std::get<double>(val);
		const double b = std::get<double>(ref);
		const double eps = 1e-9;
		return std::fabs(a - b) <= eps * std::max(1.0, std::fabs(b));
	}
	case eInt:
		return std::get<int>(val) == std::get<int>(ref);
	case eBool:
		return static_cast<bool>(std::get<int>(val)) == static_cast<bool>(std::get<int>(ref));
	case eString:
		return std::get<std::string>(val) == std::get<std::string>(ref);
	case eStringList: {
		const auto& a = std::get<std::vector<std::string>>(val);
		const auto& b = std::get<std::vector<std::string>>(ref);
		return a == b;
	}
	default:
		return false;
	}
}

bool TestUtils::compare_lists(const t_list& inputList, const t_list& refList)
{
	if (inputList.size() != refList.size()) {
		std::cout << "Size mismatch: input=" << inputList.size() << " ref=" << refList.size() << std::endl;
		return false;
	}

	// order is not important
	std::unordered_set<std::string> refSet(refList.begin(), refList.end());
	for (const auto& item : inputList) {
		if (refSet.find(item) == refSet.end()) {
			std::cout << "Missing element in reference list: " << item << std::endl;
			return false;
		}
	}
	return true;
}

bool TestUtils::compare_dict(const t_dict& inputDict, const t_dict& refDict)
{
	// Quick size check
	if (inputDict.size() != refDict.size()) {
		std::cout << "Dict size mismatch: input=" << inputDict.size()
			<< " ref=" << refDict.size() << std::endl;
		return false;
	}

	// Compare each key/value pair
	for (const auto& [key, inputValue] : inputDict)
	{
		auto refIt = refDict.find(key);
		if (refIt == refDict.end()) {
			std::cout << "Missing key in reference dict: " << key << std::endl;
			return false;
		}

		const t_value& refValue = refIt->second;
		if (inputValue != refValue) {
			std::cout << "Value mismatch for key: " << key << std::endl;
			std::cout << "  Input    : " << valueToString(inputValue) << std::endl;
			std::cout << "  Reference: " << valueToString(refValue) << std::endl;
			return false;
		}
	}

	// All checks passed
	return true;
}


bool TestUtils::contains(const t_list& inputList, const std::string& val)
{
	return std::find(inputList.begin(), inputList.end(), val) != inputList.end();
}

bool TestUtils::CreateReferenceList(const std::vector<std::vector<std::string>>& dataRef, t_list& outputSolverList)
{
	outputSolverList.clear();
	for (const auto& row : dataRef) {
		outputSolverList.insert(outputSolverList.end(), row.begin(), row.end());
	}
	return !outputSolverList.empty();
}

std::string TestUtils::valueToString(const t_value& value)
{
	return std::visit([](const auto& val) -> std::string {
		using T = std::decay_t<decltype(val)>;
		std::ostringstream oss;
		if constexpr (std::is_same_v<T, double>) {
			oss << val; 
			return oss.str();
		}
		else if constexpr (std::is_same_v<T, int>) {
			oss << val; 
			return oss.str();
		}
		else if constexpr (std::is_same_v<T, std::string>) {
			return val;
		}
		else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
			oss << "["; 
			for (size_t i = 0; i < val.size(); ++i) { 
				if (i) oss << ", "; 
				oss << val[i]; 
			} 
			oss << "]"; 
			return oss.str();
		}
		else if constexpr (std::is_same_v<T, std::vector<double>>) {
			oss << "["; 
			for (size_t i = 0; i < val.size(); ++i) { 
				if (i) oss << ", "; 
				oss << val[i]; 
			} 
			oss << "]"; 
			return oss.str();
		}
		else if constexpr (std::is_same_v<T, std::vector<int>>) {
			oss << "["; 
			for (size_t i = 0; i < val.size(); ++i) { 
				if (i) oss << ", "; 
				oss << val[i]; 
			} 
			oss << "]"; 
			return oss.str();
		}
		return "unknown";
		}, value);
}

// Read and Save Csv File 
t_list TestUtils::readCSV(const std::string& filename)
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

t_list TestUtils::parseLineCSV(const std::string& a_line, char a_sep)
{
	std::vector<std::string> elems;
	std::istringstream iss(a_line);
	std::string cell;
	while (std::getline(iss, cell, a_sep)) {
		// trim both ends
		auto l = cell.find_first_not_of(" \t\r\n");
		auto r = cell.find_last_not_of(" \t\r\n");
		if (l == std::string::npos) 
			elems.emplace_back("");
		else 
			elems.emplace_back(cell.substr(l, r - l + 1));
	}
	return elems;
}


bool TestUtils::ComparaisonCsvFile(const std::string& path1, const std::string& path2)
{
	auto lines1 = readCSV(path1);
	auto lines2 = readCSV(path2);
	const size_t maxLines = std::max(lines1.size(), lines2.size());

	auto numericEqual = [](double a, double b) {
		const double abs_eps = 1e-6;
		const double rel_eps = 5e-2; // 5%
		if (std::fabs(b) < 1e-12) 
			return std::fabs(a - b) <= abs_eps;
		return std::fabs(a - b) <= std::max(abs_eps, rel_eps * std::fabs(b));
	};

	for (size_t i = 0; i < maxLines; ++i) {
		if (i >= lines1.size()) { 
			std::cout << "Missing line in " << path1 << ": " << lines2[i] << "\n"; 
			return false; 
		}
		if (i >= lines2.size()) { 
			std::cout << "Missing line in " << path2 << ": " << lines1[i] << "\n"; 
			return false; 
		}

		auto v1 = parseLineCSV(lines1[i]);
		auto v2 = parseLineCSV(lines2[i]);
		if (v1.size() != v2.size()) { 
			std::cout << "Different column count on line " << i + 1 << "\n"; 
			return false; 
		}

		if (i == 0) { // header: string compare
			if (v1 != v2) { 
				std::cout << "Header mismatch\n"; 
				return false; 
			}
			continue;
		}

		for (size_t j = 0; j < v1.size(); ++j) {
			// try numeric compare, fallback to string compare
			char* end1 = nullptr; char* end2 = nullptr;
			double d1 = std::strtod(v1[j].c_str(), &end1);
			double d2 = std::strtod(v2[j].c_str(), &end2);
			bool n1 = (end1 != v1[j].c_str());
			bool n2 = (end2 != v2[j].c_str());
			if (n1 && n2) {
				if (!numericEqual(d1, d2)) {
					std::cout << "Numeric difference at line " << i + 1 << " col " << j + 1 << "\n";
					return false;
				}
			}
			else {
				if (v1[j] != v2[j]) {
					std::cout << "String difference at line " << i + 1 << " col " << j + 1 << "\n";
					return false;
				}
			}
		}
	}
	std::cout << "Test Ok - Files are identical.\n";
	return true;
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