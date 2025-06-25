import os
from os import path
import pytest
import sys
import pandas as pd
from CairnNRT import CairnNRT
from PerseeSensParam import run_sensitivity_echantillonnage_manuel

@pytest.mark.Cairn
def test_check_results_ts(test_case,subcase):
    app_home = path.dirname(path.realpath(__file__))
    tnr = CairnNRT(app_home)
    
    tnr.check("", "Report_s"+subcase+"/"+test_case+"_"+subcase+"_Results.csv", test_case+"_"+subcase+"_Results_Ref.csv")