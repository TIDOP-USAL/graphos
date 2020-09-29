#include "ComponentsManager.h"

#include "inspector/ui/MainWindowModel.h"
#include "inspector/ui/MainWindowView.h"
#include "inspector/ui/MainWindowPresenter.h"
#include "inspector/ui/ProjectModel.h"
#include "inspector/ui/SettingsModel.h"
#include "inspector/ui/SettingsView.h"
#include "inspector/ui/SettingsPresenter.h"

#include "inspector/ui/NewProjectPresenter.h"
#include "inspector/ui/NewProjectView.h"
#include "inspector/ui/featextract/FeatureExtractorModel.h"
#include "inspector/ui/featextract/FeatureExtractorView.h"
#include "inspector/ui/featextract/FeatureExtractorPresenter.h"
#include "inspector/ui/featmatch/FeatureMatchingModel.h"
#include "inspector/ui/featmatch/FeatureMatchingView.h"
#include "inspector/ui/featmatch/FeatureMatchingPresenter.h"
#include "inspector/ui/images/ImagesModel.h"
#include "inspector/ui/images/ImagesView.h"
#include "inspector/ui/images/ImagesPresenter.h"
#include "inspector/ui/orientation/OrientationModel.h"
#include "inspector/ui/orientation/OrientationView.h"
#include "inspector/ui/orientation/OrientationPresenter.h"
#include "inspector/ui/densification/DensificationModel.h"
#include "inspector/ui/densification/DensificationView.h"
#include "inspector/ui/densification/DensificationPresenter.h"
#include "inspector/ui/cameras/CamerasModel.h"
#include "inspector/ui/cameras/CamerasView.h"
#include "inspector/ui/cameras/CamerasPresenter.h"
#include "inspector/ui/featviewer/FeaturesViewerModel.h"
#include "inspector/ui/featviewer/FeaturesViewerView.h"
#include "inspector/ui/featviewer/FeaturesViewerPresenter.h"
#include "inspector/ui/matchviewer/MatchViewerModel.h"
#include "inspector/ui/matchviewer/MatchViewerView.h"
#include "inspector/ui/matchviewer/MatchViewerPresenter.h"
#include "inspector/ui/export/orientations/ExportOrientationsModel.h"
#include "inspector/ui/export/orientations/ExportOrientationsView.h"
#include "inspector/ui/export/orientations/ExportOrientationsPresenter.h"
#include "inspector/ui/georeference/impl/GeoreferenceModel.h"
#include "inspector/ui/georeference/impl/GeoreferenceView.h"
#include "inspector/ui/georeference/impl/GeoreferencePresenter.h"
//#include "inspector/ui/MultiViewModel.h"
//#include "inspector/ui/MultiViewView.h"
//#include "inspector/ui/MultiViewPresenter.h"
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
    mImagesModel(nullptr),
    mImagesPresenter(nullptr),
    mCamerasModel(nullptr),
    mCamerasPresenter(nullptr),
    mFeaturesModel(nullptr),
    mMatchesModel(nullptr),
    mSettings(new SettingsImp),
    mSettingsController(new SettingsControllerImp),
    mSettingsModel(nullptr),
    mSettingsPresenter(nullptr),
    mNewProjectPresenter(nullptr),
    mFeatureExtractorModel(nullptr),
    mFeatureExtractorPresenter(nullptr),
    mFeatureMatchingModel(nullptr),
    mFeatureMatchingPresenter(nullptr),
    mOrientationModel(nullptr),
    mOrientationPresenter(nullptr),
    mDensificationModel(nullptr),
    mDensificationPresenter(nullptr),
    mFeaturesViewerModel(nullptr),
    mFeaturesViewerPresenter(nullptr),
    mMatchesViewerModel(nullptr),
    mMatchesViewerPresenter(nullptr),
    mExportOrientationsModel(nullptr),
    mExportOrientationsPresenter(nullptr),
    mGeoreferenceModel(nullptr),
    mGeoreferencePresenter(nullptr),
    //mAboutDialog(nullptr),
    mHelpDialog(nullptr),
    mProgressHandler(nullptr),
    mProgressDialog(nullptr)
{

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

  if (mImagesModel){
    delete mImagesModel;
    mImagesModel = nullptr;
  }

  if (mImagesPresenter){
    delete mImagesPresenter;
    mImagesPresenter = nullptr;
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

  if (mFeatureExtractorModel){
    delete mFeatureExtractorModel;
    mFeatureExtractorModel = nullptr;
  }

  if (mFeatureExtractorPresenter){
    delete mFeatureExtractorPresenter;
    mFeatureExtractorPresenter = nullptr;
  }

  if (mFeatureMatchingModel){
    delete mFeatureMatchingModel;
    mFeatureMatchingModel = nullptr;
  }

  if (mFeatureMatchingPresenter){
    delete mFeatureMatchingPresenter;
    mFeatureMatchingPresenter = nullptr;
  }

  if (mOrientationModel){
    delete mOrientationModel;
    mOrientationModel = nullptr;
  }

  if (mOrientationPresenter){
    delete mOrientationPresenter;
    mOrientationPresenter = nullptr;
  }

  if (mDensificationModel){
    delete mDensificationModel;
    mDensificationModel = nullptr;
  }

  if (mDensificationPresenter){
    delete mDensificationPresenter;
    mDensificationPresenter = nullptr;
  }

  if (mFeaturesViewerModel) {
    delete mFeaturesViewerModel;
    mFeaturesViewerModel = nullptr;
  }

  if (mFeaturesViewerPresenter){
    delete mFeaturesViewerPresenter;
    mFeaturesViewerPresenter = nullptr;
  }

  if (mMatchesViewerModel){
    delete mMatchesViewerModel;
    mMatchesViewerModel = nullptr;
  }

  if (mMatchesViewerPresenter){
    delete mMatchesViewerPresenter;
    mMatchesViewerPresenter = nullptr;
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
    mMainWindowModel = new MainWindowModel;
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
                                                   this->imagesModel(),
                                                   this->camerasModel(),
                                                   this->featuresModel(),
                                                   this->matchesModel(),
                                                   this->orientationModel());

//    mMainWindowPresenter->setHelp(this->helpDialog());

    connect(mMainWindowPresenter, &MainWindowPresenter::openNewProjectDialog,
            this, &ComponentsManager::initAndOpenNewProjectDialog);
    connect(mMainWindowPresenter, &MainWindowPresenter::openLoadImagesDialog,
            this, &ComponentsManager::initAndOpenLoadImagesDialog);
    connect(mMainWindowPresenter, &MainWindowPresenter::openFeatureExtractionDialog,
            this, &ComponentsManager::initAndOpenFeatureExtractionDialog);
    connect(mMainWindowPresenter, &MainWindowPresenter::openFeatureMatchingDialog,
            this, &ComponentsManager::initAndOpenFeatureMatchingDialog);
    connect(mMainWindowPresenter, &MainWindowPresenter::openOrientationDialog,
            this, &ComponentsManager::initAndOpenOrientationDialog);
    connect(mMainWindowPresenter, &MainWindowPresenter::openDensificationDialog,
            this, &ComponentsManager::initAndOpenDensificationDialog);
    connect(mMainWindowPresenter, &MainWindowPresenter::openKeypointsViewerDialog,
            this, &ComponentsManager::initAndOpenKeypointsViewerDialog);
    connect(mMainWindowPresenter, &MainWindowPresenter::openKeypointsViewerDialogFromImage,
            this, &ComponentsManager::initAndOpenKeypointsViewerDialogFromImage);
    connect(mMainWindowPresenter, &MainWindowPresenter::openMatchesViewerDialog,
            this, &ComponentsManager::initAndOpenMatchesViewerDialog);
    connect(mMainWindowPresenter, &MainWindowPresenter::openMatchesViewerDialogFromImages,
            this, &ComponentsManager::initAndOpenMatchesViewerDialogFromImages);

//    connect(mMainWindowPresenter, SIGNAL(openExportFeaturesDialog()),    this, SLOT(initAndOpenExportFeaturesDialog()));
//    connect(mMainWindowPresenter, SIGNAL(openExportMatchesDialog()),     this, SLOT(initAndOpenExportMatchesDialog()));
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
//    connect(mMainWindowPresenter, SIGNAL(openAboutDialog()),            this, SLOT(initAndOpenAboutDialog()));

  }

  return mMainWindowPresenter;
}

