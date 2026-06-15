# -*- coding: utf-8 -*-
"""
Created on Sun Dec 22 15:33:30 2024

@author: sc258201
"""

import pytest
from cairn import *
import os
import shutil
from os import path
import pandas as pd
import filecmp
import glob
import sys

scripts_home = os.path.join(path.dirname(path.realpath(__file__)), "../scripts")
sys.path.append(scripts_home)
import CairnNRT as cNRT

@pytest.fixture(autouse=True, scope="session")
def clean():
    app_home = path.dirname(path.realpath(__file__))
    result_path =  path.join(app_home, 'results')
    if path.exists(result_path):
        print("***************************************************")
        print(os.path.join(result_path, '*.csv'))
        for csvpath in glob.iglob(os.path.join(result_path, '*.csv')):
            try:
                os.remove(csvpath)
            except FileNotFoundError as e:
                print(f"{e} already deleted!")
        for jsonpath in glob.iglob(os.path.join(result_path, '*.json')):
            try:
                os.remove(jsonpath)
            except FileNotFoundError as e:
                print(f"{e} already deleted!")
    yield

@pytest.fixture()
def problem():
    app_home = path.dirname(path.realpath(__file__))
    simu_full =  path.join(app_home, './data/cairn_training.json')    
    cairn_instance = CairnAPI(False)
    problem = cairn_instance.read_study(simu_full)
    yield problem

@pytest.fixture()
def instance():
    app_home = path.dirname(path.realpath(__file__))
    simu_full =  path.join(app_home, './data/cairn_training.json')
    cairn_instance = CairnAPI(False)
    problem = cairn_instance.read_study(simu_full)
    yield cairn_instance

def read_compos(problem):
    print(problem.get_components())
    app_home = path.dirname(path.realpath(__file__))
    resultPath = path.join(app_home, 'results')
    for compo in problem.get_components():
        my_compo = problem.get_component(compo)
        my_compo_setting_values = my_compo.setting_values
        param_list = []
        values_list = []
        for name in my_compo_setting_values:
            param_list.append(name)
            values_list.append(my_compo_setting_values[name])
        # dataframe
        data = {'paramValue': values_list, 'paramName': param_list}
        df = pd.DataFrame(data)
        df.set_index('paramName', inplace=True)
        df.to_csv(path.join(resultPath, compo + ".csv"), sep=';', decimal='.')

        # Comparer chaque fichier à une référence ?


def write_compo(problem):
    app_home = path.dirname(path.realpath(__file__))
    dataPath = path.join(app_home, 'data')
    ely_pem_csv = path.join(dataPath, "ELY_PEM_to_write.csv")
    ely_pem_df = pd.read_csv(ely_pem_csv, sep=';', decimal='.')
    ely_pem_df.set_index('paramName', inplace=True)
    ely_pem = problem.get_component("ELY_PEM")
    assert ely_pem.get_setting_value("AddAuxConso") == 0
    ely_pem.set_setting_value("AddAuxConso", ely_pem_df.at["AddAuxConso", "paramValue"])
    assert ely_pem.get_setting_value("AddAuxConso") == 1
    assert ely_pem.get_setting_value("AuxConso") == 0
    ely_pem.set_setting_value("AuxConso", ely_pem_df.at["AuxConso", "paramValue"])
    assert ely_pem.get_setting_value("AuxConso") == 0.01
    # timeseries
    ely_pem.set_setting_value("ComponentAvailability", "Availability")
    assert ely_pem.get_setting_value("ComponentAvailability") == "Availability"


