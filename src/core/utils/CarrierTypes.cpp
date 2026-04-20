#include "CarrierTypes.h"
#include "OrJsonUtils.h"
#include "OrUnitsConverter.h"
#include "CairnLogger.h"
#include <fstream>

std::vector<CarrierTypes::CarrierProperties> CarrierTypes::m_Carriers;
std::map<std::string, size_t> CarrierTypes::m_mapCarriers;

bool CarrierTypes::Load(const std::string& a_FileName)
{
	bool vRet = false;

	// Chargement du fichier de configuration
	json input;
	std::ifstream file(a_FileName);
	if (file.is_open()) {
		// chargement 
		try
		{
			file >> input;			
			if (input.contains("carriers")) {
				const json& jF = input["carriers"];
				if (jF.is_array()) {
					vRet = true;
					for (const auto& j : jF) {
						vRet &= addCarrierType(j);
					}
				}
			}			
			if (vRet)
				cDebug() << a_FileName << " loading";
		}
		catch (const std::exception& e)
		{
			cError() << "Error in load " << a_FileName << ", " << e.what();			
		}
	}
	else
		cError() << "cannot load " << a_FileName;

	return vRet;
}

double CarrierTypes::getCarrierProp(const std::string& a_CarrierType, const std::string a_PropName)
{
	double vRet = 0.0;
	std::map<std::string, size_t>::iterator vIter = m_mapCarriers.find(a_CarrierType);
	if (vIter != m_mapCarriers.end()) {
		vRet = m_Carriers[vIter->second].getProp(a_PropName);
	}
	else
		cError() << "cannot find carrier " << a_CarrierType;
	return vRet;
}

double CarrierTypes::computeCp(const std::string& a_CarrierType, double a_temp)
{
	double vRet = 0.0;
	std::map<std::string, size_t>::iterator vIter = m_mapCarriers.find(a_CarrierType);
	if (vIter != m_mapCarriers.end()) {
		vRet = m_Carriers[vIter->second].computeCp(a_temp);
	}
	else
		cError() << "cannot find carrier " << a_CarrierType;
	return vRet;
}

bool CarrierTypes::addCarrierType(const json& a_def)
{
	bool vRet = false;
	if (a_def.contains("name")) {
		vRet = true;
		std::string vName = a_def["name"];
		if (m_mapCarriers.find(vName) == m_mapCarriers.end()) {
			m_mapCarriers[vName] = m_Carriers.size();
			m_Carriers.push_back(CarrierProperties(a_def));
		}		
	}
	return vRet;
}

CarrierTypes::CarrierProperties::CarrierProperties(const json& a_def)
{
	for (auto& [key, value] : a_def.items()) {
		if (key != "name") {
			if (m_Props.find(key) == m_Props.end()) {
				double vValue;
				if (orjson::from_json(a_def, key, vValue)) {
					m_Props[key] = vValue;					
				}
			}
		}
	}
}

double CarrierTypes::CarrierProperties::getProp(const std::string a_PropName)
{
	double vRet = 0.0;
	std::map<std::string, double>::iterator vIter = m_Props.find(a_PropName);
	if (vIter != m_Props.end()) {
		vRet = vIter->second;
	}
	else
		cError() << "cannot find property " << a_PropName;
	return vRet;
}

double CarrierTypes::CarrierProperties::computeCp(double a_temp_C)
{
	double  Temp_K = UnitsConverter::Convert(a_temp_C, "DegC", "K"),
		Cp = 0.0;

	if (getProp("a1") != 0.0)
	{
		// Use of NASA equation and coefficients
		Cp = (8.314472 * (getProp("a1")
			+ getProp("a2") * Temp_K
			+ getProp("a3") * pow(Temp_K, 2.0)
			+ getProp("a4") * pow(Temp_K, 3.0)
			+ getProp("a5") * pow(Temp_K, 4.0)))
			/ getProp("Molar_Mass");
	}
	else
	{
		// Use of previous method
		Cp = (getProp("Cp_0")
			+ getProp("Cp_1") * Temp_K
			+ getProp("Cp_2") * pow(Temp_K, 2)
			+ getProp("Cp_3") * pow(Temp_K, 3)
			+ getProp("Cp_4") * pow(Temp_K, 4)
			+ getProp("Cp_5") * pow(Temp_K, 5))
			/ getProp("Molar_Mass");
	}

	return Cp;
}