ProjectModel *ComponentsManager::projectModel()
{
  if (mProjectModel == nullptr){
    mProjectModel = new ProjectModelImp(mProject);
  }
  return mProjectModel;
}

ImagesModel *ComponentsManager::imagesModel()
{
  if (mImagesModel == nullptr){
    mImagesModel = new ImagesModelImp(mProject);
  }
  return mImagesModel;
}

ImagesPresenter *ComponentsManager::imagesPresenter()
{
  if (mImagesPresenter == nullptr){
    ImagesView *view = new ImagesViewImp(this->mainWindowView());
    mImagesPresenter = new ImagesPresenterImp(view,
                                              this->imagesModel(),
                                              this->camerasModel());
  }
  return mImagesPresenter;
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
                                                this->camerasModel(),
                                                this->imagesModel());
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

//ExportFeaturesPresenter *ComponentsManager::exportFeaturesPresenter()
//{
//  if (mExportFeaturesPresenter == nullptr){
//    ExportFeaturesView *exportFeaturesView = new ExportFeaturesViewImp(this->mainWindowView());
//    mExportFeaturesPresenter = new ExportFeaturesPresenterImp(exportFeaturesView, this->exportFeaturesModel());
//  }
//  return mExportFeaturesPresenter;
//}

//ExportFeaturesModel *ComponentsManager::exportFeaturesModel()
//{
//  if (mExportFeaturesModel == nullptr){
//    mExportFeaturesModel = new ExportFeaturesModelImp(this->projectModel());
//  }
//  return mExportFeaturesModel;
//}

