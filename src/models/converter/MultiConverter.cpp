/**
* \file		MultiConverter.cpp
* \brief	Multiconverter model with X inputs and Y outputs linked by a matrix
* \version	1.0
* \author	Thibaut Wissocq
* \date		05/2024
*/

#include "MultiConverter.h"
using namespace GS;
extern "C" MODELS_DECLSPEC CairnObject * createModel(CairnObject * aParent)
{
    return new MultiConverter(aParent);
}

MultiConverter::MultiConverter(CairnObject* aParent)
    : ConverterSubModel(aParent), 
    mPortINPUTFlux1(nullptr),
    mPortOUTPUTFlux1(nullptr)
{
    mVariablePortNumber = true;
}

MultiConverter::~MultiConverter() {}

void MultiConverter::setTimeData() {
    ConverterSubModel::setTimeData();
    mInput.clear();
    mOutput.clear();
}

void MultiConverter::computeInitialData()
{
    setMinValue(mMinSize);
    setMaxValue(mMaxPower);
}

void MultiConverter::computeModelContribution()
{
    Eigen::MatrixXd matrixEigenA = convertToEigen(mCoefficient_A);
 
    //A,B,C,D is the matrixEigenA decomposed by blocks [A B ; C D]
    Eigen::MatrixXd A = matrixEigenA.topLeftCorner(mNbInputFlux, mNbInputFlux);
    Eigen::MatrixXd B = matrixEigenA.topRightCorner(mNbInputFlux, matrixEigenA.cols() - mNbInputFlux);
    Eigen::MatrixXd C = matrixEigenA.bottomLeftCorner(mNbOutputFlux, mNbInputFlux);
    Eigen::MatrixXd D = matrixEigenA.bottomRightCorner(mNbOutputFlux, mNbOutputFlux);

    double norm_A = norm1(matrixEigenA);
    double norm_B = norm1(B);

    std::vector <double> maxInput;
    for (int i = 0; i < mNbInputFlux; i++) {
        maxInput.push_back(norm_B / smallestNonZeroCoefficient(A) * fabs(mMaxPower));
    }

    std::vector <double> maxOutput;
    for (int i = 0; i < mNbOutputFlux; i++) {
        maxOutput.push_back(norm_A / smallestNonZeroCoefficient(matrixEigenA) * fabs(mMaxPower));
    }

    // effective output production on each port (thermal / electrical) : 1D variable
    mInput.resize(mNbInputFlux);
    for (int i = 0; i < mNbInputFlux; i++) {
        addVariable(mInput[i], "Fluxin"+std::to_string(i), 0., maxInput[i]);
    }

    mOutput.resize(mNbOutputFlux);
    for (int j = 0; j < mNbOutputFlux; j++) {
        addVariable(mOutput[j], "Fluxout_"+std::to_string(j), 0., maxOutput[j]);
    }

    for (int i = 0; i < mNbInputFlux; i++)  {
        for (uint64_t t = 0; t < mHorizon; ++t) {
            mExpInput[i][t] += mInput[i](t);
        }
    }

    for (int i = 0; i < mNbOutputFlux; i++) {
        for (uint64_t t = 0; t < mHorizon; ++t) {
            mExpOutput[i][t] += mOutput[i](t);
        }
    }   

    // \sum_j a_ij x_j + \sum_j a_ij y_j = 0
    for (int i = 0; i < mNbOutputFlux + mNbInputFlux; i++) 
    {
        for (int x = 0; x < mNbInputFlux; x++) {
            if (mCoefficient_A[i][x] != 0) {
                for (uint64_t t = 0; t < mHorizon; ++t) {
                    mExpMatrixProduct[i][t] += mCoefficient_A[i][x] * mExpInput[x][t];// \sum_{k} b_jk* flow_out[k]
                    if (mIsIneqCstr) {
                        mExpMatrixProduct_ineq[i][t] += mCoefficient_C[i][x] * mExpInput[x][t];// \sum_{k} b_jk* flow_out[k]
                    }
                }
            }
        }        

        for (int y = mNbInputFlux; y < mNbOutputFlux + mNbInputFlux; y++) {
            if (mCoefficient_A[i][y] != 0) {
                for (uint64_t t = 0; t < mHorizon; ++t) {
                    mExpMatrixProduct[i][t] += mCoefficient_A[i][y] * mExpOutput[y - mNbInputFlux][t];
                    if (mIsIneqCstr) {
                        mExpMatrixProduct_ineq[i][t] += mCoefficient_C[i][y] * mExpOutput[y - mNbInputFlux][t];
                    }
                }
            }
        }    
    } 

    for (int i = 0; i < mNbOutputFlux + mNbInputFlux; i++) {
        for (uint64_t t = 0; t < mHorizon; ++t) {
            addConstraint(mExpMatrixProduct[i][t] - mCoefficient_B[i] == 0, "cEfficiency_" + std::to_string(i), t);
            if (mIsIneqCstr) {
                addConstraint(mExpMatrixProduct_ineq[i][t] <= mCoefficient_D[i], "cInequality_" + std::to_string(i), t);
            }
        }
    }

    /** Add Sizing */
    if (mMaxPower < 0) {
        for (uint64_t t = 0; t < mHorizon; t++) {
            addConstraint(mExpOutput[0][t] <= mVarSizeMax * mComponentAvailabilityTS[t], "cPowMax_" + std::to_string(0), t);
        }
    }    
}

