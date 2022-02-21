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
# Ejecutable graphos                                                                         #
#--------------------------------------------------------------------------------------------#

#TODO: Subir un nivel y cargar los componentes automaticamente de la carpeta components

project(graphos LANGUAGES CXX)

set(GRAPHOS_SOURCES
    main.cpp
    #AppStatus.cpp
    #Component.cpp
    ComponentsManager.cpp
    ProjectModel.cpp
    MainWindowModel.cpp
    MainWindowPresenter.cpp
    MainWindowView.cpp
    settings/SettingsComponent.cpp
    settings/impl/SettingsModel.cpp
    settings/impl/SettingsView.cpp
    settings/impl/SettingsPresenter.cpp
    FeaturesModel.cpp
    MatchesModel.cpp
    createproject/CreateProjectComponent.cpp
    createproject/impl/CreateProjectModel.cpp
    createproject/impl/CreateProjectView.cpp
    createproject/impl/CreateProjectPresenter.cpp
    createproject/impl/CreateProjectCommand.cpp
    openproject/OpenProjectComponent.cpp
    openproject/impl/OpenProjectModel.cpp
    openproject/impl/OpenProjectView.cpp
    openproject/impl/OpenProjectPresenter.cpp
    cameras/CamerasComponent.cpp
    cameras/impl/CamerasModel.cpp
    cameras/impl/CamerasView.cpp
    cameras/impl/CamerasPresenter.cpp
    featextract/FeatureExtractorComponent.cpp
    featextract/impl/FeatureExtractorView.cpp
    featextract/impl/FeatureExtractorModel.cpp
    featextract/impl/FeatureExtractorPresenter.cpp
    featextract/impl/FeatureExtractorCommand.cpp
    featmatch/FeatureMatchingComponent.cpp
    featmatch/impl/FeatureMatchingModel.cpp
    featmatch/impl/FeatureMatchingView.cpp
    featmatch/impl/FeatureMatchingPresenter.cpp
    featmatch/impl/FeatureMatchingCommand.cpp
    images/ImageLoaderComponent.cpp
    images/impl/ImageLoaderModel.cpp
    images/impl/ImageLoaderPresenter.cpp
    images/impl/ImageLoaderView.cpp
    images/impl/ImageLoaderCommand.cpp
    orientation/OrientationComponent.cpp
    orientation/impl/OrientationModel.cpp
    orientation/impl/OrientationView.cpp
    orientation/impl/OrientationPresenter.cpp
    orientation/impl/OrientationCommand.cpp
    densification/DensificationComponent.cpp
    densification/impl/DensificationModel.cpp
    densification/impl/DensificationView.cpp
    densification/impl/DensificationPresenter.cpp
    densification/impl/DensificationCommand.cpp
    featviewer/FeaturesViewerComponent.cpp
    featviewer/impl/FeaturesViewerView.cpp
    featviewer/impl/FeaturesViewerModel.cpp
    featviewer/impl/FeaturesViewerPresenter.cpp
    matchviewer/MatchViewerComponent.cpp
    matchviewer/impl/MatchViewerModel.cpp
    matchviewer/impl/MatchViewerView.cpp
    matchviewer/impl/MatchViewerPresenter.cpp
    export/orientations/ExportOrientationsComponent.cpp
    export/orientations/impl/ExportOrientationsModel.cpp
    export/orientations/impl/ExportOrientationsView.cpp
    export/orientations/impl/ExportOrientationsPresenter.cpp
    export/densemodel/ExportPointCloudComponent.cpp
    export/densemodel/impl/ExportPointCloudModel.cpp
    export/densemodel/impl/ExportPointCloudView.cpp
    export/densemodel/impl/ExportPointCloudPresenter.cpp
    #process/ProcessPresenter.cpp
    #process/ProcessView.cpp
    georeference/GeoreferenceComponent.cpp
    georeference/impl/GeoreferenceModel.cpp
    georeference/impl/GeoreferenceView.cpp
    georeference/impl/GeoreferencePresenter.cpp
    import/cameras/ImportCamerasComponent.cpp
    import/cameras/impl/ImportCamerasModel.cpp
    import/cameras/impl/ImportCamerasView.cpp
    import/cameras/impl/ImportCamerasPresenter.cpp
    dtm/DTMComponent.cpp
    dtm/impl/DTMModel.cpp
    dtm/impl/DTMView.cpp
    dtm/impl/DTMPresenter.cpp
    orthophoto/OrthophotoComponent.cpp
    orthophoto/impl/OrthophotoModel.cpp
    orthophoto/impl/OrthophotoView.cpp
    orthophoto/impl/OrthophotoPresenter.cpp
    about/AboutComponent.cpp
    about/impl/AboutModel.cpp
    about/impl/AboutView.cpp
    about/impl/AboutPresenter.cpp
    HelpDialog.cpp
    utils/ProgressDialog.cpp
    #utils/Progress.cpp
    #utils/Viewer3d.cpp
    utils/TabHandler.cpp
    ${CMAKE_SOURCE_DIR}/third_party/easyexif/exif.cpp
)