//ExportMatchesPresenter *ComponentsManager::exportMatchesPresenter()
//{
//  if (mExportMatchesPresenter == nullptr){
//    ExportMatchesView *exportMatchesView = new ExportMatchesViewImp(this->mainWindowView());
//    mExportMatchesPresenter = new ExportMatchesPresenterImp(exportMatchesView, this->exportMatchesModel());
//  }
//  return mExportMatchesPresenter;
//}

//ExportMatchesModel *ComponentsManager::exportMatchesModel()
//{
//  if (mExportMatchesModel == nullptr){
//    mExportMatchesModel = new ExportMatchesModelImp(this->projectModel());
//  }
//  return mExportMatchesModel;
//}

FeatureExtractorModel *ComponentsManager::featureExtractorModel()
{
  if (mFeatureExtractorModel == nullptr){
    mFeatureExtractorModel = new FeatureExtractorModelImp(mProject);
  }
  return mFeatureExtractorModel;
}

FeatureExtractorPresenter *ComponentsManager::featureExtractorPresenter()
{
  if (mFeatureExtractorPresenter == nullptr){
    FeatureExtractorView *featureExtractorView = new FeatureExtractorViewImp(this->mainWindowView());
    mFeatureExtractorPresenter = new FeatureExtractorPresenterImp(featureExtractorView,
                                                                  this->featureExtractorModel(),
                                                                  this->imagesModel(),
                                                                  this->camerasModel(),
                                                                  this->settingsModel());
  }
  return mFeatureExtractorPresenter;
}

FeatureMatchingModel *ComponentsManager::featureMatchingModel()
{
  if (mFeatureMatchingModel == nullptr){
    mFeatureMatchingModel = new FeatureMatchingModelImp(mProject);
  }
  return mFeatureMatchingModel;
}

FeatureMatchingPresenter *ComponentsManager::featureMatchingPresenter()
{
  if (mFeatureMatchingPresenter == nullptr){
    FeatureMatchingView *featureMatchingView = new FeatureMatchingViewImp(this->mainWindowView());
    mFeatureMatchingPresenter = new FeatureMatchingPresenterImp(featureMatchingView,
                                                                this->featureMatchingModel(),
                                                                this->settingsModel());
  }
  return mFeatureMatchingPresenter;
}

OrientationModel *ComponentsManager::orientationModel()
{
  if (mOrientationModel == nullptr){
    mOrientationModel = new OrientationModelImp(mProject);
  }
  return mOrientationModel;
}

OrientationPresenter *ComponentsManager::orientationPresenter()
{
  if (mOrientationPresenter == nullptr){
    OrientationView *orientationview = new OrientationViewImp(this->mainWindowView());
    mOrientationPresenter = new OrientationPresenterImp(orientationview,
                                                        this->orientationModel(),
                                                        this->imagesModel(),
                                                        this->camerasModel(),
                                                        this->settingsModel());
  }
  return mOrientationPresenter;
}

