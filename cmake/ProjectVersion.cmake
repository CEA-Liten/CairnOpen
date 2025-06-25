# --- set current version ---
# -- MIPModeler Version => MIPMODELER_VERSION -----
if ("${PROJECT_NAME}" MATCHES "mipmodeler")
	include(${MIPMODELER_HOME}/cmake/ProjectVersion.cmake)	
endif()
# -- CairnCore Version => PROJECT_VERSION -----
if ("${PROJECT_NAME}" MATCHES "cairn")
	include(cmake/CairnVersion.cmake)
endif()
