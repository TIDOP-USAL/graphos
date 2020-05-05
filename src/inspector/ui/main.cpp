#include "MainWindowView.h"
#include "MainWindowModel.h"
#include "MainWindowPresenter.h"
#include "ComponentsManager.h"

#include <QApplication>

using namespace inspector;
using namespace inspector::ui;

int main(int argc, char *argv[])
{
  QApplication a(argc, argv);

  ComponentsManager componentsManager;
  componentsManager.mainWindowPresenter()->open();

  bool r = a.exec();

  return r;
}
