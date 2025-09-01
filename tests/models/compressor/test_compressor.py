from os import path
import pytest
from CairnNRT import CairnNRT

@pytest.mark.Cairn
def test_check_results_ts(test_case, subcase):
    app_home = path.dirname(path.realpath(__file__))
    tnr = CairnNRT(app_home)
    
    tnr.check("", "Report_s"+subcase+"/"+test_case+"_results_Results.csv", test_case+"_"+subcase+"_Results_Ref.csv")