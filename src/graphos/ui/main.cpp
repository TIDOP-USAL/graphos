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

//#include "graphos/ui/createproject/CreateProjectComponent.h"
//#include "graphos/ui/openproject/OpenProjectComponent.h"

#include "graphos/ui/images/ImageLoaderComponent.h"
#include "graphos/ui/featextract/FeatureExtractorComponent.h"
#include "graphos/ui/featmatch/FeatureMatchingComponent.h"
#include "graphos/ui/orientation/OrientationComponent.h"
#include "graphos/ui/densification/DensificationComponent.h"
#include "graphos/ui/dtm/DTMComponent.h"
#include "graphos/ui/orthophoto/OrthophotoComponent.h"
#include "graphos/ui/georeference/GeoreferenceComponent.h"
#include "graphos/ui/featviewer/FeaturesViewerComponent.h"
#include "graphos/ui/matchviewer/MatchViewerComponent.h"
#include "graphos/ui/export/orientations/ExportOrientationsComponent.h"
#include "graphos/ui/export/densemodel/ExportPointCloudComponent.h"
#include "graphos/core/Application.h"
#include "graphos/core/AppStatus.h"

#include <QApplication>

#ifdef HAVE_VLD
#include "gflags/gflags.h"
#include "vld.h"
#endif

using namespace graphos;

int main(int argc, char *argv[])
{
  QApplication a(argc, argv);

  Application &app = Application::instance();

  ComponentsManager componentsManager;

  //CreateProjectComponent create_project_component(componentsManager.project());
  //componentsManager.registerComponent(&create_project_component);
  //OpenProjectComponent open_project_component(componentsManager.project());
  //componentsManager.registerComponent(&open_project_component);

  /* File menu */

  ExportOrientationsComponent export_orientations_component(componentsManager.project(), &app);
  componentsManager.registerComponent(&export_orientations_component);
  ExportPointCloudComponent export_point_cloud_component(componentsManager.project(), &app);
  componentsManager.registerComponent(&export_point_cloud_component);

  /* Workflow menu */

  ImageLoaderComponent image_loader_component(componentsManager.project(), &app);
  componentsManager.registerComponent(&image_loader_component, 
                                      ComponentsManager::Flags::separator_after);
  FeatureExtractorComponent feature_extractor_component(componentsManager.project(), &app);
  componentsManager.registerComponent(&feature_extractor_component);
  FeatureMatchingComponent feature_matching_component(componentsManager.project(), &app);
  componentsManager.registerComponent(&feature_matching_component);
  OrientationComponent orientation_component(componentsManager.project(), &app);
  componentsManager.registerComponent(&orientation_component);
  DensificationComponent densification_component(componentsManager.project(), &app);
  componentsManager.registerComponent(&densification_component);
  
  /* Tools menu */

  GeoreferenceComponent georeference_component(componentsManager.project(), &app);
  componentsManager.registerComponent(&georeference_component,
                                      ComponentsManager::Flags::separator_before);
  DTMComponent dtm_component(componentsManager.project(), &app);
  componentsManager.registerComponent(&dtm_component, 
                                      ComponentsManager::Flags::separator_before);
  OrthophotoComponent orthophoto_component(componentsManager.project(), &app);
  componentsManager.registerComponent(&orthophoto_component,
                                      ComponentsManager::Flags::separator_before);
  FeaturesViewerComponent features_viewer_component(componentsManager.project(), &app);
  componentsManager.registerComponent(&features_viewer_component, 
                                      ComponentsManager::Flags::separator_before);
  MatchViewerComponent match_viewer_component(componentsManager.project(), &app);
  componentsManager.registerComponent(&match_viewer_component,
                                      ComponentsManager::Flags::separator_after);


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
  QObject::connect(componentsManager.mainWindowView(), &MainWindowView::openKeypointsViewer,
                   &features_viewer_component, &FeaturesViewerComponent::openKeypointsViewer);
  QObject::connect(componentsManager.mainWindowView(), &MainWindowView::openMatchesViewer,
                   &match_viewer_component, &MatchViewerComponent::openMatchesViewer);
  QObject::connect(&georeference_component, SIGNAL(georeferenceFinished()), 
                   componentsManager.mainWindowPresenter(), SLOT(loadOrientation()));

  //AppStatus &app_status = AppStatus::instance();
  app.status()->activeFlag(AppStatus::Flag::none, true);

  componentsManager.mainWindowPresenter()->open();

  bool r = a.exec();

#ifdef HAVE_VLD
  // Clean up memory allocated by flags.  This is only needed to reduce
  // the quantity of "potentially leaked" reports emitted by memory
  // debugging tools such as valgrind. 
  google::ShutDownCommandLineFlags();
#endif

  return r;
}
