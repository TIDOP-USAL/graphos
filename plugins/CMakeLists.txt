project(plugins)

file( GLOB plugins *)

foreach(plugin_dir ${plugins})
  if( IS_DIRECTORY ${plugin_dir} )
    if (NOT ${plugin_dir} STREQUAL ${CMAKE_CURRENT_SOURCE_DIR}/CMakeFiles)
      add_subdirectory(${plugin_dir})
    endif()
  endif()
endforeach()
