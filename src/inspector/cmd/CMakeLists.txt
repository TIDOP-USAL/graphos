# Tools
# Macro para crear los ejecutables en modo consola. Es una adaptación de una macro de OpenCV

if(BUILD_INSPECTOR_COMMANDS)

  project(commands)

  if(CMAKE_COMPILER_IS_GNUCXX AND NOT ENABLE_NOISY_WARNINGS)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-unused-function -Wno-missing-declarations")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-unused-function -Wno-missing-declarations")
  endif()

  if (UNIX)
    include(CheckCXXCompilerFlag)
    CHECK_CXX_COMPILER_FLAG("-std=c++11" COMPILER_SUPPORTS_CXX11)
    if(COMPILER_SUPPORTS_CXX11)
      set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
    else()
       message(STATUS "The compiler ${CMAKE_CXX_COMPILER} has no C++11 support. Please use a different C++ compiler.")
    endif()

    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")

  endif()

add_subdirectory(createproject)

# # -----------------------------------------------------------------------------

  # MACRO(MAKE_CMD cmd_name srcs)

    # include_directories(${CMAKE_CURRENT_SOURCE_DIR})
    # include_directories(${CMAKE_SOURCE_DIR}/src)
    # include_directories(${CMAKE_SOURCE_DIR}/third_party)
	
    # add_executable(${cmd_name} 
                   # ${srcs} 
                   # #${CMAKE_SOURCE_DIR}/tools/utils/project.cpp
                   # #${CMAKE_SOURCE_DIR}/tools/utils/project.h
                   # )

    # target_link_libraries(${cmd_name} 
                          # inspector_core
                          # libprocess
                          # tl_core
                          # ${COLMAP_LIBRARIES} 
                          # Qt5::Sql
                          # ${OpenCV_LIBS})
    # # link boost libraries
    # target_link_libraries(${cmd_name} 
                          # ${Boost_FILESYSTEM_LIBRARY}
                          # ${Boost_SYSTEM_LIBRARY})
  
    # if (UNIX)
      # target_link_libraries(${cmd_name} 
                            # -lpthread 
                            # -lgomp
                            # -ldl
                            # )
    # endif()

    # # Añade el proyecto a la carpeta 'tools'
    # set_property(TARGET ${cmd_name} PROPERTY FOLDER "commands")

    # set_target_properties(${cmd_name} PROPERTIES
                          # OUTPUT_NAME "${cmd_name}"
                          # PROJECT_LABEL "${cmd_name}")

    # if(ENABLE_SOLUTION_FOLDERS)
      # set_target_properties(${cmd_name} PROPERTIES FOLDER "commands")
    # endif()

    # if(WIN32)

      # if (MSVC AND NOT BUILD_SHARED_LIBS)
        # set_target_properties(${cmd_name} PROPERTIES LINK_FLAGS "/NODEFAULTLIB:atlthunk.lib /NODEFAULTLIB:atlsd.lib /DEBUG")
      # endif()
    # endif()

    # #install(TARGETS ${cmd_name}
    # #        DESTINATION bin
    # #        COMPONENT tools)

  # ENDMACRO()

# # -----------------------------------------------------------------------------


  # file(GLOB_RECURSE commands RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *.cpp)
  

  # foreach(cmd_file ${commands})
    # get_filename_component(cmd ${cmd_file} NAME_WE)
    # MAKE_CMD(${cmd}  ${cmd_file})
  # endforeach()

endif()
