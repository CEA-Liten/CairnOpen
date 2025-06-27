# find MIPModeler, MIPSolver, ModelerInterface
include(FindPackageHandleStandardArgs)

 set(MIPMODELER_LIBS
      MIPModeler MIPSolver ModelerInterface
 )

 if (BUILD_MIPMODELER)
    # build sources
    if (NOT MIPMODELER_HOME)
        set(MIPMODELER_HOME $ENV{MIPMODELER_HOME})
    endif()

    set (MIPMODELER_INCLUDE_DIR 
        ${MIPMODELER_HOME}/core
        ${MIPMODELER_HOME}/MIPSolverInterface/MIPSolver
        ${MIPMODELER_HOME}/ModelerInterface
    )

    find_package_handle_standard_args(MIPModeler REQUIRED_VARS MIPMODELER_INCLUDE_DIR)

    if(MIPModeler_FOUND)
        foreach(MIPMODELER_LIBRARY IN LISTS MIPMODELER_LIBS)     
            add_library(mipmodeler::${MIPMODELER_LIBRARY} IMPORTED INTERFACE)
            set_property(TARGET mipmodeler::${MIPMODELER_LIBRARY} PROPERTY INTERFACE_LINK_LIBRARIES ${MIPMODELER_LIBRARY})
            if(MIPMODELER_INCLUDE_DIR)
                set_target_properties(mipmodeler::${MIPMODELER_LIBRARY} PROPERTIES INTERFACE_INCLUDE_DIRECTORIES "${MIPMODELER_INCLUDE_DIR}")
            endif()
        endforeach()
    endif()
else()
    find_package(MIPModeler REQUIRED CONFIG)

endif()


