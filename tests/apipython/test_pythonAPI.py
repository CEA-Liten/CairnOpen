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

@pytest.fixture(autouse=True, scope="module")
def clean():
    app_home = path.dirname(path.realpath(__file__))
    resultPath =  path.join(app_home, 'results')
    if path.exists(resultPath):
        print("***************************************************")
        print(os.path.join(resultPath, '*.csv'))
        for csvpath in glob.iglob(os.path.join(resultPath, '*.csv')):
            os.remove(csvpath)
        for jsonpath in glob.iglob(os.path.join(resultPath, '*.json')):
            os.remove(jsonpath)
    yield

@pytest.fixture
def problem():
    app_home = path.dirname(path.realpath(__file__))
    simu_full =  path.join(app_home, './data/cairn_training.json')    
    cairn_instance = CairnAPI(False)
    problem = cairn_instance.read_study(simu_full)    
    yield problem


def set_get_SimulationControl(problem):
    assert problem.simulation_control['FutureSize']==48
    problem.simulation_control = {'FutureSize': 168}
    assert problem.simulation_control['FutureSize']==168

def set_get_TecEcoAnalysis(problem):
    #Double value
    assert problem.tech_eco_analysis['DiscountRate']==0.07
    problem.tech_eco_analysis = {'DiscountRate':  0.06}
    assert problem.tech_eco_analysis['DiscountRate']==0.06
    print(problem.tech_eco_analysis['DiscountRate'])
    #String list value
    assert len(problem.tech_eco_analysis['ConsideredEnvironmentalImpacts']) == 2
    assert 'Climate change#Global Warming Potential 100' in problem.tech_eco_analysis['ConsideredEnvironmentalImpacts']
    assert 'Acidification#Accumulated Exceedance' in problem.tech_eco_analysis['ConsideredEnvironmentalImpacts']
    try:
        problem.tech_eco_analysis = {'ConsideredEnvironmentalImpacts': 'Climate change#Global Warming Potential 100' }
    except ValueError:
        print("Not possible to set tech eco analysis - string list")
    assert len(problem.tech_eco_analysis['ConsideredEnvironmentalImpacts']) == 1
    assert 'Acidification#Accumulated Exceedance' not in problem.tech_eco_analysis['ConsideredEnvironmentalImpacts']
    assert 'Climate change#Global Warming Potential 100' in problem.tech_eco_analysis['ConsideredEnvironmentalImpacts']

def set_get_Solvers(problem):
    print(problem.solver)
    print(problem.solver['Gap'])
    #Double value
    assert problem.solver['Gap']==0.001
    problem.solver = {'Gap': 0.0005}
    assert problem.solver['Gap']==0.0005
    #String value
    assert problem.solver['WriteLp']=="YES"
    problem.solver = {'WriteLp': "NO"}
    assert problem.solver['WriteLp']=="NO"

def read_compos(problem):    
    print(problem.get_components())
    app_home = path.dirname(path.realpath(__file__))
    resultPath =  path.join(app_home, 'results')
    for compo in problem.get_components():
        my_compo = problem.get_component(compo)
        my_compo_setting_values = my_compo.setting_values
        param_list=[]
        values_list=[]
        for name in my_compo_setting_values:
            param_list.append(name)
            values_list.append(my_compo_setting_values[name])
        #dataframe
        data = {'paramValue': values_list, 'paramName': param_list}
        df = pd.DataFrame(data)
        df.set_index('paramName', inplace=True)
        df.to_csv(resultPath+"./"+compo+".csv", sep=';', decimal='.')
        
        #Comparer chaque fichier à une référence ?
    
