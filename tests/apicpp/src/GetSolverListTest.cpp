#include "TEST_CairnCore.h"
#include <iostream>
#include "Utils.h"

using namespace std;


int main()
{
	//Variables	
	CairnAPI m_Cairn;
	t_list AllSolverList; 
	string const lsSolverListFilePath = TEST_DATA + (std::string)"/SolverReferenceListUnitTest.txt";

	//Get the Reference Data
	vector<vector<string>> lsSolverList = TestUtils::ParserTxt(lsSolverListFilePath);
	TestUtils::CreateRefrenceList(lsSolverList, AllSolverList);
	
	t_list vRet = m_Cairn.get_Solvers();
	return TestUtils::compare_lists(vRet, AllSolverList);	
}