set(GRAPHOS_HEADERS
    ComponentsManager.h
    ProjectModel.h
    MainWindowModel.h
    MainWindowPresenter.h
    MainWindowView.h
	settings/SettingsComponent.h
    settings/SettingsModel.h
    settings/SettingsView.h
    settings/SettingsPresenter.h
    settings/impl/SettingsModel.h
    settings/impl/SettingsView.h
    settings/impl/SettingsPresenter.h
    FeaturesModel.h
    MatchesModel.h
    createproject/CreateProjectComponent.h
    createproject/CreateProjectModel.h
    createproject/CreateProjectView.h
    createproject/CreateProjectPresenter.h
    createproject/impl/CreateProjectModel.h
    createproject/impl/CreateProjectView.h
    createproject/impl/CreateProjectPresenter.h
    createproject/impl/CreateProjectCommand.h
    openproject/OpenProjectComponent.h
    openproject/OpenProjectModel.h
    openproject/OpenProjectView.h
    openproject/OpenProjectPresenter.h
    openproject/impl/OpenProjectModel.h
    openproject/impl/OpenProjectView.h
    openproject/impl/OpenProjectPresenter.h
    cameras/CamerasComponent.h
    cameras/CamerasModel.h
    cameras/CamerasView.h
    cameras/CamerasPresenter.h
    cameras/impl/CamerasModel.h
    cameras/impl/CamerasView.h
    cameras/impl/CamerasPresenter.h
    featextract/FeatureExtractorComponent.h
    featextract/FeatureExtractorView.h
    featextract/FeatureExtractorModel.h
    featextract/FeatureExtractorPresenter.h
    featextract/impl/FeatureExtractorView.h
    featextract/impl/FeatureExtractorModel.h
    featextract/impl/FeatureExtractorPresenter.h
    featextract/impl/FeatureExtractorCommand.h
    featmatch/FeatureMatchingComponent.h
    featmatch/FeatureMatchingModel.h
    featmatch/FeatureMatchingView.h
    featmatch/FeatureMatchingPresenter.h
    featmatch/impl/FeatureMatchingModel.h
    featmatch/impl/FeatureMatchingView.h
    featmatch/impl/FeatureMatchingPresenter.h
    featmatch/impl/FeatureMatchingCommand.h
    images/ImageLoaderComponent.h
    images/ImageLoaderModel.h
    images/ImageLoaderPresenter.h
    images/ImageLoaderView.h
    images/impl/ImageLoaderModel.h
    images/impl/ImageLoaderPresenter.h
    images/impl/ImageLoaderView.h
    orientation/OrientationComponent.h
    orientation/OrientationModel.h
    orientation/OrientationView.h
    orientation/OrientationPresenter.h
    orientation/impl/OrientationModel.h
    orientation/impl/OrientationView.h
    orientation/impl/OrientationPresenter.h
    orientation/impl/OrientationCommand.h
    densification/DensificationComponent.h
    densification/DensificationModel.h
    densification/DensificationView.h
    densification/DensificationPresenter.h
    densification/impl/DensificationModel.h
    densification/impl/DensificationView.h
    densification/impl/DensificationPresenter.h
    densification/impl/DensificationCommand.h
    featviewer/FeaturesViewerComponent.h
    featviewer/FeaturesViewerView.h
    featviewer/FeaturesViewerModel.h
    featviewer/FeaturesViewerPresenter.h
    featviewer/impl/FeaturesViewerView.h
    featviewer/impl/FeaturesViewerModel.h
    featviewer/impl/FeaturesViewerPresenter.h
    matchviewer/MatchViewerComponent.h
    matchviewer/MatchViewerModel.h
    matchviewer/MatchViewerView.h
    matchviewer/MatchViewerPresenter.h
    matchviewer/impl/MatchViewerModel.h
    matchviewer/impl/MatchViewerView.h
    matchviewer/impl/MatchViewerPresenter.h
    export/orientations/ExportOrientationsComponent.h
    export/orientations/ExportOrientationsModel.h
    export/orientations/ExportOrientationsView.h
    export/orientations/ExportOrientationsPresenter.h
    export/orientations/impl/ExportOrientationsModel.h
    export/orientations/impl/ExportOrientationsView.h
    export/orientations/impl/ExportOrientationsPresenter.h
    export/densemodel/ExportPointCloudComponent.h
    export/densemodel/ExportPointCloudModel.h
    export/densemodel/ExportPointCloudView.h
    export/densemodel/ExportPointCloudPresenter.h
    export/densemodel/impl/ExportPointCloudModel.h
    export/densemodel/impl/ExportPointCloudView.h
    export/densemodel/impl/ExportPointCloudPresenter.h
    georeference/GeoreferenceComponent.h
    georeference/GeoreferenceModel.h
    georeference/GeoreferencePresenter.h
    georeference/GeoreferenceView.h
    georeference/impl/GeoreferenceModel.h
    georeference/impl/GeoreferencePresenter.h
    georeference/impl/GeoreferenceView.h
    import/cameras/ImportCamerasComponent.h
    import/cameras/ImportCamerasModel.h
    import/cameras/ImportCamerasView.h
    import/cameras/ImportCamerasPresenter.h
    import/cameras/impl/ImportCamerasModel.h
    import/cameras/impl/ImportCamerasView.h
    import/cameras/impl/ImportCamerasPresenter.h
    dtm/DTMComponent.h
    dtm/DTMModel.h
    dtm/DTMView.h
    dtm/DTMPresenter.h
    dtm/impl/DTMModel.h
    dtm/impl/DTMView.h
    dtm/impl/DTMPresenter.h
    orthophoto/OrthophotoComponent.h
    orthophoto/OrthophotoModel.h
    orthophoto/OrthophotoView.h
    orthophoto/OrthophotoPresenter.h
    orthophoto/impl/OrthophotoModel.h
    orthophoto/impl/OrthophotoView.h
    orthophoto/impl/OrthophotoPresenter.h
    HelpDialog.h
    about/AboutComponent.h
    about/AboutModel.h
    about/AboutView.h
    about/AboutPresenter.h
    about/impl/AboutModel.h
    about/impl/AboutView.h
    about/impl/AboutPresenter.h
    utils/ProgressDialog.h
    #utils/Viewer3d.h
    utils/TabHandler.h
    ${CMAKE_SOURCE_DIR}/third_party/easyexif/exif.h
)

