
import pytest
try:
    from cairn import *
except:
    from cairnopen import *
from os import path

scripts_home = os.path.join(path.dirname(path.realpath(__file__)), "../scripts")
sys.path.append(scripts_home)
import CairnNRT as cNRT


@pytest.fixture()
def problem():
    app_home = path.dirname(path.realpath(__file__))
    simu_full =  path.join(app_home, './data/cairn_training.json')    
    cairn_instance = CairnAPI(False)
    cairn_instance.read_study(simu_full)
    problem = cairn_instance.get_study()
    yield problem

def compareList(a,b):
    if len(a)!=len(b):
        return False

    c = [i for i, j in zip(a, b) if i != j]
    return len(c)==0


@pytest.mark.Cairn
@pytest.mark.PythonAPI
def test_get_Objects(problem):
    
    # get all objects
    objectNames = problem.objects

    # class object in categories
    objects={'BusCompo':[], 'EnergyVector':[], 'MilpComponent':[], 'Solver':[], 'SimulationControl':[], 'TecEcoCompo':[]}
    
    for objectName in objectNames:
        objectCairn=problem.get_object(objectName)
        for k,v in objects.items():
            if objectCairn.objectType == k:
                v.append(objectCairn.name)
        
    # check buses
    assert compareList(objects['BusCompo'], problem.buses)

     # check EnergyVector
    assert compareList(objects['EnergyVector'], problem.energy_carriers)

    # check MilpComponent 
    assert compareList(objects['MilpComponent'], problem.get_components())
    assert compareList(objects['MilpComponent'], problem.components)

    # check Solver
    assert len(objects['Solver'])==1    
    assert objects['Solver'][0] == problem.get_solver().name

    # check SimulationControl
    assert len(objects['SimulationControl'])==1
    assert objects['SimulationControl'][0] == problem.get_simulation_control().name

    # test TeEco
    assert len(objects['TecEcoCompo'])==1
    assert problem.get_tech_eco_analysis().name==objects['TecEcoCompo'][0]
    tecEco = problem.get_object(objects['TecEcoCompo'][0])
    assert len(tecEco.settings)>2

    # tests settings object
    objectComp = problem.get_object("H2_Load")
	# Verify the value of MaxFlow."
    assert objectComp.get_setting_value("MaxFlow") == 1000.0
    
    values = problem.get_object(objects['SimulationControl'][0]).setting_values
    assert values["FutureSize"] == 48
    
    # object doesn't exist
    try:
        noObject = problem.get_object("Hello")
        assert False
    except:
        assert True

    

	


    