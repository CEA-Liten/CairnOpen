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
			// Cas particulier de l'unité SI, c'est l'unité de référence de la grandeur
			AddLink(ap_Unit);
		}
	}
}
