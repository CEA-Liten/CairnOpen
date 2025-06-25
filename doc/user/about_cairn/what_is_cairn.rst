#############################
Overview of the |cairn| tool
#############################

|cairn| principles 
===================

CEA has been developing an optimization software called |cairn| to conduct techno-economic and environmental studies 
that help decision making when looking for optimal energy mixes at several scales of the **energy system**, from industrial premises 
to territories or countries. 

|

.. admonition:: What is an energy system?

	- An **energy system** is a system designed to supply **energy-services to end-users**.
	- An **energy system** consists of **inlet primary energies** (fossil, renewables,...), **conversion** of primary energies to intermediate ones
	  and **outlet or final energies** to **end-users**.
	- An **energy system** is constituted by all components related to the **production, conversion, storage, delivery, and use of energy**.
	- An **energy system** accounts for all technical, environmental, economic and social aspects.

The |cairn| tool allows in a modular way to define and solve a **techno-economic problem** including environmental aspects by assembling 
several Mixed Integer Linear Programming (|milp|) contributions of a dedicated C++ core library by modelling the main technological components 
of multi-energy systems.

|cairn| is mainly used to solve the optimization problems of so-called **unit-commitment problems** with **size optimization**, 
trying to find the **optimal energy mix** for the energy system submitted to one or several time load profiles. 

Specific constraints between components and their variables can be defined without any code development and through 
**smart connectors**.

Solving the **problem** is performed thanks to an Application Programming Interface (|api|) of various Open Source or 
commercial mixed integer linear solvers. 

.. caution:: 

	- |cairn| is not a solver.
	- |cairn| does not supply solvers.
	- Solver has to be installed by users.

The user is able to build the **energy system** either through a C++ Application Programming Interface (|api|) 
or through a Graphical User Interface (|gui|). 

Modelling principles of |cairn| 
================================

A multi-energy system (:ref:`energysystem`) is modelled by an set of technological components exchanging flows 
of different **energy carriers**, and/or state data (internal temperature, operating status...). 
Each technological components is associated to a model that writes |milp| contribution to a global **optimization problem**. 
An **optimization problem** consists of maximization or minimization of an **objective function** under constraints. 

Each energy or mass term (electrical, thermal or material, H2) is associated with an **energy carrier**. 
These energy carriers give the nature of the flow associated to it and the different units to be used.
Energy carriers are linked to **smart connections** (bus or nodes). These smart connections enable data exchange between models. 

|

.. admonition:: Technological components models are divided into **4 main classes** 

	- **Converters models** describing the behavior of conversion technologies (Heat Pumps, combined heat and power units, electrolyzers, 
	  methaners, biogas or biomethane units) with their efficiencies and constraints (start-up or ramp) to pass flow from a given energy carrier to another one.
	- **Storage models** modelling capacities of energy or mass.
	- **Load models** which impose source or load time profiles, with flexibility, load shedding or curtailment abilities.
	- **Grid models** which calculate injected or extracted flow, at a given cost.

	
A technological component model includes:

	- **Input parameters** : the techno-economic and environmental parameters (eg efficiencies, environmental impacts, costs),
	  the boundary condition time series (eg the prices), the maps (CAPEX, performance…) and the setting options.
	- **Decision variables** :
		- About sizing : capacities (nominal production power or flow rate, maximum storage capacity).
		- About operation and control : state variables that are time-dependent, and instantaneous flows
		  exchanged between components (UsedPower between Grid and Electrolyzer).
	- **Constraints** :
		- **Linear constraints** linking decision variables and parameters (e.g. the relationship between
		  input and output, as a simple efficiency between electricity consumption and Hydrogen production in an Electrolysis system).
		- **Advanced constraints** like piecewise linear equations involving decision variables to reflect
		  non linear operation constraints (e.g. an efficiency map for a Heat Pump).

Several modeling levels are possible to match the desired compromise between time and accuracy, as for the example of a **Li-Ion battery** : 
	- it can be modeled either like a generic energy storage with constant charging and discharging 
	  powers assuming main use in the range 20% to 80% of full charge,
	- or it can be modeled using a performance map giving precisely charging and discharging 
	  powers as function of the state of charge of the battery. 

