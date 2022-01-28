set(test_filename tst_openprojectmodel.cpp)
get_filename_component(test_name ${test_filename} NAME_WE)

include_directories(${CMAKE_SOURCE_DIR}/src)

set(test_target "${test_name}")

add_executable(${test_target}
               ${test_filename}
               ${CMAKE_SOURCE_DIR}/src/graphos/components/openproject/impl/OpenProjectModel.cpp
               ${CMAKE_SOURCE_DIR}/src/graphos/components/openproject/impl/OpenProjectModel.h
               ${CMAKE_SOURCE_DIR}/src/graphos/components/openproject/OpenProjectModel.h
               ${CMAKE_SOURCE_DIR}/src/graphos/interfaces/mvp.h
               ${CMAKE_SOURCE_DIR}/test/fake/ProjectFake.cpp
               ${CMAKE_SOURCE_DIR}/test/fake/ProjectFake.h)

target_link_libraries(${test_target} PRIVATE
                      Qt5::Core 
                      Qt5::Widgets 
                      Qt5::Test
                      graphos_core
                      TidopLib::tl_core
                      TidopLib::tl_img
                      TidopLib::tl_geospatial
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
					  
set_target_properties(${test_target} PROPERTIES
                      OUTPUT_NAME ${test_target}
                      PROJECT_LABEL ${test_target})

set_target_properties(${test_target} PROPERTIES FOLDER "test/openproject/model")

add_test(NAME ${test_target} COMMAND $<TARGET_FILE:${test_target}>)
