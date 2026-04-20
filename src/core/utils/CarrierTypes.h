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
	static double getCarrierProp(const std::string & a_CarrierType, const std::string a_PropName);
	static double computeCp(const std::string& a_CarrierType, double a_temp);

protected:
	static bool addCarrierType(const json& a_def);

	class CarrierProperties {
	public:
		CarrierProperties(const json& a_def);
		double getProp(const std::string a_PropName);
		double computeCp(double a_temp);
	private:		
		std::map<std::string, double> m_Props;
	};
	static std::vector<CarrierProperties> m_Carriers;
	static std::map<std::string, size_t> m_mapCarriers;
};