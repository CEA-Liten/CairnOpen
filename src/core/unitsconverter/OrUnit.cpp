#include "OrUnit.h"
#include "OrParam.h"
#include "OrUnitsConverter.h"
#include "OrQuantities.h"
#include "OrDispUnitRef.h"

OrUnit::OrUnit(OrObject* ap_Parent, const std::string& a_Kind)
	: OrObject(ap_Parent)
{
	// Name contient le nom (unique -> ID) de l'unité
	m_Kind = a_Kind;	// c'est la grandeur 'Quantity' (temperature, pressure,...)	

	m_AIdx = AddParam("A", new OrParam(1.0));
	m_BIdx = AddParam("B", new OrParam(1.0));
	m_CIdx = AddParam("C", new OrParam(0.0));
	m_DIdx = AddParam("D", new OrParam(1.0));
	m_InvIdx = AddParam("inverse", new OrParam(false));

	m_kA = 1.0;
	m_kB = 0.0;
	m_IsSI = true;
	m_ElemsName = "alias";
}

OrUnit::~OrUnit(void)
{
}

bool OrUnit::Add(const std::string& a_Kind)
{
	OrObject::Add(new OrDispUnitRef(this));
	return true;
}

void OrUnit::InitProperties()
{
	double vA = get_A();
	double vB = get_B();
	double vC = get_C();
	double vD = get_D();
	if (vB)
		m_kA = vA / vB;
	else
		m_kA = vA;
	if (!m_kA)
		m_kA = 1.0;

	if (vD)
		m_kB = vC / vD;
	else
		m_kB = vC;

	if (m_kA == 1.0 && m_kB == 0.0)
		m_IsSI = true;
	else
		m_IsSI = false;

	// initialise les liens entre unités/grandeurs
	OrQuantities* pQuantities = (OrQuantities*)p_Root->get_ElemByKey(OrUnitsConverter::eQuantities);
	pQuantities->Create(this);
}

double OrUnit::ConvertToSI(double a_Value)
{
	double vRet = m_kA * a_Value + m_kB;

	if (IsInv()) {
		if (vRet != 0.0)
			vRet = 1.0 / vRet;
		else
			return a_Value;
	}
	return vRet;
}

double OrUnit::ConvertFromSI(double a_Value)
{
	double vRet = a_Value;

	if (IsInv()) {
		if (vRet != 0.0)
			vRet = 1.0 / vRet;
		else
			return a_Value;
	}
	return (vRet - m_kB) / m_kA;
}

bool OrUnit::IsSI()
{
	return m_IsSI;
}

double OrUnit::get_A() const
{
	return m_Params[m_AIdx]->get_DoubleValue();
}

double OrUnit::get_B() const
{
	return m_Params[m_BIdx]->get_DoubleValue();
}

double OrUnit::get_C() const
{
	return m_Params[m_CIdx]->get_DoubleValue();
}

double OrUnit::get_D() const
{
	return m_Params[m_DIdx]->get_DoubleValue();
}

bool OrUnit::IsInv() const
{
	return m_Params[m_InvIdx]->get_BoolValue();
}
