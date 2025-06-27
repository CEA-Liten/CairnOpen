#ifndef CAIRNAPI_H
#define CAIRNAPI_H

#include <string>
#include <map>
#include <vector>
#include <variant>
typedef std::vector<std::string> t_list;
//Adding type bool to t_value will cause a problem for std::string being casted as bool
typedef std::variant<double, int, std::string, std::vector<std::string>, std::vector<double>, std::vector<int>> t_value;
typedef std::map<std::string, t_value> t_dict;

typedef std::vector<t_value> t_values;
typedef std::map<std::string, t_values> t_dictValues;


#if defined(_MSC_VER)
#define EXPORT __declspec(dllexport)
#define IMPORT __declspec(dllimport)
#elif defined(__GNUC__)
//  GCC
#define EXPORT __attribute__((visibility("default")))
#define IMPORT
#else
//  do nothing and hope for the best?
#define EXPORT
#define IMPORT
#pragma warning Unknown dynamic link import/export semantics.
#endif

#ifdef CAIRNCORE_LIBRARY
#define DECLSPEC EXPORT
#else
#define DECLSPEC IMPORT
#endif

class DECLSPEC CairnAPI
{
public:    
	CairnAPI(bool a_Log = true);
	~CairnAPI();
        
    enum ESettingsLimited
    {
        all = 0, 
        mandatory, 
        optional,
		used
    };
	  
    
    // -- List of possibles components
    
    // Return the list of the all possibles Component
    t_list get_PossibleModelNames(); // --> donne les noms des modèles nécessaires à la création de composants (liste des classes)

    // Return the list of the all possibles component types 
    t_list get_PossibleComponentTypes(); // converter, sources, grid, etc

	// Return the type of Component given its Model name
	std::string get_ComponentType(const std::string& a_Model); 

    // Return the list of the possibles model class of a type
    t_list get_TechnoTypes(const std::string& a_ComponentCategory); // electrolyzer, PAC, 
	
	t_list get_Models(const std::string& a_TechnoType); // electrolyzerDetail, electrolyzerBasic ...

	// Return the list of the possibles EnergyCarrier type
	t_list get_EnergyCarrierTypes();

    t_list get_ModelAttributs(
        const std::string& a_ModelClass,
        const std::string& a_SettingsType, // param, option,timeseries, ports par défaut
        ESettingsLimited a_setLimited = ESettingsLimited::all); // paramètres optionnels, obligatoires, tous les paramètres

	t_value get_DefaultParameter(
		const std::string& a_ModelClass,
		const std::string& a_attributeName);


    // Return the list of the possibles Solver
    t_list get_Solvers();

	// --------------------------------------------------------------------------------
	class DECLSPEC ObjectAPI {
	public:
		ObjectAPI();
		ObjectAPI(const std::string& a_Name, const std::string& a_Type);		
		virtual const std::string& get_Name() const;
		virtual const std::string& get_Type();
		void set_Type(const std::string& a_Type);

		// Returns the value of an existing setting
		virtual t_value get_SettingValue(const std::string& a_SettingName);
		// Returns the name and the value of all settings
		virtual t_dict get_SettingValues();

		// Set the value of an existing setting
		virtual void set_SettingValue(const std::string& a_SettingName, const t_value& a_SettingValue);
		// set values of several settings
		virtual void set_SettingValues(const t_dict& a_SettingValues);
	protected:
		t_dict m_Params;
		std::string m_Name;
		std::string m_Type;		
	};

    // Definition of an EnergyVector
	class DECLSPEC EnergyVectorAPI : public ObjectAPI {
	public:
		EnergyVectorAPI();		
		EnergyVectorAPI(const std::string& a_Name, const std::string& a_Type);
		void set_EnergyVector(class EnergyVector* ap_EnergyVector);
		class EnergyVector* get_EnergyVector() const;
		
		// -- Settings ---
		// Returns the list of names 
		t_list get_SettingsList(ESettingsLimited a_setLimited = ESettingsLimited::all);

		// Returns the value of an existing setting
		t_value get_SettingValue(const std::string& a_SettingName);
		// Returns the name and the value of all settings
		t_dict get_SettingValues();

		// Set the value of an existing setting
		void set_SettingValue(const std::string& a_SettingName, const t_value& a_SettingValue);
		// set values of several settings
		void set_SettingValues(const t_dict& a_SettingValues);

	private:
		class EnergyVector* m_EnergyVector{ nullptr };
	};

	// --------------------------------------------------------------------------------	
	class DECLSPEC MilpPortAPI : public ObjectAPI {
	public:
		MilpPortAPI();
		MilpPortAPI(const std::string& a_Name, const EnergyVectorAPI& a_EnergyVector);
		void set_Port(class MilpPort* ap_Port);
		//std::string get_ID() const;
		std::string get_EnergyVector() const;
		class MilpPort* get_MilpPort() const;
		

