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

    lpNew = test_case+"_model.lp"
    lpRef = test_case+"_model_ref.lp"
    
    tnr.checklp(typeFile='lp', fileNew=lpNew,fileRef=lpRef)
        

if __name__ == '__main__':
    test_runTNR_Json()