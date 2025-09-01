#include "CairnAPIUtils.h"
#include "CairnCore.h"
#include "Cairn_Exception.h"

#include <stdexcept>
#include <iostream>
#include <fstream>
#include <map>

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

	//void lookupModelTypes(const std::string& modelsDir)
	//{
	//	t_list modelsList = get_Possible_Model_Names();
	//	std::vector<std::string> dirList = lookupSubDirectories(modelsDir);

	//	for (size_t i = 0; i < modelsList.size(); ++i)
	//	{
	//		bool exists = false;
	//		std:string model = modelsList[i];
	//		//look in which directory the model exist
	//		for (size_t j = 0; j < dirList.size(); ++j)
	//		{
	//			exists = std::filesystem::exists(dirList[j] + "/" + model + ".h");
	//			if (exists) {
	//				std::string type = std::filesystem::path(dirList[j]).filename().string();
	//				//if(type.size()) type[0] = toupper(type[0]);
	//				convertTypeToUpperCase(type);
	//				if (mModelTypes.find(type) != mModelTypes.end()) {
	//					mModelTypes[type].push_back(model);
	//				}
	//				else {
	//					mModelTypes[type] = { model };
	//				}
	//				break;
	//			}
	//		}
	//		if (!exists) {
	//			qWarning() << "The type of " + QString::fromStdString(model) + " is unknown!";
	//		}
	//	}
	//}

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

		// Read File - string Format
		std::stringstream buffer;
		buffer << iFile.rdbuf();
		string inputData = buffer.str();

		// Close File
		iFile.close();

		// Parse File - Line-Line
		std::istringstream iDataStream(inputData);
		while (getline(iDataStream, line))
		{
			// Parse line with seperator ','			
			std::string cell;
			std::istringstream iLineStream(line);
			t_list lineVec;

			while (getline(iLineStream, cell, ','))
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

	void initModelTypesMap(const std::string& sourceDir)
	{
		//read model types from .txt file
		QString exeDir = qEnvironmentVariable("CAIRN_BIN", QDir::currentPath());
		std::string modelTypesFileName = exeDir.toStdString() + (std::string)"/../resources/modelTypes.txt";
		mModelTypes = ParserTxt(modelTypesFileName);

//		lookupModelTypes(sourceDir + "/models");
//#ifdef PRIVATE_MODELS
//	lookupModelTypes(sourceDir + "/privateModels");
//#endif
	}

	t_list get_Possible_Model_Names()
	{
		//Dynamics list from *CairnModel.dll		
		ModelFactory vModelFactory;
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
		if (a_Model == "NodeLaw") return "BusFlowBalance";
		else if (a_Model == "NodeEquality") return "BusSameValue";
		else if (a_Model == "ManualObjective") return "MultiObjCompo";
		else return "Unknown";
	}

	t_list getParametersName(std::vector<InputParam*> a_Inputs, CairnAPI::ESettingsLimited a_setLimited)
	{
		t_list vRet;
		for (auto& vInput : a_Inputs) {
			if (vInput) {
				QList<QString> vQList;
				vInput->getParameters(vQList, a_setLimited);
				for (auto& vParam : vQList) {
					vRet.push_back(vParam.toStdString());
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

	std::vector<double> getParamVectorValue(const t_value& a_Value)
	{
		if (std::holds_alternative<std::vector<double>>(a_Value)) {
			return std::get<std::vector<double>>(a_Value);
		}
		return {};
	}

	t_value getParameter(std::vector<InputParam*> a_Inputs, const std::string& a_Name) {
		t_value vRet = "parameter doesn't exist";
		QString vQName = QString(a_Name.c_str());
		for (auto& vInput : a_Inputs) {
			if (vInput) {				
				if (vInput->getParameterValue(vQName, vRet))
					break;				
			}
		}
		return vRet;
	}

	void getParameters(std::vector<InputParam*> a_Inputs, t_dict& a_Params) {
		for (auto& vInput : a_Inputs) {
			if (vInput) {
				QList<QString> vQList;
				vInput->getParameters(vQList);
				for (auto& vParamName : vQList) {
					// mettre les valeurs vide ?					
					t_value vRet;
					if (vInput->getParameterValue(vParamName, vRet))
						a_Params[vParamName.toStdString()] = vRet;
				}
			}
		}
	}

	bool setParameter(std::vector<InputParam*> a_Inputs, const std::string& a_Name, const t_value& a_Value) {
		bool vOk = true;
		if (find(mNonModifiableParams.begin(), mNonModifiableParams.end(), a_Name) == mNonModifiableParams.end())
		{
			QString vQName = QString(a_Name.c_str());
			QString vQValue = QString(getParamValue(a_Value).c_str());
			std::vector<double> vVectValue;
			if (vQValue == "NON_COMPATIBLE") {
				//try vector of double
				vVectValue = getParamVectorValue(a_Value);
			}
			bool vFind = false;
			for (auto& vInput : a_Inputs) {
				if (vInput) {
					if (vQValue != "NON_COMPATIBLE") {
						vFind = vInput->setParameterValue(vQName, vQValue);
					}
					else {
						vFind = vInput->setParameterValue(vQName, vVectValue);
					}
					if (vFind) break;
				}
			}
			vOk &= vFind;
		}
		else {
			setError(errDefault, a_Name + " cannot be modified!");
		}
		return vOk;
	}

	bool setParameters(std::vector<InputParam*> a_Inputs, const t_dict& a_Params) {
		bool vOk = true;
		for (auto& vParam : a_Params) {
			if (find(mNonModifiableParams.begin(), mNonModifiableParams.end(), vParam.first) != mNonModifiableParams.end()) 
			{
				//qWarning() << (vParam.first+" cannot be modified!").c_str();
				continue;
			}
			vOk &= setParameter(a_Inputs, vParam.first, vParam.second);
		}
		return vOk;
	}

	t_value getShowConfig(std::vector<InputParam*> a_Inputs, const std::string& a_Name)
	{
		t_value vRet = "parameter doesn't exist";
		QString vQName = QString(a_Name.c_str());
		for (auto& vInput : a_Inputs) {
			if (vInput) {
				if (vInput->getParameter(vQName))
				{
					vRet = vInput->getParameter(vQName)->getShowConfig();
					break;
				}
			}
		}
		return vRet;
	}

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
			QString vErrMsg(a_msg.c_str());
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
			throw std::range_error(cairn_error.message().toStdString());
		}
	}
}
