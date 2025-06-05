import os
from subprocess import Popen
from os import path
import pytest
import shutil
import sys
sys.path.append(os.getenv('PERSEE_APP')+"\\Scripts")
from PegaseTNR import PegaseTNR
from PerseeExtractResult import *

@pytest.mark.Persee

class TestClass:
    def test_runTNR_Json(self):
        app_home = path.dirname(path.realpath(__file__))
        tnr = PegaseTNR(app_home)

        testcase="test_storageGen"

        fileNew=testcase+"_results_PLAN.csv"
        fileRef=testcase+"_results_PLAN_Ref.csv"

        tnr.runPersee(tnr_dir=r".", tnr_xml=testcase+".json", tnr_series=testcase+"_dataseries.csv", tnr_name=testcase)

        tnr.checkGlobal(typeFile='PLAN', fileNew=fileNew,fileRef=fileRef)
        tnr.check(r"", testcase+"_results-Reference.csv",testcase+"_results.csv")

if __name__ == '__main__':
    tc = TestClass()
    tc.test_runTNR_Json()
