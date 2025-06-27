Setting advanced constraints on components
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Sometimes, you may want to add your own constraints between components.
This may be the case when:

-  you want to limit the power that a component may extract from a grid
   (example 1 below)

-  you want to combine the operation of two production units, imposing
   an exclusion constraints on the two units (example 2 below),

-  you want to impose a yearly power partition between two production
   units (example 3 below)

Example 1: constraint for limiting the power a component may extract from a grid
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

| Let us suppose you have an electroloysis with electrical input and H2
  output, and want to exchange with a grid component the maximum power
  the electrolyzer is allowed to extract from grid at each time of a
  simulation horizon.
| You need a way to get this maximum value from grid to electrolyzer.
| This is illustrated by the example below, and explained just below. To
  do this you must:

#. define a bus with model NodeEquality, and energy vector Electricity.

#. create a port on the Grid component, specifing the value MaxPower as
   variable to export, and connect it to the bus you have just created.
   It is possible as it is present in the list of published values by
   the Grid.

#. create a port on the electrolyzer component, to get the value, and
   connect it to the bus you have just created. It is possible as it is
   present in the list of published values by the Electrolyzer.

This will add a constraint to your optimization problem telling that
MaxPower of Grid should be the same as MaxPower of Electrolyzer.

.. code-block:: xml

	<BusFlowBalance id="Elec_Bus">
		<Model>NodeLaw</Model>
		<VectorName>ElectricityDistrib</VectorName>
	</BusFlowBalance>
	<BusSameValue id="CNX_GridEly">
		<Model> NodeEquality </Model>
		<VectorName>ElectricityDistrib</VectorName>
	</BusSameValue>
	<BusSameValue id="P_GridEly">
		<Model> NodeEquality </Model>
		<VectorName>ElectricityDistrib</VectorName>
	</BusSameValue>
	<Converter id="QUALYGRIDS_ALK">
		<Model> Electrolyzer </Model>
		<Port1 Variable=" H2MassFlowRate " Use="OUTPUT">H2_Bus</Port1>
		<Port2 Variable="UsedPower" Use="INPUT">Elec_Bus</Port2>
		<Port3 Variable="MaxPower" Use="OUTPUT"> CNX_GridEly </Port3>
		<Port4 Variable="UsedPower" Use="OUTPUT">P_GridEly</Port4>
		< ComputeProductionCost > H2MassFlowRate </ ComputeProductionCost >
		<UsedPower.Coeff>+1.</UsedPower.Coeff>
	</Converter>
	< GridFCRService id="Elec_Grid">
		<file>.\lib\ GridFCRService .dll</file>\textsl{}
		< ExtractFromGrid >1</ ExtractFromGrid >
		< UseZEPrice >true</UseZEPrice >
		< UseProfileFCRCapacityPrice >Elec_Grid.CapacityFCR </ UseProfileFCRCapacityPrice >
		<Port1 Variable="GridFlow" Use="OUTPUT">Elec_Bus</Port1>
		<Port2 Variable="MaxPower" Use="INPUT"> CNX_GridEly </Port2>
		<Port3 Variable="UsedPower" Use="INPUT">P_GridEly</Port3>
	</ GridFCRService >

Example 2: constraint for combining the operation of two production units 
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

This is illustrated by the example below, and explained just below. To
do this you must:

#. define a bus Heat_SUMBIOGAZ with model NodeLaw, to add a Summation
   type constraint, and energy vector of whatever type you want (Fluid,
   Electrical or Thermal), it has no meaning in this case,

#. set the bus settings: StrictConstraint = false, MaxConstraint=true,
   and MaxConstraintBusValue = 1.,

#. create an additionnal port on each production unit, specifing the
   value "States" as variable to export, (ie with use="OUPUT"
   attribute), and connect it to the bus you have just created. It is
   possible only for unit production models publishing States in the
   list of published values (see for instance ProductionUC or
   ThermalGroup submodels).

This will add a constraint to your optimization problem telling that the
sum of both States should lower than 1., at each timestep of the
planning horizon.

.. code-block:: xml

	<EnergyVector id="CH4">
               <Type>FluidCH4</Type>
               <FluxUnit>kg/h</FluxUnit>
               < StorageUnit >kg</StorageUnit >
       </EnergyVector>
       <EnergyVector id="BoisSec">
               <Type> FluidDryWood </Type>
               <FluxUnit>kg/h</FluxUnit>
               < StorageUnit >kg</StorageUnit >
       </EnergyVector>
       <EnergyVector id="Chaleur">
               <Type>Thermal</Type>
               <FluxUnit>MW</FluxUnit>
               < StorageUnit >MWh</StorageUnit >
       </EnergyVector>
       <Converter id="GenGAZ">
               <Model> ThermalGroup </Model>
               <Port1 Variable="INPUTFlux1" Use="INPUT"> CH4_Balance </Port1>
               <Port2 Variable="OUTPUTFlux1" Use="OUTPUT"> Heat_Balance </Port2>
               <Port3 Variable="States" Use="OUTPUT"> Heat_SUMBIOGAZ </Port3>
               < ComputeProductionCost > OUTPUTFlux1 </ ComputeProductionCost >
       </Converter>
       <Converter id="GenBIO">
               <Model> ThermalGroup </Model>
               <Port1 Variable="INPUTFlux1" Use="INPUT"> Wood_Balance </Port1>
               <Port2 Variable="OUTPUTFlux1" Use="OUTPUT"> Heat_Balance </Port2>
               <Port3 Variable="States" Use="OUTPUT" Coeff="1"> Heat_SUMBIOGAZ </Port3>
               < ComputeProductionCost > OUTPUTFlux1 </ ComputeProductionCost >
       </Converter>
       <BusFlowBalance id="CH4_Balance">
               <Model>NodeLaw</Model>
               <VectorName>CH4</VectorName>
       </BusFlowBalance>
       <BusFlowBalance id="Wood_Balance">
               <Model>NodeLaw</Model>
               <VectorName>BoisSec</VectorName>
       </BusFlowBalance>
       <BusFlowBalance id="Heat_Balance">
               <Model>NodeLaw</Model>
               <VectorName>Chaleur</VectorName>
       </BusFlowBalance>
       <BusFlowBalance id="Heat_SUMBIOGAZ">
               <Model>NodeLaw</Model>
               <VectorName>Chaleur</VectorName>
