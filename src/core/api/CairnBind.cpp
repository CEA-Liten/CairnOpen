#include "CairnAPI.h"
#include <Python.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>


namespace py = pybind11;

   
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
        .def("create_study", &CairnAPI::create_Study, "creates study with a given name")
        .def("read_study", &CairnAPI::read_Study, "reads a study from file", py::arg("fileName"))
        .def("component_type", &CairnAPI::get_ComponentType, "returns the type of a given model", py::arg("model"))
        .def_property_readonly("component_types", &CairnAPI::get_PossibleComponentTypes)
        .def_property_readonly("all_models", &CairnAPI::get_PossibleModelNames)
        .def_property_readonly("carrier_types", &CairnAPI::get_EnergyCarrierTypes)
        .def_property_readonly("solvers", &CairnAPI::get_Solvers)
        .def("close_study", &CairnAPI::close_Study, "close the current study");

    py::class_<CairnAPI::OptimProblemAPI>(m, "OptimProblem")
        .def("export_parameters", &CairnAPI::OptimProblemAPI::export_Parameters, "exports the study parameters to a csv file", py::arg("filename") = "")
        .def("export_plan", &CairnAPI::OptimProblemAPI::export_PLAN, "exports PLAN results to a csv file", py::arg("filename") = "", py::arg("solNb") = 0)
        .def("add_link", py::overload_cast<CairnAPI::MilpPortAPI&, CairnAPI::BusAPI&>(&CairnAPI::OptimProblemAPI::add),"adds a link between two given ports")
        .def("remove_link", py::overload_cast<CairnAPI::MilpPortAPI&, CairnAPI::BusAPI&>(&CairnAPI::OptimProblemAPI::remove), "removes a given link")
        .def("save_study", &CairnAPI::OptimProblemAPI::save_Study, py::arg("filename"), py::arg("mode") = "", "saves a study with a given name (it can also take a mode to position the components e.g. gradient)")
        .def("add_timeseries", py::overload_cast<const std::string&>(&CairnAPI::OptimProblemAPI::add_TimeSeries), "adds a given timeseries file")
        .def("run", &CairnAPI::OptimProblemAPI::run, py::arg("resultsPath") = "", "runs the optim problem")
        .def("create_bus", &CairnAPI::OptimProblemAPI::create_Bus, "creates and returns a new bus with a given name, model and energy carrier, e.g., create_bus('H2_Bus', 'NodeLaw', vH2)")
        .def("get_bus", &CairnAPI::OptimProblemAPI::get_Bus, "returns a given bus")
        .def("remove_bus", py::overload_cast<CairnAPI::BusAPI&>(&CairnAPI::OptimProblemAPI::remove_Bus), "removes a given bus")
        .def("create_energy_carrier", &CairnAPI::OptimProblemAPI::create_EnergyCarrier, "creates and returns a new energy carrier with a given name and type, e.g., create_energy_carrier('H2', 'FluidH2')")
        .def("get_energy_carrier", &CairnAPI::OptimProblemAPI::get_EnergyCarrier, "returns a given energy carrier")
        .def("remove_energy_carrier", py::overload_cast<CairnAPI::EnergyVectorAPI&>(&CairnAPI::OptimProblemAPI::remove_EnergyCarrier), "removes a given energy carrier from the optim problem")
        .def("create_component", &CairnAPI::OptimProblemAPI::create_Component, "creates and returns a new component with a given name and model, e.g., create_component('Elec_Grid', 'GridFree')")
        .def("get_component", &CairnAPI::OptimProblemAPI::get_Component, "returns a given component")
        .def("remove_component", py::overload_cast<CairnAPI::MilpComponentAPI&>(&CairnAPI::OptimProblemAPI::remove_Component), "removes a geiven component from the optim problem")
        .def("get_components", &CairnAPI::OptimProblemAPI::get_Components, "returns all the components of the optim problem specifying the category if needed", py::arg("category") = "")
        .def("get_indicator_value", &CairnAPI::OptimProblemAPI::get_TecEco_IndicatorValue, py::arg("name"), py::arg("range") = "PLAN", "returns the result of a given TecEco indicator for a given range, by default PLAN value")
        .def("get_indicators_values", &CairnAPI::OptimProblemAPI::get_TecEco_IndicatorValues, py::arg("range") = "PLAN", "returns the TecEco indicators for a given range, by default PLAN value")
        .def("get_all_indicators_values", &CairnAPI::OptimProblemAPI::get_All_IndicatorValues, py::arg("range") = "PLAN", "returns the indicators of all components for a given range, by default PLAN value")
        .def("rename_tech_eco_analysis", &CairnAPI::OptimProblemAPI::rename_TecEcoAnalysis, "rename TecEcoAnalysis")
        .def("rename_simulation_control", &CairnAPI::OptimProblemAPI::rename_SimulationControl, "rename SimulationControl")
        .def("rename_solver", &CairnAPI::OptimProblemAPI::rename_Solver, "rename Solver")
        .def("add_label", &CairnAPI::OptimProblemAPI::add_Label, "add a label to the problem")
        .def("remove_label", &CairnAPI::OptimProblemAPI::remove_Label, "remove a label from the problem")
        .def_property("tech_eco_analysis", &CairnAPI::OptimProblemAPI::get_TecEcoAnalysisSettings, &CairnAPI::OptimProblemAPI::set_TecEcoAnalysisSettings, "to change a value: problem.tech_eco_analysis = {`NbYear`:10}")
        .def_property("simulation_control", &CairnAPI::OptimProblemAPI::get_SimulationControlSettings, &CairnAPI::OptimProblemAPI::set_SimulationControlSettings)
        .def_property("solver", &CairnAPI::OptimProblemAPI::get_MIPSolverSettings, &CairnAPI::OptimProblemAPI::set_MIPSolverSettings)
        .def_property("labels", &CairnAPI::OptimProblemAPI::get_Labels, &CairnAPI::OptimProblemAPI::set_Labels, "get/set the labels of the problem")
        .def_property_readonly("tech_eco_show_config", &CairnAPI::OptimProblemAPI::get_TecEcoSettingShowConfig, "returns ShowConfig of a given parameter. The parameter is displayed in the GUI when this ShowConfig is selected in the DataFilter")
        .def_property_readonly("tech_eco_show_configs", &CairnAPI::OptimProblemAPI::get_TecEcoShowConfigList, "returns a list of all ShowConfigs which is used in the GUI DataFilter")
        .def_property_readonly("simulation_control_show_config", &CairnAPI::OptimProblemAPI::get_ControlSettingShowConfig, "returns ShowConfig of a given parameter. The parameter is displayed in the GUI when this ShowConfig is selected in the DataFilter")
        .def_property_readonly("simulation_control_show_configs", &CairnAPI::OptimProblemAPI::get_ControlShowConfigList, "returns a list of all ShowConfigs which is used in the GUI DataFilter")
        .def_property_readonly("solver_show_config", &CairnAPI::OptimProblemAPI::get_SolverSettingShowConfig, "returns ShowConfig of a given parameter. The parameter is displayed in the GUI when this ShowConfig is selected in the DataFilter")
        .def_property_readonly("solver_show_configs", &CairnAPI::OptimProblemAPI::get_SolverShowConfigList, "returns a list of all ShowConfigs which is used in the GUI DataFilter")
        .def_property_readonly("components", &CairnAPI::OptimProblemAPI::get_Components)
        .def_property_readonly("energy_carriers", &CairnAPI::OptimProblemAPI::get_EnergyCarriers, "returns all the energy carriers of the optim problem")
        .def_property_readonly("buses", &CairnAPI::OptimProblemAPI::get_Buses)
        .def_property_readonly("links", &CairnAPI::OptimProblemAPI::get_Links)
        .def_property_readonly("indicators", &CairnAPI::OptimProblemAPI::get_TecEco_IndicatorNames)
        .def_property_readonly("indicators_shortnames", &CairnAPI::OptimProblemAPI::get_TecEco_IndicatorShortNames)
        .def_property_readonly("indicators_units", &CairnAPI::OptimProblemAPI::get_TecEco_IndicatorUnits)
        .def_property_readonly("optimized_components", &CairnAPI::OptimProblemAPI::get_optimized_components)
        .doc() = "OptimProblem class.";

    py::class_<CairnAPI::EnergyVectorAPI>(m, "EnergyVector")
        .def(py::init <const CairnAPI::OptimProblemAPI&, const std::string&, const std::string&>())
        .def("rename", &CairnAPI::EnergyVectorAPI::rename, "rename EnergyVector component")
        .def("get_setting_value", &CairnAPI::EnergyVectorAPI::get_SettingValue, "returns the value of a given parameter")
        .def("set_setting_value", &CairnAPI::EnergyVectorAPI::set_SettingValue, "sets the value of a given parameter")
        .def("get_settings", &CairnAPI::EnergyVectorAPI::get_SettingsListByType, "returns a list of all (by default), used, optional or mandatory parameter names", py::arg("setLimited") = CairnAPI::ESettingsLimited::all)
        .def_property("setting_values", &CairnAPI::EnergyVectorAPI::get_SettingValues, &CairnAPI::EnergyVectorAPI::set_SettingValues, "get/set all parameters as a dictionary")
        .def_property_readonly("show_config", &CairnAPI::EnergyVectorAPI::get_SettingShowConfig, "returns ShowConfig of a given parameter. The parameter is displayed in the GUI when this ShowConfig is selected in the DataFilter")
        .def_property_readonly("show_configs", &CairnAPI::EnergyVectorAPI::get_ShowConfigList, "returns a list of all ShowConfigs which is used in the GUI DataFilter")
        .def_property_readonly("settings", &CairnAPI::EnergyVectorAPI::get_SettingsList, "returns a list of all parameter names")
        .def_property_readonly("name", &CairnAPI::EnergyVectorAPI::get_Name)
        .def_property_readonly("type", &CairnAPI::EnergyVectorAPI::get_Type)
        .doc() = "EnergyCarrier class. The constructor takes several arguments: \
        arg1: OptimProblemAPI aProblem, the study where the EnergyVector will be added  \
        arg2: string aName to give the name of the energy vector \
        arg3: string aType to give the type of the energy vector (list available thanks to property carrier_types of CairnAPI)";

    py::class_<CairnAPI::MilpPortAPI>(m, "Port")
        .def(py::init<CairnAPI::MilpComponentAPI&, const std::string&, const CairnAPI::EnergyVectorAPI&, const std::string&, const std::string&>(), 
            py::arg("component"), py::arg("name"), py::arg("carrier"), py::arg("direction") = "DATAEXCHANGE", py::arg("variable") = "")
        .def("rename", &CairnAPI::MilpPortAPI::rename, "rename port")
        .def("get_setting_value", &CairnAPI::MilpPortAPI::get_SettingValue, "returns the value of a given parameter")
        .def("set_setting_value", &CairnAPI::MilpPortAPI::set_SettingValue, "sets the value of a given parameter")
        .def("get_settings", &CairnAPI::MilpPortAPI::get_SettingsListByType, "returns a list of all (by default), used, optional or mandatory parameter names", py::arg("setLimited") = CairnAPI::ESettingsLimited::all)
        .def("set_carrier", &CairnAPI::MilpPortAPI::set_EnergyCarrier, "sets the energy carrier of the port (takes componenet not name)")
        .def_property_readonly("carrier_name", &CairnAPI::MilpPortAPI::get_CarrierName, "return the name of energy carrier")
        .def_property_readonly("name", &CairnAPI::MilpPortAPI::get_Name)
        .def_property_readonly("settings", &CairnAPI::MilpPortAPI::get_SettingsList, "returns a list of all parameter names")
        .def_property("setting_values", &CairnAPI::MilpPortAPI::get_SettingValues, &CairnAPI::MilpPortAPI::set_SettingValues, "get/set all parameters as a dictionary")
        .doc() = "Port class. The constructor takes several arguments: \
        arg1: MilpComponentAPI aComponent which will own the port \
        arg2: string aName to give the name of the port \
        arg3: EnergyVectorAPI aEnergyVector to give the energy vector of the port \
        arg3 (optional): string aDirection (Input, Output or DataExchange) to set the direction of the port\
        arg3 (optional): string aVariable to set the variable of the port";

    py::class_<CairnAPI::BusAPI>(m, "Bus")
        .def(py::init<const CairnAPI::OptimProblemAPI&, const std::string&, const std::string&, const CairnAPI::EnergyVectorAPI&>())
        .def("rename", &CairnAPI::BusAPI::rename, "rename Bus componenet")
        .def("get_setting_value", &CairnAPI::BusAPI::get_SettingValue, "returns the value of a given parameter")
        .def("set_setting_value", &CairnAPI::BusAPI::set_SettingValue, "sets the value of a given parameter")
        .def("get_settings", &CairnAPI::BusAPI::get_SettingsListByType, "returns a list of all (by default), used, optional or mandatory parameter names", py::arg("setLimited") = CairnAPI::ESettingsLimited::all)
        .def("get_indicator_value", &CairnAPI::BusAPI::get_IndicatorValue, py::arg("name"), py::arg("range") = "PLAN", "returns the result of a given indicator for a given range, by default PLAN value")
        .def("get_indicators_values", &CairnAPI::BusAPI::get_IndicatorValues, py::arg("range") = "PLAN", "returns the  indicators for a given range, by default PLAN value")
        .def("get_label_value", &CairnAPI::BusAPI::get_LabelValue, "return the value of a given label")
        .def("set_label_value", &CairnAPI::BusAPI::set_LabelValue, "set the value of a given label")
        .def_property_readonly("name", &CairnAPI::BusAPI::get_Name)
        .def_property_readonly("type", &CairnAPI::BusAPI::get_Type)
        .def_property_readonly("model_class", &CairnAPI::BusAPI::get_ModelClass)
        .def_property_readonly("settings", &CairnAPI::BusAPI::get_SettingsList, "returns a list of all parameter names")
        .def_property("setting_values", &CairnAPI::BusAPI::get_SettingValues, &CairnAPI::BusAPI::set_SettingValues, "get/set all parameters as a dictionary")
        .def_property("label_values", &CairnAPI::BusAPI::get_LabelValues, &CairnAPI::BusAPI::set_LabelValues, "get/set all label values as a dictionary")
        .def_property_readonly("show_config", &CairnAPI::BusAPI::get_SettingShowConfig, "returns ShowConfig of a given parameter. The parameter is displayed in the GUI when this ShowConfig is selected in the DataFilter")
        .def_property_readonly("show_configs", &CairnAPI::BusAPI::get_ShowConfigList, "returns a list of all ShowConfigs which is used in the GUI DataFilter")
        .def_property_readonly("indicators", &CairnAPI::BusAPI::get_IndicatorNames)
        .def_property_readonly("indicators_shortnames", &CairnAPI::BusAPI::get_IndicatorShortNames)
        .def_property_readonly("indicators_units", &CairnAPI::BusAPI::get_IndicatorUnits)
        .doc() = "Bus class. The constructor takes several arguments: \
        arg1: OptimProblemAPI aProblem, the study where the Bus will be added  \
        arg2: string aName to give the name of the bus \
        arg3: string aModelName to give the model. Available Bus models are NodeLaw, NodeEquality and ManualObjective \
        arg4: string aEnergyVector to give the energy vector of the bus";

    py::class_<CairnAPI::MilpComponentAPI>(m, "Component")
        .def(py::init<const CairnAPI::OptimProblemAPI&, const std::string&, const std::string&>())
        .def("rename", &CairnAPI::MilpComponentAPI::rename, "rename componenet")
        .def("get_port", &CairnAPI::MilpComponentAPI::get_Port, "returns a given port from the component")
        .def("add_port", &CairnAPI::MilpComponentAPI::add_Port, py::arg("name"), py::arg("carrier"), py::arg("direction") = "DATAEXCHANGE", py::arg("variable") = "", py::arg("reinitializeCompo") = true, "creates a new port for the component")
        .def("remove_port", &CairnAPI::MilpComponentAPI::remove_Port, "removes a given port from the component")
        .def("get_setting_value", &CairnAPI::MilpComponentAPI::get_SettingValue, "returns the value of a given parameter")
        .def("set_setting_value", &CairnAPI::MilpComponentAPI::set_SettingValue, py::arg("name"), py::arg("value"), py::arg("verify") = true, "sets the value of a given parameter")
        .def("get_settings", &CairnAPI::MilpComponentAPI::get_SettingsListByType, "returns a list of all (by default), used, optional or mandatory parameter names", py::arg("setLimited") = CairnAPI::ESettingsLimited::all)
        .def("get_timeseries_vector", &CairnAPI::MilpComponentAPI::get_TimeSeriesVector, "returns the vector value of a given timeseries parameter")
        .def("set_timeseries_vector", &CairnAPI::MilpComponentAPI::set_TimeSeriesVector, "sets the vector value of a given timeseries parameter")
        .def("get_indicator_value", &CairnAPI::MilpComponentAPI::get_IndicatorValue, py::arg("name"), py::arg("range") = "PLAN", "returns the result of a given indicator for a given range, by default PLAN value")
        .def("get_indicators_values", &CairnAPI::MilpComponentAPI::get_IndicatorValues, py::arg("range") = "PLAN", "returns the  indicators for a given range, by default PLAN value")
        .def("get_var_value", &CairnAPI::MilpComponentAPI::get_varValue, "returns the result of a given IO for the optimal solution")
        .def("get_var_values", &CairnAPI::MilpComponentAPI::get_varValues, "returns the result of all IOs for the optimal solution")
        .def("get_label_value", &CairnAPI::MilpComponentAPI::get_LabelValue, "return the value of a given label")
        .def("set_label_value", &CairnAPI::MilpComponentAPI::set_LabelValue, "set the value of a given label")
        .def_property_readonly("name", &CairnAPI::MilpComponentAPI::get_Name)
        .def_property_readonly("type", &CairnAPI::MilpComponentAPI::get_Type)
        .def_property_readonly("dim_param", &CairnAPI::MilpComponentAPI::get_dimParam,"return the parameter used for dimensionning")
        .def_property_readonly("model_class", &CairnAPI::MilpComponentAPI::get_ModelClass)
        .def_property_readonly("direction", &CairnAPI::MilpComponentAPI::get_Direction, "return the direction based on the default port. Valid only for components of types Grid and SourceLoad. ")
        .def_property_readonly("ports", &CairnAPI::MilpComponentAPI::get_Ports, "returns the list of all port names")
        .def_property_readonly("default_ports", &CairnAPI::MilpComponentAPI::get_DefaultPorts, "returns the list of default port names")
        .def_property_readonly("settings", &CairnAPI::MilpComponentAPI::get_SettingsList, "returns a list of all parameter names")
        .def_property("setting_values", &CairnAPI::MilpComponentAPI::get_SettingValues, &CairnAPI::MilpComponentAPI::set_SettingValues,"get/set all parameters as a dictionary")
        .def_property("label_values", &CairnAPI::MilpComponentAPI::get_LabelValues, &CairnAPI::MilpComponentAPI::set_LabelValues, "get/set all label values as a dictionary")
        .def_property_readonly("show_config", &CairnAPI::MilpComponentAPI::get_SettingShowConfig, "returns ShowConfig of a given parameter. The parameter is displayed in the GUI when this ShowConfig is selected in the DataFilter")
        .def_property_readonly("show_configs", &CairnAPI::MilpComponentAPI::get_ShowConfigList, "returns a list of all ShowConfigs which is used in the GUI DataFilter")
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

    py::class_<CairnAPI::SolutionAPI>(m, "Solution")
        .def(py::init())
        .def("get_ts_results", &CairnAPI::SolutionAPI::get_TimeSeries, "returns the output ts results")
        .def_property_readonly("status", &CairnAPI::SolutionAPI::get_Status, "returns the status of the run")
        .def_property_readonly("nb_solutions", &CairnAPI::SolutionAPI::get_NbSolutions, "returns the number of solutions found")
        .doc() = "Solution class.";
}