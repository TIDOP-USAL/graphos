include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_SOURCE_DIR}/src)
include_directories(${CMAKE_SOURCE_DIR}/third_party)

#--------------------------------------------------------------------------------------------#
# Configuración para Qt                                                                      #
#--------------------------------------------------------------------------------------------#

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)
# Instruct CMake to run moc automatically when needed
set(CMAKE_AUTOMOC ON)
# Create code from a list of Qt designer ui files
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

#--------------------------------------------------------------------------------------------#
# Ejecutable inspector                                                                             #
#--------------------------------------------------------------------------------------------#

project(inspector_image)

set(INSPECTOR_SOURCES
    main.cpp
    ComponentsManager.cpp
    ProjectModel.cpp
    MainWindowModel.cpp
    MainWindowPresenter.cpp
    MainWindowView.cpp
    NewProjectView.cpp
    NewProjectPresenter.cpp
    SettingsModel.cpp
    SettingsPresenter.cpp
    SettingsView.cpp
    # PreprocessView.cpp
    # PreprocessModel.cpp
    # PreprocessPresenter.cpp
    # FeatureExtractorView.cpp
    # FeatureExtractorModel.cpp
    # FeatureExtractorPresenter.cpp
    # FeaturesViewerView.cpp
    # FeaturesViewerModel.cpp
    # FeaturesViewerPresenter.cpp
    # DescriptorMatcherView.cpp
    # DescriptorMatcherModel.cpp
    # DescriptorMatcherPresenter.cpp
    # MatchViewerModel.cpp
    # MatchViewerView.cpp
    # MatchViewerPresenter.cpp
    # MultiViewModel.cpp
    # MultiViewView.cpp
    # MultiViewPresenter.cpp
    # HomographyViewerModel.cpp
    # HomographyViewerView.cpp
    # HomographyViewerPresenter.cpp
    # CurvesViewerModel.cpp
    # CurvesViewerView.cpp
    # CurvesViewerPresenter.cpp
    # GroundTruthModel.cpp
    # GroundTruthView.cpp
    # GroundTruthPresenter.cpp
    # RepeatabilityModel.cpp
    # RepeatabilityView.cpp
    # RepeatabilityPresenter.cpp
    # ExportFeaturesModel.cpp
    # ExportFeaturesView.cpp
    # ExportFeaturesPresenter.cpp
    # ExportMatchesModel.cpp
    # ExportMatchesView.cpp
    # ExportMatchesPresenter.cpp
    # #BatchModel.cpp
    # #BatchView.cpp
    # #BatchPresenter.cpp
    # AboutDialog.cpp
    # HelpDialog.cpp
    utils/GraphicViewer.cpp
    # utils/ProgressDialog.cpp
    # utils/Progress.cpp
    # #utils/KeyPointGraphicsItem.cpp
    # #utils/CrossGraphicItem.cpp
    # #utils/DiagonalCrossGraphicItem.cpp
    # #utils/CircleGraphicItem.cpp
    utils/GraphicItem.cpp
    utils/TabHandler.cpp
    # ${CMAKE_SOURCE_DIR}/third_party/easyexif/exif.cpp
)

