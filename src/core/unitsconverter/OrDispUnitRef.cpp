#include "OrDispUnitRef.h"
#include "OrUnitsConverter.h"
#include "OrDispUnits.h"

OrDispUnitRef::OrDispUnitRef(OrObject* ap_Parent)
	: OrObject(ap_Parent)
{
	p_DispUnits = (OrDispUnits*)p_Root->get_ElemByKey(OrUnitsConverter::eDispUnits);
}

OrDispUnitRef::~OrDispUnitRef(void)
{
}

void OrDispUnitRef::Init()
{
	// initialise les liens entre unit�s/grandeurs/unit�s affichables	
	p_DispUnits->Create(this);	
}
