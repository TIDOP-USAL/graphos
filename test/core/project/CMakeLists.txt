include_directories(${CMAKE_SOURCE_DIR}/src)
include_directories(${CMAKE_SOURCE_DIR}/test)
include_directories(${CMAKE_SOURCE_DIR}/third_party)

set(test_filename tst_project.cpp)
get_filename_component(test_name ${test_filename} NAME_WE)
set(test_target "${test_name}")

add_executable(${test_target}
               ${test_filename}
               #${CMAKE_SOURCE_DIR}/test/fake/ProjectFake.cpp
               #${CMAKE_SOURCE_DIR}/test/fake/ProjectFake.h
               )

target_link_libraries(${test_target} PRIVATE
                      inspector_core
                      TidopLib::tl_core
                      TidopLib::tl_img
                      Qt5::Core
                      Qt5::Test
                      Qt5::Sql
                      ${OpenCV_LIBS}
                      ${COLMAP_LIBRARIES}
                      ${FREEIMAGE_LIBRARIES}
                      GLEW::GLEW
                      ${CERES_LIBRARIES}
                      ${CUDA_LIBRARIES}
                      ${OPENGL_gl_LIBRARY}
                      ${OPENGL_glu_LIBRARY}
                      ${GDAL_LIBRARY})

target_link_libraries(${test_target} PRIVATE
                      ${Boost_FILESYSTEM_LIBRARY}
                      ${Boost_SYSTEM_LIBRARY}
                      ${Boost_REGEX_LIBRARY})

set_target_properties(${test_target} PROPERTIES
                      OUTPUT_NAME ${test_target}
                      PROJECT_LABEL ${test_target})

if(ENABLE_SOLUTION_FOLDERS)
    set_target_properties(${test_target} PROPERTIES 
	                      FOLDER "test/core")
endif()

add_test(NAME ${test_target} COMMAND $<TARGET_FILE:${test_target}>)