void MultiConverter::readAndVerifyMatrixA(const std::string& filename, std::vector<std::vector<double>>& matrix, const bool& isMatrixC)
{
    std:string matrixName = "MatrixA";
    if(isMatrixC) matrixName = "MatrixC";

    //verify that the file exists
    std::string absoluteFileName = getAbsoluteFileName(filename);
    if (!fs::exists(absoluteFileName)) {
        Cairn_Exception cairn_error(Name() + " - the file of matrix \"" + matrixName + "\" doesn't exist:" 
            + absoluteFileName, -1);
        throw cairn_error;
    }

    //read the matrix from the file
    std::vector<std::vector<std::string>> data_Inputs_A = CairnUtils::readFromCsvFile(absoluteFileName, ";");
    if (!data_Inputs_A.empty()) {
        matrix = GS::getDataMatrix(data_Inputs_A, 0);
    }

    //verify that the dimensions of the matrix are (mNbInputFlux + mNbOutputFlux) x (mNbInputFlux + mNbOutputFlux)
    Eigen::MatrixXd matrixEigenA = convertToEigen(mCoefficient_A);
    
    if (matrixEigenA.rows() != mNbInputFlux + mNbOutputFlux) {
        Cairn_Exception cairn_error(Name() + ": please verify the number of rows of the conversion matrix \"" + matrixName 
            + "\". The number of rows should be NbInputFlux + NbOutputFlux = " + std::to_string(mNbInputFlux + mNbOutputFlux)
            + ". File: " + absoluteFileName, -1);
        throw cairn_error;
    }

    if (matrixEigenA.cols() != mNbInputFlux + mNbOutputFlux) {
        Cairn_Exception cairn_error(Name() + ": please verify the number of columns of the conversion matrix \"" + matrixName
            + "\". The number of columns should be NbInputFlux + NbOutputFlux = " + std::to_string(mNbInputFlux + mNbOutputFlux)
            + ". File: " + absoluteFileName, -1);
        throw cairn_error;
    }

    //verify that the column mNbInputFlux + 1 (first output column i.e OUTPUTFlux1) is not a vector of Zeros
    bool isNotZeroColumn = matrixEigenA.col(mNbInputFlux).any();

    if (!isNotZeroColumn) {
        Cairn_Exception cairn_error(Name() + ": please verify the conversion matrix \"" + matrixName
            + "\" as the entier column corresponding to OUTPUTFlux1 is zero. File: " + absoluteFileName, -1);
        throw cairn_error;
    }
}

