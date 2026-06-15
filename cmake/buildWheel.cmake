#================================================================
# cmake utilities to build and install wheel Cairn 
#================================================================
include(GNUInstallDirs)

if (EXISTS ${CMAKE_SOURCE_DIR}/cmake/__init__.py.in)        
  
    string(JOIN , PROJECT_OPTIONS                       
            "'-DPRESETNAME:STRING=${PRESETNAME}'"
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
    if (WITH_PRIVATEMODELS)
        set(WHEEL_NAME ${PROJECT_NAME})
    else()
        set(WHEEL_NAME ${PROJECT_NAME}open)
    endif()
    set(PYTHON_INSTALL_PACKAGE ${PYTHON_PACKAGES}/${WHEEL_NAME})    
	configure_file(${CMAKE_SOURCE_DIR}/cmake/setup.py.in ${CMAKE_HOME_DIRECTORY}/setup.py @ONLY)    
	configure_file(${CMAKE_SOURCE_DIR}/cmake/pyproject.toml.in ${CMAKE_HOME_DIRECTORY}/pyproject.toml @ONLY)      
	   
    set(Python_DIR ${PYTHON_HOME})
    set(Python_ROOT_DIR ${PYTHON_HOME})
    message(STATUS "Python: ${Python_EXECUTABLE}")
    message(STATUS "Python_ROOT_DIR: ${Python_ROOT_DIR}")

    unset(Python_EXECUTABLE)
   
	find_package(Python COMPONENTS Interpreter Development REQUIRED)
    message(STATUS "Python: ${Python_EXECUTABLE}")
    message(STATUS "Python libs: ${Python_LIBRARIES}")

    file(REMOVE_RECURSE ${CMAKE_SOURCE_DIR}/_skbuild/)
    message("-- Build Wheel in ${CMAKE_HOME_DIRECTORY}/${CMAKE_INSTALL_BINDIR}")           
    install(CODE "execute_process(COMMAND ${Python_EXECUTABLE} -m pip wheel ${CMAKE_HOME_DIRECTORY} -w ${CMAKE_HOME_DIRECTORY}/${CMAKE_INSTALL_BINDIR})")

    if (INSTALL_WHEEL)
        configure_file(${CMAKE_SOURCE_DIR}/cmake/installWheel_options.in  ${CMAKE_CURRENT_BINARY_DIR}/cmake/installWheel_options.cmake @ONLY)            
        install(FILES ${CMAKE_CURRENT_BINARY_DIR}/cmake/installWheel_options.cmake DESTINATION lib/cmake)
        install(SCRIPT ${CMAKE_SOURCE_DIR}/cmake/installWheel.cmake)
    endif()
endif()
