/**
* \file		MultiConverter.cpp
* \brief	Multigeneration model with at most 3 inputs and 2 outputs model
* \version	1.0
* \author	Thibaut Wissocq
* \date		05/2024
*/

#ifndef MultiConverter_H
#define MultiConverter_H

#include "globalModel.h"
#include "ConverterSubModel.h"
#include <Eigen/Dense>

/**
 * \details

This component models conversion between different fluxes.

 .. figure:: ../images/MultiConverter.svg
   :alt: IO MultiConverter
   :name: IOMultiConverter
   :width: 200
   :align: center

   I/O MultiConverter

The number of input fluxes :rc:`NbInputFlux` and output fluxes :rc:`NbOutputFlux` 
are model parameters. Be careful to set the number of ports consistently 
with :rc:`NbInputFlux` and :rc:`NbOutputFlux`.

Flows are of two types:

- Input flux : unit system = [Power or Flowrate, Energy or Mass],   
  with type Electrical, Thermal or Fluid, named :rc:`INPUTFluxI`
  where :rc:`I` is between :rc:`1` and :rc:`NbInputFlux`.

- Output flux : unit system = [Power or Flowrate, Energy or Mass],
  with type Electrical, Thermal or Fluid, named :rc:`OUTPUTFluxJ`
  where :rc:`J` is between :rc:`1` and :rc:`NbOutputFlux`.

:rc:`INPUTFluxI` and :rc:`OUTPUTFluxJ` are linked by the matrices :math:`A` and :math:`B`,
defined as :math:`A [X\ Y]^T = B`, where:

- :math:`X` is the vector of input fluxes (size :rc:`NbInputFlux`)
- :math:`Y` is the vector of output fluxes (size :rc:`NbOutputFlux`)
- :math:`A` is a block matrix :math:`[A_1\ A_2;\ A_3\ A_4]`
- :math:`B = [B_1\ B_2]^T` is an offset vector

This defines the system:

.. math::
    A_1 X + A_2 Y = B_1 \qquad (\text{NbInputFlux equations})

.. math::
    A_3 X + A_4 Y = B_2 \qquad (\text{NbOutputFlux equations})

The option :rc:`Inequality Constraint` allows defining the additional system
:math:`C [X\ Y]^T \le D` using the same structure.

Sizing is done relative to the first output :rc:`OUTPUTFlux1` for :rc:`MaxPower` and :rc:`Capex`.

-------------------------------------------
Upper Bounds on Inputs and Outputs
-------------------------------------------

Upper limits for inputs and outputs can be specified using the parameter :rc:`UpperBoundsFile` (CSV format). 

The File must contain a single column with exactly :rc:`NbInputFlux + NbOutputFlux` values and no header,
following the same structure as the Matrix-B CSV File (see below: points 1 and 2).

If all bounds are identical, a single value may be provided. This value is then expanded to a vector of size 
:rc:`NbInputFlux + NbOutputFlux`.

Each bound is interpreted as a relative limit and the actual upper bound is: 
:math:`\text{MaxPower} \times \text{givenLimit}` for every input and output flux.

If no CSV File is provided, an upper limit :math:`\text{MaxPower} \times \text{weight}` is applied only on the first output. 
No upper limits are applied to the other outputs or to any input.

-------------------------------------------
Format and Size of Matrix A (and Matrix C) 
-------------------------------------------

The conversion matrix :math:`A` (or :math:`C` when inequality constraints are used)
must be a **square matrix** of size:

.. math::
    (\text{NbInputFlux + NbOutputFlux}) \times (\text{NbInputFlux + NbOutputFlux})

The CSV File must therefore contain exactly:

- :rc:`NbInputFlux + NbOutputFlux` rows
- :rc:`NbInputFlux + NbOutputFlux` columns
- numeric values separated by :rc:`;`

Additional validity rule:

- The column corresponding to :rc:`OUTPUTFlux1`
  (i.e. column index :rc:`NbInputFlux`) must contain **at least one non-zero value**,
  ensuring that the first output flux participates in the conversion.

------------------------------------------
Format and Size of Vector B (and Vector D) 
------------------------------------------ 

The vector :math:`B` (or :math:`D` for inequality constraints) is a column vector of size:

.. math::
    \text{NbInputFlux + NbOutputFlux}

Accepted CSV formats:

1. **Full vector:** 
   a single column with exactly :rc:`NbInputFlux + NbOutputFlux` numeric values.

2. **Single-value shortcut:** 
   if the File contains only one cell :rc:`[k]`, it is expanded to a vector:

   .. math::
       [k,\ k,\ \dots,\ k]^T \quad \text{(size NbInputFlux + NbOutputFlux)}

3. **Missing File:** 
   if the File does not exist, a warning is issued and a **zero vector** of the correct size is used.

Any other dimension (e.g., multiple columns or an incorrect number of rows)
results in an exception.
*/


