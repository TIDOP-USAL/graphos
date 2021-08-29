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

#include "ComponentsManager.h"

#include "graphos/core/Component.h"
#include "graphos/core/AppStatus.h"
#include "graphos/ui/MainWindowModel.h"
#include "graphos/ui/MainWindowView.h"
#include "graphos/ui/MainWindowPresenter.h"
#include "graphos/ui/createproject/CreateProjectComponent.h"
#include "graphos/ui/openproject/OpenProjectComponent.h"
#include "graphos/ui/import/cameras/ImportCamerasComponent.h"
#include "graphos/ui/cameras/CamerasComponent.h"
#include "graphos/ui/ProjectModel.h"
#include "graphos/ui/SettingsModel.h"
#include "graphos/ui/SettingsView.h"
#include "graphos/ui/SettingsPresenter.h"
//#include "graphos/ui/export/densemodel/impl/ExportPointCloudModel.h"
//#include "graphos/ui/export/densemodel/impl/ExportPointCloudPresenter.h"
//#include "graphos/ui/export/densemodel/impl/ExportPointCloudView.h"
#include "graphos/ui/FeaturesModel.h"
#include "graphos/ui/MatchesModel.h"
#include "graphos/ui/HelpDialog.h"
#include "graphos/core/process/Progress.h"
#include "graphos/ui/utils/ProgressDialog.h"
#include "graphos/interfaces/Plugin.h"

#include <QProgressBar>
#include <QAction>
#include <QPluginLoader>
#include <QDir>
#include <QCoreApplication>


