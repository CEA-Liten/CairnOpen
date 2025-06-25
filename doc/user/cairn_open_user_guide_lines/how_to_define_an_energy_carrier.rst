.. _SetEnergyVector:

How to define an energy carrier? 
--------------------------------

How to set parameters and options of an energy carrier? 
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

An energy carrier is defined using the **EnergyVector class**. 
The EnergyVector class allows the definition of several parameters :

-  Set main type if not predefined : Electrical, Thermal or Material.

-  Set or adjust main units : Mass, Flowrate, Energy, Power.

	.. seealso:: 

		a. :ref:`Setunits`.


-  Set or adjust main features : heatcarrier, fuelcarrier.

-  Set or adjust sell/buy price.

    .. caution::
        
        - if sell or buy prices are added to grid or load components (using constant or timeserie profile); these values are not taken into account.

.. list-table:: How to define parameters of an energy carrier?   

	* - .. figure:: images/energy_carrier2.PNG
		   :scale: 60 
		   :name: energy_carrier2
		   :align: center

		   Using the |gui|

	  - .. 	literalinclude:: energy_carrier2.txt 
		   :caption: In the json file

.. list-table:: How to set options of an energy carrier?   

	* - .. figure:: images/energy_carrier.PNG
		   :scale: 60 
		   :name: energy_carrier1
		   :align: center

		   Using the |gui|

	  - .. literalinclude:: energy_carrier.txt 
		   :caption: In the json file


.. _Setunits:
		   
How to set my own units? 
~~~~~~~~~~~~~~~~~~~~~~~~

| Any component connected to one bus will inherit the units of the energy carrier <EnergyVector> it is carrying.
| To change the units, refer to the unit properties of the energy carrier (:ref:`SetEnergyVector`).

To date, it is up to the user to ensure unit consistency between input time series and cairn units. No checking is
performed by |cairn| on this matter.

.. caution::

	- Be aware that changing the default units **will NOT automatically** convert the default values for physical properties
	  (eg heat values and so on). 
	
	- Then the user should define these properties again if the energy carrier units are not the default ones.