def write_compo(problem):
    app_home = path.dirname(path.realpath(__file__))
    dataPath =  path.join(app_home, 'data')
    ely_pem_csv=dataPath+"./ELY_PEM_to_write.csv"
    ely_pem_df=pd.read_csv(ely_pem_csv, sep=';', decimal='.')
    ely_pem_df.set_index('paramName', inplace=True)
    ely_pem = problem.get_component("ELY_PEM")
    assert ely_pem.get_setting_value("AddAuxConso") == 0
    ely_pem.set_setting_value("AddAuxConso", ely_pem_df.at["AddAuxConso","paramValue"])
    assert ely_pem.get_setting_value("AddAuxConso") == 1
    assert ely_pem.get_setting_value("AuxConso") == 0
    ely_pem.set_setting_value("AuxConso", ely_pem_df.at["AuxConso","paramValue"])
    assert ely_pem.get_setting_value("AuxConso") == 0.01
    #timeseries
    ely_pem.set_setting_value("ComponentAvailability", "Availability")
    assert ely_pem.get_setting_value("ComponentAvailability") == "Availability"

def add_grid(problem):
    my_GFSP = problem.create_component("Grid_Surplus", "GridFree")
    h2_bus = problem.get_bus("H2_Bus")
    port_GFSP = my_GFSP.get_port("PortR0")
    h2_carrier = problem.get_energy_carrier("H2")
    port_GFSP.set_carrier(h2_carrier)
    print(port_GFSP.settings)  
    port_GFSP.set_setting_value("Direction", "INPUT") 
    problem.add_link(port_GFSP, h2_bus)
    assert port_GFSP.get_setting_value("Direction") == "INPUT"
    assert my_GFSP.direction == "InjectToGrid"
    

def read_energy_carriers(problem):
    print(problem.energy_carriers)
    app_home = path.dirname(path.realpath(__file__))
    resultPath =  path.join(app_home, 'results')
    for carrier in problem.energy_carriers:
        my_carrier = problem.get_energy_carrier(carrier)
        my_carrier_setting_values = my_carrier.setting_values
        param_list=[]
        values_list=[]
        for name in my_carrier_setting_values:
            param_list.append(name)
            values_list.append(my_carrier_setting_values[name])
        #dataframe
        data = {'paramValue': values_list, 'paramName': param_list}
        df = pd.DataFrame(data)
        df.set_index('paramName', inplace=True)
        df.to_csv(resultPath+"./"+carrier+".csv", sep=';', decimal='.')
        
        #Comparer chaque fichier à une référence ?

def write_energy_carrier(problem):
    app_home = path.dirname(path.realpath(__file__))
    dataPath =  path.join(app_home, 'data')
    h2_csv=dataPath + "./H2_to_write.csv"
    h2_df=pd.read_csv(h2_csv, sep=';', decimal='.')
    h2_df.set_index('paramName', inplace=True)
    h2 = problem.get_energy_carrier("H2")
    assert h2.get_setting_value("LHV") == 0.03332
    h2.set_setting_value("LHV", h2_df.at["LHV","paramValue"])
    assert h2.get_setting_value("LHV") == 0.03333333

def add_component(problem):
    my_pv_field = problem.create_component("PV", "SourceLoad")
    elec_bus = problem.get_energy_carrier("ElectricityDistrib")
    defaultPorts = my_pv_field.default_ports
    assert len(defaultPorts) == 1
    my_PV_L0 = my_pv_field.get_port(defaultPorts[0]) #"PortL0"
    my_PV_L0.set_carrier(elec_bus)
    my_PV_L0.setting_values = {
    				"Direction": "OUTPUT",
    				"Variable": "SourceLoadFlow"
    		}
    my_pv_field.setting_values = {
	"Weight": 1,
	"Opex": 0 ,
    "Capex":1000,
	"MaxFlow": -10,
	"EcoInvestModel": 1,
    "UseProfileLoadFlux":"PVProduction"
	}
    assert my_pv_field.get_setting_value("UseProfileLoadFlux") == "PVProduction"
    my_pv_field.set_setting_value("UseProfileLoadFlux","WindFarmProduction")
    assert my_pv_field.get_setting_value("UseProfileLoadFlux") == "WindFarmProduction"

def modify_port(problem):
    h2_load = problem.get_component("H2_Load")
    port = h2_load.get_port("PortL0")
    assert port.get_setting_value("Coeff") == 1.0
    port.set_setting_value("Coeff", 2)
    assert port.get_setting_value("Coeff") == 2.0
    assert port.get_setting_value("Offset") == 0.0
    port.set_setting_value("Offset", 10)
    assert port.get_setting_value("Offset") == 10.0

