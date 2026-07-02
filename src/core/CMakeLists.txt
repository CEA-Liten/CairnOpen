#================================================================
# cmake utilities to build Cairn component
#================================================================
#
# The objective is to call component_setup to create the target <COMPONENT>.
# Before, it's necessary to set:
# 
# - COMPONENT component name
# - <COMPONENT>_DIRS: the list of paths (relative to CMAKE_CURRENT_SOURCE_DIR) that
#   contain source files
# - <COMPONENT>_EXCLUDE_SRCS: the list of files, in <COMPONENT>_DIRS, that must be excluded
#   from build.
# - <COMPONENT>_INTERFACE_INCLUDE_DIRECTORIES: a list of directories
#   to populate the interface of the target for include directories at build time

# Component name (i.e. target name)
set(COMPONENT CairnCore)
message("\n-- Set up for ${PROJECT_NAME}_${COMPONENT} library ...")

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/CairnVersion.in ${CMAKE_CURRENT_SOURCE_DIR}/CairnVersion.h @ONLY)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/CairnDefine.in ${CMAKE_CURRENT_SOURCE_DIR}/CairnDefine.h @ONLY)

# ------ source directories for current component ------
# What is needed by component to compile ?
# List here all directories that contain sources files for current component.
# Path must be relative to component path (i.e. to CMAKE_CURRENT_SOURCE_DIR)
set(${COMPONENT}_DIRS
 . 
 api
 base
 base/timeSeries
 components
 parser
 utils
 submodels
 submodels/common
 unitsconverter
)

set(${COMPONENT}_EXCLUDE_SRCS
api/CairnBind.cpp
)

# ------ include interface ------
set(${COMPONENT}_INTERFACE_INCLUDE_DIRECTORIES
 ${${COMPONENT}_DIRS}	
)

# -- create/setup component target --
set(${COMPONENT}_RESOURCES_DIR
  ../../resources  
  ../../resources/images
  )

include(ComponentSetup)
include(EigenSetup)
include(SpdlogSetup)
find_package(MIPModeler REQUIRED ModelerInterface MIPSolver)
find_package(Cairn REQUIRED CairnModelInterface)

