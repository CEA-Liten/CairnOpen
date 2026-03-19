|cairn| Tool
----------------------

.. image:: images/cairn.png
  :width: 300
  :alt: cairn Logo
  :align: center
  
|

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

.. toctree:: 
   :maxdepth: 2
   :caption: Cairn User Guide Lines
   
   cairn_open_user_guide_lines/general
   cairn_open_user_guide_lines/time_management/time_management
   cairn_open_user_guide_lines/input_output_files
   cairn_open_user_guide_lines/economic_aspects
   cairn_open_user_guide_lines/environmental_aspects
   cairn_open_user_guide_lines/mixed_integer_linear_optimization
   privateDoc/advanced_features/modelling_guidelines
   privateDoc/advanced_features/how_to_perform
   privateDoc/debug


    

.. toctree::
   :maxdepth: 2
   :caption: Cairn Viewer Guide 
   
   hmi_guide/build_a_problem
   hmi_guide/display_results
   hmi_guide/shortcuts_and_options 
   hmi_guide/How_to_build_a_UDI
   hmi_guide/postprocessing
  


.. toctree::
    :maxdepth: 2
    :caption: API Guide

    api_guide/quick_api
    api_guide/commands
    api_guide/notebook


.. toctree::
    :maxdepth: 2
    :caption: Components description
    
    models/models
    models/grids
    models/bus
    models/converters
    models/sourceloads
    models/storages
    models/operationconstraints
    models/physicalequation

.. toctree::
    :maxdepth: 2
    :caption: Guidelines and examples

    guidelines_examples/converter_advanced_use
	


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