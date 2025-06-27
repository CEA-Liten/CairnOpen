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

@pytest.fixture(autouse=True, scope="module")
def clean():
    app_home = path.dirname(path.realpath(__file__))
    resultPath =  path.join(app_home, 'results')
    if path.exists(resultPath):
        shutil.rmtree(resultPath)            
    os.makedirs(resultPath)           
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
    ely_pem.set_setting_value("UseProfileConverterUse", "Availability")
    assert ely_pem.get_setting_value("UseProfileConverterUse") == "Availability"
    
    

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
    my_pv_field = Component("PV", "SourceLoad", "SourceLoad")
    my_pv_field.setting_values = {
		"Direction": "Source" ,
		"Weight": 1,
		"Opex": 0 ,
        "Capex":1000,
		"MaxFlow": -10,
		"EcoInvestModel": 1,
        "UseProfileLoadFlux":"PVProduction"
		}
    elec_bus=problem.get_energy_carrier("ElectricityDistrib")
    my_PV_R0 = Port("PortR0", elec_bus)
    my_PV_R0.setting_values = {
    				"Direction": "OUTPUT",
    				"Variable": "SourceLoadFlow"
    		}
    my_pv_field.add_port(my_PV_R0)
    problem.add_component(my_pv_field)
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

def read_results(solution):
    assert solution.status == "Optimal"
    assert solution.nb_solutions == 1
    assert round(solution.get_optimal_solution(0),2) == 2.48

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
def test_set_get_Solvers(problem):    
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
def test_sequence_1(problem):        
    set_get_SimulationControl(problem)
    set_get_TecEcoAnalysis(problem)
    set_get_Solvers(problem)
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
    read_results(solution)
    get_plan_results(problem)
    get_ts_results(problem)
    

if __name__ == '__main__':
    app_home = path.dirname(path.realpath(__file__))
    simu_full =  path.join(app_home, './data/cairn_training.json')    
    timeseries =  path.join(app_home, './data/cairn_training_dataseries.csv')    
    cairn_instance = CairnAPI(True)
    problem = cairn_instance.read_study(simu_full)
    #set_get_SimulationControl(problem)
    #set_get_TecEcoAnalysis(problem)
    #add_component(problem)
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
    
    #add_component(problem)
    
    
    
    