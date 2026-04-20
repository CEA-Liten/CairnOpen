# find Cairn
include(FindPackageHandleStandardArgs)

# default  package
if (NOT Cairn_FIND_COMPONENTS)
    set(Cairn_FIND_COMPONENTS CairnCore)    
endif()


if (NOT CAIRN_INSTALL)
    # build sources
    if (NOT CAIRN_HOME)
        set(CAIRN_HOME $ENV{CAIRN_HOME})
    endif()

    set (CAIRN_INCLUDE_DIR 
        ${CAIRN_HOME}/core        
        ${CAIRN_HOME}/modelInterface        
    )

    find_package_handle_standard_args(Cairn REQUIRED_VARS CAIRN_INCLUDE_DIR)

    if(Cairn_FOUND)
     if(NOT TARGET CAIRN::CAIRN)
        message("-- Cairn packages: ${Cairn_FIND_COMPONENTS}")
        add_library(CAIRN::CAIRN IMPORTED INTERFACE)
        set_property(TARGET CAIRN::CAIRN PROPERTY INTERFACE_LINK_LIBRARIES ${Cairn_FIND_COMPONENTS})
        if(CAIRN_INCLUDE_DIR)
          set_target_properties(CAIRN::CAIRN PROPERTIES
            INTERFACE_INCLUDE_DIRECTORIES "${CAIRN_INCLUDE_DIR}")
        endif()
      endif()            
    endif()
else()  
     find_package(cairn REQUIRED CONFIG)
endif()
