#================================================================
# cmake utilities to build Cairn 
#================================================================

# Common setup
message("\n--------------------------------------------------")
add_subdirectory(${CAIRN_HOME}/modelInterface)
add_subdirectory(${CAIRN_HOME}/core)        
add_subdirectory(${CAIRN_HOME}/models)        
if (WITH_PRIVATEMODELS)
    message("\n---------- private ----------------------------")
    add_subdirectory(${CAIRN_HOME}/privateModels)  
endif()    

  
# extra files
if (WITH_GENERICAPPENV)
    set(PYTHON_VENV_PROJECT ${PYTHON_HOME}/../envCairn${PROJECT_VERSION})
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

# Generate resources/modelTypes.txt (needed for api)
execute_process(COMMAND python generateModelTypes.py ${WITH_PRIVATEMODELS}
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/scripts)



if (WITH_PYBIND)
    if (EXISTS ${CMAKE_SOURCE_DIR}/cmake/__init__.py.in)
        configure_file(${CMAKE_SOURCE_DIR}/cmake/__init__.py.in ${CMAKE_BINARY_DIR}/__init__.py @ONLY)

        set(PROJECT_OPTION1 "-DWITH_PRIVATEMODELS:BOOL=${WITH_PRIVATEMODELS}")
        set(PROJECT_OPTION2 "-DCPLEX_ROOT:STRING=${CPLEX_ROOT}")
        set(PROJECT_OPTION3 "-DPYTHON_HOME:STRING=${PYTHON_HOME}")
        set(PROJECT_OPTION4 "-DPYTHON_VENV:STRING=${PYTHON_VENV}")       
        set(PROJECT_OPTION5 "-Dpybind11_DIR:STRING=${pybind11_DIR}")       
	    configure_file(${CMAKE_SOURCE_DIR}/cmake/setup.py.in ${CMAKE_HOME_DIRECTORY}/setup.py @ONLY)
	    configure_file(${CMAKE_SOURCE_DIR}/cmake/pyproject.toml.in ${CMAKE_HOME_DIRECTORY}/pyproject.toml @ONLY)      
	    install(
              FILES ${CMAKE_BINARY_DIR}/__init__.py
              DESTINATION .
              )
        if (BUILD_WHEEL)                
            message("-- Build Wheel in ${CMAKE_HOME_DIRECTORY}/${CMAKE_INSTALL_BINDIR}")           
            install(CODE "execute_process(COMMAND ${Python_EXECUTABLE} -m pip wheel ${CMAKE_HOME_DIRECTORY} -w ${CMAKE_HOME_DIRECTORY}/${CMAKE_INSTALL_BINDIR})")
        endif()
        
        if (INSTALL_WHEEL)
            configure_file(${CMAKE_SOURCE_DIR}/cmake/installWheel_options.in  ${CMAKE_CURRENT_BINARY_DIR}/cmake/installWheel_options.cmake @ONLY)            
            install(FILES ${CMAKE_CURRENT_BINARY_DIR}/cmake/installWheel_options.cmake DESTINATION lib/cmake)
            install(SCRIPT ${CMAKE_SOURCE_DIR}/cmake/installWheel.cmake)
        endif()
    endif()
endif()

# ============= Package configuration =============
# i.e. what should be generated/configured at install.
include(PackageSetup)
