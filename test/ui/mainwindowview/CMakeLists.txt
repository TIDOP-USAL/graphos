set(test_filename tst_mainwindowview.cpp)
get_filename_component(test_name ${test_filename} NAME_WE)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_SOURCE_DIR}/src)
include_directories(${CMAKE_SOURCE_DIR}/third_party)

set(test_kind test)
set(test_KIND TEST)
set(test_target "${test_name}")

add_executable(${test_target}
               ${test_filename}
               ${CMAKE_SOURCE_DIR}/src/inspector/ui/MainWindowView.cpp
               ${CMAKE_SOURCE_DIR}/src/inspector/ui/MainWindowView.h
               ${CMAKE_SOURCE_DIR}/src/inspector/ui/MainWindowView.ui
               ${CMAKE_SOURCE_DIR}/src/inspector/ui/utils/GraphicViewer.cpp
               ${CMAKE_SOURCE_DIR}/src/inspector/ui/utils/GraphicViewer.h
               ${CMAKE_SOURCE_DIR}/src/inspector/ui/utils/GraphicItem.cpp
               ${CMAKE_SOURCE_DIR}/src/inspector/ui/utils/GraphicItem.h
               ${CMAKE_SOURCE_DIR}/src/inspector/ui/utils/TabHandler.cpp
               ${CMAKE_SOURCE_DIR}/src/inspector/ui/utils/TabHandler.h
               ${CMAKE_SOURCE_DIR}/src/inspector/ui/utils/Viewer3d.cpp
               ${CMAKE_SOURCE_DIR}/src/inspector/ui/utils/Viewer3d.h
               )

target_link_libraries(${test_target} PRIVATE
                      inspector_core
                      inspector_widgets
                      tl_core
                      Qt5::Core
                      Qt5::Widgets
                      Qt5::Test
                      Qt5::Concurrent
                      ${OpenCV_LIBS})

target_link_libraries(${test_target} PRIVATE
                      ${Boost_FILESYSTEM_LIBRARY}
                      ${Boost_SYSTEM_LIBRARY})

if (HAVE_CLOUDCOMPARE)
target_link_libraries(${test_target} PRIVATE
                      Qt5::OpenGL
                      Qt5::OpenGLExtensions
                      ${CLOUDCOMPARE_LIBRARIES})
endif(HAVE_CLOUDCOMPARE)

set_target_properties(${test_target} PROPERTIES
                      OUTPUT_NAME ${test_target}
                      PROJECT_LABEL "(${test_KIND}) ${test_name}")

if(ENABLE_SOLUTION_FOLDERS)
  set_target_properties(${test_target} PROPERTIES
                        FOLDER "test/mainwindowview")
endif()

add_test(NAME ${test_target} COMMAND $<TARGET_FILE:${test_target}>)
