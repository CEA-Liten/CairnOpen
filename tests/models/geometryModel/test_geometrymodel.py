import os
from subprocess import Popen
from os import path
import pytest
import shutil
import sys
from CairnNRT import CairnNRT
from PerseeExtractResult import *

@pytest.mark.Cairn
class TestClass:
    def test_runTNR_Json(self):
        app_home = path.dirname(path.realpath(__file__))
        tnr = CairnNRT(app_home)
        
        testcase="geometry_model"

        fileNew=testcase+"_results_PLAN.csv"
        fileRef=testcase+"_results_PLAN_REF.csv"

        tnr.runPersee(tnr_dir=r".", tnr_xml=testcase+".json", tnr_series=testcase+"_dataseries.csv", tnr_name=testcase)
        
        tnr.checkGlobal(typeFile='PLAN', fileNew=fileNew,fileRef=fileRef)
    
    def test_runTNR_Json2(self):
        app_home = path.dirname(path.realpath(__file__))
        tnr = CairnNRT(app_home)
        
        testcase="geo_wo_relax"
        ts_name = "geometry_model"

        fileNew=testcase+"_results_PLAN.csv"
        fileRef=testcase+"_results_PLAN_REF.csv"

        tnr.runPersee(tnr_dir=r".", tnr_xml=testcase+".json", tnr_series=ts_name+"_dataseries.csv", tnr_name=testcase)
        
        tnr.checkGlobal(typeFile='PLAN', fileNew=fileNew,fileRef=fileRef)
        
    def test_runTNR_Json3(self):
        app_home = path.dirname(path.realpath(__file__))
        tnr = CairnNRT(app_home)
        
        testcase="linear"
        ts_name = "geometry_model"

        fileNew=testcase+"_results_PLAN.csv"
        fileRef=testcase+"_results_PLAN_REF.csv"

        tnr.runPersee(tnr_dir=r".", tnr_xml=testcase+".json", tnr_series=ts_name+"_dataseries.csv", tnr_name=testcase)
        
        tnr.checkGlobal(typeFile='PLAN', fileNew=fileNew,fileRef=fileRef)
        
    def test_runTNR_Json4(self):
        app_home = path.dirname(path.realpath(__file__))
        tnr = CairnNRT(app_home)
        
        testcase="constraint"
        ts_name = "geometry_model"

        fileNew=testcase+"_results_PLAN.csv"
        fileRef=testcase+"_results_PLAN_REF.csv"

        tnr.runPersee(tnr_dir=r".", tnr_xml=testcase+".json", tnr_series=ts_name+"_dataseries.csv", tnr_name=testcase)
        
        tnr.checkGlobal(typeFile='PLAN', fileNew=fileNew,fileRef=fileRef)
     
if __name__ == '__main__':
    tc = TestClass()
    tc.test_runTNR_Json()
    tc.test_runTNR_Json2()
    tc.test_runTNR_Json3()
    tc.test_runTNR_Json4()