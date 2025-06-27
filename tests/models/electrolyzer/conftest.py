import os
from os import path
import pytest
import sys
import pandas as pd

from PerseeSensParam import run_sensitivity_echantillonnage_manuel

"""
def pytest_addoption(parser):
    parser.addoption("--all", action="store_true", help="run all combinations")
"""

def pytest_generate_tests(metafunc):
    app_home = path.dirname(path.realpath(__file__))
    
    test_case = "test_electrolyzer"
    
    run_sensitivity_echantillonnage_manuel(test_case,app_home,"sampling.csv",1000,True,"", [test_case+"_dataseries.csv"])
    
    #Read sampling to know which case to compare
    df = pd.read_csv(os.path.join(app_home, "sampling.csv"), sep=";", decimal=".", index_col=0)
    
    metafunc.parametrize("test_case", [test_case])
    metafunc.parametrize("subcase", df.index.tolist())
    
if __name__ == '__main__':
    app_home = path.dirname(path.realpath(__file__))
    test_case = "test_electrolyzer"
    run_sensitivity_echantillonnage_manuel(test_case,app_home,"sampling.csv",1000,True,"", [test_case+"_dataseries.csv"])