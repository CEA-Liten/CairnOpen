.. _cairn_principles:

Principles of the |cairn| tool
-------------------------------

Main characteristics of the |cairn| process
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The optimization problem consists of:

  - a set of constraints describing the technological, environmental, 
    economic features and behaviour of the system components,

  - a set of linked constraints between several components (ensuring
    mass or energy balance for instance) and time or integrated constraints for the system,

  - a set of global parameters for the system (basic, economic and simulation data),

  - the establishment of an objective function.

This is performed by assembling predefined components available in a model library written in C++ (or |gams|, on-going implementation).

The description of this assembly consists of the following parts:

  1.  The description part of the topology describing the components and their connections, and associated settings.
      The architecture file (using the |json| syntax) contains the topologies of the system and also the parameters used for the calculation, 
      the optimization and the simulation.
  #.  The time series defining boundary conditions, using the |csv| format.

  #.  The performance maps defining potential maps for component performances,
      using the |csv| format.

Methodology to define a |cairn| problem and simulation
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

1.  **Define the architecture** of the optimization problem:
          
    a.  In order to build your optimization problem for an energy system, first identify the **relevant energy carriers** 
        and define them in the json architecture file.

        .. caution::
        
          - Be aware that different voltages, temperatures or pressures for a same material should be considered as 
            different energy carriers to allow the modelling of converter from one voltage to another, from one temperature 
            to another of one pressure to another.
          - Passing from one energy carrier to another always require an energy converter. 
          - For example, a compressor converter to pass from a low pressure to an high pressure hydrogen. In this
            case, the pressure values are given by the pressure input in the hydrogen carriers across the compressor.
		
        .. seealso:: 

			- :ref:`SetEnergyVector`
			
    #.  Then identify **the technological components** and their contribution to mass and energy balance on each energy carriers. 
        These components can be of 4 main types (converters, storages, source and load, and grid).
        Each component adds its own internal constraints using the model that is defined by the user.  
    #.  Link these components **through their ports to nodes** (or bus components, using FlowBalance models), to enable mass and energy 
        balances checks thanks to dedicated constraints. 
#.  **Fill in the settings for each model** added to the architecture. This data will be read before each optimization solving step.  
#.  **Define the input and output time series** in csv file, if any. The input and output time series are used to model loads,
    powers, flowrates, prices used by the technological components and the system. They are given in function of time.	
	
	.. seealso::
	
		- :ref:`cairn_timeseriesfile`
		
#.  **Define the performance maps** in csv file, if any. Last, add some performance map in several csv files to define some additional 
    performance features. This is optional, depending on the model's fitness.
	
	.. seealso:: 
	
		- :ref:`cairn_map_file`

#.  **Define the main characteristics** of the |cairn| problem. The main characteristics of the |cairn| problem are:
    - the **economic and environemental parameters** of the system, 
    - the **objective function**, the **total time and time step calculation** 
    - the **solver management**.
	
	.. todo:: 
	
		- How to define the economic parameters of the problem?
		- :ref:`cairn_environmental`
		- How to define the objective function of the problem?
		- How to define the total calculation time and the time step management?
		- How to define the solver management?
		
#.  **Run** of the |cairn| problem:	

	.. seealso:: 
		- :ref:`running_mode`
		
#.  **Analyse the results** of the |cairn| problem:

	.. seealso:: 
		- :ref:`cairn_files`

#.  **Perform sensitivity analysis**:

	.. todo:: 
		- How to perform sensitivity analysis?
    



