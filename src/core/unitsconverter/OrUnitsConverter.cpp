#include "OrUnitsConverter.h"
#include "OrUnits.h"
#include "OrQuantities.h"
#include "OrDispUnits.h"
#include "OrSystems.h"

OrUnitsConverter UnitsConverter::m_Converter("DefUnits");

OrUnitsConverter::OrUnitsConverter(const std::string& a_Name, const std::string& a_File)
	:OrRoot()
{
	if (a_Name == "")
		m_AppName = "OriondeUnits";
	else
		m_AppName = a_Name;
	m_Name = "Orionde_Units";

	// Création des groupes
	Add("units", new OrUnits(eUnits, this));
	Add("quantities", new OrQuantities( eQuantities, this));
	Add("systems", new OrSystems( eSystems, this));
	Add("dispUnits", new OrDispUnits( eDispUnits, this));		
	
	p_Systems = get_ElemByKey(eSystems);		
	p_Units = (OrUnits*)get_ElemByKey(eUnits);
	
	p_Units->AddLink(get_ElemByKey(eQuantities));
	p_Units->AddLink(get_ElemByKey(eDispUnits));

	if (a_File != "")
		Load(a_File);
}

OrUnitsConverter::~OrUnitsConverter(void)
{
}

double OrUnitsConverter::Convert(double a_Value, const std::string& a_SrcUnitName, const std::string& a_DestUnitName, OrResUnit* a_ResUnit, const std::string& a_System)
{
	return Convert(a_Value, OrDefUnit(a_SrcUnitName), OrDefUnit(a_DestUnitName), a_ResUnit, a_System);
}

double OrUnitsConverter::Convert(double a_Value, const OrDefUnit& a_SrcUnit, const OrDefUnit& a_DestUnit, OrResUnit* a_ResUnit, const std::string& a_System)
{
	iterator vIter = p_Systems->find(a_System);
	OrObject* pSystem = (vIter != p_Systems->end()) ? *vIter : nullptr;
	return p_Units->Convert(a_Value, a_SrcUnit, a_DestUnit, pSystem, a_ResUnit);
}

std::string OrUnitsConverter::get_Quantity(const std::string& a_UnitName)
{
	std::string vRet = "";
	iterator vIter = p_Units->find(a_UnitName);
	OrObject* pUnit = (vIter != p_Units->end()) ? *vIter : nullptr;
	if (pUnit) {
		vRet = pUnit->get_Kind();
	}
	return vRet;
}

std::vector<std::string> OrUnitsConverter::get_Quantities(const OrDefUnit& a_UnitName)
{
	return p_Units->get_Quantities(a_UnitName);
}

bool OrUnitsConverter::CheckUnits(const OrDefUnit& a_SrcUnit, const OrDefUnit& a_DestUnit, OrCheckUnits* a_CheckUnits)
{
	bool vRet = true;
	std::vector<std::string> vSrcQs = p_Units->get_Quantities(a_SrcUnit);
	if (!vSrcQs.size()) {
		if (a_CheckUnits)
			a_CheckUnits->isExist1 = false;
		vRet = false;
	}
	std::vector<std::string> vDestQs = p_Units->get_Quantities(a_DestUnit);
	if (!vDestQs.size()) {
		if (a_CheckUnits)
			a_CheckUnits->isExist2 = false;
		vRet = false;
	}
	if (vRet) {
		for (auto& vSrc : vSrcQs) {
			if (std::find(vDestQs.begin(), vDestQs.end(), vSrc) == vDestQs.end()) {
				// not find!
				if (a_CheckUnits)
					a_CheckUnits->isConsistency = false;
				return false;
			}
		}
		if (a_CheckUnits) {
			a_CheckUnits->isConsistency = true;
			p_Units->getInfos(a_SrcUnit, a_DestUnit, *a_CheckUnits);
		}
	}
	return vRet;
}
