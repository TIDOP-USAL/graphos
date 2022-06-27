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

#include "MainWindowPresenter.h"

#include "graphos/core/Application.h"
#include "graphos/core/AppStatus.h"
#include "graphos/core/utils.h"
#include "graphos/widgets/StartPageWidget.h"
#include "graphos/widgets/TabWidget.h"
#include "graphos/widgets/GraphicViewer.h"
#include "graphos/widgets/Viewer3d.h"
#include "graphos/gui/MainWindowView.h"
#include "graphos/gui/MainWindowModel.h"

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
#include <QSettings>

namespace graphos
{


MainWindowPresenter::MainWindowPresenter(MainWindowView *view,
                                         MainWindowModel *model)
  : Presenter(),
    mView(view),
    mModel(model),
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
  try {

    if (QFileInfo(file).exists()) {

      // Se comprueba si hay un proyecto abierto con cambios sin guardar
      Application &app = Application::instance();
      AppStatus *app_status = app.status();

      if (app_status && app_status->isActive(AppStatus::Flag::project_modified)) {
        int i_ret = QMessageBox(QMessageBox::Information,
                                tr("Save Changes"),
                                tr("There are unsaved changes. Do you want to save them?"),
                                QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel).exec();
        if (i_ret == QMessageBox::Yes) {
          saveProject();
        } else if (i_ret == QMessageBox::Cancel) {
          return;
        }
      }

      mModel->clear();

      if (mModel->checkOldVersion(file)) {
        int i_ret = QMessageBox(QMessageBox::Information,
                                tr("It is loading an old project"),
                                tr("If you accept, a copy of the old project will be created"),
                                QMessageBox::Yes | QMessageBox::No).exec();
        if (i_ret == QMessageBox::Yes) {
          mModel->oldVersionBackup(file);
          Application &app = Application::instance();
          app.status()->activeFlag(AppStatus::Flag::project_modified, true);
        } else if (i_ret == QMessageBox::Cancel) {
          return;
        }

      }

      mModel->load(file);
      loadProject();

    } else {
      QByteArray ba = file.toLocal8Bit();
      const char *cfile = ba.data();
      msgWarning("Project file not found: %s", cfile);
    }

  } catch (std::exception &e) {
    tl::printException(e);
  }
}

void MainWindowPresenter::deleteHistory()
{
  Application::instance().clearHistory();
  mStartPageWidget->setHistory(QStringList());
  mView->deleteHistory();
}

void MainWindowPresenter::saveProject()
{
  try {
    
    mModel->save();
    
    Application &app = Application::instance();
    app.status()->activeFlag(AppStatus::Flag::project_modified, false);
 
  } catch (std::exception &e) {
    tl::printException(e);
  }
}

void MainWindowPresenter::saveProjectAs()
{
  try {

    QString file = QFileDialog::getSaveFileName(Q_NULLPTR,
                                                tr("Save project as..."),
                                                mProjectDefaultPath,
                                                tr("Graphos Project (*.xml)"));
    if (file.isEmpty() == false) {
      mModel->saveAs(file);
      Application &app = Application::instance();
      app.status()->activeFlag(AppStatus::Flag::project_modified, false);
    }

  } catch (std::exception &e) {
    tl::printException(e);
  }
}

