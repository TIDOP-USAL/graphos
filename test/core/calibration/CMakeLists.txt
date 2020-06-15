include_directories(${CMAKE_SOURCE_DIR}/src)
include_directories(${CMAKE_SOURCE_DIR}/third_party)

set(test_filename tst_calibration.cpp)
get_filename_component(test_name ${test_filename} NAME_WE)
set(test_target "${test_name}")

add_executable(${test_target} ${test_filename})

target_link_libraries(${test_target} PRIVATE
                      inspector_core
                      tl_core
                      Qt5::Core
                      Qt5::Test
                      ${COLMAP_LIBRARIES})

target_link_libraries(${test_target} PRIVATE
                      ${Boost_FILESYSTEM_LIBRARY}
                      ${Boost_SYSTEM_LIBRARY})

set_target_properties(${test_target} PROPERTIES
                      OUTPUT_NAME ${test_target}
                      PROJECT_LABEL ${test_name})

if(ENABLE_SOLUTION_FOLDERS)
    set_target_properties(${test_target} PROPERTIES 
	                      FOLDER "test/core")
endif()

add_test(NAME ${test_target} COMMAND $<TARGET_FILE:${test_target}>)
