/*
* \file		IndicatorName.h
* \brief	A class for a descriptive indicator name 
* \version	1.0
* \author	Ali KASSEM
* \date		18/12/2025
*/

#ifndef IndicatorName_H
#define IndicatorName_H

#include "CairnCore_global.h"
#include <variant>

using namespace std;

struct SExtFunctionName {
    class SubModel* pModel{ nullptr };
    class MilpPort* pPort{ nullptr };
    std::string(*pFunct)(class SubModel* ap_Model, class MilpPort* ap_Port, const std::vector<std::string>& a_NameParts) { nullptr };
    std::vector<std::string> nameParts{};
};

typedef std::variant<std::string, const std::string*, SExtFunctionName> t_Name;

class CAIRNCORESHARED_EXPORT IndicatorName
{
public:
    IndicatorName();
    void set_Value(t_Name a_Name);
    std::string get_Value() const;

private:
    std::string m_Value;
    const std::string* p_Value;
    SExtFunctionName m_ExtFunct;
};

#endif // IndicatorName_H
