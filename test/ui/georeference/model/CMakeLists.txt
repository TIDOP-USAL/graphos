include_directories(${CMAKE_SOURCE_DIR}/src)
include_directories(${CMAKE_SOURCE_DIR}/third_party)
include_directories(${CMAKE_SOURCE_DIR}/test)

set(test_filename tst_georeference_model.cpp)
get_filename_component(test_name ${test_filename} NAME_WE)


set(test_target "${test_name}")

add_executable(${test_target}
               ${test_filename}
               ${CMAKE_SOURCE_DIR}/src/inspector/ui/georeference/GeoreferenceModel.h
               ${CMAKE_SOURCE_DIR}/src/inspector/ui/georeference/impl/GeoreferenceModel.cpp
               ${CMAKE_SOURCE_DIR}/src/inspector/ui/georeference/impl/GeoreferenceModel.h
               ${CMAKE_SOURCE_DIR}/src/inspector/interfaces/mvp.h)

target_link_libraries(${test_target} PRIVATE
                      inspector_core
                      TidopLib::tl_core
                      TidopLib::tl_img
                      Qt5::Core
                      Qt5::Widgets
                      Qt5::Test
                      Qt5::Sql
                      ${COLMAP_LIBRARIES}
                      ${CUDA_LIBRARIES}
                      ${FREEIMAGE_LIBRARIES}
                      GLEW::GLEW
                      ${CERES_LIBRARIES}
                      ${OPENGL_gl_LIBRARY}
                      ${OPENGL_glu_LIBRARY}
                      ${OpenCV_LIBS}
                      ${GDAL_LIBRARY})

target_link_libraries(${test_target} PRIVATE
                      ${Boost_FILESYSTEM_LIBRARY}
                      ${Boost_SYSTEM_LIBRARY}
                      ${Boost_THREAD_LIBRARY}
                      ${Boost_DATE_TIME_LIBRARY}
                      ${Boost_CHRONO_LIBRARY}
                      ${Boost_REGEX_LIBRARY})

set_property(TARGET ${test_target} PROPERTY
             FOLDER "test/georeference_model")

set_target_properties(${test_target} PROPERTIES
                      OUTPUT_NAME ${test_target}
                      PROJECT_LABEL ${test_target})

if(ENABLE_SOLUTION_FOLDERS)
  set_target_properties(${test_target} PROPERTIES
                        FOLDER "test/georeference_model")
endif()

add_test(NAME ${test_target} COMMAND $<TARGET_FILE:${test_target}>)
