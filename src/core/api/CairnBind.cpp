#include "CairnAPI.h"
#include <Python.h>
#include "pybind11/pybind11.h"
#include "pybind11/stl.h"


namespace py = pybind11;
class PyObjectAPI : public CairnAPI::ObjectAPI , public py::trampoline_self_life_support  {
public:    
    using  CairnAPI::ObjectAPI::ObjectAPI;

    void rename(const std::string& name) override {
        PYBIND11_OVERRIDE_PURE(
            void, /* Return type */
            CairnAPI::ObjectAPI,      /* Parent class */
            rename,          /* Name of function in C++ (must match Python name) */
            name      /* Argument(s) */
        );
    }
    t_value get_SettingValue(const std::string& a_SettingName) override {
        PYBIND11_OVERRIDE_PURE(
            t_value, /* Return type */
            CairnAPI::ObjectAPI,      /* Parent class */
            get_SettingValue,          /* Name of function in C++ (must match Python name) */
            a_SettingName      /* Argument(s) */
        );
    }
    t_dict get_SettingValues() override {
        PYBIND11_OVERRIDE_PURE(
            t_dict, /* Return type */
            CairnAPI::ObjectAPI,      /* Parent class */
            get_SettingValues          /* Name of function in C++ (must match Python name) */            
        );
    }
    void set_SettingValue(const std::string& a_SettingName, const t_value& a_SettingValue, bool checkExistance = true) override {
        PYBIND11_OVERRIDE_PURE(
            void, /* Return type */
            CairnAPI::ObjectAPI,      /* Parent class */
            set_SettingValue,          /* Name of function in C++ (must match Python name) */
            a_SettingName,
            a_SettingValue,
            checkExistance
        );
    }
    void set_SettingValues(const t_dict& a_SettingValues) override {
        PYBIND11_OVERRIDE_PURE(
            void, /* Return type */
            CairnAPI::ObjectAPI,      /* Parent class */
            set_SettingValues,          /* Name of function in C++ (must match Python name) */
            a_SettingValues
        );
    }
};
 

