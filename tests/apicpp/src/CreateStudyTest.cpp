#include "CairnAPIUtils.h"
#include "TEST_CairnCore.h"
#include <iostream>
#include "UtilsJson.h"
#include "Utils.h"
#include <iomanip>

int main()
{
	string const StudyName = "createStudy";
	string const StudyRoot = TEST_RESULTS + (std::string)"/createStudy/";

	string const vFileName = StudyRoot + StudyName + ".json";
	string const TimeseriesFileName = StudyRoot + (std::string)"/formation_cairn_dataseries.csv";
	string const ScenarioName = "scenario1";
	string const ResultFileName = StudyRoot + ScenarioName + "/" + StudyName + "_results_Results.csv";

	string const vFileName_saved = StudyRoot + StudyName + "_saved.json";
	string const ResultFileName_saved = StudyRoot + StudyName + "_saved_results_Results.csv";

	if (fs::exists(StudyRoot)) {
		fs::remove_all(StudyRoot);
	}
	if (!fs::exists(TEST_RESULTS)) {
		fs::create_directory(TEST_RESULTS);
	}
	fs::create_directory(StudyRoot);
	fs::copy_file(TEST_DATA + (std::string)"/formation_cairn_dataseries.csv", TimeseriesFileName);

	string const ReferenceResultFileName = TEST_DATA + (std::string)"/formation_cairn_Results_Reference.csv";

	int liRet = 0;

	CairnAPI m_Cairn;
	cout << "Creation of a new study" << endl;
	CairnAPI::OptimProblemAPI m_Problem = m_Cairn.create_Study(StudyRoot + StudyName);

	// Creation of TechEcoAnalysis	
	CairnAPI::TecEcoAnalysisAPI vTecEcoAnalysis = m_Problem.get_TecEcoAnalysis();
	TESTAPI("set_TecEcoAnalysisSettings",
		vTecEcoAnalysis.set_SettingValues(
			{
				{"DiscountRate", 0.07},
				{"NbYear", 20},
				{"Range","HISTandPLAN"},
				{"ForceExportAllIndicators",  true},
				{"ConsideredEnvironmentalImpacts", "Climate change#Global Warming Potential 100,Acidification#Accumulated Exceedance"}
			}
		)
	)

	// Creation of SimulationControl
	CairnAPI::SimulationControlAPI vSimulationControl = m_Problem.get_SimulationControl();
	TESTAPI("create_simulationcontrol",
		vSimulationControl.set_SettingValues(
			{
				{"ExportJson", 1},
				{"ExportResults", 1},
				{"FutureSize", 48},
				{"FutureVariableTimestep", 8760}
			}
		)
	)

	// Creation of Solver
	CairnAPI::SolverAPI vSolver = m_Problem.get_Solver();
	TESTAPI("create_MIPSolver",
		vSolver.set_SettingValues(
			{
				{"Solver", "Cplex"},
				{"WriteLp", "YES"},
				{"Gap", 0.001},
				{"TimeLimit", 0}
			})
	)

	//Create EnergyVectors
	CairnAPI::EnergyVectorAPI vElec(m_Problem, "ElectricityDistrib", "Electrical");
	TESTAPI("reset Potential ElectricityDistrib", vElec.set_SettingValue("Potential", 220))
	TESTAPI("set UseProfileBuyPrice of ElectricityDistrib",
			vElec.set_SettingValue("UseProfileBuyPrice", "Elec_Grid.ElectricityPrice"))

	CairnAPI::EnergyVectorAPI vH2;
	TESTAPI("create EnergyVector H2", vH2 = m_Problem.create_EnergyCarrier("H2", "FluidH2"))
	TESTAPI("set parameters of EnergyVector H2",
		vH2.set_SettingValues(
			{
				{"Potential", 30},
				{"LHV", 0.03332},
				{"RHO", 0.0899},
				{"IsMassCarrier", true},
				{"IsFuelCarrier", true}
			})
	)

	t_list vEVs = m_Problem.get_EnergyCarriers();
	t_list vRef = { "H2","ElectricityDistrib" };
	TESTAPI2("check list Energy Vector", TestUtils::compare_lists(vRef, vEVs))

	// Adding a component
	// -------------------------------------------------------------------
	TESTAPI2("check list components", TestUtils::compare_lists(m_Problem.get_Components(), { }))

	CairnAPI::MilpComponentAPI vELY_PEM(m_Problem, "ELY_PEM", "Electrolyzer");

	t_list vELY_PEM_Ports = vELY_PEM.get_Ports();
	t_list vELY_PEM_DefaultPorts = vELY_PEM.get_DefaultPorts();

	TESTAPI2("Default ports", TestUtils::compare_lists(vELY_PEM_Ports, vELY_PEM_DefaultPorts));

	CairnAPI::MilpPortAPI vELY_PEM_R0;
	TESTAPI("get default port PortR0 of ELY_PEM", vELY_PEM_R0 = vELY_PEM.get_Port("PortR0"))
	TESTAPI("set the EnergyCarrier of the port", vELY_PEM_R0.set_EnergyCarrier(vH2))
	vELY_PEM_R0.set_SettingValues({
		{"Direction", "OUTPUT"},
		{"Variable", "H2MassFlowRate"}
		});

	TESTAPI2("Check if it is possible to delete default port",
		TestUtils::compare_scalar(vELY_PEM.remove_Port(vELY_PEM_R0), false, eBool)
	)

	CairnAPI::MilpPortAPI vELY_PEM_L0 = vELY_PEM.get_Port("PortL0");
	vELY_PEM_L0.set_EnergyCarrier(vH2); // To be changed below
	vELY_PEM_L0.set_SettingValues({
		{"Direction", "INPUT"},
		{"Variable", "UsedPower"}
		});

	vELY_PEM.set_SettingValues({
		{ "Capex", 480000 },
		{ "FixedOpex", "0.04" },
		{ "Efficiency", "0.6667" },
		{ "MaxPower", "-30" },
		{ "MinPower", "0" },
		{"EnvironmentModel", true},  
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
		{"PortL0.Acidification#Accumulated Exceedance EnvContentOffset_B", 0},
		{"PortR0.Climate change#Global Warming Potential 100 EnvContentCoefficient_A", 0},
		{"PortR0.Climate change#Global Warming Potential 100 EnvContentOffset_B", 0},
		{"PortR0.Acidification#Accumulated Exceedance EnvContentCoefficient_A", 0},
		{"PortR0.Acidification#Accumulated Exceedance EnvContentOffset_B", 0}
		}
	);

	TESTAPI("Change EnergyVector of vELY_PEM", vELY_PEM_L0.set_EnergyCarrier(vElec))

	//Verify Get and Set methods
	TESTAPI2("Verify the value of ELY_PEM.Capex.",
		TestUtils::compare_scalar(vELY_PEM.get_SettingValue("Capex"), 480000.0, eDouble)
	)

	TESTAPI("Set the value of ELY_PEM.Capex.",
		vELY_PEM.set_SettingValue("Capex", 100000.0);
	)

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
	TESTAPI2("check list components", TestUtils::compare_lists(m_Problem.get_Components(), { "ELY_PEM" }))

	// Adding a component
	// -------------------------------------------------------------------
	CairnAPI::MilpComponentAPI vElecGrid(m_Problem, "Elec_Grid", "GridFree");

	CairnAPI::MilpPortAPI vElecGrid_R0 = vElecGrid.get_Port("PortR0");
	vElecGrid_R0.set_EnergyCarrier(vElec);
	vElecGrid_R0.set_SettingValues({
		{"Direction", "OUTPUT"},
		{"Variable", "GridFlow"}
		});

	vElecGrid.set_SettingValues({
		{"MaxFlow","400"},
		{"Direction", "ExtractFromGrid"},
		{"EnvironmentModel", true},  
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

	// Adding a component
	// -------------------------------------------------------------------
	CairnAPI::MilpComponentAPI vElecGridInject = m_Problem.create_Component("Elec_Grid_Inject", "GridFree");

	CairnAPI::MilpPortAPI vElecGridInject_R0 = vElecGridInject.get_Port("PortR0");
	vElecGridInject_R0.set_EnergyCarrier(vElec);
	vElecGridInject_R0.set_SettingValues({
		{"Direction", "INPUT"},
		{"Variable", "GridFlow"}
	});

	vElecGridInject.set_SettingValues({
		{"Direction", "InjectToGrid"} ,
		  {"MaxFlow", "1500000"}
		}
	);

	// Adding a component
	// -------------------------------------------------------------------
	CairnAPI::MilpComponentAPI vH2_Load = m_Problem.create_Component("H2_Load", "SourceLoad");

	CairnAPI::MilpPortAPI vH2_Load_L0 = vH2_Load.get_Port("PortL0");
	vH2_Load_L0.set_EnergyCarrier(vH2);
	vH2_Load_L0.set_SettingValues({
		{"Direction", "INPUT"},
		{"Variable", "SourceLoadFlow"}
	});

	vH2_Load.set_SettingValues({
		{"Direction", "Sink" },
		{"LPModelONLY", false},
		{"Weight", "1"},
		{"FixedOpex", "0" },
		{"Capex", "0" },
		{"MaxFlow", "1000"},
		{"EcoInvestModel", "1"},
		{"UseProfileLoadFlux","H2_Load.LoadMassFlowrate"}
		}
	);

	// Adding a component
	// -------------------------------------------------------------------
	CairnAPI::MilpComponentAPI vWind_farm(m_Problem, "Wind_farm", "SourceLoad");


	CairnAPI::MilpPortAPI vWind_farm_L0 = vWind_farm.get_Port("PortL0");
	vWind_farm_L0.set_EnergyCarrier(vElec);
	vWind_farm_L0.set_SettingValues({
		{"Direction", "OUTPUT"},
		{"Variable", "SourceLoadFlow"}
	});


	vWind_farm.set_SettingValues({
		{"Direction", "Source"},
		{"LPModelONLY", false},
		{"MaxFlow", "1e+06"},
		{"Weight", "-1"},
		{"UseProfileLoadFlux","WindFarmProduction"},
		{"EnvironmentModel", true},
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
	// Adding a component
	// -------------------------------------------------------------------
	CairnAPI::MilpComponentAPI vH2_Tank(m_Problem, "H2_Tank", "StorageGen");

	CairnAPI::MilpPortAPI vH2_Tank_L0 = vH2_Tank.get_Port("PortL0");
	vH2_Tank_L0.set_EnergyCarrier(vH2);
	vH2_Tank_L0.set_SettingValues({
		{"Direction", "OUTPUT"},
		{"Variable", "Flow"}
	});

	vH2_Tank.set_SettingValues({
		  {"Capex", "50"},
		  {"FixedOpex", "0"},
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

	// Bus creation
	CairnAPI::BusAPI vElec_Bus(m_Problem, "Elec_Bus", "NodeLaw", vElec);

	CairnAPI::BusAPI vH2_Bus;
	TESTAPI("add H2_Bus", m_Problem.create_Bus("H2_Bus", "NodeLaw", vH2))

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

	m_Cairn.close_Study();
	//Re-load the created study (after saving it) and then test again
	CairnAPI m_Cairn2;

	CairnAPI::OptimProblemAPI m_Problem2;
	TESTAPI("Load",
		m_Problem2 = m_Cairn2.read_Study(vFileName_saved)
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