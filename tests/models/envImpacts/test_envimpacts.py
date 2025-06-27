import os
from os import path
import pytest
import sys
import pandas as pd
from PerseeSensParam import run_sensitivity_echantillonnage_manuel
from CairnNRT import CairnNRT

"""
def pytest_addoption(parser):
    parser.addoption("--all", action="store_true", help="run all combinations")
"""

def pytest_generate_tests(metafunc):
    idlist = []
    argvalues = []
    for scenario in metafunc.cls.scenarios:
        idlist.append(scenario[0])
        items = scenario[1].items()
        argnames = [x[0] for x in items]
        argvalues.append([x[1] for x in items])
    metafunc.parametrize(argnames, argvalues, ids=idlist, scope="class")
    print(argnames)
    print(argvalues)
    print(idlist)


#Read sampling to know which case to compare
app_home = path.dirname(path.realpath(__file__))

test_case="test_envimpacts"
df1 = pd.read_csv(os.path.join(app_home, "sampling.csv"), sep=";", decimal=".", index_col=0)
run_sensitivity_echantillonnage_manuel(test_case,app_home,"sampling.csv",1000,True,"", [test_case+"_dataseries.csv"])
scenarios1=[]
for subcase in df1.index.tolist():
    scenarios1.append((test_case, {"test_case": test_case, "subcase": subcase}))

test_case="test_envimpactsconstraint"
df2 = pd.read_csv(os.path.join(app_home, "sampling_constraints.csv"), sep=";", decimal=".", index_col=0)
run_sensitivity_echantillonnage_manuel(test_case,app_home,"sampling_constraints.csv",1000,True,"", [test_case+"_dataseries.csv"])
scenarios2=[]
for subcase in df2.index.tolist():
    scenarios2.append((test_case, {"test_case": test_case, "subcase": subcase}))

class TestSampleWithScenarios:
    scenarios = scenarios1 + scenarios2
    
    @pytest.mark.Cairn
    def test_check_results(self, test_case, subcase):
        app_home = path.dirname(path.realpath(__file__))
        tnr = CairnNRT(app_home)
        
        print(test_case)
        print(subcase)
        
        tnr.check("", "Report_s"+subcase+"/"+test_case+"_"+subcase+"_Results.csv", test_case+"_"+subcase+"_Results_Ref.csv")
        tnr.checkGlobal(typeFile='PLAN', fileNew="Report_s"+subcase+"/"+test_case+"_"+subcase+"_Results_PLAN.csv",fileRef=test_case+"_"+subcase+"_results_PLAN_Ref.csv")
    
