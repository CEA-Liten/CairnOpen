from os import path, remove
import pytest
from CairnNRT import CairnNRT

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
def test_peak_shaving():
    app_home = path.dirname(path.realpath(__file__))
    tnr = CairnNRT(app_home)
    
    testcase="test_sourceload"

    fileNew=testcase+"_results_PLAN.csv"
    fileRef=testcase+"_results_PLAN_Ref.csv"

    tnr.runPersee(tnr_dir=r".", tnr_xml=testcase+".json", tnr_series=testcase+"_dataseries.csv", tnr_name=testcase)
    
    tnr.check("", testcase+"_Results_Ref.csv",testcase+"_results_Results.csv")
    tnr.checkGlobal(typeFile='PLAN', fileNew=fileNew,fileRef=fileRef)
    tnr.checkGlobal(typeFile='HIST', fileNew=testcase+"_results_HIST.csv",fileRef=testcase+"_results_HIST_REF.csv")

   
@pytest.mark.Cairn
def test_peak_shedding():
    app_home = path.dirname(path.realpath(__file__))
    tnr = CairnNRT(app_home)
    
    testcase="test_sourceload_shedding"

    fileNew=testcase+"_results_PLAN.csv"
    fileRef=testcase+"_results_PLAN_Ref.csv"
    
    tnr.runPersee(tnr_dir=r".", tnr_xml=testcase+".json", tnr_series=testcase+"_dataseries.csv", tnr_name=testcase)
    
    tnr.check("", testcase+"_Results_Ref.csv",testcase+"_results_Results.csv")
    tnr.checkGlobal(typeFile='PLAN', fileNew=fileNew,fileRef=fileRef)
    tnr.checkGlobal(typeFile='HIST', fileNew=testcase+"_results_HIST.csv",fileRef=testcase+"_results_HIST_REF.csv") 	
    

if __name__ == '__main__':
    test_peak_shaving()
    test_peak_shedding()