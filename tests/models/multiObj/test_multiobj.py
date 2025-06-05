import os
from subprocess import Popen
from os import path
import pytest
from PegaseTNR import PegaseTNR

@pytest.mark.Persee
def test_runTNR_Json():
    app_home = path.dirname(path.realpath(__file__))
    tnr = PegaseTNR(app_home)
    
    testcase = "test_multiobj"
    
    tnr.runPersee(tnr_dir=r".", tnr_xml=testcase+".json", tnr_series=testcase+"_dataseries.csv", tnr_name="MultiObj", maxtime=1200)
    tnr.check(r".", testcase+"_results_Results.csv", testcase+"_results_Results_Ref.csv")
    tnr.checkGlobal(typeFile='PLAN', fileNew=testcase+"_results_PLAN.csv", fileRef=testcase+"_results_PLAN_Ref.csv")
    tnr.checkGlobal(typeFile='HIST', fileNew=testcase+"_results_HIST.csv", fileRef=testcase+"_results_HIST_Ref.csv")
 
if __name__ == '__main__':
    test_runTNR_Json()
