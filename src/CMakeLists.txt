include_directories(${CMAKE_CURRENT_SOURCE_DIR})

add_subdirectory(inspector/core)
add_subdirectory(inspector/process)

if (BUILD_INSPECTOR_IMAGE)
add_subdirectory(inspector/widgets)
add_subdirectory(inspector/ui)
endif (BUILD_INSPECTOR_IMAGE)

if(BUILD_INSPECTOR_COMMANDS)
add_subdirectory(inspector/cmd)
endif (BUILD_INSPECTOR_COMMANDS)

if(BUILD_INSPECTOR_MICROSERVICES)
add_subdirectory(inspector/microservices)
endif (BUILD_INSPECTOR_MICROSERVICES)
