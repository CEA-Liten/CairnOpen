# find QtCSV
include(FindPackageHandleStandardArgs)

if (NOT QTCSV_HOME)
    set(QTCSV_HOME $ENV{QTCSV_HOME})
endif()

set (QTCSV_INCLUDE_DIR 
    ${QTCSV_HOME}/include    
)

foreach(dir IN LISTS QTCSV_INCLUDE_DIR)       
    target_include_directories(${COMPONENT} INTERFACE
          $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/${dir}>)        
endforeach()     

set(QTCSV_LIBS
     qtcsv
)
foreach(QTCSV_LIBRARY IN LISTS QTCSV_LIBS)                
    target_link_libraries(${COMPONENT} PRIVATE ${QTCSV_LIBRARY})
endforeach()     

