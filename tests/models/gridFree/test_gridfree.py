import os
from subprocess import Popen
from os import path
import pytest
import shutil
import sys
from CairnNRT import CairnNRT
from PerseeExtractResult import *

class TestClass:
    def test_runTNR_Json(self, test_case):
        app_home = path.dirname(path.realpath(__file__))
        tnr = CairnNRT(app_home)
        
        tnr.runPersee(tnr_dir=r".", tnr_xml=test_case+".json", tnr_series=test_case+"_dataseries.csv", tnr_name=test_case)
        
        tnr.check(r"", test_case+"_results_Results.csv", test_case+"_results_Results_Ref.csv")
        tnr.checkGlobal(typeFile='HIST', fileNew=test_case+"_results_HIST.csv", fileRef=test_case+"_results_HIST_Ref.csv")
        tnr.checkGlobal(typeFile='PLAN', fileNew=test_case+"_results_PLAN.csv", fileRef=test_case+"_results_PLAN_Ref.csv")
    
    @pytest.mark.Cairn
    def test_run_base(self):
        self.test_runTNR_Json("test_gridfree")

if __name__ == '__main__':
    tc = TestClass()
    test_case = "test_gridfree"
    tc.test_runTNR_Json(test_case)
