#pragma once
#include "OrObject.h"
class OrQuantity :
    public OrObject
{
public:
    OrQuantity(OrObject* ap_Parent);
    virtual ~OrQuantity(void);

    void AddUnit(class OrUnit* ap_Unit);
};