namespace graphos
{

ComponentsManager::ComponentsManager(QObject *parent)
  : QObject(parent),
    mMainWindowView(nullptr),
    mMainWindowModel(nullptr),
    mMainWindowPresenter(nullptr),
    mProject(new ProjectImp),
    mProjectModel(nullptr),
    mFeaturesModel(nullptr),
    mMatchesModel(nullptr),
    mSettings(new SettingsImp),
    mSettingsController(new SettingsControllerImp),
    mSettingsModel(nullptr),
    mSettingsPresenter(nullptr),
    //mExportPointCloudModel(nullptr),
    //mExportPointCloudPresenter(nullptr),
    mHelpDialog(nullptr),
    mProgressHandler(nullptr),
    mProgressDialog(nullptr),
    mCreateProjectComponent(nullptr),
    mOpenProjectComponent(nullptr),
    mImportCamerasComponent(nullptr)
{
  this->mainWindowPresenter();
  
  Application &app = Application::instance();

  mCreateProjectComponent = new CreateProjectComponent(this->project(), &app);
  this->mainWindowView()->setCreateProjectAction(mCreateProjectComponent->action());
  mOpenProjectComponent = new OpenProjectComponent(this->project(), &app);
  this->mainWindowView()->setOpenProjectAction(mOpenProjectComponent->action());
  mImportCamerasComponent = new ImportCamerasComponent(this->project(), &app);
  this->mainWindowView()->setImportCamerasAction(mImportCamerasComponent->action());
  mCamerasComponent = new CamerasComponent(this->project(), &app);
  this->mainWindowView()->setCamerasToolAction(mCamerasComponent->action());

  ///TODO: por ahora hasta que refactorice MainWindowView, MainWindowPresenter
  connect(mCreateProjectComponent, SIGNAL(projectCreated()), 
          this->mainWindowPresenter(), SLOT(loadProject()));
  connect(mOpenProjectComponent, SIGNAL(projectLoaded()), 
          this->mainWindowPresenter(), SLOT(loadProject()));
  connect(this->mainWindowPresenter(), &MainWindowPresenter::openCreateProjectDialog, 
          mCreateProjectComponent->action(), &QAction::trigger);
  connect(this->mainWindowPresenter(), &MainWindowPresenter::openProjectDialog, 
          mOpenProjectComponent->action() , &QAction::trigger);

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

  if (mProject) {
    delete mProject;
    mProject = nullptr;
  }

  if (mProjectModel){
    delete mProjectModel;
    mProjectModel = nullptr;
  }

  if (mFeaturesModel){
    delete mFeaturesModel;
    mFeaturesModel = nullptr;
  }

  if (mMatchesModel){
    delete mMatchesModel;
    mMatchesModel = nullptr;
  }
  if (mSettings){
    delete mSettings;
    mSettings = nullptr;
  }

  if (mSettingsController){
    delete mSettingsController;
    mSettingsController = nullptr;
  }

  if (mSettingsModel){
    delete mSettingsModel;
    mSettingsModel = nullptr;
  }

  if (mSettingsPresenter){
    delete mSettingsPresenter;
    mSettingsPresenter = nullptr;
  }

  //if (mExportPointCloudModel){
  //  delete mExportPointCloudModel;
  //  mExportPointCloudModel = nullptr;
  //}

  //if (mExportPointCloudPresenter){
  //  delete mExportPointCloudPresenter;
  //  mExportPointCloudPresenter = nullptr;
  //}

  if (mProgressHandler){
    delete mProgressHandler;
    mProgressHandler = nullptr;
  }

  if (mProgressDialog){
    delete mProgressDialog;
    mProgressDialog = nullptr;
  }

  if (mCreateProjectComponent) {
    delete mCreateProjectComponent;
    mCreateProjectComponent = nullptr;
  }

  if (mOpenProjectComponent) {
    delete mOpenProjectComponent;
    mOpenProjectComponent = nullptr;
  }
   
  if (mImportCamerasComponent) {
    delete mImportCamerasComponent;
    mImportCamerasComponent = nullptr;
  }

  if (mCamerasComponent) {
    delete mCamerasComponent;
    mCamerasComponent = nullptr;
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
    mMainWindowModel = new MainWindowModel(this->project());
  }
  return mMainWindowModel;
}

MainWindowPresenter *ComponentsManager::mainWindowPresenter()
{
  if (mMainWindowPresenter == nullptr){
    mMainWindowPresenter = new MainWindowPresenter(this->mainWindowView(),
                                                   this->mainWindowModel(),
                                                   this->projectModel(),
                                                   this->settingsModel(),
                                                   this->featuresModel(),
                                                   this->matchesModel());


    connect(mMainWindowPresenter, &MainWindowPresenter::openSettingsDialog,
            this, &ComponentsManager::initAndOpenSettingsDialog);
    connect(mMainWindowPresenter, &MainWindowPresenter::openViewSettingsDialog,
            this, &ComponentsManager::initAndOpenViewSettingsDialog);
    connect(mMainWindowPresenter, &MainWindowPresenter::openToolSettingsDialog,
            this, &ComponentsManager::initAndOpenToolSettingsDialog);
    //connect(this->mainWindowPresenter(), &MainWindowPresenter::openExportPointCloudDialog,
    //        this, &ComponentsManager::initAndOpenExportPointCloudDialog);
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

  if (register_flags.isActive(Flags::separator_before)) {
    mMainWindowView->addSeparatorToToolbar(app_toolbar);
  }
  mMainWindowView->addActionToToolbar(action, app_toolbar);
  if (register_flags.isActive(Flags::separator_after)) {
    mMainWindowView->addSeparatorToToolbar(app_toolbar);
  }
}

void ComponentsManager::loadPlugins()
{
  auto pluginsDir = QDir(QCoreApplication::applicationDirPath());
  const auto entryList = pluginsDir.entryList(QDir::Files);
  for (const QString &fileName : entryList) {

    QPluginLoader loader(pluginsDir.absoluteFilePath(fileName));
    loadPlugin(loader.instance());

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
        registerComponent(plugin_component->component());
      //} else if (auto plugin_multi_component = qobject_cast<CVStudioPluginMultiComponent *>(plugin)) {
      //  QString name = plugin_multi_component->name();
      //  QString description = plugin_multi_component->description();
      //  plugin_multi_component->setApp(&Application::instance());
      //  //registerComponent(plugin_component->component());
      }

    }

  } catch (const std::exception &e) {
    msgError("Could not load plugin: %s", e.what());
  }
}

Project *ComponentsManager::project()
{
  return mProject;
}

ProjectModel *ComponentsManager::projectModel()
{
  if (mProjectModel == nullptr){
    mProjectModel = new ProjectModelImp(mProject);
  }
  return mProjectModel;
}

