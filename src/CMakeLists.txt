#================================================================
# cmake utilities to build Cairn 
#================================================================

# Common setup
message("\n--------------------------------------------------")
add_subdirectory(${CAIRN_HOME}/ModelInterface)
add_subdirectory(${CAIRN_HOME}/core)        
add_subdirectory(${CAIRN_HOME}/models)        
if (WITH_PRIVATEMODELS)
    message("\n---------- private ----------------------------")
    add_subdirectory(${CAIRN_HOME}/privateModels)  
endif()    

  
# extra files
if (WITH_GENERICAPPENV)
    if(CMAKE_SYSTEM_NAME MATCHES Windows)
        msys_to_win_path( ${Qt5_HOME} Qt5_HOME)
        configure_file(${CMAKE_SOURCE_DIR}/cmake/GenericAppEnv.in ${CMAKE_BINARY_DIR}/GenericAppEnv.bat @ONLY)
        install(
          FILES ${CMAKE_BINARY_DIR}/GenericAppEnv.bat
          DESTINATION ${CMAKE_HOME_DIRECTORY}
        )
    else()
	    configure_file(${CMAKE_SOURCE_DIR}/cmake/GenericAppEnv.sh.in ${CMAKE_BINARY_DIR}/GenericAppEnv.sh @ONLY)
        install(
          FILES ${CMAKE_BINARY_DIR}/GenericAppEnv.sh
          DESTINATION ${CMAKE_HOME_DIRECTORY}
          )
    endif()
endif()


if (WITH_PYBIND)
    if (EXISTS ${CMAKE_SOURCE_DIR}/cmake/__init__.py.in)
        configure_file(${CMAKE_SOURCE_DIR}/cmake/__init__.py.in ${CMAKE_BINARY_DIR}/__init__.py @ONLY)

        set(PROJECT_OPTION1 "-DWITH_PRIVATEMODELS:BOOL=${WITH_PRIVATEMODELS}")
        set(PROJECT_OPTION2 "-DCPLEX_ROOT:STRING=${CPLEX_ROOT}")
	    configure_file(${CMAKE_SOURCE_DIR}/cmake/setup.py.in ${CMAKE_HOME_DIRECTORY}/setup.py @ONLY)
        configure_file(${CMAKE_SOURCE_DIR}/cmake/pyproject.toml.in ${CMAKE_HOME_DIRECTORY}/pyproject.toml @ONLY)
	    install(
              FILES ${CMAKE_BINARY_DIR}/__init__.py
              DESTINATION .
              )
    endif()
endif()

# ============= Package configuration =============
# i.e. what should be generated/configured at install.
include(PackageSetup)
