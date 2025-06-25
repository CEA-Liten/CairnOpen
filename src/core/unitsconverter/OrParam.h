// *********************************************************
//	Orionde
//   JCh-2009
// *********************************************************
//
#pragma once
#include "COrHeader.h"

class OrParam
{
public:
	OrParam(const std::string &a_Value, bool a_Save = true);
	OrParam(double a_Value, bool a_Save = true);
	OrParam(int a_Value, bool a_Save = true);
	OrParam(bool a_Value, bool a_Save = true);
	
	virtual ~OrParam(void);
	virtual void set_Value(const std::string &a_Value);
	virtual void set_Value(const double &a_Value);
	virtual void set_Value(const int &a_Value);
	virtual void set_Value(const bool &a_Value);
	
	void set_Value(OrParam &a_Value);
	virtual std::string get_Value();
	virtual const std::string &get_StrValue();
	virtual const bool &get_BoolValue();
	virtual const int &get_LongValue();
	virtual const double &get_DoubleValue();
		
	
	void EnableSave(bool a_Save);
	const bool &Save() const;
	virtual void SaveProperties(ojson& a_Group, const std::string &a_Name);
	virtual void LoadProperties(const json& a_Group, const std::string &a_Name);

	enum EParamType {
		eInt=0,
		eDouble,
		eString,
		eBool
	};
	const EParamType&get_Type() const;
protected:
	EParamType m_Type;
	bool m_Save;

	std::variant<int, double, std::string, bool> m_Value;			
};
