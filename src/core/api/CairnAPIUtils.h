#ifndef CAIRNAPIUTILS_H
#define CAIRNAPIUTILS_H

#include <stdexcept>
#include <iostream>
#include <fstream>
#include <map>

#include "CairnAPI.h"
class InputParam;

namespace CairnAPIUtils {

	enum ECodeError {
		errDefault = -2,  //Default Error to be used with customized messages
		errRead = -1,  //Error in the Read Method
		noError = 0,
		errSize = 1,      //File Size Error(size not equal - empty)
		errCompare = 2,  // Error in the compare Method - Not Equal
		errInit = 3,
		errRun = 4,
		noCairn = 5,   // Cairn (OptimProblem) is not initialized (by read_study or create_Study)
		errNotFound = 6, // Item (problem, component, port, variable...) not found
		errAlreadyExist = 7,    // Item (problem, component, port, variable,...) already exists
		errParam = 8,  //parameter not found (error while setting a parameter) //TODO: merge it with errNotFound ?
		errCreate = 9,    // Error in the Create Method
		errSet = 10,    // Error in the Set Method
		errGet = 11,   // Error in the Get Method
		errAdd = 12,  // Error in the Add Method (component or port)
		errWrite = 13,
		errErase = 14,
		errFile = 15,
		errLink = 16
	};

	std::map<std::string, std::string> ParserTxt(const std::string& filename);

	std::string convertTypeToUpperCase(const std::string type);

	// Return the list of all sub-directories of dirPath
	t_list lookupSubDirectories(const std::string& dirPath);

	// Lookup model types in sourceDir
	static std::map<std::string, std::string> mModelTypes;
	void initModelTypesMap();
	
	// Return the list of the all possibles model names
	t_list get_Possible_Model_Names();

	// Return the list of the all possibles component types 
	t_list get_Possible_Component_Types(); 

	// Return the type of Component given its Model name
	std::string get_Component_Type(const std::string& a_Model);

	// Return the type of Bus given its Model name
	std::string get_Bus_Type(const std::string& a_Model);

	t_list getParametersName(std::vector<InputParam*> a_Inputs, CairnAPI::ESettingsLimited a_setLimited);

	DECLSPEC std::string getParamValue(const t_value& a_Value);
	DECLSPEC std::vector<double> getParamVectorValue(const t_value& a_Value);

	t_value getParameter(std::vector<InputParam*> a_Inputs, const std::string& a_Name);

	void getParameters(std::vector<InputParam*> a_Inputs, t_dict& a_Params);

	bool setParameter(std::vector<InputParam*> a_Inputs, const std::string& a_Name, const t_value& a_Value);

	bool setParameters(std::vector<InputParam*> a_Inputs, const t_dict& a_Params);

	t_list getShowConfigList(std::vector<InputParam*> a_Inputs);
	std::string getParamShowConfig(std::vector<InputParam*> a_Inputs, const std::string& a_Name);

	bool getParamMandatoryValue(std::vector<InputParam*> a_Inputs, const std::string& a_Name);

	bool isDependentParam(std::vector<InputParam*> a_Inputs, const std::string& a_Name);

	std::string getParamUnit(std::vector<InputParam*> a_Inputs, const std::string& a_Name);

	void setError(ECodeError a_Err, const std::string& a_msg = "");	
}


#endif