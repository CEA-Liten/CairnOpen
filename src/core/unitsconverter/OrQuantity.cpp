#include "OrQuantity.h"
#include "OrUnit.h"

OrQuantity::OrQuantity(OrObject* ap_Parent)
	: OrObject(ap_Parent)
{
}

OrQuantity::~OrQuantity(void)
{
}

void OrQuantity::AddUnit(OrUnit* ap_Unit)
{
	if (ap_Unit) {	
		OrObject::Add(ap_Unit->get_Name(), "");
		if (ap_Unit->IsSI()) {
			// Cas particulier de l'unit� SI, c'est l'unit� de r�f�rence de la grandeur
			AddLink(ap_Unit);
		}
	}
}