FeaturesModel *ComponentsManager::featuresModel()
{
  if (mFeaturesModel == nullptr){
    mFeaturesModel = new FeaturesModelImp(mProject);
  }
  return mFeaturesModel;
}

MatchesModel *ComponentsManager::matchesModel()
{
  if (mMatchesModel == nullptr){
    mMatchesModel = new MatchesModelImp(mProject);
  }
  return mMatchesModel;
}

SettingsModel *ComponentsManager::settingsModel()
{
  if (mSettingsModel == nullptr){
    mSettingsModel = new SettingsModelImp(mSettings, mSettingsController);
    mSettingsModel->read();
  }
  return mSettingsModel;
}

SettingsPresenter *ComponentsManager::settingsPresenter()
{
  if (mSettingsPresenter == nullptr){
    SettingsView *view = new SettingsViewImp(this->mainWindowView());
    mSettingsPresenter = new SettingsPresenterImp(view, this->settingsModel());
  }
  return mSettingsPresenter;
}

//ExportPointCloudModel *ComponentsManager::exportPointCloudModel()
//{
//  if (mExportPointCloudModel == nullptr) {
//    mExportPointCloudModel = new ExportPointCloudModelImp(mProject);
//  }
//  return mExportPointCloudModel;
//}
//
//ExportPointCloudPresenter *ComponentsManager::exportPointCloudPresenter()
//{
//  if (mExportPointCloudPresenter == nullptr){
//    ExportPointCloudView *view = new ExportPointCloudViewImp(this->mainWindowView());
//    mExportPointCloudPresenter = new ExportPointCloudPresenterImp(view,
//                                                                  this->exportPointCloudModel());
//  }
//  return mExportPointCloudPresenter;
//}

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

void ComponentsManager::initAndOpenSettingsDialog()
{
  this->initSettingsDialog();
  this->settingsPresenter()->open();
}

void ComponentsManager::initAndOpenViewSettingsDialog()
{
  this->initSettingsDialog();
  this->settingsPresenter()->openViewSettings();
}

void ComponentsManager::initAndOpenToolSettingsDialog()
{
  this->initSettingsDialog();
  this->settingsPresenter()->openToolSettings();
}

void ComponentsManager::initSettingsDialog()
{
  disconnect(this->mainWindowPresenter(), &MainWindowPresenter::openSettingsDialog,
    this, &ComponentsManager::initAndOpenSettingsDialog);
  disconnect(mMainWindowPresenter, &MainWindowPresenter::openViewSettingsDialog,
    this, &ComponentsManager::initAndOpenViewSettingsDialog);
  disconnect(mMainWindowPresenter, &MainWindowPresenter::openToolSettingsDialog,
    this, &ComponentsManager::initAndOpenToolSettingsDialog);

  connect(this->mainWindowPresenter(), SIGNAL(openSettingsDialog()), this->settingsPresenter(), SLOT(open()));
  connect(this->mainWindowPresenter(), SIGNAL(openViewSettingsDialog()), this->settingsPresenter(), SLOT(openViewSettings()));
  connect(this->mainWindowPresenter(), SIGNAL(openQualityControlSettingsDialog()), this->settingsPresenter(), SLOT(openQualityControlSettings()));
  connect(this->mainWindowPresenter(), SIGNAL(openToolSettingsDialog()), this->settingsPresenter(), SLOT(openToolSettings()));

  this->settingsPresenter()->setHelp(this->helpDialog());
}

//void ComponentsManager::initAndOpenExportPointCloudDialog()
//{
//  disconnect(this->mainWindowPresenter(), &MainWindowPresenter::openExportPointCloudDialog,
//             this, &ComponentsManager::initAndOpenExportPointCloudDialog);
//  connect(this->mainWindowPresenter(), &MainWindowPresenter::openExportPointCloudDialog,
//          this->exportPointCloudPresenter(), &ExportPointCloudPresenter::open);
//
//  this->exportPointCloudPresenter()->setHelp(this->helpDialog());
//  this->exportPointCloudPresenter()->open();
//}

} // namespace graphos
