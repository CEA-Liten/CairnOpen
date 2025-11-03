#ifndef FlagParam_H
#define FlagParam_H

#include "CairnCore_global.h"
#include <variant>

enum EFunctFlagType {
    eFTypeUndefined = -1,
    eFTypeNotAnd = 0,   // !Flags_i && !Flags_i ... && Flags2_j && Flags2_j ...
    eFTypeOrNot,         // Flags_i || Flags_i ... || !Flags2_j || !Flags2_j ...        
    eFTypeNotAndOr      // !Flags_i && !Flags_i ... && (Flags2_j || Flags2_j ...)
};

struct SExtFunctionFlag {
    bool (*pFunct)(class SubModel* ap_Model) { nullptr };
    class SubModel* pModel{ nullptr };
};

struct SFunctionFlag {
    EFunctFlagType Type;
    std::vector<bool*> Flags;
    std::vector<bool*> Flags2;
    SExtFunctionFlag ExtFunct;
};

typedef std::variant<bool, bool*, SFunctionFlag, SExtFunctionFlag> t_flag;

class CAIRNCORESHARED_EXPORT FlagParam
{
public:
    FlagParam();
    void set_Value(t_flag a_Flag);
    bool get_Value();

    bool is_Scalar();

private:
    bool m_Value;
    bool* p_Value;
    SFunctionFlag m_Function;
    SExtFunctionFlag m_ExtFunct;
};

#endif // FlagParam_H