void MainWindowPresenter::closeProject()
{
  if(mModel->checkUnsavedChanges()){
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

  mModel->clear();
/////TODO:  mModel->finishLog();
  mView->clear();

  Application &app = Application::instance();
  app.status()->clear();
}

void MainWindowPresenter::exit()
{
  ///TODO: Se cierra la aplicación
  /// - Comprobar que no haya ningún proceso corriendo
  
  Application &app = Application::instance();
  AppStatus *app_status = app.status();

  if(app_status && app_status->isActive(AppStatus::Flag::project_modified)){
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

}

void MainWindowPresenter::openStartPage()
{
  initStartPage();

  auto tab = mView->tabWidget();

  const QSignalBlocker blocker(tab);
  int id = -1;
  for (int i = 0; i < tab->count(); i++){
    if (tab->tabText(i) == mStartPageWidget->windowTitle()){
      id = -1;
      tab->setCurrentIndex(id);
      break;
    }
  }

  if (id == -1){
    id = tab->addTab(mStartPageWidget, mStartPageWidget->windowTitle());
    tab->setCurrentIndex(id);
    tab->setTabToolTip(id, mStartPageWidget->windowTitle());
  }

  AppStatus *status = Application::instance().status();
  status->activeFlag(AppStatus::Flag::tab_image_active, false);
  status->activeFlag(AppStatus::Flag::tab_3d_model_active, false);

}

void MainWindowPresenter::loadProject()
{
  mView->clear();

  mView->setProjectTitle(mModel->projectName());
  Application &app = Application::instance();
  app.status()->activeFlag(AppStatus::Flag::project_exists, true);

  QString project_path = mModel->projectPath();

  /// Se añade al historial de proyectos recientes
  app.addToHistory(project_path);
  mView->updateHistory(app.history());
  mStartPageWidget->setHistory(app.history());

  QString msg = tr("Load project: ").append(project_path);
  mView->setStatusBarMsg(msg);
  QByteArray ba = project_path.toLocal8Bit();
  const char *cfile = ba.data();
  msgInfo("Load project: %s", cfile);

  for (const auto &image : mModel->images()) {
    mView->addImage(image.second.path(), image.second.id());
    app.status()->activeFlag(AppStatus::Flag::images_added, true);
  }
  //QStringList images;
  //for(auto it = mModel->imageBegin(); it != mModel->imageEnd(); it++){
  //  images.push_back((*it).path());
  //}

  //if (images.size() > 0){
  //  mView->addImages(images);
  //  Application &app = Application::instance();
  //  app.status()->activeFlag(AppStatus::Flag::images_added, true);
  //}

  for (const auto &feat : mModel->features()) {
    this->loadFeatures(feat.first);
  }

  //for(auto it = mFeaturesModel->begin(); it != mFeaturesModel->end(); it++){
  //  this->loadFeatures(it->first);
  //}

  this->updateMatches();
  this->loadOrientation();
  this->loadDenseModel();
  //this->loadDTM();
  //this->loadOrtho();
}

void MainWindowPresenter::updateProject()
{
  Application &app = Application::instance();
  AppStatus *app_status = app.status();
  //app_status->activeFlag(AppStatus::Flag::feature_extraction, false);
  //app_status->activeFlag(AppStatus::Flag::feature_matching, false);
  //app_status->activeFlag(AppStatus::Flag::oriented, false);
  //app_status->activeFlag(AppStatus::Flag::absolute_oriented, false);
  //app_status->activeFlag(AppStatus::Flag::dense_model, false);
  //app_status->activeFlag(AppStatus::Flag::dtm, false);
  //app_status->activeFlag(AppStatus::Flag::ortho, false);

  //this->loadOrtho();
  //this->loadDTM();
  this->loadDenseModel();
  this->loadOrientation();
  this->updateMatches();

  //for (auto it = mModel->imageBegin(); it != mModel->imageEnd(); it++) {
  //  QString imageLeft = it->name();
  //  bool del_features = true;
  //  for (auto it = mFeaturesModel->begin(); it != mFeaturesModel->end(); it++) {
  //    if (imageLeft.compare(it->first) == 0) {
  //      this->loadFeatures(it->first);
  //      del_features = false;
  //      break;
  //    }
  //  }
  //  if (del_features) mView->deleteFeatures(imageLeft);
  //}

  //for(auto it = mFeaturesModel->begin(); it != mFeaturesModel->end(); it++){

  //}

  ////for(auto it = mModel->imageBegin(); it != mModel->imageEnd(); it++){
  ////  QString imageLeft = it->name();
  ////  std::vector<QString> pairs = mMatchesModel->matchesPairs(imageLeft);
  ////  for (auto &imageRight : pairs){
  ////    mView->deleteMatches(imageLeft);
  ////    AppStatus::instance().activeFlag(AppStatus::Flag::feature_matching, true);
  ////  }
  ////}

}

void MainWindowPresenter::loadFeatures(size_t imageId)
{
  mView->addFeatures(imageId);
  Application &app = Application::instance();
  app.status()->activeFlag(AppStatus::Flag::feature_extraction, true);
}

void MainWindowPresenter::updateMatches()
{
  Application &app = Application::instance();
  
  app.status()->activeFlag(AppStatus::Flag::feature_matching, false);

  for (const auto &image : mModel->images()){
    size_t image_left_id = image.first;
    std::vector<size_t> pairs = mModel->imagePairs(image_left_id);
    if (!pairs.empty()) {
      mView->addMatches(image_left_id);
      app.status()->activeFlag(AppStatus::Flag::feature_matching, true);
    } else {
      mView->deleteMatches(image_left_id);
    }
  }
}

void MainWindowPresenter::loadOrientation()
{
  TL_TODO("completar")
  
  Application &app = Application::instance();
  AppStatus *app_status = app.status();

  QString sparse_model = mModel->sparseModel();

  if (!sparse_model.isEmpty()){
    mView->setSparseModel(sparse_model);
    TL_TODO("Por ahora lo añado aqui aunque hay que revisarlo")

    app_status->activeFlag(AppStatus::Flag::oriented, true);
    app_status->activeFlag(AppStatus::Flag::absolute_oriented, mModel->isAbsoluteOrientation());
  } else {
    mView->deleteSparseModel();
    app_status->activeFlag(AppStatus::Flag::oriented, false);
    app_status->activeFlag(AppStatus::Flag::absolute_oriented, false);
  }
}

void MainWindowPresenter::loadDenseModel()
{
  Application &app = Application::instance();

  QString dense_model = mModel->denseModel();

  if (!dense_model.isEmpty()) {
    mView->setDenseModel(dense_model);
    
    app.status()->activeFlag(AppStatus::Flag::dense_model, true);
  } else {
    mView->deleteDenseModel();
    app.status()->activeFlag(AppStatus::Flag::dense_model, false);
  }
}

void MainWindowPresenter::loadDTM()
{
//  QString dtm = mProjectModel->dtm();
//  if (!dtm.isEmpty()) {
//    mView->setDSM(dtm);
//    Application &app = Application::instance();
//    app.status()->activeFlag(AppStatus::Flag::dtm, true);
//  } else {
//    mView->deleteDsm();
//  }
}

void MainWindowPresenter::loadOrtho()
{
  //QString ortho = mProjectModel->ortho();
  //if (!ortho.isEmpty()) {
  //  mView->setOrtho(ortho);
  //  Application& app = Application::instance();
  //  app.status()->activeFlag(AppStatus::Flag::ortho, true);
  //} else {
  //  mView->deleteOrtho();
  //}
}

void MainWindowPresenter::openImage(size_t imageId)
{
  try {

    auto tab_widget = mView->tabWidget();
    QString image = mModel->image(imageId).path();
    
    int tab_id = tab_widget->fileTab(image);

    if (tab_id != -1) {
      tab_widget->setCurrentIndex(tab_id);
    } else {
      GraphicViewer *graphic_viewer = new GraphicViewerImp(mView);
      graphic_viewer->setImage(mModel->readImage(imageId));
      tab_id = tab_widget->addTab(graphic_viewer, QFileInfo(image).fileName());
      tab_widget->setCurrentIndex(tab_id);
      tab_widget->setTabToolTip(tab_id, image);

      graphic_viewer->zoomExtend();
    }

    AppStatus *status = Application::instance().status();
    status->activeFlag(AppStatus::Flag::tab_image_active, true);
    status->activeFlag(AppStatus::Flag::tab_3d_model_active, false);

  } catch (std::exception &e) {
    tl::printException(e);
  }


}

void MainWindowPresenter::activeImage(size_t imageId)
{
  try {

    const Image &image = mModel->image(imageId);
    std::list<std::pair<QString, QString>> properties = mModel->exif(image.path());
    mView->setProperties(properties);
    mView->setActiveImage(imageId);

  } catch (std::exception &e) {
    tl::printException(e);
  }
}

void MainWindowPresenter::activeImages(const std::vector<size_t> &imageIds)
{
  mView->setActiveImages(imageIds);
}

void MainWindowPresenter::deleteImages(const std::vector<size_t> &imageIds)
{
  try {

    mModel->deleteImages(imageIds);
    mView->deleteImages(imageIds);
    //for (const auto &imageName : imageNames){
    //  mFeaturesModel->removeFeatures(imageName);
    //  mMatchesModel->removeMatchesPair(imageName);
    //  mView->deleteImage(imageName);
    //  msgInfo("Delete image %s", imageName.toStdString().c_str());
    //}
    
    Application &app = Application::instance();
    AppStatus *app_status = app.status();
    app_status->activeFlag(AppStatus::Flag::project_modified, true);
    app_status->activeFlag(AppStatus::Flag::images_added, mModel->images().size() != 0);

  } catch (std::exception &e) {
    tl::printException(e);
  }
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

void MainWindowPresenter::open3DModel(const QString &model3D, 
                                      bool loadCameras)
{
  try {

    QFileInfo fileInfo(model3D);
    TL_ASSERT(fileInfo.exists(), "File not found");

    auto tab_widget = mView->tabWidget();
    int tab_id = tab_widget->fileTab(model3D);

    if (tab_id != -1) {
      tab_widget->setCurrentIndex(tab_id);
    } else {
      CCViewer3D *viewer3D = new CCViewer3D();
      viewer3D->loadFromFile(model3D);
      tab_id = tab_widget->addTab(dynamic_cast<QWidget *>(viewer3D), fileInfo.fileName());
      tab_widget->setCurrentIndex(tab_id);
      tab_widget->setTabToolTip(tab_id, model3D);

      viewer3D->setGlobalZoom();

      //mTabHandler->setModel3D(model3D);

      // Load Cameras
      if (loadCameras) {
        TL_TODO("Reemplazar por poses() y quitar isPhotoOriented()")
          for (const auto &image : mModel->images()) {

            size_t image_id = image.first;
            QString name = image.second.name();

            if (mModel->isPhotoOriented(image_id)) {

              CameraPose photoOrientation = mModel->cameraPose(image_id);

              std::array<double, 3> position;
              position[0] = photoOrientation.position().x;
              position[1] = photoOrientation.position().y;
              position[2] = photoOrientation.position().z;

              std::array<std::array<float, 3>, 3> cameraRotationMatrix;
              cameraRotationMatrix[0][0] = static_cast<float>(photoOrientation.rotationMatrix().at(0, 0));
              cameraRotationMatrix[0][1] = static_cast<float>(photoOrientation.rotationMatrix().at(0, 1));
              cameraRotationMatrix[0][2] = static_cast<float>(photoOrientation.rotationMatrix().at(0, 2));
              cameraRotationMatrix[1][0] = static_cast<float>(photoOrientation.rotationMatrix().at(1, 0));
              cameraRotationMatrix[1][1] = static_cast<float>(photoOrientation.rotationMatrix().at(1, 1));
              cameraRotationMatrix[1][2] = static_cast<float>(photoOrientation.rotationMatrix().at(1, 2));
              cameraRotationMatrix[2][0] = static_cast<float>(photoOrientation.rotationMatrix().at(2, 0));
              cameraRotationMatrix[2][1] = static_cast<float>(photoOrientation.rotationMatrix().at(2, 1));
              cameraRotationMatrix[2][2] = static_cast<float>(photoOrientation.rotationMatrix().at(2, 2));

              //mTabHandler->addCamera(name, position, cameraRotationMatrix);
              viewer3D->addCamera(name, position[0], position[1], position[2], cameraRotationMatrix);

            }
          }
      }
    }

    AppStatus *status = Application::instance().status();
    status->activeFlag(AppStatus::Flag::tab_image_active, false);
    status->activeFlag(AppStatus::Flag::tab_3d_model_active, true);

  } catch (std::exception &e) {
    tl::MessageManager::release(e.what(), tl::MessageLevel::msg_error);
  }
}

void MainWindowPresenter::openDtm()
{
  try {
    //mTabHandler->setImage(mProjectModel->dtm());
  } catch (std::exception &e) {
    tl::MessageManager::release(e.what(), tl::MessageLevel::msg_error);
  }
}

void MainWindowPresenter::deleteFeatures()
{
//  TL_TODO("completar")
//
//  mFeaturesModel->clear();
}

void MainWindowPresenter::deleteMatches()
{
//  TL_TODO("completar")
//  mMatchesModel->clear();
}

//void MainWindowPresenter::processFinished()
//{
//  Application &app = Application::instance();
//  AppStatus *app_status = app.status();
//
//  app_status->activeFlag(AppStatus::Flag::processing, false);
//  app_status->activeFlag(AppStatus::Flag::project_modified, true);
//}
//
//void MainWindowPresenter::processRunning()
//{
//  //mView->setFlag(MainWindowView::Flag::processing, true);
//
//  Application &app = Application::instance();
//  app.status()->activeFlag(AppStatus::Flag::processing, true);
//}
//
//void MainWindowPresenter::processFailed()
//{
//}

void MainWindowPresenter::loadingImages(bool loading)
{
  Application &app = Application::instance();
  app.status()->activeFlag(AppStatus::Flag::loading_images, loading);
}

/// Refactorizado
void MainWindowPresenter::loadImage(size_t imageId)
{
  try {

    const Image &image = mModel->image(imageId);
    mView->addImage(image.path(), image.id());

    Application &app = Application::instance();
    app.status()->activeFlag(AppStatus::Flag::images_added, true);

  } catch (std::exception &e) {
    tl::printException(e);
  }
}

void MainWindowPresenter::onProjectModified()
{
  Application &app = Application::instance();
  app.status()->activeFlag(AppStatus::Flag::project_modified, true);
}

void MainWindowPresenter::open()
{
  mView->showMaximized();
}

void MainWindowPresenter::init()
{
  initDefaultPath();

  openStartPage(); /// Show Start Page

  /* Projects history */
  Application &app = Application::instance();
  mView->updateHistory(app.history());
  mStartPageWidget->setHistory(app.history());

  bool bUseGPU = cudaEnabled(10.0, 3.0);
  QSettings settings(QSettings::IniFormat, QSettings::UserScope, "TIDOP", "Graphos");
  settings.setValue("UseCuda", settings.value("UseCuda", bUseGPU).toBool());
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

  //connect(mView,   &MainWindowView::openSettings,         this, &MainWindowPresenter::openSettingsDialog);
  //connect(mView,   &MainWindowView::openDtmDialog,         this, &MainWindowPresenter::openDtmDialog);

  /* Menú Ayuda */

  //connect(mView, &MainWindowView::openHelpDialog,     this, &MainWindowPresenter::help);
  connect(mView, &MainWindowView::openHelpDialog, [&]() {
    emit help("");
  });

  //connect(mView, &MainWindowView::openAboutDialog,    this, &MainWindowPresenter::openAboutDialog);

  /* Panel de vistas en miniatura */

  connect(mView, &MainWindowView::openImage,          this, &MainWindowPresenter::openImage);
  connect(mView, SIGNAL(selectImage(size_t)),        this, SLOT(activeImage(size_t)));
  connect(mView, SIGNAL(selectImages(std::vector<size_t>)),   this, SLOT(activeImages(std::vector<size_t>)));
  connect(mView, SIGNAL(delete_images(std::vector<size_t>)),   this, SLOT(deleteImages(std::vector<size_t>)));

  /* Visor de imagenes */

  connect(mView, SIGNAL(open3DModel(QString, bool)),          this, SLOT(open3DModel(QString, bool)));

  /* Visor DTM/DSM */

  connect(mView, SIGNAL(openDtm()), this, SLOT(openDtm()));

  connect(mView, &MainWindowView::allTabsClosed, []() { 
    AppStatus *status = Application::instance().status();
    status->activeFlag(AppStatus::Flag::tab_image_active, false);
    status->activeFlag(AppStatus::Flag::tab_3d_model_active, false);
  });
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
    //connect(mStartPageWidget,   &StartPageWidget::openSettings,           this, &MainWindowPresenter::openSettingsDialog);
    connect(mStartPageWidget,   &StartPageWidget::clearHistory,           this, &MainWindowPresenter::deleteHistory);
    connect(mStartPageWidget,   &StartPageWidget::openProjectFromHistory, this, &MainWindowPresenter::openFromHistory);
  }
}

} // namespace graphos
