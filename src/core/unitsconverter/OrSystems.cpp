#include "OrSystems.h"
#include "OrSysQuantity.h"

OrSystems::OrSystems(long a_Key, OrObject* ap_Parent)
	: OrObject(a_Key, ap_Parent)
{
	m_CanSave = false;
}

OrSystems::~OrSystems(void)
{
}

bool OrSystems::Add()
{
	OrObject::Add(new OrSystem(this));
	return true;
}


OrSystem::OrSystem(OrObject* ap_Parent)
	: OrObject(ap_Parent)
{
}

OrSystem::~OrSystem(void)
{
}

bool OrSystem::Add()
{
	OrObject::Add(new OrSysQuantity(this));
	return true;
}