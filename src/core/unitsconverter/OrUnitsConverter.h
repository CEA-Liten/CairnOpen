#pragma once
#include "CairnAPI.h"
#include "OrRoot.h"

class DECLSPEC OrCheckUnits
{
public:
	bool isExist1{ false };
	bool isExist2{ false };
	bool isConsistency{ false };
	bool isSame{ false };
	int keyUnit1{ -1 };
	int keyUnit2{ -1 };
};

class DECLSPEC OrUnitsConverter :
    public OrRoot
{
public:
	OrUnitsConverter(const std::string &a_Name="", const std::string& a_File = "");
	virtual ~OrUnitsConverter(void);

	enum {
		eUnits = 0,
		eQuantities,
		eSystems,
		eDispUnits
	};

	class OrDefUnit
	{
	public:
		int Key{ -1 };
		std::string Name{ "" };
		std::string DispName{ "" };
		std::string Quantity{ "" };
		OrDefUnit() {};
		OrDefUnit(const std::string a_DispName) { DispName = a_DispName; };
		OrDefUnit(int a_Key) { Key = a_Key; };
	}; 
	class OrResUnit
	{
	public:
		bool isConverted;
		bool isInverse;		
		double Src;
		std::string SrcUnit;
		double Dest;
		std::string DestUnit;
	};
	
	double Convert(double a_Value, const std::string& a_SrcUnitName, const std::string& a_DestUnitName,
		OrResUnit *a_ResUnit = nullptr, const std::string& a_System = "");
	
	double Convert(double a_Value, const OrDefUnit& a_SrcUnit, const OrDefUnit& a_DestUnit,
		OrResUnit *a_ResUnit = nullptr, const std::string& a_System = "");

	std::string get_Quantity(const std::string& a_UnitName);	
	std::vector<std::string> get_Quantities(const OrDefUnit& a_UnitName);
	bool CheckUnits(const OrDefUnit& a_SrcUnit, const OrDefUnit& a_DestUnit, OrCheckUnits *a_CheckUnits = nullptr);

protected:
	class OrUnits* p_Units;
	OrObject* p_Systems;
};


// version statique
class DECLSPEC UnitsConverter {
public:
	UnitsConverter(const std::string& a_FileName = "") {
		if (a_FileName!="")
			m_Converter.Load(a_FileName);
	};
	static double Convert(double a_Value, const std::string& a_SrcUnitName, const std::string& a_DestUnitName,
		OrUnitsConverter::OrResUnit* a_ResUnit = nullptr, const std::string& a_System = "")
	{
		return m_Converter.Convert(a_Value, a_SrcUnitName, a_DestUnitName, a_ResUnit, a_System);
	};
	static double Convert(double a_Value, const OrUnitsConverter::OrDefUnit& a_SrcUnit, const OrUnitsConverter::OrDefUnit& a_DestUnit,
		OrUnitsConverter::OrResUnit* a_ResUnit = nullptr, const std::string& a_System = "")
	{
		return m_Converter.Convert(a_Value, a_SrcUnit, a_DestUnit, a_ResUnit, a_System);
	};
	static bool Load(const std::string& a_FileName)
	{
		return m_Converter.Load(a_FileName);
	};
	static std::string get_Quantity(const std::string& a_UnitName)
	{
		return m_Converter.get_Quantity(a_UnitName);
	};
	static std::vector<std::string> get_Quantities(const OrUnitsConverter::OrDefUnit& a_UnitName)
	{
		return m_Converter.get_Quantities(a_UnitName);
	};
	static bool CheckUnits(const OrUnitsConverter::OrDefUnit& a_SrcUnit, const OrUnitsConverter::OrDefUnit& a_DestUnit, 
		OrCheckUnits* a_CheckUnits = nullptr)
	{
		return m_Converter.CheckUnits(a_SrcUnit, a_DestUnit, a_CheckUnits);
	}
private:
	static OrUnitsConverter m_Converter;
};

