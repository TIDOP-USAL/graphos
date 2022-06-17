include_directories(${CMAKE_CURRENT_SOURCE_DIR})

add_subdirectory(graphos/core)

if (BUILD_GRAPHOS)
add_subdirectory(graphos/widgets)
add_subdirectory(graphos/components)
add_subdirectory(graphos/gui)
endif (BUILD_GRAPHOS)
