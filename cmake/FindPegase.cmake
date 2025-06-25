# find PegaseCommon (useful to compile ModulePersee, specifi module for projects)
# inputs:
#    PEGASE_INSTALL (string) : path where Pegase is installed   
include(FindPackageHandleStandardArgs)

set (PEGASE_INCLUDE_DIR 
    ${PEGASE_INSTALL}/${CMAKE_BUILD_TYPE}/include/pegase
)

if(NOT PEGASE_LIBRARIES)        
    foreach(PEGASE_LIBRARY ${Pegase_FIND_COMPONENTS})                
        message("-- PEgase package for ${COMPONENT},  ${PEGASE_LIBRARY}")
        find_library(PEGASE_LIB ${PEGASE_LIBRARY}
            PATHS ${PEGASE_INSTALL}/lib ${PEGASE_INSTALL}/${CMAKE_BUILD_TYPE}/lib
            REQUIRED)            
        list(APPEND PEGASE_LIBRARIES ${PEGASE_LIB})                
        unset(PEGASE_LIB CACHE)
    endforeach()      
endif()

find_package_handle_standard_args(Pegase
  REQUIRED_VARS PEGASE_LIBRARIES PEGASE_INCLUDE_DIR)

if(PEGASE_FOUND)
  option(USE_PEGASE "Use PEGASE" ON)    
    foreach(PEGASE_LIBRARY PEGASE_FULLLIBRARY IN ZIP_LISTS Pegase_FIND_COMPONENTS PEGASE_LIBRARIES)        
        add_library(${PEGASE_LIBRARY} SHARED IMPORTED) 
        set_property(TARGET ${PEGASE_LIBRARY} PROPERTY INTERFACE_LINK_LIBRARIES ${PEGASE_FULLLIBRARY})                        
        set_property(TARGET ${PEGASE_LIBRARY} PROPERTY IMPORTED_IMPLIB ${PEGASE_FULLLIBRARY})
        set_target_properties(${PEGASE_LIBRARY} PROPERTIES            
            OUTPUT_NAME "${PEGASE_LIBRARY}"          
            INTERFACE_INCLUDE_DIRECTORIES "${PEGASE_INCLUDE_DIR}/${PEGASE_LIBRARY}"  
        )
       target_link_libraries(${COMPONENT} PUBLIC ${PEGASE_LIBRARY})
       
    endforeach()

  
endif()
