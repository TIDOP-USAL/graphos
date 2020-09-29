set(test_filename tst_settingsmodel.cpp)
get_filename_component(test_name ${test_filename} NAME_WE)

include_directories(${CMAKE_SOURCE_DIR}/src)
include_directories(${CMAKE_SOURCE_DIR}/third_party)
include_directories(${CMAKE_SOURCE_DIR}/test)

set(the_target "${test_name}")

add_executable(${the_target}
               ${test_filename}
               ${CMAKE_SOURCE_DIR}/src/inspector/ui/SettingsModel.cpp
               ${CMAKE_SOURCE_DIR}/src/inspector/ui/SettingsModel.h
               ${CMAKE_SOURCE_DIR}/src/inspector/ui/Settings.h
               ${CMAKE_SOURCE_DIR}/src/inspector/interfaces/mvp.h
               ${CMAKE_SOURCE_DIR}/test/fake/SettingsControllerFake.cpp
               ${CMAKE_SOURCE_DIR}/test/fake/SettingsControllerFake.h)

target_link_libraries(${the_target} PRIVATE
                      TidopLib::tl_core
                      Qt5::Core
                      Qt5::Widgets
                      Qt5::Test
                      inspector_core
                      ${OpenCV_LIBS}
                      ${PIXKIT_LIBRARIES})

target_link_libraries(${the_target} PRIVATE
                      ${Boost_FILESYSTEM_LIBRARY}
                      ${Boost_SYSTEM_LIBRARY}
                      ${Boost_THREAD_LIBRARY}
                      ${Boost_DATE_TIME_LIBRARY}
                      ${Boost_CHRONO_LIBRARY})

set_target_properties(${the_target} PROPERTIES
  OUTPUT_NAME ${the_target}
  PROJECT_LABEL ${the_target})

if(ENABLE_SOLUTION_FOLDERS)
  set_target_properties(${the_target} PROPERTIES
                        FOLDER "test/settings_model")
endif()


add_test(NAME ${the_target} COMMAND $<TARGET_FILE:${the_target}>)
