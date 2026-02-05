# ================================================================
# All the default values for cairn cmake parameters
# ================================================================

set(Python_ROOT_DIR ${PYTHON_HOME} CACHE INTERNAL "Python installation path")

# ================================================================
# --------- User-defined options ---------
# Use cmake -DOPTION_NAME=some-value ... to modify default value.
# --- Build/compiling options ---
option(WITH_TESTING "Build tests. Default = ON" OFF)
option(WITH_PYBIND "Cairn python binding" ON)
option(WITH_GENERICAPPENV "Generate file GenericAppEnv" OFF)

# TODO!!
set(WARNINGS_LEVEL 0 CACHE INTERNAL "Set compiler diagnostics level. 0: no warnings, 1: developer's minimal warnings, 2: strict level, warnings to errors and so on. Default =0")
option(BUILD_SHARED_LIBS "Building of shared libraries. Default = ON" ON)

# Compilation de MIPModeler 
option(BUILD_MIPMODELER "build MIPModeler if ON" ON)
set(MIPMODELER_HOME ${CMAKE_SOURCE_DIR}/lib/MIPModeler CACHE INTERNAL "MIPModeler installation path")
set(MIPModeler_DIR ${CMAKE_SOURCE_DIR}/bin/${CMAKE_BUILD_TYPE}/lib/cmake/mipmodeler)

set(highs_DIR ${DEPS_ROOT}/bin/${CMAKE_BUILD_TYPE}/lib/cmake/highs)
set(eigen_DIR ${DEPS_ROOT}/bin/${CMAKE_BUILD_TYPE}/share/eigen3/cmake)
set(spdlog_DIR ${DEPS_ROOT}/bin/${CMAKE_BUILD_TYPE}/lib/cmake/spdlog)

# Compilation de Cairn
option(BUILD_CAIRN "build Cairn if ON" ON)
#set(cairn_DIR  ${CMAKE_SOURCE_DIR}/bin/${CMAKE_BUILD_TYPE}/lib/cmake/cairn)
#set(CAIRN_INSTALL  ${CMAKE_SOURCE_DIR}/bin/${CMAKE_BUILD_TYPE})


set(CAIRN_HOME ${CMAKE_SOURCE_DIR}/src CACHE INTERNAL "Cairn installation path")
set(CAIRNTESTS_HOME ${CMAKE_SOURCE_DIR}/tests CACHE INTERNAL "Cairn tests path")
set(CAIRNMODELINTERFACE_HOME ${CAIRN_HOME}/modelInterface CACHE INTERNAL "Cairn Model interface installation path")
set(CAIRN_DEFAULTSOLVER Highs CACHE INTERNAL "Cairn default solver")
