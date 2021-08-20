#include "MainWindowPresenter.h"

#include "graphos/ui/MainWindowView.h"
#include "graphos/ui/MainWindowModel.h"
#include "graphos/ui/ProjectModel.h"
#include "graphos/ui/SettingsModel.h"
#include "graphos/ui/SettingsPresenter.h"
#include "graphos/ui/SettingsView.h"
#include "graphos/ui/utils/TabHandler.h"
#include "graphos/ui/HelpDialog.h"
#include "graphos/widgets/StartPageWidget.h"
#include "graphos/ui/cameras/CamerasModel.h"
#include "graphos/ui/FeaturesModel.h"
#include "graphos/ui/MatchesModel.h"
#include "graphos/ui/AppStatus.h"
#include "graphos/core/utils.h"

/* TidopLib */
#include <tidop/core/messages.h>
#include <tidop/core/exception.h>

/* Qt */
#include <QFileDialog>
#include <QMessageBox>
#include <QDesktopServices>
#include <QUrl>
#include <QSqlQuery>
#include <QSqlError>
#include <QApplication>

namespace graphos
{

namespace ui
{

MainWindowPresenter::MainWindowPresenter(MainWindowView *view,
                                         MainWindowModel *model,
                                         ProjectModel *projectModel,
                                         SettingsModel *settingsModel,
                                         FeaturesModel *featuresModel,
                                         MatchesModel *matchesModel)
  : Presenter(),
    mView(view),
    mModel(model),
    mProjectModel(projectModel),
    mSettingsModel(settingsModel),
    mFeaturesModel(featuresModel),
    mMatchesModel(matchesModel),
    mHelpDialog(nullptr),
    mTabHandler(nullptr),
    mStartPageWidget(nullptr)
{
  this->init();
  this->initSignalAndSlots();
}

MainWindowPresenter::~MainWindowPresenter()
{
}

void MainWindowPresenter::openFromHistory(const QString &file)
{
  if (QFileInfo(file).exists()) {

    // Se comprueba si hay un proyecto abierto con cambios sin guardar
    if(mProjectModel->checkUnsavedChanges()) {
      int i_ret = QMessageBox(QMessageBox::Information,
                              tr("Save Changes"),
                              tr("There are unsaved changes. Do you want to save them?"),
                              QMessageBox::Yes|QMessageBox::No|QMessageBox::Cancel).exec();
      if (i_ret == QMessageBox::Yes) {
        saveProject();
      } else if (i_ret == QMessageBox::Cancel) {
        return;
     }
    }

    mProjectModel->clear();

    if (mProjectModel->checkOldVersion(file)){
      int i_ret = QMessageBox(QMessageBox::Information,
                              tr("It is loading an old project"),
                              tr("If you accept, a copy of the old project will be created"),
                              QMessageBox::Yes|QMessageBox::No).exec();
      if (i_ret == QMessageBox::Yes) {
        mProjectModel->oldVersionBackup(file);
        //mView->setFlag(MainWindowView::Flag::project_modified, true);
        AppStatus::instance().activeFlag(AppStatus::Flag::project_modified, true);
      } else if (i_ret == QMessageBox::Cancel) {
        return;
      }

    }

    mProjectModel->load(file);
    loadProject();

  } else {
    QByteArray ba = file.toLocal8Bit();
    const char *cfile = ba.data();
    msgWarning("Project file not found: %s", cfile);
  }
}

void MainWindowPresenter::deleteHistory()
{
  mSettingsModel->clearHistory();
  mStartPageWidget->setHistory(QStringList());
  mView->deleteHistory();
}

void MainWindowPresenter::saveProject()
{
  mProjectModel->save();
  AppStatus::instance().activeFlag(AppStatus::Flag::project_modified, false);
  TL_TODO("Quitar")
  //mView->setFlag(MainWindowView::Flag::project_modified, false);
}

void MainWindowPresenter::saveProjectAs()
{
  QString file = QFileDialog::getSaveFileName(Q_NULLPTR,
                                              tr("Save project as..."),
                                              mProjectDefaultPath,
                                              tr("Graphos Project (*.xml)"));
  if (file.isEmpty() == false) {
    mProjectModel->saveAs(file);
    AppStatus::instance().activeFlag(AppStatus::Flag::project_modified, false);
    TL_TODO("Quitar")
    //mView->setFlag(MainWindowView::Flag::project_modified, false);
  }
}

void MainWindowPresenter::closeProject()
{
  if(mProjectModel->checkUnsavedChanges()){
    int i_ret = QMessageBox(QMessageBox::Information,
                            tr("Save Changes"),
                            tr("There are unsaved changes. Do you want to save the changes before closing the project?"),
                            QMessageBox::Yes|QMessageBox::No|QMessageBox::Cancel).exec();
    if (i_ret == QMessageBox::Yes) {
      saveProject();
    } else if (i_ret == QMessageBox::Cancel) {
      return;
    }
  }

  mProjectModel->clear();
/////TODO:  mModel->finishLog();
  mView->clear();

  AppStatus::instance().clear();
}

void MainWindowPresenter::exit()
{
  ///TODO: Se cierra la aplicación
  /// - Comprobar que no haya ningún proceso corriendo
  if(mProjectModel->checkUnsavedChanges()){
    int i_ret = QMessageBox(QMessageBox::Information,
                            tr("Save Changes"),
                            tr("There are unsaved changes. Do you want to save the changes before closing the project?"),
                            QMessageBox::Yes|QMessageBox::No|QMessageBox::Cancel).exec();
    if (i_ret == QMessageBox::Yes) {
      saveProject();
    } else if (i_ret == QMessageBox::Cancel) {
      return;
    }
  }

  mView->close();
}

void MainWindowPresenter::openStartPage()
{
  initStartPage();

  const QSignalBlocker blocker(mTabHandler);
  int id = -1;
  for (int i = 0; i < mTabHandler->count(); i++){
    if (mTabHandler->tabText(i) == mStartPageWidget->windowTitle()){
      id = -1;
      mTabHandler->setCurrentIndex(id);
      break;
    }
  }

  if (id == -1){
    id = mTabHandler->addTab(mStartPageWidget, mStartPageWidget->windowTitle());
    mTabHandler->setCurrentIndex(id);
    mTabHandler->setTabToolTip(id, mStartPageWidget->windowTitle());
  }
}

void MainWindowPresenter::loadProject()
{
  mView->clear();

  mView->setProjectTitle(mProjectModel->projectName());
  //mView->setFlag(MainWindowView::Flag::project_exists, true);
  ///TODO: Borrar lo anterior cuando termine de refactorizar todos los componentes
  AppStatus::instance().activeFlag(AppStatus::Flag::project_exists, true);

  QString prjFile = mProjectModel->projectPath();

  /// Se añade al historial de proyectos recientes
  mSettingsModel->addToHistory(prjFile);
  mView->updateHistory(mSettingsModel->history());
  mStartPageWidget->setHistory(mSettingsModel->history());

  QString msg = tr("Load project: ").append(prjFile);
  mView->setStatusBarMsg(msg);
  QByteArray ba = prjFile.toLocal8Bit();
  const char *cfile = ba.data();
  msgInfo("Load project: %s", cfile);

  QStringList images;
  for(auto it = mModel->imageBegin(); it != mModel->imageEnd(); it++){
    images.push_back((*it).path());
  }

  if (images.size() > 0){
    mView->addImages(images);
    AppStatus::instance().activeFlag(AppStatus::Flag::images_added, true);
    TL_TODO("Quitar")
    //mView->setFlag(MainWindowView::Flag::images_added, true);
    //mView->setFlag(MainWindowView::Flag::loading_images, true);
    //connect(mView, SIGNAL(imagesLoaded()),   this,  SLOT(onLoadImages()));
  }

  for(auto it = mFeaturesModel->begin(); it != mFeaturesModel->end(); it++){
    this->loadFeatures(it->first);
  }

  this->loadMatches();
  this->loadOrientation();
  this->loadDenseModel();
}

void MainWindowPresenter::updateProject()
{
  AppStatus &app_status = AppStatus::instance();
  app_status.activeFlag(AppStatus::Flag::feature_extraction, false);
  app_status.activeFlag(AppStatus::Flag::feature_matching, false);
  app_status.activeFlag(AppStatus::Flag::oriented, false);
  app_status.activeFlag(AppStatus::Flag::absolute_oriented, false);
  app_status.activeFlag(AppStatus::Flag::dense_model, false);

  this->loadDenseModel();
  this->loadOrientation();
  this->loadMatches(); /// TODO: cambiar a update matches

  for (auto it = mModel->imageBegin(); it != mModel->imageEnd(); it++) {
    QString imageLeft = it->name();
    bool del_features = true;
    for (auto it = mFeaturesModel->begin(); it != mFeaturesModel->end(); it++) {
      if (imageLeft.compare(it->first) == 0) {
        this->loadFeatures(it->first);
        del_features = false;
        break;
      }
    }
    if (del_features) mView->deleteFeatures(imageLeft);
  }

  for(auto it = mFeaturesModel->begin(); it != mFeaturesModel->end(); it++){

  }

  //for(auto it = mModel->imageBegin(); it != mModel->imageEnd(); it++){
  //  QString imageLeft = it->name();
  //  std::vector<QString> pairs = mMatchesModel->matchesPairs(imageLeft);
  //  for (auto &imageRight : pairs){
  //    mView->deleteMatches(imageLeft);
  //    AppStatus::instance().activeFlag(AppStatus::Flag::feature_matching, true);
  //  }
  //}

}

void MainWindowPresenter::loadFeatures(const QString &featId)
{
  mView->addFeatures(featId);
  AppStatus::instance().activeFlag(AppStatus::Flag::feature_extraction, true);
}

void MainWindowPresenter::loadMatches()
{
  for(auto it = mModel->imageBegin(); it != mModel->imageEnd(); it++){
    QString imageLeft = it->name();
    std::vector<QString> pairs = mMatchesModel->matchesPairs(imageLeft);
    if (!pairs.empty()) {
      mView->addMatches(imageLeft);
      AppStatus::instance().activeFlag(AppStatus::Flag::feature_matching, true);
    } else {
      mView->deleteMatches(imageLeft);
    }
  }
}

void MainWindowPresenter::loadOrientation()
{
  TL_TODO("completar")
  QString sparse_model = mProjectModel->sparseModel();
  if (!sparse_model.isEmpty()){
    mView->setSparseModel(mProjectModel->sparseModel());
    TL_TODO("Por ahora lo añado aqui aunque hay que revisarlo")
    AppStatus &app_status = AppStatus::instance();
    app_status.activeFlag(AppStatus::Flag::oriented, true);
    if (mProjectModel->isAbsoluteOriented()) {
      app_status.activeFlag(AppStatus::Flag::absolute_oriented, true);
    }
  } else {
    mView->deleteSparseModel();
  }
}

void MainWindowPresenter::loadDenseModel()
{
  QString dense_model = mProjectModel->denseModel();
  if (!dense_model.isEmpty()) {
    mView->setDenseModel(mProjectModel->denseModel());
    AppStatus &app_status = AppStatus::instance();
    app_status.activeFlag(AppStatus::Flag::dense_model, true);
  } else {
    mView->deleteDenseModel();
  }
}

void MainWindowPresenter::openImage(const QString &imageName)
{
  try {
    Image image = mModel->findImageByName(imageName);
    mTabHandler->setImage(image.path());
  } catch (std::exception &e) {
    tl::MessageManager::release(e.what(), tl::MessageLevel::msg_error);
  }
}

void MainWindowPresenter::activeImage(const QString &imageName)
{
  try {
    Image image = mModel->findImageByName(imageName);
    std::list<std::pair<QString, QString>> properties = mModel->exif(image.path());
    mView->setProperties(properties);
    mView->setActiveImage(imageName);
  } catch (std::exception &e) {
    tl::MessageManager::release(e.what(), tl::MessageLevel::msg_error);
  }
}

void MainWindowPresenter::activeImages(const QStringList &imageNames)
{
  mView->setActiveImages(imageNames);
}

void MainWindowPresenter::deleteImages(const QStringList &imageNames)
{
  mModel->removeImages(imageNames);

  for (const auto &imageName : imageNames){
    mFeaturesModel->removeFeatures(imageName);
    mMatchesModel->removeMatchesPair(imageName);
    mView->deleteImage(imageName);
    msgInfo("Delete image %s", imageName.toStdString().c_str());
  }

  //mView->setFlag(MainWindowView::Flag::project_modified, true);
  //mView->setFlag(MainWindowView::Flag::images_added, mModel->imageBegin() != mModel->imageEnd());
  AppStatus::instance().activeFlag(AppStatus::Flag::project_modified, true);
  AppStatus::instance().activeFlag(AppStatus::Flag::images_added, mModel->imageBegin() != mModel->imageEnd());
}

//void MainWindowPresenter::deleteImage(const QString &imageName)
//{
//  try {
//    mFeaturesModel->removeFeatures(imageName);
//    mMatchesModel->removeMatchesPair(imageName);
//    /// TODO: Borrar los matches cuando este a la izquierda tambien
//    //size_t image_id = mModel->imageID(imageName);
//    //mModel->removeImage(image_id);
//    mModel->removeImage(imageName);
//    mView->deleteImage(imageName);
//  } catch (std::exception &e) {
//    msgError(e.what());
//  }
//}

void MainWindowPresenter::openImageMatches(const QString &sessionName, const QString &imgName1, const QString &imgName2)
{

}

void MainWindowPresenter::openModel3D(const QString &model3D, bool loadCameras)
{
  try {

    QFileInfo fileInfo(model3D);
    TL_ASSERT(fileInfo.exists(), "File not found")

    mTabHandler->setModel3D(model3D);
    
    // Load Cameras
    if (loadCameras) {
      for (auto image = mModel->imageBegin(); image != mModel->imageEnd(); image++) {
        QFileInfo(image->path()).fileName();
        QString name = image->name();
        QString file_name = QFileInfo(image->path()).fileName();
        if (mModel->isPhotoOriented(name)) {

          CameraPose photoOrientation = mModel->photoOrientation(name);

          std::array<double, 3> position;
          position[0] = photoOrientation.position.x;
          position[1] = photoOrientation.position.y;
          position[2] = photoOrientation.position.z;

          std::array<std::array<float, 3>, 3> cameraRotationMatrix;
          photoOrientation.rotation.at(0, 0);
          cameraRotationMatrix[0][0] = photoOrientation.rotation.at(0, 0);
          cameraRotationMatrix[0][1] = photoOrientation.rotation.at(0, 1);
          cameraRotationMatrix[0][2] = photoOrientation.rotation.at(0, 2);
          cameraRotationMatrix[1][0] = photoOrientation.rotation.at(1, 0);
          cameraRotationMatrix[1][1] = photoOrientation.rotation.at(1, 1);
          cameraRotationMatrix[1][2] = photoOrientation.rotation.at(1, 2);
          cameraRotationMatrix[2][0] = photoOrientation.rotation.at(2, 0);
          cameraRotationMatrix[2][1] = photoOrientation.rotation.at(2, 1);
          cameraRotationMatrix[2][2] = photoOrientation.rotation.at(2, 2);

          mTabHandler->addCamera(name, position, cameraRotationMatrix);

        }
      }
    }

  } catch (std::exception &e) {
    tl::MessageManager::release(e.what(), tl::MessageLevel::msg_error);
  }
}

void MainWindowPresenter::deleteFeatures()
{
  TL_TODO("completar")

  mFeaturesModel->clear();
}

void MainWindowPresenter::deleteMatches()
{
  TL_TODO("completar")
  mMatchesModel->clear();
}

void MainWindowPresenter::processFinished()
{
  //mView->setFlag(MainWindowView::Flag::processing, false);
  //mView->setFlag(MainWindowView::Flag::project_modified, true);

  AppStatus::instance().activeFlag(AppStatus::Flag::processing, false);
  AppStatus::instance().activeFlag(AppStatus::Flag::project_modified, true);
}

void MainWindowPresenter::processRunning()
{
  //mView->setFlag(MainWindowView::Flag::processing, true);

  AppStatus::instance().activeFlag(AppStatus::Flag::processing, true);
}

void MainWindowPresenter::processFailed()
{
}

void MainWindowPresenter::loadingImages(bool loading)
{
  AppStatus::instance().activeFlag(AppStatus::Flag::loading_images, loading);

  TL_TODO("Quitar")
  //mView->setFlag(MainWindowView::Flag::loading_images, loading);
}

void MainWindowPresenter::loadImage(const QString &image)
{
  mView->addImage(image);

  AppStatus::instance().activeFlag(AppStatus::Flag::images_added, true);

  TL_TODO("Quitar")
  //mView->setFlag(MainWindowView::Flag::images_added, true);
}

void MainWindowPresenter::onProjectModified()
{
  AppStatus::instance().activeFlag(AppStatus::Flag::project_modified, true);
  //mView->setFlag(MainWindowView::Flag::project_modified, true);
}

void MainWindowPresenter::help()
{
  if (mHelpDialog) {
    mHelpDialog->navigateHome();
    mHelpDialog->setModal(true);
    mHelpDialog->showMaximized();
  }
}

void MainWindowPresenter::open()
{
  mView->showMaximized();
}

void MainWindowPresenter::setHelp(HelpDialog *help)
{
  mHelpDialog = help;
}

void MainWindowPresenter::init()
{
  initDefaultPath();

  mTabHandler = mView->tabHandler();
  openStartPage(); /// Show Start Page

  /* Projects history */
  mView->updateHistory(mSettingsModel->history());
  mStartPageWidget->setHistory(mSettingsModel->history());

  bool bUseGPU = cudaEnabled(8.0, 5.0);
}

void MainWindowPresenter::initSignalAndSlots()
{

/* Menú Archivo */

  connect(mView, &MainWindowView::openProjectFromHistory, this, &MainWindowPresenter::openFromHistory);  ///TODO: falta test señal
  connect(mView, &MainWindowView::clearHistory,           this, &MainWindowPresenter::deleteHistory);
  connect(mView, &MainWindowView::openCamerasImport,      this, &MainWindowPresenter::openCamerasImportDialog);
  connect(mView, &MainWindowView::openExportOrientations, this, &MainWindowPresenter::openExportOrientationsDialog);
  connect(mView, &MainWindowView::openExportPointCloud,   this, &MainWindowPresenter::openExportPointCloudDialog);
  connect(mView, &MainWindowView::saveProject,            this, &MainWindowPresenter::saveProject);
  connect(mView, &MainWindowView::saveProjectAs,          this, &MainWindowPresenter::saveProjectAs);
  connect(mView, &MainWindowView::closeProject,           this, &MainWindowPresenter::closeProject);
  connect(mView, &MainWindowView::exit,                   this, &MainWindowPresenter::exit);

  /* Menú View */

  connect(mView,   &MainWindowView::openStartPage,        this, &MainWindowPresenter::openStartPage);

  /* Menú flujo de trabajo */

  /* Menú herramientas */

  connect(mView,   &MainWindowView::openSettings,         this, &MainWindowPresenter::openSettingsDialog);
  connect(mView,   &MainWindowView::openDtmDialog,         this, &MainWindowPresenter::openDtmDialog);

  /* Menú Ayuda */

  connect(mView, &MainWindowView::openHelpDialog,     this, &MainWindowPresenter::help);
  connect(mView, &MainWindowView::openAboutDialog,    this, &MainWindowPresenter::openAboutDialog);

  /* Panel de vistas en miniatura */

  connect(mView, SIGNAL(openImage(QString)),          this, SLOT(openImage(QString)));
  connect(mView, SIGNAL(selectImage(QString)),        this, SLOT(activeImage(QString)));
  connect(mView, SIGNAL(selectImages(QStringList)),   this, SLOT(activeImages(QStringList)));
  connect(mView, SIGNAL(deleteImages(QStringList)),   this, SLOT(deleteImages(QStringList)));

  /* Visor de imagenes */

  connect(mView, SIGNAL(openModel3D(QString, bool)),          this, SLOT(openModel3D(QString, bool)));

}

void MainWindowPresenter::initDefaultPath()
{
  mProjectDefaultPath = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
  mProjectDefaultPath.append("/graphos/Projects");

  QDir dir(mProjectDefaultPath);
  if (!dir.exists()) {
    dir.mkpath(".");
  }
}

void MainWindowPresenter::initStartPage()
{
  TL_TODO("revisar")
  if (mStartPageWidget == nullptr){
    mStartPageWidget = new StartPageWidget(mView);

    connect(mStartPageWidget,   &StartPageWidget::openNew,                this, &MainWindowPresenter::openCreateProjectDialog);
    connect(mStartPageWidget,   &StartPageWidget::openProject,            this, &MainWindowPresenter::openProjectDialog);
    connect(mStartPageWidget,   &StartPageWidget::openSettings,           this, &MainWindowPresenter::openSettingsDialog);
    connect(mStartPageWidget,   &StartPageWidget::clearHistory,           this, &MainWindowPresenter::deleteHistory);
    connect(mStartPageWidget,   &StartPageWidget::openProjectFromHistory, this, &MainWindowPresenter::openFromHistory);
  }
}

} // namespace ui

} // namespace graphos
