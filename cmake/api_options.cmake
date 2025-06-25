# ================================================================
# All the default values for pegase cmake parameters
#
# --------- User-defined options ---------
# Use cmake -DOPTION_NAME=some-value ... to modify default value.


# --- Build/compiling options ---
option(WITH_TESTING "Build tests. Default = ON" OFF)
option(WITH_PYBIND "Cairn python binding" ON)
option(WITH_GENERICAPPENV "Generate file GenericAppEnv" OFF)

# TODO!!
set(WARNINGS_LEVEL 0 CACHE INTERNAL "Set compiler diagnostics level. 0: no warnings, 1: developer's minimal warnings, 2: strict level, warnings to errors and so on. Default =0")
option(BUILD_SHARED_LIBS "Building of shared libraries. Default = ON" ON)


# Qt, version, chemin,...
set(Qt5_HOME C:/Qt/Qt5.15.0/5.15.0/msvc2019_64 CACHE INTERNAL "Qt5 home path")
set(Qt5_DIR ${Qt5_HOME}/lib/cmake/Qt5 CACHE INTERNAL "Qt5 installation path")
set(Qt5_BIN ${Qt5_HOME}/bin CACHE INTERNAL "Qt5 bin path")

# Compilation de MIPModeler 
option(BUILD_MIPMODELER "build MIPModeler if ON" ON)
#set(GAMS_ROOT C:/GAMS/39 CACHE INTERNAL "GAMS installation path (if exists build GAMSModeler)")
set(MIPMODELER_HOME ${CMAKE_SOURCE_DIR}/lib/MIPModeler CACHE INTERNAL "MIPModeler installation path")
set(CPLEX_ROOT "C:/Program Files/IBM/ILOG/CPLEX_Studio201/cplex" CACHE INTERNAL "CPLEX installation path (if exists use CPLEX)")
option(WITH_HIGHS_INSTALL "Highs install" ON) 
option(WITH_EIGEN_INSTALL "Eigen install" ON) 

# Compilation de Cairn
option(BUILD_CAIRN "build Cairn if ON" ON)
set(CAIRN_HOME ${CMAKE_SOURCE_DIR}/src CACHE INTERNAL "Cairn installation path")
set(CAIRNTESTS_HOME ${CMAKE_SOURCE_DIR}/tests CACHE INTERNAL "Cairn tests path")
set(CAIRNMODELINTERFACE_HOME ${CAIRN_HOME}/ModelInterface CACHE INTERNAL "Cairn Model interface installation path")
set(CAIRN_DEFAULTSOLVER Highs CACHE INTERNAL "Cairn default solver")

# Python, to force Python (if not defined, use find_package Python3)
set(PYTHON_HOME C:/PythonPegase/3_10_9/python CACHE INTERNAL "Python installation path")
set(PYTHON_VENV ${PYTHON_HOME}/../envPegase CACHE INTERNAL "Python virtual environment")
set(pybind11_DIR ${PYTHON_VENV}/Lib/site-packages/pybind11/share/cmake/pybind11)

set(Python_EXECUTABLE ${PYTHON_HOME}/python CACHE INTERNAL "Python exe")
set(Python_INCLUDE_DIRS ${PYTHON_HOME}/include CACHE INTERNAL "Python include")
set(Python_LIBRARIES ${PYTHON_HOME}/python310.dll CACHE INTERNAL "Python libs")

