include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_SOURCE_DIR}/src)
include_directories(${CMAKE_SOURCE_DIR}/third_party)
	
project(featextract)

add_executable(${PROJECT_NAME}
               featextract.cpp
			   )

target_link_libraries(${PROJECT_NAME}
                      inspector_core
                      inspector_process
                      tl_core
                      ${COLMAP_LIBRARIES} 
                      Qt5::Sql
                      ${OpenCV_LIBS})
					  
# link boost libraries
#target_link_libraries(${PROJECT_NAME}
#                      ${Boost_FILESYSTEM_LIBRARY}
#                      ${Boost_SYSTEM_LIBRARY})

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