class MODELS_DECLSPEC MultiConverter : public ConverterSubModel {

public:
    //-----------------------------------------------------------------------------------------------------
    MultiConverter(CairnObject* aParent);
    ~MultiConverter();
    //----------------------------------------------------------------------------------------------------
    void computeInitialData() override;
    void computeModelContribution() override;
    void computeEconomicalContribution();
    void computeAllIndicators(const double* optSol) override;

    void setTimeData();
    int checkConsistency();

    void readAndVerifyMatrixA(const std::string& filename, std::vector<std::vector<double>>& matrix, const bool& isMatrixC);
    void readAndVerifyVectorB(const std::string& filename, std::vector<double>& aVector, const bool& isVectorD);
    void readUpperBounds();

    Eigen::MatrixXd convertToEigen(const std::vector<std::vector<double>>& matrix) {
        if (matrix.empty()) {
            return Eigen::MatrixXd(0, 0); // Returns an empty matrix if the input matrix is empty.
        }

        int rows = matrix.size();
        int columns = matrix[0].size();
        Eigen::MatrixXd matriceEigen(rows, columns);

        for (int i = 0; i < rows; ++i) {
            for (int j = 0; j < columns; ++j) {
                matriceEigen(i, j) = matrix[i][j];
            }
        }

        return matriceEigen;
    }

    //double norm1(const Eigen::MatrixXd& matrix) {
    //    return matrix.cwiseAbs().sum();
    //}

    //double smallestNonZeroCoefficient(const Eigen::MatrixXd& matrix);

//----------------------------------------------------------------------------------------------------
    void declareModelConfigurationParameters()
    {
        ConverterSubModel::declareDefaultModelConfigurationParameters();
        //int
        addParameter("NbInputFlux", &mNbInputFlux, 1, true, true, "Number of first Inputs dedicated to Fluxes  <= NbInputPorts declared in component definition");  
        addParameter("NbOutputFlux", &mNbOutputFlux, 1, true, true, "Number of first outputs dedicated to Fluxes <= NbOutputPorts declared in component definition");  
        addParameter("Inequality Constraint", &mIsIneqCstr, false, false, true, "Use inequality constraint if true if false");
    }
    
    // Units: use the following, instead of the IS Units leading to "scaling" troubles during solving step
    // Mass Flow    : kg/h
    // Power flow   : MW
    // Mass         : kg
    // Energy       : MWh
    // Time         : Hours
    void declareModelInterface()
    {
        if (mNbInputFlux < 1) mNbInputFlux = 1;
        if (mNbOutputFlux < 1) mNbOutputFlux = 1;
        mExpInput.resize(mNbInputFlux);
        mExpOutput.resize(mNbOutputFlux);
        mExpMatrixProduct.resize(mNbInputFlux + mNbOutputFlux);
        mExpMatrixProduct_ineq.resize(mNbInputFlux + mNbOutputFlux);

        ConverterSubModel::declareDefaultModelInterface();

        addSizeMaxIO("MaxPower", &mExpSizeMax, true, mPortINPUTFlux1->pFluxUnit());          /** Sizing W */
        addIO("INPUTFlux1", &mExpInput[0], true, mPortINPUTFlux1->pFluxUnit()); /** Computed input flow at default port PortINPUTFlux1 */
        addIO("OUTPUTFlux1", &mExpOutput[0], true, mPortOUTPUTFlux1->pFluxUnit()); /** Computed output flow at default port PortOUTPUTFlux1 */

        ConverterSubModel::declareInputFluxIOs(mPortINPUTFlux1);
        ConverterSubModel::declareOutputFluxIOs(mPortOUTPUTFlux1);

        for (int i = 0; i < mNbInputFlux + mNbOutputFlux; i++)
        {
            if (mExpMatrixProduct.size() > i) {
                addExp(&mExpMatrixProduct[i], &mHorizon);
            }
            if (mExpMatrixProduct_ineq.size() > i) {
                addExp(&mExpMatrixProduct_ineq[i], &mHorizon);
            }
        }
    }

