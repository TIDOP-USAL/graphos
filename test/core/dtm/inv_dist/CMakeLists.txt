include_directories(${CMAKE_SOURCE_DIR}/src)
include_directories(${CMAKE_SOURCE_DIR}/third_party)

set(test_filename tst_inv_dist.cpp)
get_filename_component(test_name ${test_filename} NAME_WE)
set(test_target "${test_name}")

add_executable(${test_target} ${test_filename})

target_link_libraries(${test_target} PRIVATE
                      graphos_core
                      TidopLib::tl_core
                      TidopLib::tl_img
                      TidopLib::tl_geospatial
                      Qt5::Core
                      Qt5::Test
                      Qt5::Sql
                      ${COLMAP_LIBRARIES}
                      ${CUDA_LIBRARIES}
                      ${FREEIMAGE_LIBRARIES}
                      GLEW::GLEW
                      ${CERES_LIBRARIES}
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
	                  FOLDER "test/core/dtm")

add_test(NAME ${test_target} COMMAND $<TARGET_FILE:${test_target}>)
