#include "CairnAPI.h"
#include "CairnCore.h"
#include "CairnAPIUtils.h"
#include "InputParam.h"
using namespace CairnAPIUtils;

CairnAPI::EnergyVectorAPI::EnergyVectorAPI(EnergyVector* ap_EnergyVector)
	: CairnAPI::ObjectAPI(ap_EnergyVector)
{	
}

CairnAPI::EnergyVectorAPI::EnergyVectorAPI(const OptimProblemAPI& a_Problem, 
	const std::string& a_Name, const std::string& a_Type)
	: CairnAPI::ObjectAPI()
{
	*this = a_Problem.create_EnergyCarrier(a_Name, a_Type);
}

EnergyVector* CairnAPI::EnergyVectorAPI::get_EnergyVector() const
{
	return (EnergyVector*)get_Object();
}

void CairnAPI::EnergyVectorAPI::set_EnergyVector(EnergyVector* ap_EnergyVector)
{
	set_Object(ap_EnergyVector);
}

std::string CairnAPI::EnergyVectorAPI::get_Type() const
{
	if (m_Object) {
		EnergyVector* pEnergyVector = (EnergyVector*)m_Object;
		return pEnergyVector->Type();
	}
	return "";
}

// Set the value of a parameter
void CairnAPI::EnergyVectorAPI::set_SettingValue(const std::string& a_SettingName, const t_value& a_SettingValue, bool checkExistance)
{	
	ECodeError vRet = noError;
	if (m_Object) {
		try
		{
			CairnAPI::ObjectAPI::set_SettingValue(a_SettingName, a_SettingValue);
			EnergyVector* pEnergyVector = (EnergyVector*)m_Object;
			bool vOk = pEnergyVector->InitEnergyVectorParam();
			vRet = (vOk) ? noError : errParam;
		}
		catch (const std::exception&)
		{
			vRet = errParam;
		}
	}	
	CairnAPIUtils::setError(vRet);	
}

// Set the values of several parameters
void CairnAPI::EnergyVectorAPI::set_SettingValues(const t_dict& a_SettingValues)
{
	ECodeError vRet = noError;
	if (m_Object) {
		try
		{
			CairnAPI::ObjectAPI::set_SettingValues(a_SettingValues);
			EnergyVector* pEnergyVector = (EnergyVector*)m_Object;
			bool vOk = pEnergyVector->InitEnergyVectorParam();
			vRet = (vOk) ? noError : errParam;
		}
		catch (const std::exception&)
		{
			vRet = errParam;
		}
	}	
	CairnAPIUtils::setError(vRet);	
}
