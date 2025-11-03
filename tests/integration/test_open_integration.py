import json
from numpy import record
import pytest
from os import path
from pathlib import Path
import CairnNRT as CNRT
import pandas as pd
import csv


BASE_DIR = Path(path.dirname(path.realpath(__file__)))
file_report = path.join(BASE_DIR,"rapport_modeles.csv")
with open(file_report, mode='w', newline='') as file:
    writer = csv.writer(file,delimiter=';')
    writer.writerow(["name_study","PLAN","HIST","TIMESERIES","RUNLPFILE","LPFILE","SAMPLING"])
    
# Récupère tous les fichiers .csv dans les sous-dossiers de "data"
# Ne récupère pas les sensibilités

def find_all_tests(BASE_DIR):
    test_cases = []
    for case_dir in BASE_DIR.iterdir():
        if case_dir.is_dir():
            jsons = list(case_dir.glob("*.json"))
            jsons = [
                j for j in jsons
                if "_ref.json" not in j.name and j.name != "settings.json" and "Report_s" not in str(j)
            ]
            try:
                sampling = list(case_dir.glob("sampling.csv"))[0]
            except:
                sampling=""
            print(len(jsons))
            #assert (len(jsons) <= 1), "Problem in "+case_dir.name+" : folder should contain one study .json and list is " + str([str(j) for j in jsons])
            for i in range(len(jsons)):
                test_cases.append([jsons[i].stem,jsons[i].parent,sampling])
    return(test_cases)



test_cases = find_all_tests(BASE_DIR)
@pytest.mark.parametrize(
    "name_study, app_home, sampling",
    test_cases,
    ids=[p[0] +"."+ p[1].name for p in test_cases]  # shorter ids
)


@pytest.mark.Cairn
def test_generic(name_study,app_home,sampling, main=False):
    tnr = CNRT.CairnNRT(name=name_study,app_home=app_home,sampling=sampling)
    tnr.check_study_file_existence()
    status = tnr.generic_testing()
    status["SAMPLING"] = "NA"
    if sampling != "":
        sampling_status = tnr.sampling_test()
        status["SAMPLING"] = sampling_status
    if not ((status["PLAN"] == True) and (status["TIMESERIES"] == True)):
            print("Test " + name_study + " failed")
    print("status: " + str(status))
    with open(file_report, mode='a', newline='') as file:
        writer = csv.writer(file,delimiter=';')
        writer.writerow([name_study,status["PLAN"],status["HIST"],status["TIMESERIES"],status["RUNLPFILE"],status["LPFILE"],status["SAMPLING"]])
    if (__name__=='__main__')==False:
        assert (status["PLAN"] == True)
        assert (status["HIST"] == True) 
        assert (status["SAMPLING"]=="Success" or status["SAMPLING"]=="NA")
    return status


def init_study(name_study, app_home):
    tnr = CNRT.CairnNRT(name=name_study,app_home=app_home)
    tnr.check_study_file_existence()
    tnr.updateTest(False,False)

def generate_report(record_property, main=False):
    status_list = dict()
    for tc in test_cases:
        #init_study(tc[0],tc[1])
        status_list[tc[0] +"."+ tc[1].name]=test_generic(tc[0],tc[1],tc[2], main=True)


if __name__ == '__main__':
    init = False
    def record_property1(a,b):
        return
    generate_report(record_property1, main=True)