#include "TEST_CairnCore.h"
#include <iostream>
#include "Utils.h"

using namespace std;

/*
* ToDo list
-to fix the LiRet Logic
*/

int main()
{
	//Variables
	int liRet = 0;
	CairnAPI m_Persee;
	string const lsFilePath = TEST_DATA + (std::string)"/ModelClassListUnitTest.txt";
	t_list AllModelClassList;

	//Get the Reference Data
	vector<vector<string>> ReferenceData = TestUtils::ParserTxt(lsFilePath);	
	liRet = TestUtils::CreateRefrenceList(ReferenceData, AllModelClassList);
	//TestUtils::Display_list(AllModelClassList);
	
	TestUtils::Display_list(m_Persee.get_PossibleModelNames());
	TestUtils::Display_list(m_Persee.get_EnergyCarrierTypes());
	TestUtils::Display_list(m_Persee.get_PossibleComponentTypes());

	t_list vCats = m_Persee.get_PossibleComponentTypes();
	for (auto& vCat : vCats) {
		t_list vTechnos = m_Persee.get_TechnoTypes(vCat);
		TestUtils::Display_list(vTechnos);

		for (auto& vTechnoType : vTechnos) {
			t_list vRet = m_Persee.get_Models(vTechnoType);
			TestUtils::Display_list(vRet);

		}
	}
	return noError;
}