**Smart connections** between ports of models use specific constraint modules; which can be of the following classes:
	
	- **System constraints** (Bus) :
		- **Node Laws** : the variables connected to the node are summed at each time step, a sum 
		  value can be imposed (eg 0 to ensure flow balances).
		- **Node equality** : the variables connected to the node must be equal at each time step.
	- **Generic operational constraints** to add to technological models (for instance ramp constraints).
	- **Physical models to add physical generic equations** to some technological components (temperature).

This modularity, based on object-oriented programming permits having a great modeling freedom as it is illustrated in :numref:`figmain2` :ref:`figmain2`, as well as the ability / capability of capitalizing on models.

|
|

|cairn| is a tool that allows building a model of a multi-energy system and its associated data, and to turn it into an optimization problem.
|cairn| is used to :

	- **solve optimization** problems of so-called **unit-commitment problems** 
	  with size optimization trying to find **optimal energy mix** for an energy system submitted to one or several time load profiles. 
	- **simulate the operation** of the system. The set of equations describes the state of the system 
	  (with accounts for all constraints), depending on the past and on the boundary conditions.

.. admonition:: What are **optimization** and **simulation** modes?

    Optimization and simulation are two commonly used approaches in system modeling and analysis, but they serve different purposes.

	- **Optimization** aims to find **the best possible solution to a given problem**, by maximizing or minimizing an objective function.
	  For example, this could involve minimizing costs, maximizing profit, or achieving the best trade-off between multiple criteria.
	- **Simulation**, on the other hand, is used to model the behavior of a real or hypothetical system under various conditions without
	  necessarily seeking the optimal solution. It allows understanding how a system works, assessing the impact of different 
	  scenarios or conditions, as well as testing hypotheses.

After the optimization, |cairn| allows analyzing the results. The :numref:`figmain1` :ref:`figmain1` illustrates the global steps of the |cairn| operation.

.. container:: cadre 

	.. figure:: images/cairn_steps.PNG
	 :width: 600
	 :alt: Description of what |cairn| does
	 :align: center
	 :name: figmain1

	 Overview of the steps of |cairn|'s operation.
	 
.. seealso:: 

	#. How to run |cairn| ?
	#. What is the methodolgy to use |cairn| ?




Screenshots 
===========

.. container:: cadre 

	.. figure:: images/screenshot_view_architecture.PNG
		:width: 600
		:alt: View and modify easily energy architectures with |gui|
		:align: center
		:name: figmain2
		
		View and modify easily energy architectures with |gui|

.. container:: cadre

	.. figure:: images/screenshot_parameters.PNG
		:width: 600
		:alt: View and modify easily energy architectures with |gui|
		:align: center
		:name: figmain3
		 
		View and modify easily energy architectures with |gui|, with drag-and-and-drop intuitive commands. 

.. container:: cadre

	.. figure:: images/screenshot_plotter.PNG
		:width: 600
		:alt: View and modify easily energy architectures with |gui|
		:align: center
		:name: figmain4
		 
		Visualize and compare results of several studies with the plotter

.. container:: cadre

	.. figure:: images/screenshot_kpi.PNG
		:width: 600
		:alt: View and modify easily energy architectures with |gui|
		:align: center
		:name: figmain5
		 
		Compute automatically KPIs and display it - or publish it into files

.. container:: cadre

	.. figure:: images/screenshot_multistudy.PNG
		:width: 600
		:align: center
		:name: figmain6
		 
		Automatically lauch studies following an experiment plan 

.. container:: cadre

	.. figure:: images/screenshot_timegraph.PNG
		:width: 600
		:align: center
		:name: figmain7
		 
		Publish reports with interactive graphs generated

.. container:: cadre

	.. figure:: images/screenshot_sankey.PNG
		:width: 600
		:align: center
		:name: figmain8
		 
		Screenshot of a balance of energy graph

.. container:: cadre

	.. figure:: images/screenshot_bargraph.PNG
		:width: 600
		:align: center
		:name: figmain9
		 
		Compare several studies through various graphs:

.. container:: cadre

	.. figure:: images/screenshot_pies.PNG
		:width: 600
		:align: center
		:name: figmain10
		 
		Screenshot piegraph

Historic 
========


Citing
======

If you use |cairn|, please cite the following paper [RPCG24]_.

The folling paper [CBRL21]_ refers |cairn| .

