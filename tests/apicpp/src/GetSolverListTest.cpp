#include "TEST_CairnCore.h"
#include <iostream>
#include "StudyCTest.h"

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
	vector<t_list> lsSolverList = TestUtils::ParserTxt(lsSolverListFilePath);
	TESTAPIBOOL("Create Ref data", TestUtils::CreateReferenceList(lsSolverList, AllSolverList));

#ifndef 	USE_CPLEX
	t_list::iterator vIter = find(AllSolverList.begin(), AllSolverList.end(), "Cplex");
	if (vIter != AllSolverList.end())
		AllSolverList.erase(vIter);
#endif
	t_list vRet = m_Cairn.get_Solvers();
	TESTAPIBOOL("Create solver list", TestUtils::compare_lists(vRet, AllSolverList));

	return noError;
}