		// -- Settings ---
		// Returns the list of names contains in the <settingsType> parameter ('options', 'params', 'timeseries')
		t_list get_SettingsList(ESettingsLimited a_setLimited = ESettingsLimited::all);

		// Returns the value of an existing settingsType
		t_value get_SettingValue(const std::string& a_SettingName);
		t_dict get_SettingValues();

		// Set the value of an existing settings
		void set_SettingValue(const std::string& a_SettingName, const t_value& a_SettingValue);
		void set_SettingValues(const t_dict& a_SettingValues);

		// Check if all mandatory settings are initialized
		bool checkSettings(std::string &a_ErrMsg);

	private:
		//std::string m_ID;
		class MilpPort* m_Port{ nullptr };
		class EnergyVector* m_EnergyVector{ nullptr };
	};

	// --------------------------------------------------------------------------------	
	class DECLSPEC BusAPI : public ObjectAPI {
	public:
		BusAPI();
		BusAPI(const std::string& a_Name, const std::string& a_Type, const std::string& a_ModelName, const EnergyVectorAPI& a_EnergyVector);
		BusAPI(const std::string& a_Name, const std::string& a_ModelName, const EnergyVectorAPI& a_EnergyVector);
		void set_Bus(class BusCompo* ap_Bus);
		
		void set_ModelClass(const std::string& a_ModelName);
		const std::string get_ModelClass();

		std::string get_EnergyVector() const;
		class BusCompo* get_BusCompo() const;

		// -- Settings ---
		// Returns the list of names contains in the <settingsType> parameter ('options', 'params', 'timeseries')
		t_list get_SettingsList(ESettingsLimited a_setLimited = ESettingsLimited::all);

		// Returns the value of an existing settingsType
		t_value get_SettingValue(const std::string& a_SettingName);
		t_dict get_SettingValues();

		// Set the value of an existing settings
		void set_SettingValue(const std::string& a_SettingName, const t_value& a_SettingValue);
		void set_SettingValues(const t_dict& a_SettingValues);

	private:
		class BusCompo* m_Bus{ nullptr };
		class EnergyVector* m_EnergyVector{ nullptr };
		std::string m_ModelClass;
	};

	// --------------------------------------------------------------------------------	
	class DECLSPEC SolutionAPI {
	public:
		SolutionAPI();
		// internal methods
		void set_Problem(class OptimProblem* ap_Problem);
		void set_Results(int a_Step);
		void set_Status(int a_Status);
		const double* getOptimalSolution(int a_numSol) const;

		// get the status of the solution
		std::string get_Status() const;

		// return number of solutions
		int get_NbSolutions() const;

		// ---- export --------------------
		// export solutions in CSV files
		void exportTimeSeries(const std::string& a_path="", int a_numSol = 0); // si -1 toutes ?
		void exportPLAN(const std::string& a_path, int a_numSol = 0);
		void exportHIST(const std::string& a_path, int a_numSol = 0);
		void exportIndicators(const std::string& a_path, int a_numSol = 0);

		// get solutions in tab
		t_dictValues get_TimeSeries(int a_numSol = 0);
		t_values get_Times(int a_numSol = 0);
		t_values get_Values(const std::string& a_Name, int a_numSol = 0);

		static std::string s_Time;

	private:
		class OptimProblem* m_Problem{ nullptr };
		int m_status{ 0 };
		t_dictValues m_timeSeries;
	};
	// --------------------------------------------------------------------------------	
	class DECLSPEC MilpComponentAPI : public ObjectAPI {
	public:
		MilpComponentAPI();
		MilpComponentAPI(const std::string& a_Name, const std::string& a_Type, const std::string& a_ModelName);
		MilpComponentAPI(const std::string& a_Name, const std::string& a_ModelName);
		void set_MilpComponent(class MilpComponent* ap_Component);
		class MilpComponent* get_MilpComponent() const;

		void set_ModelClass(const std::string& a_ModelName);
		const std::string get_ModelClass();

		bool isTimeSeriesParam(const std::string& a_TimeSeriesName);

		// -- Settings ---
		// Returns the list of names contains in the <settingsType> parameter ('options', 'params', 'timeseries')
		t_list get_SettingsList(ESettingsLimited a_setLimited = ESettingsLimited::all);

		// Returns the value of an existing settingsType
		t_value get_SettingValue(const std::string& a_SettingName);
		t_dict get_SettingValues();
		t_value get_TimeSeriesVector(const std::string& a_SettingName);

		// Set the value of an existing settings
		void set_SettingValue(const std::string& a_SettingName, const t_value& a_SettingValue);
		void set_SettingValues(const t_dict& a_SettingValues);
		void set_TimeSeriesVector(const std::string& a_TimeSeriesName, const std::vector<double> a_TimeSeriesValue);

