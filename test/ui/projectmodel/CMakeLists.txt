set(test_filename tst_projectmodel.cpp)
get_filename_component(test_name ${test_filename} NAME_WE)

include_directories(${CMAKE_SOURCE_DIR}/src)
include_directories(${CMAKE_SOURCE_DIR}/third_party)
include_directories(${CMAKE_SOURCE_DIR}/test)

set(test_kind test)
set(test_KIND TEST)
set(test_target "${test_filename}")

add_executable(${test_target}
    ${test_filename}
    ${CMAKE_SOURCE_DIR}/src/inspector/ui/ProjectModel.cpp
    ${CMAKE_SOURCE_DIR}/src/inspector/ui/ProjectModel.h
    ${CMAKE_SOURCE_DIR}/src/inspector/interfaces/mvp.h
    ${CMAKE_SOURCE_DIR}/test/fake/ProjectControllerFake.cpp
    ${CMAKE_SOURCE_DIR}/test/fake/ProjectControllerFake.h
    )

target_link_libraries(${test_target} PRIVATE
                      Qt5::Core
                      Qt5::Widgets
                      Qt5::Test
                      tl_core
                      inspector_core
                      ${OpenCV_LIBS}
                      ${PIXKIT_LIBRARIES})

target_link_libraries(${test_target} PRIVATE
                      ${Boost_FILESYSTEM_LIBRARY}
                      ${Boost_SYSTEM_LIBRARY}
                      ${Boost_THREAD_LIBRARY}
                      ${Boost_DATE_TIME_LIBRARY}
                      ${Boost_CHRONO_LIBRARY})

set_property(TARGET ${test_target} PROPERTY FOLDER "test/project_model")

set_target_properties(${test_target} PROPERTIES
  OUTPUT_NAME ${test_target}
  PROJECT_LABEL "(${test_KIND}) ${test_name}")

if(ENABLE_SOLUTION_FOLDERS)
  set_target_properties(${test_target} PROPERTIES FOLDER "test/project_model")
endif()

add_test(NAME ${test_target} COMMAND $<TARGET_FILE:${test_target}>)
