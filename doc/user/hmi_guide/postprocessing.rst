.. _post-processing:

#############################
Post-processing
#############################

It is possible to produce automatic HTML reports with graphs to compare Persee studies.

An example can be found here: (rajouter le lien).

To create a first report, follow this two steps:
- generate a Scenario by checking the box next to the run button, then click on run, choose a name, click on run. 
- click on Post-processing -> Generate HTML Interactive report. 

A first report, not fully parametrized, will be produced (onpen it with Edge or Chrome, not internet explorer).

You can configure this report through several files.

===========================
The configuration file
===========================

This file contains the list of all the graphs you want to find in the report.

Here is an example.

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


To add a graph, simply another block among the one presented in the following part.


===========================
Basic graphs
===========================
Here are the exisiting graph list with the parameters available. 

**TableDiff**: produces a table of all the parameter that changes between scenarios.

- type: TableDiff
- title: *Title*



**NPV**: produces the graph of the evolution of NPV through years.

- type: NPV
- title: *Title*


**ResolCplex**: Produces a table and a graph of the times of resolutions, and the gaps.
- type: ResolCplex
- title: *Title*




==============================
Parametrizable global graphs
==============================
These graphs are based on the PLAN files. Some of them are plotted for each study (BarGraph, PieChart), others are plotted to compare
studies (StackBarGraph, MultiPieChart).

**StackBarGraph**:



**MultiPieChart**, **BarGraph**, **PieChart**

**XYGraph**, **XYGraphMultiStudy**, **MonotonicLoadGraph**

**SankeyGraph**:


.. raw:: html

    <iframe src="htmlReportExemple.html"></iframe>

 **AreaGraph**

