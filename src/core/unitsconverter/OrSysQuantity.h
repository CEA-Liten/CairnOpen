#pragma once
#include "OrObject.h"
class OrSysQuantity :
    public OrObject
{
public:
    OrSysQuantity(OrObject* ap_Parent);
    virtual ~OrSysQuantity(void);

    virtual void InitProperties();

protected:
    long m_UnitIdx;
    long m_DispUnitIdx;

};

