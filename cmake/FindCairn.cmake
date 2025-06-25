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
        ${CAIRN_HOME}/ModelInterface        
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
      target_link_libraries(${COMPONENT} PRIVATE CAIRN::CAIRN)
    endif()
else()
     # get libs
      message("-- Cairn packages (get libs): ${Cairn_FIND_COMPONENTS}")
     foreach(_component IN LISTS Cairn_FIND_COMPONENTS)
        list(APPEND CAIRN_INCLUDE_DIR ${CAIRN_INSTALL}/include/cairn/${_component})
     endforeach()
   
     if(NOT CAIRN_LIBRARIES)                
        foreach(_component IN LISTS Cairn_FIND_COMPONENTS)                
            find_library(CAIRN_LIB ${_component}
                PATHS ${CAIRN_INSTALL}/lib ${CAIRN_INSTALL}/${CMAKE_BUILD_TYPE}/lib
                REQUIRED)            
            list(APPEND CAIRN_LIBRARIES ${CAIRN_LIB})       
            unset(CAIRN_LIB CACHE)
        endforeach()      
    endif()

    find_package_handle_standard_args(Cairn REQUIRED_VARS CAIRN_LIBRARIES CAIRN_INCLUDE_DIR)
    if(Cairn_FOUND)        
        foreach(CAIRN_LIBRARY CAIRN_FULLLIBRARY IN ZIP_LISTS Cairn_FIND_COMPONENTS CAIRN_LIBRARIES)        
            add_library(${CAIRN_LIBRARY} SHARED IMPORTED) 
            set_property(TARGET ${CAIRN_LIBRARY} PROPERTY INTERFACE_LINK_LIBRARIES ${CAIRN_FULLLIBRARY})                        
            set_property(TARGET ${CAIRN_LIBRARY} PROPERTY IMPORTED_IMPLIB ${CAIRN_FULLLIBRARY})
            set_target_properties(${CAIRN_LIBRARY} PROPERTIES            
                OUTPUT_NAME "${CAIRN_LIBRARY}"                
                INTERFACE_INCLUDE_DIRECTORIES "${CAIRN_INCLUDE_DIR}"  
            )
           target_link_libraries(${COMPONENT} PUBLIC ${CAIRN_LIBRARY})       
        endforeach()
    endif()
endif()
