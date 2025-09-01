from os import path, remove
import pytest
from CairnNRT import CairnNRT
from cairn import *

@pytest.fixture(autouse=True, scope="module")
def clean():
    app_home = path.dirname(path.realpath(__file__))
    file_list = ["cairn_training_Sortie.csv"]
    for f in file_list:
        p = path.join(app_home, "TNR", f)
        if path.exists(p): 
            remove(p)
    yield

@pytest.mark.Cairn
def test_runTNR_Json():
    test_case="cairn_training"
    
    app_home = path.dirname(path.realpath(__file__))
    simu_full =  path.join(app_home, test_case+'.json')    
    timeseries =  path.join(app_home, test_case+'_dataseries.csv')
    
    cairn_instance = CairnAPI(True)
    problem = cairn_instance.read_study(simu_full)
    problem.add_timeseries(timeseries)
    
    problem.run()
    
    tnr = CairnNRT(app_home)
    
    tnr.checkGlobal(typeFile='HIST', fileNew=test_case+"_results_PLAN.csv",fileRef=test_case+"_results_PLAN_REF.csv")
    tnr.checkGlobal(typeFile='HIST', fileNew=test_case+"_results_HIST.csv",fileRef=test_case+"_results_HIST_REF.csv")

   
@pytest.mark.Cairn
def test_runTNRHighs_Json():
    test_case="cairn_training_HiGHS"
    
    app_home = path.dirname(path.realpath(__file__))
    simu_full =  path.join(app_home, test_case+'.json')    
    timeseries =  path.join(app_home, 'cairn_training_dataseries.csv')
    
    cairn_instance = CairnAPI(True)
    problem = cairn_instance.read_study(simu_full)
    problem.add_timeseries(timeseries)
    
    problem.run()
    
    tnr = CairnNRT(app_home)
    
    tnr.checkGlobal(typeFile='HIST', fileNew=test_case+"_results_PLAN.csv",fileRef=test_case+"_results_PLAN_REF.csv")	
    

if __name__ == '__main__':
    test_runTNR_Json()
    test_runTNRHighs_Json()