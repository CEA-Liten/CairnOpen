# --- set current version ---
# -- MIPModeler Version => MIPMODELER_VERSION -----
if ("${PROJECT_NAME}" MATCHES "mipmodeler")
	include(${MIPMODELER_HOME}/cmake/ProjectVersion.cmake)	
endif()
# -- CairnCore Version => PROJECT_VERSION -----
if ("${PROJECT_NAME}" MATCHES "cairn")
	include(cmake/CairnVersion.cmake)
endif()
### SOVERSION (linux)
set(SO_current 1)
set(SO_revision 0)
set(SO_age 0)

# Aggregate variables, to be passed to linker.
# libraries will be named e.g.,
set(SO_version_info "${SO_current}:${SO_revision}:${SO_age}")
math(EXPR SO_current_minus_age "(${SO_current}) - (${SO_age})")
set(PROJECT_SOVERSION "${SO_current_minus_age}.${SO_revision}.${SO_age}" CACHE STRING "SONAME")
set(PROJECT_SOVERSION_MAJOR "${SO_current_minus_age}" CACHE STRING "SONAME current-minus-age")


