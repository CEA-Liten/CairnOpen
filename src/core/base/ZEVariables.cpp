#include "ZEVariables.h"
#include "GlobalSettings.h"

ZEVariables::ZEVariables(
    const std::string& a_Name,
    const t_unit& a_Unit,
    const std::string& a_Desc,
    const std::string& a_Coeff,
    const std::string& a_Offset,
    const bool& aIsMPC
)
{
    // export
    mName = a_Name;
    mUnit.set_Value(a_Unit);
    mDesc = a_Desc;
    m_IsMPC = aIsMPC;
    try
    {
        mCoeffExport = std::stod(a_Coeff);
        if (mCoeffExport != 1.0)
            mDesc += " x " + a_Coeff;
    }
    catch (const std::exception&)
    {
        mCoeffExport = 1.0;
    }
    try
    {
        mCoeffOffset = std::stod(a_Offset);
        if (mCoeffOffset)
            mDesc += " + " + a_Offset;
    }
    catch (const std::exception&)
    {
        mCoeffOffset = 0.0;
    }    
}

ZEVariables::ZEVariables(
    const std::string& a_Name,
    const UnitParam* a_Unit,
    const std::string& a_Desc,
    const std::string& a_Coeff,
    const std::string& a_Offset,
    const bool& aIsMPC
)
    : ZEVariables::ZEVariables(a_Name, t_unit(std::string("-")), a_Desc, a_Coeff, a_Offset, aIsMPC)
{
    if (a_Unit) mUnit = *a_Unit;
}

std::string ZEVariables::Unit() const
{
    return mUnit.get_Value();
}

std::vector<double>* ZEVariables::ptrVariable()
{
   return &m_Values;
}

std::vector<double>* ZEVariables::ptrOutVariable()
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
        if (std::isnan(m_Values[npdtPast - timeShift + i]) && !std::isnan(m_Values[npdtPast]))
        {
            m_Values[npdtPast - timeShift + i] = m_Values[npdtPast]; // update next past with new present

            vRet = true;;
        }
    }
    return vRet;
}