if (WITH_PYBIND)
	# --- build package for python
	set(COMPONENT cairn)
	message("-- Set up for ${PROJECT_NAME}_${COMPONENT} python binding ...")
	message(STATUS "Python: ${Python_EXECUTABLE}")
    message(STATUS "Python_INCLUDE_DIR: ${Python_INCLUDE_DIRS}")
    message(STATUS "Python_LIBRARIES: ${Python_LIBRARIES}") 

	set(BUILD_PYBIND ON)
	set(${COMPONENT}_DIRS
		${CairnCore_DIRS}
	)
	set(${COMPONENT}_INTERFACE_INCLUDE_DIRECTORIES
	 ${${COMPONENT}_DIRS}	
	)
	set(${COMPONENT}_RESOURCES_DIR
		${CairnCore_RESOURCES_DIR}
	)
	# find python 
	message("Python_ROOT_DIR: ${Python_ROOT_DIR}")
	set(Python_DIR ${Python_ROOT_DIR})
	find_package(Python REQUIRED COMPONENTS Interpreter Development)
	message(STATUS "Python: ${Python_EXECUTABLE}")
    message(STATUS "Python_INCLUDE_DIR: ${Python_INCLUDE_DIRS}")
    message(STATUS "Python_LIBRARIES: ${Python_LIBRARIES}") 		
	set(Python_EXECUTABLE ${Python_EXECUTABLE} PARENT_SCOPE)

	find_package(pybind11 CONFIG REQUIRED)	
	create_component(${COMPONENT})

	target_compile_definitions(${COMPONENT} PRIVATE -DUNICODE)
	target_compile_definitions(${COMPONENT} PRIVATE -DCAIRNCORE_LIBRARY)
	target_compile_definitions(${COMPONENT} PRIVATE -DMILPCOMPONENT_LIBRARY)

	if (WITH_PRIVATEMODELS)
		target_compile_definitions(${COMPONENT} PRIVATE -DPRIVATE_MODELS)
	endif()

	target_link_libraries(${COMPONENT} PRIVATE pybind11::headers)
	target_link_libraries(${COMPONENT} PRIVATE mipmodeler::MIPModeler)
	target_link_libraries(${COMPONENT} PRIVATE mipmodeler::ModelerInterface)
	target_link_libraries(${COMPONENT} PRIVATE mipmodeler::MIPSolver)
	
	target_link_libraries(${COMPONENT} PRIVATE cairn::CairnModelInterface)

	# -- Eigen to Cairn
	target_compile_definitions(${COMPONENT} PRIVATE -DEIGEN_MPL2_ONLY)
	target_link_libraries(${COMPONENT} PRIVATE Eigen3::Eigen)
	target_link_libraries(${COMPONENT} PRIVATE spdlog::spdlog)
	
	if(CMAKE_SYSTEM_NAME MATCHES Windows)
		set(CAIRN_INSTALL_BINDIR ${CMAKE_INSTALL_BINDIR})
	else()
		set(CAIRN_INSTALL_BINDIR ${CMAKE_INSTALL_LIBDIR})
	endif()
	configure_file(${CMAKE_SOURCE_DIR}/cmake/__init__.py.in ${CMAKE_BINARY_DIR}/__init__.py @ONLY)

	# --- install package for python	
	component_install_setup(${COMPONENT})
	
	install(
            FILES ${CMAKE_BINARY_DIR}/__init__.py
            DESTINATION .
            )

	install(
            DIRECTORY ${CAIRN_HOME}/fmiInterface
            DESTINATION .
            )
	install(
            DIRECTORY ${CAIRN_HOME}/pyInterface
            DESTINATION .
            )

	#install MIPModeler
	install(IMPORTED_RUNTIME_ARTIFACTS mipmodeler::MIPModeler)
	install(IMPORTED_RUNTIME_ARTIFACTS mipmodeler::ModelerInterface)
	install(IMPORTED_RUNTIME_ARTIFACTS mipmodeler::MIPSolver)
	install(IMPORTED_RUNTIME_ARTIFACTS mipmodeler::MIPHiGHSSolver)
	if (DEFINED CPLEX_ROOT)
		install(IMPORTED_RUNTIME_ARTIFACTS mipmodeler::MIPCpxSolver)
	endif()

	install(IMPORTED_RUNTIME_ARTIFACTS cairn::CairnCore)
	
	if (DEPS_INSTALL AND EXISTS ${DEPS_ROOT})
		install(DIRECTORY ${DEPS_ROOT}/bin/${CMAKE_BUILD_TYPE}/bin DESTINATION .)
		install(DIRECTORY ${DEPS_ROOT}/bin/${CMAKE_BUILD_TYPE}/lib DESTINATION .)		
	endif()
else()
	#build for C++
	create_component(${COMPONENT})

	target_compile_definitions(${COMPONENT} PRIVATE -DUNICODE)
	target_compile_definitions(${COMPONENT} PRIVATE -DCAIRNCORE_LIBRARY)
	target_compile_definitions(${COMPONENT} PRIVATE -DMILPCOMPONENT_LIBRARY)
	target_compile_definitions(${COMPONENT} PRIVATE -DEIGEN_MPL2_ONLY)

	if (WITH_PRIVATEMODELS)
		target_compile_definitions(${COMPONENT} PRIVATE -DPRIVATE_MODELS)
	endif()

	if(MSVC)
	  target_compile_options(${COMPONENT} PRIVATE "/bigobj")
	endif()

	target_link_libraries(${COMPONENT} PRIVATE CAIRN::CAIRN)
	target_link_libraries(${COMPONENT} PRIVATE mipmodeler::MIPModeler)
	target_link_libraries(${COMPONENT} PRIVATE mipmodeler::ModelerInterface)
	target_link_libraries(${COMPONENT} PRIVATE mipmodeler::MIPSolver)

	# -- Eigen to Cairn
	target_link_libraries(${COMPONENT} PRIVATE Eigen3::Eigen)
	target_link_libraries(${COMPONENT} PRIVATE spdlog::spdlog)

	# ---- Installation ----	
	set(${COMPONENT}_INSTALL_INTERFACE_INCLUDE_DIRECTORIES
	  ${${COMPONENT}_DIRS} # All .h are installed
	  )
	
	component_install_setup(${COMPONENT})
	if (DEPS_INSTALL AND EXISTS ${DEPS_ROOT})
		install(DIRECTORY ${DEPS_ROOT}/bin/${CMAKE_BUILD_TYPE}/bin DESTINATION .)
		install(DIRECTORY ${DEPS_ROOT}/bin/${CMAKE_BUILD_TYPE}/lib DESTINATION .)
		install(DIRECTORY ${DEPS_ROOT}/bin/${CMAKE_BUILD_TYPE}/include DESTINATION .)
		install(DIRECTORY ${DEPS_ROOT}/bin/${CMAKE_BUILD_TYPE}/share DESTINATION .)

		if(MSVC)
			# Bidouille pour recopier les fichiers deps dans le répertoire de travail des tests, permet de lancer les tests avec le TestExplorer de VS
			# La commande ci-dessous serait plus 'propre' mais ne fonctionne pas (à ajouter dans CTestTools après une commande add_test):
			#	set_tests_properties(
			#		${TESTUNIT_NAME}
			#		PROPERTIES
			#		ENVIRONMENT_MODIFICATION "PATH=path_list_prepend:D:/Users/JC799343/Cairn/cairnopen/deps/bin/Debug/bin"
			#	)
			install(DIRECTORY ${DEPS_ROOT}/bin/${CMAKE_BUILD_TYPE}/bin DESTINATION $<TARGET_FILE_DIR:${COMPONENT}>/..)

			# La commande ci-dessous ne fonctionne pas non plus...
			#add_custom_command(TARGET ${COMPONENT} POST_BUILD
			#	COMMAND ${CMAKE_COMMAND} -E copy ${DEPS_ROOT}/bin/${CMAKE_BUILD_TYPE}/bin ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
			#)
		endif()		

	endif()
	
	
