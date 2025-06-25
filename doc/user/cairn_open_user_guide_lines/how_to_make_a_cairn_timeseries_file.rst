.. _Set_cairn_timeseries:

.. _cairn_timeseriesfile:

How to make a |cairn| timeseries file?
---------------------------------------

.. raw:: html

   <style> 
    .. table.table1 th {background-color: lightgray ; col}
    .. table.table1 td:nth-child(1) {background-color: orange}
    .. table.table1 tr:nth-child(even) { background-color: yellow}
   </style>

   <style> 
    table.table1 th {background-color: #2d666e ; color: white}
   </style>

|

A |cairn| timeseries file gives all the input and output data used for the simulation and optimization process.

- **Possible input data** are :
	- the "SourceLoad" flux and environmental impacts,
	- the energy and materials buy and sell prices, 
	- the profile for converter use,
	- the profile for charge/load and discharge/unload for storages.
- **Possible output data** are the "SourceLoad" flux used for modelling the imposed loads.

A |cairn| timeseries file is a |csv| file ; the separator is the semicolumn (";") and is formatted 
as the following (see :numref:`exemple_dataseries`) :

- 4 compulsory header rows that give : 
	1. the name of the timeseries, the first column give the time timeseries in second,
	#. a comment, the time comment is the starting point date,
	#. the unit, the time unit is the second,
	#. a boolean used in case of |pegase| coupling.
- Others rows giving for each time the values of all parameters.

.. csv-table:: |cairn| timeseries file
	:file: exemple_dataseries.csv
	:header-rows: 4
	:delim: ;
	:widths: 5 5 5 5 
	:name: exemple_dataseries
	:align: center
	:class: table1

.. caution:: 

	- The time step of the |cairn| simulation is given by |cairn| component.
	- The |cairn| process will interpolate in the timeseries file to find the values at a given time.
	- If the simulation time is higher than the maximum time of the timeseries file, the 
	  |cairn| process will loop along the file.
