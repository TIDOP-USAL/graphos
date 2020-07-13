include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_SOURCE_DIR}/src)
include_directories(${CMAKE_SOURCE_DIR}/third_party)
	
project(create_project_service)

add_executable(${PROJECT_NAME}
               main.cpp
               service/service.cpp
               service/service.h
               controller/Controller.h
               controller/CreateProjectController.cpp
               controller/CreateProjectController.h
               model/CreateProjectModel.cpp
               model/CreateProjectModel.h
               ${CMAKE_SOURCE_DIR}/src/inspector/interfaces/mvp.h
               ${CMAKE_SOURCE_DIR}/src/inspector/interfaces/CreateProject.h)

target_link_libraries(${PROJECT_NAME}
                      inspector_core
                      tl_core
                      cpprestsdk::cpprest
                      Qt5::Core
                      Qt5::Sql
                      Qt5::Widgets
                      ${OpenCV_LIBS}
                      ${COLMAP_LIBRARIES}
                      ${FREEIMAGE_LIBRARIES}
                      ${GLEW_LIBRARIES}
                      ${CERES_LIBRARIES}
                      ${OPENGL_gl_LIBRARY}
                      ${OPENGL_glu_LIBRARY}
                      ${CUDA_LIBRARIES})

# link boost libraries
target_link_libraries(${PROJECT_NAME}
                      ${Boost_FILESYSTEM_LIBRARY}
                      ${Boost_SYSTEM_LIBRARY}
                      ${Boost_REGEX_LIBRARY}
                      ${Boost_FILESYSTEM_LIBRARY} 
                      ${CURL_LIBRARIES})


set_target_properties(${PROJECT_NAME} PROPERTIES
                      OUTPUT_NAME "${PROJECT_NAME}"
                      PROJECT_LABEL "${PROJECT_NAME}")


set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER "microservices")


if(WIN32)

  if (MSVC AND NOT BUILD_SHARED_LIBS)
    set_target_properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS "/NODEFAULTLIB:atlthunk.lib /NODEFAULTLIB:atlsd.lib /DEBUG")
  endif()
endif()


