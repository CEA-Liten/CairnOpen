import os
from os import path, remove
import pytest
import sys
sys.path.append(os.getenv('PERSEE_APP')+"\\Scripts")
from PegaseTNR import PegaseTNR

@pytest.fixture(autouse=True, scope="module")
def clean():
    app_home = path.dirname(path.realpath(__file__))
    file_list = ["formation_persee_Sortie.csv"]
    for f in file_list:
        p = path.join(app_home, "TNR", f)
        if path.exists(p): 
            remove(p)
    yield

@pytest.mark.Persee
class TestClass:

    @pytest.mark.PerseeJson
    def test_runTNR_Json(self):
        app_home = path.dirname(path.realpath(__file__))
        tnr = PegaseTNR(app_home)
        
        testcase="formation_persee"

        lpNew = testcase+"_model.lp"
        lpRef = testcase+"_model_ref.lp"
        
        tnr.runPersee(tnr_dir=r".", tnr_xml=testcase+".json", tnr_series="formation_persee_dataseries.csv", tnr_name=testcase)
        tnr.checklp(typeFile='lp', fileNew=lpNew,fileRef=lpRef)
        

if __name__ == '__main__':
    tc = TestClass()
    tc.test_runTNR_Json