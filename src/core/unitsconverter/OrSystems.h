#pragma once
#include "OrObject.h"
class OrSystems :
    public OrObject
{
public:
    OrSystems(long a_Key, OrObject* ap_Parent);
    virtual ~OrSystems(void);

protected:
    virtual bool Add();
};

class OrSystem :
    public OrObject
{
public:
    OrSystem(OrObject* ap_Parent);
    virtual ~OrSystem(void);

protected:
    virtual bool Add();
};

