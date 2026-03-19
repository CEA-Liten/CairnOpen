.. _post-processing:

#############################
Post-processing
#############################

It is possible to produce automatic HTML reports with graphs to compare |cairn| studies.

The reports are generated automatically to generate graphs that compare studies.

To use it, follow this steps:

#. Run at least one scenario (this creates one subfolder `Report_yourscenario`)

#. Click on post-processing , Generate interactive report

#. This will generate your first HTML report by default, and create tree files, that you can modify to personalize your post processing: `config_extract.xml`, `config_color.csv`, and if needed, `sankey.xml`.

An example of a full report can be found here: `click here <../_static/report_example.html>`_


===========================
The configuration files
===========================

config_extract.xml
---------------------

This file contains the list of all the graphs you want to find in the report.


.. code-block:: XML

    <graph_list>
    <graph>
        <type>PieChart</type>
        <title>Cost structure [EUR]</title>
        <search_strings>Total Cost</search_strings>
        <threshold>1.00E+03</threshold>
    </graph>
    <graph>
        <type>StackBarGraph</type>
        <title>Balance of costs and incomes</title>
        <search_strings>Total Cost</search_strings>
        <threshold>1</threshold>
    </graph>
    <graph>
        <type>BarGraph</type>
        <title>Installed Optimal Size for Converters</title>
        <search_strings>Installed Optimal Size</search_strings>
        <threshold>1</threshold>
    </graph>
    <graph>
        <type>DynamicGraph</type>
        <title>Thermal Utility loads</title>
        <plotted_variables>LNG180_Load.SourceLoadFlow</plotted_variables>
        <section_title>Time series : Thermal Utility loads</section_title>
        <comment>It shows thermal power loads imposed by plant utilities.</comment>
    </graph>
    <graph>
        <type>MonotonicLoadGraph</type>
        <title>Number of hours at given RES powers</title>
        <plotted_variables>PV_Field1.SourceLoadFlow,WT_OnShore.SourceLoadFlow,Solar_Tower.SourceLoadFlow,Solar_CSP.SourceLoadFlow</plotted_variables>
        <section_title>Time series : RES available powers</section_title>
        <comment>It represents number of hours at given available RES powers.</comment>
    </graph>
    </graph_list>


To add a graph, simply add another block among the one presented in the following part.


Set common colors
--------------------

Graphs are more simple to understand if the colors remain the same for each component.

To set colors, add a file in the same folder than config_extract file. **Note that the name has to be exactly `config_Colors.csv`**. 

.. csv-table:: |cairn| setting file config_Colors.csv
	:file: ./example_config_Colors.csv
	:header-rows: 1
	:delim: ;
	:widths: 5 5 
	:name: config_color.csv
	:align: center
	:class: table1


===========================
Global graphs
===========================
Here are the exisiting graph list with the parameters available. 

TableDiff
--------------------

Produces a table of all the parameter that changes between scenarios.

.. code-block:: XML

	<graph>
		<type>TableDiff</type>
		<title>Differences between parameters</title>
		<comment>test</comment>
	</graph>


Here is an example.

.. raw:: html

   <iframe src="../_static/postprocessing/TableDiff.html"
           width="100%"
           height="600"
           style="border:none;">
   </iframe>



NPV
----

Produces the graph of the evolution of NPV through years.

- type: NPV
- title: *Title*

.. code-block:: XML

    <graph>
		<type>NPV</type>
		<title>NPV over years</title>
	</graph>


Here is an example.

.. raw:: html

   <iframe src="../_static/postprocessing/NPV.html"
           width="100%"
           height="600"
           style="border:none;">
   </iframe>


ResolCplex 
-----------

Produces a table and a graph of the times of resolutions, and the gaps.

.. code-block:: XML

	<graph>
		<type>ResolCplex</type>
		<title>Statistics about resolution times</title>
	</graph>


Here is an example.

.. raw:: html

   <iframe src="../_static/postprocessing/cplex_stats.html"
           width="100%"
           height="600"
           style="border:none;">
   </iframe>



=================================
Graphs to analyse global results
=================================
These graphs are based on the PLAN files. Some of them are plotted for each study (BarGraph, PieChart, Sankey), others are plotted to compare
studies (StackBarGraph, MultiPieChart).

To select a variable in PLAN file, you need to put the corresponding Alias string in `search_strings` and if needed remove other with `excluded_strings`. If several variables are needed, separate the string with a comma, and without spaces. 




SankeyGraph 
-------------------

Produces a graph of sankey of the energy vectors used in the project.

.. code-block:: XML

	<graph>
		<type>SankeyGraph</type>
		<title>Sankey graph</title>
		<overwrite>True</overwrite>
		<file_path>.\sankey_graph.xml</file_path>
	</graph>


If the option `overwrite` is set on `true`, the file describing Cairn

.. raw:: html

   <iframe src="../_static/postprocessing/Sankey.html"
           width="100%"
           height="600"
           style="border:none;">
   </iframe>

.. important::

    If the option `overwrite` is set on true, the file `sankey_graph.xml` is generated automatically from the json study file. It is possible then to modify by hand this file to change the appearance of the sankey. Don't forget then to set `overwrite` on false to avoid to re-generate the file the next time.


PieChart 
-------------------

Produces pie chart a wanted variables in PLAN file.

.. code-block:: XML

    <graph>
        <type>PieChart</type>
        <title>CAPEX repartition</title>
        <color>True</color>
        <search_strings>CAPEX</search_strings>
        <excluded_strings>TecEco</excluded_strings>
        <threshold>1.00E+03</threshold>
    </graph>

