MACRO(DEFINE_TEST_WIDGETS name srcs)

    include_directories(${CMAKE_SOURCE_DIR}/src)

    set(test_kind test)
    set(test_KIND TEST)

    set(the_target "${test_kind}_${name}")
    add_executable(${the_target} ${srcs})

    target_link_libraries(${the_target} PRIVATE
                          tl_core
                          inspector_widgets
                          Qt5::Core
                          Qt5::Widgets
                          Qt5::Test
                          Qt5::Concurrent
                          ${OpenCV_LIBS})

    target_link_libraries(${the_target} PRIVATE 
	                      ${Boost_FILESYSTEM_LIBRARY} 
                        ${Boost_SYSTEM_LIBRARY})

    set_target_properties(${the_target} PROPERTIES
      OUTPUT_NAME ${the_target}
      PROJECT_LABEL "(${test_KIND}) ${name}")

    if(ENABLE_SOLUTION_FOLDERS)
      set_target_properties(${the_target} PROPERTIES FOLDER "test/widgets")
    endif()

    add_test(NAME ${the_target} COMMAND $<TARGET_FILE:${the_target}>)

ENDMACRO()


add_subdirectory(siftwidget)
add_subdirectory(siftwidget_signals)
add_subdirectory(thumbnailswidget)
add_subdirectory(startpagewidget)
