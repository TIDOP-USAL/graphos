/************************************************************************
 *                                                                      *
 *  Copyright 2016 by Tidop Research Group <daguilera@usal.se>          *
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
 * along with Foobar.  If not, see <http://www.gnu.org/licenses/>.      *
 *                                                                      *
 * @license GPL-3.0+ <http://spdx.org/licenses/GPL-3.0+>                *
 *                                                                      *
 ************************************************************************/

#include "MainWindowView.h"
#include "MainWindowModel.h"
#include "MainWindowPresenter.h"
#include "ComponentsManager.h"

#include "graphos/components/createproject/CreateProjectComponent.h"
#include "graphos/components/openproject/OpenProjectComponent.h"
#include "graphos/components/import/cameras/ImportCamerasComponent.h"
#include "graphos/components/cameras/CamerasComponent.h"
#include "graphos/components/images/ImageLoaderComponent.h"
#include "graphos/components/featextract/FeatureExtractorComponent.h"
#include "graphos/components/featmatch/FeatureMatchingComponent.h"
#include "graphos/components/orientation/OrientationComponent.h"
#include "graphos/components/densification/DensificationComponent.h"
#include "graphos/components/dtm/DTMComponent.h"
#include "graphos/components/orthophoto/OrthophotoComponent.h"
#include "graphos/components/georeference/GeoreferenceComponent.h"
#include "graphos/components/featviewer/FeaturesViewerComponent.h"
#include "graphos/components/matchviewer/MatchViewerComponent.h"
#include "graphos/components/export/orientations/ExportOrientationsComponent.h"
#include "graphos/components/export/densemodel/ExportPointCloudComponent.h"
#include "graphos/components/settings/SettingsComponent.h"
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

  ComponentsManager componentsManager; /// Sacar project de ComponentsManager para retrasar su inicialización

  /// Load Components

  CreateProjectComponent create_project_component(&app);
  app.addComponent(&create_project_component);

  OpenProjectComponent open_project_component(&app);
  app.addComponent(&open_project_component);

  ImportCamerasComponent import_cameras_component(&app);
  app.addComponent(&import_cameras_component);

  CamerasComponent cameras_component(&app);
  app.addComponent(&cameras_component);

  ExportOrientationsComponent export_orientations_component(&app);
  app.addComponent(&export_orientations_component);

  ExportPointCloudComponent export_point_cloud_component(&app);
  app.addComponent(&export_point_cloud_component);

  ImageLoaderComponent image_loader_component(&app);
  app.addComponent(&image_loader_component);

  FeatureExtractorComponent feature_extractor_component(&app);
  app.addComponent(&feature_extractor_component);

  FeatureMatchingComponent feature_matching_component(&app);
  app.addComponent(&feature_matching_component);

  OrientationComponent orientation_component(&app);
  app.addComponent(&orientation_component);

  DensificationComponent densification_component(&app);
  app.addComponent(&densification_component);

  GeoreferenceComponent georeference_component(&app);
  app.addComponent(&georeference_component);

  DTMComponent dtm_component(&app);
  app.addComponent(&dtm_component);

  OrthophotoComponent orthophoto_component(&app);
  app.addComponent(&orthophoto_component);

  FeaturesViewerComponent features_viewer_component(&app);
  app.addComponent(&features_viewer_component);

  MatchViewerComponent match_viewer_component(&app);
  app.addComponent(&match_viewer_component);

  SettingsComponent settings_component(&app);
  app.addComponent(&settings_component);

  AboutComponent about_component(&app);
  app.addComponent(&about_component);

  tl::Console &console = tl::Console::instance();
  console.setMessageLevel(tl::MessageLevel::msg_verbose);
  console.setTitle("Graphos");
  app.messageManager()->addListener(&console);

  bool r;

  tl::CommandList::Status status = app.parse(argc, argv);
  if (status == tl::CommandList::Status::parse_success) {

    r = app.runCommand();

  } else if (status == tl::CommandList::Status::parse_error) {

//    TL_TODO("Añadir como opción")
#if defined WIN32
    HWND hwnd = GetConsoleWindow();
    ShowWindow(hwnd, 0);
#endif

    app.freeMemory();

    /// Load gui

    /* File menu */

    componentsManager.mainWindowView()->setCreateProjectAction(create_project_component.action());
    
    componentsManager.mainWindowView()->setOpenProjectAction(open_project_component.action());

    componentsManager.mainWindowView()->setImportCamerasAction(import_cameras_component.action());

    componentsManager.mainWindowView()->setCamerasToolAction(cameras_component.action());

    componentsManager.registerComponent(&export_orientations_component);

    componentsManager.registerComponent(&export_point_cloud_component);

    /* Workflow menu */

    componentsManager.registerComponent(&image_loader_component,
                                        ComponentsManager::Flags::separator_after);
    
    componentsManager.registerComponent(&feature_extractor_component);
    
    componentsManager.registerComponent(&feature_matching_component);
    
    componentsManager.registerComponent(&orientation_component);
    
    componentsManager.registerComponent(&densification_component);

    /* Tools menu */

    componentsManager.registerComponent(&georeference_component,
                                        ComponentsManager::Flags::separator_before);

    componentsManager.registerComponent(&dtm_component,
                                        ComponentsManager::Flags::separator_before);

    componentsManager.registerComponent(&orthophoto_component,
                                        ComponentsManager::Flags::separator_before);

    componentsManager.registerComponent(&features_viewer_component,
                                        ComponentsManager::Flags::separator_before);
    
    componentsManager.registerComponent(&match_viewer_component,
                                        ComponentsManager::Flags::separator_after);

    componentsManager.registerComponent(&settings_component);

    /* Help menu */

    componentsManager.registerComponent(&about_component,
                                        ComponentsManager::Flags::separator_before);
    /// 

    QObject::connect(&create_project_component, SIGNAL(projectCreated()),
                     componentsManager.mainWindowPresenter(), SLOT(loadProject()));
   
    QObject::connect(componentsManager.mainWindowPresenter(), &MainWindowPresenter::openCreateProjectDialog,
                     create_project_component.action(), &QAction::trigger);
    
    QObject::connect(&open_project_component, SIGNAL(projectLoaded()),
                     componentsManager.mainWindowPresenter(), SLOT(loadProject()));

    QObject::connect(componentsManager.mainWindowPresenter(), &MainWindowPresenter::openProjectDialog,
                     open_project_component.action(), &QAction::trigger);

    QObject::connect(&app, SIGNAL(imageLoaded(QString)),
                     componentsManager.mainWindowPresenter(), SLOT(loadImage(QString)));

    /////TODO: por ahora hasta que refactorice MainWindow
    QObject::connect(&image_loader_component, SIGNAL(imageLoaded(QString)),
                     componentsManager.mainWindowPresenter(), SLOT(loadImage(QString)));
    QObject::connect(&feature_extractor_component, SIGNAL(featuresExtracted(QString)),
                     componentsManager.mainWindowPresenter(), SLOT(loadFeatures(QString)));
    QObject::connect(&feature_extractor_component, SIGNAL(featuresDeleted()),
                     componentsManager.mainWindowPresenter(), SLOT(updateProject()));
    QObject::connect(&feature_matching_component, SIGNAL(matchingFinished()),
                     componentsManager.mainWindowPresenter(), SLOT(loadMatches()));
    QObject::connect(&feature_matching_component, SIGNAL(matchesDeleted()),
                     componentsManager.mainWindowPresenter(), SLOT(updateProject()));
    QObject::connect(&orientation_component, SIGNAL(orientationFinished()),
                     componentsManager.mainWindowPresenter(), SLOT(loadOrientation()));
    QObject::connect(&orientation_component, SIGNAL(orientationDeleted()),
                     componentsManager.mainWindowPresenter(), SLOT(updateProject()));
    QObject::connect(&densification_component, SIGNAL(densificationFinished()),
                     componentsManager.mainWindowPresenter(), SLOT(loadDenseModel()));
    QObject::connect(&dtm_component, SIGNAL(finished()),
                     componentsManager.mainWindowPresenter(), SLOT(loadDTM()));
    QObject::connect(&orthophoto_component, SIGNAL(finished()),
                     componentsManager.mainWindowPresenter(), SLOT(loadOrtho()));
    QObject::connect(componentsManager.mainWindowView(), &MainWindowView::openKeypointsViewer,
                     &features_viewer_component, &FeaturesViewerComponent::openKeypointsViewer);
    QObject::connect(componentsManager.mainWindowView(), &MainWindowView::openMatchesViewer,
                     &match_viewer_component, &MatchViewerComponent::openMatchesViewer);
    QObject::connect(&georeference_component, SIGNAL(georeferenceFinished()),
                     componentsManager.mainWindowPresenter(), SLOT(loadOrientation()));

    componentsManager.loadPlugins();

    app.status()->activeFlag(AppStatus::Flag::none, true);

    componentsManager.mainWindowPresenter()->open();

    r = a.exec();

#if defined WIN32
    ShowWindow(hwnd, 1);
#endif

  } else if (status == tl::CommandList::Status::show_help) {
    r = false;
  } else if (status == tl::CommandList::Status::show_licence) {
    r = false;
  } else if (status == tl::CommandList::Status::show_version) {
    r = false;
  }

#ifdef HAVE_VLD
  // Clean up memory allocated by flags.  This is only needed to reduce
  // the quantity of "potentially leaked" reports emitted by memory
  // debugging tools such as valgrind. 
  google::ShutDownCommandLineFlags();
#endif

  return r;
}
