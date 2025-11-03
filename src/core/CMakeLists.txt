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
create_component(${COMPONENT})

target_compile_definitions(${COMPONENT} PRIVATE -DUNICODE)
target_compile_definitions(${COMPONENT} PRIVATE -DCAIRNCORE_LIBRARY)
target_compile_definitions(${COMPONENT} PRIVATE -DMILPCOMPONENT_LIBRARY)

if (WITH_PRIVATEMODELS)
	target_compile_definitions(${COMPONENT} PRIVATE -DPRIVATE_MODELS)
endif()

if(MSVC)
  target_compile_options(${COMPONENT} PRIVATE "/bigobj")
endif()

find_package(MIPModeler REQUIRED)
find_package(Cairn REQUIRED CairnModelInterface)

target_link_libraries(${COMPONENT} PRIVATE mipmodeler::MIPModeler)
target_link_libraries(${COMPONENT} PRIVATE mipmodeler::ModelerInterface)
target_link_libraries(${COMPONENT} PRIVATE mipmodeler::MIPSolver)

# -- Eigen to Cairn
include(EigenSetup)
target_link_libraries(${COMPONENT} PRIVATE Eigen3::Eigen)

include(SpdlogSetup)
target_link_libraries(${COMPONENT} PRIVATE spdlog::spdlog_header_only)

# ---- Installation ----
# Call component_install_setup(<COMPONENT>)
# to prepare installation of the current target.
#
# Before, it's necessary to set:
# 
# - <COMPONENT>_INSTALL_INTERFACE_INCLUDE_DIRECTORIES with all directories
#    that contain headers files that must be installed.
# 
set(${COMPONENT}_INSTALL_INTERFACE_INCLUDE_DIRECTORIES
  ${${COMPONENT}_DIRS} # All .h are installed
  )

# List header files (in dir above) that must not be installed 
#set(${COMPONENT}_HDRS_EXCLUDE)
component_install_setup(${COMPONENT})

if(WITH_TESTING)	
	include(CTestTools)	
	add_tests(${CAIRNTESTS_HOME}/apicpp/src 
	DATA ${CAIRNTESTS_HOME}/apicpp/data 
	RESULTS ${CAIRNTESTS_HOME}/apicpp/results 
	DEPS CairnCore
	EXTRA_DIR_SRC ${CAIRNTESTS_HOME}/apicpp/src/utils)	
endif()


if (WITH_PYBIND)
	
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
	# find python package
	find_package(Python REQUIRED COMPONENTS Interpreter Development)
	set(PYBIND11_FINDPYTHON ${Python_FOUND})
	find_package(pybind11 CONFIG REQUIRED)

	message(STATUS "Python: ${Python_EXECUTABLE}")
    message(STATUS "Python_INCLUDE_DIR: ${Python_INCLUDE_DIRS}")
    message(STATUS "Python_LIBRARIES: ${Python_LIBRARIES}") 
	set(Python_EXECUTABLE ${Python_EXECUTABLE} PARENT_SCOPE)
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
	
	target_link_libraries(${COMPONENT} PRIVATE CAIRN::CAIRN)

	# -- Eigen to Cairn
	target_compile_definitions(${COMPONENT} PRIVATE -DEIGEN_MPL2_ONLY)
	target_link_libraries(${COMPONENT} PRIVATE Eigen3::Eigen)
	target_link_libraries(${COMPONENT} PRIVATE spdlog::spdlog_header_only)

	component_install_setup(${COMPONENT} DESTINATION ..)
	
endif()