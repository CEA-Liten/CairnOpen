#ifndef ZEVariables_H
#define ZEVariables_H
#include "CairnCore_global.h"
#include "InputParam.h"

/**
 * \details
* This component defines the variables to be exchanged with PEGASE simulation environment.
*/
class CAIRNCORESHARED_EXPORT ZEVariables
{
    
public:
    ZEVariables(
        const std::string& aName = "",
        const t_unit& aUnit = "",
        const std::string& aDesc = "",        
        const std::string& asCoeffExport = "1",
        const std::string& asOffsetExport = "0",
        const bool& aIsMPC = false);
  
    ZEVariables(
        const std::string& aName,
        const UnitParam* aUnit,
        const std::string& aDesc = "",
        const std::string& asCoeffExport = "1",
        const std::string& asOffsetExport = "0",
        const bool& aIsMPC = false);

    std::vector<double>* ptrVariable();     /** Access to Pointer to vector of IO float variable */
    std::vector<double>* ptrOutVariable();
    std::string Name() {return mName;}             /** Access to Associated name of variable */
    std::string Unit() const;                      /** Access to Associated unit of variable */
    std::string Desc() {return mDesc;}             /** Access to Associated description of variable */
    float initValue() {return minitValue;}         /** Access to Associated initial value of variable */
    float CoeffExport() {return mCoeffExport;}     /** Associated multiplicative factor for export */
    float CoeffOffset() {return mCoeffOffset;}     /** Associated offset value for export */
    bool IsMPC() { return m_IsMPC; }

    void setName(const std::string& a_Name) { mName = a_Name; }

    void setCoeffExport(float aCoeffExport) {mCoeffExport=aCoeffExport;}
    void setCoeffOffset(float aCoeffOffset) {mCoeffOffset=aCoeffOffset;}

    bool set_Values(class InputParam::ModelParam* a_Param,
        double aTimeStepOut, const std::vector<double>& aTimeStepsIn, uint aNpdtPast);
    bool update_PastValues(int nptPast, int timeShift);

    void IsExt(bool a_IsExt) { m_IsExt = a_IsExt; };
private:
    std::string mName ;             /** Associated name of variable */
    UnitParam mUnit ;             /** Associated unit of variable */
    std::string mDesc ;             /** Associated description of variable */
    float minitValue ;              /** Associated initial value of variable */
    float mCoeffExport ;             /** Associated multiplicative factor for export */
    float mCoeffOffset ;             /** Associated offset factor for export */

    std::vector<double> m_Values;
    std::vector<double> m_OutValues;
    bool m_IsMPC{ false };
    bool m_IsExt{ false };
};

typedef std::map<std::string, ZEVariables*> t_mapExchange;

#endif // ZEVariables_H
