.. attention:: 

	Work in progress

Cairn Open SubModels
====================

Principles
----------

Models are libraries represent elements that are commonly used in energy systems. They add to the global MILP problem the following items:

- variables representing fluxes, states of the system, costs...
- constraints linking these variables together
- a constribution to objectives functions.

A model added to the system has to be connected to other components through ports, that link some of its variables to variables of other models.

Each model can be parametrized with static and time-dependant parameters, that make them versatile. The following part of the documentation describes precisely each model.

If none of the models corresponds to your need, it is possible to add your own model to cairn. See the ref:`develop_own_model`.

Submodels summary
-----------------

Models library is divided into 2 categories in cairn:

- the **technical models**: they represent physical technologies that will be added to the energy system. They are subdivided into 4 categories:
   - the **converters** are used to convert energy vectors to others.
   - the **storage** are used to store energy vector for some time and redistribute it later.
   - the **Source Loads** allow to impose a time series profiles to represent either a imposed production or a consumption.
   - the **Grids** represent a degree of liberty of the system: it is possible either to import or export an energy vector with the outside of the system.
- the **operation constraint models**: they allow to add constraints to components, either due to **physical phenomenons**, either due to **rules of functionning** to components. The **buses** allow to write constraints between components such as the node law (Kirchhoff).


Submodels documentation description
-----------------------------------

The description is made of 3 main parts:

- Model Parameters : it describes which parameters must be set by the user via the .ini settings file. When specified, some of them will be automatically filled in by cairn.

- Model Interface Expression : it describes the model linear expression published by the model to enable the user to 
impose combined constraints by connecting ports using these variable names, to bus.

- Model equations : it gathers the linear equations used to write linear constraints and objective contribution.

Units
-----

The Interface Expression also specifies a type of unit:

- the first field gives the possible energy carrier types, namely Electrical, Thermal, Fluid or Material

- and the second field indicates the type of unit, whether it is relative to energy Flux, or to Capacity storage, or Currency for economic data.

The effective unit will be built from these data and from the units specified in each associated EnergyVector components.
Sometimes, the units are indicated explicitly, meaning that the model only supports this particular one.
Be aware that parameters set in settings file must be consistent with the units defined in the EnergyVector component.
Also notice that using cairn self defined physical propeties like Low Heat Values of fluids, imposes to use the same units in the EnergyVectorcomponent. To do it another way, please define the fluid physical properties again consistently with the unit system chosen in your particular EnergyVector.




Additional guidelines and examples
--------------------------------------
.. toctree::
   :maxdepth: 2
   :caption: Component guidelines
    
   ../cairn_open_user_guide_lines/how_to_model_multiconverter
   ../privateDoc/cairn_user_guide_lines/howToModel/how_to_model_a_grid
   ../privateDoc/cairn_user_guide_lines/howToModel/how_to_model_a_source_load
   ../privateDoc/cairn_user_guide_lines/howToModel/how_to_model_an_energy_storage
   ../privateDoc/cairn_user_guide_lines/howToModel/how_to_model_an_electrolyzer
   ../privateDoc/cairn_user_guide_lines/howToModel/how_to_model_a_heat_pump
   ../privateDoc/cairn_user_guide_lines/howToModel/how_to_model_a_thermal_group
   ../privateDoc/cairn_user_guide_lines/howToModel/how_to_model_auxiliaries
   ../privateDoc/cairn_user_guide_lines/howToModel/how_to_model_transportation

