#include "CairnAPIUtils.h"
#include "TEST_PerseeLib.h"
#include <iostream>
#include "UtilsJson.h"
#include "Utils.h"
#include <iomanip>

int main()
{

	string const StudyName = "createStudy";
	string const StudyRoot = TEST_RESULTS + (std::string)"/createStudy/";

	string const vFileName = StudyRoot + StudyName + ".json";
	string const TimeseriesFileName = StudyRoot + (std::string)"/formation_persee_dataseries.csv";
	string const ScenarioName = "scenario1";
	string const ResultFileName = StudyRoot + ScenarioName + "/" + StudyName + "_Results.csv";

	string const vFileName_saved = StudyRoot + StudyName + "_saved.json";
	string const ResultFileName_saved = StudyRoot + StudyName + "_saved_Results.csv";

	if (fs::exists(StudyRoot)) {
		fs::remove_all(StudyRoot);
	}
	if (!fs::exists(TEST_RESULTS)) {
		fs::create_directory(TEST_RESULTS);
	}
	fs::create_directory(StudyRoot);
	fs::copy_file(TEST_DATA + (std::string)"/formation_persee_dataseries.csv", TimeseriesFileName);

	string const ReferenceResultFileName = TEST_DATA + (std::string)"/formation_persee_Results_Reference.csv";

	int liRet = 0;
		
	CairnAPI m_Persee;
	// création d'une nouvelle étude:
	cout << "Creation of a new study" << endl;	
	CairnAPI::OptimProblemAPI m_Problem = m_Persee.create_Study(StudyRoot + StudyName);

	// création du TechEcoAnalysis	
	TESTAPI("set_TechEcoAnalysisSettings",
		m_Problem.set_TechEcoAnalysisSettings(
			{
				{"DiscountRate", 0.07},
				{"NbYear", 20},
				{"Range","HISTandPLAN"},
				{"ForceExportAllIndicators",  true}, 
				{"ConsideredEnvironmentalImpacts", "Climate change#Global Warming Potential 100,Acidification#Accumulated Exceedance"}
			}
		)
	)

	// création du SimulationControl	
	TESTAPI("create_simulationcontrol",
		m_Problem.set_SimulationControlSettings (
			{	
				{"ExportJson", 1},
				{"ExportResults", 1},
			    {"FutureSize", 48},
				{"FutureVariableTimestep", 8760}
			}
		)
	)

	// création du Solver
	TESTAPI("create_MIPSolver",
		m_Problem.set_MIPSolverSettings(
			{
				{"Solver", "Cplex"},
				{"WriteLp", "YES"},
			    {"Gap", 0.001},
			    {"TimeLimit", 0}
			})
	)

	// création des EnergyVector
	CairnAPI::EnergyVectorAPI vElec("ElectricityDistrib", "Electrical");
	TESTAPI("set_EnergyVector ElectricityDistrib",
		vElec.set_SettingValue("Potential", 100)
	)
	TESTAPI("set_EnergyVector ElectricityDistrib",
		vElec.set_SettingValue( "Potential", 220 )			
	)
	TESTAPI("set_EnergyVector ElectricityDistrib",
			vElec.set_SettingValue("UseProfileBuyPrice", "Elec_Grid.ElectricityPrice"  )
	)

	TESTAPI("add_EnergyVector ElectricityDistrib",	
		m_Problem.add(vElec)
	)
	
	CairnAPI::EnergyVectorAPI vH2("H2", "FluidH2");
	TESTAPI("create_EnergyVector H2",
		vH2.set_SettingValues(
			{
				{"Potential", 30},
				{"LHV", 0.03332},
				{"RHO", 0.0899},
				{"IsMassCarrier", true},
				{"IsFuelCarrier", true}
			})
	)

	TESTAPI("add_EnergyVector H2",
		m_Problem.add(vH2)
	)

	t_list vEVs = m_Problem.get_EnergyCarriers();
	t_list vRef = { "H2","ElectricityDistrib" };
	TESTAPI2("check list Energy Vector", TestUtils::compare_lists(vRef, vEVs))

	// ajout d'un composant
	// -------------------------------------------------------------------
	CairnAPI::MilpComponentAPI vELY_PEM("ELY_PEM", "Electrolyzer");
	vELY_PEM.set_SettingValues({
		{ "ModelClass", "Electrolyzer" },  
		{ "Capex", 480000 },
		{ "Opex", "0.04" },
		{ "Efficiency", "0.6667" },
		{ "MaxPower", "-30" },
		{ "MinPower", "0" },
		{ "ComputeProductionCost", "H2MassFlowRate" },
		{"EnvironmentModel", true}, //It is necessary to set to true in order to use EnvImpacts
		{"Climate change#Global Warming Potential 100 EnvGreyContentCoefficient_A", 100},
		{"Climate change#Global Warming Potential 100 EnvGreyContentOffset_B", 0},
		{"Climate change#Global Warming Potential 100 EnvGreyReplacement", 10}, 
		{"Climate change#Global Warming Potential 100 EnvGreyReplacementConstant", 0},
		{"Acidification#Accumulated Exceedance EnvGreyContentCoefficient_A", 0},
		{"Acidification#Accumulated Exceedance EnvGreyContentOffset_B", 0},
		{"Acidification#Accumulated Exceedance EnvGreyReplacement", 0},
		{"Acidification#Accumulated Exceedance EnvGreyReplacementConstant", 0},
		{"PortL0.Climate change#Global Warming Potential 100 EnvContentCoefficient_A", 0},  
		{"PortL0.Climate change#Global Warming Potential 100 EnvContentOffset_B", 0},
	    {"PortL0.Acidification#Accumulated Exceedance EnvContentCoefficient_A", 0},        
		{"PortL0.Acidification#Accumulated Exceedance EnvContentOffset_B", 0}
		}
	);

	// Le type du port est le vecteur d'énergie
	CairnAPI::MilpPortAPI vELY_PEM_L0("PortL0", vElec);
	vELY_PEM_L0.set_SettingValues({
		{"Direction", "INPUT"},
		{"Variable", "UsedPower"}
	});

	CairnAPI::MilpPortAPI vELY_PEM_R0("PortR0", vH2);
	vELY_PEM_R0.set_SettingValues({
		{"Direction", "OUTPUT"},
		{"Variable", "H2MassFlowRate"}
	});

	TESTAPI("add port L0 to ELY_PEM",
		vELY_PEM.add(vELY_PEM_L0)
	)

	TESTAPI2("check list components", TestUtils::compare_lists(m_Problem.get_Components(), {  }))

	//add a port after creating the component
	TESTAPI("add port R0 to ELY_PEM",
		vELY_PEM.add(vELY_PEM_R0)
	)

	//Set port EnvImpacts
	vELY_PEM.set_SettingValues(
			{
			{"PortR0.Climate change#Global Warming Potential 100 EnvContentCoefficient_A", 0},
			{"PortR0.Climate change#Global Warming Potential 100 EnvContentOffset_B", 0},
			{"PortR0.Acidification#Accumulated Exceedance EnvContentCoefficient_A", 0},
			{"PortR0.Acidification#Accumulated Exceedance EnvContentOffset_B", 0}
			}
	);

	//add the componenet
	TESTAPI("add ELY_PEM",
		m_Problem.add(vELY_PEM)
	)



	t_list vPorts = vELY_PEM.get_Ports();

	//Verify Get and Set methods
	TESTAPI2("Verify the value of ELY_PEM.Capex.",
		TestUtils::compare_scalar(vELY_PEM.get_SettingValue("Capex"), 480000.0, eDouble)
	)

	vELY_PEM.set_SettingValue("Capex", 100000.0);

	TESTAPI2("Verify the value of ELY_PEM.Capex.",
		TestUtils::compare_scalar(vELY_PEM.get_SettingValue("Capex"), 100000.0, eDouble)
	)

	vELY_PEM.set_SettingValue("Capex", 480000.0);


	vELY_PEM_R0.set_SettingValue("Coeff", 2.0);

	TESTAPI2("Verify the value of ELY_PEM.PortR0.Coeff",
		TestUtils::compare_scalar(vELY_PEM_R0.get_SettingValue("Coeff"), 2.0, eDouble)
	)

	vELY_PEM_R0.set_SettingValue("Coeff", 1.0);

	//Verify componenet list and port list
	TESTAPI2("check list components", TestUtils::compare_lists(m_Problem.get_Components(), {"ELY_PEM"}))
	TESTAPI2("check list ports", TestUtils::compare_lists(vPorts, vELY_PEM.get_Ports()))

	// ajout d'un composant
	// -------------------------------------------------------------------
	CairnAPI::MilpComponentAPI vElecGrid("Elec_Grid", "GridFree");	
	vElecGrid.set_SettingValues({
		{"MaxFlow","400"},
		{"Direction", "ExtractFromGrid"},
		{"EnvironmentModel", true}, //It is necessary to set to true in order to use EnvImpacts
		{"Climate change#Global Warming Potential 100 EnvGreyContentCoefficient_A", 0},
		{"Climate change#Global Warming Potential 100 EnvGreyContentOffset_B", 0},
		{"Climate change#Global Warming Potential 100 EnvGreyReplacement", 0},
		{"Climate change#Global Warming Potential 100 EnvGreyReplacementConstant", 0},
		{"Acidification#Accumulated Exceedance EnvGreyContentCoefficient_A", 0},
		{"Acidification#Accumulated Exceedance EnvGreyContentOffset_B", 0},
		{"Acidification#Accumulated Exceedance EnvGreyReplacement", 0},
		{"Acidification#Accumulated Exceedance EnvGreyReplacementConstant", 0},
		{"PortR0.Climate change#Global Warming Potential 100 EnvContentCoefficient_A", 20},
		{"PortR0.Climate change#Global Warming Potential 100 EnvContentOffset_B", 0},
		{"PortR0.Acidification#Accumulated Exceedance EnvContentCoefficient_A", 0.5},
		{"PortR0.Acidification#Accumulated Exceedance EnvContentOffset_B", 0}
		}
	);

	CairnAPI::MilpPortAPI vElecGrid_R0("PortR0", vElec);
	vElecGrid_R0.set_SettingValues({
				{"Direction", "OUTPUT"},
				{"Variable", "GridFlow"}
		});

	
	TESTAPI("add port R0 to Elec_Grid",
		vElecGrid.add(vElecGrid_R0)
	)
		
	TESTAPI("add Elec_Grid",
		m_Problem.add(vElecGrid)
	)

	// ajout d'un composant
	// -------------------------------------------------------------------
	CairnAPI::MilpComponentAPI vElecGridInject("Elec_Grid_Inject", "GridFree");	
	vElecGridInject.set_SettingValues({
		{"Direction", "InjectToGrid"} ,
		  {"MaxFlow", "1500000"}
		}
	);

	CairnAPI::MilpPortAPI vElecGridInject_R0("PortR0", vElec);
	vElecGridInject_R0.set_SettingValues({
				{"Direction", "INPUT"},
				{"Variable", "GridFlow"}
	});


	TESTAPI("add port R0 to Elec_Grid_Inject",
		vElecGridInject.add(vElecGridInject_R0)
	)

	TESTAPI("add Elec_Grid_Inject",
		m_Problem.add(vElecGridInject)
	)

	// ajout d'un composant
	// -------------------------------------------------------------------
	CairnAPI::MilpComponentAPI vH2_Load("H2_Load", "SourceLoad");	
	vH2_Load.set_SettingValues({
		{"Direction", "Sink" },
		{"LPModelONLY", false},
		{"Weight", "1"},
		{"Opex", "0" },
		{"Capex", "0" },
		{"MaxFlow", "1000"},
		{"EcoInvestModel", "1"},
		{"UseProfileLoadFlux","H2_Load.LoadMassFlowrate"}
		}
	);
	CairnAPI::MilpPortAPI vH2_Load_L0("PortL0", vH2);
	vH2_Load_L0.set_SettingValues({
				{"Direction", "INPUT"},
				{"Variable", "SourceLoadFlow"}
		});


	TESTAPI("add port L0 to H2_Load",
		vH2_Load.add(vH2_Load_L0)
	)

	TESTAPI("add H2_Load",
		m_Problem.add(vH2_Load)
	)


	// ajout d'un composant
	// -------------------------------------------------------------------
	CairnAPI::MilpComponentAPI vWind_farm("Wind_farm", "SourceLoad", 
		{
		{"Direction", "Source"},
		{"LPModelONLY", false},
		{"MaxFlow", "1e+06"},
		{"Weight", "-1"},
		{"UseProfileLoadFlux","WindFarmProduction"},
		{"EnvironmentModel", true}, //It is necessary to set to true in order to use EnvImpacts
		{"Climate change#Global Warming Potential 100 EnvGreyContentCoefficient_A", 250},
		{"Climate change#Global Warming Potential 100 EnvGreyContentOffset_B", 0},
		{"Climate change#Global Warming Potential 100 EnvGreyReplacement", 0},
		{"Climate change#Global Warming Potential 100 EnvGreyReplacementConstant", 0},
		{"Acidification#Accumulated Exceedance EnvGreyContentCoefficient_A", 50},
		{"Acidification#Accumulated Exceedance EnvGreyContentOffset_B", 0},
		{"Acidification#Accumulated Exceedance EnvGreyReplacement", 0},
		{"Acidification#Accumulated Exceedance EnvGreyReplacementConstant", 0},
		{"PortL0.Climate change#Global Warming Potential 100 EnvContentCoefficient_A", 0},
		{"PortL0.Climate change#Global Warming Potential 100 EnvContentOffset_B", 0},
		{"PortL0.Acidification#Accumulated Exceedance EnvContentCoefficient_A", 0},
		{"PortL0.Acidification#Accumulated Exceedance EnvContentOffset_B", 0}
		}
	);
	CairnAPI::MilpPortAPI vWind_farm_L0("PortL0", vElec);
	vWind_farm_L0.set_SettingValues({
				{"Direction", "OUTPUT"},
				{"Variable", "SourceLoadFlow"}
		});


	TESTAPI("add port L0 to Wind_farm",
		vWind_farm.add(vWind_farm_L0)
	)

	TESTAPI("add Wind_farm",
		m_Problem.add(vWind_farm)
	)

	// ajout d'un composant
	// -------------------------------------------------------------------
	CairnAPI::MilpComponentAPI vH2_Tank("H2_Tank", "StorageGen");	
	vH2_Tank.set_SettingValues({
		  {"Capex", "50"},
		  {"Opex", "0"},
		  {"Eta", "0.999"},
		  {"FlowDirection", "1"},
		  {"EcoInvestModel", "1"},
		  {"MaxPressure", "350"},
		  {"MaxEsto", "-20000000000"},
		  {"MinEsto", "0"},
		  {"MaxFlowCharge", "1100"},
		  {"MinFlowCharge","0"},
		  {"MaxFlowDischarge", "1200"},
		  {"MinFlowDischarge", "0"},
		  {"InitSOC", "0.0" },
		  {"FinalSOC", "0"}
		}
	);

	CairnAPI::MilpPortAPI vH2_Tank_L0("PortL0", vH2);
	vH2_Tank_L0.set_SettingValues({
				{"Direction", "OUTPUT"},
				{"Variable", "Flow"}
	});


	TESTAPI("add port L0 to H2_Tank",
		vH2_Tank.add(vH2_Tank_L0)
	)

	TESTAPI("add H2_Tank",
		m_Problem.add(vH2_Tank)
	)

	// création des bus
	CairnAPI::BusAPI vElec_Bus("Elec_Bus", "NodeLaw", vElec);
	CairnAPI::BusAPI vH2_Bus("H2_Bus", "NodeLaw", vH2);

	TESTAPI("add Elec_Bus",
		m_Problem.add(vElec_Bus)
	)

	TESTAPI("add H2_Bus",
		m_Problem.add(vH2_Bus)
	)

	// création link
	vElec_Bus = m_Problem.get_Bus("Elec_Bus");
	vH2_Bus = m_Problem.get_Bus("H2_Bus");

	m_Problem.add(vELY_PEM_L0, vElec_Bus);	
	m_Problem.add(vElecGrid_R0, vElec_Bus);
	m_Problem.add(vWind_farm_L0, vElec_Bus);
	m_Problem.add(vElecGridInject_R0, vElec_Bus);

	m_Problem.add(vELY_PEM_R0, vH2_Bus);
	m_Problem.add(vH2_Tank_L0, vH2_Bus);
	m_Problem.add(vH2_Load_L0, vH2_Bus);
	
	t_list vComps = m_Problem.get_Components();
	t_list vBuses = m_Problem.get_Buses();
	t_dict vLinks = m_Problem.get_Links();

	TESTAPI("Check if ELY_PEM is optimized",
		TestUtils::compare_scalar(vELY_PEM.isOptimized(), 1, eInt)
	)

	TESTAPI("Check if H2_Tank is optimized",
		TestUtils::compare_scalar(vH2_Tank.isOptimized(), 1, eInt)
	)

	TESTAPI("Check components to be optimized",
		TestUtils::compare_lists(m_Problem.get_optimized_components(), { "ELY_PEM", "Wind_farm", "H2_Tank" })
	)

	vEVs = m_Problem.get_EnergyCarriers();

	TESTAPI("Write Model File in the file path : " + vFileName,
		m_Problem.save_Study(vFileName_saved)
	)

	TESTAPI("Read the Timeseries from the file path : " + TimeseriesFileName,
		m_Problem.add_TimeSeries(TimeseriesFileName)
	)

	CairnAPI::SolutionAPI vSolution;
	TESTAPI("Run",
		vSolution = m_Problem.run(ScenarioName)
	)

	TESTAPI2("Compare results", TestUtils::ComparaisonCsvFile(ResultFileName, ReferenceResultFileName))

	// run again the same problem!
	TESTAPI("Run",
		vSolution = m_Problem.run(ScenarioName)
	)

	//Re-load the created study (after saving it) and then test again
	CairnAPI m_Persee2;

	CairnAPI::OptimProblemAPI m_Problem2;
	TESTAPI("Load",
		m_Problem2 = m_Persee2.read_Study(vFileName_saved)
	)

	TESTAPI2("check list components", TestUtils::compare_lists(m_Problem2.get_Components(), vComps))
	TESTAPI2("check list bus", TestUtils::compare_lists(m_Problem2.get_Buses(), vBuses))

	TESTAPI("Read the Timeseries from the file path : " + TimeseriesFileName,
		m_Problem2.add_TimeSeries(TimeseriesFileName)
	)

	TESTAPI("Run", vSolution = m_Problem2.run())
	vSolution.exportTimeSeries();

	TESTAPI2("Compare results 2",
		TestUtils::ComparaisonCsvFile(ResultFileName_saved, ReferenceResultFileName)
	)

	return noError;
}