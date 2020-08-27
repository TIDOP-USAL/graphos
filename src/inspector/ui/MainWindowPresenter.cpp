#include "MainWindowPresenter.h"

#include "inspector/ui/MainWindowView.h"
#include "inspector/ui/MainWindowModel.h"
#include "inspector/ui/ProjectModel.h"
#include "inspector/ui/SettingsModel.h"
#include "inspector/ui/SettingsPresenter.h"
#include "inspector/ui/SettingsView.h"
#include "inspector/ui/utils/TabHandler.h"
//#include "inspector/ui/utils/GraphicViewer.h"
#include "inspector/ui/HelpDialog.h"
#include "inspector/widgets/StartPageWidget.h"
#include "inspector/ui/cameras/CamerasModel.h"
#include "inspector/ui/images/ImagesModel.h"
#include "inspector/ui/FeaturesModel.h"
#include "inspector/ui/MatchesModel.h"
#include "inspector/ui/orientation/OrientationModel.h"
#include "inspector/core/utils.h"

/* TidopLib */
#include <tidop/core/messages.h>

/* Qt */
#include <QFileDialog>
#include <QMessageBox>
#include <QDesktopServices>
#include <QUrl>
#include <QSqlQuery>
#include <QSqlError>
#include <QApplication>

namespace inspector
{

namespace ui
{

MainWindowPresenter::MainWindowPresenter(MainWindowView *view,
                                         MainWindowModel *model,
                                         ProjectModel *projectModel,
                                         SettingsModel *settingsModel,
                                         ImagesModel *imagesModel,
                                         CamerasModel *camerasModel,
                                         FeaturesModel *featuresModel,
                                         MatchesModel *matchesModel,
                                         OrientationModel *orientationModel)
  : IPresenter(),
    mView(view),
    mModel(model),
    mProjectModel(projectModel),
    mSettingsModel(settingsModel),
    mImagesModel(imagesModel),
    mCamerasModel(camerasModel),
    mFeaturesModel(featuresModel),
    mMatchesModel(matchesModel),
    mOrientationModel(orientationModel),
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

void MainWindowPresenter::openNew()
{
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

  mView->clear();

  emit openNewProjectDialog();
}

void MainWindowPresenter::openProject()
{
  QString file = QFileDialog::getOpenFileName(Q_NULLPTR,
                                              tr("Open Inspector Image Project"),
                                              mProjectDefaultPath,
                                              tr("Inspector Image Project (*.xml)"));
  if (!file.isEmpty()) {
    // Se comprueba si hay un proyecto abierto con cambios sin guardar
    if (mProjectModel->checkUnsavedChanges()) {
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
        mView->setFlag(MainWindowView::Flag::project_modified, true);
      } else if (i_ret == QMessageBox::Cancel) {
        return;
      }

    }
    mProjectModel->load(file);
    ///TODO: o cambiar el nombre o hacerlo de otra forma
    loadProject();
  }
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
        mView->setFlag(MainWindowView::Flag::project_modified, true);
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
  mView->setFlag(MainWindowView::Flag::project_modified, false);
}

void MainWindowPresenter::saveProjectAs()
{
  QString file = QFileDialog::getSaveFileName(Q_NULLPTR,
                                              tr("Save project as..."),
                                              mProjectDefaultPath,
                                              tr("Inspector Image Project (*.xml)"));
  if (file.isEmpty() == false) {
    mProjectModel->saveAs(file);
    mView->setFlag(MainWindowView::Flag::project_modified, false);
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

//void MainWindowPresenter::openKeypointsViewer()
//{
//  emit openKeypointsViewerDialogFromSession(mProjectModel->currentSession()->name());
//}

//void MainWindowPresenter::openMatchesViewer()
//{
//  emit openMatchesViewerDialogFromSession(mProjectModel->currentSession()->name());
//}

///TODO: borrar
//void MainWindowPresenter::loadImages()
//{
//  QStringList fileNames = QFileDialog::getOpenFileNames(Q_NULLPTR,
//                                                        tr("Add images"),
//                                                        mProjectModel->projectFolder(),
//                                                        tr("Image files (*.tif *.tiff *.jpg *.png);;TIFF (*.tif *.tiff);;png (*.png);;JPEG (*.jpg)"));
//  if (fileNames.empty()) return;
//
//  TL_TODO("Esto tiene que ser un proceso")
//  tl::Chrono chrono("Load Images");
//  chrono.run();
//
//  QStringList images;
//  colmap::Bitmap bitmap;
//
//  for (auto &image : fileNames){
//    try {
////      int id_camera = mCamerasModel->addCamera(image);
////      mImagesModel->addImage(image, id_camera);
////      images.push_back(image);
////      //mView->addImage(image);
//
//      int id_camera = 0;
//      if (!bitmap.Read(image.toStdString(), false)) {
//        throw std::runtime_error("  Failed to read image file");
//      }
//
//      int width = bitmap.Width();
//      int height = bitmap.Height();
//
//      Camera camera;
//
//      std::string camera_make;
//      std::string camera_model;
//
//      if (bitmap.ReadExifTag(FIMD_EXIF_MAIN, "Make", &camera_make) &&
//          bitmap.ReadExifTag(FIMD_EXIF_MAIN, "Model", &camera_model)) {
//
//        msgInfo(" - Camera: %s %s", camera_make.c_str(), camera_model.c_str());
//
//        id_camera = mCamerasModel->cameraID(camera_make.c_str(), camera_model.c_str());
//        if (id_camera == 0){  /// Se añade la cámara al proyecto
//
//          camera = Camera(camera_make.c_str(), camera_model.c_str());
//
//          camera.setWidth(width);
//          camera.setHeight(height);
//
//          double focal;
//          if (bitmap.ExifFocalLength(&focal)){
//            camera.setFocal(focal);
//          } else {
//            camera.setFocal(1.2 * std::max(width, height));
//          }
//
//          QSqlDatabase database_cameras = QSqlDatabase::addDatabase("QSQLITE");
//          QString database_cameras_path;
//#ifdef _DEBUG
//          database_cameras_path = QString(INSPECTOR_SOURCE_PATH).append("/res");
//#else
//          database_cameras_path = qApp->applicationDirPath();
//#endif
//          database_cameras_path.append("/cameras.db");
//          database_cameras.setDatabaseName(database_cameras_path);
//          bool db_open = false;
//          if (QFileInfo(database_cameras_path).exists()){
//            db_open = database_cameras.open();
//          } else {
//            msgError("The camera database does not exist");
//          }
//
//          /// Lectura del tamaño del sensor de la base de datos
//          if (db_open) {
//            double sensor_width_px = std::max(bitmap.Width(), bitmap.Height());
//            double sensor_width_mm = -1;
//            double scale = 1.0;
//
//            QSqlQuery query;
//            query.prepare("SELECT id_camera FROM cameras WHERE camera_make LIKE :camera_make LIMIT 1");
//            query.bindValue(":camera_make", camera_make.c_str());
//            if (query.exec()){
//              while(query.next()){
//                id_camera = query.value(0).toInt();
//              }
//            } else {
//              QSqlError err = query.lastError();
//              throw err.text().toStdString();
//            }
//
//            if (id_camera == -1) {
//              msgWarning("Camera '%s' not found in database", camera_make.c_str());
//            } else {
//              query.prepare("SELECT sensor_width FROM models WHERE camera_model LIKE :camera_model AND id_camera LIKE :id_camera");
//              query.bindValue(":camera_model", camera_model.c_str());
//              query.bindValue(":id_camera", id_camera);
//              if (query.exec()){
//                while(query.next()){
//                  sensor_width_mm = query.value(0).toDouble();
//                  scale = sensor_width_mm / sensor_width_px;
//                  camera.setSensorSize(sensor_width_mm);
//
//                  ///TODO: Focal en mm?
//                  //msgInfo("Sensor size for camera %s %s is %f (mm)", camera_make.c_str(), camera_model.c_str(), sensor_width_mm);
//                }
//
//                if (sensor_width_mm < 0.){
//                  msgWarning("Camera model (%s %s) not found in database", camera_make.c_str(), camera_model.c_str());
//                }
//
//              } else {
//                QSqlError err = query.lastError();
//                msgWarning("%s", err.text().toStdString().c_str());
//              }
//            }
//
//          }
//
//          if (db_open) database_cameras.close();
//
//          id_camera = mCamerasModel->addCamera(camera);
//        }
//
//      } else {
//        /// Camara desconocida
//        msgWarning("Unknow camera for image: %s", image.toStdString().c_str());
//        Camera camera2;
//        bool bFound = false;
//        //int id_camera;
//        int counter = 0;
//        for (auto it = mCamerasModel->begin(); it != mCamerasModel->end(); it++) {
//          camera2 = it->second;
//          if (camera2.make().compare("Unknown camera") == 0){
//            if (camera2.width() == width && camera2.height() == height) {
//              // Misma camara
//              id_camera = it->first;
//              bFound = true;
//              break;
//            }
//            counter++;
//          }
//        }
//
//        if (bFound == false){
//          camera = Camera("Unknown camera", QString::number(counter));
//          camera.setWidth(width);
//          camera.setHeight(height);
//          camera.setFocal(1.2 * std::max(width, height));
//          id_camera = mCamerasModel->addCamera(camera);
//          camera.setModel(QString::number(id_camera));
//        }
//
//      }
//
//      Image img(image);
//      double lon;
//      if (bitmap.ExifLongitude(&lon)){
//        img.setLongitudeExif(lon);
//      }
//      double lat;
//      if (bitmap.ExifLatitude(&lat)){
//        img.setLatitudeExif(lat);
//      }
//      double altitude;
//      if (bitmap.ExifAltitude(&altitude)){
//        img.setAltitudeExif(altitude);
//      }
//
//      msgInfo(" - Coordinates: (%.4lf, %.4lf, %.2lf)", img.longitudeExif(), img.latitudeExif(), img.altitudeExif());
//
//      img.setCameraId(id_camera);
//      mImagesModel->addImage(img);
//
//      images.push_back(image);
//    } catch (std::exception &e) {
//      msgError(e.what());
//    }
//  }
//
//  uint64_t time = chrono.stop();
//  msgInfo("[Time: %f seconds]", time/1000.);
//
//  mView->addImages(images);
//
//  //msgInfo("Images Loaded");
//
//
//  mView->setFlag(MainWindowView::Flag::project_modified, true);
//  mView->setFlag(MainWindowView::Flag::images_added, true);
//  mView->setFlag(MainWindowView::Flag::loading_images, true);
//
//  emit openCamerasDialog();
//
//  connect(mView, SIGNAL(imagesLoaded()),   this,  SLOT(onLoadImages()));
//}

void MainWindowPresenter::loadProject()
{
  mView->clear();

  mView->setProjectTitle(mProjectModel->projectName());
  mView->setFlag(MainWindowView::Flag::project_exists, true);

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
  for(auto it = mImagesModel->begin(); it != mImagesModel->end(); it++){
    images.push_back((*it).path());
  }

  if (images.size() > 0){
    mView->addImages(images);
    mView->setFlag(MainWindowView::Flag::images_added, true);
    mView->setFlag(MainWindowView::Flag::loading_images, true);
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

}

void MainWindowPresenter::loadFeatures(const QString &featId)
{
  mView->addFeatures(featId);
  mView->setFlag(MainWindowView::Flag::feature_extraction, true);
}

void MainWindowPresenter::loadMatches()
{
  for(auto it = mImagesModel->begin(); it != mImagesModel->end(); it++){
    QString imageLeft = it->name();
    std::vector<QString> pairs = mMatchesModel->matchesPairs(imageLeft);
    for (auto &imageRight : pairs){
      mView->addMatches(imageLeft, imageRight);
      mView->setFlag(MainWindowView::Flag::feature_matching, true);
    }
  }
}

void MainWindowPresenter::loadOrientation()
{
  TL_TODO("completar")
  QString sparse_model = mProjectModel->sparseModel();
  if (!sparse_model.isEmpty()){
    mView->setSparseModel(mProjectModel->sparseModel());
    mView->setFlag(MainWindowView::Flag::oriented, true);
  }
}

void MainWindowPresenter::loadDenseModel()
{
  QString dense_model = mProjectModel->denseModel();
  if (!dense_model.isEmpty()) {
    mView->setDenseModel(mProjectModel->denseModel());
    QByteArray ba = mProjectModel->denseModel().toLocal8Bit();
  }
}

void MainWindowPresenter::openImage(const QString &imageName)
{
  try {
    Image image = mImagesModel->findImageByName(imageName);
    mTabHandler->setImage(image.path());
  } catch (std::exception &e) {
    tl::MessageManager::release(e.what(), tl::MessageLevel::msg_error);
  }
}

void MainWindowPresenter::activeImage(const QString &imageName)
{
  try {
    Image image = mImagesModel->findImageByName(imageName);
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
  for (const auto &imageName : imageNames){
    this->deleteImage(imageName);
  }

  mView->setFlag(MainWindowView::Flag::project_modified, true);
  mView->setFlag(MainWindowView::Flag::images_added, mImagesModel->begin() != mImagesModel->end());
}

void MainWindowPresenter::deleteImage(const QString &imageName)
{
  try {
    size_t image_id = mImagesModel->imageID(imageName);
    mImagesModel->removeImage(image_id);
    mView->deleteImage(imageName);
    TL_TODO("Se tienen que eliminar de la vista las imagenes procesadas, y los ficheros de keypoints y de matches")
  } catch (std::exception &e) {
    msgError(e.what());
  }
}

//void MainWindowPresenter::selectFeatures(const QString &session)
//{
//  std::shared_ptr<Session> _session = mProjectModel->findSession(session);
//  std::shared_ptr<Feature> detector = _session->detector();
//  std::shared_ptr<Feature> descriptor = _session->descriptor();

//  QString detector_name = detector->name();
//  QString descriptor_name = descriptor->name();

//  std::list<std::pair<QString, QString>> properties;
//  properties.push_back(std::make_pair(QString("Detector"), detector_name));
//  properties.push_back(std::make_pair(QString("Descriptor"), descriptor_name));
//  mView->setProperties(properties);
//}

//void MainWindowPresenter::selectDetector(const QString &session)
//{
//  std::shared_ptr<Session> _session = mProjectModel->findSession(session);
//  std::shared_ptr<Feature> detector = _session->detector();

//  std::list<std::pair<QString, QString>> properties;
//  if (detector->type() == Feature::Type::agast){
//    Agast *agast = dynamic_cast<Agast *>(detector.get());
//    properties.push_back(std::make_pair(QString("Name"), QString("AGAST")));
//    properties.push_back(std::make_pair(QString("Threshold"), QString(agast->threshold())));
//    properties.push_back(std::make_pair(QString("Nonmax Suppression"), agast->nonmaxSuppression() ? "true" : "false"));
//    properties.push_back(std::make_pair(QString("Detector Type"), QString(agast->detectorType())));
//  } else if (detector->type() == Feature::Type::akaze){
//    Akaze *akaze = dynamic_cast<Akaze *>(detector.get());
//    properties.push_back(std::make_pair(QString("Name"), QString("AKAZE")));
//    properties.push_back(std::make_pair(QString("Descriptor Type"), QString(akaze->descriptorType())));
//    properties.push_back(std::make_pair(QString("Descriptor Size"), QString::number(akaze->descriptorSize())));
//    properties.push_back(std::make_pair(QString("Descriptor Channels"), QString::number(akaze->descriptorChannels())));
//    properties.push_back(std::make_pair(QString("Threshold"), QString::number(akaze->threshold())));
//    properties.push_back(std::make_pair(QString("Octaves"), QString::number(akaze->octaves())));
//    properties.push_back(std::make_pair(QString("Octave Layers"), QString::number(akaze->octaveLayers())));
//    properties.push_back(std::make_pair(QString("Diffusivity"), akaze->diffusivity()));
//  } else if (detector->type() == Feature::Type::brisk){
//    Brisk *brisk = dynamic_cast<Brisk *>(detector.get());
//    properties.push_back(std::make_pair(QString("Name"), QString("BRISK")));
//    properties.push_back(std::make_pair(QString("Threshold"), QString::number(brisk->threshold())));
//    properties.push_back(std::make_pair(QString("Octaves"), QString::number(brisk->octaves())));
//    properties.push_back(std::make_pair(QString("Pattern Scale"), QString::number(brisk->patternScale())));
//  } else if (detector->type() == Feature::Type::fast){
//    Fast *fast = dynamic_cast<Fast *>(detector.get());
//    properties.push_back(std::make_pair(QString("Name"), QString("FAST")));
//    properties.push_back(std::make_pair(QString("Threshold"), QString::number(fast->threshold())));
//    properties.push_back(std::make_pair(QString("Nonmax Suppression"), fast->nonmaxSuppression() ? "true" : "false"));
//    properties.push_back(std::make_pair(QString("Detector Type"), QString(fast->detectorType())));
//  } else if (detector->type() == Feature::Type::gftt){
//    Gftt *gftt = dynamic_cast<Gftt *>(detector.get());
//    properties.push_back(std::make_pair(QString("Name"), QString("GFTT")));
//    properties.push_back(std::make_pair(QString("Max Features"), QString::number(gftt->maxFeatures())));
//    properties.push_back(std::make_pair(QString("Quality Level"), QString::number(gftt->qualityLevel())));
//    properties.push_back(std::make_pair(QString("Min Distance"), QString::number(gftt->minDistance())));
//    properties.push_back(std::make_pair(QString("Block Size"), QString::number(gftt->blockSize())));
//    properties.push_back(std::make_pair(QString("Harris Detector"), gftt->harrisDetector() ? "true" : "false"));
//    properties.push_back(std::make_pair(QString("K"), QString::number(gftt->k())));
//  } else if (detector->type() == Feature::Type::kaze){
//    Kaze *kaze = dynamic_cast<Kaze *>(detector.get());
//    properties.push_back(std::make_pair(QString("Name"), QString("KAZE")));
//    properties.push_back(std::make_pair(QString("Extended Descriptor"), kaze->extendedDescriptor() ? "true" : "false"));
//    properties.push_back(std::make_pair(QString("Upright"), kaze->uprightDescriptor() ? "true" : "false"));
//    properties.push_back(std::make_pair(QString("Threshold"), QString::number(kaze->threshold())));
//    properties.push_back(std::make_pair(QString("Octaves"), QString::number(kaze->octaves())));
//    properties.push_back(std::make_pair(QString("Octave Layers"), QString::number(kaze->octaveLayers())));
//    properties.push_back(std::make_pair(QString("Diffusivity"), kaze->diffusivity()));
//  } else if (detector->type() == Feature::Type::msd){
//    Msd *msd = dynamic_cast<Msd *>(detector.get());
//    properties.push_back(std::make_pair(QString("Name"), QString("MSD")));
//    properties.push_back(std::make_pair(QString("Threshold Saliency"), QString::number(msd->thresholdSaliency())));
//    properties.push_back(std::make_pair(QString("Patch Radius"), QString::number(msd->knn())));
//    properties.push_back(std::make_pair(QString("Search Area Radius"), QString::number(msd->searchAreaRadius())));
//    properties.push_back(std::make_pair(QString("Scale Factor"), QString::number(msd->scaleFactor())));
//    properties.push_back(std::make_pair(QString("NMS Radius"), QString::number(msd->NMSRadius())));
//    properties.push_back(std::make_pair(QString("NScales"), QString::number(msd->nScales())));
//    properties.push_back(std::make_pair(QString("NMS Scale Radius"), QString::number(msd->NMSScaleRadius())));
//    properties.push_back(std::make_pair(QString("Compute Orientation"), msd->computeOrientation() ? "true" : "false"));
//    properties.push_back(std::make_pair(QString("Affine MSD"), msd->affineMSD() ? "true" : "false"));
//    properties.push_back(std::make_pair(QString("Affine Tilts"),QString::number(msd->affineTilts())));
//  } else if (detector->type() == Feature::Type::mser){
//    Mser *mser = dynamic_cast<Mser *>(detector.get());
//    properties.push_back(std::make_pair(QString("Name"), QString("MSER")));
//    properties.push_back(std::make_pair(QString("Delta"), QString::number(mser->delta())));
//    properties.push_back(std::make_pair(QString("Min Area"), QString::number(mser->minArea())));
//    properties.push_back(std::make_pair(QString("Max Area"), QString::number(mser->maxArea())));
//    properties.push_back(std::make_pair(QString("Max Variation"), QString::number(mser->maxVariation())));
//    properties.push_back(std::make_pair(QString("Min Diversity"), QString::number(mser->minDiversity())));
//    properties.push_back(std::make_pair(QString("Max Evolution"), QString::number(mser->maxEvolution())));
//    properties.push_back(std::make_pair(QString("Area Threshold"), QString::number(mser->areaThreshold())));
//    properties.push_back(std::make_pair(QString("Min Margin"), QString::number(mser->minMargin())));
//    properties.push_back(std::make_pair(QString("Edge Blur Size"), QString::number(mser->edgeBlurSize())));
//  } else if (detector->type() == Feature::Type::orb){
//    Orb *orb = dynamic_cast<Orb *>(detector.get());
//    properties.push_back(std::make_pair(QString("Name"), QString("ORB")));
//    properties.push_back(std::make_pair(QString("Features Number"), QString::number(orb->featuresNumber())));
//    properties.push_back(std::make_pair(QString("Scale Factor"), QString::number(orb->scaleFactor())));
//    properties.push_back(std::make_pair(QString("Levels Number"), QString::number(orb->levelsNumber())));
//    properties.push_back(std::make_pair(QString("Edge Threshold"), QString::number(orb->edgeThreshold())));
//    properties.push_back(std::make_pair(QString("Wta k"), QString::number(orb->wta_k())));
//    properties.push_back(std::make_pair(QString("Score Type"), orb->scoreType()));
//    properties.push_back(std::make_pair(QString("Patch Size"), QString::number(orb->patchSize())));
//    properties.push_back(std::make_pair(QString("Fast Threshold"), QString::number(orb->fastThreshold())));
//  } else if (detector->type() == Feature::Type::sift){
//    Sift *sift = dynamic_cast<Sift *>(detector.get());
//    properties.push_back(std::make_pair(QString("Name"), QString("SIFT")));
//    properties.push_back(std::make_pair(QString("Features Number"), QString::number(sift->featuresNumber())));
//    properties.push_back(std::make_pair(QString("Octave Layers"), QString::number(sift->octaveLayers())));
//    properties.push_back(std::make_pair(QString("Contrast Threshold"), QString::number(sift->contrastThreshold())));
//    properties.push_back(std::make_pair(QString("Edge Threshold"), QString::number(sift->edgeThreshold())));
//    properties.push_back(std::make_pair(QString("Sigma"), QString::number(sift->sigma())));
//  } else if (detector->type() == Feature::Type::star){
//    Star *star = dynamic_cast<Star *>(detector.get());
//    properties.push_back(std::make_pair(QString("Name"), QString("STAR")));
//    properties.push_back(std::make_pair(QString("Max Size"), QString::number(star->maxSize())));
//    properties.push_back(std::make_pair(QString("Response Threshold"), QString::number(star->responseThreshold())));
//    properties.push_back(std::make_pair(QString("Line Threshold Projected"), QString::number(star->lineThresholdProjected())));
//    properties.push_back(std::make_pair(QString("Line Threshold Binarized"), QString::number(star->lineThresholdBinarized())));
//    properties.push_back(std::make_pair(QString("Suppress Nonmax Size"), QString::number(star->suppressNonmaxSize())));
//  } else if (detector->type() == Feature::Type::surf){
//    Surf *surf = dynamic_cast<Surf *>(detector.get());
//    properties.push_back(std::make_pair(QString("Name"), QString("SURF")));
//    properties.push_back(std::make_pair(QString("Hessian Threshold"), QString::number(surf->hessianThreshold())));
//    properties.push_back(std::make_pair(QString("Octaves"), QString::number(surf->octaves())));
//    properties.push_back(std::make_pair(QString("Octave Layers"), QString::number(surf->octaveLayers())));
//    properties.push_back(std::make_pair(QString("Extended Descriptor"), surf->extendedDescriptor() ? "true" : "false"));
//    properties.push_back(std::make_pair(QString("Rotated Features"), surf->upright() ? "true" : "false"));
//  }

//  mView->setProperties(properties);
//}

//void MainWindowPresenter::selectDescriptor(const QString &session)
//{
//  ///TODO: ¿Mover a modelo?

//  std::shared_ptr<Session> _session = mProjectModel->findSession(session);
//  std::shared_ptr<Feature> descriptor = _session->descriptor();

//  std::list<std::pair<QString, QString>> properties;
//  if (descriptor->type() == Feature::Type::akaze){
//    Akaze *akaze = dynamic_cast<Akaze *>(descriptor.get());
//    properties.push_back(std::make_pair(QString("Name"), QString("AKAZE")));
//    properties.push_back(std::make_pair(QString("Descriptor Type"), QString(akaze->descriptorType())));
//    properties.push_back(std::make_pair(QString("Descriptor Size"), QString::number(akaze->descriptorSize())));
//    properties.push_back(std::make_pair(QString("Descriptor Channels"), QString::number(akaze->descriptorChannels())));
//    properties.push_back(std::make_pair(QString("Threshold"), QString::number(akaze->threshold())));
//    properties.push_back(std::make_pair(QString("Octaves"), QString::number(akaze->octaves())));
//    properties.push_back(std::make_pair(QString("Octave Layers"), QString::number(akaze->octaveLayers())));
//    properties.push_back(std::make_pair(QString("Diffusivity"), akaze->diffusivity()));
//  } else if (descriptor->type() == Feature::Type::brief){
//    Brief *brief = dynamic_cast<Brief *>(descriptor.get());
//    properties.push_back(std::make_pair(QString("Name"), QString("BRIEF")));
//    properties.push_back(std::make_pair(QString("Bytes"), brief->bytes()));
//    properties.push_back(std::make_pair(QString("Use Orientation"), brief->useOrientation() ? "true" : "false"));
//  } else if (descriptor->type() == Feature::Type::brisk){
//    Brisk *brisk = dynamic_cast<Brisk *>(descriptor.get());
//    properties.push_back(std::make_pair(QString("Name"), QString("BRISK")));
//    properties.push_back(std::make_pair(QString("Threshold"), QString::number(brisk->threshold())));
//    properties.push_back(std::make_pair(QString("Octaves"), QString::number(brisk->octaves())));
//    properties.push_back(std::make_pair(QString("Pattern Scale"), QString::number(brisk->patternScale())));
//  } else if (descriptor->type() == Feature::Type::daisy){
//    Daisy *daisy = dynamic_cast<Daisy *>(descriptor.get());
//    properties.push_back(std::make_pair(QString("Name"), QString("DAISY")));
//    properties.push_back(std::make_pair(QString("Q Radius"), QString::number(daisy->radius())));
//    properties.push_back(std::make_pair(QString("Q Theta"), QString::number(daisy->qRadius())));
//    properties.push_back(std::make_pair(QString("Q Hist Scale"), QString::number(daisy->qTheta())));
//    properties.push_back(std::make_pair(QString("Q Radius"), QString::number(daisy->qHist())));
//    properties.push_back(std::make_pair(QString("Norm"), daisy->norm()));
//    properties.push_back(std::make_pair(QString("Interpolation"), daisy->interpolation() ? "true" : "false"));
//    properties.push_back(std::make_pair(QString("Use Orientation"), daisy->useOrientation() ? "true" : "false"));
//  } else if (descriptor->type() == Feature::Type::freak){
//    Freak *freak = dynamic_cast<Freak *>(descriptor.get());
//    properties.push_back(std::make_pair(QString("Name"), QString("FREAK")));
//    properties.push_back(std::make_pair(QString("Orientation Normalized"), freak->orientationNormalized() ? "true" : "false"));
//    properties.push_back(std::make_pair(QString("Scale Normalized"), freak->scaleNormalized() ? "true" : "false"));
//    properties.push_back(std::make_pair(QString("PatternScale"), QString::number(freak->patternScale())));
//    properties.push_back(std::make_pair(QString("Octaves"), QString::number(freak->octaves())));
//  } else if (descriptor->type() == Feature::Type::hog){
//    Hog *hog = dynamic_cast<Hog *>(descriptor.get());
//    properties.push_back(std::make_pair(QString("Name"), QString("HOG")));
//    properties.push_back(std::make_pair(QString("Window Size"), QString::number(hog->winSize().width()).append("x").append(QString::number(hog->winSize().height()))));
//    properties.push_back(std::make_pair(QString("Block Size"), QString::number(hog->blockSize().width()).append("x").append(QString::number(hog->blockSize().height()))));
//    properties.push_back(std::make_pair(QString("Block Stride"), QString::number(hog->blockStride().width()).append("x").append(QString::number(hog->blockStride().height()))));
//    properties.push_back(std::make_pair(QString("Cell Size"), QString::number(hog->cellSize().width()).append("x").append(QString::number(hog->cellSize().height()))));
//    properties.push_back(std::make_pair(QString("Nbins"), QString::number(hog->nbins())));
//    properties.push_back(std::make_pair(QString("DerivAperture"), QString::number(hog->derivAperture())));
//  } else if (descriptor->type() == Feature::Type::kaze){
//    Kaze *kaze = dynamic_cast<Kaze *>(descriptor.get());
//    properties.push_back(std::make_pair(QString("Name"), QString("KAZE")));
//    properties.push_back(std::make_pair(QString("Extended Descriptor"), kaze->extendedDescriptor() ? "true" : "false"));
//    properties.push_back(std::make_pair(QString("Upright"), kaze->uprightDescriptor() ? "true" : "false"));
//    properties.push_back(std::make_pair(QString("Threshold"), QString::number(kaze->threshold())));
//    properties.push_back(std::make_pair(QString("Octaves"), QString::number(kaze->octaves())));
//    properties.push_back(std::make_pair(QString("Octave Layers"), QString::number(kaze->octaveLayers())));
//    properties.push_back(std::make_pair(QString("Diffusivity"), kaze->diffusivity()));
//  } else if (descriptor->type() == Feature::Type::latch){
//    Latch *latch = dynamic_cast<Latch *>(descriptor.get());
//    properties.push_back(std::make_pair(QString("Name"), QString("LATCH")));
//    properties.push_back(std::make_pair(QString("Bytes"), latch->bytes()));
//    properties.push_back(std::make_pair(QString("Rotation Invariance"), latch->rotationInvariance() ? "true" : "false"));
//    properties.push_back(std::make_pair(QString("Half SSD Size"), QString::number(latch->halfSsdSize())));
//  } else if (descriptor->type() == Feature::Type::lucid){
//    Lucid *lucid = dynamic_cast<Lucid *>(descriptor.get());
//    properties.push_back(std::make_pair(QString("Name"), QString("LUCID")));
//    properties.push_back(std::make_pair(QString("LUCID Kernel"), QString::number(lucid->lucidKernel())));
//    properties.push_back(std::make_pair(QString("Blur Kernel"), QString::number(lucid->blurKernel())));
//  } else if (descriptor->type() == Feature::Type::orb){
//    Orb *orb = dynamic_cast<Orb *>(descriptor.get());
//    properties.push_back(std::make_pair(QString("Name"), QString("ORB")));
//    properties.push_back(std::make_pair(QString("Features Number"), QString::number(orb->featuresNumber())));
//    properties.push_back(std::make_pair(QString("Scale Factor"), QString::number(orb->scaleFactor())));
//    properties.push_back(std::make_pair(QString("Levels Number"), QString::number(orb->levelsNumber())));
//    properties.push_back(std::make_pair(QString("Edge Threshold"), QString::number(orb->edgeThreshold())));
//    properties.push_back(std::make_pair(QString("Wta k"), QString::number(orb->wta_k())));
//    properties.push_back(std::make_pair(QString("Score Type"), orb->scoreType()));
//    properties.push_back(std::make_pair(QString("Patch Size"), QString::number(orb->patchSize())));
//    properties.push_back(std::make_pair(QString("Fast Threshold"), QString::number(orb->fastThreshold())));
//  } else if (descriptor->type() == Feature::Type::sift){
//    Sift *sift = dynamic_cast<Sift *>(descriptor.get());
//    properties.push_back(std::make_pair(QString("Name"), QString("SIFT")));
//    properties.push_back(std::make_pair(QString("Features Number"), QString::number(sift->featuresNumber())));
//    properties.push_back(std::make_pair(QString("Octave Layers"), QString::number(sift->octaveLayers())));
//    properties.push_back(std::make_pair(QString("Contrast Threshold"), QString::number(sift->contrastThreshold())));
//    properties.push_back(std::make_pair(QString("Edge Threshold"), QString::number(sift->edgeThreshold())));
//    properties.push_back(std::make_pair(QString("Sigma"), QString::number(sift->sigma())));
//  } else if (descriptor->type() == Feature::Type::surf){
//    Surf *surf = dynamic_cast<Surf *>(descriptor.get());
//    properties.push_back(std::make_pair(QString("Name"), QString("SURF")));
//    properties.push_back(std::make_pair(QString("Hessian Threshold"), QString::number(surf->hessianThreshold())));
//    properties.push_back(std::make_pair(QString("Octaves"), QString::number(surf->octaves())));
//    properties.push_back(std::make_pair(QString("Octave Layers"), QString::number(surf->octaveLayers())));
//    properties.push_back(std::make_pair(QString("Extended Descriptor"), surf->extendedDescriptor() ? "true" : "false"));
//    properties.push_back(std::make_pair(QString("Rotated Features"), surf->upright() ? "true" : "false"));
//  }

//  mView->setProperties(properties);
//}

//void MainWindowPresenter::selectImageFeatures(const QString &imageFeatures)
//{
//  std::vector<QPointF> feat = mModel->loadKeyPointsCoordinates(imageFeatures);
//  std::list<std::pair<QString, QString>> properties;
//  properties.push_back(std::make_pair(QString("Features"), QString::number(feat.size())));
//  mView->setProperties(properties);
//}

void MainWindowPresenter::openImageMatches(const QString &sessionName, const QString &imgName1, const QString &imgName2)
{
//  std::vector<std::pair<QPointF, QPointF>> matches;
//  QString imgPath1 = mProjectModel->findImageByName(imgName1)->path();
//  QString imgPath2 = mProjectModel->findImageByName(imgName2)->path();

//  if (QFileInfo(imgPath1).exists() == false || QFileInfo(imgPath2).exists() == false)
//    return;

//  QString name(sessionName);
//  name.append("/").append(imgName1).append("-").append(imgName2);
//  int idTab = mTabHandler->graphicViewerId(name);
//  if (idTab == -1){

//    if (std::shared_ptr<Session> session = mProjectModel->findSession(sessionName)){

//      std::vector<std::pair<QString, QString>> pairs = session->matches(imgName1);
//      if (!pairs.empty()){
//        for (auto &pair : pairs){
//          if (pair.first.compare(imgName2) == 0){
//            matches = mModel->loadMatches(pair.second, session->features(imgName1), session->features(imgName2));

//            break;
//          }
//        }
//      }
//    }

//    GraphicViewer *graphicViewer = mTabHandler->addGraphicViewer(name);
//    if (graphicViewer){
//      QImage imageLeft(imgPath1);
//      QImage imageRight(imgPath2);
//      int height = imageLeft.height() > imageRight.height() ? imageLeft.height() : imageRight.height();
//      QImage pair(imageLeft.width() + imageRight.width(), height, imageLeft.format());

//      QPainter painter;
//      painter.begin(&pair);
//      painter.drawImage(0, 0, imageLeft);
//      painter.drawImage(imageLeft.width(), 0, imageRight);
//      QPen point_pen(QColor(0, 0, 255), 2.);
//      point_pen.setCosmetic(true);
//      QPen line_pen(QColor(229, 9, 127), 2.);
//      line_pen.setCosmetic(true);
//      painter.end();

//      graphicViewer->setImage(pair);

//      for (size_t i = 0; i < matches.size(); i++){
//        graphicViewer->scene()->addLine(matches[i].first.x(), matches[i].first.y(),
//                                        imageLeft.width() + matches[i].second.x(),
//                                        matches[i].second.y(), line_pen);
//        graphicViewer->scene()->addEllipse(matches[i].first.x() - 5, matches[i].first.y() - 5, 10, 10, point_pen);
//        graphicViewer->scene()->addEllipse(imageLeft.width() + matches[i].second.x() - 5, matches[i].second.y() - 5, 10, 10, point_pen);
//      }

//      graphicViewer->zoomExtend();

//    }
//  } else {
//    mTabHandler->setCurrentTab(idTab);
  //  }
}

void MainWindowPresenter::openModel3D(const QString &model3D, bool loadCameras)
{
  try {
    //Image image = mImagesModel->findImageByName(imageName);
    mTabHandler->setModel3D(model3D);
    
    // Load Cameras
    if (loadCameras) {
      for (auto image = mImagesModel->begin(); image != mImagesModel->end(); image++) {
        QFileInfo(image->path()).fileName();
        QString name = image->name();
        QString file_name = QFileInfo(image->path()).fileName();
        PhotoOrientation photoOrientation = mOrientationModel->photoOrientation(name);
      
        std::array<double, 3> position;
        position[0] = photoOrientation.x;
        position[1] = photoOrientation.y;
        position[2] = photoOrientation.z;
      
        mTabHandler->addCamera(name, position, photoOrientation.rot);

      }
    }

  } catch (std::exception &e) {
    tl::MessageManager::release(e.what(), tl::MessageLevel::msg_error);
  }
}

//void MainWindowPresenter::updateFeatures()
//{
//  std::shared_ptr<Session> _session = mProjectModel->currentSession();
//  if (_session){
//    bool project_modified = loadFeatures(_session->name());
//    if (project_modified)
//      mView->setFlag(MainWindowView::Flag::project_modified, true);
//  }
//}

//void MainWindowPresenter::updateMatches()
//{
//  std::shared_ptr<Session> _session = mProjectModel->currentSession();
//  if (_session){
//    bool project_modified = loadMatches(_session->name());
//    if (project_modified)
//      mView->setFlag(MainWindowView::Flag::project_modified, project_modified);
//  }
//}

void MainWindowPresenter::deleteFeatures()
{
  TL_TODO("completar")

  mFeaturesModel->clear();
//  if (std::shared_ptr<Session> session = mProjectModel->currentSession()){
    
//    for (auto &feat : session->features()) {
//      mView->deleteFeatures(session->name(), feat);
//      QFile::remove(feat);
//    }
//    mProjectModel->clearFeatures();

//    mView->setFlag(MainWindowView::Flag::project_modified, true);

//    this->deleteMatches();

//    mProjectModel->deleteMatcher();
//    mView->setFlag(MainWindowView::Flag::feature_matching, false);
//  }
}

void MainWindowPresenter::deleteMatches()
{
  TL_TODO("completar")
  mMatchesModel->clear();
//  if (std::shared_ptr<Session> session = mProjectModel->currentSession()){

//    for (auto it = mProjectModel->imageBegin(); it != mProjectModel->imageEnd(); it++){
//      std::vector<std::pair<QString, QString>> pairs = session->matches((*it)->name());
//      if (!pairs.empty()){
//        for (auto &pair : pairs){
//          mView->deleteMatches(session->name(), pair.second);
//          QFile::remove(pair.second);

//          /// Cierre de ficheros abiertos
//          QString name(session->name());
//          name.append("/").append(QFileInfo(pair.second).baseName());
//          mTabHandler->hideTab(mTabHandler->graphicViewerId(name));
//        }
//      }
//    }

//    mProjectModel->clearMatches();
//  }
}

void MainWindowPresenter::processFinish()
{
  mView->setFlag(MainWindowView::Flag::processing, false);
  mView->setFlag(MainWindowView::Flag::project_modified, true);
}

void MainWindowPresenter::processRunning()
{
  mView->setFlag(MainWindowView::Flag::processing, true);
}

//void MainWindowPresenter::onLoadImages()
//{
//  mView->setFlag(MainWindowView::Flag::loading_images, false);
//  disconnect(mView, SIGNAL(imagesLoaded()),   this,  SLOT(onLoadImages()));
//}

void MainWindowPresenter::loadingImages(bool loading)
{
  mView->setFlag(MainWindowView::Flag::loading_images, loading);
}

void MainWindowPresenter::loadImage(const QString &image)
{
  mView->addImage(image);
  mView->setFlag(MainWindowView::Flag::images_added, true);
}

void MainWindowPresenter::onProjectModified()
{
  mView->setFlag(MainWindowView::Flag::project_modified, true);
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

  connect(mView, &MainWindowView::openNew,                this, &MainWindowPresenter::openNew);
  connect(mView, &MainWindowView::openProject,            this, &MainWindowPresenter::openProject);
  connect(mView, &MainWindowView::openProjectFromHistory, this, &MainWindowPresenter::openFromHistory);  ///TODO: falta test señal
  connect(mView, &MainWindowView::clearHistory,           this, &MainWindowPresenter::deleteHistory);
  //connect(mView, &MainWindowView::openExportFeatures,     this, &MainWindowPresenter::openExportFeaturesDialog);
  //connect(mView, &MainWindowView::openExportMatches,      this, &MainWindowPresenter::openExportMatchesDialog);
  connect(mView, &MainWindowView::openExportOrientations, this, &MainWindowPresenter::openExportOrientationsDialog);
  connect(mView, &MainWindowView::saveProject,            this, &MainWindowPresenter::saveProject);
  connect(mView, &MainWindowView::saveProjectAs,          this, &MainWindowPresenter::saveProjectAs);
  connect(mView, &MainWindowView::closeProject,           this, &MainWindowPresenter::closeProject);
  connect(mView, &MainWindowView::exit,                   this, &MainWindowPresenter::exit);

  /* Menú View */

  connect(mView,   &MainWindowView::openStartPage,        this, &MainWindowPresenter::openStartPage);

  /* Menú flujo de trabajo */

  //connect(mView,   &MainWindowView::loadImages,            this, &MainWindowPresenter::loadImages);
  connect(mView,   &MainWindowView::loadImages,            this, &MainWindowPresenter::openLoadImagesDialog);
  connect(mView,   &MainWindowView::openFeatureExtraction, this, &MainWindowPresenter::openFeatureExtractionDialog);
  connect(mView,   &MainWindowView::openFeatureMatching,   this, &MainWindowPresenter::openFeatureMatchingDialog);
  connect(mView,   &MainWindowView::openOrientation,       this, &MainWindowPresenter::openOrientationDialog);
  connect(mView,   &MainWindowView::openDensification,     this, &MainWindowPresenter::openDensificationDialog);

  /* Menú herramientas */

  connect(mView,  &MainWindowView::openCamerasDialog,   this, &MainWindowPresenter::openCamerasDialog);
  connect(mView,  &MainWindowView::openKeypointsViewer, this, &MainWindowPresenter::openKeypointsViewerDialog);
  connect(mView,  &MainWindowView::openMatchesViewer,   this, &MainWindowPresenter::openMatchesViewerDialog);
  //connect(mView,  &MainWindowView::openMultiviewMatchingAssessment,  this, &MainWindowPresenter::openMultiviewMatchingAssessmentDialog);
  connect(mView,   &MainWindowView::openSettings,         this, &MainWindowPresenter::openSettingsDialog);

  /* Menú Ayuda */

  connect(mView, &MainWindowView::openHelpDialog,     this, &MainWindowPresenter::help);
  connect(mView, &MainWindowView::openAboutDialog,    this, &MainWindowPresenter::openAboutDialog);

  /* Panel de vistas en miniatura */

  connect(mView, SIGNAL(openImage(QString)),          this, SLOT(openImage(QString)));
  connect(mView, SIGNAL(selectImage(QString)),        this, SLOT(activeImage(QString)));
  connect(mView, SIGNAL(selectImages(QStringList)),   this, SLOT(activeImages(QStringList)));
  connect(mView, SIGNAL(deleteImages(QStringList)),   this, SLOT(deleteImages(QStringList)));

//  connect(mView, SIGNAL(selectFeatures(QString)),     this, SLOT(selectFeatures(QString)));
//  connect(mView, SIGNAL(selectDetector(QString)),     this, SLOT(selectDetector(QString)));
//  connect(mView, SIGNAL(selectDescriptor(QString)),   this, SLOT(selectDescriptor(QString)));
//  connect(mView, SIGNAL(selectImageFeatures(QString)),   this, SLOT(selectImageFeatures(QString)));

  /* Visor de imagenes */

  //connect(mView, SIGNAL(openImageMatches(QString,QString,QString)),   this, SLOT(openImageMatches(QString,QString,QString)));

  connect(mView, &MainWindowView::openKeypointsViewerFromImage, this, &MainWindowPresenter::openKeypointsViewerDialogFromImage);
  connect(mView, &MainWindowView::openMatchesViewerFromImages,  this, &MainWindowPresenter::openMatchesViewerDialogFromImages);

  connect(mView, SIGNAL(openModel3D(QString, bool)),          this, SLOT(openModel3D(QString, bool)));

}

void MainWindowPresenter::initDefaultPath()
{
  mProjectDefaultPath = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
  mProjectDefaultPath.append("/Inspector/Projects");

  QDir dir(mProjectDefaultPath);
  if (!dir.exists()) {
    dir.mkpath(".");
  }
}

void MainWindowPresenter::initStartPage()
{
  if (mStartPageWidget == nullptr){
    mStartPageWidget = new StartPageWidget(mView);

    connect(mStartPageWidget,   &StartPageWidget::openNew,                this, &MainWindowPresenter::openNew);
    connect(mStartPageWidget,   &StartPageWidget::openProject,            this, &MainWindowPresenter::openProject);
    connect(mStartPageWidget,   &StartPageWidget::openSettings,           this, &MainWindowPresenter::openSettingsDialog);
    connect(mStartPageWidget,   &StartPageWidget::clearHistory,           this, &MainWindowPresenter::deleteHistory);
    connect(mStartPageWidget,   &StartPageWidget::openProjectFromHistory, this, &MainWindowPresenter::openFromHistory);
  }
}

} // namespace ui

} // namespace inspector
