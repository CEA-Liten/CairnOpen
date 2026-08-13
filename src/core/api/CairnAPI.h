#ifndef CAIRNAPI_H
#define CAIRNAPI_H

#include <string>
#include <map>
#include <vector>
#include <variant>
#include <memory>
#include <functional>

typedef std::vector<std::string> t_list;
//Adding type bool to t_value will cause a problem for std::string being casted as bool
typedef std::variant<double, int, std::string, std::vector<std::string>, std::vector<double>, std::vector<int>> t_value;
typedef std::map<std::string, t_value> t_dict;
typedef std::map<std::string, std::string> t_dictComment;

typedef std::vector<t_value> t_values;
typedef std::map<std::string, t_values> t_dictValues;
typedef std::vector <std::map<std::string, t_value>> t_dicts;
typedef std::vector <t_dicts> t_dictsValues;



struct ExtraParameterData {
	std::string component;
	std::string parameter;
	std::string value;
};

enum class IndicatorProperty {
	Name,
	Unit,
	ShortName
};

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
	class ObjectAPI;

	CairnAPI();
	CairnAPI(bool a_Log);
	CairnAPI(t_dict a_DefLogs);
	~CairnAPI();
        
    enum ESettingsLimited
    {
        all = 0, 
        mandatory, 
        optional,
		used
    };

	enum ESettingsCategory {
		eAll = -1,
		eParameters = 0,
		eOptions,
		eTimeSeries,
		eEnvImpacts,
		ePortEnvImpacts
	};

    // Return the list of the all possibles Component
    t_list get_PossibleModelNames(); // --> donne les noms des modèles nécessaires à la création de composants (liste des classes)

    // Return the list of the all possibles component types 
    t_list get_PossibleComponentTypes(); // converter, sources, grid, etc

	// Return the type of Component given its Model name
	std::string get_ComponentType(const std::string& a_Model); 

    // Return the list of the possibles model class of a type
    t_list get_TechnoTypes(const std::string& a_ComponentCategory); // electrolyzer, PAC, 
	
	t_list get_Models(const std::string& a_TechnoType); // electrolyzerDetail, electrolyzer ...

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
    t_list get_Solvers() const;

	// --------------------------------------------------------------------------------	
	class DECLSPEC ParamAPI {
	public:
		ParamAPI(std::shared_ptr <class ObjectAPI> ap_Parent = nullptr, class ModelParam* ap_Param = nullptr);

		std::string get_Name() const;
		std::string get_Type() const;
		std::string get_Description() const;
		std::string get_Unit() const;

		t_value get_Value() const;
		std::string get_StrValue() const;
		void set_Value(const t_value& a_SettingValue);
		t_value get_Default() const;
		std::string get_StrDefaultValue() const;
		t_value get_Min() const;
		t_value get_Max() const;
		bool isMandatory() const;
		bool isDependent() const;
		bool isUsed() const;
		std::string getShowConfig() const;

		std::string get_Comment() const;
		void set_Comment(const std::string& a_SettingComment);

	protected:
		class ModelParam* m_Param{ nullptr };
		std::shared_ptr<ObjectAPI> m_Parent{ nullptr };
	};
	// --------------------------------------------------------------------------------
	class DECLSPEC ObjectAPI : public std::enable_shared_from_this<ObjectAPI> {
	public:
		ObjectAPI(class CairnObject* ap_Object = nullptr);
	
		class CairnObject* get_Object() const;
		void set_Object(class CairnObject* ap_EnergyVector);

		std::string get_ObjectType() const;
		std::string get_Name() const;
		virtual void rename(const std::string& name);

		//returns a list of parameter names
		t_list get_SettingsList();
		t_list get_SettingsListByType(ESettingsLimited a_setLimited = ESettingsLimited::all);
		
		//returns the list of parameter names for a given category (required for GUI)
		t_list get_SettingsListByCategory(ESettingsCategory category);

		ParamAPI get_Setting(const std::string& a_SettingName);

		virtual t_value get_SettingValue(const std::string& a_SettingName);
		virtual t_dict get_SettingValues();

		virtual void set_SettingValue(const std::string& a_SettingName, const t_value& a_SettingValue, bool checkExistance = true);
		virtual void set_SettingValues(const t_dict& a_SettingValues);

		std::string get_SettingComment(const std::string& a_SettingName);
		t_dictComment get_SettingComments();

		virtual void set_SettingComment(const std::string& a_SettingName, const std::string& a_SettingComment, bool checkExistence = true);
		void set_SettingComments(const t_dictComment& a_SettingComments);

		virtual t_list get_PerfParamList() const;

		virtual t_list get_VarList() const; // IO vars
		std::string get_VarDescription(const std::string& varName) const;

		virtual t_list get_ShowConfigList();

		std::string get_SettingShowConfig(const std::string& a_SettingName);

		bool is_MandatorySetting(const std::string& a_SettingName);
		bool is_UsedSetting(const std::string& a_SettingName);

		std::string get_SettingUnit(const std::string& a_SettingName);

	protected:
		class CairnObject* m_Object{ nullptr };
		std::vector<class InputParam*> get_InputParams();
		std::vector<class InputParam*> get_InputParams(ESettingsCategory category);
	};

	// --------------------------------------------------------------------------------
	class DECLSPEC SolverAPI : public ObjectAPI {
	public:
		SolverAPI();		
		virtual void set_SettingValue(const std::string& a_SettingName, const t_value& a_SettingValue, bool checkExistance = true);
		virtual void set_SettingValues(const t_dict& a_SettingValues);	

		class Solver* get_Solver() const;

		t_list get_ProblemTypes() const;
		t_list get_PossibleModelTypes() const;
	};

	// --------------------------------------------------------------------------------
	class DECLSPEC SimulationControlAPI : public ObjectAPI {
	public:
		SimulationControlAPI();
		virtual void set_SettingValue(const std::string& a_SettingName, const t_value& a_SettingValue, bool checkExistance = true);
		virtual void set_SettingValues(const t_dict& a_SettingValues);

		class SimulationControl* get_SimulationControl() const;

		t_list get_ReadingModes() const;
		t_list get_RollingModes() const;

	protected:		
		void updateMilpData();
	};

	// --------------------------------------------------------------------------------
	class DECLSPEC EnergyVectorAPI : public ObjectAPI {
	public:
		EnergyVectorAPI(class EnergyVector* ap_EnergyVector = nullptr);
		EnergyVectorAPI(const OptimProblemAPI& a_Problem, const std::string& a_Name, 
			const std::string& a_Type = "MaterialCarrier", const std::string& a_TechnoType = "Material");
		
		std::string get_Type() const;
		std::string get_TechnoType() const;
		class EnergyVector* get_EnergyVector() const;
		void set_EnergyVector(class EnergyVector* ap_EnergyVector);
		
		void set_SettingValue(const std::string& a_SettingName, const t_value& a_SettingValue, bool checkExistance = true);
		void set_SettingValues(const t_dict& a_SettingValues);

		void set_SettingComment(const std::string& a_SettingName, const std::string& a_SettingComment, bool checkExistence = true);
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

		void set_SettingComment(const std::string& a_SettingName, const std::string& a_SettingComment, bool checkExistence = true);

		std::shared_ptr < EnergyVectorAPI> get_EnergyCarrier();
		bool set_EnergyCarrier(const EnergyVectorAPI& a_EnergyVector);

	private:
		bool configureParentComponent();
	};

	// --------------------------------------------------------------------------------

	class DECLSPEC TecEcoAnalysisAPI : public ObjectAPI {
	public:
		TecEcoAnalysisAPI();

		class TecEcoCompo* get_TecEcoComponent() const;

		t_list get_PossibleOptimModels();

		virtual void set_SettingValue(const std::string& a_SettingName, const t_value& a_SettingValue, bool checkExistance = true);
		virtual void set_SettingValues(const t_dict& a_SettingValues);

		// -- Ports ---
		std::map<std::string, std::string> get_DefaultPortData(const std::string& portId) const;
		t_list get_DefaultPortIDs() const;
		t_list get_DefaultPorts() const; //names
		t_list get_Ports() const;
		std::shared_ptr < MilpPortAPI> get_Port(const std::string& a_Name);
		std::shared_ptr < MilpPortAPI> add_Port(const std::string& a_Name, const EnergyVectorAPI& a_EnergyVector,
			const std::string& a_Direction = "DATAEXCHANGE", const std::string& a_Variable = "", const std::string& a_PortId = "");
		bool remove_Port(MilpPortAPI& a_Port, const bool isDeleteCompo = false);

		bool useEnergyVector(const std::string& a_EnergyCarrierName);
		void get_Links(t_dict& a_Links);

		// -- Indicators TODO: move to ObjectAPI?
		t_list get_IndicatorNames() const;
		t_list get_IndicatorUnits() const;
		t_list get_IndicatorShortNames() const;
		t_dict get_IndicatorValues(const std::string& range = "PLAN") const;
		double get_IndicatorValue(const std::string& name, const std::string& range = "PLAN") const;

	private:
		t_list getIndicatorProperty(IndicatorProperty property) const;
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
		t_list get_PossibleModelClasses() const;
		t_list get_PossibleControlValues() const;
		t_list get_PossibleObjectiveTypes() const;

		std::string get_CarrierName() const;

		class EnergyVector* get_Carrier() const;
		void set_Carrier(const std::string& a_CarrierName);

		void set_Carrier(const EnergyVectorAPI& EnergyCarrier);

		std::string get_LabelValue(const std::string& a_Label) const;
		void set_LabelValue(const std::string& a_Label, const std::string& a_Value);
		t_dict get_LabelValues() const; // only string values are used
		void set_LabelValues(const t_dict& a_Labels);
		
		void set_SettingValue(const std::string& a_SettingName, const t_value& a_SettingValue, bool checkExistance = true);
		void set_SettingValues(const t_dict& a_SettingValues);
		
		void set_SettingComment(const std::string& a_SettingName, const std::string& a_SettingComment, bool checkExistence = true);

		// -- Ports ---
		std::map<std::string, std::string> get_DefaultPortData(const std::string& portId) const;
		t_list get_DefaultPortIDs() const;
		t_list get_DefaultPorts() const; //names
		t_list get_Ports() const;
		std::shared_ptr < MilpPortAPI> get_Port(const std::string& a_Name);
		std::shared_ptr < MilpPortAPI> add_Port(const std::string& a_Name, const EnergyVectorAPI& a_EnergyVector = EnergyVectorAPI(),
			const std::string& a_Direction = "DATAEXCHANGE", const std::string& a_Variable = "", const std::string& a_PortId = "");
		bool remove_Port(MilpPortAPI& a_Port, const bool isDeleteCompo = false);

		//bool useEnergyVector(const std::string& a_EnergyCarrierName);
		void get_Links(t_dict& a_Links);

		// -- IOs ---
		// Returns the list of variables contains the component
		//t_list get_VarList() const;
		t_value get_varValue(const std::string& a_VarName); 
		t_dict get_varValues();  

		// -- Indicators ---
		t_list get_IndicatorNames();
		t_list get_IndicatorUnits();
		t_list get_IndicatorShortNames();
		t_dict get_IndicatorValues(const std::string& range = "PLAN") const;
		double get_IndicatorValue(const std::string& name, const std::string& range = "PLAN") const;

	private:		
		void configure_Carrier(class EnergyVector* vEnergyVector);
	};

	// --------------------------------------------------------------------------------	

	class DECLSPEC MilpComponentAPI : public ObjectAPI {
	public:

		struct ComponentState {
			std::map<std::string, t_dict> portSettings;
			std::map<std::string, CairnAPI::EnergyVectorAPI> portCarriers;
			t_dict links;

			t_dict paramValues;
			t_dictComment paramComments;
			t_dict labelValues;
		};

		struct ReentranceGuard
		{
			explicit ReentranceGuard(bool& flag) : m_flag(flag) { m_flag = true; }
			~ReentranceGuard() { m_flag = false; } // always resets - even on exception
			bool& m_flag;
		};

		MilpComponentAPI(class MilpComponent* ap_Component=nullptr);
		MilpComponentAPI(const OptimProblemAPI& a_Problem, const std::string& a_Name, const std::string& a_ModelName);

		class MilpComponent* get_MilpComponent() const;
		void set_MilpComponent(class MilpComponent* ap_Component);

		std::string get_Type() const;
		std::string get_ModelClass() const;
		t_list get_PossibleModelClasses() const;
		t_list get_PossibleControlValues() const;

		std::string get_Direction();
		
		std::string get_LabelValue(const std::string& a_Label) const;
		void set_LabelValue(const std::string& a_Label, const std::string& a_Value);
		t_dict get_LabelValues() const; // only string values are used
		void set_LabelValues(const t_dict& a_Labels);

		// -- Parameters ---
	
		t_value get_SettingValue(const std::string& a_SettingName);
		t_dict get_SettingValues();
		t_value get_TimeSeriesVector(const std::string& a_TimeSeriesName);

		void set_SettingValue(const std::string& a_SettingName, const t_value& a_SettingValue, bool checkExistance = true);
		void set_SettingValues(const t_dict& a_SettingValues);

		void set_SettingComment(const std::string& a_SettingName, const std::string& a_SettingComment, bool checkExistence = true);

		bool isTimeSeriesParam(const std::string& a_TimeSeriesName);
		void set_TimeSeriesVector(const std::string& a_TimeSeriesName, const std::vector<double> a_TimeSeriesValue);

		void modify_ModelClass(const std::string& a_prevModelClass, const std::string& a_newModelClass);

		void rebuildModel();
		void reconfigureModel();

		t_list get_ShowConfigList() override;

		// -- Ports --- TODO: move port-related methods to ObjectAPI
		std::map<std::string, std::string> get_DefaultPortData(const std::string& portId) const;
		t_list get_DefaultPortIDs() const;
		t_list get_DefaultPorts() const; //names
		t_list get_Ports() const;				
		std::shared_ptr < MilpPortAPI> get_Port(const std::string& a_Name);
		std::shared_ptr < MilpPortAPI> add_Port(const std::string& a_Name, const EnergyVectorAPI& a_EnergyVector,
			const std::string& a_Direction="DATAEXCHANGE", const std::string& a_Variable = "", const std::string& a_PortId = "", 
			const bool& reinitializeCompo = true);
		bool remove_Port(MilpPortAPI& a_Port, const bool isDeleteCompo=false);

		bool useEnergyVector(const std::string& a_EnergyCarrierName);
		void get_Links(t_dict &a_Links);

		// -- IOs ---
		// Returns the list of variables contains the component
		t_list get_VarList() const;
		t_value get_varValue(const std::string& a_VarName); // , const SolutionAPI& a_solution, int a_numSol = 0);
		t_dict get_varValues(); // (const SolutionAPI& a_solution, int a_numSol = 0);

		// -- ControlVar ---
		std::vector<double> getControlVarHistValues(const std::string& a_name);

		// -- Indicators ---
		t_list get_IndicatorNames();
		t_list get_IndicatorUnits();
		t_list get_IndicatorShortNames();
		t_dict get_IndicatorValues(const std::string& range = "PLAN") const;
		double get_IndicatorValue(const std::string& name, const std::string& range = "PLAN") const;

		bool isInstalled() const;

		//isOptimized
		t_value isOptimized();
		t_value get_dimParam();

		/* Other methods */
		void redeclarePortImpactParameters();
		void removePortImpactParameters(const std::string& portName);
		std::string get_OptimalSizeExpression();

	private:
		void checkDefaultPortCarriers() const;	

		ComponentState saveComponentState();
		void rebuildComponentModel(const std::map<std::string, CairnAPI::EnergyVectorAPI>& portCarriers);
		void restoreComponentState(const ComponentState& st);
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

		std::map <std::string, std::string> import_Group_GUI(const std::string& a_filename); /* return <nameFromFile, name> */
		t_list import_Group(const std::string& a_filename);

		// Export parameters to a file
		void export_Parameters(const std::string& fileName = "", const std::string& encoding = "UTF-8",
			const std::map<std::string, bool>& optionsMap = {},
			const std::map< std::string, std::vector<ExtraParameterData> >& extraData = {});

		// Export results to a file
		void export_PLAN(const std::string& fileName = "", const int& aNsol = 0);

		void add_Label(const std::string& a_Label);
		void remove_Label(const std::string& a_Label);
		t_list get_Labels() const;
		void set_Labels(const t_list& a_Labels);

		// --------- Objects ---------
		t_list get_Objects();
		std::shared_ptr<ObjectAPI> get_Object(const std::string& a_Name);

		// --------- EnergyCarriers ---------
		t_list get_EnergyCarriers() const;
		std::shared_ptr <EnergyVectorAPI> get_EnergyCarrier(const std::string& a_Name) const;
		std::shared_ptr <EnergyVectorAPI> create_EnergyCarrier(const std::string& a_Name, 
			const std::string& a_Type = "MaterialCarrier", const std::string& a_TechnoType = "Material") const;
		void remove_EnergyCarrier(const std::string& a_Name, bool forceDeletion = false);
		void remove_EnergyCarrier(EnergyVectorAPI& a_EnergyVector); // Expose to PythonAPI
		void remove_EnergyCarrier(EnergyVectorAPI& a_EnergyVector, bool forceDeletion);

		// --------- Components ---------
		t_list get_Components() const;
		t_list get_ComponentsByCategory(const std::string& a_Category = "") const;
		std::shared_ptr <MilpComponentAPI> get_Component(const std::string &a_Name) const;
		std::shared_ptr <MilpComponentAPI> create_Component(const std::string& a_Name, const std::string& a_ModelName) const;
		void remove_Component(const std::string& a_Name);
		void remove_Component(MilpComponentAPI& a_Component);

		std::shared_ptr <MilpComponentAPI> copy_Component(const std::string& name, const std::string& newName, bool connexions = true);

		// --------- Bus ----------
		t_list get_Buses() const; // Returne the list of all bus names
		std::shared_ptr <BusAPI> get_Bus(const std::string &a_Name) const;
		std::shared_ptr <BusAPI> create_Bus(const std::string& a_Name, const std::string& a_ModelName,
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
		std::shared_ptr<TecEcoAnalysisAPI> get_TecEcoAnalysis() const;

		// --------- Solver ---------
		std::shared_ptr<SolverAPI> get_Solver() const;
		void set_Solver(const std::string& name) const; // solver name

		// --------- SimulationControl ---------
		std::shared_ptr<SimulationControlAPI> get_SimulationControl() const;
		
		// -- Run ---
		 // Adds a time series file.
		void add_TimeSeries(const std::string& a_fileName);

		// The following methods allow the user to write Cairn input timeseries:
		// Adds a time serie to the model time series list.
		void add_TimeSeries(const t_dict& a_TS);

		// -- Interfaces --
		t_list getSubscribedVariables();
		t_list getPublishedVariables();
		void setSubscribedVariableValue(const std::string& a_name, const std::vector<double>& a_values);
		std::vector<double> getPublishedVariableValue(const std::string& a_name);

		// run_Cairn
		void initialize();		
		SolutionAPI run(const std::string &a_resultsPath = "", const bool& a_coSim = false);
		
		void runSensitivityCSV(const std::string& a_samplingFileName, int a_max_time = -1, const std::string& a_indicatorsFileName = "");		
		t_dicts runSensitivity(const t_dictsValues& a_sampling, int a_max_time = -1, 
			const t_dicts& a_indicators = {}, std::function<void(int)> on_iter = nullptr);

		// Indicators
		t_dict get_All_IndicatorValues(const std::string& range = "PLAN") const; //return the indicator values of all components

		//optimized components (those that have negative VarSize)
		t_list get_optimized_components() const;

	private:
		class ModelValue {
		public:
			ModelValue(const t_dict& a_values);
			ModelValue(const std::string& a_Model, const std::string& a_Setting);						
			bool setValue(OptimProblemAPI& a_Problem, const std::string& a_Value);
			bool setValue(OptimProblemAPI& a_Problem);
			void reset();			
		private:
			std::string m_Model;
			std::string m_Port;
			std::string m_Setting;			
			t_value m_Value;
			t_value m_SaveValue;
			std::shared_ptr <CairnAPI::ObjectAPI> m_Component;

			bool get_Port(CairnAPI::MilpPortAPI& a_Port);
		};
		class KPI {
		public:
			KPI(const t_dict &a_values);
			KPI(const std::string& a_Model, const std::string& a_Indicator);
			void printHeader(std::fstream& f);
			void printValue(OptimProblemAPI& a_Problem, std::fstream& f);

			void printValue(OptimProblemAPI& a_Problem, t_dict& a_res);

		private:
			std::string m_Model;
			std::string m_Indicator;

			double getValue(OptimProblemAPI& a_Problem);
		};

		class OptimProblem* m_Problem{ nullptr };
	};

    // -- Creation of a study ---
    OptimProblemAPI create_Study(const std::string& a_StudyName);

	// Read a study specified by 'a_filename'
	OptimProblemAPI read_Study(const std::string& a_filename);

	// Get the current study
	OptimProblemAPI get_Study();

	// close the current Study
	void close_Study();

	CairnAPI::OptimProblemAPI apply_Compatibility_Script();

private:
    class CairnCore* m_Cairn{ nullptr };
};

#endif