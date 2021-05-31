#include "MainWindowView.h"
#include "MainWindowModel.h"
#include "MainWindowPresenter.h"
#include "ComponentsManager.h"

//#include "inspector/ui/createproject/CreateProjectComponent.h"
//#include "inspector/ui/openproject/OpenProjectComponent.h"

#include "inspector/ui/images/ImageLoaderComponent.h"
#include "inspector/ui/featextract/FeatureExtractorComponent.h"
#include "inspector/ui/featmatch/FeatureMatchingComponent.h"
#include "inspector/ui/orientation/OrientationComponent.h"
#include "inspector/ui/densification/DensificationComponent.h"
#include "inspector/ui/dtm/DTMComponent.h"
#include "inspector/ui/georeference/GeoreferenceComponent.h"
#include "inspector/ui/featviewer/FeaturesViewerComponent.h"
#include "inspector/ui/matchviewer/MatchViewerComponent.h"
#include "inspector/ui/AppStatus.h"

#include <QApplication>

#ifdef HAVE_VLD
#include "gflags/gflags.h"
#include "vld.h"
#endif

using namespace inspector;
using namespace inspector::ui;

int main(int argc, char *argv[])
{
  QApplication a(argc, argv);

  ComponentsManager componentsManager;

  //CreateProjectComponent create_project_component(componentsManager.project());
  //componentsManager.registerComponent(&create_project_component);
  //OpenProjectComponent open_project_component(componentsManager.project());
  //componentsManager.registerComponent(&open_project_component);

  /* Workflow menu */

  ImageLoaderComponent image_loader_component(componentsManager.project());
  componentsManager.registerComponent(&image_loader_component, 
                                      ComponentsManager::Flags::separator_after);
  FeatureExtractorComponent feature_extractor_component(componentsManager.project());
  componentsManager.registerComponent(&feature_extractor_component);
  FeatureMatchingComponent feature_matching_component(componentsManager.project());
  componentsManager.registerComponent(&feature_matching_component);
  OrientationComponent orientation_component(componentsManager.project());
  componentsManager.registerComponent(&orientation_component);
  DensificationComponent densification_component(componentsManager.project());
  componentsManager.registerComponent(&densification_component);
  
  /* Tools menu */

  GeoreferenceComponent georeference_component(componentsManager.project());
  componentsManager.registerComponent(&georeference_component,
                                      ComponentsManager::Flags::separator_before);
  DTMComponent dtm_component(componentsManager.project());
  componentsManager.registerComponent(&dtm_component, 
                                      ComponentsManager::Flags::separator_before);
  FeaturesViewerComponent features_viewer_component(componentsManager.project());
  componentsManager.registerComponent(&features_viewer_component, 
                                      ComponentsManager::Flags::separator_before);
  MatchViewerComponent match_viewer_component(componentsManager.project());
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

  AppStatus &app_status = AppStatus::instance();
  app_status.activeFlag(AppStatus::Flag::none, true);

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
