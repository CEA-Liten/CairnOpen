#================================================================
# cmake utilities to build Cairn 
#================================================================
include(GNUInstallDirs)

# Common setup
if (BUILD_CAIRN)  
    message("\n--------------------------------------------------")
    add_subdirectory(${CAIRN_HOME}/modelInterface)
    add_subdirectory(${CAIRN_HOME}/core)        
    add_subdirectory(${CAIRN_HOME}/models)        
    if (WITH_PRIVATEMODELS)
        message("\n---------- private ----------------------------")
        add_subdirectory(${CAIRN_HOME}/privateModels)  
    endif()    
else()
    find_package(Cairn)
endif()
  
# extra files
if (WITH_GENERICAPPENV)
    set(PYTHON_VENV_PROJECT ${PYTHON_HOME}/../envCairn${PROJECT_VERSION})
    if (INSTALL_WHEEL)
        set(PYTHON_VENV_PROJECT ${INSTALL_WHEEL_VENV})    
    endif()
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

if (BUILD_CAIRN)  
    # Generate resources/modelTypes.txt (needed for api)
    execute_process(COMMAND python generateModelTypes.py ${WITH_PRIVATEMODELS}
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/scripts)
endif()


if (WITH_PYBIND)
    if (EXISTS ${CMAKE_SOURCE_DIR}/cmake/__init__.py.in)
        set(CAIRN_INSTALL_LIBDIR ${CMAKE_INSTALL_LIBDIR})   
        if(CMAKE_SYSTEM_NAME MATCHES Windows)
            set(CAIRN_INSTALL_LIBDIR ${CMAKE_INSTALL_BINDIR})   
        endif()
        configure_file(${CMAKE_SOURCE_DIR}/cmake/__init__.py.in ${CMAKE_BINARY_DIR}/__init__.py @ONLY)

        string(JOIN , PROJECT_OPTIONS
             "'-DWITH_PRIVATEMODELS:BOOL=${WITH_PRIVATEMODELS}'"
             "'-DCPLEX_ROOT:STRING=${CPLEX_ROOT}'"
             "'-DPYTHON_HOME:STRING=${PYTHON_HOME}'"
             "'-DPYTHON_VENV:STRING=${PYTHON_VENV}'"
             "'-Dpybind11_DIR:STRING=${pybind11_DIR}'"
             "'-DPYTHON_PACKAGES:STRING=${PYTHON_PACKAGES}'")  
        
        if (DEFINED DEPS_ROOT)
            string(JOIN , PROJECT_OPTIONS ${PROJECT_OPTIONS}
                "'-DDEPS_INSTALL:BOOL=ON'"
                "'-DDEPS_ROOT:STRING=${DEPS_ROOT}'")  
        else()
            string(JOIN , PROJECT_OPTIONS ${PROJECT_OPTIONS}
                "'-DDEPS_INSTALL:BOOL=OFF'"  )              
        endif()

        message("PROJECT_OPTIONS: ${PROJECT_OPTIONS}")
        set(PYTHON_INSTALL_PACKAGE ${PYTHON_PACKAGES}/${PROJECT_NAME})
	    configure_file(${CMAKE_SOURCE_DIR}/cmake/setup.py.in ${CMAKE_HOME_DIRECTORY}/setup.py @ONLY)
	    configure_file(${CMAKE_SOURCE_DIR}/cmake/pyproject.toml.in ${CMAKE_HOME_DIRECTORY}/pyproject.toml @ONLY)      
	    install(
              FILES ${CMAKE_BINARY_DIR}/__init__.py
              DESTINATION .
              )
        string(TOLOWER ${CMAKE_BUILD_TYPE} CAIRN_BUILD_TYPE)
        if (BUILD_WHEEL AND (CAIRN_BUILD_TYPE STREQUAL release))         
            set(Python_DIR ${Python_ROOT_DIR})
	        find_package(Python REQUIRED COMPONENTS Interpreter Development)
            message(STATUS "Python: ${Python_EXECUTABLE}")

            file(REMOVE_RECURSE ${CMAKE_SOURCE_DIR}/_skbuild/)
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
if (BUILD_CAIRN) 
    include(PackageSetup)
endif()
