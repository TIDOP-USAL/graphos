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
                      Qt5::Core
                      Qt5::Widgets
                      Qt5::Sql
                      ${COLMAP_LIBRARIES}
                      ${FREEIMAGE_LIBRARIES}
                      ${GLEW_LIBRARIES}
                      ${CERES_LIBRARIES}
                      ${OPENGL_gl_LIBRARY}
                      ${OPENGL_glu_LIBRARY}
                      ${OpenCV_LIBS}
                      ${CUDA_LIBRARIES})
					  
# link boost libraries
target_link_libraries(${PROJECT_NAME}
                      ${Boost_SYSTEM_LIBRARY}
                      ${Boost_FILESYSTEM_LIBRARY}
                      ${Boost_REGEX_LIBRARY})

if (UNIX)
  target_link_libraries(${PROJECT_NAME} 
                        -lpthread 
                        -lgomp
                        -ldl)
endif()

set_target_properties(${PROJECT_NAME} PROPERTIES
                      OUTPUT_NAME "${PROJECT_NAME}"
                      PROJECT_LABEL "${PROJECT_NAME}")

set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER "commands")

if(WIN32)
  if (MSVC AND NOT BUILD_SHARED_LIBS)
    set_target_properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS "/NODEFAULTLIB:atlthunk.lib /NODEFAULTLIB:atlsd.lib /DEBUG")
  endif()
endif()


