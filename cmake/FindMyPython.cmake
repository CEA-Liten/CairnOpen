# find Python 


# Here is the trick
## update the environment with VIRTUAL_ENV variable (mimic the activate script)
set (ENV{VIRTUAL_ENV} ${PYTHON_VENV})

message("Python: ${Python_EXECUTABLE}")
if("${Python_EXECUTABLE}" STREQUAL "")    
    ## change the context of the search
    set (Python_FIND_VIRTUALENV FIRST)
    # unset Python3_EXECUTABLE because it is also an input variable (see documentation, Artifacts Specification section)
    unset (Python_EXECUTABLE)
    # Launch a new search
    find_package (Python COMPONENTS Interpreter Development)    
else()
    #execute_process (COMMAND "${Python_EXECUTABLE}" -m venv ${PYTHON_VENV})
    set (Python_FOUND ON)

endif()

if (Python_FOUND)
    message("Python: ${Python_EXECUTABLE}")
    message("Python_INCLUDE_DIR: ${Python_INCLUDE_DIRS}")
    message("Python_LIBRARIES: ${Python_LIBRARIES}")  
    
    # install requirements   
    #execute_process(
    #    COMMAND "${Python_EXECUTABLE}" -m pip install -r "${PYTHON_REQS}" COMMAND_ERROR_IS_FATAL ANY
    #)


    if(NOT TARGET Python::Python)
        add_library(Python::Python IMPORTED INTERFACE)
        if (Python_LIBRARIES)
            set_property(TARGET Python::Python PROPERTY INTERFACE_LINK_LIBRARIES ${Python_LIBRARIES})    
        endif()
        if(Python_INCLUDE_DIRS)
          set_target_properties(Python::Python PROPERTIES
            INTERFACE_INCLUDE_DIRECTORIES "${Python_INCLUDE_DIRS}")
        endif()
    endif()
    target_link_libraries(${COMPONENT} PRIVATE Python::Python)
    set(PYTHON_EXECUTABLE  ${Python_EXECUTABLE})
endif()