.. raw:: html

   <iframe src="../_static/postprocessing/pieChart.html"
           width="100%"
           height="600"
           style="border:none;">
   </iframe>


MultiPieChart 
-------------------

Produces pie chart a wanted variables in PLAN file. Each size is proportionnal to the total of each case.

.. code-block:: XML

    <graph>
        <type>MultiPieChart</type>
        <title>Electricity consumption</title>
        <search_strings>TotGridFlow,TotImposedProfileSourceLoadFlow
    </search_strings>
        <excluded_strings>GridFree#5,Demande_H2</excluded_strings>
        <threshold>1.00E+03</threshold>
        <color>True</color>
        <nbcol>2</nbcol>
    </graph>

.. raw:: html

   <iframe src="../_static/postprocessing/multiPieChart.html"
           width="100%"
           height="600"
           style="border:none;">
   </iframe>


StackBarGraph 
-------------------

Produces a stack bar graph of a given variable, grouped by case. 

.. code-block:: XML

    <graph>
        <type>StackBarGraph</type>
        <title>Electrical Power produced and consumed</title>
        <search_strings>TotGridFlow,TotImposedProfileSourceLoadFlow</search_strings>
        <excluded_strings>Demande_H2,Lvlz</excluded_strings>
        <threshold>1</threshold>
        <color>True</color>
    </graph>

.. raw:: html

   <iframe src="../_static/postprocessing/stackBarGraph.html"
           width="100%"
           height="600"
           style="border:none;">
   </iframe>



XYGraphMultiStudy
---------------------

Produces a graph of xy variables issued from PLAN file (one point for one study).

.. code-block:: XML

    <graph>
        <type>XYGraphMultiStudy</type>
        <title>Wind and storage optimal sizes</title>
        <unit>MW</unit>
        <xlabel>StorageGen#1.Storage Optimal Capacity kgMaxEsto </xlabel>
        <xvar>StorageGen#1.Capacity</xvar>
        <ylabel>MW</ylabel>
        <yvar>Wind.Weight</yvar>
        <section_title>Time series: Electrical Power Provenance</section_title>
        <search_strings>Capacity,Weight</search_strings>
        <comment>.</comment>
    </graph>


.. raw:: html

   <iframe src="../_static/postprocessing/xymulti.html"
           width="100%"
           height="600"
           style="border:none;">
   </iframe>


=================================
Graphs to analyse timeseries
=================================

These graphs are base on the _Results files. Use the names of the columns in "plotted_variables" options.

AreaGraph 
-------------------

Produces a stacked graph of a given energy vectors producers and consumers. The graph is generated from the sankey graph file. The positive part is produced and negative is consumed.

.. code-block:: XML

	<graph>
		<type>AreaGraph</type>
		<vector>H2Vector#1</vector>
		<title>H2 balance</title>
		<unit>MW</unit>
		<section_title>Time series : Electrical Power Provenance</section_title>
		<file_path>.\sankey_graph.xml</file_path>
		<comment>Positive : produced, negative: consumed</comment>
	</graph>


If the option `overwrite` is set on `true`, the file describing Cairn

.. raw:: html

   <iframe src="../_static/postprocessing/areaGraph.html"
           width="100%"
           height="600"
           style="border:none;">
   </iframe>


DistributionGraph 
-------------------

Produces a graph of DistributionGraph of several variables.

.. code-block:: XML

	<graph>
		<type>DistributionGraph</type>
		<title>Distribution graph of the SOEC use</title>
		<ylabel>Number of hours</ylabel>
		<xlabel>MW</xlabel>
		<plotted_variables>Electrolyzer#1.UsedPower</plotted_variables>
	</graph>

.. raw:: html

   <iframe src="../_static/postprocessing/distributionGraph.html"
           width="100%"
           height="600"
           style="border:none;">
   </iframe>


MonotonicLoadGraph 
-------------------

Produces a graph of monotonic graph of several variables.

.. code-block:: XML

    <graph>
        <type>MonotonicLoadGraph</type>
        <title>Electrical Power produced </title>
        <unit>MW</unit>
        <plotted_variables>Wind.SourceLoadFlow,Solar.SourceLoadFlow</plotted_variables>
        <section_title>Time series : Electrical Power Provenance</section_title>
        <comment>.</comment>
    </graph>


.. raw:: html

   <iframe src="../_static/postprocessing/Monotonic.html"
           width="100%"
           height="600"
           style="border:none;">
   </iframe>

MonotonicLoadGraph 
-------------------

Produces a graph of a variable x in function of y. Possibility to respect the color code if color is set on True. Otherwise the colors are set in a random order. 

.. code-block:: XML

    <graph>
        <type>XYGraph</type>
        <title>Electrical Power produced (XYGraph) </title>
        <unit>MW</unit>
        <plotted_variables>Wind.SourceLoadFlow,Solar.SourceLoadFlow,GridFree#4.GridFlow</plotted_variables>
        <xvar>Wind.SourceLoadFlow</xvar>
        <xlabel>Wind.SourceLoadFlow(MW)</xlabel>
        <ylabel>(MW)</ylabel>
        <yvar>Solar.SourceLoadFlow,GridFree#4.GridFlow</yvar>
        <section_title>Time series : Electrical Power Provenance</section_title>
        <color>True</color>
        <comment>.</comment>
    </graph>


.. raw:: html

   <iframe src="../_static/postprocessing/xyGraph.html"
           width="100%"
           height="600"
           style="border:none;">
   </iframe>



