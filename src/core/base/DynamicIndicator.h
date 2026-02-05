/*
* \file		DynamicIndicator.h
* \brief	A class to compute user-defined indicators from existing indicators
* \version	1.0
* \author	Ali KASSEM
* \date		24/06/2024
* 
 ******************************************************************
 *                                                                *
 * This file uses the C++ Mathematical Expression Toolkit Library *
 * by Arash Partow (1999-2024) under the MIT License:             *
 * https://www.opensource.org/licenses/MIT                        *
 *                                                                *
 * URL: https://www.partow.net/programming/exprtk/index.html      *
 * Email: arash@partow.net                                        *
 *                                                                *
 ******************************************************************
*/

#ifndef DynamicIndicator_H
#define DynamicIndicator_H
class DynamicIndicator;

#include "CairnCore_global.h"
#include "Cairn_Exception.h"

#include <string> 
#include <sstream>
#include "exprtk.hpp"

#define double_NaN double(std::sqrt(-1))

/**
 * DynamicIndicator is the base class for user defined indicators
 */

class CAIRNCORESHARED_EXPORT DynamicIndicator : public CairnObject
{
    
public:

    DynamicIndicator(CairnObject* aParent = nullptr, std::string aName = std::string(""), std::string aFormula = std::string(""), std::vector<std::string> aUDINames={});
    ~DynamicIndicator();

    std::string getName() const;
    std::string getFormula() const;

    std::map<std::string, std::string> variableRenamingMap() const;
    std::map<std::string, double*> variableValueMap() const;

    void setName(const std::string& aName);
    void setFormula(const std::string& aFormula);

    void renameVariables();
    void updateExpression();
    void compile(); //generate and compile exprtk::expression  
    double compute() const;

protected:
    std::string mName;    // Dynamic Indicator name
    std::string mFormula; // Dynamic Indicator formula in function of TecEco and component expressions
    std::string mRenamedFormula; // The Indicator formula renamed to use x1, ..., xn as variable names; This is to not confuse the parser with long names that may contain special characters.
    std::map<std::string, std::string> mVariableRenamingMap{}; // <x1, TecEco.NetOpexDiscounted>
    std::map<std::string, double*> mVariableValueMap{};    // <x1, 10.1> 

    std::vector<std::string> mUDINames; //list of user-defined indicator names that are allowed to be used as variables

    //Parser (double can be replace by a general type T; if needed)
    typedef exprtk::symbol_table<double> t_symbol_table;
    typedef exprtk::expression<double>   t_expression;
    typedef exprtk::parser<double>       t_parser;

    t_symbol_table mSymbolTable;
    t_expression mExpression;
    t_parser mParser;
};
#endif // DynamicIndicator_H