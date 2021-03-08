#include "ComponentsManager.h"

#include "inspector/ui/Component.h"
#include "inspector/ui/AppStatus.h"

#include "inspector/ui/MainWindowModel.h"
#include "inspector/ui/MainWindowView.h"
#include "inspector/ui/MainWindowPresenter.h"
#include "inspector/ui/ProjectModel.h"
#include "inspector/ui/SettingsModel.h"
#include "inspector/ui/SettingsView.h"
#include "inspector/ui/SettingsPresenter.h"

#include "inspector/ui/NewProjectPresenter.h"
#include "inspector/ui/NewProjectView.h"
#include "inspector/ui/cameras/CamerasModel.h"
#include "inspector/ui/cameras/CamerasView.h"
#include "inspector/ui/cameras/CamerasPresenter.h"
#include "inspector/ui/export/orientations/ExportOrientationsModel.h"
#include "inspector/ui/export/orientations/ExportOrientationsView.h"
#include "inspector/ui/export/orientations/ExportOrientationsPresenter.h"
#include "inspector/ui/georeference/impl/GeoreferenceModel.h"
#include "inspector/ui/georeference/impl/GeoreferenceView.h"
#include "inspector/ui/georeference/impl/GeoreferencePresenter.h"
#include "inspector/ui/import/cameras/impl/CameraPositionsModel.h"
#include "inspector/ui/import/cameras/impl/CameraPositionsView.h"
#include "inspector/ui/import/cameras/impl/CameraPositionsPresenter.h"
#include "inspector/ui/export/densemodel/impl/ExportPointCloudModel.h"
#include "inspector/ui/export/densemodel/impl/ExportPointCloudPresenter.h"
#include "inspector/ui/export/densemodel/impl/ExportPointCloudView.h"
#include "inspector/ui/FeaturesModel.h"
#include "inspector/ui/MatchesModel.h"
#include "inspector/ui/HelpDialog.h"
#include "inspector/ui/utils/Progress.h"
#include "inspector/ui/utils/ProgressDialog.h"

#include <QProgressBar>

