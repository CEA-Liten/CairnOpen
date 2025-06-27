/**
* \file		MultiConverter.cpp
* \brief	Multiconverter model with X inputs and Y outputs linked by a matrix
* \version	1.0
* \author	Thibaut Wissocq
* \date		05/2024
*/

#include "MultiConverter.h"
using namespace GS;
extern "C" MODELS_DECLSPEC QObject * createModel(QObject * aParent)
{
    return new MultiConverter(aParent);
}

MultiConverter::MultiConverter(QObject* aParent)
    : ConverterSubModel(aParent), 
    mPortINPUTFlux1(nullptr),
    mPortOUTPUTFlux1(nullptr)
{
    mVariablePortNumber = true;
}

MultiConverter::~MultiConverter() {}


void MultiConverter::buildModel() 
{
    if (mNbInputFlux > mNbInputPorts || mNbOutputFlux > mNbOutputPorts)
    {
        Cairn_Exception persee_error("Error: the number of NbInputFlux/NbOutputFlux of " + getCompoName() + " must be less than the number of Input/Output ports.", -1);
        throw persee_error;
    }

    // constraint on maximum Input power of component.
    setExpSizeMax(mMaxPower, "MaxPower");

    if (mAllocate)
    {
        Q_ASSERT(mNbInputFlux <= mNbInputPorts);
        Q_ASSERT(mNbOutputFlux <= mNbOutputPorts);
        
        for (int i = 0; i < mNbOutputFlux; i++)
        {
            mExpOutput[i] = MIPModeler::MIPExpression1D(mHorizon);
            mExpInputPart[i] = MIPModeler::MIPExpression1D(mHorizon);
            mExpOutputPart[i] = MIPModeler::MIPExpression1D(mHorizon);
        }
        for (int i = 0; i < mNbInputFlux; i++)
        {
            mExpInput[i] = MIPModeler::MIPExpression1D(mHorizon);
        }
        for (int i = 0; i < mNbInputFlux + mNbOutputFlux; i++) 
        {
            mExpMatrixProduct[i] = MIPModeler::MIPExpression1D(mHorizon);
            mExpMatrixProduct_ineq[i] = MIPModeler::MIPExpression1D(mHorizon);
        }
    }
    else
    {
       // closeExpression1D(mExpOutputTotalPower);
        for (int i = 0; i < mNbOutputFlux; i++)
        {
            closeExpression1D(mExpOutput[i]);
            closeExpression1D(mExpInputPart[i]);
            closeExpression1D(mExpOutputPart[i]);
        }
        for (int i = 0; i < mNbInputFlux; i++)
        {
            closeExpression1D(mExpInput[i]);
        }
        for (int i = 0; i < mNbInputFlux + mNbOutputFlux; i++)
        {
            closeExpression1D(mExpMatrixProduct[i]);
            closeExpression1D(mExpMatrixProduct_ineq[i]);
        }
        mInput.clear();
        mOutput.clear();
    }


    // precomputation
    std::vector <double> maxInput;
    std::vector<std::vector<double>> coefficient_A;
    std::vector<std::vector<double>> coefficient_B;
    std::vector<std::vector<double>> coefficient_C;
    std::vector<std::vector<double>> coefficient_D;

    // TODO : check inside fonctions
    //TODO : assert good size of matrixes depending on NbInputFlux and NbOutputFlux
    QList<QStringList> data_Inputs_A = GS::readFromCsvFile(mMatrixA, ";");
    if (data_Inputs_A.empty()) {
        // Initialize coefficient_A with a null matrix of the good size
        coefficient_A.resize(mNbOutputFlux + mNbInputFlux, std::vector<double>(mNbOutputFlux + mNbInputFlux, 0.0));
    }
    else {
        coefficient_A = GS::getDataMatrix(data_Inputs_A, 0);
    }
    Eigen::MatrixXd mMatrixEigenA = convertToEigen(coefficient_A);

    QList<QStringList> data_Inputs_B = GS::readFromCsvFile(mMatrixB, ";");
    if (data_Inputs_B.empty()) {
        // Initialize coefficient_B with a null matrix of the good size
        coefficient_B.resize(mNbOutputFlux + mNbInputFlux, std::vector<double>(1, 0.0));
    }
    else {
        coefficient_B = GS::getDataMatrix(data_Inputs_B, 0);
    }
    Eigen::MatrixXd mMatrixEigenB = convertToEigen(coefficient_B);



    QList<QStringList> data_Inputs_C = GS::readFromCsvFile(mMatrixC, ";");
    if (data_Inputs_C.empty()) {
        // Initialize coefficient_C with a null matrix of the good size
        coefficient_C.resize(mNbOutputFlux + mNbInputFlux, std::vector<double>(mNbOutputFlux + mNbInputFlux, 0.0));
    }
    else {
        coefficient_C = GS::getDataMatrix(data_Inputs_C, 0);
    }
    Eigen::MatrixXd mMatrixEigenC = convertToEigen(coefficient_C);

    QList<QStringList> data_Inputs_D = GS::readFromCsvFile(mMatrixD, ";");    
    if (data_Inputs_D.empty()) {
        // Initialize coefficient_D with a null matrix of the good size
        coefficient_D.resize(mNbOutputFlux + mNbInputFlux, std::vector<double>(1, 0.0));
    }
    else {
        coefficient_D = GS::getDataMatrix(data_Inputs_D, 0);
    }
    Eigen::MatrixXd mMatrixEigenD = convertToEigen(coefficient_D);

    checkConsistency();
 
    //A,B,C,D is the mMatrixEigenA decomposed by blocks [A B ; C D]
    Eigen::MatrixXd A = mMatrixEigenA.topLeftCorner(mNbInputFlux, mNbInputFlux);
    Eigen::MatrixXd B = mMatrixEigenA.topRightCorner(mNbInputFlux, mMatrixEigenA.cols() - mNbInputFlux);
    Eigen::MatrixXd C = mMatrixEigenA.bottomLeftCorner(mNbOutputFlux, mNbInputFlux);
    Eigen::MatrixXd D = mMatrixEigenA.bottomRightCorner(mNbOutputFlux, mNbOutputFlux);
    double norm_A = norm1(mMatrixEigenA);
    double norm_B = norm1(B);

    for (int i = 0; i < mNbInputFlux; i++)
        maxInput.push_back(norm_B/ smallestNonZeroCoefficient(A)*fabs(mMaxPower));

    std::vector <double> maxOutput;
    for (int i = 0; i < mNbOutputFlux; i++)
        maxOutput.push_back(norm_A / smallestNonZeroCoefficient(mMatrixEigenA) * fabs(mMaxPower));


    // effective output production on each port (thermal / electrical) : 1D variable
    //TODO faire clear avant pour gerer les multiples appels
    for (int i = 0; i < mNbInputFlux; i++)
    {
        mInput.push_back(MIPModeler::MIPVariable1D(mHorizon, 0., maxInput[i])); 
        addVariable(mInput[i], "Fluxin"+std::to_string(i));
    }

    for (int j = 0; j < mNbOutputFlux; j++)
    {
        mOutput.push_back(MIPModeler::MIPVariable1D(mHorizon, 0., maxOutput[j]));
        addVariable(mOutput[j], "Fluxout_"+std::to_string(j));
    }

    for (int i = 0; i < mNbInputFlux; i++)
    {
        for (uint64_t t = 0; t < mHorizon; ++t)
        {
            mExpInput[i][t] += mInput[i](t);
        }
    }
    for (int i = 0; i < mNbOutputFlux; i++)
    {
        for (uint64_t t = 0; t < mHorizon; ++t)
        {
            mExpOutput[i][t] += mOutput[i](t);
        }
    }   

    // \sum_j a_ij x_j + \sum_j a_ij y_j = 0
    for (int i = 0; i < mNbOutputFlux + mNbInputFlux; i++) {
        for (int x = 0; x < mNbInputFlux; x++)
        {
            if (coefficient_A[i][x] != 0)
            {
                for (uint64_t t = 0; t < mHorizon; ++t)
                {
                    mExpMatrixProduct[i][t] += coefficient_A[i][x] * mExpInput[x][t];// \sum_{k} b_jk* flow_out[k]
                    mExpMatrixProduct_ineq[i][t] += coefficient_C[i][x] * mExpInput[x][t];// \sum_{k} b_jk* flow_out[k]

                }
            }
        }        
        for (int y = mNbInputFlux; y < mNbOutputFlux + mNbInputFlux; y++)
        {
            if (coefficient_A[i][y] != 0)
            {
                for (uint64_t t = 0; t < mHorizon; ++t)
                {
                    mExpMatrixProduct[i][t] += coefficient_A[i][y] * mExpOutput[y - mNbInputFlux][t];
                    mExpMatrixProduct_ineq[i][t] += coefficient_C[i][y] * mExpOutput[y - mNbInputFlux][t];
                }
            }
        }    
    }

    for (int i = 0; i < mNbOutputFlux + mNbInputFlux; i++) {
        for (uint64_t t = 0; t < mHorizon; ++t)
        {
            addConstraint(mExpMatrixProduct[i][t] - coefficient_B[i][0] == 0, "cEfficiency_" + std::to_string(i), t);
            if (mIsIneqCstr) {
                addConstraint(mExpMatrixProduct_ineq[i][t] <= coefficient_D[i][0], "cInequality_" + std::to_string(i), t);
            }
        }
    }

    /** Add Sizing */
    if (mMaxPower < 0) {
        for (uint64_t t = 0; t < mHorizon; t++)
        {
            addConstraint(mExpOutput[0][t] <= mVarSizeMax * mConverterUse[t], "cPowMax_" + std::to_string(0), t);
        }

    }    
    
    /** Compute all expressions */
    computeAllContribution();

    mAllocate = false;
}