def read_energy_carriers(problem):
    print(problem.energy_carriers)
    app_home = path.dirname(path.realpath(__file__))
    resultPath = path.join(app_home, 'results')
    for carrier in problem.energy_carriers:
        my_carrier = problem.get_energy_carrier(carrier)
        my_carrier_setting_values = my_carrier.setting_values
        param_list = []
        values_list = []
        for name in my_carrier_setting_values:
            param_list.append(name)
            values_list.append(my_carrier_setting_values[name])
        # dataframe
        data = {'paramValue': values_list, 'paramName': param_list}
        df = pd.DataFrame(data)
        df.set_index('paramName', inplace=True)
        df.to_csv(path.join(resultPath, carrier + ".csv"), sep=';', decimal='.')

        # Comparer chaque fichier à une référence ?


def write_energy_carrier(problem):
    app_home = path.dirname(path.realpath(__file__))
    dataPath = path.join(app_home, 'data')
    h2_csv = path.join(dataPath, "H2_to_write.csv")
    h2_df = pd.read_csv(h2_csv, sep=';', decimal='.')
    h2_df.set_index('paramName', inplace=True)
    h2 = problem.get_energy_carrier("H2")
    assert h2.get_setting_value("LHV") == 0.03332
    h2.set_setting_value("LHV", h2_df.at["LHV", "paramValue"])
    assert h2.get_setting_value("LHV") == 0.03333333


def load_timeseries(problem, file):
    problem.add_timeseries(file)

def load_df_timeseries(problem, file):
     df = pd.read_csv(file, header=None, sep=";")
     sizeValues = df.index.stop          
     for col in df.columns:
         values = []
         for row in range(4,sizeValues):
             values.append(df.values[row][col])
         ts = {"Name": df.values[0][col], "Unit": df.values[2][col], "Description": df.values[1][col], "Values": values}
         problem.add_onetimeseries(ts)
     

def save(problem, folder, new_name):
    if not os.path.exists(folder):
        os.makedirs(folder)
    problem.save_study(new_name)
    # Verify that the file exists
    assert os.path.isfile(new_name)


def initialize(problem):
    problem.initialize()


def run(problem, folder):
    return problem.run(folder)


def get_plan_results(problem):
    ely_pem = problem.get_component("ELY_PEM")
    indicators = ely_pem.indicators
    assert round(ely_pem.get_indicator_value("Installed Optimal Size", "PLAN"), 5) == 2.48392
    ind = ely_pem.get_indicators_values("PLAN")
    print(ind)
    df = pd.DataFrame([ind]).transpose
    return df


def get_ts_results(problem):
    ely_pem = problem.get_component("ELY_PEM")
    outputs = ely_pem.variables
    print(outputs)
    assert len(ely_pem.get_var_value("UsedPower")) > 0
    assert ely_pem.get_var_value("UsedPower")[0] > 0

    
@pytest.mark.Cairn
@pytest.mark.PythonAPI
@pytest.mark.xdist_group("PythonAPI")
def test_set_get_Param(problem):
    simulation_control = problem.get_simulation_control()
    P1 = simulation_control.get_setting("FutureSize")
    assert P1.value == 48
    assert P1.isMandatory == True
    assert P1.defaultValue == 8760
    P1.value = 168
    assert simulation_control.get_setting_value('FutureSize') == 168
    
    ely_pem = problem.get_component("ELY_PEM")
    P2 = ely_pem.get_setting("Capex")
    assert P2.value == 480000.0
    assert P2.isMandatory == True
    assert P2.unit == 'EUR/MW'
    assert P2.show_config == 'EcoInvestModel'

   


@pytest.mark.Cairn
@pytest.mark.PythonAPI
@pytest.mark.xdist_group("PythonAPI")
def test_set_get_SimulationControl(problem):
    simulation_control = problem.get_simulation_control()
    assert simulation_control.get_setting_value('FutureSize') == 48
    simulation_control.set_setting_value('FutureSize', 168)
    assert simulation_control.get_setting_value('FutureSize') == 168

