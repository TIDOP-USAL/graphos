/************************************************************************
 *                                                                      *
 *  Copyright 2016 by Tidop Research Group <daguilera@usal.es>          *
 *                                                                      *
 * This file is part of GRAPHOS - inteGRAted PHOtogrammetric Suite.     *
 *                                                                      *
 * GRAPHOS - inteGRAted PHOtogrammetric Suite is free software: you can *
 * redistribute it and/or modify it under the terms of the GNU General  *
 * Public License as published by the Free Software Foundation, either  *
 * version 3 of the License, or (at your option) any later version.     *
 *                                                                      *
 * GRAPHOS - inteGRAted PHOtogrammetric Suite is distributed in the     *
 * hope that it will be useful, but WITHOUT ANY WARRANTY; without even  *
 * the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR  *
 * PURPOSE.  See the GNU General Public License for more details.       *
 *                                                                      *
 * You should have received a copy of the GNU General Public License    *
 * along with Graphos.  If not, see <http://www.gnu.org/licenses/>.     *
 *                                                                      *
 * https://spdx.org/licenses/GPL-3.0-or-later.html                      *
 *                                                                      *
 ************************************************************************/

#include "graphos/graphos_global.h"

#include "MainWindowView.h"
#include "MainWindowModel.h"
#include "MainWindowPresenter.h"
#include "ComponentsManager.h"

#include "graphos/core/project.h"
#include "graphos/core/Application.h"
#include "graphos/core/AppStatus.h"
#include "graphos/widgets/TabWidget.h"

