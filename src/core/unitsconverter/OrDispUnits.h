#pragma once
#include "OrObject.h"
class OrDispUnits :
    public OrObject
{
public:
    OrDispUnits(long a_Key, OrObject* ap_Parent);
    virtual ~OrDispUnits(void);

    void Create(OrObject* a_DisUnit);

};

