
#include <QDebug>
#include "DynamicIndicator.h"

DynamicIndicator::DynamicIndicator(QObject* aParent, QString aName, QString aFormula, QStringList aUDINames) : QObject(aParent),
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
    QMapIterator<std::string, double*> vIter(mVariableValueMap);
    while (vIter.hasNext())
    {
        vIter.next();
        double* var = vIter.value();
        if (var) delete var;
    }
    mVariableValueMap.clear();
}

QString DynamicIndicator::getName() const { return mName; }
QString DynamicIndicator::getFormula() const { return mFormula; }

QMap<std::string, QString> DynamicIndicator::variableRenamingMap() const { return mVariableRenamingMap; }
QMap<std::string, double*> DynamicIndicator::variableValueMap() const { return mVariableValueMap; }

void DynamicIndicator::setName(const QString& aName) 
{
    mName = aName;
}

void DynamicIndicator::setFormula(const QString& aFormula)
{
    mFormula = aFormula;
    updateExpression();
}

void DynamicIndicator::renameVariables()
{
    //Initialize to formula
    mRenamedFormula = mFormula.toStdString();

    //Rename variables
    int k = 1; //variables counter
    std::string word;
    std::stringstream ss(mFormula.toStdString());
    while (std::getline(ss, word, '\"')) {
        bool isFound = mRenamedFormula.find('\"'+ word + '\"') != std::string::npos;
        if (isFound && word.size() >= 3 && word.find('.') < word.length()  // a better filter to lookup for variables?  
            || mUDINames.contains(QString::fromStdString(word)) ) //User-defined Indicator
        {
            std::string var_k = 'x' + std::to_string(k); 
            mRenamedFormula.replace(mRenamedFormula.find('\"' + word + '\"'), word.length() + 2, var_k);
            mVariableRenamingMap[var_k] = QString::fromStdString(word);
            k++;
        }
    }     
}

void DynamicIndicator::compile() 
{
    //Initialize values to nan
    QMapIterator<std::string, QString> var_Itr_1(mVariableRenamingMap);
    while (var_Itr_1.hasNext())
    {
        var_Itr_1.next();
        mVariableValueMap[var_Itr_1.key()] = new double;
        *mVariableValueMap[var_Itr_1.key()] = double_NaN;
    }
 
    //Add variables
    QMapIterator<std::string, double*> var_Itr_2(mVariableValueMap);
    while (var_Itr_2.hasNext())
    {
        var_Itr_2.next();
        mSymbolTable.add_variable(var_Itr_2.key(), *var_Itr_2.value());
    }
    
    //Add known constants: 
    mSymbolTable.add_constants();
    
    //Register expression (Formula)
    mExpression.register_symbol_table(mSymbolTable);

    //Parser expression 
    if(!mParser.compile(mRenamedFormula, mExpression)){
        //mExpression.release();
        //mSymbolTable.clear();
        qWarning("Error while compiling the formula (%s) of dynamic indicator %s: \n%s", mFormula.toStdString().c_str(), mName.toStdString().c_str(), mParser.error().c_str());
    }
}

void DynamicIndicator::updateExpression()
{
    renameVariables();
    compile();
}

double DynamicIndicator::compute() const { return mExpression.value(); } //might be nan