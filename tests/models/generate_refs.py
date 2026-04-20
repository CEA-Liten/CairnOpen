import json
import pytest
from os import path
import os
import sys
sys.path.append(os.path.join(os.path.dirname(__file__), '..', 'scripts'))
from pathlib import Path
import CairnNRT as CNRT
import pandas as pd
import test_open_models as tom

BASE_DIR = Path(path.dirname(path.realpath(__file__)))

test_cases = tom.find_all_tests(BASE_DIR)

overwrite = True

def init_study(name_study, app_home):
    tnr = CNRT.CairnNRT(name=name_study,app_home=app_home)
    tnr.check_study_file_existence()
    tnr.updateTest(overwrite,False)

init = False
status_list = dict()
print(test_cases)
#tc= test_cases[-1]
#tom.init_study(tc[0],tc[1])
for tc in test_cases:
    init_study(tc[0],tc[1])