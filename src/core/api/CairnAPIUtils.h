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

	static std::map<std::string, t_list> mModelsMap = {
		{ "BusFlowBalance", {"NodeLaw"} },
		{ "BusSameValue", {"NodeEquality"} },
		{ "MultiObjCompo", {"ManualObjective"} },
		{ "Grid", {"GridFree"} },
		{ "SourceLoad", {"SourceLoad", "SourceLoadFlexible", "SourceLoadMinMax", "BuildingFlexibleBasic", "BuildingFlexible"} },
		{ "Storage", {"StorageGen", "StorageLinearBounds", "StorageThermal", "Battery_V1"} },
		{ "Converter", {"Electrolyzer", "PipelineBasic", "ThermalGroup", "ProductionUC", "Cogeneration", "FuelCell", "Dam", "Mixer",
			"Transportation", "NeuralNetwork", "HeatPump", "SMReformer", "Methaner", "Methanizer", "Compressor", "Converter",
			"MultiConverter", "HeatExchange", "PowerToFluidT", "PowerToFluidH", "Cluster"} },
		{ "OperationConstraint", {"Ramp", "MinStateTime", "ConstantProduction"} },
		{ "PhysicalEquation", {"HeatEquation","HeatExchangeEquation"} }
	};

	//This list only used to get the type of a private model. Not all models are necessarily available 
	static std::map<std::string, t_list> mPrivateModelsMap = { //TODO: add Type inside ModelFactory Info.?!
			{ "Grid", {"GridaFRRService", "GridFCRService"} },
			{ "Storage", {"StorageSeasonal"} },
			{ "Converter", {"ElectrolyzerDetailed"} }
	};

	// Return the list of the all possibles model names
	t_list get_Possible_Model_Names();

	// Return the list of the all possibles component types 
	t_list get_Possible_Component_Types(); 

	// Return the type of Component given its Model name
	std::string get_Component_Type(const std::string& a_Model);

	t_list getParametersName(std::vector<InputParam*> a_Inputs, CairnAPI::ESettingsLimited a_setLimited);

	DECLSPEC std::string getParamValue(const t_value& a_Value);
	DECLSPEC std::vector<double> getParamVectorValue(const t_value& a_Value);

	t_value getParameter(std::vector<InputParam*> a_Inputs, const std::string& a_Name);

	void getParameters(std::vector<InputParam*> a_Inputs, t_dict& a_Params);

	bool setParameter(std::vector<InputParam*> a_Inputs, const std::string& a_Name, const t_value& a_Value);

	bool setParameters(std::vector<InputParam*> a_Inputs, const t_dict& a_Params);

	void setError(ECodeError a_Err, const std::string& a_msg = "");	
}


#endif