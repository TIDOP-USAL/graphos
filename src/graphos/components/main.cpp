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

#include "MainWindowView.h"
#include "MainWindowModel.h"
#include "MainWindowPresenter.h"
#include "ComponentsManager.h"

#include "graphos/components/createproject/CreateProjectComponent.h"
#include "graphos/components/openproject/OpenProjectComponent.h"
//#include "graphos/components/import/cameras/ImportCamerasComponent.h"
#include "graphos/components/cameras/CamerasComponent.h"
#include "graphos/components/images/ImageLoaderComponent.h"
#include "graphos/components/featextract/FeatureExtractorComponent.h"
#include "graphos/components/featmatch/FeatureMatchingComponent.h"
#include "graphos/components/orientation/OrientationComponent.h"
#include "graphos/components/dense/DensificationComponent.h"
#include "graphos/components/dtm/DTMComponent.h"
#include "graphos/components/orthophoto/OrthophotoComponent.h"
//#include "graphos/components/georeference/GeoreferenceComponent.h"
#include "graphos/components/featviewer/FeaturesViewerComponent.h"
#include "graphos/components/matchviewer/MatchViewerComponent.h"
//#include "graphos/components/export/orientations/ExportOrientationsComponent.h"
//#include "graphos/components/export/densemodel/ExportPointCloudComponent.h"
//#include "graphos/components/settings/SettingsComponent.h"
#include "graphos/components/about/AboutComponent.h"

