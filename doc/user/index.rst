|cairn| Tool
----------------------

.. image:: images/cairn.png
  :width: 300
  :alt: cairn Logo
  :align: center
  
|

  
.. container:: center-text 


.. admonition:: What is |cairn|?

	- |cairn| is an optimizer tool for energy system studies. It allows to perform techno-economic and environmental analysis of energy systems 
	  by optimization of both the sizing of power systems and the operation planning.
	- |cairn| is based on the Mixed Integer Linear Progamming formalism (|milp|).
	- |cairn| has been build to allow for several purposes :
	
		- allow to build any architecture of multi-energy system with a verified library of components.
		- allow non-specialists to build easily optimisation problems, and analyze the results.

.. important::

	- This guide gathers all the usefull information for users of |cairn|.
	- For more information, developpers can contact :ref:`contactus`.

.. toctree::
   :maxdepth: 2
   :caption: Start with Cairn

   about_cairn/what_is_cairn
   about_cairn/installation
   about_cairn/running_modes_of_cairn

.. toctree::
   :maxdepth: 2
   :caption: Theoretical elements

   theoretical_elements/principles_cairn.rst
   theoretical_elements/optimization_vs_simulation
   theoretical_elements/milp
   theoretical_elements/investment_decision
   theoretical_elements/LCA

.. ifconfig:: cea_content

   .. toctree::
      :maxdepth: 2
      :caption: Cairn Solus User Guide Lines

      privateDoc/cairn_user_guide_lines/how_to_model
      privateDoc/cairn_user_guide_lines/how_to_perform

.. toctree::
   :maxdepth: 2
   :caption: Cairn Open User Guide Lines

   cairn_open_user_guide_lines/how_to_define_an_energy_carrier
   cairn_open_user_guide_lines/how_to_make_a_cairn_timeseries_file
   cairn_open_user_guide_lines/how_to_make_a_performance_map_file
   cairn_open_user_guide_lines/What_are_the_input_and_output_files_of_a_cairn_simulation
   cairn_open_user_guide_lines/how_to_take_into_account_economic_aspects
   cairn_open_user_guide_lines/how_to_take_into_account_environmental_aspects
   cairn_open_user_guide_lines/What_are_the_input_and_output_files_of_a_cairn_simulation
   cairn_open_user_guide_lines/rolling_horizon

   
.. toctree::
   :maxdepth: 2
   :caption: GUI Guide 
   
   hmi_guide/build_a_problem
   hmi_guide/display_results
   hmi_guide/shortcuts_and_options 
   hmi_guide/How_to_take_into_account_the_environmental_impacts
   hmi_guide/How_to_build_a_UDI
   hmi_guide/postprocessing
  


.. toctree::
    :maxdepth: 2
    :caption: API Guide

    api_guide/commands

.. toctree::
    :maxdepth: 2
    :caption: Components detailed description
    
    models/models
    privateDoc/models/modelSolus_toc

.. toctree::
    :maxdepth: 2
    :caption: Cairn Solus components detailed description
    
    privateDoc/models

.. toctree::
    :maxdepth: 2
    :caption: Guidelines and examples

    guidelines_examples/simple_case
    guidelines_examples/rolling_horizon
    guidelines_examples/performance_maps
    guidelines_examples/time_agregation
    guidelines_examples/converter_advanced_use
	
.. toctree:: 
   :maxdepth: 2
   :caption: Advanced features

   privateDoc/advanced_features/pegase_cairn
   privateDoc/advanced_features/develop_model_with_installer
   privateDoc/advanced_features/cplex
   privateDoc/advanced_features/better_write_milp_pb
   privateDoc/advanced_features/cairn_Uranie
   privateDoc/advanced_features/Transport_network
   privateDoc/advanced_features/investment_phases

.. toctree::
   :maxdepth: 2
   :caption: Contact us

   about_cairn/contact_us

.. toctree::
    :maxdepth: 2
    :caption: FAQ-Glossary-Ref.

    FAQ/FAQ 
    glossary 
    references 

.. toctree::
   :hidden:

   genindex
	
.. raw:: latex

  \listoffigures
  \listoftables