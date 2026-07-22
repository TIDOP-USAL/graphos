include_directories(${CMAKE_CURRENT_SOURCE_DIR})

if(BUILD_DEM_COMPONENT)
    add_subdirectory(csf)
endif(BUILD_DEM_COMPONENT)