endif()

if(WITH_TESTING)	
	include(CTestTools)
	set(CAIRN_SCRIPTS ${CMAKE_SOURCE_DIR}/tests/scripts)
	# Install Python environement for tests (useful to compare tests results)
	find_package(Python REQUIRED COMPONENTS Interpreter Development)		
	python_venv2(${PYTHON_VENV_PROJECT} ${Python_EXECUTABLE} ${CAIRNTESTS_HOME}/scripts/reqs_tests.txt)	

	# CTests : API Python
	if (INSTALL_WHEEL)
		#add_test (NAME cairn.python.python-tests
		#	COMMAND ${Python_EXECUTABLE} -m pytest ${CAIRNTESTS_HOME}/apipython
		#	WORKING_DIRECTORY ${CAIRN_SCRIPTS}
		#)
		#list(APPEND CMAKE_MODULE_PATH ${PYTHON_VENV_PROJECT}/share/Pytest/cmake) 	
		#set(Pytest_ROOT ${Python_BIN})
		#find_package(Pytest)	
		#if (${Pytest_FOUND})
		#	pytest_discover_tests( python			
		#		WORKING_DIRECTORY ${CAIRNTESTS_HOME}/apipython
		#		PYTHON_PATH_PREPEND ${CAIRN_SCRIPTS}		
		#		INCLUDE_FILE_PATH
		#	)
		#endif()
	endif()
	
	# CTests : API C
	if (NOT WITH_PRIVATEMODELS)
		set(${COMPONENT}_EXCLUDE_TESTS ../../tests/apicpp/src/ChangeModelClassTest.cpp)
	endif()
	if (NOT TEST_WITH_COVERAGE)
		list(APPEND ${COMPONENT}_EXCLUDE_TESTS ../../tests/apicpp/src/GenericTests.cpp)
	endif()
	if (TEST_WITH_CTESTS)
		add_tests(${CAIRNTESTS_HOME}/apicpp/src 
			DATA ${CAIRNTESTS_HOME}/apicpp/data 
			RESULTS ${CAIRNTESTS_HOME}/apicpp/results 
			DEPS CairnCore CairnModelInterface spdlog::spdlog
			EXTRA_DIR_SRC ${CAIRNTESTS_HOME}/apicpp/src/utils
			EXCLUDE_TESTS ${${COMPONENT}_EXCLUDE_TESTS}			
		)
	endif()

	# CTests: generic NRTs
	if (TEST_WITH_GENERICTESTS)
		create_genericTest(GenericTests 
			${CAIRNTESTS_HOME}/apicpp/src
			${CAIRNTESTS_HOME}/apicpp/src/GenericTests.cpp
			DEPS CairnCore CairnModelInterface spdlog::spdlog
			EXTRA_DIR_SRC ${CAIRNTESTS_HOME}/apicpp/src/utils
		)

		add_genericTests(models)
		add_genericTests(integration)

		if (WITH_PRIVATEMODELS)
			add_genericTests(privateTests/models)
			add_genericTests(privateTests/integration)
		endif()

	endif()

endif()
