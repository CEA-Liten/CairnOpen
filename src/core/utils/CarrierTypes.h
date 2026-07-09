#pragma once
#include "CairnAPI.h"
#include "json.hpp"
using json = nlohmann::json;
using ojson = nlohmann::ordered_json;


class DECLSPEC CarrierTypes {
public:
	CarrierTypes(const std::string& a_FileName = "") {
		if (a_FileName != "")
			Load(a_FileName);
	};
	static bool Load(const std::string& a_FileName);

	static std::vector<std::string> getCarrierTypes();
	static std::map<std::string, double> getCarrierProperties(const std::string& a_CarrierType); 
	static double getCarrierProp(const std::string & a_CarrierType, const std::string a_PropName);
	//static double computeCp(const std::string& a_CarrierType, double a_temp);

	static std::vector<std::string> getChemicalCompositions();
	static std::map<std::string, double> getChemicalCompProperties(const std::string& a_ChemicalCompo);
	static double getChemicalCompProp(const std::string& a_ChemicalCompo, const std::string a_PropName = "MolarMass");

protected:
	
	class BaseProperties {
	public:
		BaseProperties(const json& a_def);
		std::map<std::string, double> getProperties() const { return m_Props; }
		double getProp(const std::string a_PropName) const;		
	protected:
		std::map<std::string, double> m_Props;
	};
	class CarrierProperties : BaseProperties {
	public:
		CarrierProperties(const json& a_def);		
		//double computeCp(double a_temp);	
	};
	static std::vector<CarrierProperties> m_Carriers;
	static std::map<std::string, size_t> m_mapCarriers;

	static std::vector<BaseProperties> m_ChemicalCompositions;
	static std::map<std::string, size_t> m_mapChemicalCompositions;

	static bool addCarrierType(const json& a_def);
	static bool addChemicalComposition(const json& a_def);
	static std::vector<std::string> get(const std::map<std::string, size_t>& a_Elems);
	static std::map<std::string, double> getProperties(const std::map<std::string, size_t>& a_mapElems, const std::vector<BaseProperties> *a_Elems, const std::string& a_ElemName);
	static double getProperty(const std::map<std::string, size_t>& a_mapElems, const std::vector<BaseProperties>* a_Elems, const std::string& a_ElemName, const std::string &a_PropName);

};