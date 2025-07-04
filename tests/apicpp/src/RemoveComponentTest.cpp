#include "TEST_CairnCore.h"
#include <iostream>
#include "Utils.h"
#include "UtilsJson.h"

using namespace std;

/* This test reads the study formation_persee.json and formation_persee_dataseries.csv.
   Then, it deletes a component and re-add it again before executing a simulation.

   The result of the simulation is compared to the reference formation_persee_Results_Reference.csv
   The reference is generated by running the original formation_persee.json using the GUI

   After, as an additional test: it saves the study as AfterReAddingComponent.json, then re-reads AfterReAddingComponent.json 
   and performs another simulation before comparing to the reference formation_persee_Results_Reference.csv again
*/

int main()
{
	string const StudyRoot = TEST_RESULTS + (std::string)"/removeComp/";
	std::string vFileName = StudyRoot + (std::string)"/formation_persee.json";
	string const TimeseriesFileName = StudyRoot + (std::string)"/formation_persee_dataseries.csv";
	string const ResultFileName = StudyRoot + "formation_persee_Results.csv";

	string const AfterReAddingComponentFileName = StudyRoot + (std::string)"/AfterReAddingComponent.json";
	string const ResultFileNameAfterReAddingComponent = StudyRoot + (std::string)"/AfterReAddingComponent_Results.csv";

	if (fs::exists(StudyRoot)) {
		fs::remove_all(StudyRoot);
	}
	if (!fs::exists(TEST_RESULTS)) {
		fs::create_directory(TEST_RESULTS);
	}
	fs::create_directory(StudyRoot);
	fs::copy_file(TEST_DATA + (std::string)"/formation_persee.json", vFileName);
	fs::copy_file(TEST_DATA + (std::string)"/formation_persee_dataseries.csv", TimeseriesFileName);

	string const ReferenceResultFileName = TEST_DATA + (std::string)"/formation_persee_Results_Reference.csv";

	string componentToBeRemoved = "H2_Load";

	CairnAPI m_Persee;
	CairnAPI::OptimProblemAPI m_Problem;
	TESTAPI("read study : " + vFileName, m_Problem = m_Persee.read_Study(vFileName))

	CairnAPI::MilpComponentAPI vDelComp = m_Problem.get_Component(componentToBeRemoved);
	TESTAPI("remove component : " + componentToBeRemoved, m_Problem.remove(vDelComp))

	t_list ListAfterRemove = m_Problem.get_Components();
	t_list::iterator vIter = find(ListAfterRemove.begin(), ListAfterRemove.end(), componentToBeRemoved);
	bool found = (vIter != ListAfterRemove.end());
	if (!found) {
		CairnAPI::MilpComponentAPI vH2_Load("H2_Load", "SourceLoad");
		vH2_Load.set_SettingValues({
			{"Direction", "Sink"},
			{"LPModelONLY", false},
			{"Weight", "1"},
			{"Opex", "0" },
			{"Capex", "0" },
			{"MaxFlow", "1000"},
			{"EcoInvestModel", "1"},
			{"UseProfileLoadFlux","H2_Load.LoadMassFlowrate"}
			}
		);
		CairnAPI::EnergyVectorAPI vH2 = m_Problem.get_EnergyCarrier("H2");
		CairnAPI::MilpPortAPI vH2_Load_L0("PortL0", vH2);
		vH2_Load_L0.set_SettingValues({
					{"Direction", "INPUT"},
					{"Variable", "SourceLoadFlow"}
		});

		TESTAPI("add port L0 to H2_Load", vH2_Load.add(vH2_Load_L0))
		TESTAPI("add H2_Load", m_Problem.add(vH2_Load))

		CairnAPI::BusAPI vBusH2 = m_Problem.get_Bus("H2_Bus");
		TESTAPI("Re - create link",
			m_Problem.add(vBusH2, vH2_Load_L0)
		)

		TESTAPI("Read the Timeseries from the file path : " + TimeseriesFileName,
			m_Problem.add_TimeSeries(TimeseriesFileName)
		)

		CairnAPI::SolutionAPI vSolution;
		TESTAPI("Run",
			vSolution = m_Problem.run()
		)
		vSolution.exportTimeSeries();

		TESTAPI2("Compare results",
			TestUtils::ComparaisonCsvFile(ResultFileName, ReferenceResultFileName)
		)

		//Additional test: read the saved file AfterReAddingComponentFileName and perform  a simulation
		// This test is similar to SaveStudyTest.cpp but within a different scenario
		TESTAPI("Write Model File after the re-adding component in the file path : " + AfterReAddingComponentFileName,
			m_Problem.save_Study(AfterReAddingComponentFileName)
		)

		CairnAPI m_Persee2;
		CairnAPI::OptimProblemAPI m_Problem2;

		TESTAPI("read study file from the file path : " + AfterReAddingComponentFileName,
			m_Problem2 = m_Persee2.read_Study(AfterReAddingComponentFileName)
		)

		TESTAPI("Read the Timeseries from the file path : " + TimeseriesFileName,
			m_Problem2.add_TimeSeries(TimeseriesFileName)
		)

		CairnAPI::SolutionAPI vSolution2;
		TESTAPI("Run 2",
			vSolution2 = m_Problem2.run()
		)
		vSolution2.exportTimeSeries();

		TESTAPI2("Compare results 2",
			TestUtils::ComparaisonCsvFile(ResultFileNameAfterReAddingComponent, ReferenceResultFileName)
		)
	}
	else
	{
		//Test Not Ok - the Size of list of settings does not conforms to the expected list
		cout << "There are no missing component in the list after the remove Method ";
		return errSize;
	}

	return noError;
}