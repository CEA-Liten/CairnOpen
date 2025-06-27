MultiConverter usage
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
The components of |cairn| are specific components with hardcoded behavior laws in the model (e.g., HeatPump, electrolyzer) and a limited number of inputs/outputs (linked to the component's behavior).

One option could be to add ports linked to one of the existing variables. However, this does not allow for linking multiple variables together.

The MultiConverter offers a solution by allowing the user to write its own matrix of constraints.

Principles
^^^^^^^^^^

This component allows to configure its behavior owing to two constraints:

- Equality constraint:  :math:`A \cdot V = B`
  where :math:`V = \begin{pmatrix}X\\Y\end{pmatrix}` and :math:`B = \begin{pmatrix}B_{X}\\B_{Y}\end{pmatrix}`
  
  X are in the inputs and Y the outputs.

  Thus we have:

  :math:`\forall{m} \in{[|1;N_{I}+N_{J}|]}, \sum_{k=1}^{N_{I}} a_{m,k}x_{k} + \sum_{k=1}^{N_{J}} a_{m,k+N_{I}}y_{k} = B_{m}`

- Unequality constraint: :math:`C \cdot V <= D`

  With the same principle as the equality constraint.

.. note::

    To correctly configure the component, be aware of:
    
    - If we have I, the number of inputs and J, the number of outputs, A and C matrices are two square matrices with a size of :math:`N_{I} + N_{J}`
    - The matrices' coefficients are in the order of inputs then outputs
    - CAPEX is set relative to the first output
    - MaxPower is set relative to the first output
    - All the matrices must be provided owing to .csv files

Advanced example of use: High Temperature Steam Electrolysis
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

An example can be found in Cairn Viewer, in examples/models/multiConverter.

.. warning::
    This example is purely fictive, and made to illustrate some aspects of the multiconverter. It is probably not realistic at all. 

We want to model an electrolyzer that consumes water, heat and electricity and produces hydrogen, fatal heat, and oxygen.

We write the vector X as follows:

.. math::

    \begin{pmatrix}
    P_{elec}\\
    Q_{H_2O}\\
    P_{600}\\
    Q_{H_2}\\
    P_{60}\\
    Q_{O_2}
    \end{pmatrix}

With :math:`P_{elec}` the electric power in MW (InputFlux1), :math:`Q_{H_2O}`, :math:`Q_{H_2}` and :math:`Q_{O_2}` the fluxes in kg/h of water, hydrogen and oxygen, :math:`P_{600}` and :math:`P_{60}` the thermal power of high temperature (600°C) and low temperature (60°C). 

The following set of equation describes the functionning of the system:

The energy balance : 

.. math:: P_{elec} + P_{600} = 33.3 Q_{H2} + P_{60}

.. note::

    Note that if you give only this line of equation, it means that you can choose to put only eletricity or only heat to produce hydrogen, and you can choose to convert everything in hydrogen. This means that you probably have to add some constraint to force to have a fixed proportion between high temperature heat and eletricity or to precise the proportion of thermic losses.  

To which we add an efficiency : the quantity of hydrogen out is limited by a ratio between heat and electricity.  

.. math::    Q_{elec} + 0.5P_{600} = 19.8 Q_{H_2} 

We add the stoechiometric relation describing the chemical transformation :math:`2 H_2O \rightarrow 2H_2 + O_2`.

.. math:: 4.032 Q_{H_2O} = Q_{H_2}

.. math:: 31.99 Q_{H_2} = 4.0332 Q_{O_2}


Then the matrix :math:`A` is the following : 

.. math:: 

    \begin{pmatrix} 
    1 & 0 & 1 & -33.3 & -1 & 0 \\ 
    1 & 0 & 0.5 & -19.8 & 0 & 0 \\ 
    0 & 0 & 0 & -31.99 & 0 & 4.032 \\ 
    0 & 0 & 4.032 & 0 & -36 & 0 & 0 \\ 
    0 & 0 & 0 & 0 & 0 & 0 \\ 
    0 & 0 & 0 & 0 & 0 & 0 
    \end{pmatrix} \cdot
    \begin{pmatrix}
    P_{elec}\\
    Q_{H_2O}\\
    P_{600}\\
    Q_{H_2}\\
    P_{60}\\
    Q_{O_2}
    \end{pmatrix} = 
    \begin{pmatrix}
    0\\
    0\\
    0\\
    0\\
    0\\
    0
    \end{pmatrix}

And the system can be written :math:`AX = B`, with :math:`B=0`.

.. note::
    
    Note that the matrix A is not inversible, which not a problem, otherwise, the system leaves very few freedom to the variables.