DensificationModel *ComponentsManager::densificationModel()
{
  if (mDensificationModel == nullptr){
    mDensificationModel = new DensificationModelImp(mProject);
  }
  return mDensificationModel;
}

DensificationPresenter *ComponentsManager::densificationPresenter()
{
  if (mDensificationPresenter == nullptr){
    DensificationView *densificationView = new DensificationViewImp(this->mainWindowView());
    mDensificationPresenter = new DensificationPresenterImp(densificationView,
                                                            this->densificationModel());
  }
  return mDensificationPresenter;
}

FeaturesViewerModel *ComponentsManager::featuresViewerModel()
{
  if (mFeaturesViewerModel == nullptr){
    mFeaturesViewerModel = new FeaturesViewerModelImp(mProject);
  }
  return mFeaturesViewerModel;
}

FeaturesViewerPresenter *ComponentsManager::featuresViewerPresenter()
{
  if (mFeaturesViewerPresenter == nullptr){
    Qt::WindowFlags f(Qt::WindowMinMaxButtonsHint | Qt::WindowCloseButtonHint);
    FeaturesViewerView *featuresViewerView = new FeaturesViewerViewImp(this->mainWindowView(), f);
    mFeaturesViewerPresenter = new FeaturesViewerPresenterImp(featuresViewerView,
                                                              this->featuresViewerModel(),
                                                              this->settingsModel());
  }
  return mFeaturesViewerPresenter;
}

MatchViewerModel *ComponentsManager::matchesViewerModel()
{
  if (mMatchesViewerModel == nullptr) {
    mMatchesViewerModel = new MatchViewerModelImp(mProject);
  }
  return mMatchesViewerModel;
}

MatchViewerPresenter *ComponentsManager::matchesViewerPresenter()
{
  if (mMatchesViewerPresenter == nullptr) {
    Qt::WindowFlags f(Qt::WindowMinMaxButtonsHint | Qt::WindowCloseButtonHint);
    MatchViewerView *matchViewerView = new MatchViewerViewImp(this->mainWindowView(), f);
    mMatchesViewerPresenter = new MatchViewerPresenterImp(matchViewerView,
                                                          this->matchesViewerModel(),
                                                          this->settingsModel());
  }
  return mMatchesViewerPresenter;
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
    GeoreferenceView *view = new GeoreferenceViewImp(this->mainWindowView());
    mGeoreferencePresenter = new GeoreferencePresenterImp(view,
                                                          this->georeferenceModel());
  }
  return mGeoreferencePresenter;
}

HelpDialog *ComponentsManager::helpDialog()
{
  if (mHelpDialog == nullptr) {
    mHelpDialog = new HelpDialog(this->mainWindowView());
    mHelpDialog->setModal(true);
  }
  return mHelpDialog;
}

//MatchViewerModel *ComponentsManager::matchesViewerModel()
//{
//  if (mMatchesViewerModel == nullptr) {
//    mMatchesViewerModel = new MatchViewerModelImp(this->projectModel());
//  }
//  return mMatchesViewerModel;
//}

//MultiViewMatchingAssessmentModel *ComponentsManager::multiviewMatchingAssessmentModel()
//{
//  if (mMultiviewMatchingAssessmentModel == nullptr) {
//    mMultiviewMatchingAssessmentModel = new MultiviewMatchingAssessmentModelImp(mProjectModel);
//  }
//  return mMultiviewMatchingAssessmentModel;
//}

//MultiViewMatchingAssessmentPresenter *ComponentsManager::multiviewMatchingAssessmentPresenter()
//{
//  if (mMultiviewMatchingAssessmentPresenter == nullptr){

//    Qt::WindowFlags f(Qt::WindowMinMaxButtonsHint | Qt::WindowCloseButtonHint);
//    MultiviewMatchingAssessmentView *multiviewView = new MultiviewMatchingAssessmentViewImp(this->mainWindowView(), f);
//    mMultiviewMatchingAssessmentPresenter = new MultiViewMatchingAssessmentPresenterImp(multiviewView, this->multiviewMatchingAssessmentModel());
//    mMultiviewMatchingAssessmentPresenter->setHelp(this->helpDialog());
//  }
//  return mMultiviewMatchingAssessmentPresenter;
//}


