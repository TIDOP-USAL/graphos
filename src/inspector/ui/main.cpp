#include "MainWindowView.h"
#include "MainWindowModel.h"
#include "MainWindowPresenter.h"
#include "ComponentsManager.h"

#include "inspector/ui/dtm/DTMComponent.h"
#include "inspector/ui/AppStatus.h"

#include <QApplication>

using namespace inspector;
using namespace inspector::ui;

int main(int argc, char *argv[])
{
  QApplication a(argc, argv);

  ComponentsManager componentsManager;

  /*DtmComponent dtm_component(componentsManager.project());
  componentsManager.registerComponent(&dtm_component, 
                                      ComponentsManager::Flags::separator_after | 
                                      ComponentsManager::Flags::separator_before);*/
  DTMComponent dtm_component(componentsManager.project());
  componentsManager.registerComponent(&dtm_component, 
                                      ComponentsManager::Flags::separator_after | 
                                      ComponentsManager::Flags::separator_before);

  AppStatus &app_status = AppStatus::instance();
  app_status.activeFlag(AppStatus::Flag::none, true);

  componentsManager.mainWindowPresenter()->open();

  bool r = a.exec();

  return r;
}
