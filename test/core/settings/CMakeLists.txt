include_directories(${CMAKE_SOURCE_DIR}/src)
include_directories(${CMAKE_SOURCE_DIR}/test)
include_directories(${CMAKE_SOURCE_DIR}/third_party)

set(test_filename tst_settings.cpp)
get_filename_component(test_name ${test_filename} NAME_WE)
set(test_target "${test_name}")

add_executable(${test_target}
               ${test_filename})
			   
target_link_libraries(${test_target} PRIVATE
                      inspector_core
                      TidopLib::tl_core
                      Qt5::Core
                      Qt5::Test
                      ${OpenCV_LIBS})
					  
target_link_libraries(${test_target} PRIVATE
                      ${Boost_FILESYSTEM_LIBRARY}
                      ${Boost_SYSTEM_LIBRARY})

set_target_properties(${test_target} PROPERTIES
                      OUTPUT_NAME ${test_target}
                      PROJECT_LABEL ${test_target})
					  
set_target_properties(${test_target} PROPERTIES 
	                    FOLDER "test/core")

add_test(NAME ${test_target} COMMAND $<TARGET_FILE:${test_target}>)