    //----------------------------------------------------------------------------------------------------

    void declareModelParameters()
    {
        ConverterSubModel::declareDefaultModelParameters();

        //double
        addParameter("MaxPower", &mMaxPower, INFINITY_VAL, true, true, "Maximum output of OUTPUTFlux1");	 
        //std::string
        addParameter("MatrixA", &mMatrixA, "", true, true, "CSV file of the matrix A in the formula : A * [Input Output] = B  of size NbInput + NbOutput", "string");
        addParameter("MatrixB", &mMatrixB, "", true, true, "CSV file of the matrix B in the formula : A * [Input Output] = B  of size NbInput + NbOutput", "string");

        addParameter("MatrixC", &mMatrixC, "", mIsIneqCstr, mIsIneqCstr, "CSV file of the matrix C in the formula : C * [Input Output] <= D", "string");
        addParameter("MatrixD", &mMatrixD, "", mIsIneqCstr, mIsIneqCstr, "CSV file of the matrix D in the formula : C * [Input Output] <= D", "string");
    
        addParameter("UpperBoundsFile", &mUpperBoundsFile, "", false, true, "CSV file of upper bounds on inputs and outpts. The size of the vector must be NbInputFlux + NbOutputFlux", "string");
    }

    void declareModelIndicators() {
        ConverterSubModel::declareDefaultModelIndicators(&mExportIndicators);
    }

    void initDefaultPorts()
    {
        mDefaultPorts.clear();
        //PortINPUTFlux1 - left
        std::map<std::string, std::string> portINPUTFlux1;
        portINPUTFlux1["Name"] = "PortL0"; //Needed only old versions
        portINPUTFlux1["Position"] = "left";
        portINPUTFlux1["CarrierType"] = ANY_TYPE();
        portINPUTFlux1["Direction"] = KCONS(); //INPUT
        portINPUTFlux1["Variable"] = "INPUTFlux1";
        mDefaultPorts["PortINPUTFlux1"] = portINPUTFlux1; //ID, paramMap

        //PortOUTPUTFlux1 - right
        std::map<std::string, std::string> portOUTPUTFlux1;
        portOUTPUTFlux1["Name"] = "PortR0";
        portOUTPUTFlux1["Position"] = "right";
        portOUTPUTFlux1["CarrierType"] = ANY_TYPE();
        portOUTPUTFlux1["Direction"] = KPROD();
        portOUTPUTFlux1["Variable"] = "OUTPUTFlux1";
        mDefaultPorts["PortOUTPUTFlux1"] = portOUTPUTFlux1;
    }

    void setPortPointers() {
        mPortINPUTFlux1 = getPort("PortINPUTFlux1");
        mPortOUTPUTFlux1 = getPort("PortOUTPUTFlux1");
    }

    //----------------------------------------------------------------------------------------------------
protected:
    MilpPort* mPortINPUTFlux1;
    MilpPort* mPortOUTPUTFlux1;

    MIPModeler::MIPVariable1D mYOnOff;     /** ON/OFF status allowing for complete stopping or run at minimal power */

    MIPModeler::MIPVariable1D mInputTot;

    std::vector <MIPModeler::MIPVariable1D> mInput;
    std::vector <MIPModeler::MIPVariable1D> mOutput;

    std::vector <MIPModeler::MIPExpression1D> mExpMatrixProduct;
    std::vector <MIPModeler::MIPExpression1D> mExpMatrixProduct_ineq;

    // model parameters

    double mMinTotalPower;
    double mMaxTotalPowerFraction;
    double mMaxPower;

    bool mIsIneqCstr;

    std::string mMatrixA;
    std::string mMatrixB;

    std::vector<std::vector<double>> mCoefficient_A;
    std::vector<double> mCoefficient_B;

    std::string mMatrixC;
    std::string mMatrixD;

    std::vector<std::vector<double>> mCoefficient_C;
    std::vector<double> mCoefficient_D;

    std::string mUpperBoundsFile;
    std::vector<double> mUpperBounds;
};


#endif