//CurvesViewerModel *ComponentsManager::curvesROCViewerModel()
//{
//  if (mCurvesROCViewerModel == nullptr){
//    mCurvesROCViewerModel = new ROCCurvesViewerModel(mProjectModel);
//  }
//  return mCurvesROCViewerModel;
//}

//AboutDialog *ComponentsManager::aboutDialog()
//{
//  if (mAboutDialog == nullptr) {
//    mAboutDialog = new AboutDialog(this->mainWindowView());
//  }
//  return mAboutDialog;
//}

//HelpDialog *ComponentsManager::helpDialog()
//{
//  if (mHelpDialog == nullptr) {
//    mHelpDialog = new HelpDialog(this->mainWindowView()/*, f*/);
//    mHelpDialog->setModal(true);
//  }
//  return mHelpDialog;
//}

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

void ComponentsManager::initAndOpenNewProjectDialog()
{
  disconnect(this->mainWindowPresenter(), &MainWindowPresenter::openNewProjectDialog,
             this, &ComponentsManager::initAndOpenNewProjectDialog);

  connect(this->mainWindowPresenter(), &MainWindowPresenter::openNewProjectDialog, 
          this->newProjectPresenter(), &IPresenter::open);
  connect(this->newProjectPresenter(), SIGNAL(projectCreate()),
          this->mainWindowPresenter(), SLOT(loadProject()));

  this->newProjectPresenter()->setHelp(this->helpDialog());
  this->newProjectPresenter()->open();
}

void ComponentsManager::initAndOpenLoadImagesDialog()
{
  disconnect(this->mainWindowPresenter(), &MainWindowPresenter::openLoadImagesDialog,
             this, &ComponentsManager::initAndOpenLoadImagesDialog);
 
  connect(this->mainWindowPresenter(), SIGNAL(openLoadImagesDialog()),
          this->imagesPresenter(), SLOT(open()));

  TL_TODO("indicar que se están cargando imagenes")
  connect(this->imagesPresenter(), SIGNAL(running()),  this->mainWindowPresenter(), SLOT(processRunning()));
  connect(this->imagesPresenter(), SIGNAL(finished()), this->mainWindowPresenter(), SLOT(processFinish()));
  connect(this->imagesPresenter(), SIGNAL(loadingImages(bool)), this->mainWindowPresenter(), SLOT(loadingImages(bool)));
  connect(this->imagesPresenter(), SIGNAL(imageLoaded(QString)), this->mainWindowPresenter(), SLOT(loadImage(QString)));

  connect(this->progressDialog(), SIGNAL(cancel()),     this->imagesPresenter(), SLOT(cancel()));

  this->imagesPresenter()->setProgressHandler(this->progressHandler());
  this->imagesPresenter()->open();
}

void ComponentsManager::initAndOpenFeatureExtractionDialog()
{
  disconnect(this->mainWindowPresenter(), &MainWindowPresenter::openFeatureExtractionDialog,
             this, &ComponentsManager::initAndOpenFeatureExtractionDialog);
  connect(this->mainWindowPresenter(), &MainWindowPresenter::openFeatureExtractionDialog,
          this->featureExtractorPresenter(), &IPresenter::open);

  connect(this->featureExtractorPresenter(), SIGNAL(running()),                  this->mainWindowPresenter(), SLOT(processRunning()));
  TL_TODO("Ahora se estan borrando al iniciar el comando...")
  //connect(this->featureExtractorPresenter(), SIGNAL(running()),                  this->mainWindowPresenter(), SLOT(deleteFeatures()));
  connect(this->featureExtractorPresenter(), SIGNAL(finished()),                 this->mainWindowPresenter(), SLOT(processFinish()));
  connect(this->featureExtractorPresenter(), SIGNAL(featuresExtracted(QString)), this->mainWindowPresenter(), SLOT(loadFeatures(QString)));

  connect(this->progressDialog(), SIGNAL(cancel()),     this->featureExtractorPresenter(), SLOT(cancel()));

  this->featureExtractorPresenter()->setProgressHandler(this->progressHandler());
  this->featureExtractorPresenter()->setHelp(this->helpDialog());
  this->featureExtractorPresenter()->open();
}

