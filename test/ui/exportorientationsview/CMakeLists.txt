set(test_filename tst_exportorientationsview.cpp)
get_filename_component(test_name ${test_filename} NAME_WE)

include_directories(${CMAKE_SOURCE_DIR}/src)

set(test_kind test)
set(test_KIND TEST)

set(the_target "${test_kind}_${test_name}")
add_executable(${the_target}
    ${test_filename}
    ${CMAKE_SOURCE_DIR}/src/inspector/ui/export/orientations/ExportOrientationsView.cpp
    ${CMAKE_SOURCE_DIR}/src/inspector/ui/export/orientations/ExportOrientationsView.h
    ${CMAKE_SOURCE_DIR}/src/inspector/ui/export/orientations/ExportOrientations.h
    ${CMAKE_SOURCE_DIR}/src/inspector/interfaces/mvp.h
    )

target_link_libraries(${the_target} PRIVATE Qt5::Core Qt5::Widgets Qt5::Test)

set_property(TARGET ${the_target} PROPERTY FOLDER "test/export_orientations_view")

set_target_properties(${the_target} PROPERTIES
  OUTPUT_NAME ${the_target}
  PROJECT_LABEL "(${test_KIND}) ${test_name}")

if(ENABLE_SOLUTION_FOLDERS)
  set_target_properties(${the_target} PROPERTIES FOLDER "test/export_orientations_view")
endif()

add_test(NAME ${the_target} COMMAND $<TARGET_FILE:${the_target}>)
