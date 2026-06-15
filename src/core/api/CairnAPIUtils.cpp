#include "CairnAPIUtils.h"
#include "CairnCore.h"
#include "Cairn_Exception.h"

#include <stdexcept>
#include <iostream>
#include <fstream>
#include <map>

class ModelParam;

namespace CairnAPIUtils {
	
	t_list lookupSubDirectories(const std::string& dirPath)
	{
		std::vector<std::string> vRet = {};
		for (auto& dir : std::filesystem::recursive_directory_iterator(dirPath))
		{
			if (dir.is_directory())
			{
				vRet.push_back(dir.path().string());
			}
		}
		return vRet;
	}

	std::string convertTypeToUpperCase(const std::string type)
	{
		if (type == "converter") return "Converter";
		else if (type == "storage") return "Storage";
		else if (type == "grid") return "Grid";
		else if (type == "sourceload") return "SourceLoad";
		else if (type == "operationconstraint") return "OperationConstraint";
		else if (type == "physicalequation") return "PhysicalEquation";
		else if (type == "busflowbalance") return "BusFlowBalance";
		else if (type == "bussamevalue") return "BusSameValue";
		else if (type == "multiobjcompo") return "MultiObjCompo";
		else return "";
	}

	std::map<std::string, std::string> ParserTxt(const std::string& filename)
	{
		std::string line;
		std::map<std::string, std::string> dataMap = {};

		// Open File
		std::ifstream iFile(filename);
		if (!iFile.is_open())
		{
			cerr << "Error : cannot open the file " << filename << endl;
			return dataMap;
		}
		while (std::getline(iFile, line, ';'))
		{
			// Parse line with seperator ','			
			line.erase(std::remove(line.begin(), line.end(), '\r'), line.end());
			std::string cell;
			std::istringstream iLineStream(line);
			t_list lineVec;

			while (std::getline(iLineStream, cell, ':'))
			{
				lineVec.push_back(cell);
			}
			//Add line to dataMap
			if (lineVec.size() > 1) {
				dataMap.insert({ lineVec.at(0), lineVec.at(1) });
			}
		}
		return dataMap;
	}

	void initModelTypesMap()
	{
		//read model types from .txt file
		if (const char* env_p = std::getenv("CAIRN_BIN")) {
			std::string exeDir(env_p);
			std::string modelTypesFileName = exeDir + (std::string)"/../resources/modelTypes.txt";
			mModelTypes = ParserTxt(modelTypesFileName);
		}
		else {
			spdlog::critical("environment variable CAIRN_BIN does not exist!");
		}		
	}

	t_list get_Possible_Model_Names()
	{
		//Dynamics list from *CairnModel.dll		
		ModelFactory vModelFactory(spdlog::default_logger());
		vModelFactory.findModels();
		return vModelFactory.getModelList();		
	}

	t_list get_Possible_Component_Types() {
		t_list vRet = { "BusFlowBalance", "BusSameValue", "MultiObjCompo" };
		for (auto const& vItem : mModelTypes) 
		{
			if (vItem.second == "bus" 
				|| (std::find(vRet.begin(), vRet.end(), vItem.second) != vRet.end()))
				continue;
			vRet.push_back(convertTypeToUpperCase(vItem.second));
		}
		return vRet;
	}

	std::string get_Component_Type(const std::string& a_Model) {
		for (auto const& vItem : mModelTypes) {
			if (mModelTypes.find(a_Model) != mModelTypes.end())
			{
				return convertTypeToUpperCase(mModelTypes[a_Model]);
			}
		}
		return "Unknown";
	}

	std::string get_Bus_Type(const std::string& a_Model) 
	{
		if (a_Model == "NodeLaw" || a_Model == "ManualConstraint") return "BusFlowBalance";
		else if (a_Model == "NodeEquality") return "BusSameValue";
		else if (a_Model == "ManualObjective") return "MultiObjCompo";
		else return "Unknown";
	}

