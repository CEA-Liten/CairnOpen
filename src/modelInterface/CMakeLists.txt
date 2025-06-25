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
set(COMPONENT CairnModelInterface)
message("-- Set up for ${PROJECT_NAME}_${COMPONENT} library ...\n")

# ------ source directories for current component ------
# What is needed by component to compile ?
# List here all directories that contain sources files for current component.
# Path must be relative to component path (i.e. to CMAKE_CURRENT_SOURCE_DIR)
set(${COMPONENT}_DIRS
 .  
)

# ------ include interface ------
set(${COMPONENT}_INTERFACE_INCLUDE_DIRECTORIES 
	${${COMPONENT}_DIRS}	
)

# -- create/setup component target --
include(ComponentSetup)
create_component(${COMPONENT})
target_compile_definitions(${COMPONENT} PRIVATE -DMODELINTERFACE_LIBRARY)

find_package(Qt REQUIRED)
set_property(TARGET ${COMPONENT} PROPERTY AUTOMOC OFF)


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

