.. _cairn_files:

What are the input and output files of a cairn simulation?
-----------------------------------------------------------

.. raw:: html

   <style> 
    table.table1 th {background-color: gray ; }
    table.table1 td:nth-child(1) {background-color: lightgray}
    table.table1 tr:nth-child(even) { background-color: lightgray}
   </style>

.. csv-table:: |cairn| timeseries file
	:file: What_are_the_input_and_output_files_of_a_cairn_simulation.csv
	:header-rows: 1
	:delim: ;
	:widths: 10 10 50
	:name: input_and_output_files
	:align: center
	:class: table1

Running a calculation necessarily requires a .json file and a time series file.

Depending on the selected solver, a .lp can be generated if the parameter "WriteLp" is set to "YES".

How to use the log files?
~~~~~~~~~~~~~~~~~~~~~~~~~

Three log files are available.

* The {myproject}.log file gathers all the information processed when using the HMI.

  INFO, WARNING and CRITICAL messages can be retrieved in this log file.

* The {myproject}_optim.log gathers all the information given by the solver during the resolution of the MILP problem.

  It can be used to follow the status of the resolution.

* The {myproject}_cplex.log contains a summary of the solution found by |cplex|.

  The file is present only if |cplex| solver is used.

How to use the result files?
~~~~~~~~~~~~~~~~~~~~~~~~~~~~

* {myproject}_results_PLAN.csv file gathers all the computed indicators for the last optimization (cycle).

  Indicators are predefined but through the HMI, it is possible to build new indicators that will be exported both in PLAN and HIST files.

  .. seealso:: 
	:ref:`cairn_UDI`
  
  
  .. todo:: add info

    Levelization factor, extrapolation factor

* {myproject}_results_HIST.csv file gathers all the cumulated computed indicators for the all optimizations (cycles).

  .. todo:: add info

    Levelization factor, extrapolation factor

|

.. caution:: 
	- All |csv| |cairn| files are with the semicolumn (";") separator.
	- The SAVE file can be used to restart a calculation 

.. seealso:: 
	#. :ref:`cairn_timeseriesfile`
