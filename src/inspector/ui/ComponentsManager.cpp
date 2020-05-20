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

#include "inspector/ui/cameras/CamerasModel.h"
#include "inspector/ui/cameras/CamerasView.h"
#include "inspector/ui/cameras/CamerasPresenter.h"

#include "inspector/ui/ImagesModel.h"

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
    mCamerasModel(nullptr),
    mCamerasPresenter(nullptr),
    mSettings(new SettingsImp),
    mSettingsController(new SettingsControllerImp),
    mSettingsModel(nullptr),
    mSettingsPresenter(nullptr),
    mNewProjectPresenter(nullptr),
    mFeatureExtractorModel(nullptr),
    mFeatureExtractorPresenter(nullptr),
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

  if (mCamerasModel){
    delete mCamerasModel;
    mCamerasModel = nullptr;
  }

  if (mCamerasPresenter){
    delete mCamerasPresenter;
    mCamerasPresenter = nullptr;
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
                                                   this->camerasModel());

//    mMainWindowPresenter->setHelp(this->helpDialog());

    connect(mMainWindowPresenter, &MainWindowPresenter::openNewProjectDialog,
            this, &ComponentsManager::initAndOpenNewProjectDialog);

    connect(mMainWindowPresenter, &MainWindowPresenter::openFeatureExtractionDialog,
            this, &ComponentsManager::initAndOpenFeatureExtractionDialog);
    connect(mMainWindowPresenter, &MainWindowPresenter::openFeatureMatchingDialog,
            this, &ComponentsManager::initAndOpenFeatureMatchingDialog);

//    connect(mMainWindowPresenter, SIGNAL(openKeypointsViewerDialogFromSession(QString)),   this, SLOT(initAndOpenKeypointsViewerDialogFromSession(QString)));
//    connect(mMainWindowPresenter, SIGNAL(openKeypointsViewerDialogFromSessionAndImage(QString, QString)), this, SLOT(initAndOpenKeypointsViewerDialogFromSessionAndImage(QString, QString)));
//    connect(mMainWindowPresenter, SIGNAL(openMatchesViewerDialogFromSession(QString)),     this, SLOT(initAndOpenMatchesViewerDialogFromSession(QString)));
//    connect(mMainWindowPresenter, SIGNAL(openMatchesViewerDialogFromSessionAndImages(QString, QString, QString)),     this, SLOT(initAndOpenMatchesViewerDialogFromSessionAndImages(QString, QString, QString)));
//    connect(mMainWindowPresenter, SIGNAL(openExportFeaturesDialog()),    this, SLOT(initAndOpenExportFeaturesDialog()));
//    connect(mMainWindowPresenter, SIGNAL(openExportMatchesDialog()),     this, SLOT(initAndOpenExportMatchesDialog()));
//    connect(mMainWindowPresenter, SIGNAL(openGroundTruthEditorDialog()), this, SLOT(initAndOpenGroundTruthEditorDialog()));
//    connect(mMainWindowPresenter, SIGNAL(openHomographyViewerDialog()),  this, SLOT(initAndOpenHomographyViewerDialog()));
//    connect(mMainWindowPresenter, SIGNAL(openPRCurvesViewerDialog()),  this, SLOT(initAndOpenPRCurvesViewerDialog()));
//    connect(mMainWindowPresenter, SIGNAL(openROCCurvesViewerDialog()),  this, SLOT(initAndOpenROCCurvesViewerDialog()));
//    connect(mMainWindowPresenter, SIGNAL(openDETCurvesViewerDialog()),  this, SLOT(initAndOpenDETCurvesViewerDialog()));
//    connect(mMainWindowPresenter, SIGNAL(openAboutDialog()),            this, SLOT(initAndOpenAboutDialog()));

    connect(mMainWindowPresenter, &MainWindowPresenter::openCamerasDialog,
             this, &ComponentsManager::initAndOpenCamerasDialog);
    connect(mMainWindowPresenter, &MainWindowPresenter::openSettingsDialog,
            this, &ComponentsManager::initAndOpenSettingsDialog);
    connect(mMainWindowPresenter, &MainWindowPresenter::openViewSettingsDialog,
            this, &ComponentsManager::initAndOpenViewSettingsDialog);
    connect(mMainWindowPresenter, &MainWindowPresenter::openToolSettingsDialog,
            this, &ComponentsManager::initAndOpenToolSettingsDialog);

