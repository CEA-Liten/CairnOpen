#pragma once
#include "OrObject.h"
class OrUnit :
    public OrObject
{
public:
    OrUnit(OrObject* ap_Parent, const std::string& a_Kind);
    virtual ~OrUnit(void);

	virtual bool Add(const std::string& a_Kind);
	virtual void InitProperties();

	double ConvertToSI(double a_Value);
	double ConvertFromSI(double a_Value);

	bool IsSI();
	double get_A() const;
	double get_B() const;
	double get_C() const;
	double get_D() const;
	bool IsInv() const;

private:
	long m_AIdx;
	long m_BIdx;
	long m_CIdx;
	long m_DIdx;
	long m_InvIdx;

	double m_kA;
	double m_kB;
	bool m_IsSI;
};