void MultiConverter::computeEconomicalContribution()
{
    TechnicalSubModel::computeEconomicalContribution();
}
//----------------------------------------------------------------------------
void MultiConverter::computeAllIndicators(const double* optSol)
{
    ConverterSubModel::computeDefaultIndicators(optSol);
}

void MultiConverter::checkConsistencyMatrix(QString filePath)
{
    QList<QStringList> data_Inputs_A = GS::readFromCsvFile(filePath, ";");
    std::vector<std::vector<double>> coefficient_A = GS::getDataMatrix(data_Inputs_A, 0);
    mMatrixEigenA = convertToEigen(coefficient_A);
    if (mMatrixEigenA.rows() != mNbInputFlux + mNbOutputFlux)
    {
        Cairn_Exception cairn_error(getCompoName() + " : Please check size of conversion matrix \"MatrixA\", it should be equal to NbInputFlux + NbOutputFlux ", -1);
        throw cairn_error;
    }

}

int MultiConverter::checkConsistency()
{
    int ier = TechnicalSubModel::checkConsistency();
    QList<QStringList> data_Inputs_A = GS::readFromCsvFile(mMatrixA, ";");
    std::vector<std::vector<double>> coefficient_A = GS::getDataMatrix(data_Inputs_A, 0);
    mMatrixEigenA = convertToEigen(coefficient_A);
    
    if (mMatrixEigenA.rows() != mNbInputFlux + mNbOutputFlux)
    {
        Cairn_Exception cairn_error(getCompoName() + " : Please check size of conversion matrix \"MatrixA\", it should be equal to NbInputFlux + NbOutputFlux ", -1);
        throw cairn_error;
    }
    bool isNotZeroColumn = mMatrixEigenA.col(mNbInputFlux).any();

    if (!isNotZeroColumn)
    {
        Cairn_Exception cairn_error(getCompoName() + " : Please check conversion matrix \"MatrixA\", as the column corresponding to OUTPUTFlux1 is zero ", -1);
        throw cairn_error;
    }
    return ier;
}
