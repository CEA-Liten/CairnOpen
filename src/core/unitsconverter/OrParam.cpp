// *********************************************************
//	Orionde
//   JCh-2009
// *********************************************************
//
#include "OrParam.h"
#include "OrJsonUtils.h"

OrParam::OrParam(const std::string &a_Value, bool a_Save)
{
	m_Type = eString;
	m_Value = a_Value;
	m_Save = a_Save;
}

OrParam::OrParam(bool a_Value, bool a_Save)
{
	m_Type = eBool;
	m_Value = a_Value;
	m_Save = a_Save;
}

OrParam::OrParam(int a_Value, bool a_Save) 
{
	m_Type = eInt;
	m_Value = a_Value;
	m_Save = a_Save;
}

OrParam::OrParam(double a_Value, bool a_Save) 
{
	m_Type = eDouble;
	m_Value = a_Value;
	m_Save = a_Save;
}

OrParam::~OrParam(void)
{
}

void OrParam::EnableSave(bool a_Save)
{
	m_Save = a_Save;
}

const bool &OrParam::Save() const
{
	return m_Save;
}

void OrParam::SaveProperties(ojson& a_Group, const std::string& a_Name)
{
	if (m_Save) {
	//	ap_Group->AddProperty(a_Name, (std::string)get_Value());
	}
}

void OrParam::LoadProperties(const json& a_Group, const std::string& a_Name)
{
	if (m_Type == eBool) {
		bool vValue;
		if (orjson::from_json(a_Group, a_Name, vValue))	
			set_Value(vValue);
	}
	else if (m_Type == eInt) {
		int vValue;
		if (orjson::from_json(a_Group, a_Name, vValue))
			set_Value(vValue);
	}
	else if (m_Type == eDouble) {
		double vValue;
		if (orjson::from_json(a_Group, a_Name, vValue))
			set_Value(vValue);
	}
	else {
		std::string vValue;
		if (orjson::from_json(a_Group, a_Name, vValue))
			set_Value(vValue);
	}				
}

void OrParam::set_Value(const std::string &a_Value)
{
	if (m_Type == eBool) {
		
	}		
	else if (m_Type == eInt) {
		
	}
	else if (m_Type == eDouble) {

	}			
	else
		m_Value = a_Value;
}

void OrParam::set_Value(const int &a_Value)
{
	if (m_Type == eInt)
		m_Value = a_Value;
}

void OrParam::set_Value(const bool &a_Value)
{
	if (m_Type == eBool)
		m_Value = a_Value;
}

void OrParam::set_Value(const double &a_Value)
{
	if (m_Type == eDouble)
		m_Value = a_Value;
}

void OrParam::set_Value(OrParam &a_Value)
{
	m_Type = a_Value.get_Type();
	if (m_Type == eBool)
		m_Value = a_Value.get_BoolValue();
	else if (m_Type == eInt)
		m_Value = a_Value.get_LongValue();
	else if (m_Type == eDouble)
		m_Value = a_Value.get_DoubleValue();
	else
		m_Value = a_Value.get_StrValue();
}

std::string OrParam::get_Value()
{
	std::string vRet;
	if (m_Type == eBool)
		return std::to_string(get_BoolValue());	
	else if (m_Type == eInt)
		return std::to_string(get_LongValue());
	else if (m_Type == eDouble)
		return std::to_string(get_DoubleValue());
	else
		vRet = get_StrValue();
	return vRet;
}

const std::string &OrParam::get_StrValue()
{
	return std::get<std::string> (m_Value);
}

const bool &OrParam::get_BoolValue()
{
	return std::get<bool>(m_Value);
}

const int &OrParam::get_LongValue()
{
	return std::get<int>(m_Value);
}

const double &OrParam::get_DoubleValue()
{
	return std::get<double>(m_Value);
}

const OrParam::EParamType &OrParam::get_Type() const
{
	return m_Type;
}
