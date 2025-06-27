#include "OrSysQuantity.h"
#include "OrParam.h"
#include "OrUnitsConverter.h"

OrSysQuantity::OrSysQuantity(OrObject* ap_Parent)
	: OrObject(ap_Parent)
{
	m_UnitIdx = AddParam("unit", new OrParam((std::string)""));
	m_DispUnitIdx = AddParam("alias", new OrParam((std::string)""));
}

OrSysQuantity::~OrSysQuantity(void)
{
}

void OrSysQuantity::InitProperties()
{
	OrObject *pUnits = p_Root->get_ElemByKey(OrUnitsConverter::eUnits);
	OrObject* pUnit = *pUnits->find(m_Params[m_UnitIdx]->get_StrValue());
	if (pUnit) {
		AddLink(pUnit);
	}	
}
