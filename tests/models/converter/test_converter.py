import os
from os import path
import pytest
import sys

from CairnNRT import CairnNRT
from PerseeExtractResult import *

@pytest.mark.Cairn
def test_runTNR_Json():
    app_home = path.dirname(path.realpath(__file__))
    tnr = CairnNRT(app_home)

    test_case="test_converter"

    tnr.runPersee(tnr_dir=r".", tnr_xml=test_case+".json", tnr_series=test_case+"_dataseries.csv", tnr_name=test_case)

    tnr.checkGlobal(typeFile='PLAN', fileNew=test_case+"_results_PLAN.csv",fileRef=test_case+"_results_PLAN_Ref.csv")
    tnr.check("", test_case+"_Results_Ref.csv",test_case+"_Results.csv")

if __name__ == '__main__':
    test_runTNR_Json()
