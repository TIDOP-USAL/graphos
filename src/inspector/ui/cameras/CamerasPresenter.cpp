#include "CamerasPresenter.h"

#include "inspector/ui/cameras/CamerasModel.h"
#include "inspector/ui/ImagesModel.h"
#include "inspector/ui/HelpDialog.h"

#include <tidop/core/messages.h>

namespace inspector
{

namespace ui
{

CamerasPresenterImp::CamerasPresenterImp(CamerasView *view,
                                         CamerasModel *model,
                                         ImagesModel *imagesModel)
  : CamerasPresenter(),
    mView(view),
    mModel(model),
    mImagesModel(imagesModel),
    mHelp(nullptr),
    bModifiedProject(false)
{
  this->init();
  this->initSignalAndSlots();
}

void CamerasPresenterImp::help()
{
  if (mHelp){
    mHelp->setPage("cameras.html");
    mHelp->setModal(true);
    mHelp->showMaximized();
  }
}

void CamerasPresenterImp::open()
{
  mView->clear();

  loadCameras();
  //activeCamera(1);

  mView->exec();
}

void CamerasPresenterImp::setHelp(HelpDialog *help)
{
  mHelp = help;
}

void CamerasPresenterImp::init()
{
}

void CamerasPresenterImp::initSignalAndSlots()
{
  connect(mView, &CamerasView::cameraChange, this, &CamerasPresenterImp::activeCamera);
  connect(mView, &CamerasView::typeChange,   this, &CamerasPresenterImp::updateCurrentCameraType);
  connect(mView, &CamerasView::makeChanged,  this, &CamerasPresenterImp::updateCurrentCameraMake);
  connect(mView, &CamerasView::modelChanged,  this, &CamerasPresenterImp::updateCurrentCameraModel);


  connect(mView, &QDialog::accepted, this, &CamerasPresenterImp::save);
  connect(mView, &QDialog::rejected, this, &CamerasPresenterImp::discart);
  connect(mView, &IDialogView::help, this, &CamerasPresenterImp::help);
}

void CamerasPresenterImp::activeCamera(int id)
{
  try {
    mView->setActiveCamera(id);

    mActiveCameraId = id;

    Camera camera;
    auto it = mCameraCache.find(id);
    if (it != mCameraCache.end()){
      camera = it->second;
    } else {
      camera = mModel->camera(id);
      mCameraCache[id] = camera;
    }

    mView->setMake(camera.make());
    mView->setModel(camera.model());
    mView->setFocal(QString::number(camera.focal()));
    mView->setWidth(camera.width());
    mView->setHeight(camera.height());
    mView->setType(camera.type());
    mView->setSensorSize(QString::number(camera.sensorSize()));
    
    std::shared_ptr<Calibration> calibration = camera.calibration();
    if (calibration) {
      for (auto param = calibration->parametersBegin(); param != calibration->parametersEnd(); param++) {
        Calibration::Parameters parameter = param->first;
        double value = param->second;
        switch (parameter) {
          case inspector::Calibration::Parameters::focal:
            mView->setCalibF(QString::number(value));
            break;
          case inspector::Calibration::Parameters::focalx:
            mView->setCalibFx(QString::number(value));
            break;
          case inspector::Calibration::Parameters::focaly:
            mView->setCalibFy(QString::number(value));
            break;
          case inspector::Calibration::Parameters::cx:
            mView->setCalibCx(QString::number(value));
            break;
          case inspector::Calibration::Parameters::cy:
            mView->setCalibCy(QString::number(value));
            break;
          case inspector::Calibration::Parameters::k1:
            mView->setCalibK1(QString::number(value));
            break;
          case inspector::Calibration::Parameters::k2:
            mView->setCalibK2(QString::number(value));
            break;
          case inspector::Calibration::Parameters::k3:
            mView->setCalibK3(QString::number(value));
            break;
          case inspector::Calibration::Parameters::k4:
            mView->setCalibK4(QString::number(value));
            break;
          case inspector::Calibration::Parameters::k5:
            mView->setCalibK5(QString::number(value));
            break;
          case inspector::Calibration::Parameters::k6:
            mView->setCalibK6(QString::number(value));
            break;
          case inspector::Calibration::Parameters::p1:
            mView->setCalibP1(QString::number(value));
            break;
          case inspector::Calibration::Parameters::p2:
            mView->setCalibP2(QString::number(value));
            break;
          default:
            break;
        }
      }
    }

    /// Carga las imagenes de la cámara activa
    QStringList images;
    for(auto image = mImagesModel->begin(); image != mImagesModel->end(); image++){
      if (image->cameraId() == mActiveCameraId){
        images.push_back(image->path());
      }
    }
    mView->setImages(images);
  } catch (std::exception &e) {
    msgError(e.what());
  }
}

void CamerasPresenterImp::updateCurrentCameraMake(const QString &make)
{
  mCameraCache[mActiveCameraId].setMake(make);
  bModifiedProject = true;
}

void CamerasPresenterImp::updateCurrentCameraModel(const QString &model)
{
  mCameraCache[mActiveCameraId].setModel(model);
  bModifiedProject = true;
}

void CamerasPresenterImp::updateCurrentCameraWidth(int width)
{
  mCameraCache[mActiveCameraId].setWidth(width);
  bModifiedProject = true;
}

void CamerasPresenterImp::updateCurrentCameraHeight(int height)
{
  mCameraCache[mActiveCameraId].setHeight(height);
  bModifiedProject = true;
}

void CamerasPresenterImp::updateCurrentCameraSensorSize(const QString &sensorSize)
{
  mCameraCache[mActiveCameraId].setSensorSize(sensorSize.toDouble());
  bModifiedProject = true;
}

void CamerasPresenterImp::updateCurrentCameraFocal(const QString &focal)
{
  mCameraCache[mActiveCameraId].setFocal(focal.toDouble());
  bModifiedProject = true;
}

void CamerasPresenterImp::updateCurrentCameraType(const QString &type)
{
  mCameraCache[mActiveCameraId].setType(type);
  bModifiedProject = true;
}

void CamerasPresenterImp::save()
{
  if (bModifiedProject){
    for (auto it = mCameraCache.begin(); it != mCameraCache.end(); it++){
      mModel->updateCamera(it->first, it->second);
    }
    emit projectModified();
  }

  this->clear();
}

void CamerasPresenterImp::discart()
{
  this->clear();
}

void CamerasPresenterImp::clear()
{
  bModifiedProject = false;
  mCameraCache.clear();
  mView->clear();
}
void CamerasPresenterImp::loadCameras()
{
  for(auto it = mModel->begin(); it != mModel->end(); it++) {
    int camera_id = it->first;
    Camera camera = it->second;
    QString camera_name = QString(camera.make()).append("-").append(camera.model());
    mView->addCamera(camera_id, camera_name);
  }
}

} // namespace ui

} // namespace inspector
