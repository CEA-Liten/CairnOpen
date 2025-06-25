#include "OrUnits.h"
#include "OrUnit.h"

OrUnits::OrUnits(long a_Key, OrObject* ap_Parent)
	: OrObject(a_Key, ap_Parent)
{
}

OrUnits::~OrUnits(void)
{
}

bool OrUnits::Add(const std::string& a_Kind)
{
	OrObject::Add(new OrUnit(this, a_Kind));
	return true;
}

OrObject* OrUnits::get_Unit(const OrUnitsConverter::OrDefUnit& a_Unit)
{
	OrObject* vRet = nullptr;
	if (a_Unit.Name != "") {
		iterator vIter = find(a_Unit.Name);
		if (vIter != end()) {
			vRet = *vIter;
		}
	}
	else if (a_Unit.DispName != "") {
		OrObject* pDispUnits = get_Ref(OrUnitsConverter::eDispUnits);
		iterator vIter = pDispUnits->find(a_Unit.DispName);
		if (vIter != pDispUnits->end()) {
			// trouvé, peut avoir plusieurs unités ayant la même 'dispUnit'			
			// si la quantité n'est pas spécifiée, prends la première
			OrObject* pDispUnit = *vIter;
			if (a_Unit.Quantity == "") {
				OrObject* pDispUnitRef = pDispUnit->get_Ref();
				if (pDispUnitRef) {
					vRet = pDispUnitRef->get_Parent();
				}
			}
			else {
				const std::vector<OrObject*>& pRefs = pDispUnit->get_Refs();
				const_iterator vIterDisp;
				for (vIterDisp = pRefs.begin(); vIterDisp < pRefs.end(); vIterDisp++) {
					if (*vIterDisp != nullptr) {
						if ((*vIterDisp)->get_Parent()->get_Kind() == a_Unit.Quantity) {
							vRet = (*vIterDisp)->get_Parent();
							break;
						}
					}
				}
			}
		}
	}
	else if (a_Unit.Key >=0) {
		vRet = get_ElemByKey(a_Unit.Key);
	}
	return vRet;
}

std::vector<OrObject*> OrUnits::get_Units(const OrUnitsConverter::OrDefUnit& a_Unit)
{
	std::vector<OrObject*> vRet;
	if (a_Unit.Name != "") {
		iterator vIter = find(a_Unit.Name);
		if (vIter != end()) {
			vRet.push_back( *vIter );
		}
	}
	else if (a_Unit.DispName != "") {
		OrObject* pDispUnits = get_Ref(OrUnitsConverter::eDispUnits);
		iterator vIter = pDispUnits->find(a_Unit.DispName);
		if (vIter != pDispUnits->end()) {			
			OrObject* pDispUnit = *vIter;			
			const std::vector<OrObject*>& pRefs = pDispUnit->get_Refs();
			const_iterator vIterDisp;
			for (vIterDisp = pRefs.begin(); vIterDisp < pRefs.end(); vIterDisp++) {
				if (*vIterDisp != nullptr) {					
					vRet.push_back((*vIterDisp)->get_Parent());					
				}
			}			
		}
	}
	return vRet;
}

OrObject* OrUnits::get_Unit(OrObject* ap_System, const std::string& a_Quantity)
{
	OrObject* vRet = nullptr;
	iterator vIter = ap_System->find(a_Quantity);
	OrObject* pQuantity = nullptr;
	if (vIter != ap_System->end()) {
		pQuantity = *vIter;
	}	
	if (pQuantity)
		vRet = pQuantity->get_Ref();
	return vRet;
}

double OrUnits::Convert(double a_Value, const OrUnitsConverter::OrDefUnit& a_SrcUnit, 
	const  OrUnitsConverter::OrDefUnit& a_DestUnit, OrObject *ap_System,
	OrUnitsConverter::OrResUnit* a_ResUnit)
{
	double vRet = a_Value;
	if (a_ResUnit)
		a_ResUnit->isConverted = false;

	OrUnit* pSrcUnit = (OrUnit*)get_Unit(a_SrcUnit);
	OrUnit* pDestUnit = (OrUnit*)get_Unit(a_DestUnit);

	// on récupère l'unité à partir du système
	OrObject* pSystem = ap_System;
	if (pSystem == nullptr)
		pSystem = get_Ref(); // pas de système, utilise la liste des quantités définies

	if ((pSrcUnit != nullptr) && (pDestUnit == nullptr)) {
		pDestUnit = (OrUnit*)get_Unit(pSystem, pSrcUnit->get_Kind());
	}
	else {
		if ((pSrcUnit == nullptr) && (pDestUnit != nullptr)) {
			pSrcUnit = (OrUnit*)get_Unit(pSystem, pDestUnit->get_Kind());
		}
	}
	if ((pSrcUnit != nullptr) && (pDestUnit != nullptr)) {		
		double vSI = pSrcUnit->ConvertToSI(a_Value);
		vRet = pDestUnit->ConvertFromSI(vSI);

		if (a_ResUnit) {
			a_ResUnit->isInverse = pSrcUnit->IsInv() ? !pDestUnit->IsInv() : pDestUnit->IsInv();
			a_ResUnit->isConverted = true;
			a_ResUnit->Src = a_Value;
			a_ResUnit->SrcUnit = pSrcUnit->get_Name();
			a_ResUnit->DestUnit = pDestUnit->get_Name();
			a_ResUnit->Dest = vRet;
		}
	}
	return vRet;
}

std::vector<std::string> OrUnits::get_Quantities(const OrUnitsConverter::OrDefUnit& a_UnitName)
{
	std::vector<std::string> vRet;
	std::vector<OrObject*> pUnits = get_Units(a_UnitName);
	for (auto& pUnit : pUnits) {
		if (pUnit) vRet.push_back(pUnit->get_Kind());
	}
	return vRet;
}

void OrUnits::getInfos(const OrUnitsConverter::OrDefUnit& a_SrcUnit, const OrUnitsConverter::OrDefUnit& a_DestUnit, OrCheckUnits& a_Infos)
{
	a_Infos.isSame = false;
	OrUnit* pSrcUnit = (OrUnit*)get_Unit(a_SrcUnit);
	OrUnit* pDestUnit = (OrUnit*)get_Unit(a_DestUnit);
	if (pSrcUnit && pDestUnit) {
		a_Infos.isSame = (pSrcUnit == pDestUnit);		
	}
	if (pSrcUnit) {
		a_Infos.keyUnit1 = pSrcUnit->get_Key();
	}
	if (pDestUnit) {
		a_Infos.keyUnit2 = pDestUnit->get_Key();
	}
}