void ComponentsManager::initAndOpenFeatureMatchingDialog()
{
  disconnect(this->mainWindowPresenter(), &MainWindowPresenter::openFeatureMatchingDialog,
             this, &ComponentsManager::initAndOpenFeatureMatchingDialog);
  connect(this->mainWindowPresenter(), &MainWindowPresenter::openFeatureMatchingDialog,
          this->featureMatchingPresenter(), &IPresenter::open);

  connect(this->featureMatchingPresenter(), SIGNAL(running()),   this->mainWindowPresenter(), SLOT(processRunning()));
  TL_TODO("Ahora se estan borrando al iniciar el comando...")
  //connect(this->featureMatchingPresenter(), SIGNAL(running()),   this->mainWindowPresenter(), SLOT(deleteMatches()));
  connect(this->featureMatchingPresenter(), SIGNAL(finished()),  this->mainWindowPresenter(), SLOT(processFinish()));
  connect(this->featureMatchingPresenter(), SIGNAL(matchingFinished()),  this->mainWindowPresenter(), SLOT(loadMatches()));

  connect(this->progressDialog(), SIGNAL(cancel()),     this->featureMatchingPresenter(), SLOT(cancel()));

  this->featureMatchingPresenter()->setProgressHandler(this->progressHandler());
  this->featureMatchingPresenter()->setHelp(this->helpDialog());
  this->featureMatchingPresenter()->open();
}

void ComponentsManager::initAndOpenOrientationDialog()
{
  disconnect(this->mainWindowPresenter(), &MainWindowPresenter::openOrientationDialog,
             this, &ComponentsManager::initAndOpenOrientationDialog);
  connect(this->mainWindowPresenter(), &MainWindowPresenter::openOrientationDialog,
          this->orientationPresenter(), &IPresenter::open);

  connect(this->orientationPresenter(), SIGNAL(running()),   this->mainWindowPresenter(), SLOT(processRunning()));
  connect(this->orientationPresenter(), SIGNAL(finished()),  this->mainWindowPresenter(), SLOT(processFinish()));
  connect(this->orientationPresenter(), SIGNAL(orientationFinished()),  this->mainWindowPresenter(), SLOT(loadOrientation()));

  connect(this->progressDialog(), SIGNAL(cancel()),     this->orientationPresenter(), SLOT(cancel()));

  this->orientationPresenter()->setProgressHandler(this->progressHandler());
  this->orientationPresenter()->setHelp(this->helpDialog());
  this->orientationPresenter()->open();
}

void ComponentsManager::initAndOpenDensificationDialog()
{
  disconnect(mMainWindowPresenter, &MainWindowPresenter::openDensificationDialog,
             this, &ComponentsManager::initAndOpenDensificationDialog);
  connect(this->mainWindowPresenter(), &MainWindowPresenter::openDensificationDialog,
          this->densificationPresenter(), &IPresenter::open);

  connect(this->densificationPresenter(), SIGNAL(running()),   this->mainWindowPresenter(), SLOT(processRunning()));
  connect(this->densificationPresenter(), SIGNAL(finished()),  this->mainWindowPresenter(), SLOT(processFinish()));
  connect(this->densificationPresenter(), SIGNAL(densificationFinished()),  this->mainWindowPresenter(), SLOT(loadDenseModel()));

  connect(this->progressDialog(), SIGNAL(cancel()),     this->densificationPresenter(), SLOT(cancel()));

  this->densificationPresenter()->setProgressHandler(this->progressHandler());
  this->densificationPresenter()->setHelp(this->helpDialog());
  this->densificationPresenter()->open();
}




//void ComponentsManager::initAndOpenExportFeaturesDialog()
//{
//  disconnect(this->mainWindowPresenter(), SIGNAL(openExportFeaturesDialog()), this, SLOT(initAndOpenExportFeaturesDialog()));
//  connect(this->mainWindowPresenter(), SIGNAL(openExportFeaturesDialog()), this->exportFeaturesPresenter(), SLOT(open()));
//  this->exportFeaturesPresenter()->setHelp(this->helpDialog());
//  this->exportFeaturesPresenter()->open();
//}

