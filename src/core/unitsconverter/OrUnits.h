#pragma once
#include "OrObject.h"
#include "OrUnitsConverter.h"

class OrUnits :
    public OrObject
{
public:
    OrUnits(long a_Key, OrObject* ap_Parent);
    virtual ~OrUnits(void);

    double Convert(double a_Value, const OrUnitsConverter::OrDefUnit& a_SrcUnit,
        const OrUnitsConverter::OrDefUnit& a_DestUnit,
        OrObject *a_System,
        OrUnitsConverter::OrResUnit *a_ResUnit);

    std::vector<std::string> get_Quantities(const OrUnitsConverter::OrDefUnit& a_UnitName);
    void getInfos(const OrUnitsConverter::OrDefUnit& a_SrcUnit,
        const OrUnitsConverter::OrDefUnit& a_DestUnit, OrCheckUnits &a_Infos);

protected:
    virtual bool Add(const std::string& a_Kind);

    OrObject* get_Unit(const OrUnitsConverter::OrDefUnit& a_Unit);
    std::vector<OrObject*> get_Units(const OrUnitsConverter::OrDefUnit& a_Unit);
    OrObject* get_Unit(OrObject* ap_System, const std::string& a_Quantity);
};

