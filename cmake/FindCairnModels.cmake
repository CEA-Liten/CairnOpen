# find CairnModels
include(FindPackageHandleStandardArgs)

# default CairnModels package
if (NOT CairnModels_FIND_COMPONENTS)
    set(CairnModels_FIND_COMPONENTS ElectrolyzerModel)    
endif()

if (NOT CairnModels_INSTALL)
    
    find_package_handle_standard_args(CairnModels REQUIRED_VARS CAIRNMODELS_INCLUDE_DIR)

    if(CairnModels_FOUND)       
        foreach(_MODEL_LIBRARY IN LISTS CairnModels_FIND_COMPONENTS)
            set(CAIRNMODEL_LIBRARY ${_MODEL_LIBRARY}${MODELS_SFX})
            if(NOT TARGET CairnModels::${CAIRNMODEL_LIBRARY})
                message("-- CairnModels package: ${CAIRNMODEL_LIBRARY}")
                add_library(CairnModels::${CAIRNMODEL_LIBRARY} IMPORTED INTERFACE)
                set_property(TARGET CairnModels::${CAIRNMODEL_LIBRARY} PROPERTY INTERFACE_LINK_LIBRARIES ${CAIRNMODEL_LIBRARY})
                if(CAIRNMODELS_INCLUDE_DIR)
                    set_target_properties(CairnModels::${CAIRNMODEL_LIBRARY} PROPERTIES            
                            INTERFACE_INCLUDE_DIRECTORIES "${CAIRNMODELS_INCLUDE_DIR}")
                endif()
            endif() 
        endforeach()                                
    endif()

else()
     # TODO
endif()
