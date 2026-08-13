#include "ZEVariables.h"
#include "GlobalSettings.h"

ZEVariables::ZEVariables(
    const std::string& a_Name,
    const std::string& a_Unit,
    const std::string& a_Desc,
    const std::string& a_Coeff,
    const std::string& a_Offset,
    const bool& aIsMPC
) 
    : mName(a_Name)
    , mUnit(a_Unit)
    , mDesc(a_Desc)
    , m_IsMPC(aIsMPC)
    , minitValue(std::numeric_limits<float>::quiet_NaN()) /* Never set! but used in ModuleCairn! */
{
    resolveCoeff(a_Coeff);
    resolveOffset(a_Offset);
}

ZEVariables::ZEVariables(
    const std::string& a_Name,
    const FlagParam* const a_IsUsed,
    const UnitParam* const a_Unit,
    const std::string& a_Desc,
    const std::string& a_Coeff,
    const std::string& a_Offset,
    const bool& aIsMPC
)
    : mName(a_Name)
    , pIsUsed(a_IsUsed)
    , pUnit(a_Unit)
    , mDesc(a_Desc)
    , m_IsMPC(aIsMPC)
    , minitValue(std::numeric_limits<float>::quiet_NaN())
{
    resolveCoeff(a_Coeff);
    resolveOffset(a_Offset);
}

void ZEVariables::resolveCoeff(const std::string& coeff)
{
    try
    {
        mCoeffExport = std::stod(coeff);
        if (mCoeffExport != 1.0)
            mDesc += " x " + coeff;
    }
    catch (const std::exception&)
    {
        mCoeffExport = 1.0;
    }
}

void ZEVariables::resolveOffset(const std::string& offset)
{
    try
    {
        mCoeffOffset = std::stod(offset);
        if (mCoeffOffset)
            mDesc += " + " + offset;
    }
    catch (const std::exception&)
    {
        mCoeffOffset = 0.0;
    }
}

std::string ZEVariables::Unit() const
{
    return pUnit ? (*pUnit).get_Value() : mUnit;
}

bool ZEVariables::IsUsed() const
{
    return pIsUsed ? (*pIsUsed).get_Value() : true;
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

bool ZEVariables::set_Values(ModelParam* a_Param, double aTimeStepOut, const std::vector<double>& aTimeStepsIn, uint aNpdtPast)
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
                GS::uExpandVecxf2QVector(&m_Values, m_Values.size(), *lptr, aTimeStepOut, 
                    aTimeStepsIn, aNpdtPast, mCoeffExport, mCoeffOffset);
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