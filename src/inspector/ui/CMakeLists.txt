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
    #ImagesModel.cpp
    FeaturesModel.cpp
    MatchesModel.cpp
    cameras/CamerasModel.cpp
    cameras/CamerasView.cpp
    cameras/CamerasPresenter.cpp
    featextract/FeatureExtractorView.cpp
    featextract/FeatureExtractorModel.cpp
    featextract/FeatureExtractorPresenter.cpp
    featmatch/FeatureMatchingModel.cpp
    featmatch/FeatureMatchingView.cpp
    featmatch/FeatureMatchingPresenter.cpp
    images/ImagesModel.cpp
    images/ImagesPresenter.cpp
    images/ImagesView.cpp
    orientation/OrientationModel.cpp
    orientation/OrientationView.cpp
    orientation/OrientationPresenter.cpp
    densification/DensificationModel.cpp
    densification/DensificationView.cpp
    densification/DensificationPresenter.cpp
    featviewer/FeaturesViewerView.cpp
    featviewer/FeaturesViewerModel.cpp
    featviewer/FeaturesViewerPresenter.cpp
    matchviewer/MatchViewerModel.cpp
    matchviewer/MatchViewerView.cpp
    matchviewer/MatchViewerPresenter.cpp
    export/orientations/ExportOrientationsModel.cpp
    export/orientations/ExportOrientationsView.cpp
    export/orientations/ExportOrientationsPresenter.cpp
    process/ProcessPresenter.cpp
    process/ProcessView.cpp
    georeference/impl/GeoreferenceModel.cpp
    georeference/impl/GeoreferenceView.cpp
    georeference/impl/GeoreferencePresenter.cpp
    import/cameras/impl/CameraPositionsModel.cpp
    import/cameras/impl/CameraPositionsView.cpp
    import/cameras/impl/CameraPositionsPresenter.cpp
    # MultiViewModel.cpp
    # MultiViewView.cpp
    # MultiViewPresenter.cpp
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
    HelpDialog.cpp
    utils/GraphicViewer.cpp
    utils/ProgressDialog.cpp
    utils/Progress.cpp
    utils/Viewer3d.cpp
    # #utils/KeyPointGraphicsItem.cpp
    # #utils/CrossGraphicItem.cpp
    # #utils/DiagonalCrossGraphicItem.cpp
    # #utils/CircleGraphicItem.cpp
    utils/GraphicItem.cpp
    utils/TabHandler.cpp
    ${CMAKE_SOURCE_DIR}/third_party/easyexif/exif.cpp
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
    #ImagesModel.h
    FeaturesModel.h
    MatchesModel.h
    cameras/Cameras.h
    cameras/CamerasModel.h
    cameras/CamerasView.h
    cameras/CamerasPresenter.h
    featextract/FeatureExtractor.h
    featextract/FeatureExtractorView.h
    featextract/FeatureExtractorModel.h
    featextract/FeatureExtractorPresenter.h
    featmatch/FeatureMatching.h
    featmatch/FeatureMatchingModel.h
    featmatch/FeatureMatchingView.h
    featmatch/FeatureMatchingPresenter.h
    images/Images.h
    images/ImagesModel.h
    images/ImagesPresenter.h
    images/ImagesView.h
    orientation/Orientation.h
    orientation/OrientationModel.h
    orientation/OrientationView.h
    orientation/OrientationPresenter.h
    densification/Densification.h
    densification/DensificationModel.h
    densification/DensificationView.h
    densification/DensificationPresenter.h
    featviewer/FeaturesViewer.h
    featviewer/FeaturesViewerView.h
    featviewer/FeaturesViewerModel.h
    featviewer/FeaturesViewerPresenter.h
    matchviewer/MatchViewer.h
    matchviewer/MatchViewerModel.h
    matchviewer/MatchViewerView.h
    matchviewer/MatchViewerPresenter.h
    export/orientations/ExportOrientations.h
    export/orientations/ExportOrientationsModel.h
    export/orientations/ExportOrientationsView.h
    export/orientations/ExportOrientationsPresenter.h
    process/ProcessPresenter.h
    process/ProcessView.h
    georeference/GeoreferenceModel.h
    georeference/GeoreferencePresenter.h
    georeference/GeoreferenceView.h
    georeference/impl/GeoreferenceModel.h
    georeference/impl/GeoreferencePresenter.h
    georeference/impl/GeoreferenceView.h
    import/cameras/CameraPositionsModel.h
    import/cameras/CameraPositionsView.h
    import/cameras/CameraPositionsPresenter.h
    import/cameras/impl/CameraPositionsModel.h
    import/cameras/impl/CameraPositionsView.h
    import/cameras/impl/CameraPositionsPresenter.h
    # Multiview.h
    # MultiViewModel.h
    # MultiViewView.h
    # MultiViewPresenter.h
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
    HelpDialog.h
    utils/GraphicViewer.h
    utils/ProgressDialog.h
    utils/Progress.h
    utils/Viewer3d.h
    # #utils/KeyPointGraphicsItem.h
    # #utils/CrossGraphicItem.h
    # #utils/DiagonalCrossGraphicItem.h
    # #utils/CircleGraphicItem.h
    utils/GraphicItem.h
    utils/TabHandler.h
    ${CMAKE_SOURCE_DIR}/third_party/easyexif/exif.h
)

