#include "TEST_CairnCore.h"
#include <iostream>
#include "StudyCTest.h"

using namespace std;

/*
* ToDo list
-to fix the LiRet Logic
*/

int main()
{
	//Variables
	CairnAPI m_Cairn;
	string const lsFilePath = TEST_DATA + (std::string)"/ModelClassListUnitTest.txt";
	t_list AllModelClassList;

	//Get the Reference Data
	vector<t_list> ReferenceData = TestUtils::ParserTxt(lsFilePath);	
	TESTAPIBOOL("Create Ref data", TestUtils::CreateReferenceList(ReferenceData, AllModelClassList));
	
	TestUtils::Display_list(m_Cairn.get_PossibleModelNames());
	TestUtils::Display_list(m_Cairn.get_EnergyCarrierTypes());
	TestUtils::Display_list(m_Cairn.get_PossibleComponentTypes());

	t_list vCats = m_Cairn.get_PossibleComponentTypes();
	for (auto& vCat : vCats) {
		t_list vTechnos = m_Cairn.get_TechnoTypes(vCat);
		TestUtils::Display_list(vTechnos);

		for (auto& vTechnoType : vTechnos) {
			t_list vRet = m_Cairn.get_Models(vTechnoType);
			TestUtils::Display_list(vRet);

		}
	}
	return noError;
}

