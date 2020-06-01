set(test_filename tst_featurematchingview.cpp)
get_filename_component(test_name ${test_filename} NAME_WE)

include_directories(${CMAKE_SOURCE_DIR}/src)
include_directories(${CMAKE_SOURCE_DIR}/third_party)

set(test_kind test)
set(test_KIND TEST)
set(the_target "${test_name}")

add_executable(${the_target}
               ${test_filename}
               ${CMAKE_SOURCE_DIR}/src/inspector/ui/featmatch/FeatureMatchingView.cpp
               ${CMAKE_SOURCE_DIR}/src/inspector/ui/featmatch/FeatureMatchingView.h
               ${CMAKE_SOURCE_DIR}/src/inspector/ui/featmatch/FeatureMatching.h
               ${CMAKE_SOURCE_DIR}/src/inspector/interfaces/mvp.h)

target_link_libraries(${the_target} PRIVATE
                      inspector_widgets
                      tl_core
                      Qt5::Core 
                      Qt5::Widgets 
                      Qt5::Test
                      Qt5::Concurrent
                      ${OpenCV_LIBS})

set_target_properties(${the_target} PROPERTIES
                      OUTPUT_NAME ${the_target}
                      PROJECT_LABEL "(${test_KIND}) ${test_name}")

if(ENABLE_SOLUTION_FOLDERS)
  set_target_properties(${the_target} PROPERTIES
                        FOLDER "test/featurematchingview")
endif()

add_test(NAME ${the_target} COMMAND $<TARGET_FILE:${the_target}>)
