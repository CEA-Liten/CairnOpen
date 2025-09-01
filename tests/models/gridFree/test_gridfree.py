from os import path
import pytest
from CairnNRT import CairnNRT
from cairn import *

@pytest.mark.Cairn
def test_runTNR_Json():
    test_case="test_gridfree"
    
    app_home = path.dirname(path.realpath(__file__))
    simu_full =  path.join(app_home, test_case+'.json')    
    timeseries =  path.join(app_home, test_case+'_dataseries.csv')
    
    cairn_instance = CairnAPI(True)
    problem = cairn_instance.read_study(simu_full)
    problem.add_timeseries(timeseries)
    
    problem.run()
    
    tnr = CairnNRT(app_home)

    tnr.checkGlobal(typeFile='PLAN', fileNew=test_case+"_results_PLAN.csv", fileRef=test_case+"_results_PLAN_Ref.csv")
    tnr.checkGlobal(typeFile='HIST', fileNew=test_case+"_results_HIST.csv", fileRef=test_case+"_results_HIST_Ref.csv")
    tnr.check("", test_case+"_results_Results.csv",test_case+"_results_Results_Ref.csv")

if __name__ == '__main__':
    test_runTNR_Json()
