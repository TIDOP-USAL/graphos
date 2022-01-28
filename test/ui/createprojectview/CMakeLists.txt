set(test_filename tst_createprojectview.cpp)
get_filename_component(test_name ${test_filename} NAME_WE)

include_directories(${CMAKE_SOURCE_DIR}/src)

set(test_target "${test_name}")

add_executable(${test_target}
               ${test_filename}
               ${CMAKE_SOURCE_DIR}/src/graphos/components/createproject/impl/CreateProjectView.cpp
               ${CMAKE_SOURCE_DIR}/src/graphos/components/createproject/impl/CreateProjectView.h
               ${CMAKE_SOURCE_DIR}/src/graphos/components/createproject/CreateProjectView.h
               ${CMAKE_SOURCE_DIR}/src/graphos/interfaces/mvp.h)

target_link_libraries(${test_target} PRIVATE
                      Qt5::Core 
                      Qt5::Widgets 
                      Qt5::Test)

set_target_properties(${test_target} PROPERTIES
                      OUTPUT_NAME ${test_target}
                      PROJECT_LABEL ${test_target})

set_target_properties(${test_target} PROPERTIES FOLDER "test/createproject/view")

add_test(NAME ${test_target} COMMAND $<TARGET_FILE:${test_target}>)