def load_timeseries(problem, file):
    problem.add_timeseries(file)

def save(problem, new_name):
    problem.save_study(new_name)
    #Verify that the file exists
    assert os.path.isfile(new_name)
    
def initialize(problem):
    problem.initialize()

def run(problem, folder):
    return problem.run(folder)

def add_label(problem):
    problem.add_label("country")
    problem.add_label("year")
    problem.add_label("site")
    problem.remove_label("year")
    print(problem.labels)
    # problem.labels = {"country", "year"}
    # print(problem.labels)

    assert len(problem.labels) == 2 and 'country' in problem.labels and 'site' in problem.labels

    ely_pem = problem.get_component("ELY_PEM")
    ely_pem.label_values = {"country": "France", "site": "Grenoble"}
    print(ely_pem.label_values)

    wind_farm = problem.get_component("Wind_farm")
    wind_farm.set_label_value("country", "France")
    wind_farm.set_label_value("site", "Noyarey")

def get_plan_results(problem):
    ely_pem = problem.get_component("ELY_PEM")
    indicators = ely_pem.indicators
    assert round(ely_pem.get_indicator_value("Installed Size", "PLAN"),5) == 2.48392
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
def test_set_get_SimulationControl(problem):    
    set_get_SimulationControl(problem)

@pytest.mark.Cairn
@pytest.mark.PythonAPI
def test_set_get_TecEcoAnalysis(problem):    
    set_get_TecEcoAnalysis(problem)

@pytest.mark.Cairn
@pytest.mark.PythonAPI
#@pytest.mark.skip("need to manage the Highs case")
def test_set_get_Solvers(problem):
    #print("need to manage the Highs case")    
    set_get_Solvers(problem)

@pytest.mark.Cairn
@pytest.mark.PythonAPI
def test_add_component(problem):    
    add_component(problem)

@pytest.mark.Cairn
@pytest.mark.PythonAPI
def test_modify_port(problem):    
    modify_port(problem)

@pytest.mark.Cairn
@pytest.mark.PythonAPI
def test_get_energy_carrier(problem):
    # Add a new energy carrier
    energy_carrier_name = "ElectricityDistrib2"
    energy_carrier_type = "Electrical"  # Remplacez par le type approprié
    problem.create_energy_carrier(energy_carrier_name, energy_carrier_type)

    # Get the energy vector
    energy_carrier = problem.get_energy_carrier(energy_carrier_name)

    # Vérifier que le porteur d'énergie a été correctement récupéré
    assert energy_carrier is not None
    assert energy_carrier.name == energy_carrier_name
    assert energy_carrier.type == energy_carrier_type

    # Check the settings
    ev_settings = energy_carrier.settings
    assert isinstance(ev_settings, list)
    assert "Potential" in ev_settings

    # Check values
    setting_value = energy_carrier.get_setting_value("Potential")
    assert setting_value is not None
    new_value = 440
    energy_carrier.set_setting_value("Potential", new_value)
    updated_value = energy_carrier.get_setting_value("Potential")
    assert updated_value == new_value

@pytest.mark.Cairn
@pytest.mark.PythonAPI
def test_add_udl(problem):
    add_label(problem)

@pytest.mark.Cairn
@pytest.mark.PythonAPI
def test_add_grid(problem):    
    add_grid(problem)

@pytest.mark.Cairn
@pytest.mark.PythonAPI
def test_add_port(problem):
    ely_pem = problem.get_component("ELY_PEM")
    carrier = problem.get_energy_carrier("ElectricityDistrib")
    port_new = Port(ely_pem,"test_port",carrier)
    assert port_new.carrier_name == "ElectricityDistrib"
    ely_pem.remove_port(port_new, True)

@pytest.mark.Cairn
@pytest.mark.PythonAPI
def test_sequence_1(problem):        
    set_get_SimulationControl(problem)
    set_get_TecEcoAnalysis(problem)
    #set_get_Solvers(problem)
    read_compos(problem)
    write_compo(problem)
    read_energy_carriers(problem)
    #print(problem)
    app_home = path.dirname(path.realpath(__file__))
    resultPath =  path.join(app_home, 'results')
    save(problem, resultPath + "./cairn_training_new.json")