@pytest.mark.Cairn
@pytest.mark.PythonAPI
@pytest.mark.xdist_group("PythonAPI")
def test_set_get_TecEcoAnalysis(problem):
    tech_eco_analysis = problem.get_tech_eco_analysis()
    # Double value
    assert tech_eco_analysis.get_setting_value('DiscountRate') == 0.07
    tech_eco_analysis.set_setting_value('DiscountRate', 0.06)
    assert tech_eco_analysis.get_setting_value('DiscountRate') == 0.06
    print(tech_eco_analysis.get_setting_value('DiscountRate'))
    # String list value
    assert len(tech_eco_analysis.get_setting_value('ConsideredEnvironmentalImpacts')) == 2
    assert 'Climate change#Global Warming Potential 100' in tech_eco_analysis.get_setting_value(
        'ConsideredEnvironmentalImpacts')
    assert 'Acidification#Accumulated Exceedance' in tech_eco_analysis.get_setting_value(
        'ConsideredEnvironmentalImpacts')
    try:
        tech_eco_analysis.set_setting_value('ConsideredEnvironmentalImpacts',
                                            'Climate change#Global Warming Potential 100')
    except ValueError:
        print("Not possible to set tech eco analysis - string list")
    assert len(tech_eco_analysis.get_setting_value('ConsideredEnvironmentalImpacts')) == 1
    assert 'Acidification#Accumulated Exceedance' not in tech_eco_analysis.get_setting_value(
        'ConsideredEnvironmentalImpacts')
    assert 'Climate change#Global Warming Potential 100' in tech_eco_analysis.get_setting_value(
        'ConsideredEnvironmentalImpacts')

@pytest.mark.Cairn
@pytest.mark.PythonAPI
@pytest.mark.xdist_group("PythonAPI")
#@pytest.mark.skip("need to manage the Highs case")
def test_set_get_Solvers(problem):
    #print("need to manage the Highs case")
    solver = problem.get_solver()
    print(solver.get_setting_value('Gap'))
    # Double value
    assert solver.get_setting_value('Gap') == 0.001
    solver.set_setting_value('Gap', 0.0005)
    assert solver.get_setting_value('Gap') == 0.0005
    # String value
    assert solver.get_setting_value('WriteLp') == "YES"
    solver.set_setting_value('WriteLp', "NO")
    assert solver.get_setting_value('WriteLp') == "NO"

@pytest.mark.Cairn
@pytest.mark.PythonAPI
@pytest.mark.xdist_group("PythonAPI")
def test_add_component(problem):
    my_pv_field = problem.create_component("PV", "SourceLoad")
    carrier = problem.get_energy_carrier("ElectricityDistrib")
    defaultPorts = my_pv_field.default_ports
    assert len(defaultPorts) == 1
    my_PV_L0 = my_pv_field.get_port(defaultPorts[0])  # "PortL0"
    my_PV_L0.set_carrier(carrier)
    my_PV_L0.setting_values = {
        "Direction": "OUTPUT",
        "Variable": "SourceLoadFlow"
    }
    my_pv_field.setting_values = {
        "Weight": 1,
        "Opex": 0,
        "Capex": 1000,
        "MaxFlow": -10,
        "EcoInvestModel": 1,
        "UseProfileLoadFlux": "PVProduction"
    }
    assert my_pv_field.get_setting_value("UseProfileLoadFlux") == "PVProduction"
    my_pv_field.set_setting_value("UseProfileLoadFlux", "WindFarmProduction")
    assert my_pv_field.get_setting_value("UseProfileLoadFlux") == "WindFarmProduction"

@pytest.mark.Cairn
@pytest.mark.PythonAPI
@pytest.mark.xdist_group("PythonAPI")
def test_modify_port(problem):
    h2_load = problem.get_component("H2_Load")
    port = h2_load.get_port("PortL0")
    assert port.get_setting_value("Coeff") == 1.0
    port.set_setting_value("Coeff", 2)
    assert port.get_setting_value("Coeff") == 2.0
    assert port.get_setting_value("Offset") == 0.0
    port.set_setting_value("Offset", 10)
    assert port.get_setting_value("Offset") == 10.0

