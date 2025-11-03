import json
import pytest
from os import path
from pathlib import Path
import CairnNRT as CNRT
import pandas as pd
import test_open_models as tom

BASE_DIR = Path(path.dirname(path.realpath(__file__)))

test_cases = tom.find_all_tests(BASE_DIR)

init = False
status_list = dict()
for tc in test_cases:
    tom.init_study(tc[0],tc[1])