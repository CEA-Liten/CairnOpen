#================================================================
# cmake utilities to build Cairn 
#================================================================
include(GNUInstallDirs)

# Common setup
set(PYTHON_VENV_PROJECT ${PYTHON_HOME}/../envs/envCairn${PROJECT_VERSION})
if (INSTALL_WHEEL)
    if (USE_ENVCAIRN) 
        # force use on envCairn
        set(INSTALL_WHEEL_VENV ${PYTHON_VENV_PROJECT})    
    else()
        set(PYTHON_VENV_PROJECT ${INSTALL_WHEEL_VENV})    
    endif() 
endif()
if(CMAKE_HOST_SYSTEM_NAME MATCHES Windows)	
    set(PYTHON_CMD_PROJECT ${PYTHON_VENV_PROJECT}/scripts/python)
else()	
    set(PYTHON_CMD_PROJECT ${PYTHON_VENV_PROJECT}/bin/python)
endif()


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
    file(GENERATE OUTPUT ${CMAKE_BINARY_DIR}/modelTypes.txt CONTENT "${list_models}")
    install(FILES ${CMAKE_BINARY_DIR}/modelTypes.txt DESTINATION resources)
endif()

# ============= Doc configuration =============
if (BUILD_DOC)
    # find python 	
	set(Python_DIR ${Python_ROOT_DIR})
	find_package(Python REQUIRED COMPONENTS Interpreter Development)	
	
    # get whl file    
    file(GLOB FILES_LIST ${CMAKE_INSTALL_PREFIX}/../cairn-${PROJECT_VERSION}*.whl)

    # install cairn with the wheel file
    list(LENGTH FILES_LIST nbWhl)
    if (nbWhl GREATER 0)
	    # take the first elem
	    list(GET FILES_LIST 0 CAIRN_WHL)	    
    else()
        message(FATAL_ERROR "Cannot find cairn-${PROJECT_VERSION} wheel, build it before.")
    endif()

    # virtual environment 	
	python_venv(${PYTHON_VENVDOC} ${Python_EXECUTABLE} REQS ${CAIRNDOC_HOME}/requirements.txt PCK ${CAIRN_WHL} FORCE ON)		
	
    # Extracting RST parts from Cairn source code...   
    set(DOC_CEA FALSE)
    if (${WITH_PRIVATEMODELS})
        set(DOC_CEA TRUE)
    endif()

    execute_process(COMMAND ${Python_EXECUTABLE} userDocGenRST.py ${DOC_CEA}
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/scripts/doc)
          
    # Build htmlSphinx   
    configure_file(${CAIRNDOC_HOME}/confDoc.py.in ${CAIRNDOC_HOME}/conf.py @ONLY)

    execute_process(COMMAND ${PYTHON_VENVDOC}/scripts/sphinx-build -M html . ./_build
        WORKING_DIRECTORY ${CAIRNDOC_HOME})
endif()

# ============= Package configuration =============
# i.e. what should be generated/configured at install.
if (BUILD_CAIRN) 
    include(PackageSetup)
endif()
