#include "MainWindowView.h"
#include "MainWindowModel.h"
#include "MainWindowPresenter.h"
#include "ComponentsManager.h"

#include "inspector/ui/images/ImageLoaderComponent.h"
#include "inspector/ui/featextract/FeatureExtractorComponent.h"
#include "inspector/ui/featmatch/FeatureMatchingComponent.h"
#include "inspector/ui/orientation/OrientationComponent.h"
#include "inspector/ui/densification/DensificationComponent.h"
#include "inspector/ui/dtm/DTMComponent.h"
#include "inspector/ui/featviewer/FeaturesViewerComponent.h"
#include "inspector/ui/matchviewer/MatchViewerComponent.h"
#include "inspector/ui/AppStatus.h"

#include <QApplication>

using namespace inspector;
using namespace inspector::ui;

int main(int argc, char *argv[])
{
  QApplication a(argc, argv);

  ComponentsManager componentsManager;

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
  /*DtmComponent dtm_component(componentsManager.project());
  componentsManager.registerComponent(&dtm_component, 
                                      ComponentsManager::Flags::separator_after | 
                                      ComponentsManager::Flags::separator_before);*/
  DTMComponent dtm_component(componentsManager.project());
  componentsManager.registerComponent(&dtm_component, 
                                      ComponentsManager::Flags::separator_before);
  FeaturesViewerComponent features_viewer_component(componentsManager.project());
  componentsManager.registerComponent(&features_viewer_component, 
                                      ComponentsManager::Flags::separator_before);
  MatchViewerComponent match_viewer_component(componentsManager.project());
  componentsManager.registerComponent(&match_viewer_component,
                                      ComponentsManager::Flags::separator_after);

  QObject::connect(&image_loader_component, SIGNAL(imageLoaded(QString)), 
                   componentsManager.mainWindowPresenter(), SLOT(loadImage(QString)));

  AppStatus &app_status = AppStatus::instance();
  app_status.activeFlag(AppStatus::Flag::none, true);


  componentsManager.mainWindowPresenter()->open();

  bool r = a.exec();

  return r;
}