@pytest.mark.Cairn
@pytest.mark.PythonAPI
def test_sequence_2(problem):    
    add_component(problem)
    app_home = path.dirname(path.realpath(__file__))
    resultPath =  path.join(app_home, 'results')
    save(problem, resultPath +  "./cairn_training_modif.json")

@pytest.mark.Cairn
@pytest.mark.PythonAPI
def test_sequence_run_compare(problem):
    app_home = path.dirname(path.realpath(__file__))
    dataPath =  path.join(app_home, 'data')    
    load_timeseries(problem,dataPath + "./cairn_training_dataseries.csv")
    solution = run(problem, "")
    get_plan_results(problem)
    get_ts_results(problem)
    
def read_and_lauch_study_twice(study, app_home=""):
    if app_home == "":
        app_home = path.dirname(path.realpath(__file__))
    filePath = path.join(app_home,study+".json")
    ts_path = path.join(app_home,study+"_dataseries.csv")
    try:
        Cairn_instance = CairnAPI(True) 
        problem = Cairn_instance.read_study(filePath)
        if os.path.exists(ts_path):
            problem.add_timeseries(ts_path)
            print("problème créé")
        else:
            print("manque ", ts_path)
            return
    except ValueError as e:
        if "already exist" in str(e):
            print(f"Le problème {e}")
        else:
            print(e)
        return
    try:
        sol = problem.run()
    except:
        print("Error:",study, app_home)
        assert False
    print(sol.status)
    if os.path.isfile(filePath.replace(".json","_model1.lp")):
        os.remove(filePath.replace(".json","_model1.lp"))
    os.rename(filePath.replace(".json","_model.lp"),filePath.replace(".json","_model1.lp"))
    try:
        sol = problem.run()
    except:
        print("Error:",study,app_home)
        assert False
    assert filecmp.cmp(filePath.replace(".json","_model.lp"),filePath.replace(".json","_model1.lp"), shallow=False)
    Cairn_instance.close_study()
    return

@pytest.mark.Cairn
@pytest.mark.PythonAPI
def test_loop_on_all_models_twice():
    models_home = path.join(path.dirname(path.realpath(__file__)),"..//models/")
    for root, dirs, files in os.walk(models_home):
        for f in files:
            if ".json" in f and not("Report" in root):
                print(f)
                read_and_lauch_study_twice(f.replace(".json",""), app_home = root)



if __name__ == '__main__':
    app_home = path.dirname(path.realpath(__file__))
    simu_full =  path.join(app_home, './data/cairn_training.json')    
    timeseries =  path.join(app_home, './data/cairn_training_dataseries.csv')    
    cairn_instance = CairnAPI(True)
    problem = cairn_instance.read_study(simu_full)

    #test_sequence_run_compare(problem)
    #add_grid(problem)
    #save(problem, "./cairn_training_new_grid.json")
    add_label(problem)

    save(problem, "./cairn_training_labels.json")

    load_timeseries(problem, timeseries)
    solution = problem.run("testDir")

    """
    #set_get_SimulationControl(problem)
    set_get_TecEcoAnalysis(problem)
    add_component(problem)
    #set_get_Solver(problem)
    #read_compos(problem)
    #write_compo(problem)
    #read_energy_carriers(problem)
    #write_energy_carrier(problem)
    #modify_port(problem)
    #save(problem, "./results/cairn_training_new.json")
    load_timeseries(problem,timeseries)
    #df_sens=pd.read_csv("../data/pythonAPI/tab_echantillonnage.csv", sep=";", decimal='.', header=[0,1])
    #df_sens.set_index(('Unnamed: 0_level_0', 'Unnamed: 0_level_1'), inplace=True)
    #run_sensitivity(problem, df_sens, 1000)
    #initialize(problem)
    #save(problem, "../results/pythonAPI/cairn_training_new_2.json")
    solution = problem.run("testDir")
    #print(solution)
    read_results(solution)
    df_ind = get_plan_results(problem)
    get_ts_results(problem)
    read_and_lauch_study_twice("constraint")
    test_loop_on_all_models_twice()
    #add_component(problem)
    """
    
    
    