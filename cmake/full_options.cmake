# ================================================================
# All the default values for cairn cmake parameters
# ================================================================
# Dependencies flag
option(DEPS_INSTALL "Install dependencies" ON)

# Qt5 home path
set(Qt5_HOME C:/Qt/Qt5.15.0/5.15.0/msvc2019_64 CACHE INTERNAL "Qt5 home path")

# Cplex path
set(CPLEX_ROOT "C:/Program Files/IBM/ILOG/CPLEX_Studio201/cplex" CACHE INTERNAL "CPLEX installation path (if exists use CPLEX)")

# Python, to force Python (if not defined, use find_package Python3)
set(PYTHON_HOME C:/Python/Python313 CACHE INTERNAL "Python installation path")
set(PYTHON_VENV C:/Python/envs/buildCairn CACHE INTERNAL "Python virtual environment")
set(PYTHON_PACKAGES Lib/site-packages CACHE INTERNAL "Python packages path")
set(pybind11_DIR ${PYTHON_VENV}/${PYTHON_PACKAGES}/pybind11/share/cmake/pybind11)

set(Python_ROOT_DIR ${PYTHON_HOME} CACHE INTERNAL "Python installation path")

# doc
set(PYTHON_VENVDOC C:/Python/envs/docCairn CACHE INTERNAL "Python virtual environment")

# ================================================================
# --------- User-defined options ---------
# Use cmake -DOPTION_NAME=some-value ... to modify default value.
# --- Build/compiling options ---
option(WITH_TESTING "Build tests. Default = OFF" ON)
option(TEST_WITH_CTESTS "Build C unit tests. Default = OFF" ON)
option(TEST_WITH_GENERICTESTS "Build generic tests. Default = OFF" ON)
option(WITH_GENERICAPPENV "Generate file GenericAppEnv" ON)
option(WITH_PRIVATEMODELS "Generate private models" ON)
option(WITH_PROFILING "Enable runtime profiling instrumentation" ON)
set(INSTALL_WHEEL_VENV ${CMAKE_SOURCE_DIR}/virtualPy CACHE INTERNAL "Cairn wheel installation path")

# TODO!!
set(WARNINGS_LEVEL 0 CACHE INTERNAL "Set compiler diagnostics level. 0: no warnings, 1: developer's minimal warnings, 2: strict level, warnings to errors and so on. Default =0")
option(BUILD_SHARED_LIBS "Building of shared libraries. Default = ON" ON)

# Qt, version, chemin,...
set(Qt5_DIR ${Qt5_HOME}/lib/cmake/Qt5 CACHE INTERNAL "Qt5 installation path")
set(Qt5_BIN ${Qt5_HOME}/bin CACHE INTERNAL "Qt5 bin path")

# Compilation of MIPModeler 
option(BUILD_MIPMODELER "build MIPModeler if ON" ON)
set(MIPMODELER_HOME ${CMAKE_SOURCE_DIR}/lib/MIPModeler CACHE INTERNAL "MIPModeler installation path")
set(COINOR_ROOT ${MIPMODELER_HOME}/external/CoinOR CACHE INTERNAL "Cbc, Clp installation path")
set(highs_DIR ${DEPS_ROOT}/bin/${CMAKE_BUILD_TYPE}/lib/cmake/highs)
set(eigen_DIR ${DEPS_ROOT}/bin/${CMAKE_BUILD_TYPE}/share/eigen3/cmake)
set(spdlog_DIR ${DEPS_ROOT}/bin/${CMAKE_BUILD_TYPE}/lib/cmake/spdlog)

# Compilation of Cairn
option(BUILD_CAIRN "build Cairn if ON" ON)
set(CAIRN_HOME ${CMAKE_SOURCE_DIR}/src CACHE INTERNAL "Cairn installation path")
set(CAIRNTESTS_HOME ${CMAKE_SOURCE_DIR}/tests CACHE INTERNAL "Cairn tests path")
set(CAIRNMODELINTERFACE_HOME ${CAIRN_HOME}/modelInterface CACHE INTERNAL "Cairn Model interface installation path")
set(CAIRN_DEFAULTSOLVER Cplex CACHE STRING "Cairn default solver")
set(CAIRNDOC_HOME ${CMAKE_SOURCE_DIR}/doc/user CACHE INTERNAL "Cairn doc path")

# Compilation of module for Pegase (needs to install Pegase in lib/PegaseInstall)
option(BUILD_MODULECAIRN "build ModuleCairn if ON" ON)
set(MODULECAIRN_HOME ${CMAKE_SOURCE_DIR}/lib/ModuleCairn/src/Cairn CACHE INTERNAL "ModuleCairn installation path")
set(PEGASE_INSTALL ${CMAKE_SOURCE_DIR}/lib/PegaseInstall CACHE INTERNAL "Pegase installation path")

# Compilation of CairnCmd
option(BUILD_CAIRNCMD "build Cairn standalone (CairnCmd) if ON" ON)

# Compilation of CairnGui
option(BUILD_CAIRNGUI "build Cairn GUI if ON" ON)
option(WITH_LICENCE "build cairn gui with licence" ON)
#option(BUILD_MODELJSON "build Model.json if ON" ON)
set(CAIRNGUI_HOME ${CMAKE_SOURCE_DIR}/gui CACHE INTERNAL "Cairn gui path")

set(CAIRN_APP ${CMAKE_SOURCE_DIR} CACHE INTERNAL "Cairn GUI installation path")
set(OPENSSL_HOME ${CMAKE_SOURCE_DIR}/gui/lib/Openssl CACHE INTERNAL "Openssl installation path")
set(IHPHIPAPI_HOME ${CMAKE_SOURCE_DIR}/gui/lib/IHPHIpApi CACHE INTERNAL "IHPHIpApi installation path")
set(COMPONENTS_GUI Plotter CACHE INTERNAL "List of Cairn GUI components to build and install")
set(COMPONENTS_LICGUI Cipher License CACHE INTERNAL "List of Cairn GUI licence components to build and install")
