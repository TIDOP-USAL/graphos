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

#include "graphos/core/Application.h"
#include "graphos/core/AppStatus.h"

#ifdef GRAPHOS_HAVE_CREATE_PROJECT
#include "graphos/components/createproject/CreateProjectComponent.h"
#endif // GRAPHOS_HAVE_CREATE_PROJECT
#ifdef GRAPHOS_HAVE_OPEN_PROJECT
#include "graphos/components/openproject/OpenProjectComponent.h"
#endif // GRAPHOS_HAVE_OPEN_PROJECT
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
#ifdef GRAPHOS_HAVE_FEATVIEWER
#include "graphos/components/featviewer/FeaturesViewerComponent.h"
#endif // GRAPHOS_HAVE_FEATVIEWER
#ifdef GRAPHOS_HAVE_MATCH_VIEWER
#include "graphos/components/matchviewer/MatchViewerComponent.h"
#endif // GRAPHOS_HAVE_MATCH_VIEWER
//#include "graphos/components/export/orientations/ExportOrientationsComponent.h"
//#include "graphos/components/export/densemodel/ExportPointCloudComponent.h"
#ifdef GRAPHOS_HAVE_SETTINGS
#include "graphos/components/settings/SettingsComponent.h"
#endif // GRAPHOS_HAVE_SETTINGS
#ifdef GRAPHOS_HAVE_ABOUT
#include "graphos/components/about/AboutComponent.h"
#endif // GRAPHOS_HAVE_ABOUT
//#include "graphos/components/tab/TabComponent.h"


#include <tidop/core/console.h>

#include <QApplication>
#include <QAction>

#ifdef HAVE_VLD
#include "gflags/gflags.h"
#include "vld.h"
#endif

//#if defined WIN32
//#include <Windows.h>
//#endif

using namespace graphos;

