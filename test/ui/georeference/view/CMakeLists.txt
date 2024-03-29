include_directories(${CMAKE_SOURCE_DIR}/src)
include_directories(${CMAKE_SOURCE_DIR}/third_party)

set(test_filename tst_georeference_view.cpp)
get_filename_component(test_name ${test_filename} NAME_WE)

set(test_target "${test_name}")

add_executable(${test_target}
               ${test_filename}
               ${CMAKE_SOURCE_DIR}/src/inspector/ui/georeference/impl/GeoreferenceView.cpp
               ${CMAKE_SOURCE_DIR}/src/inspector/ui/georeference/impl/GeoreferenceView.h
               ${CMAKE_SOURCE_DIR}/src/inspector/ui/georeference/GeoreferenceView.h
               ${CMAKE_SOURCE_DIR}/src/inspector/interfaces/mvp.h)

target_link_libraries(${test_target} PRIVATE 
                      TidopLib::tl_core
                      Qt5::Core 
                      Qt5::Widgets 
                      Qt5::Test)

set_property(TARGET ${test_target} PROPERTY FOLDER "test/georeference_view")

set_target_properties(${test_target} PROPERTIES
                      OUTPUT_NAME ${test_target}
                      PROJECT_LABEL ${test_target})

if(ENABLE_SOLUTION_FOLDERS)
  set_target_properties(${test_target} PROPERTIES FOLDER "test/georeference_view")
endif()

add_test(NAME ${test_target} COMMAND $<TARGET_FILE:${test_target}>)