#ifdef GRAPHOS_HAVE_CREATE_PROJECT
#include "graphos/components/createproject/CreateProjectComponent.h"
#endif // GRAPHOS_HAVE_CREATE_PROJECT
#ifdef GRAPHOS_HAVE_OPEN_PROJECT
#include "graphos/components/openproject/OpenProjectComponent.h"
#endif // GRAPHOS_HAVE_OPEN_PROJECT
#ifdef GRAPHOS_HAVE_RECENT_PROJECTS
#include "graphos/components/recentprojects/RecentProjectsComponent.h"
#endif // GRAPHOS_HAVE_RECENT_PROJECTS
#ifdef GRAPHOS_HAVE_SAVE_PROJECT
#include "graphos/components/saveproject/SaveProjectComponent.h"
#endif // GRAPHOS_HAVE_SAVE_PROJECT
#ifdef GRAPHOS_HAVE_SAVE_PROJECT_AS
#include "graphos/components/saveprojectas/SaveProjectAsComponent.h"
#endif // GRAPHOS_HAVE_SAVE_PROJECT_AS
#ifdef GRAPHOS_HAVE_CLOSE_PROJECT
#include "graphos/components/closeproject/CloseProjectComponent.h"
#endif // GRAPHOS_HAVE_CLOSE_PROJECT
#ifdef GRAPHOS_HAVE_IMPORT_CAMERAS
#include "graphos/components/import/cameras/ImportCamerasComponent.h"
#endif // GRAPHOS_HAVE_IMPORT_CAMERAS
#ifdef GRAPHOS_HAVE_CAMERAS
#include "graphos/components/cameras/CamerasComponent.h"
#endif // GRAPHOS_HAVE_CAMERAS
#ifdef GRAPHOS_HAVE_IMAGE_LOAD
#include "graphos/components/images/ImageLoaderComponent.h"
#endif // GRAPHOS_HAVE_IMAGE_LOAD
#ifdef GRAPHOS_HAVE_FEATEXTRACT
#include "graphos/components/featextract/FeatureExtractorComponent.h"
#endif // GRAPHOS_HAVE_FEATEXTRACT
#ifdef GRAPHOS_HAVE_FEATMATCH
#include "graphos/components/featmatch/FeatureMatchingComponent.h"
#endif // GRAPHOS_HAVE_FEATMATCH
#ifdef GRAPHOS_HAVE_ORIENTATION
#include "graphos/components/orientation/OrientationComponent.h"
#endif // GRAPHOS_HAVE_ORIENTATION
#ifdef GRAPHOS_HAVE_DENSE
#include "graphos/components/dense/DensificationComponent.h"
#endif // GRAPHOS_HAVE_DENSE
#ifdef GRAPHOS_HAVE_MESH
#include "graphos/components/mesh/MeshComponent.h"
#endif // GRAPHOS_HAVE_MESH
#ifdef GRAPHOS_HAVE_UNDISTORT
#include "graphos/components/undistort/UndistortImagesComponent.h"
#endif // GRAPHOS_HAVE_UNDISTORT
#ifdef GRAPHOS_HAVE_DTM
#include "graphos/components/dtm/DTMComponent.h"
#endif // GRAPHOS_HAVE_DTM
#ifdef GRAPHOS_HAVE_ORTHOPHOTO
#include "graphos/components/orthophoto/OrthophotoComponent.h"
#endif // GRAPHOS_HAVE_ORTHOPHOTO
#ifdef GRAPHOS_HAVE_GEOREFERENCE
#include "graphos/components/georeference/GeoreferenceComponent.h"
#endif // GRAPHOS_HAVE_GEOREFERENCE
#ifdef GRAPHOS_HAVE_SCALE
#include "graphos/components/scale/ScaleComponent.h"
#endif // GRAPHOS_HAVE_SCALE
#ifdef GRAPHOS_HAVE_FLOOR_LEVEL
#include "graphos/components/floorlevel/FloorLevelComponent.h"
#endif // GRAPHOS_HAVE_FLOOR_LEVEL
#ifdef GRAPHOS_HAVE_FEATVIEWER
#include "graphos/components/featviewer/FeaturesViewerComponent.h"
#endif // GRAPHOS_HAVE_FEATVIEWER
#ifdef GRAPHOS_HAVE_MATCH_VIEWER
#include "graphos/components/matchviewer/MatchViewerComponent.h"
#endif // GRAPHOS_HAVE_MATCH_VIEWER
#ifdef GRAPHOS_HAVE_EXPORT_CAMERAS
#include "graphos/components/export/cameras/ExportCamerasComponent.h"
#endif // GRAPHOS_HAVE_EXPORT_CAMERAS
#ifdef GRAPHOS_HAVE_EXPORT_POINT_CLOUD
#include "graphos/components/export/pointcloud/ExportPointCloudComponent.h"
#endif // GRAPHOS_HAVE_EXPORT_POINT_CLOUD
#ifdef GRAPHOS_HAVE_EXPORT_MESH
#include "graphos/components/export/mesh/ExportMeshComponent.h"
#endif // GRAPHOS_HAVE_EXPORT_MESH
#ifdef GRAPHOS_HAVE_SETTINGS
#include "graphos/components/settings/SettingsComponent.h"
#endif // GRAPHOS_HAVE_SETTINGS
#ifdef GRAPHOS_HAVE_ABOUT
#include "graphos/components/about/AboutComponent.h"
#endif // GRAPHOS_HAVE_ABOUT
#ifdef GRAPHOS_HAVE_PROPERTIES
#include "graphos/components/properties/PropertiesComponent.h"
#endif // GRAPHOS_HAVE_PROPERTIES
#ifdef GRAPHOS_HAVE_VIDEO_LOAD
#include "graphos/components/loadfromvideo/LoadFromVideoComponent.h"
#endif // GRAPHOS_HAVE_VIDEO_LOAD
#include "graphos/components/crs/CoordinateReferenceSystemComponent.h"

#include <tidop/core/console.h>
#include <tidop/core/log.h>
#include <tidop/core/msg/message.h>

#include <gdal.h>
#include <cpl_conv.h>
#include <ogr_srs_api.h>

#include <QAction>

#ifdef HAVE_VLD
#include "gflags/gflags.h"
#include "vld.h"
#endif

#if defined WIN32
#include <Windows.h>
#endif

using namespace graphos;