int main(int argc, char *argv[])
{

  QApplication a(argc, argv);
  Application &app = Application::instance();

#ifdef GRAPHOS_HAVE_CREATE_PROJECT
  CreateProjectComponent create_project_component(&app);
#endif // GRAPHOS_HAVE_CREATE_PROJECT

#ifdef GRAPHOS_HAVE_OPEN_PROJECT
  OpenProjectComponent open_project_component(&app);
#endif // GRAPHOS_HAVE_OPEN_PROJECT

#ifdef GRAPHOS_HAVE_IMPORT_CAMERAS
  ImportCamerasComponent import_cameras_component(&app);
#endif // GRAPHOS_HAVE_IMPORT_CAMERAS

#ifdef GRAPHOS_HAVE_CAMERAS
  CamerasComponent cameras_component(&app);
#endif // GRAPHOS_HAVE_CAMERAS

  //ExportOrientationsComponent export_orientations_component(&app);
  //ExportPointCloudComponent export_point_cloud_component(&app);

#ifdef GRAPHOS_HAVE_IMAGE_LOAD
  ImageLoaderComponent image_loader_component(&app);
#endif // GRAPHOS_HAVE_IMAGE_LOAD

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

  //TabComponent tab_component(&app);

  tl::Console &console = tl::Console::instance();
  console.setMessageLevel(tl::MessageLevel::msg_verbose);
  console.setTitle("Graphos");
  app.messageManager()->addListener(&console);

  bool r = false;

  if (argc > 1) {

    tl::CommandList::Status status = app.parse(argc, argv);
    if (status == tl::CommandList::Status::parse_success) {

      r = app.runCommand();

    }
  } else {
    //    TL_TODO("Añadir como opción")
//#if defined WIN32
//    HWND hwnd = GetConsoleWindow();
//    ShowWindow(hwnd, 0);
//#endif

    app.freeMemory();

    ComponentsManager componentsManager; /// Sacar project de ComponentsManager para retrasar su inicialización

    /// Load gui

    /* File menu */

#ifdef GRAPHOS_HAVE_CREATE_PROJECT
    componentsManager.mainWindowView()->setCreateProjectAction(create_project_component.action());
#endif

#ifdef GRAPHOS_HAVE_OPEN_PROJECT
    componentsManager.mainWindowView()->setOpenProjectAction(open_project_component.action());
#endif // GRAPHOS_HAVE_OPEN_PROJECT

#ifdef GRAPHOS_HAVE_IMPORT_CAMERAS
    componentsManager.mainWindowView()->setImportCamerasAction(import_cameras_component.action());
#endif // GRAPHOS_HAVE_IMPORT_CAMERAS

#ifdef GRAPHOS_HAVE_CAMERAS
    componentsManager.mainWindowView()->setCamerasToolAction(cameras_component.action());
#endif

    //componentsManager.mainWindowView()->setTabWidget(tab_component.widget());

    //componentsManager.registerComponent(&export_orientations_component);

    //componentsManager.registerComponent(&export_point_cloud_component);

    /* Workflow menu */

#ifdef GRAPHOS_HAVE_IMAGE_LOAD
    componentsManager.registerComponent(&image_loader_component,
                                        ComponentsManager::Flags::separator_after);
#endif // GRAPHOS_HAVE_IMAGE_LOAD

#ifdef GRAPHOS_HAVE_FEATEXTRACT
    componentsManager.registerComponent(&feature_extractor_component);
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
#endif // GRAPHOS_HAVE_GEOREFERENCE

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


    QObject::connect(&app, SIGNAL(image_loaded(size_t)),
                     componentsManager.mainWindowPresenter(), SLOT(loadImage(size_t)));

    /////TODO: por ahora hasta que refactorice MainWindow
#ifdef GRAPHOS_HAVE_IMAGE_LOAD
    QObject::connect(&image_loader_component, SIGNAL(image_loaded(size_t)),
                     componentsManager.mainWindowPresenter(), SLOT(loadImage(size_t)));
#endif // GRAPHOS_HAVE_IMAGE_LOAD

#ifdef GRAPHOS_HAVE_FEATEXTRACT
    QObject::connect(&feature_extractor_component, SIGNAL(features_extracted(size_t)),
                     componentsManager.mainWindowPresenter(), SLOT(loadFeatures(size_t)));
    QObject::connect(&feature_extractor_component, SIGNAL(features_deleted()),
                     componentsManager.mainWindowPresenter(), SLOT(updateProject()));
#endif // GRAPHOS_HAVE_FEATEXTRACT

#ifdef GRAPHOS_HAVE_FEATMATCH
    QObject::connect(&feature_matching_component, SIGNAL(finished()),
                     componentsManager.mainWindowPresenter(), SLOT(updateMatches()));
    QObject::connect(&feature_matching_component, SIGNAL(matches_deleted()),
                     componentsManager.mainWindowPresenter(), SLOT(updateProject()));
#endif // GRAPHOS_HAVE_FEATMATCH

#ifdef GRAPHOS_HAVE_ORIENTATION
    QObject::connect(&orientation_component, SIGNAL(finished()),
                     componentsManager.mainWindowPresenter(), SLOT(loadOrientation()));
    QObject::connect(&orientation_component, SIGNAL(orientation_deleted()),
                     componentsManager.mainWindowPresenter(), SLOT(updateProject()));
#endif // GRAPHOS_HAVE_ORIENTATION

#ifdef GRAPHOS_HAVE_DENSE
    QObject::connect(&densification_component, SIGNAL(finished()),
                     componentsManager.mainWindowPresenter(), SLOT(loadDenseModel()));
#endif // GRAPHOS_HAVE_DENSE

#ifdef GRAPHOS_HAVE_DTM
    QObject::connect(&dtm_component, SIGNAL(finished()),
                     componentsManager.mainWindowPresenter(), SLOT(loadDTM()));
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

    QObject::connect(componentsManager.mainWindowView(), &MainWindowView::openMatchesViewer,
                     &match_viewer_component, &MatchViewerComponent::openMatchesViewer);


    //QObject::connect(componentsManager.mainWindowView(), &MainWindowView::openImage,
    //                 &tab_component, &TabComponent::openImage);

    componentsManager.loadPlugins();

    app.status()->activeFlag(AppStatus::Flag::none, true);

    componentsManager.mainWindowPresenter()->open();

    r = a.exec();

    //#if defined WIN32
    //    ShowWindow(hwnd, 1);
    //#endif
  }

#ifdef HAVE_VLD
  // Clean up memory allocated by flags.  This is only needed to reduce
  // the quantity of "potentially leaked" reports emitted by memory
  // debugging tools such as valgrind. 
  google::ShutDownCommandLineFlags();
#endif

  return r;
}