if(MSVC)
    list(APPEND GRAPHOS_SOURCES resource.h graphos.rc)
endif(MSVC)

# forms
set(GRAPHOS_FORMS
  MainWindowView.ui
  utils/ProgressDialog.ui
)

# Translation files
### TODO: Por si se quiere añadir soporte para varios idiomas
SET(GRAPHOS_TS_FILES
    ${CMAKE_SOURCE_DIR}/resources/lang/graphos_en.ts
    ${CMAKE_SOURCE_DIR}/resources/lang/graphos_es.ts
)

qt5_add_translation(GRAPHOS_QM_FILES ${GRAPHOS_TS_FILES})

QT5_ADD_RESOURCES(UI_GRAPHOS_RES 
                  ${GRAPHOS_RESOURCES} 
                  ${CMAKE_SOURCE_DIR}/res/icones.qrc)

add_executable(${PROJECT_NAME}
#add_executable(${PROJECT_NAME} WIN32
               ${GRAPHOS_SOURCES}
               ${GRAPHOS_HEADERS}
               ${GRAPHOS_FORMS}
               ${UI_GRAPHOS_RES}
               #${GRAPHOS_TS_FILES}
)

source_group("Form Files"  FILES ${GRAPHOS_FORMS})
source_group("Resource Files"  FILES ${GRAPHOS_RESOURCES} ${UI_GRAPHOS_RES})
#source_group("Translation Files"  FILES ${GRAPHOS_TS_FILES})

foreach(_source IN ITEMS ${GRAPHOS_SOURCES})
    get_filename_component(_source_path "${_source}" PATH)
    string(REPLACE "/" "\\" _group_path "${_source_path}")
    source_group("Source Files\\${_group_path}" FILES "${_source}")
endforeach()

foreach(_source IN ITEMS ${GRAPHOS_HEADERS})
    get_filename_component(_source_path "${_source}" PATH)
    string(REPLACE "/" "\\" _group_path "${_source_path}")
    source_group("Header Files\\${_group_path}" FILES "${_source}")
endforeach()

# Lincado de librerias
target_link_libraries(${PROJECT_NAME}
                      TidopLib::tl_core
                      TidopLib::tl_geom
                      TidopLib::tl_geospatial
                      TidopLib::tl_img
                      TidopLib::tl_math
                      TidopLib::tl_vect
                      TidopLib::tl_graphic
                      Qt5::Core
                      Qt5::Widgets
                      Qt5::Help
                      Qt5::Concurrent
                      Qt5::Sql
                      Qt5::PrintSupport
                      graphos_core
                      graphos_widgets
                      graphos_process
                      ${OpenCV_LIBS}
                      ${COLMAP_LIBRARIES}
                      ${CUDA_LIBRARIES}
                      ${FREEIMAGE_LIBRARIES}
                      GLEW::glew
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
  set_target_properties(${PROJECT_NAME}  PROPERTIES FOLDER "graphos")
endif()

set_target_properties(${PROJECT_NAME}
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

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