	t_list getParametersName(std::vector<InputParam*> a_Inputs, CairnAPI::ESettingsLimited a_setLimited)
	{
		t_list vRet;
		for (auto& vInput : a_Inputs) {
			if (vInput) {
				std::vector<std::string> vList;
				vInput->getParameters(vList, a_setLimited);
				for (auto& vParam : vList) {
					vRet.push_back(vParam);
				}
			}
		}
		return vRet;
	}

	std::string getParamValue(const t_value& a_Value)
	{
		std::string vRet = "NON_COMPATIBLE";
		if (std::holds_alternative<std::string>(a_Value)) {
			vRet = std::get<std::string>(a_Value);
		}
		/*else if (std::holds_alternative<bool>(a_Value)) {
			vRet = std::to_string(std::get<bool>(a_Value));
		}*/
		else if (std::holds_alternative<int>(a_Value)) {
			vRet = std::to_string(std::get<int>(a_Value));
		}
		else {
			try
			{
				vRet = std::to_string(std::get<double>(a_Value));
			}
			catch (const std::exception&)
			{
				// type non compatible
			}
		}
		return vRet;
	}

	t_value getParameter(std::vector<InputParam*> a_Inputs, const std::string& a_Name) {
		t_value vRet = "parameter doesn't exist";
		for (auto& vInput : a_Inputs) {
			if (vInput) {				
				if (vInput->getParameterValue(a_Name, vRet)) 
					break;				
			}
		}
		return vRet;
	}

	void getParameters(std::vector<InputParam*> a_Inputs, t_dict& a_Params) {
		for (auto& vInput : a_Inputs) {
			if (vInput) {
				std::vector<std::string> vList;
				vInput->getParameters(vList);
				for (auto& vParamName : vList) {
					// mettre les valeurs vide ?					
					t_value vRet;
					if (vInput->getParameterValue(vParamName, vRet))
						a_Params[vParamName] = vRet;
				}
			}
		}
	}

	bool setParameter(std::vector<InputParam*> a_Inputs, const std::string& a_Name, const t_value& a_Value) {
		bool vFind = false;

		for (auto& vInput : a_Inputs) {
			if (!vInput) continue;

			vFind = std::visit([&](const auto& val) -> bool {
				return vInput->setParameterValue(a_Name, val);
				}, a_Value);

			if (vFind) break;
		}

		return vFind;
	}

	bool setParameters(std::vector<InputParam*> a_Inputs, const t_dict& a_Params) {
		bool vOk = true;
		for (auto& vParam : a_Params) {
			vOk &= setParameter(a_Inputs, vParam.first, vParam.second);
		}
		return vOk;
	}

	std::string getParamComment(std::vector<InputParam*> a_Inputs, const std::string& a_Name)
	{
		std::string vRet = "";
		for (auto& vInput : a_Inputs) {
			if (vInput) {
				if (vInput->getParameter(a_Name))
				{
					vRet = vInput->getParameter(a_Name)->getComment();
					break;
				}
			}
		}
		return vRet;
	}

	void getParamComments(std::vector<InputParam*> a_Inputs, t_dictComment& a_Params) {
		for (auto& vInput : a_Inputs) {
			if (vInput) {
				std::vector<std::string> vList;
				vInput->getParameters(vList);
				for (auto& vParamName : vList) {
					if (vInput->getParameter(vParamName)) {
						a_Params[vParamName] = vInput->getParameter(vParamName)->getComment();
					}
				}
			}
		}
	}

	bool setParamComment(std::vector<InputParam*> a_Inputs, const std::string& a_Name, const std::string& a_Comment) 
	{
		for (auto& vInput : a_Inputs) {
			if (vInput) {
				if (vInput->getParameter(a_Name))
				{
					vInput->getParameter(a_Name)->setComment(a_Comment);
					return true;
				}
			}
		}
		return false;
	}

