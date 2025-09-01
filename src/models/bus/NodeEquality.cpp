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

void NodeEquality::computeModelContribution()
{
    if (mMaxBusValue >= 0.) {
        addVariable(mVarBusValue, "BusValue", mMinBusValue, mMaxBusValue);
    }
    else {
        addVariable(mVarBusValue, "BusValue", fabs(mMaxBusValue), 0.f);
    }

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