//    connect(mMainWindowPresenter, SIGNAL(openMultiviewMatchingAssessmentDialog()),     this, SLOT(initAndOpenMultiviewMatchingAssessmentDialog()));
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

FeatureExtractorModel *ComponentsManager::featureExtractorModel()
{
  if (mFeatureExtractorModel == nullptr){
    mFeatureExtractorModel = new FeatureExtractorModelImp(mProject);
  }
  return mFeatureExtractorModel;
}

//NewSessionPresenter *ComponentsManager::newSessionPresenter()
//{
//  if (mNewSessionPresenter == nullptr){
//    NewSessionView *newSessionView = new NewSessionViewImp(this->mainWindowView());
//    mNewSessionPresenter = new NewSessionPresenterImp(newSessionView, this->projectModel());
//  }
//  return mNewSessionPresenter;
//}

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

//PreprocessPresenter *ComponentsManager::preprocessPresenter()
//{
//  if (mPreprocessPresenter == nullptr){
//    PreprocessView *preprocessView = new PreprocessViewImp(this->mainWindowView());
//    mPreprocessPresenter = new PreprocessPresenterImp(preprocessView,
//                                                   this->projectModel(),
//                                                   this->settingsModel());
//  }
//  return mPreprocessPresenter;
//}

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

//DescriptorMatcherPresenter *ComponentsManager::descriptorMatcherPresenter()
//{
//  if (mDescriptorMatcherPresenter == nullptr){
//    DescriptorMatcherView *descriptorMatcherView = new DescriptorMatcherViewImp(this->mainWindowView());
//    mDescriptorMatcherPresenter = new DescriptorMatcherPresenterImp(descriptorMatcherView,
//                                                                 this->projectModel(),
//                                                                 this->settingsModel());
//  }
//  return mDescriptorMatcherPresenter;
//}

//FeaturesViewerPresenter *ComponentsManager::featuresViewerPresenter()
//{
//  if (mFeaturesViewerPresenter == nullptr) {

//    Qt::WindowFlags f(Qt::WindowMinMaxButtonsHint | Qt::WindowCloseButtonHint);
//    FeaturesViewerView *featuresViewerView = new FeaturesViewerViewImp(this->mainWindowView(), f);
//    mFeaturesViewerPresenter = new FeaturesViewerPresenterImp(featuresViewerView,
//                                                           this->featuresViewerModel(),
//                                                           this->settingsModel());
//  }
//  return mFeaturesViewerPresenter;
//}

//FeaturesViewerModel *ComponentsManager::featuresViewerModel()
//{
//  if (mFeaturesViewerModel == nullptr) {
//    mFeaturesViewerModel = new FeaturesViewerModelImp(this->projectModel());
//  }
//  return mFeaturesViewerModel;
//}


//MatchViewerPresenter *ComponentsManager::matchesViewerPresenter()
//{
//  if (mMatchesViewerPresenter == nullptr) {
//    Qt::WindowFlags f(Qt::WindowMinMaxButtonsHint | Qt::WindowCloseButtonHint);
//    MatchViewerView *matchViewerView = new MatchViewerViewImp(this->mainWindowView(), f);
//    mMatchesViewerPresenter = new MatchViewerPresenterImp(matchViewerView,
//                                                       this->matchesViewerModel(),
//                                                       this->settingsModel());
//  }
//  return mMatchesViewerPresenter;
//}

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

//GroundTruthPresenter *ComponentsManager::groundTruthPresenter()
//{
//  if (mGroundTruthPresenter == nullptr){

//    Qt::WindowFlags f(Qt::WindowMinMaxButtonsHint | Qt::WindowCloseButtonHint);
//    GroundTruthView *groundTruthView = new GroundTruthViewImp(this->mainWindowView(), f);
//    mGroundTruthPresenter = new GroundTruthPresenterImp(groundTruthView,
//                                                     this->groundTruthModel(),
//                                                     this->settingsModel());
//  }
//  return mGroundTruthPresenter;
//}

