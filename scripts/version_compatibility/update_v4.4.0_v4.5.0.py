# -*- coding: utf-8 -*-
"""
Created on Thu Feb 27 10:37:09 2025

@author: sc258201
"""

from persee import *

json_file = "D:/Users\SC258201\GitPersee\PerseeGui\lib/export/Persee/Test_Persee/formation_persee/formation_persee.json"

persee_instance = CairnAPI(False)
problem = persee_instance.read_study(json_file)

#Modif NomPower into MaxPower
elz_names=problem.get_components("ElectrolyzerDetailed")
print(elz_names)
for elz_name in elz_names:
    elz=problem.get_component(elz_name)
    #find setting for NomPower and put it to MaxPower
    nom_pow = elz.get_setting_value("NomPower")
    elz.set_setting_value("MaxPower", nom_pow)
    
elz_names=problem.get_components("Electrolyzer")
print(elz_names)
for elz_name in elz_names:
    elz=problem.get_component(elz_name)
    #find setting for NomPower and put it to MaxPower
    nom_pow = elz.get_setting_value("NomPower")
    elz.set_setting_value("MaxPower", nom_pow)

problem.save_study(json_file)