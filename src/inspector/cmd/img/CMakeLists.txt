include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_SOURCE_DIR}/src)
include_directories(${CMAKE_SOURCE_DIR}/third_party)
	
project(img)

add_executable(${PROJECT_NAME}
               img.cpp)

target_link_libraries(${PROJECT_NAME}
                      inspector_core
                      tl_core
                      ${COLMAP_LIBRARIES} 
                      Qt5::Sql
                      ${OpenCV_LIBS})

if (UNIX)
  target_link_libraries(${PROJECT_NAME} 
                        -lpthread 
                        -lgomp
                        -ldl)
endif()

set_target_properties(${PROJECT_NAME} PROPERTIES
                      OUTPUT_NAME "${PROJECT_NAME}"
                      PROJECT_LABEL "${PROJECT_NAME}")

if(ENABLE_SOLUTION_FOLDERS)
  set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER "commands")
endif()

if(WIN32)
  if (MSVC AND NOT BUILD_SHARED_LIBS)
    set_target_properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS "/NODEFAULTLIB:atlthunk.lib /NODEFAULTLIB:atlsd.lib /DEBUG")
  endif()
endif()


