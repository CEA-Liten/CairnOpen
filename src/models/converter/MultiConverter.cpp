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

    readUpperBounds();
}

void MultiConverter::readUpperBounds()
{
    if (mUpperBoundsFile.empty())
        return;

    const std::string absoluteFileName = getAbsoluteFileName(mUpperBoundsFile);

    // ---------------------------------------------------------------------
    // Load file if it exists
    // ---------------------------------------------------------------------
    if (!fs::exists(absoluteFileName)) {
        cWarning() << Name() << ": upper-bounds file \"" << absoluteFileName
            << "\" not found. MaxPower * weight will be used as an upper-bound only on the first output. " 
            << " No upper-bounds will be set on inputs and other outputs.";
        return;
    }

    const auto csvData = CairnUtils::readFromCsvFile(absoluteFileName, ";");
    if (csvData.empty()) {
        cWarning() << Name() << ": upper-bounds file \"" << absoluteFileName
            << "\" is empty. MaxPower * weight will be used as an upper-bound only on the first output. " 
            << " No upper-bounds will be set on inputs and other outputs.";
        return;
    }

    // Extract first column only
    mUpperBounds = GS::getDataArray(csvData, 0, 0);

    // ---------------------------------------------------------------------
    // Validate size
    // ---------------------------------------------------------------------
    const std::size_t expectedSize = mNbInputFlux + mNbOutputFlux;

    if (mUpperBounds.size() == expectedSize)
        return; // all good

    // ---------------------------------------------------------------------
    // Special case: file contains a single value -> use it for all
    // ---------------------------------------------------------------------
    if (mUpperBounds.size() == 1) {
        const double k = mUpperBounds[0];
        mUpperBounds.assign(expectedSize, k);

        cInfo() << Name() << ": upper-bounds file \"" << absoluteFileName
            << "\" contains a single value (" << k
            << "). Using this value to all "
            << expectedSize << " bounds.";
        return;
    }

    // ---------------------------------------------------------------------
    // Invalid size -> error
    // ---------------------------------------------------------------------
    cError() << Name() + ": invalid upper-bounds file \"" + absoluteFileName +
        "\". Expected " + std::to_string(expectedSize) +
        " values (NbInputFlux + NbOutputFlux), but found " +
        std::to_string(mUpperBounds.size());
}

//double MultiConverter::smallestNonZeroCoefficient(const Eigen::MatrixXd& matrix) {
//    double minCoeff = std::numeric_limits<double>::infinity();
//
//    for (int i = 0; i < matrix.rows(); ++i) {
//        for (int j = 0; j < matrix.cols(); ++j) {
//            double value = matrix(i, j);
//            if (value != 0 && std::abs(value) < minCoeff) {
//                minCoeff = std::abs(value);
//            }
//        }
//    }
//
//    if (std::isinf(minCoeff)) {
//        throw Cairn_Exception("Error: the matrix must not be Zero", -1);
//    }
//
//    return minCoeff;
//}

