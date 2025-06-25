#include "ZEVariables.h"
#include "GlobalSettings.h"

ZEVariables::ZEVariables(
    const QString& a_Name,
    const QString& a_Unit,
    const QString& a_Desc,
    const QString& a_Coeff,
    const QString& a_Offset,
    const bool& aIsMPC
)
{
    // export
    mName = a_Name;
    mUnit = a_Unit;
    mDesc = a_Desc;
    m_IsMPC = aIsMPC;
    bool vOk;
    mCoeffExport = a_Coeff.toDouble(&vOk);
    if (!vOk)
        mCoeffExport = 1.0;
    else if (mCoeffExport != 1.0)
        mDesc += " x " + a_Coeff;
    mCoeffOffset = a_Offset.toDouble(&vOk);
    if (!vOk)
        mCoeffOffset = 0.0;
    else if (mCoeffOffset)
        mDesc += " + " + a_Offset;
}

QVector<float>* ZEVariables::ptrVariable()
{
   return &m_Values;
}

QVector<float>* ZEVariables::ptrOutVariable()
{
    if (m_IsExt)
        return &m_OutValues;
    else
        return &m_Values;
}

bool ZEVariables::set_Values(InputParam::ModelParam* a_Param, double aTimeStepOut, const std::vector<double>& aTimeStepsIn, uint aNpdtPast)
{
    // exportResults
    bool vRet = false;
    if (a_Param) {
        Eigen::VectorXf* lptr = std::get< Eigen::VectorXf*>(a_Param->getPtr());
        if (lptr) {
            if (m_Values.size() == 0) {
                m_Values.resize(lptr->size());
            }
            if (m_Values.size() > 0) {
                GS::uExpandVecxf2QVector(&m_Values, m_Values.size(), *lptr, aTimeStepOut, aTimeStepsIn, aNpdtPast, mCoeffExport);
                vRet = true;
            }
        }
    }
    return vRet;
}

bool ZEVariables::update_PastValues(int npdtPast, int timeShift)
{
    bool vRet = false;
    for (uint i = 0; i < timeShift; i++)
    {
        if (isnan(m_Values[npdtPast - timeShift + i]) && !isnan(m_Values[npdtPast]))
        {
            m_Values[npdtPast - timeShift + i] = m_Values[npdtPast]; // update next past with new present

            vRet = true;;
        }
    }
    return vRet;
}