//GroundTruthModel *ComponentsManager::groundTruthModel()
//{
//  if (mGroundTruthModel == nullptr){
//    mGroundTruthModel = new GroundTruthModelImp(this->projectModel());
//  }
//  return mGroundTruthModel;
//}

//HomographyViewerPresenter *ComponentsManager::homographyViewerPresenter()
//{
//  if (mHomographyViewerPresenter == nullptr) {
//    Qt::WindowFlags f(Qt::WindowMinMaxButtonsHint | Qt::WindowCloseButtonHint);
//    HomographyViewerView *homographyViewerView = new HomographyViewerViewImp(this->mainWindowView(), f);
//    mHomographyViewerPresenter = new HomographyViewerPresenterImp(homographyViewerView,
//                                                               this->homographyViewerModel(),
//                                                               this->settingsModel());
//  }
//  return mHomographyViewerPresenter;
//}

//HomographyViewerModel *ComponentsManager::homographyViewerModel()
//{
//  if (mHomographyViewerModel == nullptr){
//    mHomographyViewerModel = new HomographyViewerModelImp(mProjectModel);
//  }
//  return mHomographyViewerModel;
//}

//CurvesViewerPresenter *ComponentsManager::curvesPRViewerPresenter()
//{
//  if (mCurvesPRViewerPresenter == nullptr) {

//    Qt::WindowFlags f(Qt::WindowMinMaxButtonsHint | Qt::WindowCloseButtonHint);
//    CurvesViewerViewImp *curvesViewerView = new PRCurvesViewer(this->mainWindowView(), f);
//    mCurvesPRViewerPresenter = new CurvesViewerPresenterImp(curvesViewerView, this->curvesPRViewerModel());
//  }
//  return mCurvesPRViewerPresenter;
//}

//CurvesViewerModel *ComponentsManager::curvesPRViewerModel()
//{
//  if (mCurvesPRViewerModel == nullptr){
//    mCurvesPRViewerModel = new PRCurvesViewerModel(mProjectModel);
//  }
//  return mCurvesPRViewerModel;
//}

//CurvesViewerPresenter *ComponentsManager::curvesROCViewerPresenter()
//{
//  if (mCurvesROCViewerPresenter == nullptr) {
//    Qt::WindowFlags f(Qt::WindowMinMaxButtonsHint | Qt::WindowCloseButtonHint);
//    CurvesViewerViewImp *curvesViewerView = new ROCCurvesViewer(this->mainWindowView(), f);
//    mCurvesROCViewerPresenter = new CurvesViewerPresenterImp(curvesViewerView, this->curvesROCViewerModel());
//  }
//  return mCurvesROCViewerPresenter;
//}

//CurvesViewerModel *ComponentsManager::curvesROCViewerModel()
//{
//  if (mCurvesROCViewerModel == nullptr){
//    mCurvesROCViewerModel = new ROCCurvesViewerModel(mProjectModel);
//  }
//  return mCurvesROCViewerModel;
//}

//CurvesViewerPresenter *ComponentsManager::curvesDETViewerPresenter()
//{
//  if (mCurvesDETViewerPresenter == nullptr) {
//    Qt::WindowFlags f(Qt::WindowMinMaxButtonsHint | Qt::WindowCloseButtonHint);
//    CurvesViewerViewImp *curvesViewerView = new DETCurvesViewer(this->mainWindowView(), f);
//    mCurvesDETViewerPresenter = new CurvesViewerPresenterImp(curvesViewerView, this->curvesDETViewerModel());
//  }
//  return mCurvesDETViewerPresenter;
//}

//CurvesViewerModel *ComponentsManager::curvesDETViewerModel()
//{
//  if (mCurvesDETViewerModel == nullptr){
//    mCurvesDETViewerModel = new DETCurvesViewerModel(mProjectModel);
//  }
//  return mCurvesDETViewerModel;
//}

