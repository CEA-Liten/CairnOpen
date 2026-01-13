# ================================================================
# All the default values for cairn cmake parameters
# ================================================================

# Cplex path
set(CPLEX_ROOT /home/share/570-Energie/570.15-TRILOGY/tools/CPLEX/CPLEX_Studio201/cplex CACHE INTERNAL "CPLEX installation path (if exists use CPLEX)")

# Python, to force Python (if not defined, use find_package Python3)
set(PYTHON_HOME /home/share/570-Energie/570.15-TRILOGY/tools/python-u24/python-3.10.9 CACHE INTERNAL "Python installation path")
set(PYTHON_VENV /home/share/570-Energie/570.15-TRILOGY/tools/python-u24/venvs/envPegase CACHE INTERNAL "Python virtual environment")
set(pybind11_DIR ${PYTHON_VENV}/lib/python3.10/site-packages/pybind11/share/cmake/pybind11)
set(pybind11_INCLUDE_DIR ${PYTHON_VENV}/lib/python3.10/site-packages/pybind11/include CACHE INTERNAL "pybind11 include")

set(Python_ROOT_DIR ${PYTHON_HOME} CACHE INTERNAL "Python installation path")

# ================================================================
# --------- User-defined options ---------
# Use cmake -DOPTION_NAME=some-value ... to modify default value.
# --- Build/compiling options ---
option(WITH_TESTING "Build tests. Default = OFF" ON)
option(WITH_PYBIND "Cairn python binding" ON)
option(WITH_GENERICAPPENV "Generate file GenericAppEnv" ON)
option(WITH_PRIVATEMODELS "Generate private models" ON)
option(BUILD_WHEEL "build python wheel of cairn" ON)

# TODO!!
set(WARNINGS_LEVEL 0 CACHE INTERNAL "Set compiler diagnostics level. 0: no warnings, 1: developer's minimal warnings, 2: strict level, warnings to errors and so on. Default =0")
option(BUILD_SHARED_LIBS "Building of shared libraries. Default = ON" ON)

# Compilation of MIPModeler 
option(BUILD_MIPMODELER "build MIPModeler if ON" ON)
set(MIPMODELER_HOME ${CMAKE_SOURCE_DIR}/lib/MIPModeler CACHE INTERNAL "MIPModeler installation path")
#set(COINOR_ROOT ${MIPMODELER_HOME}/external/CoinOR CACHE INTERNAL "Cbc, Clp installation path")
option(WITH_HIGHS_INSTALL "Highs install" ON) 
option(WITH_EIGEN_INSTALL "Eigen install" ON)
option(WITH_SPDLOG_INSTALL "SPD log install" ON)
option(USE_CPLEX "Enable CPLEX support" ON) 

# Compilation of Cairn
option(BUILD_CAIRN "build Cairn if ON" ON)
set(CAIRN_HOME ${CMAKE_SOURCE_DIR}/src CACHE INTERNAL "Cairn installation path")
set(CAIRNTESTS_HOME ${CMAKE_SOURCE_DIR}/tests CACHE INTERNAL "Cairn tests path")
set(CAIRNMODELINTERFACE_HOME ${CAIRN_HOME}/modelInterface CACHE INTERNAL "Cairn Model interface installation path")
set(CAIRN_DEFAULTSOLVER Highs CACHE INTERNAL "Cairn default solver")

option(INSTALL_WHEEL "build python wheel of cairn" ON)
set(INSTALL_WHEEL_VENV ${CMAKE_SOURCE_DIR}/virtualPy CACHE INTERNAL "Cairn wheel installation path")