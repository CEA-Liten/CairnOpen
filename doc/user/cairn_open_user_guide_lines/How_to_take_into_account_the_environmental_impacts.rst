.. _cairn_environmental:

Environmental impacts
===================================================


Environmental impact calculation are activated in the |cairn| "TecEcoAnalysis" component. 
Environmental impacts have to be defined for each component.


Environmental impact are defined in |cairn| through the "TecEcoAnalysis" component in the parameters tab.
It is possible to consider several impacts (list based on EF V3.0, :numref:`lca_parameters`).

.. container:: cadre

    .. list-table:: How to define parameters of the environmental model?
       :widths: 50 50
    
       * - .. figure:: images/lca_parameters.PNG
			:name: lca_parameters
			:align: center
			:width: 400
			   
			In the "Parameters Tab" in the "TecEcoAnalysis" component
         - .. code-block:: python 
            :caption: Using the |python| |api|
            
			problem.tech_eco_analysis = {'ConsideredEnvironmentalImpacts':
			'Climate change#Global Warming Potential 100,
			'Land use#Soil quality index' }
			   
.. container:: cadre

    .. list-table:: How to enable environmental impact calculation for a component?
        :widths: 50 50	
    
        * - .. figure:: images/lca_parameters_compo.PNG
             :name: lca_parameters_compo
             :align: center
             :width: 400
        
        	 In the "Parameters Tab of the component"
          - .. code-block:: python 
             :caption: Using the |python| |api|
        	
             my_elz.set_setting_value("EnvironmentModel", True) 

.. seealso:: 

	- :ref:`lca_intro`
	- :ref:`lca_ef`

For each component:

	- Indicators are calculated only for a component itself.
	- The environmental model (EnvironmentModel, :numref:`lca_parameters_compo`) has to be activated.
	- The tabs (Env. Impacts and Ports) in which environmental impacts can be configured are colored green.
	  It is possible to set for each impact :
	  
		-  **Grey or manufacturing impact** linked to the size (in tab "Environmental impacts" of a component) owing to coeffs A and B
		   (see :numref:`lca_parameters_compo_grey`) : :math:`m = A * size + B`
		   
		   In case of replacement modelling, grey impacts can be added using the "EnvGreyReplacement" (A coeff) and 
		   "EnvGreyReplacementConstant" (B coeff)parameters.
		   
		-  **Direct or consumption impact** linked to flux (can be selected by the user in tab "Ports") owing to coeffs A and B
		   (see :numref:`lca_parameters_compo_direct`): :math:`m = A * flux + B`.
		   
		   The A coefficient can be also set using a timeseries ("UseEnvContentTS_A"=True, the timeseries is given in the "EnvContentTS_A" field).

.. container:: cadre

	.. list-table:: How to define impacts for a component?

		* - .. figure:: images/lca_parameters_compo_grey.PNG
			   :name: lca_parameters_compo_grey
			   :align: center
			   :width: 1000

			   Grey impacts

		  - .. figure:: images/lca_parameters_compo_direct.PNG
			   :name: lca_parameters_compo_direct
			   :align: center
			   :width: 600

			   Direct impacts

.. hint:: 

	1. It is possible to take into account in the economic objective function by tax penalty if the impact cost of the "TecEcoAnalysis" component
	   (green tab Env. Impacts) is given a non zero (see :numref:`lca_parameters_cost` where :math:`50\; €/kg\; CO2` is set).
	   In this case, the component "EcoInvestModel" has to be enabled (see :numref:`lca_parameters_compo`).
	
		.. container:: cadre

			.. figure:: images/lca_parameters_cost.PNG
			   :name: lca_parameters_cost
			   :align: center
			   :width: 800
			   
			   Impose impact cost and constraints in the "TecEcoAnalysis" component

	#. It is possible to take into account **a levelized maximum constraint** (see :numref:`lca_parameters_cost` where the flag ""MaxContraint is set
	   at 1 with a levelized maximum value at :math:`10000\; kg\; CO2`)

.. important:: 

	The user should be very careful not to mix models for the same carbon sources:
	
	- It is correct to define environmental impact for an energy (electrical) or material carrier (fluid, hydrogen, CH4).
	  All converters connected to the carrier and the corresponding grid will be considered.
	  Then, environmental impact due to the vector is accounted at the grid level.
	  The use of this carrier on several converters is enseared without adding any other data
	  and **without knowing which converters is contributing the most**. 
	- It would be **wrong to both** define environmental impact **on a carrier and on each converter**. 
  	  In this situation, **Environmental impacts would then be accounted TWICE**.
	
	  