//RepeatabilityPresenter *ComponentsManager::repeatabilityPresenter()
//{
//  if (mRepeatabilityPresenter == nullptr){
//    Qt::WindowFlags f(Qt::WindowMinMaxButtonsHint | Qt::WindowCloseButtonHint);
//    RepeatabilityView *repeatabilityView = new RepeatabilityViewImp(this->mainWindowView(), f);
//    mRepeatabilityPresenter = new RepeatabilityPresenterImp(repeatabilityView, this->repeatabilityModel());
//    mRepeatabilityPresenter->setHelp(this->helpDialog());
//  }
//  return mRepeatabilityPresenter;
//}

//RepeatabilityModel *ComponentsManager::repeatabilityModel()
//{
//  if (mRepeatabilityModel == nullptr) {
//    mRepeatabilityModel = new RepeatabilityModelImp(mProjectModel);
//  }
//  return mRepeatabilityModel;
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
  disconnect(this->mainWindowPresenter(), SIGNAL(openNewProjectDialog()), this, SLOT(initAndOpenNewProjectDialog()));

  connect(this->mainWindowPresenter(), SIGNAL(openNewProjectDialog()), this->newProjectPresenter(), SLOT(open()));
  connect(this->newProjectPresenter(), SIGNAL(projectCreate()),        this->mainWindowPresenter(), SLOT(loadProject()));

  //this->newProjectPresenter()->setHelp(this->helpDialog());
  this->newProjectPresenter()->open();
}

void ComponentsManager::initAndOpenFeatureExtractionDialog()
{
  disconnect(this->mainWindowPresenter(), SIGNAL(openFeatureExtractionDialog()), this, SLOT(initAndOpenFeatureExtractionDialog()));
  connect(this->mainWindowPresenter(), SIGNAL(openFeatureExtractionDialog()), this->featureExtractorPresenter(), SLOT(open()));

  connect(this->featureExtractorPresenter(), SIGNAL(running()),                  this->mainWindowPresenter(), SLOT(processRunning()));
  connect(this->featureExtractorPresenter(), SIGNAL(running()),                  this->mainWindowPresenter(), SLOT(deleteFeatures()));
  connect(this->featureExtractorPresenter(), SIGNAL(finished()),                 this->mainWindowPresenter(), SLOT(processFinish()));
  connect(this->featureExtractorPresenter(), SIGNAL(imagePreprocessed(QString)), this->mainWindowPresenter(), SLOT(updatePreprocess()));
  connect(this->featureExtractorPresenter(), SIGNAL(featuresExtracted(QString)), this->mainWindowPresenter(), SLOT(updateFeatures()));

  connect(this->progressDialog(), SIGNAL(cancel()),     this->featureExtractorPresenter(), SLOT(cancel()));

  this->featureExtractorPresenter()->setProgressHandler(this->progressHandler());
  //this->featureExtractorPresenter()->setHelp(this->helpDialog());
  this->featureExtractorPresenter()->open();
}

void ComponentsManager::initAndOpenFeatureMatchingDialog()
{
//  disconnect(this->mainWindowPresenter(), SIGNAL(openFeatureMatchingDialog()), this, SLOT(initAndOpenFeatureMatchingDialog()));
//  connect(this->mainWindowPresenter(), SIGNAL(openFeatureMatchingDialog()),   this->descriptorMatcherPresenter(), SLOT(open()));

//  connect(this->descriptorMatcherPresenter(), SIGNAL(running()),             this->mainWindowPresenter(), SLOT(processRunning()));
//  connect(this->descriptorMatcherPresenter(), SIGNAL(running()),             this->mainWindowPresenter(), SLOT(deleteMatches()));
//  connect(this->descriptorMatcherPresenter(), SIGNAL(finished()),            this->mainWindowPresenter(), SLOT(processFinish()));
//  connect(this->descriptorMatcherPresenter(), SIGNAL(matchCompute(QString)), this->mainWindowPresenter(), SLOT(updateMatches()));

//  connect(this->progressDialog(), SIGNAL(cancel()),     this->descriptorMatcherPresenter(), SLOT(cancel()));

//  this->descriptorMatcherPresenter()->setProgressHandler(this->progressHandler());
//  this->descriptorMatcherPresenter()->setHelp(this->helpDialog());
  //  this->descriptorMatcherPresenter()->open();
}

