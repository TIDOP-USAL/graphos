include_directories(${CMAKE_SOURCE_DIR}/src)
include_directories(${CMAKE_SOURCE_DIR}/third_party)

set(test_filename tst_featureextractorview.cpp)
get_filename_component(test_name ${test_filename} NAME_WE)
set(test_target "${test_name}")

add_executable(${test_target}
               ${test_filename}
               ${CMAKE_SOURCE_DIR}/src/inspector/ui/featextract/FeatureExtractorView.cpp
               ${CMAKE_SOURCE_DIR}/src/inspector/ui/featextract/FeatureExtractorView.h
               ${CMAKE_SOURCE_DIR}/src/inspector/ui/featextract/FeatureExtractor.h
               ${CMAKE_SOURCE_DIR}/src/inspector/ui/process/ProcessPresenter.h
               ${CMAKE_SOURCE_DIR}/src/inspector/interfaces/mvp.h)

target_link_libraries(${test_target} PRIVATE
                      inspector_core
                      inspector_widgets
                      TidopLib::tl_core
                      TidopLib::tl_img
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
  set_target_properties(${test_name} PROPERTIES
                        FOLDER "test/featureextractor_view")
endif()

add_test(NAME ${test_name} COMMAND $<TARGET_FILE:${test_name}>)
