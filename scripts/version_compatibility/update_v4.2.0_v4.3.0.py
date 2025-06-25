# -*- coding: utf-8 -*-
"""
Created on Thu Feb 27 10:37:09 2025

@author: sc258201
"""


import pytest
from persee import *
import os
import shutil
from os import path
import pandas as pd

json_file = "D:/Users\SC258201\PerseeGui\lib\export\Persee/tests\data\compressor\compressor.json"

persee_instance = CairnAPI(True)
problem = persee_instance.read_study(json_file)

#Modif IO of compressor
compressors=problem.get_components("Compressor")
for compressor in compressors:
    comp=problem.get_component("Compressor")
    ports=comp.ports
    #find input mass flow rate and output mass flow rate to replace IO name
    for str_port in ports:
        port=comp.get_port(str_port)
        if (port.get_setting_value('Direction') == 'INPUT' and port.get_setting_value('Variable') == 'MassFlowRate'):
            port.set_setting_value('Variable', 'InMassFlowRate')
        if (port.get_setting_value('Direction') == 'OUTPUT' and port.get_setting_value('Variable') == 'MassFlowRate'):
            port.set_setting_value('Variable', 'OutMassFlowRate')

problem.save_study(json_file)