void ComponentsManager::initAndOpenOrientationDialog()
{

}

//void ComponentsManager::initAndOpenKeypointsViewerDialogFromSession(const QString &session)
//{
//  this->initKeypointsViewer();
//  this->featuresViewerPresenter()->openKeypointsFromSession(session);
//}

//void ComponentsManager::initAndOpenKeypointsViewerDialogFromSessionAndImage(const QString &session,
//                                                                            const QString &image)
//{
//  this->initKeypointsViewer();
//  this->featuresViewerPresenter()->openKeypointsFromSessionAndImage(session, image);
//}

//void ComponentsManager::initKeypointsViewer()
//{
//  disconnect(this->mainWindowPresenter(), &MainWindowPresenter::openKeypointsViewerDialogFromSession,
//             this, &ComponentsManager::initAndOpenKeypointsViewerDialogFromSession);
//  disconnect(this->mainWindowPresenter(), &MainWindowPresenter::openKeypointsViewerDialogFromSessionAndImage,
//             this, &ComponentsManager::initAndOpenKeypointsViewerDialogFromSessionAndImage);
//  connect(this->mainWindowPresenter(), &MainWindowPresenter::openKeypointsViewerDialogFromSession,
//          this->featuresViewerPresenter(), &FeaturesViewerPresenter::openKeypointsFromSession);
//  connect(this->mainWindowPresenter(), &MainWindowPresenter::openKeypointsViewerDialogFromSessionAndImage,
//          this->featuresViewerPresenter(), &FeaturesViewerPresenter::openKeypointsFromSessionAndImage);

//  this->featuresViewerPresenter()->setHelp(this->helpDialog());
//}

//void ComponentsManager::initAndOpenMatchesViewerDialogFromSession(const QString &session)
//{
//  this->initMatchesViewer();
//  this->matchesViewerPresenter()->openFromSession(session);
//}

//void ComponentsManager::initAndOpenMatchesViewerDialogFromSessionAndImages(const QString &session,
//                                                                           const QString &leftImage,
//                                                                           const QString &rightImage)
//{
//  this->initMatchesViewer();
//  this->matchesViewerPresenter()->openFromSessionAndImages(session, leftImage, rightImage);
//}

//void ComponentsManager::initMatchesViewer()
//{
//  disconnect(this->mainWindowPresenter(), &MainWindowPresenter::openMatchesViewerDialogFromSession,
//             this, &ComponentsManager::initAndOpenMatchesViewerDialogFromSession);
//  disconnect(this->mainWindowPresenter(), &MainWindowPresenter::openMatchesViewerDialogFromSessionAndImages,
//             this, &ComponentsManager::initAndOpenMatchesViewerDialogFromSessionAndImages);

//  connect(this->mainWindowPresenter(), &MainWindowPresenter::openMatchesViewerDialogFromSession,
//          this->matchesViewerPresenter(), &MatchViewerPresenter::openFromSession);
//  connect(this->mainWindowPresenter(), &MainWindowPresenter::openMatchesViewerDialogFromSessionAndImages,
//          this->matchesViewerPresenter(), &MatchViewerPresenter::openFromSessionAndImages);

//  this->matchesViewerPresenter()->setHelp(this->helpDialog());
//}

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

//void ComponentsManager::initAndOpenGroundTruthEditorDialog()
//{
//  disconnect(this->mainWindowPresenter(), SIGNAL(openGroundTruthEditorDialog()), this, SLOT(initAndOpenGroundTruthEditorDialog()));
//  connect(this->mainWindowPresenter(), SIGNAL(openGroundTruthEditorDialog()), this->groundTruthPresenter(), SLOT(open()));
//  connect(this->groundTruthPresenter(), SIGNAL(groundTruthAdded()), this->mainWindowPresenter(), SLOT(groundTruthAdded()));

//  this->groundTruthPresenter()->setHelp(this->helpDialog());
//  this->groundTruthPresenter()->open();
//}

