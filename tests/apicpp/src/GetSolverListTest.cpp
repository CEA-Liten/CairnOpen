#include "TEST_CairnCore.h"
#include <iostream>
#include "Utils.h"

using namespace std;


int main()
{
	//Variables	
	CairnAPI m_Cairn;
	t_list AllSolverList; 
#if defined(WIN32) || defined(_WIN32)
	string const lsSolverListFilePath = TEST_DATA + (std::string)"/SolverReferenceListUnitTest.txt";
#else
	string const lsSolverListFilePath = TEST_DATA + (std::string)"/SolverLinuxReferenceListUnitTest.txt";
#endif

	//Get the Reference Data
	vector<vector<string>> lsSolverList = TestUtils::ParserTxt(lsSolverListFilePath);
	TestUtils::CreateRefrenceList(lsSolverList, AllSolverList);
	
	t_list vRet = m_Cairn.get_Solvers();
	return TestUtils::compare_lists(vRet, AllSolverList);	
}