namespace inspector
{

namespace ui
{

ComponentsManager::ComponentsManager(QObject *parent)
  : QObject(parent),
    mMainWindowView(nullptr),
    mMainWindowModel(nullptr),
    mMainWindowPresenter(nullptr),
    mProject(new ProjectImp),
    mProjectModel(nullptr),
    mCamerasModel(nullptr),
    mCamerasPresenter(nullptr),
    mFeaturesModel(nullptr),
    mMatchesModel(nullptr),
    mSettings(new SettingsImp),
    mSettingsController(new SettingsControllerImp),
    mSettingsModel(nullptr),
    mSettingsPresenter(nullptr),
    mNewProjectPresenter(nullptr),
    mExportOrientationsModel(nullptr),
    mExportOrientationsPresenter(nullptr),
    mGeoreferenceModel(nullptr),
    mGeoreferencePresenter(nullptr),
    mCamerasImportModel(nullptr),
    mCamerasImportPresenter(nullptr),
    mExportPointCloudModel(nullptr),
    mExportPointCloudPresenter(nullptr),
    mHelpDialog(nullptr),
    mProgressHandler(nullptr),
    mProgressDialog(nullptr)
{
  this->mainWindowPresenter();
}

ComponentsManager::~ComponentsManager()
{
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

  if (mCamerasModel){
    delete mCamerasModel;
    mCamerasModel = nullptr;
  }

  if (mCamerasPresenter){
    delete mCamerasPresenter;
    mCamerasPresenter = nullptr;
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

  if (mNewProjectPresenter) {
    delete mNewProjectPresenter;
    mNewProjectPresenter = nullptr;
  }

  if (mExportOrientationsModel) {
    delete mExportOrientationsModel;
    mExportOrientationsModel = nullptr;
  }

  if (mExportOrientationsPresenter) {
    delete mExportOrientationsPresenter;
    mExportOrientationsPresenter = nullptr;
  }

  if (mGeoreferenceModel) {
    delete mGeoreferenceModel;
    mGeoreferenceModel = nullptr;
  }

  if (mGeoreferencePresenter) {
    delete mGeoreferencePresenter;
    mGeoreferencePresenter = nullptr;
  }

  if (mCamerasImportModel) {
    delete mCamerasImportModel;
    mCamerasImportModel = nullptr;
  }

  if (mCamerasImportPresenter) {
    delete mCamerasImportPresenter;
    mCamerasImportPresenter = nullptr;
  }

  if (mExportPointCloudModel){
    delete mExportPointCloudModel;
    mExportPointCloudModel = nullptr;
  }

  if (mExportPointCloudPresenter){
    delete mExportPointCloudPresenter;
    mExportPointCloudPresenter = nullptr;
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
                                                   //this->imagesModel(),
                                                   this->camerasModel(),
                                                   this->featuresModel(),
                                                   this->matchesModel());

    //connect(mMainWindowPresenter, &MainWindowPresenter::openNewProjectDialog,
    //        this, &ComponentsManager::initAndOpenNewProjectDialog);

    connect(mMainWindowPresenter, &MainWindowPresenter::openExportOrientationsDialog,
            this, &ComponentsManager::initAndOpenExportOrientationsDialog);

    connect(mMainWindowPresenter, &MainWindowPresenter::openCamerasDialog,
             this, &ComponentsManager::initAndOpenCamerasDialog);
    connect(mMainWindowPresenter, &MainWindowPresenter::openSettingsDialog,
            this, &ComponentsManager::initAndOpenSettingsDialog);
    connect(mMainWindowPresenter, &MainWindowPresenter::openViewSettingsDialog,
            this, &ComponentsManager::initAndOpenViewSettingsDialog);
    connect(mMainWindowPresenter, &MainWindowPresenter::openToolSettingsDialog,
            this, &ComponentsManager::initAndOpenToolSettingsDialog);
    connect(this->mainWindowPresenter(), &MainWindowPresenter::openGeoreferenceDialog,
            this, &ComponentsManager::initAndOpenGeoreferenceDialog);
    connect(this->mainWindowPresenter(), &MainWindowPresenter::openCamerasImportDialog,
            this, &ComponentsManager::initAndOpenCamerasImportDialog);
//    connect(mMainWindowPresenter, SIGNAL(openAboutDialog()),            this, SLOT(initAndOpenAboutDialog()));
    connect(this->mainWindowPresenter(), &MainWindowPresenter::openExportPointCloudDialog,
            this, &ComponentsManager::initAndOpenExportPointCloudDialog);
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
  } else if (menu.compare("view") == 0) {
    app_menu = MainWindowView::Menu::view;
  } else if (menu.compare("workflow") == 0) {
    app_menu = MainWindowView::Menu::workflow;
  } else if (menu.compare("tools") == 0) {
    app_menu = MainWindowView::Menu::tools;
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

CamerasModel *ComponentsManager::camerasModel()
{
  if (mCamerasModel == nullptr){
    mCamerasModel = new CamerasModelImp(mProject);
  }
  return mCamerasModel;
}

CamerasPresenter *ComponentsManager::camerasPresenter()
{
  if (mCamerasPresenter == nullptr){
    CamerasView *view = new CamerasViewImp(this->mainWindowView());
    mCamerasPresenter = new CamerasPresenterImp(view,
                                                this->camerasModel());
  }
  return mCamerasPresenter;
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

NewProjectPresenter *ComponentsManager::newProjectPresenter()
{
  if (mNewProjectPresenter == nullptr){
    NewProjectView *newProjectView = new NewProjectViewImp(this->mainWindowView());
    mNewProjectPresenter = new NewProjectPresenterImp(newProjectView, this->projectModel());
  }
  return mNewProjectPresenter;
}

ExportOrientationsModel *ComponentsManager::exportOrientationsModel()
{
  if (mExportOrientationsModel == nullptr) {
    mExportOrientationsModel = new ExportOrientationsModelImp(mProject);
  }
  return mExportOrientationsModel;
}

ExportOrientationsPresenter *ComponentsManager::exportOrientationsPresenter()
{
  if (mExportOrientationsPresenter == nullptr){
    ExportOrientationsView *view = new ExportOrientationsViewImp(this->mainWindowView());
    mExportOrientationsPresenter = new ExportOrientationsPresenterImp(view,
                                                                      this->exportOrientationsModel());
  }
  return mExportOrientationsPresenter;
}

GeoreferenceModel *ComponentsManager::georeferenceModel()
{
  if (mGeoreferenceModel == nullptr) {
    mGeoreferenceModel = new GeoreferenceModelImp(mProject);
  }
  return mGeoreferenceModel;
}

GeoreferencePresenter *ComponentsManager::georeferencePresenter()
{
  if (mGeoreferencePresenter == nullptr){
    Qt::WindowFlags f(Qt::WindowMinMaxButtonsHint | Qt::WindowCloseButtonHint);
    GeoreferenceView *view = new GeoreferenceViewImp(this->mainWindowView(), f);
    mGeoreferencePresenter = new GeoreferencePresenterImp(view,
                                                          this->georeferenceModel());
  }
  return mGeoreferencePresenter;
}

CamerasImportModel *ComponentsManager::camerasImportModel()
{
  if (mCamerasImportModel == nullptr) {
    mCamerasImportModel = new CamerasImportModelImp(mProject);
  }
  return mCamerasImportModel;
}

CamerasImportPresenter *ComponentsManager::camerasImportPresenter()
{
  if (mCamerasImportPresenter == nullptr){
    CamerasImportView *view = new CamerasImportViewImp(this->mainWindowView());
    mCamerasImportPresenter = new CamerasImportPresenterImp(view,
                                                            this->camerasImportModel());
  }
  return mCamerasImportPresenter;
}

ExportPointCloudModel *ComponentsManager::exportPointCloudModel()
{
  if (mExportPointCloudModel == nullptr) {
    mExportPointCloudModel = new ExportPointCloudModelImp(mProject);
  }
  return mExportPointCloudModel;
}

ExportPointCloudPresenter *ComponentsManager::exportPointCloudPresenter()
{
  if (mExportPointCloudPresenter == nullptr){
    ExportPointCloudView *view = new ExportPointCloudViewImp(this->mainWindowView());
    mExportPointCloudPresenter = new ExportPointCloudPresenterImp(view,
                                                                  this->exportPointCloudModel());
  }
  return mExportPointCloudPresenter;
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

//void ComponentsManager::initAndOpenNewProjectDialog()
//{
//  disconnect(this->mainWindowPresenter(), &MainWindowPresenter::openNewProjectDialog,
//             this, &ComponentsManager::initAndOpenNewProjectDialog);
//
//  connect(this->mainWindowPresenter(), &MainWindowPresenter::openNewProjectDialog, 
//          this->newProjectPresenter(), &IPresenter::open);
//  connect(this->newProjectPresenter(), SIGNAL(projectCreate()),
//          this->mainWindowPresenter(), SLOT(loadProject()));
//
//  this->newProjectPresenter()->setHelp(this->helpDialog());
//  this->newProjectPresenter()->open();
//}

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

void ComponentsManager::initAndOpenCamerasDialog()
{
  disconnect(mMainWindowPresenter, &MainWindowPresenter::openCamerasDialog,
             this,                 &ComponentsManager::initAndOpenCamerasDialog);

  connect(this->mainWindowPresenter(), &MainWindowPresenter::openCamerasDialog,
          this->camerasPresenter(),    &CamerasPresenter::open);

  connect(this->camerasPresenter(), SIGNAL(projectModified()), this->mainWindowPresenter(), SLOT(onProjectModified()));


  this->camerasPresenter()->setHelp(this->helpDialog());
  this->camerasPresenter()->open();

}

void ComponentsManager::initSettingsDialog()
{
  disconnect(this->mainWindowPresenter(), &MainWindowPresenter::openSettingsDialog,
    this, &ComponentsManager::initAndOpenSettingsDialog);
  disconnect(mMainWindowPresenter, &MainWindowPresenter::openViewSettingsDialog,
    this, &ComponentsManager::initAndOpenViewSettingsDialog);
  disconnect(mMainWindowPresenter, &MainWindowPresenter::openToolSettingsDialog,
    this, &ComponentsManager::initAndOpenToolSettingsDialog);
  //disconnect(this->mainWindowPresenter(), SIGNAL(openSettingsDialog()), this, SLOT(initAndOpenSettingsDialog()));
  //disconnect(this->mainWindowPresenter(), SIGNAL(openViewSettingsDialog()), this, SLOT(initAndOpenViewSettingsDialog()));
  //disconnect(this->mainWindowPresenter(), SIGNAL(openToolSettingsDialog()), this, SLOT(initAndOpenToolSettingsDialog()));

  connect(this->mainWindowPresenter(), SIGNAL(openSettingsDialog()), this->settingsPresenter(), SLOT(open()));
  connect(this->mainWindowPresenter(), SIGNAL(openViewSettingsDialog()), this->settingsPresenter(), SLOT(openViewSettings()));
  connect(this->mainWindowPresenter(), SIGNAL(openQualityControlSettingsDialog()), this->settingsPresenter(), SLOT(openQualityControlSettings()));
  connect(this->mainWindowPresenter(), SIGNAL(openToolSettingsDialog()), this->settingsPresenter(), SLOT(openToolSettings()));

  this->settingsPresenter()->setHelp(this->helpDialog());
}

void ComponentsManager::initAndOpenExportOrientationsDialog()
{
  disconnect(this->mainWindowPresenter(), &MainWindowPresenter::openExportOrientationsDialog,
             this, &ComponentsManager::initAndOpenExportOrientationsDialog);
  connect(this->mainWindowPresenter(), &MainWindowPresenter::openExportOrientationsDialog,
          this->exportOrientationsPresenter(), &ExportOrientationsPresenter::open);

  this->exportOrientationsPresenter()->setHelp(this->helpDialog());
  this->exportOrientationsPresenter()->open();
}

void ComponentsManager::initAndOpenGeoreferenceDialog()
{
  disconnect(this->mainWindowPresenter(), &MainWindowPresenter::openGeoreferenceDialog,
             this, &ComponentsManager::initAndOpenGeoreferenceDialog);
  connect(this->mainWindowPresenter(), &MainWindowPresenter::openGeoreferenceDialog,
          this->georeferencePresenter(), &GeoreferencePresenter::open);

  connect(this->georeferencePresenter(), SIGNAL(running()),               this->mainWindowPresenter(), SLOT(processRunning()));
  connect(this->georeferencePresenter(), SIGNAL(finished()),              this->mainWindowPresenter(), SLOT(processFinished()));
  connect(this->georeferencePresenter(), SIGNAL(georeferenceFinished()),  this->mainWindowPresenter(), SLOT(loadOrientation()));

  connect(this->progressDialog(), SIGNAL(cancel()),     this->georeferencePresenter(), SLOT(cancel()));

  this->georeferencePresenter()->setProgressHandler(this->progressHandler());
  this->georeferencePresenter()->setHelp(this->helpDialog());
  this->georeferencePresenter()->open();
}

void ComponentsManager::initAndOpenCamerasImportDialog()
{
  disconnect(this->mainWindowPresenter(), &MainWindowPresenter::openCamerasImportDialog,
             this, &ComponentsManager::initAndOpenCamerasImportDialog);
  connect(this->mainWindowPresenter(), &MainWindowPresenter::openCamerasImportDialog,
          this->camerasImportPresenter(), &CamerasImportPresenter::open);

  connect(this->camerasImportPresenter(), SIGNAL(projectModified()), this->mainWindowPresenter(), SLOT(onProjectModified()));

  this->camerasImportPresenter()->setHelp(this->helpDialog());
  this->camerasImportPresenter()->open();
}

void ComponentsManager::initAndOpenExportPointCloudDialog()
{
  disconnect(this->mainWindowPresenter(), &MainWindowPresenter::openExportPointCloudDialog,
             this, &ComponentsManager::initAndOpenExportPointCloudDialog);
  connect(this->mainWindowPresenter(), &MainWindowPresenter::openExportPointCloudDialog,
          this->camerasImportPresenter(), &ExportPointCloudPresenter::open);

  this->exportPointCloudPresenter()->setHelp(this->helpDialog());
  this->exportPointCloudPresenter()->open();
}


} // namespace ui

} // namespace inspector