@pytest.mark.Cairn
@pytest.mark.PythonAPI
@pytest.mark.xdist_group("PythonAPI")
def test_get_energy_carrier(problem):
    # Add a new energy carrier
    energy_carrier_name = "ElectricityDistrib2"
    energy_carrier_type = "ElectricalCarrier"
    problem.create_energy_carrier(energy_carrier_name, energy_carrier_type)

    # Get the energy vector
    energy_carrier = problem.get_energy_carrier(energy_carrier_name)

    assert energy_carrier is not None
    assert energy_carrier.name == energy_carrier_name
    assert energy_carrier.type == energy_carrier_type

    # Check the settings
    ev_settings = energy_carrier.settings
    assert isinstance(ev_settings, list)
    assert "Voltage" in ev_settings

    # Check values
    setting_value = energy_carrier.get_setting_value("Voltage")
    assert setting_value is not None
    new_value = 440
    energy_carrier.set_setting_value("Voltage", new_value)
    updated_value = energy_carrier.get_setting_value("Voltage")
    assert updated_value == new_value

@pytest.mark.Cairn
@pytest.mark.PythonAPI
@pytest.mark.xdist_group("PythonAPI")
def test_get_study(instance):
    assert instance.get_study().get_components() == ['ELY_PEM', 'Elec_Grid', 'Elec_Grid_Inject', 'H2_Load', 'H2_Tank', 'Wind_farm']

@pytest.mark.Cairn
@pytest.mark.PythonAPI
@pytest.mark.xdist_group("PythonAPI")
def test_model_class(problem):
    storage = problem.get_component("H2_Tank")
    print(storage.possible_model_classes)
    assert "StorageGen", "StorageThermal" in storage.possible_model_classes

@pytest.mark.Cairn
@pytest.mark.PythonAPI
@pytest.mark.xdist_group("PythonAPI")
def test_add_udl(problem):
    problem.add_label("country")
    problem.add_label("year")
    problem.add_label("site")
    problem.remove_label("year")
    print(problem.labels)

    assert len(problem.labels) == 2 and 'country' in problem.labels and 'site' in problem.labels

    problem.labels = ["country", "site"]

    ely_pem = problem.get_component("ELY_PEM")
    ely_pem.label_values = {"country": "France", "site": "Grenoble"}
    print(ely_pem.label_values)

    wind_farm = problem.get_component("Wind_farm")
    wind_farm.set_label_value("country", "France")
    wind_farm.set_label_value("site", "Noyarey")

@pytest.mark.Cairn
@pytest.mark.PythonAPI
@pytest.mark.xdist_group("PythonAPI")
def test_add_grid(problem):
    my_GFSP = problem.create_component("Grid_Surplus", "GridFree")
    h2_bus = problem.get_bus("H2_Bus")
    port_GFSP = my_GFSP.get_port("PortR0")
    h2_carrier = problem.get_energy_carrier("H2")
    port_GFSP.set_carrier(h2_carrier)
    print(port_GFSP.settings)
    port_GFSP.set_setting_value("Direction", "INPUT")
    problem.add_link(port_GFSP, h2_bus)
    assert port_GFSP.get_setting_value("Direction") == "INPUT"
    assert my_GFSP.direction == "injection"

@pytest.mark.Cairn
@pytest.mark.PythonAPI
@pytest.mark.xdist_group("PythonAPI")
def test_add_port(problem):
    ely_pem = problem.get_component("ELY_PEM")
    carrier = problem.get_energy_carrier("ElectricityDistrib")
    port_new = ely_pem.add_port("test_port", carrier)
    assert port_new.carrier_name == "ElectricityDistrib"
    ely_pem.remove_port(port_new)

