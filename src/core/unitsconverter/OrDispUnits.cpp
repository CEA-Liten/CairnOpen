#include "OrDispUnits.h"

OrDispUnits::OrDispUnits(long a_Key, OrObject* ap_Parent)
	: OrObject(a_Key, ap_Parent)
{
	m_CanSave = false;
}

OrDispUnits::~OrDispUnits(void)
{
}

void OrDispUnits::Create(OrObject* a_DispUnitRef)
{	
	if (a_DispUnitRef) {
		OrObject* vDispUnit = nullptr;
		iterator vIter = find(a_DispUnitRef->get_Name());
		// TODO: warning si existe déjà ?
		if (vIter == end()) {
			// Création
			vDispUnit = get_ElemByKey(OrObject::Add(a_DispUnitRef->get_Name(), ""));
		}
		else
			vDispUnit = *vIter;
		vDispUnit->AddLink(a_DispUnitRef);
	}
}
