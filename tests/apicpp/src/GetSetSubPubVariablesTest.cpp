#include "TEST_CairnCore.h"
#include <iostream>
#include "Utils.h"
#include "UtilsJson.h"

using namespace std;

/* This test reads study test_compressor.json and test_compressor_dataseries.csv

   Then, it executes a simulation and compares the result to the reference test_compressor_Results_Reference.csv
   which is generated using the GUI.
*/

int main()
{
	CairnAPI m_Cairn;
	CairnAPI::OptimProblemAPI m_Problem;

	string const Study = "formation_cairn_coSim";
	string const StudyRoot = TEST_RESULTS + (std::string)"/getsetpubsub/";



	std::string vFileName = StudyRoot + Study + ".json";
	string const ResultFileName = StudyRoot + Study + "_results_Results.csv";

	if (fs::exists(StudyRoot)) {
		fs::remove_all(StudyRoot);
	}
	if (!fs::exists(TEST_RESULTS)) {
		fs::create_directory(TEST_RESULTS);
	}
	fs::create_directory(StudyRoot);
	fs::copy_file(TEST_DATA + (std::string)"/" + Study + ".json", vFileName);

	string const ReferenceResultFileName = TEST_DATA + (std::string)"/" + Study + (std::string)"_Results_Reference.csv";


	TESTAPI("read study file from the file path: " + vFileName,
		m_Problem = m_Cairn.read_Study(vFileName)
	)
	
	t_list vSubVars = m_Problem.getSubscribedVariables();
	TESTAPI2FALSE("getSubscribedVariables", vSubVars.size() > 0);

	CairnAPI::MilpComponentAPI h2Tank = m_Problem.get_Component("H2_Tank");
	
	//m_Problem.initialize();

	vector<double> vInValues;
	vInValues.assign(60, 12);
	m_Problem.setSubscribedVariableValue("Elec_Grid.UseProfileBuyPrice.Elec_Grid.ElectricityPrice", vInValues);
	
	vector<double> vInValues2 = { 1.0, 2.0, 3.0 };
	m_Problem.setSubscribedVariableValue("H2_Load.UseProfileLoadFlux.H2_Load.LoadMassFlowrate", vInValues2);

	//Problème : la taille dans Cairn est 0 donc je n'arrive pas à mettre les valeurs dans un vecteur Hist
	vector<double> vInValuesHistEStock = { 0.71, 0.72, 0.73, 0.74, 0.75, 0.76, 0.77, 0.78, 0.79, 0.8, 0.81, nan(""), nan(""), nan(""), nan(""), nan(""), nan(""), nan(""), nan(""), nan(""), nan(""), nan("") };
	m_Problem.setSubscribedVariableValue("H2_Tank.MPCEstock", vInValuesHistEStock);

	vector<double> vInValuesHistState = { 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, nan(""), nan(""), nan(""), nan(""), nan(""), nan(""), nan(""), nan(""), nan(""), nan(""), nan("") };
	m_Problem.setSubscribedVariableValue("H2_Tank.MPCState", vInValuesHistState);

	CairnAPI::SolutionAPI vSolution;
	TESTAPI("Run 1",
		vSolution = m_Problem.run("", true)
	)

	vInValuesHistEStock = { 0.81, 0.82, 0.83, 0.84, 0.85, 0.86, 0.87, 0.88, 0.89, 0.9, 0.91, nan(""), nan(""), nan(""), nan(""), nan(""), nan(""), nan(""), nan(""), nan(""), nan(""), nan("") };
	m_Problem.setSubscribedVariableValue("H2_Tank.MPCEstock", vInValuesHistEStock);

	TESTAPI("Run 2",
		m_Problem.run("", true)
	)

	vInValuesHistEStock = { 0.61, 0.62, 0.63, 0.64, 0.65, 0.66, 0.67, 0.68, 0.69, 0.7, nan(""), nan(""), nan(""), nan(""), nan(""), nan(""), nan(""), nan(""), nan(""), nan(""), nan(""), nan("") };
	m_Problem.setSubscribedVariableValue("H2_Tank.MPCEstock", vInValuesHistEStock);

	TESTAPI("Run 3",
		m_Problem.run("", true)
	)

	t_list vPubVars = m_Problem.getPublishedVariables();
	TESTAPI2FALSE("getPublishedVariables", vPubVars.size() > 0);

	vector<double> vValues = m_Problem.getPublishedVariableValue("Elec_Grid_Inject.GridPrice");
	TESTAPI2FALSE("getPublishedVariableValue", vValues.size() > 0);

	TESTAPIFALSE("error, variable does not exist", m_Problem.getPublishedVariableValue("Nothing"));

	vValues = m_Problem.getPublishedVariableValue("H2_Tank.Estock");

	std::vector<double> histStock = h2Tank.getControlVarHistValues("Estock");
	TESTAPI2FALSE("getControlVarHistValues size", histStock.size()== 22);
	TESTVALUE(histStock[0], 0.61);
	TESTVALUE(histStock[9], 0.7)

	return noError;
}