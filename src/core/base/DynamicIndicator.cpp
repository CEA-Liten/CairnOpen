/*
* \file		DynamicIndicator.cpp
* \brief	A class to compute user-defined indicators from existing indicators
* \version	1.0
* \author	Ali KASSEM
* \date		24/06/2024
*/

#include "DynamicIndicator.h"
#include "CairnUtils.h"

DynamicIndicator::DynamicIndicator(CairnObject* aParent, std::string aName, std::string aFormula, std::vector<std::string> aUDINames) : CairnObject(aParent),
mName(aName),
mFormula(aFormula),
mUDINames(aUDINames),
mRenamedFormula("")
{
    this->setObjectName(aName);
    updateExpression();
}

DynamicIndicator::~DynamicIndicator()
{
    for (auto& [key, value] : mVariableValueMap) {    
        if (value) delete value;
    }
    mVariableValueMap.clear();
}

std::string DynamicIndicator::getName() const { return mName; }
std::string DynamicIndicator::getFormula() const { return mFormula; }

std::map<std::string, std::string> DynamicIndicator::variableRenamingMap() const { return mVariableRenamingMap; }
std::map<std::string, double*> DynamicIndicator::variableValueMap() const { return mVariableValueMap; }

void DynamicIndicator::setName(const std::string& aName) 
{
    mName = aName;
}

void DynamicIndicator::setFormula(const std::string& aFormula)
{
    mFormula = aFormula;
    updateExpression();
}

void DynamicIndicator::renameVariables()
{
    //Initialize to formula
    mRenamedFormula = mFormula;

    //Rename variables
    int k = 1; //variables counter
    std::string word;
    std::stringstream ss(mFormula);
    while (std::getline(ss, word, '\"')) {
        bool isFound = mRenamedFormula.find('\"'+ word + '\"') != std::string::npos;
        if (isFound && word.size() >= 3 && word.find('.') < word.length()  // a better filter to lookup for variables?  
            || CairnUtils::contains(mUDINames, word) ) //User-defined Indicator
        {
            std::string var_k = 'x' + std::to_string(k); 
            mRenamedFormula.replace(mRenamedFormula.find('\"' + word + '\"'), word.length() + 2, var_k);
            mVariableRenamingMap[var_k] = (word);
            k++;
        }
    }     
}

void DynamicIndicator::compile() 
{
    //Initialize values to nan    
    for (auto& [key, value] : mVariableRenamingMap) {    
        mVariableValueMap[key] = new double;
        *mVariableValueMap[key] = double_NaN;
    }
 
    //Add variables    
    for (auto& [key, value] : mVariableValueMap) {    
        mSymbolTable.add_variable(key, *value);
    }
    
    //Add known constants: 
    mSymbolTable.add_constants();
    
    //Register expression (Formula)
    mExpression.register_symbol_table(mSymbolTable);

    //Parser expression 
    if(!mParser.compile(mRenamedFormula, mExpression)){
        //mExpression.release();
        //mSymbolTable.clear();        
        cError() << "Error while compiling the formula (" << mFormula << ") of dynamic indicator " << mName << ": \n" << mParser.error();
    }
}

void DynamicIndicator::updateExpression()
{
    renameVariables();
    compile();
}

double DynamicIndicator::compute() const { return mExpression.value(); } //might be nan