</BusFlowBalance>

and the corresponding settings in the JSON file are :

.. code-block:: json

	Heat_SUMBIOGAZ . StrictConstraint =false
	Heat_SUMBIOGAZ . MaxConstraint =true
	Heat_SUMBIOGAZ . MaxConstraintBusValue =1.

Example 3: constraint for imposing a yearly power partition between two production units
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Let us suppose you want to set a production constraint such that 40
percent of the total yearly heat production be produced by the Biomass
heat production unit. This can be achieved by summing the power from
both production units, take 40 percent of it and set a constraint such
that the biomass heat production represent at least 40 percent of the
yearly total production.

This is illustrated by the example below, and explained just below. To
do this you must:

#. define a bus Heat_SUMBIOGAZ with model NodeLaw, to perform a
   Summation, and energy vector of Energy Type,

#. set the bus settings: StrictConstraint = false : NO constraint on the
   summation,

#. add port1 on Heat_SUMBIOGAZ to get the result of the summation
   through the Variable="BusBalance" Use="OUTPUT" statements at that
   port.

#. create an additionnal port on each production unit, specifing the
   value "OUTPUTFlux1" as variable to export, (ie with use="OUPUT"
   attribute), and connect it to the bus you have just created. It is
   possible only for unit production models publishing OUTPUTFlux1 in
   the list of published values (see for instance ProductionUC or
   ThermalGroup submodels).

#. define a bus Heat_YEARCONSTRAINT with model NodeLaw, to perform a
   Summation with TimeIntegration, and energy vector of Energy Type,

#. set the Heat_YEARCONSTRAINT bus settings: MaxIntegrateConstraint=true
   : Max-bounded integration constraint on the summation of 40 percent
   of the total power computed by Heat_SUMBIOGAZ, and the full BIOGAZ
   power from its port number 4.

.. code-block:: xml

	<EnergyVector id="CH4">
                 <Type>FluidCH4</Type>
                 <FluxUnit>kg/h</FluxUnit>
                 < StorageUnit >kg</StorageUnit >
         </EnergyVector>
         <EnergyVector id="BoisSec">
                 <Type> FluidDryWood </Type>
                 <FluxUnit>kg/h</FluxUnit>
                 < StorageUnit >kg</StorageUnit >
         </EnergyVector>
         <EnergyVector id="Chaleur">
                 <Type>Thermal</Type>
                 <FluxUnit>MW</FluxUnit>
                 < StorageUnit >MWh</StorageUnit >
         </EnergyVector>
         <Converter id="GenGAZ">
                 <Model> ThermalGroup </Model>
                 <Port1 Variable="INPUTFlux1" Use="INPUT"> CH4_Balance </Port1>
                 <Port2 Variable="OUTPUTFlux1" Use="OUTPUT"> Heat_Balance </Port2>
                 <Port3 Variable="OUTPUTFlux1" Use="OUTPUT" Coeff="0.4"> Heat_SUMBIOGAZ </Port3>
                 < ComputeProductionCost > OUTPUTFlux1 </ ComputeProductionCost >
         </Converter>
         <Converter id="GenBIO">
                 <Model> ThermalGroup </Model>
                 <Port1 Variable="INPUTFlux1" Use="INPUT"> Wood_Balance </Port1>
                 <Port2 Variable="OUTPUTFlux1" Use="OUTPUT"> Heat_Balance </Port2>
                 <Port3 Variable="OUTPUTFlux1" Use="OUTPUT" Coeff="0.4"> Heat_SUMBIOGAZ </Port3>
                 <Port4 Variable="OUTPUTFlux1" Use="OUTPUT" Coeff="-1"> Heat_YEARCONSTRAINT </Port4>
                 < ComputeProductionCost > OUTPUTFlux1 </ ComputeProductionCost >
         </Converter>
         <BusFlowBalance id="CH4_Balance">
                 <Model>NodeLaw</Model>
                 <VectorName>CH4</VectorName>
         </BusFlowBalance>
         <BusFlowBalance id="Wood_Balance">
                 <Model>NodeLaw</Model>
                 <VectorName>BoisSec</VectorName>
         </BusFlowBalance>
         <BusFlowBalance id="Heat_Balance">
                 <Model>NodeLaw</Model>
                 <VectorName>Chaleur</VectorName>
         </BusFlowBalance>
         <BusFlowBalance id="Heat_SUMBIOGAZ">
                 <Model>NodeLaw</Model>
                 <VectorName>Chaleur</VectorName>
                 <Port1 Variable="BusBalance" Use="OUTPUT"> Heat_YEARCONSTRAINT </Port1>
         </BusFlowBalance>
         <BusFlowBalance id="Heat_YEARCONSTRAINT">
                 <Model>NodeLaw</Model>
                 <VectorName>Chaleur</VectorName>
          </BusFlowBalance>
   
and the settings are :

.. code-block:: json

	Heat_SUMBIOGAZ . StrictConstraint =false
	Heat_YEARCONSTRAINT . StrictConstraint =false
	Heat_YEARCONSTRAINT . MaxIntegrateConstraint =true
	Heat_YEARCONSTRAINT . MaxIntegrateConstraintBusValue =0.