		// -- Ports ---
		t_list get_Ports();
		MilpPortAPI get_Port(const std::string& a_Name);
		void add(MilpPortAPI &a_Port);
		void remove(MilpPortAPI& a_Port);
		void removePort(MilpPortAPI& a_Port, const bool reinitializeCompo=true);

		void reinitialize();

		bool useEnergyVector(const std::string& a_EnergyVectorName);
		void get_Links(t_dict &a_Links);

		// -- Indicators ---
		t_list get_IndicatorNames();
		t_list get_IndicatorUnits();
		t_list get_IndicatorShortNames();
		t_dict get_IndicatorValues(const std::string="PLAN");
		double get_IndicatorValue(const std::string& name, const std::string range = "PLAN");

		// Returns the list of variables contains the component
		t_list get_VarList();

		// -- Solution ---
		t_value get_varValue(const std::string& a_VarName); // , const SolutionAPI& a_solution, int a_numSol = 0);
		t_dict get_varValues(); // (const SolutionAPI& a_solution, int a_numSol = 0);

		//isOptimized
		t_value isOptimized();

	private:
		class MilpComponent* m_Component{ nullptr };		
		std::string m_ModelClass;
		std::vector<MilpPortAPI*> m_Ports;
	};

	// --------------------------------------------------------------------------------	
    // Definition of a study (OptimProblem)
	class DECLSPEC OptimProblemAPI
	{
	public:
		OptimProblemAPI();		
		void set_Problem(class OptimProblem* ap_Problem);

		// rename the study
		void set_StudyName(const std::string& a_Name);
		
		//  Writes the file corresponding to the model. Save Mode = Fast or Slow (localise the components graphically better) 
		void save_Study(const std::string& a_filename = "", const std::string& a_posAlgorithm = "");

		// -- EnergyCarriers ---
		// Returns the list of all energy carrier in the problem. 
		t_list get_EnergyCarriers();
		EnergyVectorAPI get_EnergyCarrier(const std::string &a_Name);
		void add(EnergyVectorAPI& a_EnergyVector);
		void remove(EnergyVectorAPI& a_EnergyVector);		

		// -- Components ---
		// Returns the list of all components in the problem (names). 
		t_list get_Components(const std::string &a_Category = "");
		MilpComponentAPI get_Component(const std::string &a_Name);

		//  Create the component named *a_Name*, of type *a_ComponentType* with the given options
		void add(MilpComponentAPI& a_Component);
		
		//  remove a component (and its associated links) from its name.
		void remove(MilpComponentAPI& a_Component);


		// -- Bus ---
		// Returne the list of all bus
		t_list get_Buses();
		BusAPI get_Bus(const std::string &a_Name);
		//  Create the bus named *a_BusName*, of type *a_BusType* with the given options, return error code
		void add(BusAPI& a_Bus);
		//  remove a bus (and its associated links) from its name.
		void remove(BusAPI& a_Bus);


		// -- Links ---
		// Returne the list of all links
		t_dict get_Links();		
		// Create a link between two existing components of the model.
		void add(MilpPortAPI& a_port, BusAPI& a_bus);
		void add(BusAPI &a_bus, MilpPortAPI &a_port);
		
		// Remove a link between two existing components of the model.
		void remove(MilpPortAPI& a_port, BusAPI& a_bus);
		void remove(BusAPI& a_bus, MilpPortAPI& a_port);


		//  Get/Set parameters of the analysis component of the problem
		t_dict get_TechEcoAnalysisSettings();
		void set_TechEcoAnalysisSettings(const t_dict& a_Settings);

		//  Get/Set parameters of the solver component of the problem
		t_dict get_MIPSolverSettings();
		void set_MIPSolverSettings(const t_dict& a_Settings);

		//  Get/Set parameters of the simulation component of the problem
		t_dict get_SimulationControlSettings();
		void set_SimulationControlSettings(const t_dict& a_Settings);


		// -- Run ---
		 // Adds a time series file.
		void add_TimeSeries(const std::string& a_fileName);


		// The following methods allow the user to write Cairn input timeseries:
		// Adds a time serie to the model time series list.
		void add_TimeSeries(const std::string& a_serie_name, 
			const std::string& a_description, const std::string& a_unit, 
			const t_values& a_times, const t_values& a_values);

		// run_Cairn
		void initialize();		
		SolutionAPI run(const std::string &a_resultsPath = "");
		
		// Indicators
		t_list get_Indicators(const std::string& a_Category = "");

		//optimized components (those that have negative VarSize)
		t_list get_optimized_components();

	private:
		t_list m_timestepfileList;
		
		class OptimProblem* m_Problem{ nullptr };
	};

    // -- Creation of a study ---
    OptimProblemAPI create_Study(const std::string& a_StudyName);

	// Read a study specified by 'a_filename'
	OptimProblemAPI read_Study(const std::string& a_filename);

	// close the current Study
	void close_Study();

private:
    class CairnCore* m_Cairn{ nullptr };
};

#endif