# ================================================================
# All the default values for cairn cmake parameters
# ================================================================
# Dependencies flag
option(DEPS_INSTALL "Install dependencies" ON)

# Cplex path
set(CPLEX_ROOT /home/share/570-Energie/570.15-TRILOGY/tools/CPLEX/CPLEX_Studio201/cplex CACHE INTERNAL "CPLEX installation path (if exists use CPLEX)")

# Python, to force Python (if not defined, use find_package Python3)
set(PYTHON_HOME /home/prog/python-u24/Python-3.13.12/ CACHE INTERNAL "Python installation path")
set(PYTHON_VENV /home/share/570-Energie/570.15-TRILOGY/tools/python-u24/venvs/buildCairn CACHE INTERNAL "Python virtual environment")
set(PYTHON_PACKAGES lib/python3.13/site-packages CACHE INTERNAL "Python packages path")

set(pybind11_DIR ${PYTHON_VENV}/${PYTHON_PACKAGES}/pybind11/share/cmake/pybind11)
set(Python_ROOT_DIR ${PYTHON_HOME} CACHE INTERNAL "Python installation path")

# ================================================================
# --------- User-defined options ---------
# Use cmake -DOPTION_NAME=some-value ... to modify default value.
# --- Build/compiling options ---
option(WITH_TESTING "Build tests. Default = OFF" OFF)
option(WITH_GENERICAPPENV "Generate file GenericAppEnv" ON)
option(BUILD_WHEEL "build python wheel of cairn" ON)

# TODO!!
set(WARNINGS_LEVEL 0 CACHE INTERNAL "Set compiler diagnostics level. 0: no warnings, 1: developer's minimal warnings, 2: strict level, warnings to errors and so on. Default =0")
option(BUILD_SHARED_LIBS "Building of shared libraries. Default = ON" ON)

# Compilation of MIPModeler 
option(BUILD_MIPMODELER "build MIPModeler if ON" ON)
set(MIPMODELER_HOME ${CMAKE_SOURCE_DIR}/lib/MIPModeler CACHE INTERNAL "MIPModeler installation path")

set(highs_DIR ${DEPS_ROOT}/bin/${CMAKE_BUILD_TYPE}/lib/cmake/highs)
set(eigen_DIR ${DEPS_ROOT}/bin/${CMAKE_BUILD_TYPE}/share/eigen3/cmake)
set(spdlog_DIR ${DEPS_ROOT}/bin/${CMAKE_BUILD_TYPE}/lib/cmake/spdlog)


# Compilation of Cairn
option(BUILD_CAIRN "build Cairn if ON" ON)
set(CAIRN_HOME ${CMAKE_SOURCE_DIR}/src CACHE INTERNAL "Cairn installation path")
set(CAIRNTESTS_HOME ${CMAKE_SOURCE_DIR}/tests CACHE INTERNAL "Cairn tests path")
set(CAIRNMODELINTERFACE_HOME ${CAIRN_HOME}/modelInterface CACHE INTERNAL "Cairn Model interface installation path")
set(CAIRN_DEFAULTSOLVER Highs CACHE INTERNAL "Cairn default solver")

set(CAIRN_APP ${CMAKE_SOURCE_DIR} CACHE INTERNAL "Cairn installation path")

option(INSTALL_WHEEL "build python wheel of cairn" ON)
set(INSTALL_WHEEL_VENV ${CMAKE_SOURCE_DIR}/virtualPy CACHE INTERNAL "Cairn wheel installation path")