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

#include "ComponentsManager.h"

#include "graphos/core/Component.h"
#include "graphos/core/AppStatus.h"
#include "graphos/components/MainWindowModel.h"
#include "graphos/components/MainWindowView.h"
#include "graphos/components/MainWindowPresenter.h"
#include "graphos/components/cameras/CamerasComponent.h"
#include "graphos/components/HelpDialog.h"
#include "graphos/core/process/Progress.h"
#include "graphos/components/utils/ProgressDialog.h"
#include "graphos/interfaces/Plugin.h"

#include <tidop/core/console.h>

#include <QProgressBar>
#include <QAction>
#include <QPluginLoader>
#include <QDir>
#include <QCoreApplication>
#include <QMenu>

namespace graphos
{

ComponentsManager::ComponentsManager(QObject *parent)
  : QObject(parent),
    mMainWindowView(nullptr),
    mMainWindowModel(nullptr),
    mMainWindowPresenter(nullptr),
    mHelpDialog(nullptr),
    mProgressHandler(nullptr),
    mProgressDialog(nullptr)
{
  this->mainWindowPresenter(); /// Esto ya no hace falta llamarlo aqui
  
  
}

ComponentsManager::~ComponentsManager()
{
  if (mMainWindowView) {
    delete mMainWindowView;
    mMainWindowView = nullptr;
  }
  
  if (mMainWindowModel) {
    delete mMainWindowModel;
    mMainWindowModel = nullptr;
  }

  if (mMainWindowPresenter){
    delete mMainWindowPresenter;
    mMainWindowPresenter = nullptr;
  }

  if (mProgressHandler){
    delete mProgressHandler;
    mProgressHandler = nullptr;
  }

  if (mProgressDialog){
    delete mProgressDialog;
    mProgressDialog = nullptr;
  }

}

void ComponentsManager::openApp()
{
  this->mainWindowView()->showMaximized();
}

MainWindowView *ComponentsManager::mainWindowView()
{
  if (mMainWindowView == nullptr){
    mMainWindowView = new MainWindowView;
  }
  return mMainWindowView;
}

MainWindowModel *ComponentsManager::mainWindowModel()
{
  if (mMainWindowModel == nullptr){
    mMainWindowModel = new MainWindowModel(Application::instance().project());
  }
  return mMainWindowModel;
}

MainWindowPresenter *ComponentsManager::mainWindowPresenter()
{
  if (mMainWindowPresenter == nullptr){
    mMainWindowPresenter = new MainWindowPresenter(this->mainWindowView(),
                                                   this->mainWindowModel());

  }

  return mMainWindowPresenter;
}

void ComponentsManager::registerComponent(Component *component, 
                                          Flags flags)
{
  tl::EnumFlags<Flags> register_flags(flags);

  if (component == nullptr) return;

  QAction *action = component->action();

  QString menu = component->menu();
  MainWindowView::Menu app_menu;
  if (menu.compare("file") == 0) {
    app_menu = MainWindowView::Menu::file;
  } else if (menu.compare("file_export") == 0) {
    app_menu = MainWindowView::Menu::file_export;
  } else if (menu.compare("view") == 0) {
    app_menu = MainWindowView::Menu::view;
  } else if (menu.compare("workflow") == 0) {
    app_menu = MainWindowView::Menu::workflow;
  } else if (menu.compare("tools") == 0) {
    app_menu = MainWindowView::Menu::tools;
  } else if (menu.compare("plugins") == 0) {
    app_menu = MainWindowView::Menu::plugins;
  } else if (menu.compare("help") == 0) {
    app_menu = MainWindowView::Menu::help;
  } else {
    return;
  }

  if (register_flags.isActive(Flags::separator_before)) {
    mMainWindowView->addSeparatorToMenu(app_menu);
  }
  mMainWindowView->addActionToMenu(action, app_menu);
  if (register_flags.isActive(Flags::separator_after)) {
    mMainWindowView->addSeparatorToMenu(app_menu);
  }

  if (ProcessComponent *process_component = dynamic_cast<ProcessComponent *>(component)) {
    process_component->setProgressHandler(this->progressHandler());
  }
  
  QString toolbar = component->toolbar();
  if (!toolbar.isEmpty()) {
    MainWindowView::Toolbar app_toolbar;
    if (toolbar.compare("file") == 0) {
      app_toolbar = MainWindowView::Toolbar::file;
    } else if (toolbar.compare("view") == 0) {
      app_toolbar = MainWindowView::Toolbar::view;
    } else if (toolbar.compare("workflow") == 0) {
      app_toolbar = MainWindowView::Toolbar::workflow;
    } else if (toolbar.compare("model3d") == 0) {
      app_toolbar = MainWindowView::Toolbar::model3d;
    } else if (toolbar.compare("tools") == 0) {
      app_toolbar = MainWindowView::Toolbar::tools;
    }

    if (register_flags.isActive(Flags::separator_before)) {
      mMainWindowView->addSeparatorToToolbar(app_toolbar);
    }

    mMainWindowView->addActionToToolbar(action, app_toolbar);

    if (register_flags.isActive(Flags::separator_after)) {
      mMainWindowView->addSeparatorToToolbar(app_toolbar);
    }
  }

}

void ComponentsManager::registerMultiComponent(const QString &name,
                                               const QString &menu,
                                               const QString &toolbar,
                                               const std::list<std::shared_ptr<graphos::Component>> &components,
                                               Flags)
{

  if (components.empty()) return;

  QMenu *_menu = new QMenu(name, this->mainWindowView());

  MainWindowView::Menu app_menu;
  if (menu.compare("file") == 0) {
    app_menu = MainWindowView::Menu::file;
  } else if (menu.compare("file_export") == 0) {
    app_menu = MainWindowView::Menu::file_export;
  } else if (menu.compare("view") == 0) {
    app_menu = MainWindowView::Menu::view;
  } else if (menu.compare("workflow") == 0) {
    app_menu = MainWindowView::Menu::workflow;
  } else if (menu.compare("tools") == 0) {
    app_menu = MainWindowView::Menu::tools;
  } else if (menu.compare("plugins") == 0) {
    app_menu = MainWindowView::Menu::plugins;
  } else if (menu.compare("help") == 0) {
    app_menu = MainWindowView::Menu::help;
  } else {
    return;
  }

  for (auto component : components) {
    _menu->addAction(component->action());
    if (ProcessComponent *process_component = dynamic_cast<ProcessComponent *>(component.get())) {
      process_component->setProgressHandler(this->progressHandler());
    }
  }

  mMainWindowView->addMenuToMenu(_menu, app_menu);


  MainWindowView::Toolbar app_toolbar;
  if (toolbar.compare("file") == 0) {
    app_toolbar = MainWindowView::Toolbar::file;
  } else if (toolbar.compare("view") == 0) {
    app_toolbar = MainWindowView::Toolbar::view;
  } else if (toolbar.compare("workflow") == 0) {
    app_toolbar = MainWindowView::Toolbar::workflow;
  } else if (toolbar.compare("model3d") == 0) {
    app_toolbar = MainWindowView::Toolbar::model3d;
  } else if (toolbar.compare("tools") == 0) {
    app_toolbar = MainWindowView::Toolbar::tools;
  } else return;

}

void ComponentsManager::loadPlugins()
{
//#ifdef _DEBUG
//  QDir pluginsDir = QDir(QCoreApplication::applicationDirPath());
//#else
  QDir pluginsDir = QDir(QCoreApplication::applicationDirPath() + "/plugins");
//#endif // _DEBUG

  msgInfo("Load Plugins from: %s", pluginsDir.path().toStdString().c_str());

  const auto entryList = pluginsDir.entryList(QDir::Files);
  for (const QString &plugin : entryList) {

    QPluginLoader loader(pluginsDir.absoluteFilePath(plugin));
    if (loader.load()) {
      loadPlugin(loader.instance());
    } else {
      msgWarning(loader.errorString().toStdString().c_str());
    }
  }
}

void ComponentsManager::loadPlugin(QObject *plugin)
{
  try {

    if (plugin) {

      if (auto plugin_component = qobject_cast<GraphosPluginComponent *>(plugin)) {
        QString name = plugin_component->name();
        QString description = plugin_component->description();
        plugin_component->setApp(&Application::instance());
        msgInfo("Plugin loaded: ");
        msgInfo(" - Name: %s", name.toStdString().c_str());
        msgInfo(" - Description: %s", description.toStdString().c_str());
        registerComponent(plugin_component->component());
      //} else if (auto plugin_multi_component = qobject_cast<CVStudioPluginMultiComponent *>(plugin)) {
      //  QString name = plugin_multi_component->name();
      //  QString description = plugin_multi_component->description();
      //  plugin_multi_component->setApp(&Application::instance());
      //  //registerComponent(plugin_component->component());
      } else if (auto plugin_multi_component = qobject_cast<GraphosPluginMultiComponent *>(plugin)) {
        QString name = plugin_multi_component->name();
        QString description = plugin_multi_component->description();
        msgInfo("Plugin loaded: ");
        msgInfo(" - Name: %s", name.toStdString().c_str());
        msgInfo(" - Description: %s", description.toStdString().c_str());
        plugin_multi_component->setApp(&Application::instance());
        QString menu = plugin_multi_component->menu();
        QString toolbar = plugin_multi_component->toolbar();
        registerMultiComponent(name, menu, toolbar, plugin_multi_component->components());
      }

    }

  } catch (const std::exception &e) {
    msgError("Could not load plugin: %s", e.what());
  }
}

HelpDialog *ComponentsManager::helpDialog()
{
  if (mHelpDialog == nullptr) {
    mHelpDialog = new HelpDialog(this->mainWindowView());
    mHelpDialog->setModal(true);
  }
  return mHelpDialog;
}

ProgressHandler *ComponentsManager::progressHandler()
{
  if (mProgressHandler == nullptr){

    mProgressHandler = new ProgressHandler;

    connect(mProgressHandler, SIGNAL(rangeChange(int, int)),      this->progressDialog(), SLOT(setRange(int, int)));
    connect(mProgressHandler, SIGNAL(valueChange(int)),           this->progressDialog(), SLOT(setValue(int)));
    connect(mProgressHandler, SIGNAL(initialized()),              this->progressDialog(), SLOT(setInitialized()));
    connect(mProgressHandler, SIGNAL(finished()),                 this->progressDialog(), SLOT(setFinished()));
    connect(mProgressHandler, SIGNAL(titleChange(QString)),       this->progressDialog(), SLOT(setWindowTitle(QString)));
    connect(mProgressHandler, SIGNAL(descriptionChange(QString)), this->progressDialog(), SLOT(setStatusText(QString)));
    connect(mProgressHandler, SIGNAL(closeAuto(bool)),            this->progressDialog(), SLOT(setCloseAuto(bool)));

    connect(this->progressDialog(), SIGNAL(cancel()),             mProgressHandler, SIGNAL(cancel()));
    QProgressBar *statusBarProgress = this->mainWindowView()->progressBar();

    connect(mProgressHandler, SIGNAL(rangeChange(int, int)),      statusBarProgress, SLOT(setRange(int, int)));
    connect(mProgressHandler, SIGNAL(valueChange(int)),           statusBarProgress, SLOT(setValue(int)));
    connect(mProgressHandler, SIGNAL(initialized()),              statusBarProgress, SLOT(show()));
    connect(mProgressHandler, SIGNAL(finished()),                 statusBarProgress, SLOT(hide()));

  }

  return mProgressHandler;
}

ProgressDialog *ComponentsManager::progressDialog()
{
  if (mProgressDialog == nullptr){
    mProgressDialog = new ProgressDialogImp;
  }

  return mProgressDialog;
}

} // namespace graphos
