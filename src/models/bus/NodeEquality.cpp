/* --------------------------------------------------------------------------
 * File: NodeEquality.cpp
 * version 1.0
 * Author: Alain Ruby
 * Date 23/07/2019
 *---------------------------------------------------------------------------
 * Description: Model imposing Node addition node constraint on Bus Component
 * --------------------------------------------------------------------------
 */

#include "NodeEquality.h"
extern "C" MODELS_DECLSPEC QObject * createModel(QObject * aParent)
{
    return new NodeEquality(aParent);
}

NodeEquality::NodeEquality(QObject* aParent)
  : BusSubModel(aParent),
    mBusMeanValue(2,0.)
{
}

NodeEquality::~NodeEquality()
{

}

void NodeEquality::setParameters(double aBusValue, double aMaxBusValue)
{
	mBusValue = aBusValue;
	mMaxBusValue = aMaxBusValue;
}

void NodeEquality::setTimeData()
{
    SubModel::setTimeData();
    mExprBusValue.clear();
    mExprBusValue.resize(mHorizon);
}

void NodeEquality::buildModel()
{
    if (mAllocate)
    {
        if (mMaxBusValue >=0.)
            mVarBusValue = MIPModeler::MIPVariable1D( mHorizon, mMinBusValue, mMaxBusValue); // debit max de production Cette grandeur est mis a  jour dynamiquement dans le "buildProblem"
        else
            mVarBusValue = MIPModeler::MIPVariable1D( mHorizon, fabs(mMaxBusValue), 0.f); // debit max de production Cette grandeur est mis a  jour dynamiquement dans le "buildProblem"
        for (unsigned int t = 0; t < mHorizon ; ++t)
           mExprBusValue[t] = MIPModeler::MIPExpression () ;
    }
    else
    {
        for (unsigned int t = 0; t < mHorizon ; ++t)
            closeExpression(mExprBusValue[t]) ;
    }
    addVariable(mVarBusValue,"BusValue");

    // le bus est une contrainte systeme sous forme d'une expression a laquelle chaque composant contribue directement
    // we will loop on the list of connected ports imposing BusSameValue constraint for each of them with BusValue one

    MilpPort* port ;
    QListIterator<MilpPort*> iport (mListPort);
    while (iport.hasNext())
    {
	  port = iport.next() ;
	  for (unsigned int t = 0; t < mHorizon ; ++t)
	  {
         addConstraint(mVarBusValue(t) - port->ExpPotential()[t] == 0,"E",t) ;
      }
    }

    for (unsigned int t = 0; t < mHorizon ; ++t)
    {
       mExprBusValue[t] += mVarBusValue(t) ;
    }

    if (mImposedBusValue)
    {
        for (unsigned int t = 0; t < mHorizon ; ++t)
           addConstraint(mVarBusValue(t) == mBusValue,"I",t) ;
    }

    /** Objective contribution expressed as the sum of Capex and Opex so as to be able to account for actualization rate on Opex part only*/
    computeAllContribution() ;

    mAllocate = false ;
}
//-------------------------------------------------------
void NodeEquality::computeAllIndicators(const double* optSol)
{
    mBusMeanValue.at(0) = 0.;
    /*for (unsigned int t = 0; t < mHorizon; ++t) mBusMeanValue.at(0) += mExprBusValue.at(t).evaluate(optSol);
    mBusMeanValue.at(0) = mBusMeanValue.at(0) / mHorizon;*/
    mBusMeanValue.at(1) = 0.;
    /*for (unsigned int t = 0; t < *mptrTimeshift; ++t) mBusMeanValue.at(1) += mExprBusValue.at(t).evaluate(optSol);
    mBusMeanValue.at(1) = mBusMeanValue.at(1) / mHorizon;*/
}