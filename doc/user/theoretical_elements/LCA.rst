.. _lca_intro:


########################
Environmental assessment
########################

Introduction to |lca|
=====================

Life Cycle Assessment (|lca| or E-|lca|) is a tool to evaluate the environmental performance of products (goods, processes and services). 
The methodology of Life Cycle Assessment is formalised by the International Standards Organisation (ISO). 
According to ISO 14040-44, |lca| is defined as “compilation and evaluation of the inputs, outputs and the potential environmental 
impacts of a product system throughout its life cycle”. 
|lca| can be used in the context of decision-making to support product development, policy-making, strategic planning and so on as it facilitates 
the comparison between different processes studied according to the same parameters with the same methodology. 
|lca| aims to evaluate the sustainability of a process, a product or a service along its life cycle steps.

The full assessment covers the entire life cycle of the studied product, process or activity from raw material extraction 
through materials processing, manufacturing, distribution, use, maintenance, and disposal or recycling. 
The environmental impacts of all the inputs and outputs flows occurring during these stages are evaluated.

|lca| provides a set of environmental indicators representative of the consequences of an activity on several aspects of 
the environment (climate change, resource use, air and water quality, human health, etc.). The interpretation of |lca| study all 
along life cycle stages enables the identification of major contributions to the environmental burdens and avoids potential 
issues of impacts shifting.

The standardised |lca| framework encompasses four steps:

#. **Goal and Scope definition**: First, the objectives and intended applications of the |lca| study are explained. 
   The system under study is defined, including its boundaries (conceptual, geographical and temporal), the life cycle stages considered, 
   the quality of data and the main hypothesis applied. This step also include the description of the functional unit, 
   a reference representation of the service provided by the system studied, for which the environmental impacts will be assessed or compared.

#. **Life Cycle Inventory (LCI)** analysis: This phase is a technical process of data collection, in order to quantify the inputs of 
   energy and raw material, and outputs of products, co-products, waste and emissions related to the system, as defined in the 
   scope of the study. 

#. **Life Cycle Impact Assessment (LCIA)**: Potential environmental impacts are calculated from the inventory using a selected assessment 
   method to characterize the impacts of each elementary flow. Dedicated software (SimaPro, GaBi, etc.) and databases (Ecoinvent)
   can be used for this purpose. The choice of impacts assessment method is adapted according to the goal and scope of the study.
   The Environmental Footprint (EF 3.0) method proposed be the European Commission (https://eplca.jrc.ec.europa.eu/)
   is currently one more widely used solution.

#. **Interpretation**: Results are presented and analysed to highlight the main sources of environmental impacts and potential
   comparison with reference system. Consistency of the results is evaluated through sensitivity analysis of modelling choices. 
   |lca| is an iterative process and the conclusions can lead to a revision of the aforementioned steps, for instance 
   to evaluate a modification of the reference system, correct a modelling assumption or compensate low-quality or missing data.

.. container:: cadre 

	.. figure:: images/WorkflowLCA.png
	   :alt: 
	   :width: 600
	   :name: WorkflowLCA
	   :align: center

	   Generic workflow and applications of an |lca|

	   [European Commission, 2021, “Understanding Product Environmental Footprint and Organisation Environmental Footprint methods”,
	   https://op.europa.eu/en/publication-detail/-/publication/c43b9684-4521-11ed-92ed-01aa75ed71a1/language-en
	   ]

.. _lca_ef:

What are the impact categories considered in the EF method? 
===========================================================

.. csv-table:: Impact categories considered in the EF method
	:file: LCA.csv
	:header-rows: 1
	:delim: ;
	:widths: 5 20 200
	:class: longtable
	:name: LCA
	:align: center




 