PYBIND11_MODULE(cairn, m) {

    m.doc() = R"X(Cairn is developped by CEA)X";

    py::class_<CairnAPI> cairn(m, "CairnAPI");

    py::enum_<CairnAPI::ESettingsLimited>(cairn, "ESettingsLimited")
        .value("all", CairnAPI::ESettingsLimited::all)
        .value("mandatory", CairnAPI::ESettingsLimited::mandatory)
        .value("optional", CairnAPI::ESettingsLimited::optional)
        .value("used", CairnAPI::ESettingsLimited::used)
        .export_values();

    cairn.def(py::init<>())
        .def(py::init<bool>())
        .def(py::init<t_dict>())
        .def("create_study", &CairnAPI::create_Study, "creates study with a given name")
        .def("read_study", &CairnAPI::read_Study, "reads a study from file", py::arg("fileName"))
        .def("get_study", &CairnAPI::get_Study, "get the current study")
        .def("component_type", &CairnAPI::get_ComponentType, "returns the type of a given model", py::arg("model"))
        .def_property_readonly("component_types", &CairnAPI::get_PossibleComponentTypes)
        .def_property_readonly("all_models", &CairnAPI::get_PossibleModelNames)
        .def_property_readonly("carrier_types", &CairnAPI::get_EnergyCarrierTypes)
        .def_property_readonly("solvers", &CairnAPI::get_Solvers)
        .def("close_study", &CairnAPI::close_Study, "close the current study");

    py::class_<CairnAPI::OptimProblemAPI>(m, "OptimProblem")
        .def("export_parameters", &CairnAPI::OptimProblemAPI::export_Parameters, py::arg("fileName") = "", py::arg("encoding") = "UTF-8",
            py::arg("optionsMap") = py::dict(), py::arg("extraData") = py::dict(), "exports the study parameters to a csv file")
        .def("copy_component", &CairnAPI::OptimProblemAPI::copy_Component, "copy a MilpComponent", py::arg("name"), py::arg("newName"), py::arg("copyLinks") = true)
        .def("import_group", &CairnAPI::OptimProblemAPI::import_Group, "import a group as json file; returns list of components and buses", py::arg("filename"))
        .def("export_plan", &CairnAPI::OptimProblemAPI::export_PLAN, "exports PLAN results to a csv file", py::arg("filename") = "", py::arg("solNb") = 0)
        .def("add_link", py::overload_cast<CairnAPI::MilpPortAPI&, CairnAPI::BusAPI&>(&CairnAPI::OptimProblemAPI::add),"adds a link between two given ports")
        .def("remove_link", py::overload_cast<CairnAPI::MilpPortAPI&, CairnAPI::BusAPI&>(&CairnAPI::OptimProblemAPI::remove), "removes a given link")
        .def("save_study", &CairnAPI::OptimProblemAPI::save_Study, py::arg("filename"), py::arg("mode") = "", "saves a study with a given name, relative or absolute path (it can also take a mode to position the components e.g. gradient)")
        .def("add_timeseries", py::overload_cast<const std::string&>(&CairnAPI::OptimProblemAPI::add_TimeSeries), "adds a given timeseries file")
        .def("add_onetimeseries", py::overload_cast<const t_dict&>(&CairnAPI::OptimProblemAPI::add_TimeSeries), "adds one timeseries in dictionary format")
        .def("run", &CairnAPI::OptimProblemAPI::run, py::arg("resultsPath") = "", py::arg("coSim") = false, "runs the optim problem")
        .def("run_sensitivity", &CairnAPI::OptimProblemAPI::runSensitivity, py::arg("sampling"), py::arg("max_time"), py::arg("indicators"), "runs sensitivity study")
        .def("create_bus", &CairnAPI::OptimProblemAPI::create_Bus, "creates and returns a new bus with a given name, model and energy carrier, e.g., create_bus('H2_Bus', 'NodeLaw', vH2)")
        .def("get_bus", &CairnAPI::OptimProblemAPI::get_Bus, "returns a given bus")
        .def("remove_bus", py::overload_cast<CairnAPI::BusAPI&>(&CairnAPI::OptimProblemAPI::remove_Bus), "removes a given bus")
        .def("create_energy_carrier", &CairnAPI::OptimProblemAPI::create_EnergyCarrier, py::arg("name"), py::arg("type") = "MaterialCarrier", py::arg("technoType") = "Material", "creates and returns a new energy carrier with a given name and type, e.g., create_energy_carrier('H2', 'MaterialCarrier', 'H2Vector')")
        .def("get_energy_carrier", &CairnAPI::OptimProblemAPI::get_EnergyCarrier, "returns a given energy carrier")
        .def("remove_energy_carrier", py::overload_cast<CairnAPI::EnergyVectorAPI&>(&CairnAPI::OptimProblemAPI::remove_EnergyCarrier), "removes a given energy carrier from the optim problem")
        .def("create_component", &CairnAPI::OptimProblemAPI::create_Component, "creates and returns a new component with a given name and model, e.g., create_component('Elec_Grid', 'GridFree')")
        .def("get_component", &CairnAPI::OptimProblemAPI::get_Component, "returns a given component")
        .def("remove_component", py::overload_cast<CairnAPI::MilpComponentAPI&>(&CairnAPI::OptimProblemAPI::remove_Component), "removes a geiven component from the optim problem")
        .def("get_components", &CairnAPI::OptimProblemAPI::get_ComponentsByCategory, "returns all the components of the optim problem specifying the category if needed", py::arg("category") = "")
        .def("get_indicators_values", &CairnAPI::OptimProblemAPI::get_All_IndicatorValues, py::arg("range") = "PLAN", "returns the indicators of all components for a given range, by default PLAN value")
        .def("get_tech_eco_analysis", &CairnAPI::OptimProblemAPI::get_TecEcoAnalysis, "return TecEcoAnalysis")
        .def("get_simulation_control", &CairnAPI::OptimProblemAPI::get_SimulationControl, "return SimulationControl")
        .def("get_solver", &CairnAPI::OptimProblemAPI::get_Solver, "return Solver")
        .def("add_label", &CairnAPI::OptimProblemAPI::add_Label, "add a label to the problem")
        .def("remove_label", &CairnAPI::OptimProblemAPI::remove_Label, "remove a label from the problem")
        .def_property("labels", &CairnAPI::OptimProblemAPI::get_Labels, &CairnAPI::OptimProblemAPI::set_Labels, "get/set the labels of the problem")
        .def("get_object", &CairnAPI::OptimProblemAPI::get_Object, "returns a given object")
        .def("get_subscribed_variables", &CairnAPI::OptimProblemAPI::getSubscribedVariables, "returns the list of the subscribed variables of the problem")
        .def("get_published_variables", &CairnAPI::OptimProblemAPI::getPublishedVariables, "returns the list of the published variables of the problem")
        .def("set_subscribed_variable_value", &CairnAPI::OptimProblemAPI::setSubscribedVariableValue, py::arg("name"), py::arg("values"), "set the values for a given subscribed variable of the problem")
        .def("get_published_variable_value", &CairnAPI::OptimProblemAPI::getPublishedVariableValue, py::arg("name"), "get the values for a given published variable of the problem")
        .def_property_readonly("objects", &CairnAPI::OptimProblemAPI::get_Objects)
        .def_property_readonly("components", &CairnAPI::OptimProblemAPI::get_Components, "returns all the components of the optim problem")
        .def_property_readonly("energy_carriers", &CairnAPI::OptimProblemAPI::get_EnergyCarriers, "returns all the energy carriers of the optim problem")
        .def_property_readonly("buses", &CairnAPI::OptimProblemAPI::get_Buses)
        .def_property_readonly("links", &CairnAPI::OptimProblemAPI::get_Links)
        .def_property_readonly("optimized_components", &CairnAPI::OptimProblemAPI::get_optimized_components)
        .doc() = "OptimProblem class.";

    py::class_<CairnAPI::ParamAPI>(m, "Param")
        .def(py::init())
        .def_property_readonly("name", &CairnAPI::ParamAPI::get_Name)
        .def_property_readonly("description", &CairnAPI::ParamAPI::get_Description)
        .def_property_readonly("unit", &CairnAPI::ParamAPI::get_Unit)
        .def_property("value", &CairnAPI::ParamAPI::get_Value, &CairnAPI::ParamAPI::set_Value, "get/set value")
        .def_property("comment", &CairnAPI::ParamAPI::get_Comment, &CairnAPI::ParamAPI::set_Comment, "get/set comment")
        .def_property_readonly("defaultValue", &CairnAPI::ParamAPI::get_Default)
        .def_property_readonly("min", &CairnAPI::ParamAPI::get_Min)
        .def_property_readonly("max", &CairnAPI::ParamAPI::get_Max)
        .def_property_readonly("isMandatory", &CairnAPI::ParamAPI::isMandatory)
        .def_property_readonly("isDependent", &CairnAPI::ParamAPI::isDependent)
        .def_property_readonly("show_config", &CairnAPI::ParamAPI::getShowConfig)
        .doc() = "Param class.";

    py::class_<CairnAPI::ObjectAPI, std::shared_ptr<CairnAPI::ObjectAPI>> (m, "CairnObject")
        .def(py::init())
        .def_property_readonly("name", &CairnAPI::ObjectAPI::get_Name)
        .def_property_readonly("objectType", &CairnAPI::ObjectAPI::get_ObjectType)
        .def("rename", &CairnAPI::ObjectAPI::rename, "rename EnergyVector component")
        .def("get_setting", &CairnAPI::ObjectAPI::get_Setting, "returns the class Param of a given parameter")
        .def("get_setting_value", &CairnAPI::ObjectAPI::get_SettingValue, "returns the value of a given parameter")
        .def("set_setting_value", &CairnAPI::ObjectAPI::set_SettingValue, py::arg("name"), py::arg("value"), py::arg("verify") = true, "sets the value of a given parameter")
        .def("get_settings", &CairnAPI::ObjectAPI::get_SettingsListByType, "returns a list of all (by default), used, optional or mandatory parameter names", py::arg("setLimited") = CairnAPI::ESettingsLimited::all)
        .def("get_setting_comment", &CairnAPI::ObjectAPI::get_SettingComment, "returns the comment of a given parameter")
        .def("set_setting_comment", &CairnAPI::ObjectAPI::set_SettingComment, py::arg("name"), py::arg("comment"), py::arg("verify") = true, "sets the comment of a given parameter")
        .def_property("setting_values", &CairnAPI::ObjectAPI::get_SettingValues, &CairnAPI::ObjectAPI::set_SettingValues, "get/set all parameter values as a dictionary")
        .def_property("setting_comments", &CairnAPI::ObjectAPI::get_SettingComments, &CairnAPI::ObjectAPI::set_SettingComments, "get/set all parameter comments as a dictionary")
        .def_property_readonly("show_config", &CairnAPI::ObjectAPI::get_SettingShowConfig, "returns ShowConfig of a given parameter. The parameter is displayed in the GUI when this ShowConfig is selected in the DataFilter")
        .def_property_readonly("show_configs", &CairnAPI::ObjectAPI::get_ShowConfigList, "returns a list of all ShowConfigs which is used in the GUI DataFilter")
        .def_property_readonly("settings", &CairnAPI::ObjectAPI::get_SettingsList, "returns a list of all parameter names")
        .def_property_readonly("performance_map_params", &CairnAPI::ObjectAPI::get_PerfParamList, "returns a list of all performance parameters")
        .doc() = "Object class.";

    py::class_<CairnAPI::EnergyVectorAPI, CairnAPI::ObjectAPI, std::shared_ptr<CairnAPI::EnergyVectorAPI>>(m, "EnergyVector")
        .def(py::init <const CairnAPI::OptimProblemAPI&, const std::string&, const std::string&, const std::string&>())
        .def_property_readonly("type", &CairnAPI::EnergyVectorAPI::get_Type)
        .def_property_readonly("techno_type", &CairnAPI::EnergyVectorAPI::get_TechnoType)
        .doc() = "EnergyCarrier class. The constructor takes several arguments: \
        arg1: OptimProblemAPI aProblem, the study where the EnergyVector will be added  \
        arg2: string aName to give the name of the energy vector \
        arg3: string aType to give the type of the energy vector (list available thanks to property carrier_types of CairnAPI)";

    py::class_<CairnAPI::MilpPortAPI, CairnAPI::ObjectAPI, std::shared_ptr<CairnAPI::MilpPortAPI>>(m, "Port")
        .def(py::init<CairnAPI::MilpComponentAPI&, const std::string&, const CairnAPI::EnergyVectorAPI&, const std::string&, const std::string&>(), 
            py::arg("component"), py::arg("name"), py::arg("carrier"), py::arg("direction") = "DATAEXCHANGE", py::arg("variable") = "")        
        .def("set_carrier", &CairnAPI::MilpPortAPI::set_EnergyCarrier, "sets the energy carrier of the port (takes componenet not name)")
        .def_property_readonly("carrier_name", &CairnAPI::MilpPortAPI::get_CarrierName, "return the name of energy carrier")
        .doc() = "Port class. The constructor takes several arguments: \
        arg1: MilpComponentAPI aComponent which will own the port \
        arg2: string aName to give the name of the port \
        arg3: EnergyVectorAPI aEnergyVector to give the energy vector of the port \
        arg3 (optional): string aDirection (Input, Output or DataExchange) to set the direction of the port\
        arg3 (optional): string aVariable to set the variable of the port";

    py::class_<CairnAPI::BusAPI, CairnAPI::ObjectAPI, std::shared_ptr<CairnAPI::BusAPI>>(m, "Bus")
        .def(py::init<const CairnAPI::OptimProblemAPI&, const std::string&, const std::string&, const CairnAPI::EnergyVectorAPI&>())   
        .def("get_port", &CairnAPI::BusAPI::get_Port, "returns a given port from the component")
        .def("add_port", &CairnAPI::BusAPI::add_Port, py::arg("name"), py::arg("carrier") = CairnAPI::EnergyVectorAPI(),
            py::arg("direction") = "DATAEXCHANGE", py::arg("variable") = "", py::arg("id") = "",
            "creates a new port for the bus")
        .def("remove_port", &CairnAPI::BusAPI::remove_Port, py::arg("port"), py::arg("isDelCompoFlag") = false, "removes a given port from the bus")
        .def("get_indicator_value", &CairnAPI::BusAPI::get_IndicatorValue, py::arg("name"), py::arg("range") = "PLAN", "returns the result of a given indicator for a given range, by default PLAN value")
        .def("get_indicators_values", &CairnAPI::BusAPI::get_IndicatorValues, py::arg("range") = "PLAN", "returns the  indicators for a given range, by default PLAN value")
        .def("get_var_value", &CairnAPI::BusAPI::get_varValue, "returns the result of a given IO for the optimal solution")
        .def("get_var_values", &CairnAPI::BusAPI::get_varValues, "returns the result of all IOs for the optimal solution")
        .def("get_label_value", &CairnAPI::BusAPI::get_LabelValue, "return the value of a given label")
        .def("set_label_value", &CairnAPI::BusAPI::set_LabelValue, "sets the value of a given label")
        .def_property_readonly("type", &CairnAPI::BusAPI::get_Type)
        .def_property_readonly("model_class", &CairnAPI::BusAPI::get_ModelClass)   
        .def_property_readonly("carrier", &CairnAPI::BusAPI::get_CarrierName)
        .def_property_readonly("possible_model_classes", &CairnAPI::BusAPI::get_PossibleModelClasses, "returns a list of model classes possible for this component.")
        .def_property_readonly("possible_control_types", &CairnAPI::BusAPI::get_PossibleControlValues, "returns a list of possible time control types for this component.")
        .def_property_readonly("possible_objective_types", &CairnAPI::BusAPI::get_PossibleObjectiveTypes, "returns a list of possible time objective types. Only valid for ManualObjective.")
        .def_property_readonly("ports", &CairnAPI::BusAPI::get_Ports, "returns the list of all port names")
        .def_property_readonly("default_ports", &CairnAPI::BusAPI::get_DefaultPorts, "returns the list of default port names")
        .def_property("label_values", &CairnAPI::BusAPI::get_LabelValues, &CairnAPI::BusAPI::set_LabelValues, "get/set all label values as a dictionary")
        .def_property_readonly("variables", &CairnAPI::BusAPI::get_VarList)
        .def_property_readonly("indicators", &CairnAPI::BusAPI::get_IndicatorNames)
        .def_property_readonly("indicators_shortnames", &CairnAPI::BusAPI::get_IndicatorShortNames)
        .def_property_readonly("indicators_units", &CairnAPI::BusAPI::get_IndicatorUnits)
        .doc() = "Bus class. The constructor takes several arguments: \
        arg1: OptimProblemAPI aProblem, the study where the Bus will be added  \
        arg2: string aName to give the name of the bus \
        arg3: string aModelName to give the model. Available Bus models are NodeLaw, NodeEquality and ManualObjective \
        arg4: string aEnergyVector to give the energy vector of the bus";

    py::class_<CairnAPI::MilpComponentAPI, CairnAPI::ObjectAPI, std::shared_ptr<CairnAPI::MilpComponentAPI>>(m, "Component")
        .def(py::init<const CairnAPI::OptimProblemAPI&, const std::string&, const std::string&>())        
        .def("get_port", &CairnAPI::MilpComponentAPI::get_Port, "returns a given port from the component")
        .def("add_port", &CairnAPI::MilpComponentAPI::add_Port, py::arg("name"), py::arg("carrier"), 
            py::arg("direction") = "DATAEXCHANGE", py::arg("variable") = "", py::arg("id") = "", py::arg("reinitializeCompo") = true, 
            "creates a new port for the component")
        .def("remove_port", &CairnAPI::MilpComponentAPI::remove_Port, py::arg("port"), py::arg("isDelCompoFlag") = false, "removes a given port from the component")
        //.def("get_timeseries_vector", &CairnAPI::MilpComponentAPI::get_TimeSeriesVector, "returns the vector value of a given timeseries parameter")
        //.def("set_timeseries_vector", &CairnAPI::MilpComponentAPI::set_TimeSeriesVector, "sets the vector value of a given timeseries parameter")
        .def("get_indicator_value", &CairnAPI::MilpComponentAPI::get_IndicatorValue, py::arg("name"), py::arg("range") = "PLAN", "returns the result of a given indicator for a given range, by default PLAN value")
        .def("get_indicators_values", &CairnAPI::MilpComponentAPI::get_IndicatorValues, py::arg("range") = "PLAN", "returns the  indicators for a given range, by default PLAN value")
        .def("get_var_value", &CairnAPI::MilpComponentAPI::get_varValue, "returns the result of a given IO for the optimal solution")
        .def("get_var_values", &CairnAPI::MilpComponentAPI::get_varValues, "returns the result of all IOs for the optimal solution")
        .def("get_label_value", &CairnAPI::MilpComponentAPI::get_LabelValue, "return the value of a given label")
        .def("set_label_value", &CairnAPI::MilpComponentAPI::set_LabelValue, "sets the value of a given label")        
        .def_property_readonly("type", &CairnAPI::MilpComponentAPI::get_Type)
        .def_property_readonly("dim_param", &CairnAPI::MilpComponentAPI::get_dimParam,"returns the parameter used for dimensionning")
        .def_property_readonly("model_class", &CairnAPI::MilpComponentAPI::get_ModelClass)
        .def_property_readonly("possible_model_classes", &CairnAPI::MilpComponentAPI::get_PossibleModelClasses, "returns a list of model classes possible for this component.")
        .def_property_readonly("possible_control_types", &CairnAPI::MilpComponentAPI::get_PossibleControlValues, "returns a list of possible time control types for this component.")
        .def_property_readonly("direction", &CairnAPI::MilpComponentAPI::get_Direction, "returns the direction based on the default port. Valid only for components of types Grid and SourceLoad. ")
        .def_property_readonly("ports", &CairnAPI::MilpComponentAPI::get_Ports, "returns the list of all port names")
        .def_property_readonly("default_ports", &CairnAPI::MilpComponentAPI::get_DefaultPorts, "returns the list of default port names")                
        .def_property("label_values", &CairnAPI::MilpComponentAPI::get_LabelValues, &CairnAPI::MilpComponentAPI::set_LabelValues, "get/set all label values as a dictionary")
        .def_property_readonly("variables", &CairnAPI::MilpComponentAPI::get_VarList)
        .def_property_readonly("indicators", &CairnAPI::MilpComponentAPI::get_IndicatorNames)
        .def_property_readonly("indicators_shortnames", &CairnAPI::MilpComponentAPI::get_IndicatorShortNames)
        .def_property_readonly("indicators_units", &CairnAPI::MilpComponentAPI::get_IndicatorUnits)
        .def_property_readonly("is_optimized", &CairnAPI::MilpComponentAPI::isOptimized)
        .def_property_readonly("optimal_size_expression", &CairnAPI::MilpComponentAPI::get_OptimalSizeExpression, "returns the name of optimal size (MaxSize) variable. Note, it works only after configuring the carriers of default ports.")
        .doc() = "Component class. The constructor takes several arguments: \
        arg1: OptimProblemAPI aProblem, the study where the component will be added  \
        arg2: string aName to give the name of the component \
        arg3: string aModelName to give the model (list available thanks to property all_models of CairnAPI). \
              Note that, to create componenets NodeLaw, NodeEquality and ManualObjective use BusAPI";

    py::class_<CairnAPI::TecEcoAnalysisAPI, CairnAPI::ObjectAPI, std::shared_ptr<CairnAPI::TecEcoAnalysisAPI>>(m, "TecEcoAnalysis")
        .def(py::init())
        .def("get_port", &CairnAPI::TecEcoAnalysisAPI::get_Port, "returns a given port from the component")
        .def("add_port", &CairnAPI::TecEcoAnalysisAPI::add_Port,
            py::arg("name"),
            py::arg("carrier"),
            py::arg("direction") = "DATAEXCHANGE",
            py::arg("variable") = "",
            py::arg("id") = "", 
            "creates a new port for the component")
        .def("remove_port", &CairnAPI::TecEcoAnalysisAPI::remove_Port,
            py::arg("port"),
            py::arg("isDelCompoFlag") = false,
            "removes a given port from the component")
        .def_property_readonly("ports", &CairnAPI::TecEcoAnalysisAPI::get_Ports, "returns the list of all port names")
        .def_property_readonly("default_ports", &CairnAPI::TecEcoAnalysisAPI::get_DefaultPorts, "returns the list of default port names")
        .def("get_indicator_value", &CairnAPI::TecEcoAnalysisAPI::get_IndicatorValue, py::arg("name"), py::arg("range") = "PLAN", "returns the result of a given TecEco indicator for a given range, by default PLAN value")
        .def("get_indicators_values", &CairnAPI::TecEcoAnalysisAPI::get_IndicatorValues, py::arg("range") = "PLAN", "returns the TecEco indicators for a given range, by default PLAN value")
        .def_property_readonly("indicators", &CairnAPI::TecEcoAnalysisAPI::get_IndicatorNames)
        .def_property_readonly("indicators_shortnames", &CairnAPI::TecEcoAnalysisAPI::get_IndicatorShortNames)
        .def_property_readonly("indicators_units", &CairnAPI::TecEcoAnalysisAPI::get_IndicatorUnits)
        .doc() = "TecEcoAnalysis class.";

    py::class_<CairnAPI::SimulationControlAPI, CairnAPI::ObjectAPI, std::shared_ptr<CairnAPI::SimulationControlAPI>>(m, "SimulationControl")
        .def(py::init())       
        .def_property_readonly("reading_modes", &CairnAPI::SimulationControlAPI::get_ReadingModes, "returns a list of possible timeseries reading mode.")
        .def_property_readonly("rolling_modes", &CairnAPI::SimulationControlAPI::get_RollingModes, "returns a list of possible rolling modes.")
        .doc() = "SimulationControl class.";

    py::class_<CairnAPI::SolverAPI, CairnAPI::ObjectAPI, std::shared_ptr<CairnAPI::SolverAPI>>(m, "Solver")
        .def(py::init())        
        .doc() = "Solver class.";


    py::class_<CairnAPI::SolutionAPI>(m, "Solution")
        .def(py::init())
        .def("get_ts_results", &CairnAPI::SolutionAPI::get_TimeSeries, "returns the output ts results")
        .def_property_readonly("status", &CairnAPI::SolutionAPI::get_Status, "returns the status of the run")
        .def_property_readonly("nb_solutions", &CairnAPI::SolutionAPI::get_NbSolutions, "returns the number of solutions found")
        .doc() = "Solution class.";
}