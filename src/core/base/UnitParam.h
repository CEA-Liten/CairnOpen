/*
* \file		UnitParam.h
* \brief	A class for a descriptive unit
* \version	1.0
* \author	Ali KASSEM
* \date		15/10/2025
*/

#ifndef UnitParam_H
#define UnitParam_H

#include "CairnCore_global.h"
#include <variant>

enum EFunctUnitType {
    eFTypeUnknown = -1,
    eFTypeDivision = 0,        // Unit_1/Unit_2/.../Unit_n
    eFTypeMultiplication = 1  // Unit_1 * Unit_2 * ... * Unit_n        
};

struct SFunctionUnit {
    /* 
    * prefixUnit and suffixUnit are used only when a part of the unit is dynamic
    * suffixUnit is placed before prefixUnit because it is the most common case 
    * For example "currency/h" where only currency is dynamic is translated to :
    *    SFunctionUnit({ eFTypeDivision, { &mCurrency}, "h" })
    */
    EFunctUnitType Type;
    std::vector<const std::string*> Units;
    std::string suffixUnit = ""; 
    std::string prefixUnit = ""; /*  */
};

typedef std::variant<std::string, const std::string*, SFunctionUnit> t_unit;

class CAIRNCORESHARED_EXPORT UnitParam
{
public:
    UnitParam();
    void set_Value(t_unit a_Unit);
    std::string get_Value() const;

private:
    std::string m_Value;
    const std::string* p_Value;
    SFunctionUnit m_Function;
};

#endif // UnitParam_H