set(INSPECTOR_HEADERS
    ${CMAKE_SOURCE_DIR}/src/inspector/interfaces/mvp.h
    ComponentsManager.h
    ComponentsManager.cpp
    ProjectModel.h
    MainWindowModel.h
    MainWindowPresenter.h
    MainWindowView.h
    NewProject.h
    NewProjectView.h
    NewProjectPresenter.h
    Settings.h
    SettingsModel.h
    SettingsPresenter.h
    SettingsView.h
    # Preprocess.h
    # PreprocessView.h
    # PreprocessModel.h
    # PreprocessPresenter.h
    # FeatureExtractor.h
    # FeatureExtractorView.h
    # FeatureExtractorModel.h
    # FeatureExtractorPresenter.h
    # FeaturesViewer.h
    # FeaturesViewerView.h
    # FeaturesViewerModel.h
    # FeaturesViewerPresenter.h
    # DescriptorMatcher.h
    # DescriptorMatcherView.h
    # DescriptorMatcherModel.h
    # DescriptorMatcherPresenter.h
    # MatchViewer.h
    # MatchViewerModel.h
    # MatchViewerView.h
    # MatchViewerPresenter.h
    # Multiview.h
    # MultiViewModel.h
    # MultiViewView.h
    # MultiViewPresenter.h
    # HomographyViewer.h
    # HomographyViewerModel.h
    # HomographyViewerView.h
    # HomographyViewerPresenter.h
    # CurvesViewer.h
    # CurvesViewerModel.h
    # CurvesViewerView.h
    # CurvesViewerPresenter.h
    # GroundTruth.h
    # GroundTruthModel.h
    # GroundTruthView.h
    # GroundTruthPresenter.h
    # Repeatability.h
    # RepeatabilityModel.h
    # RepeatabilityView.h
    # RepeatabilityPresenter.h
    # ExportFeatures.h
    # ExportFeaturesModel.h
    # ExportFeaturesView.h
    # ExportFeaturesPresenter.h
    # ExportMatches.h
    # ExportMatchesModel.h
    # ExportMatchesView.h
    # ExportMatchesPresenter.h
    # #BatchModel.h
    # #BatchView.h
    # #BatchPresenter.h
    # AboutDialog.h
    # HelpDialog.h
    utils/GraphicViewer.h
    # utils/ProgressDialog.h
    # utils/Progress.h
    # #utils/KeyPointGraphicsItem.h
    # #utils/CrossGraphicItem.h
    # #utils/DiagonalCrossGraphicItem.h
    # #utils/CircleGraphicItem.h
    utils/GraphicItem.h
    utils/TabHandler.h
    # ${CMAKE_SOURCE_DIR}/third_party/easyexif/exif.h
)

if(MSVC)
    list(APPEND INSPECTOR_SOURCES resource.h inspector.rc)
endif(MSVC)

# forms
set(INSPECTOR_FORMS
  MainWindowView.ui
  # AboutDialog.ui
  # utils/ProgressDialog.ui
)

# Translation files
### TODO: Por si se quiere añadir soporte para varios idiomas
#SET(INSPECTOR_TS_FILES
#    ${CMAKE_SOURCE_DIR}/resources/lang/inspector_en.ts
#    ${CMAKE_SOURCE_DIR}/resources/lang/inspector_es.ts
#)

#qt5_add_translation(INSPECTOR_QM_FILES ${INSPECTOR_TS_FILES})

QT5_ADD_RESOURCES(UI_INSPECTOR_RES ${INSPECTOR_RESOURCES} )

add_executable(${PROJECT_NAME} WIN32
               ${INSPECTOR_SOURCES}
               ${INSPECTOR_HEADERS}
               ${INSPECTOR_FORMS}
               ${UI_INSPECTOR_RES}
               #${INSPECTOR_TS_FILES}
)

source_group("Form Files"  FILES ${INSPECTOR_FORMS})
source_group("Resource Files"  FILES ${INSPECTOR_RESOURCES})
#source_group("Translation Files"  FILES ${INSPECTOR_TS_FILES})

# Lincado de librerias
target_link_libraries(${PROJECT_NAME}
                      tl_core
                      Qt5::Core
                      Qt5::Widgets
                      Qt5::Help
                      Qt5::Concurrent
                      inspector_core
                      inspector_widgets
                      #inspector_process
                      ${OpenCV_LIBS}
)

target_link_libraries(${PROJECT_NAME}
                      ${Boost_FILESYSTEM_LIBRARY}
                      ${Boost_SYSTEM_LIBRARY}
                      ${Boost_THREAD_LIBRARY}
                      ${Boost_DATE_TIME_LIBRARY}
                      ${Boost_CHRONO_LIBRARY})

if(HAVE_VLD)
target_link_libraries(${PROJECT_NAME} ${VLD_LIBRARY})
endif()

if(ENABLE_SOLUTION_FOLDERS)
  set_target_properties(${PROJECT_NAME}  PROPERTIES FOLDER "inspector")
endif()


##############################################################
# Install                                                    #
##############################################################

if(WIN32)
    if (MSVC AND NOT BUILD_SHARED_LIBS)
        set_target_properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS "/NODEFAULTLIB:atlthunk.lib /NODEFAULTLIB:atlsd.lib /DEBUG")
    endif()

    install(TARGETS ${PROJECT_NAME}
            RUNTIME DESTINATION "${PROJECT_BINARY_DIR}"
            COMPONENT bin)
endif()


