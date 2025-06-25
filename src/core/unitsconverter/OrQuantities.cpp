#include "OrQuantities.h"
#include "OrQuantity.h"
#include "OrUnit.h"

OrQuantities::OrQuantities(long a_Key, OrObject* ap_Parent)
	: OrObject(a_Key, ap_Parent)
{
	m_CanSave = false;
}

OrQuantities::~OrQuantities(void)
{
}

void OrQuantities::Create(OrUnit* a_Unit)
{
	if (a_Unit) {
		// r�cup�re la quantit� d�finie par l'unit�
		OrQuantity* vQuantity = nullptr;
		iterator vIter = find(a_Unit->get_Kind());
		if (vIter == end()) {
			// cr�ation
			vQuantity = (OrQuantity*)get_ElemByKey(OrObject::Add(a_Unit->get_Kind(), ""));
		}
		else
			vQuantity = (OrQuantity * )*vIter;

		if (vQuantity) {
			vQuantity->AddUnit(a_Unit);
		}
	}
}

bool OrQuantities::Add(const std::string& a_Kind)
{
	OrObject::Add(new OrQuantity(this));
	return true;
}
