# -*- coding: utf-8 -*-
"""
Created on Sun Dec 22 15:33:30 2024

@author: sc258201
"""
import pytest
try:
    from cairn import *
except:
    from cairnopen import *
import os
import shutil
from os import path
import pandas as pd
import filecmp
import glob
import sys

@pytest.mark.Cairn
@pytest.mark.PythonAPI
def test_runSensitivity():
    testName = "test_compressor"
    app_home = path.dirname(path.realpath(__file__))   
    simu_path = path.join(app_home, '..', 'models', 'compressor')
    results_path = path.join(app_home, 'results', 'test_runSensitivity')
    if not os.path.exists(results_path):
        os.makedirs(results_path)

    simu_full = path.join(results_path, testName+".json")
    timeseries = path.join(results_path, testName+"_dataseries.csv")
    sampling = path.join(results_path, "sampling.csv")
    samplingkpi = path.join(results_path, "kpi_sampling.csv")
    samplingres = path.join(results_path, "sampling_results.csv")

    shutil.copy(path.join(simu_path, testName+".json"), simu_full)
    shutil.copy(path.join(simu_path, testName+"_dataseries.csv"), timeseries)
    shutil.copy(path.join(simu_path, "sampling.csv"), sampling)
    shutil.copy(path.join(simu_path, "kpi_sampling.csv"), samplingkpi)
    
    cairn_instance = CairnAPI(True)
    problem = cairn_instance.read_study(simu_full)
    problem.add_timeseries(timeseries)
    df_sens=pd.read_csv(sampling, sep=";", decimal='.', header=[0,1])
    df_sens.set_index(('Unnamed: 0_level_0', 'Unnamed: 0_level_1'), inplace=True)
    df_kpi = pd.read_csv(samplingkpi, sep=";")
    tab_res = run_sensitivity(problem,df_sens,-1, df_kpi)    
    tab_res.to_csv(samplingres,sep=";")

    tab_ref = pd.read_csv(path.join(simu_path, "sampling_results_ref.csv"),sep=";", index_col=0)
    tab_res["Case"] = tab_res["Case"].astype(str)
    tab_ref["Case"] = tab_ref["Case"].astype(str)
    diff = (tab_res.round(decimals=3).sort_values('Case', ignore_index=True).reindex(sorted(tab_res.columns), axis=1)).compare(tab_ref.round(decimals=3).sort_values('Case', ignore_index=True).reindex(sorted(tab_ref.columns), axis=1))
    assert(len(diff)==0)
     

if __name__ == '__main__':
    test_runSensitivity()

   


