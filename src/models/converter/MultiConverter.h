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

The number or INPUTFlux and OUTPUTFlux are parameters model. 

Be careful to set the number of ports consistency with NbInputFlux and NbOutputFlux.

Flows are of two types:

-  Input flux : unit system = [Power or Flowrate, Energy or Mass], with
   type "Electrical", "Thermal" or "Fluid", named INPUTFluxI where I is a number between 1 and NbInputFlux

-  Output flux : input efficiency (unit system = [Power or Flowrate,
   Energy or Mass], with type "Electrical", "Thermal" or "Fluid", named OUTPUTFluxJ where J is a number between 1 and NbOutputFlux

INPUTFluxI and OUTPUTFluxJ are linked by the matrices A and B, defined as A [X Y]^T = B
Where A is a block matrix : [A1 A2, A3 A4], X the vector of INPUTFlux (size NbInputFlux) and Y the vector of OUTPUTFlux (size NbOutputFlux), and B a vector [B1 B2]^T that can be seen as an offset
It defines the set of equations : A1 X + A2 Y = B1 (NbInput equations) and A3 X + A4 Y = B2.

An option is available to define also the set of equations C [X Y]^T <= D in the same principle.

Sizing is done relative to the first ouput OUTPUTFlux1 for MaxPower and Capex.
*/


class MODELS_DECLSPEC MultiConverter : public ConverterSubModel {

public:
    //-----------------------------------------------------------------------------------------------------
    MultiConverter(QObject* aParent);
    ~MultiConverter();
    //----------------------------------------------------------------------------------------------------
    void buildModel();

    int checkConsistency();
    void checkConsistencyMatrix(QString filePath);
    //----------------------------------------------------------------------------------------------------
    void computeEconomicalContribution();
    void computeAllIndicators(const double* optSol) override;

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

    double norm1(const Eigen::MatrixXd& matrix) {
        return matrix.cwiseAbs().sum();
    }

    double smallestNonZeroCoefficient(const Eigen::MatrixXd& matrix) {
        double minCoeff = 1e9;

        for (int i = 0; i < matrix.rows(); ++i) {
            for (int j = 0; j < matrix.cols(); ++j) {
                double value = matrix(i, j);
                if (value != 0 && std::abs(value) < minCoeff) {
                    minCoeff = std::abs(value);
                }
            }
        }

        return minCoeff;
    }

//----------------------------------------------------------------------------------------------------
    void declareModelConfigurationParameters()
    {
        ConverterSubModel::declareDefaultModelConfigurationParameters();
        mInputParam->addToConfigList({ "EcoInvestModel","EnvironmentModel" });
        //int
        addParameter("NbInputFlux", &mNbInputFlux, 1);  /** Number of first Inputs dedicated to Fluxes - <= NbInputPorts declared in component definition */
        addParameter("NbOutputFlux", &mNbOutputFlux, 1); /** Number of first outputs dedicated to Fluxes - <= NbOutputPorts declared in component definition */
        addParameter("Inequality Constraint", &mIsIneqCstr, false, false, true, "Use inequality constraint if true if false ", "", { "" });
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
        mExpInputPart.resize(mNbOutputFlux);
        mExpOutputPart.resize(mNbOutputFlux);
        mExpMatrixProduct.resize(mNbInputFlux + mNbOutputFlux);
        mExpMatrixProduct_ineq.resize(mNbInputFlux + mNbOutputFlux);

        ConverterSubModel::declareDefaultModelInterface();

        setOptimalSizeExpression("MaxPower");  // defines default expression should be used for OptimalSize computation and use in Economic analysis
        addIO("MaxPower", &mExpSizeMax, mPortINPUTFlux1->pFluxUnit());          /** Sizing W */
        setOptimalSizeUnit(mPortINPUTFlux1->pFluxUnit());  // defines default expression should be used for OptimalSize computation and use in Economic analysis

        addIO("INPUTFlux1", &mExpInput[0], mPortINPUTFlux1->pFluxUnit()); /** Computed input flow at default port PortINPUTFlux1 */
        addIO("OUTPUTFlux1", &mExpOutput[0], mPortOUTPUTFlux1->pFluxUnit()); /** Computed output flow at default port PortOUTPUTFlux1 */

        ConverterSubModel::declareInputFluxIOs(mPortINPUTFlux1);
        ConverterSubModel::declareOutputFluxIOs(mPortOUTPUTFlux1);
    }

    //----------------------------------------------------------------------------------------------------

    void declareModelParameters()
    {
        ConverterSubModel::declareDefaultModelParameters();

        //double
        addParameter("MaxPower", &mMaxPower, INFINITY_VAL);	/** Maximum output of OUTPUTFlux1 */
        //QString
        addParameter("MatrixA", &mMatrixA, "", true, true, "CSV file of the matrix A in the formula : A * [Input Output] = B  of size NbInput + NbOutput", "string", {"Base"});
        addParameter("MatrixB", &mMatrixB, "", true, true, "CSV file of the matrix B in the formula : A * [Input Output] = B  of size NbInput + NbOutput", "string", {});

        addParameter("MatrixC", &mMatrixC, "", mIsIneqCstr, mIsIneqCstr, "CSV file of the matrix C in the formula : C * [Input Output] <= D", "string", { "Base" });
        addParameter("MatrixD", &mMatrixD, "", mIsIneqCstr, mIsIneqCstr, "CSV file of the matrix D in the formula : C * [Input Output] <= D", "string", { "Base" });
    }

    void declareModelIndicators() {
        ConverterSubModel::declareDefaultModelIndicators(&mExportIndicators);
    }

    void initDefaultPorts()
    {
        mDefaultPorts.clear();
        //PortINPUTFlux1 - left
        QMap<QString, QString> portINPUTFlux1;
        portINPUTFlux1["Name"] = "PortL0"; //Needed only old versions
        portINPUTFlux1["Position"] = "left";
        portINPUTFlux1["CarrierType"] = ANY_TYPE();
        portINPUTFlux1["Direction"] = KCONS(); //INPUT
        portINPUTFlux1["Variable"] = "INPUTFlux1";
        mDefaultPorts.insert("PortINPUTFlux1", portINPUTFlux1); //ID, paramMap

        //PortOUTPUTFlux1 - right
        QMap<QString, QString> portOUTPUTFlux1;
        portOUTPUTFlux1["Name"] = "PortR0";
        portOUTPUTFlux1["Position"] = "right";
        portOUTPUTFlux1["CarrierType"] = ANY_TYPE();
        portOUTPUTFlux1["Direction"] = KPROD();
        portOUTPUTFlux1["Variable"] = "OUTPUTFlux1";
        mDefaultPorts.insert("PortOUTPUTFlux1", portOUTPUTFlux1);
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

    std::vector <MIPModeler::MIPExpression1D> mExpInputPart;
    std::vector <MIPModeler::MIPExpression1D> mExpOutputPart;

    std::vector <MIPModeler::MIPExpression1D> mExpMatrixProduct;
    std::vector <MIPModeler::MIPExpression1D> mExpMatrixProduct_ineq;

    MIPModeler::MIPExpression1D mExpInputTotalPower;
    MIPModeler::MIPExpression1D mExpOutputTotalPower;

    // model parameters

    double mMinTotalPower;
    double mMaxTotalPowerFraction;
    double mMaxPower;

    bool mIsIneqCstr;

    QString mMatrixA;
    QString mMatrixB;
    Eigen::MatrixXd mMatrixEigenA;
    Eigen::MatrixXd mMatrixEigenB;

    QList<QStringList> data_Inputs_A;
    QList<QStringList> data_Inputs_B;
    std::vector<std::vector<double>> coefficient_A;
    std::vector<std::vector<double>> coefficient_B;


    QString mMatrixC;
    QString mMatrixD;
    Eigen::MatrixXd mMatrixEigenC;
    Eigen::MatrixXd mMatrixEigenD;

    QList<QStringList> data_Inputs_C;
    QList<QStringList> data_Inputs_D;
    std::vector<std::vector<double>> coefficient_C;
    std::vector<std::vector<double>> coefficient_D;
};


#endif
