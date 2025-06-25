.. attention:: 

	Work in progress

.. _SetEconomicParam:

How to take into account economic aspects? 
------------------------------------------

What are the possible ways to consider costs at a component level? 
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Most of the time, |capex| and |opex| are considered to describe the economic part of a component.

* The parameter |capex| is expressed in % of nominal capacities of storage or production.
* The parameter |opex| is always defined in % of |capex|

These two parameters are taken into account only if EcoInvestModel option is true.

By default, it is the case for converters and source load models.

Besides, depending on the case other costs can be considered in the |opex| part:

* Variable cost
* Replacement costs
* Energy costs
* Penalty costs

How is the economic aspect considered at a project level? 
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

By default, overnight costs are considered regarding |capex|. 
It means that it is as if no interest was incurred during construction, as if the project was completed "overnight."
To account for one-year delay before payback time, set parameter NbYearOffset to 1 in component Tec-eco.

In |cairn|, the optimization is computed over a period set by the user.
To take into account the fact that the optimization is not computed over the full project duration:

- a factor is applied to have one year
- a discount rate can be applied

.. seealso::
   
    :ref:`investmentDecision`.