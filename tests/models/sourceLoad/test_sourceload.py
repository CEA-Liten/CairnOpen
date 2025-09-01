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
def test_source_load():
    test_case="test_sourceload"
    
    app_home = path.dirname(path.realpath(__file__))
    simu_full =  path.join(app_home, test_case+'.json')    
    timeseries =  path.join(app_home, test_case+'_dataseries.csv')
    
    cairn_instance = CairnAPI(True)
    problem = cairn_instance.read_study(simu_full)
    problem.add_timeseries(timeseries)
    
    problem.run()
    
    tnr = CairnNRT(app_home)
    
    tnr.check("", test_case+"_Results_Ref.csv",test_case+"_results_Results.csv")
    tnr.checkGlobal(typeFile='PLAN', fileNew=test_case+"_results_PLAN.csv",fileRef=test_case+"_results_PLAN_REF.csv")
    tnr.checkGlobal(typeFile='HIST', fileNew=test_case+"_results_HIST.csv",fileRef=test_case+"_results_HIST_REF.csv")

@pytest.mark.Cairn
def test_peak_shaving():
    test_case="test_sourceload_shaving"
    
    app_home = path.dirname(path.realpath(__file__))
    simu_full =  path.join(app_home, test_case+'.json')    
    timeseries =  path.join(app_home, test_case+'_dataseries.csv')
    
    cairn_instance = CairnAPI(True)
    problem = cairn_instance.read_study(simu_full)
    problem.add_timeseries(timeseries)
    
    problem.run()
    
    tnr = CairnNRT(app_home)
    
    tnr.check("", test_case+"_Results_Ref.csv",test_case+"_results_Results.csv")
    tnr.checkGlobal(typeFile='PLAN', fileNew=test_case+"_results_PLAN.csv",fileRef=test_case+"_results_PLAN_REF.csv")
    tnr.checkGlobal(typeFile='HIST', fileNew=test_case+"_results_HIST.csv",fileRef=test_case+"_results_HIST_REF.csv")

   
@pytest.mark.Cairn
def test_load_shedding():
    test_case="test_sourceload_shedding"
    
    app_home = path.dirname(path.realpath(__file__))
    simu_full =  path.join(app_home, test_case+'.json')    
    timeseries =  path.join(app_home, test_case+'_dataseries.csv')
    
    cairn_instance = CairnAPI(True)
    problem = cairn_instance.read_study(simu_full)
    problem.add_timeseries(timeseries)
    
    problem.run()
    
    tnr = CairnNRT(app_home)
    
    tnr.check("", test_case+"_Results_Ref.csv",test_case+"_results_Results.csv")
    tnr.checkGlobal(typeFile='PLAN', fileNew=test_case+"_results_PLAN.csv",fileRef=test_case+"_results_PLAN_REF.csv")
    tnr.checkGlobal(typeFile='HIST', fileNew=test_case+"_results_HIST.csv",fileRef=test_case+"_results_HIST_REF.csv")
    

if __name__ == '__main__':
    test_source_load()
    test_peak_shaving()
    test_load_shedding()