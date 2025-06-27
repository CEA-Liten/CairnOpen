#include "CairnAPI.h"
#include <Python.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace py = PYBIND11_NAMESPACE;

   
PYBIND11_MODULE(cairn, m) {

    m.doc() = R"X(Cairn is developped by CEA
)X";

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
        .def("add_link", py::overload_cast<CairnAPI::MilpPortAPI&, CairnAPI::BusAPI&>(&CairnAPI::OptimProblemAPI::add),"adds a link between two given ports")
        .def("remove_link", py::overload_cast<CairnAPI::MilpPortAPI&, CairnAPI::BusAPI&>(&CairnAPI::OptimProblemAPI::remove), "removes a given link")
        .def("save_study", &CairnAPI::OptimProblemAPI::save_Study, py::arg("filename"), py::arg("mode") = "", "saves a study with a given name (it can also take a mode to position the components e.g. gradient)")
        .def("add_timeseries", py::overload_cast<const std::string&>(&CairnAPI::OptimProblemAPI::add_TimeSeries), "adds a given timeseries file")
        .def("run", &CairnAPI::OptimProblemAPI::run, py::arg("resultsPath") = "", "runs the optim problem")
        .def("get_bus", &CairnAPI::OptimProblemAPI::get_Bus, "returns a given bus")
        .def("add_bus", py::overload_cast<CairnAPI::BusAPI&>(&CairnAPI::OptimProblemAPI::add), "adds a given bus")
        .def("remove_bus", py::overload_cast<CairnAPI::BusAPI&>(&CairnAPI::OptimProblemAPI::remove), "removes a given bus")
        .def("get_energy_carrier", &CairnAPI::OptimProblemAPI::get_EnergyCarrier, "returns a given energy carrier")
        .def("add_energy_carrier", py::overload_cast<CairnAPI::EnergyVectorAPI&>(&CairnAPI::OptimProblemAPI::add), "adds a given energy carrier to the optim problem")
        .def("remove_energy_carrier", py::overload_cast<CairnAPI::EnergyVectorAPI&>(&CairnAPI::OptimProblemAPI::remove), "removes a given energy carrier from the optim problem")
        .def("get_components", &CairnAPI::OptimProblemAPI::get_Components, "returns all the components of the optim problem specifying the category if needed", py::arg("category") = "")
        .def("get_component", &CairnAPI::OptimProblemAPI::get_Component, "returns a given component")
        .def("add_component", py::overload_cast<CairnAPI::MilpComponentAPI&>(&CairnAPI::OptimProblemAPI::add), "adds a given component to the optim problem")
        .def("remove_component", py::overload_cast<CairnAPI::MilpComponentAPI&>(&CairnAPI::OptimProblemAPI::remove), "removes a geiven component from the optim problem")
        .def_property("simulation_control", &CairnAPI::OptimProblemAPI::get_SimulationControlSettings, &CairnAPI::OptimProblemAPI::set_SimulationControlSettings)
        .def_property("tech_eco_analysis", &CairnAPI::OptimProblemAPI::get_TechEcoAnalysisSettings, &CairnAPI::OptimProblemAPI::set_TechEcoAnalysisSettings)
        .def_property("solver", &CairnAPI::OptimProblemAPI::get_MIPSolverSettings, &CairnAPI::OptimProblemAPI::set_MIPSolverSettings)
        .def_property_readonly("components", &CairnAPI::OptimProblemAPI::get_Components)
        .def_property_readonly("energy_carriers", &CairnAPI::OptimProblemAPI::get_EnergyCarriers, "returns all the energy carriers of the optim problem")
        .def_property_readonly("buses", &CairnAPI::OptimProblemAPI::get_Buses)
        .def_property_readonly("links", &CairnAPI::OptimProblemAPI::get_Links)
        .def_property_readonly("optimized_components", &CairnAPI::OptimProblemAPI::get_optimized_components)
        .doc() = "OptimProblem class.";

    py::class_<CairnAPI::EnergyVectorAPI>(m, "EnergyVector")
        .def(py::init<const std::string &, const std::string&>())
        .def("settings", &CairnAPI::EnergyVectorAPI::get_SettingsList, "returns the list of settings name", py::arg("setLimited") = CairnAPI::ESettingsLimited::all)                
        .def("get_setting_value", &CairnAPI::EnergyVectorAPI::get_SettingValue, "returns the value of a given parameter")
        .def("set_setting_value", &CairnAPI::EnergyVectorAPI::set_SettingValue, "sets the value of a given parameter")
        .def_property("setting_values", &CairnAPI::EnergyVectorAPI::get_SettingValues, &CairnAPI::EnergyVectorAPI::set_SettingValues)
        .def_property_readonly("name", &CairnAPI::EnergyVectorAPI::get_Name)
        .def_property_readonly("type", &CairnAPI::EnergyVectorAPI::get_Type)
        .doc() = "EnergyCarrier class. The constructor takes several arguments: \
        arg1: string aName to give the name of the energy vector \
        arg2: string aType to give the type of the energy vector (list available thanks to property carrier_types of CairnAPI)";

    py::class_<CairnAPI::MilpPortAPI>(m, "Port")
        .def(py::init<const std::string&, const CairnAPI::EnergyVectorAPI&>())
        .def("get_setting_value", &CairnAPI::MilpPortAPI::get_SettingValue, "returns the value of a given parameter")
        .def("set_setting_value", &CairnAPI::MilpPortAPI::set_SettingValue, "sets the value of a given parameter")
        .def_property("setting_values", &CairnAPI::MilpPortAPI::get_SettingValues, &CairnAPI::MilpPortAPI::set_SettingValues)
        .def_property_readonly("name", &CairnAPI::MilpPortAPI::get_Name)
        .def_property_readonly("type", &CairnAPI::MilpPortAPI::get_Type)
        .def_property_readonly("settings", &CairnAPI::MilpPortAPI::get_SettingsList)
        .doc() = "Port class. The constructor takes several arguments: \
        arg1: string aName to give the name of the port \
        arg2: EnergyVectorAPI aEnergyVector to give the energy vector of the port";

    py::class_<CairnAPI::BusAPI>(m, "Bus")
        .def(py::init<const std::string&, const std::string&, const CairnAPI::EnergyVectorAPI&>())
        .def(py::init<const std::string&, const std::string&, const std::string&, const CairnAPI::EnergyVectorAPI&>())
        .def("get_setting_value", &CairnAPI::BusAPI::get_SettingValue, "returns the value of a given parameter")
        .def("set_setting_value", &CairnAPI::BusAPI::set_SettingValue, "sets the value of a given parameter")
        .def_property("model_class", &CairnAPI::BusAPI::get_ModelClass, &CairnAPI::BusAPI::set_ModelClass)
        .def_property("setting_values", &CairnAPI::BusAPI::get_SettingValues, &CairnAPI::BusAPI::set_SettingValues)
        .def_property_readonly("name", &CairnAPI::BusAPI::get_Name)
        .def_property_readonly("type", &CairnAPI::BusAPI::get_Type)
        .def_property_readonly("settings", &CairnAPI::BusAPI::get_SettingsList)
        .doc() = "Bus class. The constructor takes several arguments: \
        arg1: string aName to give the name of the bus \
        arg3: string aModelName to give the model. Available Bus models are NodeLaw, NodeEquality and ManualObjective \
        arg4: string aEnergyVector to give the energy vector of the bus";

    py::class_<CairnAPI::MilpComponentAPI>(m, "Component")
        .def(py::init<const std::string&, const std::string&>())
        .def(py::init<const std::string&, const std::string&, const std::string&>())
        .def("get_port", &CairnAPI::MilpComponentAPI::get_Port, "returns a given port from the component")
        .def("add_port", &CairnAPI::MilpComponentAPI::add, "adds a given port to the component")
        .def("remove_port", &CairnAPI::MilpComponentAPI::remove, "removes a given port from the component")
        .def("get_setting_value", &CairnAPI::MilpComponentAPI::get_SettingValue, "returns the value of a given parameter")
        .def("set_setting_value", &CairnAPI::MilpComponentAPI::set_SettingValue, "sets the value of a given parameter")
        .def("get_timeseries_vector", &CairnAPI::MilpComponentAPI::get_TimeSeriesVector, "returns the vector value of a given timeseries parameter")
        .def("set_timeseries_vector", &CairnAPI::MilpComponentAPI::set_TimeSeriesVector, "sets the vector value of a given timeseries parameter")
        .def("get_indicator_value", &CairnAPI::MilpComponentAPI::get_IndicatorValue, py::arg("name"), py::arg("range") = "PLAN", "returns the result of a given indicator for a given range, by default PLAN value")
        .def("get_indicators_values", &CairnAPI::MilpComponentAPI::get_IndicatorValues, py::arg("range") = "PLAN", "returns the  indicators for a given range, by default PLAN value")
        .def("get_var_value", &CairnAPI::MilpComponentAPI::get_varValue, "returns the result of a given IO for the optimal solution")
        .def("get_var_values", &CairnAPI::MilpComponentAPI::get_varValues, "returns the result of all IOs for the optimal solution")
        .def_property("setting_values", &CairnAPI::MilpComponentAPI::get_SettingValues, &CairnAPI::MilpComponentAPI::set_SettingValues)
        .def_property("model_class", &CairnAPI::MilpComponentAPI::get_ModelClass, &CairnAPI::MilpComponentAPI::set_ModelClass)
        .def_property_readonly("name", &CairnAPI::MilpComponentAPI::get_Name)
        .def_property_readonly("type", &CairnAPI::MilpComponentAPI::get_Type)
        .def_property_readonly("ports", &CairnAPI::MilpComponentAPI::get_Ports, "returns the list of the ports name")
        .def_property_readonly("settings", &CairnAPI::MilpComponentAPI::get_SettingsList)
        .def_property_readonly("variables", &CairnAPI::MilpComponentAPI::get_VarList)
        .def_property_readonly("indicators", &CairnAPI::MilpComponentAPI::get_IndicatorNames)
        .def_property_readonly("indicators_shortnames", &CairnAPI::MilpComponentAPI::get_IndicatorShortNames)
        .def_property_readonly("indicators_units", &CairnAPI::MilpComponentAPI::get_IndicatorUnits)
        .def_property_readonly("is_optimized", &CairnAPI::MilpComponentAPI::isOptimized)
        .doc()="Component class. The constructor takes several arguments: \
        arg1: string aName to give the name of the component \
        arg3: string aModelName to give the model (list available thanks to property all_models of CairnAPI). \
              Note that, to create componenets NodeLaw, NodeEquality and ManualObjective use BusAPI\
        arg4 (optional): t_dict aSettingValues to give input parameters of the component";

    py::class_<CairnAPI::SolutionAPI>(m, "Solution")
        .def(py::init())
        .def("get_optimal_solution", &CairnAPI::SolutionAPI::getOptimalSolution, "returns the optimal solution")
        .def("get_ts_results", &CairnAPI::SolutionAPI::get_TimeSeries, "returns the output ts results")
        .def_property_readonly("status", &CairnAPI::SolutionAPI::get_Status, "returns the status of the run")
        .def_property_readonly("nb_solutions", &CairnAPI::SolutionAPI::get_NbSolutions, "returns the number of solutions found")
        .doc() = "Solution class.";
}