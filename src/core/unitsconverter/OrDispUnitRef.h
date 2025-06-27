#pragma once
#include "OrObject.h"
class OrDispUnitRef :
    public OrObject
{
public:
    OrDispUnitRef(OrObject* ap_Parent);
    virtual ~OrDispUnitRef(void);

    void Init();

protected:
    class OrDispUnits* p_DispUnits;
};