#include "graphos/core/Application.h"
#include "graphos/core/AppStatus.h"

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

  CreateProjectComponent create_project_component(&app);
  OpenProjectComponent open_project_component(&app);
  //ImportCamerasComponent import_cameras_component(&app);
  CamerasComponent cameras_component(&app);
  //ExportOrientationsComponent export_orientations_component(&app);
  //ExportPointCloudComponent export_point_cloud_component(&app);
  ImageLoaderComponent image_loader_component(&app);
  FeatureExtractorComponent feature_extractor_component(&app);
  FeatureMatchingComponent feature_matching_component(&app);
  OrientationComponent orientation_component(&app);
  DensificationComponent densification_component(&app);
  //GeoreferenceComponent georeference_component(&app);
  DTMComponent dtm_component(&app);
  OrthophotoComponent orthophoto_component(&app);
  FeaturesViewerComponent features_viewer_component(&app);
  MatchViewerComponent match_viewer_component(&app);
  //SettingsComponent settings_component(&app);
  AboutComponent about_component(&app);
  

  tl::Console &console = tl::Console::instance();
  console.setMessageLevel(tl::MessageLevel::msg_verbose);
  console.setTitle("Graphos");
  app.messageManager()->addListener(&console);

  bool r = false;

  if (argc > 1) {

    app.addComponent(&create_project_component);
    app.addComponent(&open_project_component);
    //app.addComponent(&import_cameras_component);
    app.addComponent(&cameras_component);
    //app.addComponent(&export_orientations_component);
    //app.addComponent(&export_point_cloud_component);
    app.addComponent(&image_loader_component);
    app.addComponent(&feature_extractor_component);
    app.addComponent(&feature_matching_component);
    app.addComponent(&orientation_component);
    app.addComponent(&densification_component);
    //app.addComponent(&georeference_component);
    app.addComponent(&dtm_component);
    app.addComponent(&orthophoto_component);
    app.addComponent(&features_viewer_component);
    app.addComponent(&match_viewer_component);
    //app.addComponent(&settings_component);
    app.addComponent(&about_component);

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

    componentsManager.mainWindowView()->setCreateProjectAction(create_project_component.action());

    componentsManager.mainWindowView()->setOpenProjectAction(open_project_component.action());

    //componentsManager.mainWindowView()->setImportCamerasAction(import_cameras_component.action());

    componentsManager.mainWindowView()->setCamerasToolAction(cameras_component.action());

    //componentsManager.registerComponent(&export_orientations_component);

    //componentsManager.registerComponent(&export_point_cloud_component);

    /* Workflow menu */

    componentsManager.registerComponent(&image_loader_component,
                                        ComponentsManager::Flags::separator_after);

    componentsManager.registerComponent(&feature_extractor_component);

    componentsManager.registerComponent(&feature_matching_component);

    componentsManager.registerComponent(&orientation_component);

    componentsManager.registerComponent(&densification_component);

    /* Tools menu */

    //componentsManager.registerComponent(&georeference_component,
    //                                    ComponentsManager::Flags::separator_before);

    componentsManager.registerComponent(&dtm_component,
                                        ComponentsManager::Flags::separator_before);

    componentsManager.registerComponent(&orthophoto_component,
                                        ComponentsManager::Flags::separator_before);

    componentsManager.registerComponent(&features_viewer_component,
                                        ComponentsManager::Flags::separator_before);

    componentsManager.registerComponent(&match_viewer_component,
                                        ComponentsManager::Flags::separator_after);

    //componentsManager.registerComponent(&settings_component);

    /* Help menu */

    componentsManager.registerComponent(&about_component,
                                        ComponentsManager::Flags::separator_before);
    /// 

    QObject::connect(&create_project_component, SIGNAL(project_created()),
                     componentsManager.mainWindowPresenter(), SLOT(loadProject()));

    QObject::connect(componentsManager.mainWindowPresenter(), &MainWindowPresenter::openCreateProjectDialog,
                     create_project_component.action(), &QAction::trigger);

    QObject::connect(&open_project_component, SIGNAL(project_loaded()),
                     componentsManager.mainWindowPresenter(), SLOT(loadProject()));

    QObject::connect(componentsManager.mainWindowPresenter(), &MainWindowPresenter::openProjectDialog,
                     open_project_component.action(), &QAction::trigger);

    QObject::connect(&app, SIGNAL(image_loaded(size_t)),
                     componentsManager.mainWindowPresenter(), SLOT(loadImage(size_t)));

    /////TODO: por ahora hasta que refactorice MainWindow
    QObject::connect(&image_loader_component, SIGNAL(image_loaded(size_t)),
                     componentsManager.mainWindowPresenter(), SLOT(loadImage(size_t)));
    QObject::connect(&feature_extractor_component, SIGNAL(features_extracted(size_t)),
                     componentsManager.mainWindowPresenter(), SLOT(loadFeatures(size_t)));
    QObject::connect(&feature_extractor_component, SIGNAL(features_deleted()),
                     componentsManager.mainWindowPresenter(), SLOT(updateProject()));
    QObject::connect(&feature_matching_component, SIGNAL(finished()),
                     componentsManager.mainWindowPresenter(), SLOT(updateMatches()));
    QObject::connect(&feature_matching_component, SIGNAL(matches_deleted()),
                     componentsManager.mainWindowPresenter(), SLOT(updateProject()));
    QObject::connect(&orientation_component, SIGNAL(orientation_finished()),
                     componentsManager.mainWindowPresenter(), SLOT(loadOrientation()));
    QObject::connect(&orientation_component, SIGNAL(orientation_deleted()),
                     componentsManager.mainWindowPresenter(), SLOT(updateProject()));
    QObject::connect(&densification_component, SIGNAL(finished()),
                     componentsManager.mainWindowPresenter(), SLOT(loadDenseModel()));
    QObject::connect(&dtm_component, SIGNAL(finished()),
                     componentsManager.mainWindowPresenter(), SLOT(loadDTM()));
    QObject::connect(&orthophoto_component, SIGNAL(finished()),
                     componentsManager.mainWindowPresenter(), SLOT(loadOrtho()));
    QObject::connect(componentsManager.mainWindowView(), &MainWindowView::openKeypointsViewer,
                     &features_viewer_component, &FeaturesViewerComponent::openKeypointsViewer);
    QObject::connect(componentsManager.mainWindowView(), &MainWindowView::openMatchesViewer,
                     &match_viewer_component, &MatchViewerComponent::openMatchesViewer);
    //QObject::connect(&georeference_component, SIGNAL(georeferenceFinished()),
    //                 componentsManager.mainWindowPresenter(), SLOT(loadOrientation()));

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
