include_directories(${CMAKE_CURRENT_SOURCE_DIR})

add_subdirectory(graphos/core)
add_subdirectory(graphos/process)

if (BUILD_GRAPHOS)
add_subdirectory(graphos/widgets)
add_subdirectory(graphos/components)
endif (BUILD_GRAPHOS)

if(BUILD_GRAPHOS_MICROSERVICES)
add_subdirectory(graphos/microservices)
endif (BUILD_GRAPHOS_MICROSERVICES)
