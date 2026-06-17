"""Create a Python FMI2-like interface for Cairn, using PythonFMU."""
import tempfile
from typing import Optional, Dict, List

from pathlib import Path
import os
import numpy as np
import pandas as pd
import json

from pythonfmu import Fmi2Slave, Fmi2Causality, Real, Fmi2Variability, DefaultExperiment, FmuBuilder

import cairn
from cairn import CairnAPI, OptimProblem

class ListElement(Real):
    def __init__(self, name: str, idx: int, **kwargs):
        super().__init__(name, **kwargs)
        self._index_in_list = idx

class FMI2Interface(Fmi2Slave):
    def __init__(self, prj_file: str, **kwargs):
        super().__init__(**kwargs)

        #assert model.check_time_range_definition(), "The time range of the model needs to be defined."
        self._resources = self.resources
        config_path = os.path.join(self._resources, "config.json")
        with open(config_path) as f:
            config = json.load(f)
        self._future_only = config["future_only"]
        self._level = config["level"]
        self._log_console = config["log_console"]
        self._log_file = config["log_file"]

        self._instance = CairnAPI(
            {"level": self._level, "flushlevel": self._level, "console": self._log_console, "file": self._log_file,
             "path": "study.log", "auxfile": False, "auxpath": ""})

        study_path = os.path.join(self._resources, "cairn_study.json")
        self._model = self._instance.read_study(study_path)

        #self._solve_parameters = kwargs["solve_parameters"] if "solve_parameters" in kwargs else {}

        simulation_control = self._model.get_simulation_control()

        self._dt = simulation_control.get_setting_value("TimeStep")
        self._past_size = simulation_control.get_setting_value("PastSize")
        self._timeshift = simulation_control.get_setting_value("TimeShift")
        self._future_size = simulation_control.get_setting_value("FutureSize")
        self._cycle = simulation_control.get_setting_value("NbCycle")

        self.default_experiment = DefaultExperiment(start_time=0.,
                                                    stop_time=self._timeshift*self._cycle*self._dt,
                                                    step_size=self._dt*self._timeshift  # FMU step size
                                                    )

        # Register variables
        self._register_variables()
        # For each Cairn variable, store the list of value_references corresponding to the FMU variables
        self._value_references = {}
        self._input_variables = []  # List of Cairn input variables
        self._output_variables = []
        for idx in self.vars:
            var = self.vars[idx]
            varname = var.name.split("[")[0]
            if varname not in self._value_references.keys():
                self._value_references[varname] = [var.value_reference]
            else:
                self._value_references[varname] += [var.value_reference]
            if var.causality == Fmi2Causality.input and varname not in self._input_variables:
                self._input_variables += [varname]
            if var.causality == Fmi2Causality.output and varname not in self._output_variables:
                self._output_variables += [varname]

        # self.inputs_defined_by_master = False
        self.inputs_defined_by_master = True

    def setup_experiment(self, start_time: float, stop_time: float, tolerance : float):
        super().setup_experiment(start_time, stop_time, tolerance)
        self.time = start_time

    def enter_initialization_mode(self):
        return True

    def do_step(self, current_time, step_size):
        #Gestion des inputs
        if self.inputs_defined_by_master:
            print("-------------- self._input_variables:")
            print(self._input_variables)
            for varname in self._input_variables:
                values = self.get_real(self._value_references[varname])
                # Inject the value to _model
                self.set_input_values(varname, values)
                print("-------------- set_subscribed_variable_value ")
                print(varname)
                print(values)
                self._model.set_subscribed_variable_value(varname, values)

        #Run du modèle
        solution = self._model.run(coSim=True)
        print(solution.status)

        #Récupération des outputs
        for varname in self._output_variables:
            #assert len(self._output_variables[varname]) == self._future_size + 1
            values = self._model.get_published_variable_value(varname)
            print(values)
            self.set_real(self._value_references[varname][0:], values[-1*self._future_size:])

        return True

    def terminate(self):
        self._instance.close_study()
        return True

    def _register_variables(self):
        for input_name in self._model.get_subscribed_variables():
            self._register_list(f"{input_name}", Fmi2Causality.input)
        for output_name in self._model.get_published_variables():
            self._register_list(f"{output_name}", Fmi2Causality.output)

    def _register_list(self,
                       variable_name: str,
                       causality: Fmi2Causality,
                       variability: Optional[Fmi2Variability] = None):

        def update_array(array, idx, value):
            array[idx] = value

        total_size = self._past_size + self._future_size
        if self._future_only:
            total_size = self._future_size
        # Define attribute
        setattr(self, variable_name, np.zeros(total_size))

        def getter(owner, variable_name: str, var: ListElement):
            return lambda: getattr(owner, variable_name)[var._index_in_list]

        def setter(owner, variable_name, var: ListElement):
            return lambda v: update_array(getattr(owner, variable_name), var._index_in_list, v)

        # Register variables
        for i in range(total_size):
            v_name = f"{variable_name}[{i}]"
            var = ListElement(name=v_name, idx=i, causality=causality, variability=variability)
            # Getter function
            var.getter = getter(self, variable_name, var)
            var.setter = setter(self, variable_name, var)

            self.register_variable(var, nested=True)

    def set_input_values(self, variable_name: str, values: List[float]):

        vr = self._value_references[variable_name]
        if self.inputs_defined_by_master:
            self.set_real(vr, values)
            # self.inputs_defined_by_master = False