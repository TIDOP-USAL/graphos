include_directories(${CMAKE_SOURCE_DIR}/src)
include_directories(${CMAKE_SOURCE_DIR}/third_party)

set(test_filename tst_featurematchingview.cpp)
get_filename_component(test_name ${test_filename} NAME_WE)
set(test_target "${test_name}")

add_executable(${test_target}
               ${test_filename}
               ${CMAKE_SOURCE_DIR}/src/graphos/ui/featmatch/impl/FeatureMatchingView.cpp
               ${CMAKE_SOURCE_DIR}/src/graphos/ui/featmatch/impl/FeatureMatchingView.h
               ${CMAKE_SOURCE_DIR}/src/graphos/ui/featmatch/FeatureMatchingView.h
               ${CMAKE_SOURCE_DIR}/src/graphos/core/process/ProcessView.h
               ${CMAKE_SOURCE_DIR}/src/graphos/interfaces/mvp.h)

target_link_libraries(${test_target} PRIVATE
                      graphos_core
                      graphos_widgets
                      TidopLib::tl_core
                      TidopLib::tl_img
                      TidopLib::tl_geospatial
                      Qt5::Core 
                      Qt5::Widgets 
                      Qt5::Test
                      Qt5::Concurrent
                      ${OpenCV_LIBS}
                      ${CUDA_LIBRARIES}
                      ${GDAL_LIBRARY})

target_link_libraries(${test_target} PRIVATE
                      ${Boost_FILESYSTEM_LIBRARY}
                      ${Boost_SYSTEM_LIBRARY}
                      ${Boost_REGEX_LIBRARY})

set_target_properties(${test_target} PROPERTIES
                      OUTPUT_NAME ${test_target}
                      PROJECT_LABEL ${test_name})

if(ENABLE_SOLUTION_FOLDERS)
  set_target_properties(${test_target} PROPERTIES
                        FOLDER "test/featurematching/view")
endif()

add_test(NAME ${test_target} COMMAND $<TARGET_FILE:${test_target}>)
