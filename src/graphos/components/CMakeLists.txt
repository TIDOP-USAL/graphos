include_directories(${CMAKE_SOURCE_DIR})

file( GLOB components *)

foreach(component_dir ${components})
  if( IS_DIRECTORY ${component_dir} )
    if (NOT ${component_dir} STREQUAL ${CMAKE_CURRENT_SOURCE_DIR}/CMakeFiles)
      add_subdirectory(${component_dir})
    endif()
  endif()
endforeach()