void messageHandlerGDAL(CPLErr errorClass, int error, const char *msg) 
{
    switch (errorClass)
    {
        case CE_None:
            break;
        case CE_Debug:
            break;        
        case CE_Warning:
            tl::Message::warning("GDAL error [{}]: {}", error, msg);
            break;       
        case CE_Failure:
            tl::Message::error("GDAL error [{}]: {}", error, msg);
            break;    
        case CE_Fatal:
            tl::Message::error("GDAL error [{}]: {}", error, msg);
            break;
        default:
            break;
    } 
}

#ifdef DEBUG

void messageHandlerQt(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    QByteArray localMsg = msg.toLocal8Bit();
    switch (type) {
        case QtDebugMsg:
            tl::Message::debug("Qt: {}", msg.toStdString());
            break;
        case QtInfoMsg:
            tl::Message::info("Qt: {}", msg.toStdString());
            break;
        case QtWarningMsg:
            tl::Message::warning("Qt: {}", msg.toStdString());
            break;
        case QtCriticalMsg:
            tl::Message::error("Qt: {}", msg.toStdString());
            break;
        case QtFatalMsg:
            tl::Message::error("Qt: {}", msg.toStdString());
            abort();
    }
}
#endif // DEBUG


int main(int argc, char *argv[])
{
#ifdef DEBUG
    qInstallMessageHandler(messageHandlerQt);
#endif // DEBUG


    tl::Path app_path(argv[0]);

#ifdef TL_OS_WINDOWS
    tl::Path graphos_path = app_path.parentPath().parentPath();
    tl::Path gdal_data_path(graphos_path);
    gdal_data_path.append("gdal\\data");
    tl::Path proj_data_path(graphos_path);
    proj_data_path.append("proj");
    CPLSetConfigOption( "GDAL_DATA", gdal_data_path.toString().c_str());
#   if GDAL_VERSION_NUM >= GDAL_COMPUTE_VERSION(3,7,0)
        CPLSetConfigOption( "PROJ_DATA", proj_data_path.toString().c_str());
#   else
        std::string s_proj = proj_data_path.toString();
        const char *proj_data[] {s_proj.c_str(), nullptr};
        OSRSetPROJSearchPaths(proj_data);
#   endif
#endif // TL_OS_WINDOWS

    CPLSetErrorHandler(messageHandlerGDAL);

    Application app(argc, argv);
    Application::setApplicationName("GRAPHOS");
    Application::setApplicationDisplayName("GRAPHOS");
    Application::setApplicationVersion(GRAPHOS_VERSION);
    Application::setOrganizationName("TIDOP");

    ProjectImp project;
    app.setProject(&project);

    QIcon::setThemeName("Material");

#ifdef GRAPHOS_HAVE_CREATE_PROJECT
    CreateProjectComponent create_project_component(&app);
#endif // GRAPHOS_HAVE_CREATE_PROJECT

#ifdef GRAPHOS_HAVE_OPEN_PROJECT
    OpenProjectComponent open_project_component(&app);
#endif // GRAPHOS_HAVE_OPEN_PROJECT

#ifdef GRAPHOS_HAVE_RECENT_PROJECTS
    RecentProjectsComponent recent_projects_component(&app);
#endif // GRAPHOS_HAVE_RECENT_PROJECTS

#ifdef GRAPHOS_HAVE_SAVE_PROJECT
    SaveProjectComponent save_project_component(&app);
#endif // GRAPHOS_HAVE_SAVE_PROJECT

#ifdef GRAPHOS_HAVE_SAVE_PROJECT_AS
    SaveProjectAsComponent save_project_as_component(&app);
#endif // GRAPHOS_HAVE_SAVE_PROJECT_AS

#ifdef GRAPHOS_HAVE_CLOSE_PROJECT
    CloseProjectComponent close_project_component(&app);
#endif // GRAPHOS_HAVE_CLOSE_PROJECT

#ifdef GRAPHOS_HAVE_IMPORT_CAMERAS
    ImportCamerasComponent import_cameras_component(&app);
#endif // GRAPHOS_HAVE_IMPORT_CAMERAS

#ifdef GRAPHOS_HAVE_CAMERAS
    CamerasComponent cameras_component(&app);
#endif // GRAPHOS_HAVE_CAMERAS

#ifdef GRAPHOS_HAVE_EXPORT_CAMERAS
    ExportCamerasComponent export_cameras_component(&app);
#endif // GRAPHOS_HAVE_EXPORT_CAMERAS

#ifdef GRAPHOS_HAVE_EXPORT_POINT_CLOUD
    ExportPointCloudComponent export_point_cloud_component(&app);
#endif // GRAPHOS_HAVE_EXPORT_POINT_CLOUD

#ifdef GRAPHOS_HAVE_EXPORT_MESH
    ExportMeshComponent export_mesh_component(&app);
#endif // GRAPHOS_HAVE_EXPORT_MESH

#ifdef GRAPHOS_HAVE_IMAGE_LOAD
    ImageLoaderComponent image_loader_component(&app);
#endif // GRAPHOS_HAVE_IMAGE_LOAD

#ifdef GRAPHOS_HAVE_VIDEO_LOAD
    LoadFromVideoComponent load_video_component(&app);
#endif // GRAPHOS_HAVE_VIDEO_LOAD

#ifdef GRAPHOS_HAVE_FEATEXTRACT
    FeatureExtractorComponent feature_extractor_component(&app);
#endif // GRAPHOS_HAVE_FEATEXTRACT

#ifdef GRAPHOS_HAVE_FEATMATCH
    FeatureMatchingComponent feature_matching_component(&app);
#endif // GRAPHOS_HAVE_FEATMATCH

#ifdef GRAPHOS_HAVE_ORIENTATION
    OrientationComponent orientation_component(&app);
#endif // GRAPHOS_HAVE_ORIENTATION

#ifdef GRAPHOS_HAVE_DENSE
    DensificationComponent densification_component(&app);
#endif // GRAPHOS_HAVE_DENSE

#ifdef GRAPHOS_HAVE_MESH
    MeshComponent mesh_component(&app);
#endif // GRAPHOS_HAVE_MESH

#ifdef GRAPHOS_HAVE_GEOREFERENCE
    GeoreferenceComponent georeference_component(&app);
#endif // GRAPHOS_HAVE_GEOREFERENCE

#ifdef GRAPHOS_HAVE_SCALE
    ScaleComponent scale_component(&app);
#endif // GRAPHOS_HAVE_SCALE

#ifdef GRAPHOS_HAVE_FLOOR_LEVEL
    FloorLevelComponent floor_level_component;
#endif // GRAPHOS_HAVE_FLOOR_LEVEL

#ifdef GRAPHOS_HAVE_UNDISTORT
    UndistortImagesComponent undistort_component(&app);
#endif // GRAPHOS_HAVE_UNDISTORT

#ifdef GRAPHOS_HAVE_DTM
    DTMComponent dtm_component(&app);
#endif // GRAPHOS_HAVE_DTM

#ifdef GRAPHOS_HAVE_ORTHOPHOTO
    OrthophotoComponent orthophoto_component(&app);
#endif

#ifdef GRAPHOS_HAVE_FEATVIEWER
    FeaturesViewerComponent features_viewer_component(&app);
#endif // GRAPHOS_HAVE_FEATVIEWER

#ifdef GRAPHOS_HAVE_MATCH_VIEWER
    MatchViewerComponent match_viewer_component(&app);
#endif // GRAPHOS_HAVE_MATCH_VIEWER

#ifdef GRAPHOS_HAVE_SETTINGS
    SettingsComponent settings_component(&app);
#endif // GRAPHOS_HAVE_SETTINGS

#ifdef GRAPHOS_HAVE_ABOUT
    AboutComponent about_component(&app);
#endif // GRAPHOS_HAVE_ABOUT

#ifdef GRAPHOS_HAVE_PROPERTIES
    PropertiesComponent properties_component(&app);
#endif // GRAPHOS_HAVE_PROPERTIES

    //TabComponent tab_component(&app);

    tl::Console &console = tl::Console::instance();
    console.setMessageLevel(tl::MessageLevel::all);
    console.setTitle(Application::applicationName().toStdString());
    console.setConsoleUnicode();
    tl::Message::addMessageHandler(&console);

    // Log file
    tl::Log &log = tl::Log::instance();
    log.setMessageLevel(tl::MessageLevel::all);
    tl::Message::addMessageHandler(&log);

    bool r = false;

    if (argc > 1) {

        if (bool status = app.parse(argc, argv)) {
            r = app.runCommand();
        }

    } else {

        //    TL_TODO("Añadir como opción")
#if defined WIN32
        HWND hwnd = GetConsoleWindow();
        ShowWindow(hwnd, 0);
#endif

        app.freeMemory();

        ComponentsManager componentsManager; /// Sacar project de ComponentsManager para retrasar su inicializaci�n

        app.setMainWindow(componentsManager.mainWindowView());

        /// Load gui

        /* File menu */

#ifdef GRAPHOS_HAVE_CREATE_PROJECT
        componentsManager.registerComponent(&create_project_component);
#endif

#ifdef GRAPHOS_HAVE_OPEN_PROJECT
        componentsManager.registerComponent(&open_project_component);
#endif // GRAPHOS_HAVE_OPEN_PROJECT

#ifdef GRAPHOS_HAVE_RECENT_PROJECTS
        componentsManager.registerComponent(&recent_projects_component);
#endif // GRAPHOS_HAVE_RECENT_PROJECTS

#ifdef GRAPHOS_HAVE_SAVE_PROJECT
        componentsManager.registerComponent(&save_project_component);

        QObject::connect(componentsManager.mainWindowPresenter(), &MainWindowPresenter::save,
                         save_project_component.action(), &QAction::trigger);

#endif // GRAPHOS_HAVE_OPEN_PROJECT

#ifdef GRAPHOS_HAVE_SAVE_PROJECT_AS
        componentsManager.registerComponent(&save_project_as_component);
#endif // GRAPHOS_HAVE_OPEN_PROJECT_AS

#ifdef GRAPHOS_HAVE_CLOSE_PROJECT
        componentsManager.registerComponent(&close_project_component);
#endif // GRAPHOS_HAVE_CLOSE_PROJECT

#ifdef GRAPHOS_HAVE_IMPORT_CAMERAS
        componentsManager.registerComponent(&import_cameras_component);
#endif // GRAPHOS_HAVE_IMPORT_CAMERAS

#ifdef GRAPHOS_HAVE_CAMERAS
        componentsManager.registerComponent(&cameras_component);
#endif

#ifdef GRAPHOS_HAVE_EXPORT_CAMERAS
        componentsManager.registerComponent(&export_cameras_component);
#endif

#ifdef GRAPHOS_HAVE_EXPORT_POINT_CLOUD
        componentsManager.registerComponent(&export_point_cloud_component);
#endif // GRAPHOS_HAVE_EXPORT_POINT_CLOUD
        
#ifdef GRAPHOS_HAVE_EXPORT_MESH
        componentsManager.registerComponent(&export_mesh_component);
#endif // GRAPHOS_HAVE_EXPORT_MESH

        /* Workflow menu */

#ifdef GRAPHOS_HAVE_IMAGE_LOAD
        componentsManager.registerComponent(&image_loader_component);
#endif // GRAPHOS_HAVE_IMAGE_LOAD

#ifdef GRAPHOS_HAVE_VIDEO_LOAD
        componentsManager.registerComponent(&load_video_component);
#endif // GRAPHOS_HAVE_VIDEO_LOAD

#ifdef GRAPHOS_HAVE_FEATEXTRACT
        componentsManager.registerComponent(&feature_extractor_component, ComponentsManager::Flags::separator_before);
#endif // GRAPHOS_HAVE_FEATEXTRACT

#ifdef GRAPHOS_HAVE_FEATMATCH
        componentsManager.registerComponent(&feature_matching_component);
#endif // GRAPHOS_HAVE_FEATMATCH


#ifdef GRAPHOS_HAVE_ORIENTATION
        componentsManager.registerComponent(&orientation_component);
#endif // GRAPHOS_HAVE_ORIENTATION

#ifdef GRAPHOS_HAVE_DENSE
        componentsManager.registerComponent(&densification_component);
#endif // GRAPHOS_HAVE_DENSE

#ifdef GRAPHOS_HAVE_MESH
        componentsManager.registerComponent(&mesh_component,
                                            ComponentsManager::Flags::separator_before);
#endif // GRAPHOS_HAVE_MESH

        /* Tools menu */

#ifdef GRAPHOS_HAVE_UNDISTORT
        componentsManager.registerComponent(&undistort_component,
                                            ComponentsManager::Flags::separator_before);
#endif // GRAPHOS_HAVE_UNDISTORT

#ifdef GRAPHOS_HAVE_GEOREFERENCE
        componentsManager.registerComponent(&georeference_component,
                                            ComponentsManager::Flags::separator_before);
        QObject::connect(&georeference_component, &GeoreferenceComponent::select_crs, [&]() {
            CoordinateReferenceSystemComponent crs_component(&app);
            QObject::connect(&crs_component, &CoordinateReferenceSystemComponent::crs_changed,
                             &georeference_component, &GeoreferenceComponent::setCRS);

            crs_component.open();
                //CoordinateReferenceSystemComponent crs_component(&app);
        });
#endif // GRAPHOS_HAVE_GEOREFERENCE

#ifdef GRAPHOS_HAVE_SCALE
        componentsManager.registerComponent(&scale_component);
#endif // GRAPHOS_HAVE_SCALE

#ifdef GRAPHOS_HAVE_FLOOR_LEVEL
        componentsManager.registerComponent(&floor_level_component);
#endif // GRAPHOS_HAVE_FLOOR_LEVEL

#ifdef GRAPHOS_HAVE_DTM
        componentsManager.registerComponent(&dtm_component,
                                            ComponentsManager::Flags::separator_before);
#endif // GRAPHOS_HAVE_DTM

#ifdef GRAPHOS_HAVE_ORTHOPHOTO
        componentsManager.registerComponent(&orthophoto_component,
                                            ComponentsManager::Flags::separator_before);
#endif // GRAPHOS_HAVE_ORTHOPHOTO

#ifdef GRAPHOS_HAVE_FEATVIEWER
        componentsManager.registerComponent(&features_viewer_component,
                                            ComponentsManager::Flags::separator_before);
#endif // GRAPHOS_HAVE_FEATVIEWER

#ifdef GRAPHOS_HAVE_MATCH_VIEWER
        componentsManager.registerComponent(&match_viewer_component,
                                            ComponentsManager::Flags::separator_after);
#endif // GRAPHOS_HAVE_MATCH_VIEWER

#ifdef GRAPHOS_HAVE_SETTINGS
        componentsManager.registerComponent(&settings_component);
#endif // GRAPHOS_HAVE_SETTINGS

        /* Help menu */

#ifdef GRAPHOS_HAVE_ABOUT
        componentsManager.registerComponent(&about_component,
                                            ComponentsManager::Flags::separator_before);
#endif // GRAPHOS_HAVE_ABOUT

        properties_component.open();
        properties_component.setAlternatingRowColors(true);
        componentsManager.mainWindowView()->setPropertiesWidget(properties_component.widget());

        QObject::connect(componentsManager.mainWindowView()->tabWidget(),
                         &TabWidget::model3dChange,
                         &app, &Application::setViewer3D);

#ifdef GRAPHOS_HAVE_CREATE_PROJECT
        QObject::connect(&create_project_component, SIGNAL(project_created()),
                         componentsManager.mainWindowPresenter(), SLOT(loadProject()));
        QObject::connect(componentsManager.mainWindowPresenter(), &MainWindowPresenter::openCreateProjectDialog,
                         create_project_component.action(), &QAction::trigger);
#endif // GRAPHOS_HAVE_CREATE_PROJECT

#ifdef GRAPHOS_HAVE_OPEN_PROJECT
        QObject::connect(&open_project_component, SIGNAL(project_loaded()),
                         componentsManager.mainWindowPresenter(), SLOT(loadProject()));

        QObject::connect(componentsManager.mainWindowPresenter(), &MainWindowPresenter::openProjectDialog,
                         open_project_component.action(), &QAction::trigger);
#endif // GRAPHOS_HAVE_OPEN_PROJECT

#ifdef GRAPHOS_HAVE_RECENT_PROJECTS
        QObject::connect(&recent_projects_component, SIGNAL(open_project(QString)),
                         componentsManager.mainWindowPresenter(), SLOT(openProject(QString)));
#endif // GRAPHOS_HAVE_OPEN_PROJECT

        // No se usa
        //QObject::connect(&app, SIGNAL(image_loaded(size_t)),
        //                 componentsManager.mainWindowPresenter(), SLOT(loadImage(size_t)));


        /////TODO: por ahora hasta que refactorice MainWindow

#ifdef GRAPHOS_HAVE_CLOSE_PROJECT
        QObject::connect(&close_project_component, &CloseProjectComponent::projectClosed, [&]() {
            componentsManager.mainWindowView()->clear();
                         });
#endif // GRAPHOS_HAVE_CLOSE_PROJECT

#ifdef GRAPHOS_HAVE_IMAGE_LOAD
        QObject::connect(&image_loader_component, &ImageLoaderComponent::image_loaded,
                         componentsManager.mainWindowPresenter(), &MainWindowPresenter::loadImage);
#endif // GRAPHOS_HAVE_IMAGE_LOAD
#ifdef GRAPHOS_HAVE_VIDEO_LOAD  
        QObject::connect(&load_video_component, &LoadFromVideoComponent::frame_loaded,
                         componentsManager.mainWindowPresenter(), &MainWindowPresenter::loadImage);
#endif // GRAPHOS_HAVE_VIDEO_LOAD

#ifdef GRAPHOS_HAVE_FEATEXTRACT
        QObject::connect(&feature_extractor_component, SIGNAL(features_extracted(size_t)),
                         componentsManager.mainWindowPresenter(), SLOT(loadFeatures(size_t)));
        QObject::connect(&feature_extractor_component, SIGNAL(features_deleted()),
                         componentsManager.mainWindowPresenter(), SLOT(updateProject()));
#else
#   undef GRAPHOS_HAVE_FEATMATCH
#endif // GRAPHOS_HAVE_FEATEXTRACT

#ifdef GRAPHOS_HAVE_FEATMATCH
        QObject::connect(&feature_matching_component, SIGNAL(finished()),
                         componentsManager.mainWindowPresenter(), SLOT(updateMatches()));
        QObject::connect(&feature_matching_component, SIGNAL(matches_deleted()),
                         componentsManager.mainWindowPresenter(), SLOT(updateProject()));
#else
#   undef GRAPHOS_HAVE_ORIENTATION
#endif // GRAPHOS_HAVE_FEATMATCH

#ifdef GRAPHOS_HAVE_ORIENTATION
        QObject::connect(&orientation_component, SIGNAL(finished()),
                         componentsManager.mainWindowPresenter(), SLOT(loadOrientation()));
        QObject::connect(&orientation_component, SIGNAL(orientation_deleted()),
                         componentsManager.mainWindowPresenter(), SLOT(updateProject()));
#else
#   undef GRAPHOS_HAVE_DENSE
#endif // GRAPHOS_HAVE_ORIENTATION

#ifdef GRAPHOS_HAVE_DENSE
        QObject::connect(&densification_component, SIGNAL(finished()),
                         componentsManager.mainWindowPresenter(), SLOT(loadDenseModel()));
        QObject::connect(&densification_component, SIGNAL(densification_deleted()),
                         componentsManager.mainWindowPresenter(), SLOT(updateProject()));
#else
#   undef GRAPHOS_HAVE_MESH
#endif // GRAPHOS_HAVE_DENSE

#ifdef GRAPHOS_HAVE_MESH
        QObject::connect(&mesh_component, SIGNAL(finished()),
                         componentsManager.mainWindowPresenter(), SLOT(loadMesh()));
        QObject::connect(&mesh_component, SIGNAL(mesh_deleted()),
                         componentsManager.mainWindowPresenter(), SLOT(updateProject()));
#endif // GRAPHOS_HAVE_MESH

#ifdef GRAPHOS_HAVE_DTM
        QObject::connect(&dtm_component, SIGNAL(finished()),
                         componentsManager.mainWindowPresenter(), SLOT(loadDTM()));
        QObject::connect(&dtm_component, SIGNAL(finished()),
                         componentsManager.mainWindowPresenter(), SLOT(loadDSM()));
#else
#   undef GRAPHOS_HAVE_ORTHOPHOTO
#endif // GRAPHOS_HAVE_DTM

#ifdef GRAPHOS_HAVE_ORTHOPHOTO
        QObject::connect(&orthophoto_component, SIGNAL(finished()),
                         componentsManager.mainWindowPresenter(), SLOT(loadOrtho()));
#endif

#ifdef GRAPHOS_HAVE_FEATVIEWER
        QObject::connect(componentsManager.mainWindowView(), &MainWindowView::openKeypointsViewer,
                         &features_viewer_component, &FeaturesViewerComponent::openKeypointsViewer);
#endif // GRAPHOS_HAVE_FEATVIEWER


#ifdef GRAPHOS_HAVE_GEOREFERENCE
        QObject::connect(&georeference_component, SIGNAL(finished()),
                         componentsManager.mainWindowPresenter(), SLOT(loadOrientation()));
#endif // GRAPHOS_HAVE_GEOREFERENCE

#ifdef GRAPHOS_HAVE_MATCH_VIEWER
        QObject::connect(componentsManager.mainWindowView(), &MainWindowView::openMatchesViewer,
                         &match_viewer_component, &MatchViewerComponent::openMatchesViewer);
#endif // GRAPHOS_HAVE_MATCH_VIEWER

        QObject::connect(componentsManager.mainWindowView(), &MainWindowView::select_image,
                         &properties_component, &PropertiesComponent::selectImage);

        QObject::connect(componentsManager.mainWindowView(), &MainWindowView::select_sparse_model,
                         &properties_component, &PropertiesComponent::selectSparseModel);
        QObject::connect(componentsManager.mainWindowView(), &MainWindowView::select_dense_model,
                         &properties_component, &PropertiesComponent::selectDenseModel);
        QObject::connect(componentsManager.mainWindowView(), &MainWindowView::select_mesh_model,
                         &properties_component, &PropertiesComponent::selectMeshModel);

#ifdef GRAPHOS_HAVE_SETTINGS
        QObject::connect(componentsManager.mainWindowPresenter(), &MainWindowPresenter::openSettings,
                         settings_component.action(), &QAction::trigger);
#endif // GRAPHOS_HAVE_SETTINGS

#ifdef GRAPHOS_HAVE_SCALE
        QObject::connect(&scale_component, &ScaleComponent::open_3d_model,
                         [&]() {
                             
                             QString dense_model = QString::fromStdString(componentsManager.mainWindowModel()->denseModel().toString());
                             componentsManager.mainWindowPresenter()->open3DModel(dense_model, false);
                         });
#endif // GRAPHOS_HAVE_SCALE

        componentsManager.loadPlugins();

        app.status()->activeFlag(AppStatus::Flag::none, true);

        componentsManager.mainWindowPresenter()->open();

        r = app.exec();

#if defined WIN32
        ShowWindow(hwnd, 1);
#endif

    }



#ifdef HAVE_VLD
    // Clean up memory allocated by flags.  This is only needed to reduce
    // the quantity of "potentially leaked" reports emitted by memory
    // debugging tools such as valgrind. 
    google::ShutDownCommandLineFlags();
#endif

    return r;

}