void MultiConverter::computeModelContribution()
{
    Eigen::MatrixXd matrixEigenA = convertToEigen(mCoefficient_A);

    //A,B,C,D is the matrixEigenA decomposed by blocks [A B ; C D]
    Eigen::MatrixXd A = matrixEigenA.topLeftCorner(mNbInputFlux, mNbInputFlux);
    Eigen::MatrixXd B = matrixEigenA.topRightCorner(mNbInputFlux, matrixEigenA.cols() - mNbInputFlux);
    Eigen::MatrixXd C = matrixEigenA.bottomLeftCorner(mNbOutputFlux, mNbInputFlux);
    Eigen::MatrixXd D = matrixEigenA.bottomRightCorner(mNbOutputFlux, mNbOutputFlux);


    // effective output production on each port (thermal / electrical) : 1D variable

    //const double norm_B = norm1(B);
    //const double smallestNonZeroCoeff_A = smallestNonZeroCoefficient(A);
    //const double maxInput = (norm_B / smallestNonZeroCoeff_A) * fabs(mMaxPower);

    //const double norm_A = norm1(matrixEigenA);
    //const double smallestNonZeroCoeff_EigenA = smallestNonZeroCoefficient(matrixEigenA);
    //const double maxOutput = (norm_A / smallestNonZeroCoeff_EigenA) * fabs(mMaxPower);

    mInput.resize(mNbInputFlux);
    mOutput.resize(mNbOutputFlux);

    if (mUpperBounds.empty()) {
        // inputs: no upper bound
        for (int i = 0; i < mNbInputFlux; i++) {
            addVariable(mInput[i], "Fluxin_" + std::to_string(i), 0.0);
        }
        // outputs
        for (int j = 0; j < mNbOutputFlux; j++) {
            if(j == 0) // first output: upper bound = getMaxBound()
                addVariable(mOutput[j], "Fluxout_" + std::to_string(j), 0.0, getMaxBound());
            else //output j > 0: upper bound = getMaxBound()
                addVariable(mOutput[j], "Fluxout_" + std::to_string(j), 0.0);
        }
    }
    else {
        for (int i = 0; i < mNbInputFlux; i++) {
            addVariable(mInput[i], "Fluxin_" + std::to_string(i), 0.0, std::fabs(mMaxPower) * mUpperBounds[i]);
        }

        for (int j = 0; j < mNbOutputFlux; j++) {
            addVariable(mOutput[j], "Fluxout_" + std::to_string(j), 0.0, std::fabs(mMaxPower) * mUpperBounds[mNbInputFlux + j]);
        }
    }

    for (uint64_t t = 0; t < mHorizon; ++t) {
        for (int i = 0; i < mNbInputFlux; i++) {
            mExpInput[i][t] += mInput[i](t);
        }

        for (int i = 0; i < mNbOutputFlux; i++) {
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
    for (uint64_t t = 0; t < mHorizon; t++) {
        addConstraint(mExpOutput[0][t] <= mExpSizeMax * mComponentAvailabilityTS[t], "cPowMax_" + std::to_string(0), t);
    }    
}

int MultiConverter::readAndVerifyMatrixA(const std::string& filename, std::vector<std::vector<double>>& matrix, const bool& isMatrixC)
{
    std:string matrixName = "MatrixA";
    if(isMatrixC) matrixName = "MatrixC";

    //verify that the file exists
    std::string absoluteFileName = getAbsoluteFileName(filename);
    if (!fs::exists(absoluteFileName)) {
        cError() << Name() + " - the file of matrix \"" + matrixName + "\" doesn't exist:"
            + absoluteFileName;
    }

    //read the matrix from the file
    std::vector<std::vector<std::string>> data_Inputs_A = CairnUtils::readFromCsvFile(absoluteFileName, ";");
    if (!data_Inputs_A.empty()) {
        matrix = GS::getDataMatrix(data_Inputs_A, 0);
    }

    //verify that the dimensions of the matrix are (mNbInputFlux + mNbOutputFlux) x (mNbInputFlux + mNbOutputFlux)
    Eigen::MatrixXd matrixEigenA = convertToEigen(mCoefficient_A);
    
    if (matrixEigenA.rows() != mNbInputFlux + mNbOutputFlux) {
        cError() << Name() + ": please verify the number of rows of the conversion matrix \"" + matrixName
            + "\". The number of rows should be NbInputFlux + NbOutputFlux = " + std::to_string(mNbInputFlux + mNbOutputFlux)
            + ". File: " + absoluteFileName;
    }

    if (matrixEigenA.cols() != mNbInputFlux + mNbOutputFlux) {
        cError() << Name() + ": please verify the number of columns of the conversion matrix \"" + matrixName
            + "\". The number of columns should be NbInputFlux + NbOutputFlux = " + std::to_string(mNbInputFlux + mNbOutputFlux)
            + ". File: " + absoluteFileName;
    }

    //verify that the column mNbInputFlux + 1 (first output column i.e OUTPUTFlux1) is not a vector of Zeros
    bool isNotZeroColumn = matrixEigenA.col(mNbInputFlux).any();

    if (!isNotZeroColumn) {
        cError() << Name() + ": please verify the conversion matrix \"" + matrixName
            + "\" as the entier column corresponding to OUTPUTFlux1 is zero. File: " + absoluteFileName;
    }

    return 0;
}

int MultiConverter::readAndVerifyVectorB(const std::string& filename, std::vector<double>& aVector, const bool& isVectorD)
{
    /*
    * It is not mandatory to provide files for matrices B and C.
    * If no file is provided, then use a vector of Zeros (vector is already initialized to 0s before calling readAndVerifyVectorB).
    * If a file is provided and it contains only one cell say [k], 
    * then use a vector N*[k] where N = mNbOutputFlux + mNbInputFlux
    */

    std:string matrixName = "MatrixB"; // parameter name is MatrixB 
    if (isVectorD) matrixName = "MatrixD";

    std::string absoluteFileName = getAbsoluteFileName(filename);

    //if file exist read values
    if (fs::exists(absoluteFileName)) {
        std::vector<std::vector<std::string>> data_Inputs_B = CairnUtils::readFromCsvFile(absoluteFileName, ";");
        if (!data_Inputs_B.empty()) {
            aVector = GS::getDataArray(data_Inputs_B, 0, 0); //take only the first column
        }
    }
    else {
        cWarning() << Name() + ": the file " + absoluteFileName + " of matrix \"" + matrixName + "\" has not been found. "
            + "A matrix of Zeros will be used.";
    }

    //verify that the size of the vector is mNbInputFlux + mNbOutputFlux
    if (aVector.size() != mNbInputFlux + mNbOutputFlux) {
        if (aVector.size() == 1) {
            //resize the vector using equal values
            double k = aVector[0];
            aVector.resize(mNbOutputFlux + mNbInputFlux, k);
            cInfo() << Name() + ": the file " + absoluteFileName + " of matrix \"" + matrixName + "\" contains only one value: " + std::to_string(k)
                + ". All the values of the matrix will be set to " + std::to_string(k);
        }
        else {
            cError() << Name() + ": please check the dimensions of matrix \"" + matrixName
                + "\". The dimensions should be NbInputFlux + NbOutputFlux (" + std::to_string(mNbInputFlux + mNbOutputFlux)
                + ") rows and 1 column. File: " + absoluteFileName;
            return -1;
        }
    }

    return 0;
}

void MultiConverter::computeEconomicalContribution()
{
    TechnicalSubModel::computeEconomicalContribution();
}

int MultiConverter::checkConsistency()
{
    int ier = 0;

    if (mNbInputFlux > mNbInputPorts || mNbOutputFlux > mNbOutputPorts) {
        cError() << "Error: the number of NbInputFlux/NbOutputFlux of " + Name() 
            + " must be less than the number of Input/Output ports.";

        ier = -1;
    }

    //initialize matrices
    mCoefficient_A.resize(mNbOutputFlux + mNbInputFlux, std::vector<double>(mNbOutputFlux + mNbInputFlux, 0.0));
    mCoefficient_B.resize(mNbOutputFlux + mNbInputFlux, 0.0);

    mCoefficient_C.resize(mNbOutputFlux + mNbInputFlux, std::vector<double>(mNbOutputFlux + mNbInputFlux, 0.0));
    mCoefficient_D.resize(mNbOutputFlux + mNbInputFlux, 0.0);

    //Read matrices and check their consistency 
    if (readAndVerifyMatrixA(mMatrixA, mCoefficient_A, false) < 0) 
    {
        ier = -1;
    };

    if (readAndVerifyVectorB(mMatrixB, mCoefficient_B, false)) 
    {
        ier = -1;
    }
    
    if (mIsIneqCstr) {
        /* If add inequality constraint is true */
        if (readAndVerifyMatrixA(mMatrixC, mCoefficient_C, true))
        {
            ier = -1;
        }

        if (readAndVerifyVectorB(mMatrixD, mCoefficient_D, false))
        {
            ier = -1;
        }
    }

    if (TechnicalSubModel::checkConsistency() < 0)
    {
        ier = -1;
    };

    return ier;
}


void MultiConverter::computeAllIndicators(const double* optSol)
{
    ConverterSubModel::computeAllIndicators(optSol);
}