if(MSVC)
    list(APPEND INSPECTOR_SOURCES resource.h inspector.rc)
endif(MSVC)

# forms
set(INSPECTOR_FORMS
  MainWindowView.ui
  # AboutDialog.ui
  utils/ProgressDialog.ui
)

# Translation files
### TODO: Por si se quiere añadir soporte para varios idiomas
SET(INSPECTOR_TS_FILES
    ${CMAKE_SOURCE_DIR}/resources/lang/inspector_en.ts
    ${CMAKE_SOURCE_DIR}/resources/lang/inspector_es.ts
)

qt5_add_translation(INSPECTOR_QM_FILES ${INSPECTOR_TS_FILES})

QT5_ADD_RESOURCES(UI_INSPECTOR_RES ${INSPECTOR_RESOURCES} )

add_executable(${PROJECT_NAME}
#add_executable(${PROJECT_NAME} WIN32
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
                      TidopLib::tl_core
                      TidopLib::tl_geom
                      TidopLib::tl_geospatial
                      TidopLib::tl_img
                      TidopLib::tl_math
                      Qt5::Core
                      Qt5::Widgets
                      Qt5::Help
                      Qt5::Concurrent
                      Qt5::Sql
                      inspector_core
                      inspector_widgets
                      inspector_process
                      ${OpenCV_LIBS}
                      ${COLMAP_LIBRARIES}
                      ${CUDA_LIBRARIES}
                      ${FREEIMAGE_LIBRARIES}
                      GLEW::GLEW
                      ${CERES_LIBRARIES}
                      ${OPENGL_gl_LIBRARY}
                      ${OPENGL_glu_LIBRARY}
                      ${GDAL_LIBRARY} 
                      ${PROJ4_LIBRARY}
)

target_link_libraries(${PROJECT_NAME}
                      ${Boost_FILESYSTEM_LIBRARY}
                      ${Boost_SYSTEM_LIBRARY}
                      ${Boost_THREAD_LIBRARY}
                      ${Boost_DATE_TIME_LIBRARY}
                      ${Boost_CHRONO_LIBRARY}
                      ${Boost_PROGRAM_OPTIONS_LIBRARY}
                      ${Boost_REGEX_LIBRARY})

if (HAVE_CLOUDCOMPARE)
target_link_libraries(${PROJECT_NAME}
                      Qt5::OpenGL
                      Qt5::OpenGLExtensions
                      ${CLOUDCOMPARE_LIBRARIES})
endif(HAVE_CLOUDCOMPARE)

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


