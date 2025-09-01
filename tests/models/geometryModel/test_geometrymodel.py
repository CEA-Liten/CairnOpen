from os import path
import pytest
from CairnNRT import CairnNRT
from cairn import *

@pytest.mark.Cairn
class TestClass:
    def test_runTNR_Json(self):
        test_case="geometry_model"
        
        app_home = path.dirname(path.realpath(__file__))
        simu_full =  path.join(app_home, test_case+'.json')    
        timeseries =  path.join(app_home, test_case+'_dataseries.csv')
        
        cairn_instance = CairnAPI(True)
        problem = cairn_instance.read_study(simu_full)
        problem.add_timeseries(timeseries)
        
        problem.run()
        
        tnr = CairnNRT(app_home)

        fileNew=test_case+"_results_PLAN.csv"
        fileRef=test_case+"_results_PLAN_REF.csv"
        
        tnr.checkGlobal(typeFile='PLAN', fileNew=fileNew,fileRef=fileRef)
    
    def test_runTNR_Json2(self):
        test_case="geo_wo_relax"
        ts_name = "geometry_model"
        
        app_home = path.dirname(path.realpath(__file__))
        simu_full =  path.join(app_home, test_case+'.json')    
        timeseries =  path.join(app_home, ts_name+'_dataseries.csv')
        
        cairn_instance = CairnAPI(True)
        problem = cairn_instance.read_study(simu_full)
        problem.add_timeseries(timeseries)
        
        problem.run()
        
        tnr = CairnNRT(app_home)

        fileNew=test_case+"_results_PLAN.csv"
        fileRef=test_case+"_results_PLAN_REF.csv"
        
        tnr.checkGlobal(typeFile='PLAN', fileNew=fileNew,fileRef=fileRef)
        
    def test_runTNR_Json3(self):
        test_case="linear"
        ts_name = "geometry_model"

        app_home = path.dirname(path.realpath(__file__))
        simu_full =  path.join(app_home, test_case+'.json')    
        timeseries =  path.join(app_home, ts_name+'_dataseries.csv')
        
        cairn_instance = CairnAPI(True)
        problem = cairn_instance.read_study(simu_full)
        problem.add_timeseries(timeseries)
        
        problem.run()
        
        tnr = CairnNRT(app_home)

        fileNew=test_case+"_results_PLAN.csv"
        fileRef=test_case+"_results_PLAN_REF.csv"
        
        tnr.checkGlobal(typeFile='PLAN', fileNew=fileNew,fileRef=fileRef)
        
    def test_runTNR_Json4(self):
        test_case="constraint"
        ts_name = "geometry_model"

        app_home = path.dirname(path.realpath(__file__))
        simu_full =  path.join(app_home, test_case+'.json')    
        timeseries =  path.join(app_home, ts_name+'_dataseries.csv')
        
        cairn_instance = CairnAPI(True)
        problem = cairn_instance.read_study(simu_full)
        problem.add_timeseries(timeseries)
        
        problem.run()
        
        tnr = CairnNRT(app_home)

        fileNew=test_case+"_results_PLAN.csv"
        fileRef=test_case+"_results_PLAN_REF.csv"
        
        tnr.checkGlobal(typeFile='PLAN', fileNew=fileNew,fileRef=fileRef)
     
if __name__ == '__main__':
    tc = TestClass()
    tc.test_runTNR_Json()
    tc.test_runTNR_Json2()
    tc.test_runTNR_Json3()
    tc.test_runTNR_Json4()