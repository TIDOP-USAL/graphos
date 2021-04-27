# Tools
# Macro para crear los ejecutables en modo consola. Es una adaptación de una macro de OpenCV

if(BUILD_INSPECTOR_COMMANDS)

  project(commands LANGUAGES CXX)

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
add_subdirectory(img)
add_subdirectory(featextract)
add_subdirectory(featmatch)
add_subdirectory(georef)
add_subdirectory(import_poses)
add_subdirectory(orientation)
endif()