void MultiConverter::readAndVerifyVectorB(const std::string& filename, std::vector<double>& vector, const bool& isVectorD)
{
    /*
    * It is not mandatory to provide files for matrices B and C.
    * If no file is provided, then use a vector of Zeros (vector is already initialized to 0s before calling readAndVerifyVectorB).
    * If a file is provided and it contains only one cell say [k], 
    * then use a vector N*[k] where N = mNbOutputFlux + mNbInputFlux
    */

    std:string vectorName = "MatrixB"; // parameter name is MatrixB 
    if (isVectorD) vectorName = "MatrixD";

    std::string absoluteFileName = getAbsoluteFileName(filename);

    //if (!fs::exists(absoluteFileName)) {
    //    Cairn_Exception cairn_error(Name() + " - The file of matrix \"" + vectorName + "\" doesn't exist:"
    //        + absoluteFileName, -1);
    //    throw cairn_error;
    //}

    //if file exist read values
    if (fs::exists(absoluteFileName)) {
        std::vector<std::vector<std::string>> data_Inputs_B = CairnUtils::readFromCsvFile(absoluteFileName, ";");
        if (!data_Inputs_B.empty()) {
            vector = GS::getDataArray(data_Inputs_B, 0, 0); //take only the first column
        }
    }
    else {
        cWarning() << Name() + ": the file " + absoluteFileName + " of matrix \"" + vectorName + "\" has not been found. " 
            + "A matrix of Zeros will be used.";
    }

    //verify that the size of the vector is mNbInputFlux + mNbOutputFlux
    if (vector.size() != mNbInputFlux + mNbOutputFlux) {
        if (vector.size() == 1) {
            //resize the vector using equal values
            double k = vector[0];
            vector.resize(mNbOutputFlux + mNbInputFlux, k);
            cWarning() << Name() + ": the file " + absoluteFileName + " of matrix \"" + vectorName + "\" contains only one value: " + std::to_string(k)
                + ". All the values of the matrix will be set to " + std::to_string(k);
        }
        else {
            Cairn_Exception cairn_error(Name() + ": please check the dimensions of matrix \"" + vectorName
                + "\". The dimensions should be NbInputFlux + NbOutputFlux (" + std::to_string(mNbInputFlux + mNbOutputFlux)
                + ") rows and 1 column. File: " + absoluteFileName, -1);
            throw cairn_error;
        }
    }
}


int MultiConverter::checkConsistency()
{
    if (mNbInputFlux > mNbInputPorts || mNbOutputFlux > mNbOutputPorts) {
        Cairn_Exception persee_error("Error: the number of NbInputFlux/NbOutputFlux of " + Name() 
            + " must be less than the number of Input/Output ports.", -1);
        throw persee_error;
    }

    //initialize matrices
    mCoefficient_A.resize(mNbOutputFlux + mNbInputFlux, std::vector<double>(mNbOutputFlux + mNbInputFlux, 0.0));
    mCoefficient_B.resize(mNbOutputFlux + mNbInputFlux, 0.0);

    mCoefficient_C.resize(mNbOutputFlux + mNbInputFlux, std::vector<double>(mNbOutputFlux + mNbInputFlux, 0.0));
    mCoefficient_D.resize(mNbOutputFlux + mNbInputFlux, 0.0);

    //Read matrices and check their consistency 
    readAndVerifyMatrixA(mMatrixA, mCoefficient_A, false);
    readAndVerifyVectorB(mMatrixB, mCoefficient_B, false);
    
    if (mIsIneqCstr) {
        /* If add inequality constraint is true */
        readAndVerifyMatrixA(mMatrixC, mCoefficient_C, true);
        readAndVerifyVectorB(mMatrixD, mCoefficient_D, false);
    }

    int ier = TechnicalSubModel::checkConsistency();
    return ier;
}


void MultiConverter::computeAllIndicators(const double* optSol)
{
    ConverterSubModel::computeDefaultIndicators(optSol);
}