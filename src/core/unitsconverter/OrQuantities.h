#pragma once
#include "OrObject.h"
class OrQuantities :
    public OrObject
{
public:
    OrQuantities(long a_Key, OrObject* ap_Parent);
    virtual ~OrQuantities(void);

    void Create(class OrUnit* a_Unit);

protected:
    virtual bool Add(const std::string& a_Kind);
};