//void ComponentsManager::initAndOpenExportMatchesDialog()
//{
//  disconnect(this->mainWindowPresenter(), SIGNAL(openExportMatchesDialog()),     this, SLOT(initAndOpenExportMatchesDialog()));
//  connect(this->mainWindowPresenter(), SIGNAL(openExportMatchesDialog()), this->exportMatchesPresenter(), SLOT(open()));

//  this->exportMatchesPresenter()->setHelp(this->helpDialog());
//  this->exportMatchesPresenter()->open();
//}

//void ComponentsManager::initAndOpenAboutDialog()
//{
//  disconnect(this->mainWindowPresenter(), &MainWindowPresenter::openAboutDialog,
//             this, &ComponentsManager::initAndOpenAboutDialog);
//  connect(this->mainWindowPresenter(), &MainWindowPresenter::openAboutDialog,
//          this->aboutDialog(), &AboutDialog::open);
//  this->aboutDialog()->open();
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

void ComponentsManager::initAndOpenKeypointsViewerDialog()
{
  this->initKeypointsViewer();
  this->featuresViewerPresenter()->open();
}

void ComponentsManager::initAndOpenKeypointsViewerDialogFromImage(const QString &image)
{
  this->initKeypointsViewer();
  this->featuresViewerPresenter()->openKeypointsFromImage(image);
}

void ComponentsManager::initKeypointsViewer()
{
  disconnect(this->mainWindowPresenter(), &MainWindowPresenter::openKeypointsViewerDialog,
             this, &ComponentsManager::initAndOpenKeypointsViewerDialog);
  disconnect(this->mainWindowPresenter(), &MainWindowPresenter::openKeypointsViewerDialogFromImage,
             this, &ComponentsManager::initAndOpenKeypointsViewerDialogFromImage);
  connect(this->mainWindowPresenter(), &MainWindowPresenter::openKeypointsViewerDialog,
          this->featuresViewerPresenter(), &FeaturesViewerPresenter::open);
  connect(this->mainWindowPresenter(), &MainWindowPresenter::openKeypointsViewerDialogFromImage,
          this->featuresViewerPresenter(), &FeaturesViewerPresenter::openKeypointsFromImage);

  //this->featuresViewerPresenter()->setHelp(this->helpDialog());
}

void ComponentsManager::initAndOpenMatchesViewerDialog()
{
  this->initMatchesViewer();
  this->matchesViewerPresenter()->open();
}

void ComponentsManager::initAndOpenMatchesViewerDialogFromImages(const QString &leftImage,
                                                                 const QString &rightImage)
{
  this->initMatchesViewer();
  this->matchesViewerPresenter()->openFromImages(leftImage, rightImage);
}

void ComponentsManager::initMatchesViewer()
{
  disconnect(this->mainWindowPresenter(), &MainWindowPresenter::openMatchesViewerDialog,
             this, &ComponentsManager::initAndOpenMatchesViewerDialog);
  disconnect(this->mainWindowPresenter(), &MainWindowPresenter::openMatchesViewerDialogFromImages,
             this, &ComponentsManager::initAndOpenMatchesViewerDialogFromImages);

  connect(this->mainWindowPresenter(), &MainWindowPresenter::openMatchesViewerDialog,
          this->matchesViewerPresenter(), &MatchViewerPresenter::open);
  connect(this->mainWindowPresenter(), &MainWindowPresenter::openMatchesViewerDialogFromImages,
          this->matchesViewerPresenter(), &MatchViewerPresenter::openFromImages);

  this->matchesViewerPresenter()->setHelp(this->helpDialog());
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

  this->georeferencePresenter()->setHelp(this->helpDialog());
  this->georeferencePresenter()->open();
}

//void ComponentsManager::initAndOpenMultiviewMatchingAssessmentDialog()
//{
//  disconnect(mMainWindowPresenter, SIGNAL(openMultiviewMatchingAssessmentDialog()), this, SLOT(initAndOpenMultiviewMatchingAssessmentDialog()));
//  connect(mMainWindowPresenter, SIGNAL(openMultiviewMatchingAssessmentDialog()), this->multiviewMatchingAssessmentPresenter(), SLOT(open()));

//  this->multiviewMatchingAssessmentPresenter()->open();
//}


} // namespace ui

} // namespace inspector
