MACRO(DEFINE_TEST_WIDGETS name srcs)

    include_directories(${CMAKE_SOURCE_DIR}/src)

    set(test_kind test)
    set(test_KIND TEST)
    set(test_target "${name}")

    add_executable(${test_target} ${srcs})

    target_link_libraries(${test_target} PRIVATE
                          Graphos::Core
                          Graphos::Widgets
                          TidopLib::Core
                          TidopLib::Img
                          TidopLib::Geospatial
                          Qt5::Core
                          Qt5::Widgets
                          Qt5::Test
                          Qt5::Concurrent
                          Qt5::OpenGL
                          Qt5::OpenGLExtensions
                          ${OpenCV_LIBS}
                          ${CUDA_LIBRARIES}
                          ${GDAL_LIBRARY}
						  ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY}
						  ${Boost_PROGRAM_OPTIONS_LIBRARY})

    target_link_libraries(${test_target} PRIVATE
                         ${Boost_FILESYSTEM_LIBRARY}
                         ${Boost_SYSTEM_LIBRARY})

    set_target_properties(${test_target} PROPERTIES
      OUTPUT_NAME ${test_target}
      PROJECT_LABEL "(${test_KIND}) ${name}")

    set_target_properties(${test_target} PROPERTIES FOLDER "test/widgets")

    add_test(NAME ${test_target} COMMAND $<TARGET_FILE:${test_target}>)

ENDMACRO()


add_subdirectory(siftwidget)
#add_subdirectory(siftwidget_signals)
#add_subdirectory(thumbnailswidget)
#add_subdirectory(startpagewidget)
#add_subdirectory(featurematchingwidget)
#add_subdirectory(cmvs_pmvs_widget)
#add_subdirectory(smvs_widget)
#add_subdirectory(mve_format_widget)
#add_subdirectory(nvm_format_widget)
#add_subdirectory(ori_format_widget)
#add_subdirectory(pointcloud_csv_format_widget)
#add_subdirectory(bundler_format_widget)
#add_subdirectory(dtm_inv_dist_widget)
#add_subdirectory(dtm_inv_dist_nn_widget)
