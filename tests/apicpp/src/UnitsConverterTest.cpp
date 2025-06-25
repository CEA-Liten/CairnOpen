// TestUnitsConverter.cpp : Ce fichier contient la fonction 'main'. L'exécution du programme commence et se termine à cet endroit.
//
#include "TEST_CairnCore.h"
#include <iostream>
#include "OrUnitsConverter.h"

#define TESTVALUE(var, ref) \
		if (fabsf(var - ref)>1e-6) return 1; \

int main()
{ 
	UnitsConverter::UnitsConverter(TEST_DATA + (std::string)"/../../../resources/DefUnits.json");
	
	OrUnitsConverter::OrResUnit vRes;
	// L'unité destination n'est pas définie, c'est l'unité SI qui est prise
	double vTemp1 = UnitsConverter::Convert(10, "°C", "", &vRes);
	std::cout << vRes.Src << vRes.SrcUnit << " = " << vRes.Dest << vRes.DestUnit << std::endl;
	TESTVALUE(vTemp1, 283.15);

	// L'unité source n'est pas définie, c'est l'unité SI qui est prise (pas de conversion dans ce cas)
	double vTemp2 = UnitsConverter::Convert(10, "", "°K");	
	TESTVALUE(vTemp2, 10);

	OrUnitsConverter::OrDefUnit vSrc;
	// L'unité destination n'est pas définie, c'est l'unité SI qui est prise
	// Pour l'unité source utilisation de l'alias à la place de l'ID
	vSrc.DispName = "DegC";
	double vTemp3 = UnitsConverter::Convert(10, vSrc, OrUnitsConverter::OrDefUnit(), &vRes);
	std::cout << vRes.Src << vRes.SrcUnit << " = " << vRes.Dest << vRes.DestUnit << std::endl;	
	TESTVALUE(vTemp3, 283.15);
	// L'unité source n'est pas définie, c'est l'unité SI qui est prise 
	// Pour l'unité destination utilisation de l'alias à la place de l'ID
	double vTemp4 = UnitsConverter::Convert(10, OrUnitsConverter::OrDefUnit(), vSrc, &vRes);
	std::cout << vRes.Src << vRes.SrcUnit << " = " << vRes.Dest << vRes.DestUnit << std::endl;	
	TESTVALUE(vTemp4, -263.15);

	// L'unité destination n'est pas définie, c'est l'unité SI qui est prise
	// Pour l'unité source utilisation de l'alias et de la quantité à la place de l'ID
	// En effet pour cet alias il existe deux quantités possibles (si la quantité n'est pas précisée, c'est la première qui est utilisée)
	vSrc.Quantity = "Delta_Temperature";
	double vTemp5 = UnitsConverter::Convert(10, vSrc, OrUnitsConverter::OrDefUnit(), &vRes);
	std::cout << vRes.Src << vRes.SrcUnit << " = " << vRes.Dest << vRes.DestUnit << std::endl;
	TESTVALUE(vTemp5, 10);
	double vTemp6 = UnitsConverter::Convert(10, OrUnitsConverter::OrDefUnit(), vSrc, &vRes);
	std::cout << vRes.Src << vRes.SrcUnit << " = " << vRes.Dest << vRes.DestUnit << std::endl;
	TESTVALUE(vTemp6, 10);

	// Utilisation d'un système d'unités
	UnitsConverter::Load(TEST_DATA + (std::string)"/unitsconverter/DefSystems.json");
	// L'unité destination n'est pas définie, c'est l'unité du système d'unité qui va être utilisée
	UnitsConverter::Convert(10, "°C", "", &vRes, "UserSystem");
	std::cout << vRes.Src << vRes.SrcUnit << " = " << vRes.Dest << vRes.DestUnit << std::endl;	
	TESTVALUE(vRes.Dest, 50);
	// L'unité source n'est pas définie, c'est l'unité du système d'unité qui va être utilisée
	UnitsConverter::Convert(10, "", "°C", &vRes, "UserSystem");
	std::cout << vRes.Src << vRes.SrcUnit << " = " << vRes.Dest << vRes.DestUnit << std::endl;
	TESTVALUE(vRes.Dest, -12.222222222);


	// Récupère la quantité (une seule possible si on utilise un ID pour définir l'unité)
	std::string vQ = UnitsConverter::get_Quantity("°C");
	std::cout << "Quantity: " << vQ << std::endl;

	// Récupère les quantités possibles (plusieurs possibles si on utilise un alias pour définir l'unité) 
	std::vector<std::string> vQs = UnitsConverter::get_Quantities(vSrc);
	
	if (!UnitsConverter::CheckUnits(vSrc, vSrc))
		return 1;
	return 0;
}
