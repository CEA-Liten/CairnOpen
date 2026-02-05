# -*- coding: utf-8 -*-
import pytest
from cairn import *
import os
import shutil
from os import path
import pandas as pd
import filecmp
import json 

from cairn import CairnAPI, EnergyVector, Component, Port, Bus, Solution



def addGridFree(problem):
    #Create a new componenet
    myGrid = Component(problem, "My_Grid", "GridFree")

    #Configure default ports
    elec_carrier = problem.get_energy_carrier("ElectricityDistrib")
    defaultPorts = myGrid.default_ports
    myDefaultPort = myGrid.get_port(defaultPorts[0]) #GridFree has only one default port
    myDefaultPort.set_carrier(elec_carrier) 

    #Add link
    elec_bus = problem.get_bus("Elec_Bus")
    problem.add_link(myDefaultPort, elec_bus)

@pytest.mark.Cairn
@pytest.mark.PythonAPI
def test_save_auto_positions_json():
    studyDir = path.dirname(path.realpath(__file__))
    studyName =  path.join(studyDir, './data/cairn_training.json')   
    cairn_instance = CairnAPI(True)

    #Read study
    problem = cairn_instance.read_study(studyName)

    addGridFree(problem)

    #Save Study
    problem.save_study(path.join(studyDir, './results/new_study.json'))

    #Close Study
    cairn_instance.close_study()
        
    problem2 = cairn_instance.read_study(path.join(studyDir, './results/new_study.json'))

    cairn_instance_ref = CairnAPI(True)
    problem_ref = cairn_instance_ref.read_study(path.join(studyDir, './refs/new_study_ref.json'))    

    assert problem_ref.get_component("My_Grid").get_setting_value("Xpos") == problem2.get_component("My_Grid").get_setting_value("Xpos")
    assert problem_ref.get_component("My_Grid").get_setting_value("Ypos") == problem2.get_component("My_Grid").get_setting_value("Ypos")

    cairn_instance.close_study()
    cairn_instance_ref.close_study()


    #---------------------------------------------------------------------------------------------#

@pytest.mark.Cairn
@pytest.mark.PythonAPI
def test_save_grad_positions_json():
    studyDir = path.dirname(path.realpath(__file__))
    studyName =  path.join(studyDir, './data/cairn_training.json')   
    cairn_instance = CairnAPI(True)

    #Read study
    problem = cairn_instance.read_study(studyName)

    addGridFree(problem)

    #Save Study - Gradient
    problem.save_study(path.join(studyDir, './results/new_study_gradient.json'), "gradient")

    #Close Study
    cairn_instance.close_study()

    problem2 = cairn_instance.read_study(path.join(studyDir, './results/new_study_gradient.json'))

    cairn_instance_ref = CairnAPI(True)
    problem_ref = cairn_instance_ref.read_study(path.join(studyDir, './refs/new_study_gradient_ref.json'))

    assert problem_ref.get_component("My_Grid").get_setting_value("Xpos") == problem2.get_component("My_Grid").get_setting_value("Xpos")
    assert problem_ref.get_component("My_Grid").get_setting_value("Ypos") == problem2.get_component("My_Grid").get_setting_value("Ypos")

    cairn_instance.close_study()
    cairn_instance_ref.close_study()


if __name__ == '__main__':
    test_save_auto_positions_json()
    test_save_grad_positions_json()



