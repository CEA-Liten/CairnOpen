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
	class OptimProblemAPI;
	class MilpComponentAPI;

	CairnAPI(bool a_Log = true);
	~CairnAPI();
        
    enum ESettingsLimited
    {
        all = 0, 
        mandatory, 
        optional,
		used
    };

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
		ObjectAPI(class CairnObject* ap_Object = nullptr);
	
		std::string get_Name() const;
		std::string get_ObjectType() const;
		virtual void rename(const std::string& name);

		class CairnObject* get_Object() const;
		void set_Object(class CairnObject* ap_EnergyVector);

		//returns a list of parameter names
		t_list get_SettingsList();
		t_list get_SettingsListByType(ESettingsLimited a_setLimited = ESettingsLimited::all);

		virtual t_value get_SettingValue(const std::string& a_SettingName);
		virtual t_dict get_SettingValues();

		virtual void set_SettingValue(const std::string& a_SettingName, const t_value& a_SettingValue, bool checkExistance = true);
		virtual void set_SettingValues(const t_dict& a_SettingValues);

		t_list get_ShowConfigList();
		std::string get_SettingShowConfig(const std::string& a_SettingName);

		bool get_SettingMandatoryValue(const std::string& a_SettingName);
		bool is_DependentSetting(const std::string& a_SettingName);

		std::string get_SettingUnit(const std::string& a_SettingName);

	protected:
		class CairnObject* m_Object{ nullptr };
		std::vector<class InputParam*> get_InputParams();
	};

	// --------------------------------------------------------------------------------
	class DECLSPEC SolverAPI : public ObjectAPI {
	public:
		SolverAPI();		
		virtual void set_SettingValue(const std::string& a_SettingName, const t_value& a_SettingValue, bool checkExistance = true);
		virtual void set_SettingValues(const t_dict& a_SettingValues);	
	};

	// --------------------------------------------------------------------------------
	class DECLSPEC SimulationControlAPI : public ObjectAPI {
	public:
		SimulationControlAPI();
		virtual void set_SettingValue(const std::string& a_SettingName, const t_value& a_SettingValue, bool checkExistance = true);
		virtual void set_SettingValues(const t_dict& a_SettingValues);

	protected:		
		void updateMilpData();
	};

	// --------------------------------------------------------------------------------
	class DECLSPEC TecEcoAnalysisAPI : public ObjectAPI {
	public:
		TecEcoAnalysisAPI();
		virtual void set_SettingValue(const std::string& a_SettingName, const t_value& a_SettingValue, bool checkExistance = true);
		virtual void set_SettingValues(const t_dict& a_SettingValues);	
	};

	// --------------------------------------------------------------------------------
	class DECLSPEC EnergyVectorAPI : public ObjectAPI {
	public:
		EnergyVectorAPI(class EnergyVector* ap_EnergyVector = nullptr);
		EnergyVectorAPI(const OptimProblemAPI& a_Problem, const std::string& a_Name, 
			const std::string& a_Type);
		
		std::string get_Type() const;
		class EnergyVector* get_EnergyVector() const;
		void set_EnergyVector(class EnergyVector* ap_EnergyVector);
		
		void set_SettingValue(const std::string& a_SettingName, const t_value& a_SettingValue, bool checkExistance = true);
		void set_SettingValues(const t_dict& a_SettingValues);
	};

	// --------------------------------------------------------------------------------	
	class DECLSPEC MilpPortAPI : public ObjectAPI {
	public:
		MilpPortAPI(class MilpPort* ap_Port=nullptr);
		MilpPortAPI(MilpComponentAPI& a_Component, const std::string& a_Name, const EnergyVectorAPI& a_EnergyVector,
			const std::string& a_Direction="DATAEXCHANGE", const std::string& a_Variable="");

		class MilpPort* get_MilpPort() const;
		void set_MilpPort(class MilpPort* ap_Port);

		std::string get_ID() const;
		std::string get_CarrierName() const;
		std::string get_Variable() const;

		void set_SettingValue(const std::string& a_SettingName, const t_value& a_SettingValue, bool checkExistance = true);
		void set_SettingValues(const t_dict& a_SettingValues);

		EnergyVectorAPI get_EnergyCarrier();
		void set_EnergyCarrier(const EnergyVectorAPI& a_EnergyVector);
	};

	// --------------------------------------------------------------------------------	
	class DECLSPEC BusAPI : public ObjectAPI {
	public:
		BusAPI(class BusCompo* ap_Bus = nullptr);
		BusAPI(const OptimProblemAPI& a_Problem, const std::string& a_Name, 
			const std::string& a_ModelName, const EnergyVectorAPI& a_EnergyVector);

		class BusCompo* get_BusCompo() const;
		void set_BusCompo(class BusCompo* ap_Bus);
				
		std::string get_Type() const;
		std::string get_ModelClass() const;
		std::string get_CarrierName() const;
		void set_Carrier(const std::string& a_CarrierName);
		void set_Carrier(const EnergyVectorAPI& EnergyCarrier);

		std::string get_LabelValue(const std::string& a_Label) const;
		void set_LabelValue(const std::string& a_Label, const std::string& a_Value);
		//use t_dict ?!
		std::map<std::string, std::string> get_LabelValues() const;
		void set_LabelValues(const std::map<std::string, std::string>& a_Labels);
		
		void set_SettingValue(const std::string& a_SettingName, const t_value& a_SettingValue, bool checkExistance = true);
		void set_SettingValues(const t_dict& a_SettingValues);
		
		// -- IOs ---
		// Returns the list of variables contains the component
		t_list get_VarList();
		t_value get_varValue(const std::string& a_VarName); 
		t_dict get_varValues();  

		// -- Indicators ---
		t_list get_IndicatorNames();
		t_list get_IndicatorUnits();
		t_list get_IndicatorShortNames();
		t_dict get_IndicatorValues(const std::string range = "PLAN");
		double get_IndicatorValue(const std::string& name, const std::string range = "PLAN");

	private:		
		void configure_Carrier(EnergyVector* vEnergyVector);
	};

	// --------------------------------------------------------------------------------	
	class DECLSPEC MilpComponentAPI : public ObjectAPI {
	public:
		MilpComponentAPI(class MilpComponent* ap_Component=nullptr);
		MilpComponentAPI(const OptimProblemAPI& a_Problem, const std::string& a_Name, const std::string& a_ModelName);

		class MilpComponent* get_MilpComponent() const;
		void set_MilpComponent(class MilpComponent* ap_Component);

		std::string get_Type() const;
		const std::string get_ModelClass();
		void set_ModelClass(const std::string& a_ModelClass) { m_ModelClass = a_ModelClass; }

		std::string get_Direction();
		
		std::string get_LabelValue(const std::string& a_Label) const;
		void set_LabelValue(const std::string& a_Label, const std::string& a_Value);
		//use t_dict ?!
		std::map<std::string, std::string> get_LabelValues() const;
		void set_LabelValues(const std::map<std::string, std::string>& a_Labels);

		// -- Parameters ---
	
		t_value get_SettingValue(const std::string& a_SettingName);
		t_dict get_SettingValues();
		t_value get_TimeSeriesVector(const std::string& a_SettingName);

		void set_SettingValue(const std::string& a_SettingName, const t_value& a_SettingValue, bool checkExistance = true);
		void set_SettingValues(const t_dict& a_SettingValues);
		void set_TimeSeriesVector(const std::string& a_TimeSeriesName, const std::vector<double> a_TimeSeriesValue);

		bool isTimeSeriesParam(const std::string& a_TimeSeriesName);
		
		void modify_ModelClass(const std::string& a_prevModelClass, const std::string& a_newModelClass);

		// -- Ports ---
		t_list get_Ports();
		t_list get_DefaultPorts();
		MilpPortAPI get_Port(const std::string& a_Name);
		MilpPortAPI add_Port(const std::string& a_Name, const EnergyVectorAPI& a_EnergyVector,
			const std::string& a_Direction="DATAEXCHANGE", const std::string& a_Variable = "", const bool& reinitializeCompo = true);
		bool remove_Port(MilpPortAPI& a_Port, const bool isDeleteCompo=false);

		bool useEnergyVector(const std::string& a_EnergyCarrierName);
		void get_Links(t_dict &a_Links);

		// -- IOs ---
		// Returns the list of variables contains the component
		t_list get_VarList();
		t_value get_varValue(const std::string& a_VarName); // , const SolutionAPI& a_solution, int a_numSol = 0);
		t_dict get_varValues(); // (const SolutionAPI& a_solution, int a_numSol = 0);


		// -- Indicators ---
		t_list get_IndicatorNames();
		t_list get_IndicatorUnits();
		t_list get_IndicatorShortNames();
		t_dict get_IndicatorValues(const std::string range = "PLAN");
		double get_IndicatorValue(const std::string& name, const std::string range = "PLAN");

		//isOptimized
		t_value isOptimized();
		t_value get_dimParam();

		/* Other methods */
		void redeclarePortImpactParameters();
		void removePortImpactParameters(const std::string& portName);
		t_value get_OptimalSizeExpression();

	private:
		void checkDefaultPortCarriers();		
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
		void exportTimeSeries(const std::string& a_path = "", int a_numSol = 0); // si -1 toutes ?
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
    // Definition of a study (OptimProblem)
	class DECLSPEC OptimProblemAPI
	{
	public:
		OptimProblemAPI();	
		class OptimProblem* get_Problem() const { return m_Problem; };
		void set_Problem(class OptimProblem* ap_Problem);
		void set_StudyName(const std::string& a_Name);
		void save_Study(const std::string& a_filename = "", const std::string& a_posAlgorithm = "");

		// Export parameters to a file
		void export_Parameters(const std::string& fileName = "", const std::map<std::string, bool>& optionsMap = {});

		// Export results to a file
		void export_PLAN(const std::string& fileName = "", const int& aNsol = 0);

		void add_Label(const std::string& a_Label);
		void remove_Label(const std::string& a_Label);
		t_list get_Labels() const;
		void set_Labels(const t_list& a_Labels);

		// --------- Objects ---------
		t_list get_Objects();
		ObjectAPI get_Object(const std::string& a_Name);

		// --------- EnergyCarriers ---------
		t_list get_EnergyCarriers();
		EnergyVectorAPI get_EnergyCarrier(const std::string& a_Name);
		EnergyVectorAPI create_EnergyCarrier(const std::string& a_Name, const std::string& a_Type) const;
		void remove_EnergyCarrier(const std::string& a_Name);
		void remove_EnergyCarrier(EnergyVectorAPI& a_EnergyVector);

		// --------- Components ---------
		t_list get_Components(const std::string &a_Category = "");
		MilpComponentAPI get_Component(const std::string &a_Name);
		MilpComponentAPI create_Component(const std::string& a_Name, const std::string& a_ModelName) const;
		void remove_Component(const std::string& a_Name);
		void remove_Component(MilpComponentAPI& a_Component);

		// --------- Bus ----------
		t_list get_Buses(); // Returne the list of all bus names
		BusAPI get_Bus(const std::string &a_Name);
		BusAPI create_Bus(const std::string& a_Name, const std::string& a_ModelName, 
			const EnergyVectorAPI& a_EnergyVector) const;
		void remove_Bus(const std::string& a_Name);
		void remove_Bus(BusAPI& a_Bus);

		// --------- Links ---------
		// Returne the list of all links
		t_dict get_Links();		
		// Create a link between two existing components of the model.
		void add(MilpPortAPI& a_port, BusAPI& a_bus);
		void add(BusAPI &a_bus, MilpPortAPI &a_port);
		
		// Remove a link between two existing components of the model.
		void remove(MilpPortAPI& a_port, BusAPI& a_bus);
		void remove(BusAPI& a_bus, MilpPortAPI& a_port);

		// --------- TecEcoAnalysis ---------
		TecEcoAnalysisAPI get_TecEcoAnalysis();

		// --------- Solver ---------
		SolverAPI get_Solver();
		
		// --------- SimulationControl ---------
		SimulationControlAPI get_SimulationControl();
		
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
		t_list get_TecEco_IndicatorNames();
		t_list get_TecEco_IndicatorUnits();
		t_list get_TecEco_IndicatorShortNames();
		t_dict get_TecEco_IndicatorValues(const std::string range = "PLAN");
		double get_TecEco_IndicatorValue(const std::string& name, const std::string range = "PLAN");

		t_dict get_All_IndicatorValues(const std::string range = "PLAN"); //return the indicator values of all components

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
	bool m_LogConsole{ true };
};

#endif