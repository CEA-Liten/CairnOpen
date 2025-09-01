import os
from os import path
import pytest
import pandas as pd
from cairn import *

"""
def pytest_addoption(parser):
    parser.addoption("--all", action="store_true", help="run all combinations")
"""

def pytest_generate_tests(metafunc):
    test_case = "test_compressor"
    
    app_home = path.dirname(path.realpath(__file__))
    simu_full =  path.join(app_home, test_case+'.json')    
    timeseries =  path.join(app_home, test_case+'_dataseries.csv')
    
    cairn_instance = CairnAPI(True)
    problem = cairn_instance.read_study(simu_full)
    problem.add_timeseries(timeseries)
    
    #Read sampling 
    df_sens = pd.read_csv(os.path.join(app_home, "sampling.csv"), sep=";", decimal=".", header=[0,1], index_col=0)
    run_sensitivity(problem, df_sens, 1000)
    
    #Send cases to compare
    metafunc.parametrize("test_case", [test_case])
    metafunc.parametrize("subcase", df_sens.index.tolist())