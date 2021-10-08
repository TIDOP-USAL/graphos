include_directories(${CMAKE_SOURCE_DIR}/src)
include_directories(${CMAKE_SOURCE_DIR}/third_party)

set(test_filename tst_sift.cpp)
get_filename_component(test_name ${test_filename} NAME_WE)
set(test_target "${test_name}")

add_executable(${test_target} ${test_filename})

target_link_libraries(${test_target} PRIVATE
                      graphos_core
                      TidopLib::tl_core
                      Qt5::Core
                      Qt5::Test
                      ${COLMAP_LIBRARIES}
                      ${CUDA_LIBRARIES}
                      ${FREEIMAGE_LIBRARIES}
                      ${CERES_LIBRARIES}
                      GLEW::GLEW
                      ${OPENGL_gl_LIBRARY}
                      ${OPENGL_glu_LIBRARY}
                      ${OpenCV_LIBS}
                      ${GDAL_LIBRARY})

target_link_libraries(${test_target} PRIVATE
                      ${Boost_FILESYSTEM_LIBRARY}
                      ${Boost_SYSTEM_LIBRARY}
                      ${Boost_REGEX_LIBRARY})

set_target_properties(${test_target} PROPERTIES
                      OUTPUT_NAME ${test_target}
                      PROJECT_LABEL ${test_name})

set_target_properties(${test_target} PROPERTIES 
	                    FOLDER "test/core/features")

add_test(NAME ${test_target} COMMAND $<TARGET_FILE:${test_target}>)