@pytest.mark.Cairn
@pytest.mark.PythonAPI
@pytest.mark.xdist_group("PythonAPI")
def test_get_default_port(problem):
    ely_pem = problem.get_component("ELY_PEM")
    default_ports = ely_pem.default_ports
    assert len(default_ports) == 2
    assert "PortR0" in default_ports
    assert "PortL0" in default_ports
    assert ely_pem.get_port("PortR0").carrier_name == "H2"
    assert ely_pem.get_port("PortL0").carrier_name == "ElectricityDistrib"

@pytest.mark.Cairn
@pytest.mark.PythonAPI
@pytest.mark.xdist_group("PythonAPI")
def test_plan_results(problem):
    app_home = path.dirname(path.realpath(__file__))
    data_path = path.join(app_home, 'data')
    results_path = path.join(app_home, 'results', 'test_plan_results')
    load_timeseries(problem, path.join(data_path, "cairn_training_dataseries.csv"))
    simulation_control = problem.get_simulation_control()
    simulation_control.set_setting_value('ExportResults', False)
    solution = run(problem, results_path)
    problem.export_plan(path.join(results_path, "cairn_training_PLAN_API.csv"))
    simulation_control.set_setting_value('ExportResults', True)
    solution = run(problem, "test_plan_results")
    cairn_nrt = cNRT.CairnNRT(name="test_plan", app_home=app_home)
    #assert cairn_nrt.checkPlanHist("PLAN")

@pytest.mark.Cairn
@pytest.mark.PythonAPI
@pytest.mark.xdist_group("PythonAPI")
def test_sequence_1(problem):
    read_compos(problem)
    write_compo(problem)
    read_energy_carriers(problem)
    #print(problem)
    app_home = path.dirname(path.realpath(__file__))
    results_path =  path.join(app_home, 'results', 'test_sequence_1')
    save(problem, results_path, path.join(results_path,  "cairn_training_new.json"))

@pytest.mark.Cairn
@pytest.mark.PythonAPI
@pytest.mark.xdist_group("PythonAPI")
def test_sequence_run_compare(problem):
    app_home = path.dirname(path.realpath(__file__))
    dataPath =  path.join(app_home, 'data')
    load_timeseries(problem, path.join(dataPath,  "cairn_training_dataseries.csv"))
    solution = run(problem, "")
    get_plan_results(problem)
    get_ts_results(problem)

@pytest.mark.Cairn
@pytest.mark.PythonAPI
@pytest.mark.xdist_group("PythonAPI")
def test_df_timeseries(problem):
    app_home = path.dirname(path.realpath(__file__))
    dataPath =  path.join(app_home, 'data')
    load_df_timeseries(problem, path.join(dataPath,  "cairn_training_dataseries.csv"))
    solution = run(problem, "")
    get_plan_results(problem)
    get_ts_results(problem) 

@pytest.mark.Cairn
@pytest.mark.PythonAPI
@pytest.mark.xdist_group("PythonAPI")
def test_df_timeseries2(problem):
    app_home = path.dirname(path.realpath(__file__))
    dataPath =  path.join(app_home, 'data')
    add_df_timeseries(problem, pd.read_csv(path.join(dataPath,  "cairn_training_dataseries.csv"), header=None, sep=";"))
    solution = run(problem, "")
    get_plan_results(problem)
    get_ts_results(problem)        


@pytest.mark.Cairn
@pytest.mark.PythonAPI
@pytest.mark.xdist_group("PythonAPI")
def test_tececo_link(problem):
    app_home = path.dirname(path.realpath(__file__))
    dataPath =  path.join(app_home, 'data')
    tecEco = problem.get_tech_eco_analysis()
    carrier = problem.get_energy_carrier("ElectricityDistrib")
    #add port
    portName = "TecEcoPort"
    tecEco_port = tecEco.add_port(portName, carrier, "DATAEXCHANGE", "Total Capex");
    assert tecEco.ports == [portName]
    #create link
    bus = problem.get_bus("Elec_Bus")
    problem.add_link(tecEco_port, bus)
    assert f"TecEco.{portName}" in problem.links
    #remove link 
    problem.remove_link(tecEco_port, bus)
    assert f"TecEco.{portName}" not in problem.links
    #remove port 
    tecEco.remove_port(tecEco_port)
    assert tecEco.ports == []