	//bool setParamComments(std::vector<InputParam*> a_Inputs, const t_dictComment& a_Params) {
	//	bool vOk = true;
	//	for (auto& vParam : a_Params) {
	//		vOk &= setParamComment(a_Inputs, vParam.first, vParam.second);
	//	}
	//	return vOk;
	//}

	bool isMandatoryParam(std::vector<InputParam*> a_Inputs, const std::string& a_Name)
	{
		bool vRet = true;
		for (auto& vInput : a_Inputs) {
			if (vInput) {
				if (vInput->getParameter(a_Name))
				{
					vRet = vInput->getParameter(a_Name)->IsBlocking();
					break;
				}
			}
		}
		return vRet;
	}

	bool isUsedParam(std::vector<InputParam*> a_Inputs, const std::string& a_Name)
	{
		bool vRet = false;
		for (auto& vInput : a_Inputs) {
			if (vInput) {
				if (vInput->getParameter(a_Name))
				{
					vRet = vInput->getParameter(a_Name)->IsUsed();
					break;
				}
			}
		}
		return vRet;
	}

	std::string getParamUnit(std::vector<InputParam*> a_Inputs, const std::string& a_Name)
	{
		std::string vRet = "-";
		for (auto& vInput : a_Inputs) {
			if (vInput) {
				if (vInput->getParameter(a_Name))
				{
					vRet = vInput->getParameter(a_Name)->getUnit();
					break;
				}
			}
		}
		return vRet;
	}

	std::string getParamShowConfig(std::vector<InputParam*> a_Inputs, const std::string& a_Name)
	{
		std::string vRet = "parameter doesn't exist";
		for (auto& vInput : a_Inputs) {
			if (vInput) {
				if (vInput->getParameter(a_Name))
				{
					vRet = vInput->getParameter(a_Name)->getShowConfig();
					break;
				}
			}
		}
		return vRet;
	}

	// Returns a list of ShowConfigs of several parameters
	t_list getShowConfigList(std::vector<InputParam*> a_Inputs)
	{
		t_list vRet;
		for (auto& vInput : a_Inputs) {
			if (vInput) {
				std::vector <std::string> vList = vInput->getShowConfigList();
				for (auto& vConfig : vList) {
					if (find(vRet.begin(), vRet.end(), vConfig) == vRet.end())
					{
						vRet.push_back(vConfig);
					}
				}
			}
		}
		return vRet;
	}

	void setError(ECodeError a_Err, const std::string& a_msg) {
		if (a_Err != noError) {
			std::string vErrMsg(a_msg.c_str());
			Cairn_Exception cairn_error;
			switch (a_Err) {
			case noCairn:
				cairn_error.setMessage("Cairn is not Initialized");
				break;
			case errInit:
				cairn_error.setMessage("Error when Initializing the problem");
				break;
			case errRead:
				cairn_error.setMessage("Failed to read " + vErrMsg);
				break;
			case errFile:
				cairn_error.setMessage("File does not exist, " + vErrMsg);
				break;
			case errWrite:
				cairn_error.setMessage("Failed to write file, " + vErrMsg);
				break;
			case errLink:
				cairn_error.setMessage("Bad link, " + vErrMsg);
				break;
			case errErase:
				cairn_error.setMessage("Cannot remove, " + vErrMsg);
				break;
			case errNotFound:
				cairn_error.setMessage("Not found: " + vErrMsg);
				break;
			case errParam:
				cairn_error.setMessage("Parameter not found!");
				break;
			case errAlreadyExist:
				cairn_error.setMessage("Already exist: " + vErrMsg);
				break;
			case errCreate:
				cairn_error.setMessage("Failed to create: " + vErrMsg);
				break;
			case errAdd:
				cairn_error.setMessage("Failed to add: " + vErrMsg);
				break;
			default:
				cairn_error.setMessage(vErrMsg);
				break;
			}
			throw std::runtime_error(cairn_error.message());
		}
	}

	std::string valueToString(const t_value& value)
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
}