//void ComponentsManager::initAndOpenHomographyViewerDialog()
//{
//  disconnect(this->mainWindowPresenter(), &MainWindowPresenter::openHomographyViewerDialog,
//             this, &ComponentsManager::initAndOpenHomographyViewerDialog);
//  connect(this->mainWindowPresenter(), &MainWindowPresenter::openHomographyViewerDialog,
//          this->homographyViewerPresenter(), &IPresenter::open);

//  this->homographyViewerPresenter()->setHelp(this->helpDialog());
//  this->homographyViewerPresenter()->open();
//}

//void ComponentsManager::initAndOpenPRCurvesViewerDialog()
//{
//  disconnect(this->mainWindowPresenter(), &MainWindowPresenter::openPRCurvesViewerDialog,
//             this, &ComponentsManager::initAndOpenPRCurvesViewerDialog);
//  connect(this->mainWindowPresenter(), &MainWindowPresenter::openPRCurvesViewerDialog,
//          this->curvesPRViewerPresenter(), &IPresenter::open);

//  this->curvesPRViewerPresenter()->setHelp(this->helpDialog());
//  this->curvesPRViewerPresenter()->open();
//}

//void ComponentsManager::initAndOpenROCCurvesViewerDialog()
//{
//  disconnect(this->mainWindowPresenter(), &MainWindowPresenter::openROCCurvesViewerDialog,
//             this, &ComponentsManager::initAndOpenROCCurvesViewerDialog);
//  connect(this->mainWindowPresenter(), &MainWindowPresenter::openPRCurvesViewerDialog,
//          this->curvesROCViewerPresenter(), &IPresenter::open);

//  this->curvesROCViewerPresenter()->setHelp(this->helpDialog());
//  this->curvesROCViewerPresenter()->open();
//}

//void ComponentsManager::initAndOpenDETCurvesViewerDialog()
//{
//  disconnect(this->mainWindowPresenter(), &MainWindowPresenter::openDETCurvesViewerDialog,
//             this, &ComponentsManager::initAndOpenDETCurvesViewerDialog);
//  connect(this->mainWindowPresenter(), &MainWindowPresenter::openPRCurvesViewerDialog,
//          this->curvesDETViewerPresenter(), &IPresenter::open);

//  this->curvesDETViewerPresenter()->setHelp(this->helpDialog());
//  this->curvesDETViewerPresenter()->open();
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


//  this->camerasPresenter()->setHelp(this->helpDialog());
  this->camerasPresenter()->open();

}

void ComponentsManager::initSettingsDialog()
{
  disconnect(this->mainWindowPresenter(), SIGNAL(openSettingsDialog()), this, SLOT(initAndOpenSettingsDialog()));
  disconnect(this->mainWindowPresenter(), SIGNAL(openViewSettingsDialog()), this, SLOT(initAndOpenViewSettingsDialog()));
  disconnect(this->mainWindowPresenter(), SIGNAL(openToolSettingsDialog()), this, SLOT(initAndOpenToolSettingsDialog()));

  connect(this->mainWindowPresenter(), SIGNAL(openSettingsDialog()), this->settingsPresenter(), SLOT(open()));
  connect(this->mainWindowPresenter(), SIGNAL(openViewSettingsDialog()), this->settingsPresenter(), SLOT(openViewSettings()));
  connect(this->mainWindowPresenter(), SIGNAL(openQualityControlSettingsDialog()), this->settingsPresenter(), SLOT(openQualityControlSettings()));
  connect(this->mainWindowPresenter(), SIGNAL(openToolSettingsDialog()), this->settingsPresenter(), SLOT(openToolSettings()));

  //this->settingsPresenter()->setHelp(this->helpDialog());
}

//void ComponentsManager::initAndOpenMultiviewMatchingAssessmentDialog()
//{
//  disconnect(mMainWindowPresenter, SIGNAL(openMultiviewMatchingAssessmentDialog()), this, SLOT(initAndOpenMultiviewMatchingAssessmentDialog()));
//  connect(mMainWindowPresenter, SIGNAL(openMultiviewMatchingAssessmentDialog()), this->multiviewMatchingAssessmentPresenter(), SLOT(open()));

//  this->multiviewMatchingAssessmentPresenter()->open();
//}


} // namespace ui

} // namespace inspector