def read_and_lauch_study_twice(study, app_home=""):
    file_path = path.join(app_home, study + ".json")
    ts_path = path.join(app_home, study + "_dataseries.csv")
    study_home = os.path.join(app_home, study)
    try:
        Cairn_instance = CairnAPI(True)
        problem = Cairn_instance.read_study(file_path)
        if os.path.exists(ts_path):
            problem.add_timeseries(ts_path)
            print("Problem created")
        else:
            print("missing ", ts_path)
            return
    except ValueError as e:
        if "already exist" in str(e):
            print(f"The problem {e}")
        else:
            print(e)
        return
    try:
        sol = problem.run(study_home)
    except:
        print("Error:", study, app_home)
        assert False
    print(sol.status)
    lp1_file = os.path.join(app_home, study, study + "_model1.lp")
    lp_file = os.path.join(app_home, study, study + "_model.lp")
    if os.path.isfile(lp1_file):
        os.remove(lp1_file)
    os.rename(lp_file, lp1_file)
    try:
        sol = problem.run(study_home)
    except:
        print("Error:", study, app_home)
        assert False
    assert filecmp.cmp(lp_file, lp1_file, shallow=False)
    Cairn_instance.close_study()
    return


@pytest.mark.Cairn
@pytest.mark.PythonAPI
@pytest.mark.xdist_group("PythonAPI")
def test_loop_on_all_models_twice():
    json_home = path.join(path.dirname(path.realpath(__file__)), "data", "json_run_twice")
    for root, dirs, files in os.walk(json_home):
        for f in files:
            if ".json" in f and not ("Report" in root):
                read_and_lauch_study_twice(f.replace(".json", ""), json_home)


if __name__ == '__main__':
    app_home = path.dirname(path.realpath(__file__))

    simu_full = path.join(app_home, './data/cairn_training.json')
    timeseries = path.join(app_home, './data/cairn_training_dataseries.csv')
    cairn_instance = CairnAPI(True)
    problem = cairn_instance.read_study(simu_full)
    ely_pem = problem.get_component("ELY_PEM")
    default_ports = ely_pem.default_ports
    print(default_ports)

    test_plan_results(problem)


    # test_sequence_run_compare(problem)
    # add_grid(problem)
    # save(problem, "./cairn_training_new_grid.json")
    # add_label(problem)

    # save(problem, "./cairn_training_labels.json")

    # load_timeseries(problem, timeseries)
    # solution = problem.run("testDir")

    #set_get_SimulationControl(problem)
    #set_get_TecEcoAnalysis(problem)
    # add_component(problem)
    #set_get_Solvers(problem)
    # read_compos(problem)
    # write_compo(problem)
    # read_energy_carriers(problem)
    # write_energy_carrier(problem)
    #modify_port(problem)
    # save(problem, "./results/cairn_training_new.json")
    # load_timeseries(problem,timeseries)
    # df_sens=pd.read_csv("../data/pythonAPI/tab_echantillonnage.csv", sep=";", decimal='.', header=[0,1])
    # df_sens.set_index(('Unnamed: 0_level_0', 'Unnamed: 0_level_1'), inplace=True)
    # run_sensitivity(problem, df_sens, 1000)
    # initialize(problem)
    # save(problem, "../results/pythonAPI/cairn_training_new_2.json")
    # solution = problem.run("testDir")
    # print(solution)
    # read_results(solution)
    # df_ind = get_plan_results(problem)
    # get_ts_results(problem)
    # read_and_lauch_study_twice("constraint")
    # test_loop_on_all_models_twice()
    # add_component(problem)



