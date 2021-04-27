#include "OrientationPresenter.h"

#include "inspector/ui/orientation/OrientationModel.h"
#include "inspector/ui/orientation/OrientationView.h"
#include "inspector/ui/utils/Progress.h"
#include "inspector/core/orientation/orientationcolmap.h"
#include "inspector/process/MultiProcess.h"
#include "inspector/process/orientation/RelativeOrientationProcess.h"
#include "inspector/process/orientation/AbsoluteOrientationProcess.h"
#include "inspector/core/orientation/photoorientation.h"

#include "inspector/ui/HelpDialog.h"

#include <tidop/core/messages.h>

#include <QFileInfo>
#include <QMessageBox>

namespace inspector
{

namespace ui
{

OrientationPresenterImp::OrientationPresenterImp(OrientationView *view,
                                                 OrientationModel *model)
  : OrientationPresenter(),
    mView(view),
    mModel(model),
    mHelp(nullptr)
{
  this->init();
  this->initSignalAndSlots();
}

OrientationPresenterImp::~OrientationPresenterImp()
{
}

void OrientationPresenterImp::help()
{
  if (mHelp){
    mHelp->setPage("orientation.html");
    mHelp->setModal(true);
    mHelp->showMaximized();
  }
}

void OrientationPresenterImp::open()
{
  TL_TODO("mSettingsModel->refinePrincipalPoint();")
  mView->setRefinePrincipalPoint(mModel->refinePrincipalPoint());
  mView->enabledAbsoluteOrientation(mModel->gpsPositions());
  mView->exec();
}

void OrientationPresenterImp::setHelp(HelpDialog *help)
{
  mHelp = help;
}

void OrientationPresenterImp::init()
{
}

void OrientationPresenterImp::initSignalAndSlots()
{
  connect(mView, &OrientationView::run,   this, &OrientationPresenterImp::run);
  connect(mView, &OrientationView::help,  this, &OrientationPresenterImp::help);
}

void OrientationPresenterImp::cancel()
{
  ProcessPresenter::cancel();
  
  msgWarning("Processing has been canceled by the user");
}

void OrientationPresenterImp::onError(int code, const QString &msg)
{
  ProcessPresenter::onError(code, msg);

  if (mProgressHandler) {
    mProgressHandler->setDescription(tr("Orientation process error"));
  }
}

void OrientationPresenterImp::onFinished()
{
  ProcessPresenter::onFinished();

  if (mProgressHandler) {
    mProgressHandler->setDescription(tr("Orientation finished"));
  }

  msgInfo("Orientation finished");
}

bool OrientationPresenterImp::createProcess()
{
  QString reconstruction_path = mModel->reconstructionPath();
  if (!reconstruction_path.isEmpty()){
    int i_ret = QMessageBox(QMessageBox::Warning,
                            tr("Previous results"),
                            tr("The previous results will be overwritten. Do you wish to continue?"),
                            QMessageBox::Yes|QMessageBox::No).exec();
    if (i_ret == QMessageBox::No) {
      return false;
    }
  }

  mModel->clearProject();
  emit orientationDeleted();

  mMultiProcess->clearProcessList();

  TL_TODO("Establecer propiedades")
  bool refine_principal_point = mView->refinePrincipalPoint();

  /// RTK
  //if (mModel->rtkOrientations()) {

  //} else {

  //}


  QString database = mModel->database();
  QString imagePath = mModel->imagePath();
  QString ori_relative = mModel->projectPath() + "/ori/relative/";
  std::shared_ptr<RelativeOrientationAlgorithm> relativeOrientationAlgorithm = std::make_shared<RelativeOrientationColmapAlgorithm>(database, 
                                                                                                                                    imagePath, 
                                                                                                                                    ori_relative,
                                                                                                                                    true,
                                                                                                                                    refine_principal_point,
                                                                                                                                    true);

  std::shared_ptr<RelativeOrientationProcess> relativeOrientationProcess(new RelativeOrientationProcess(relativeOrientationAlgorithm));

  connect(relativeOrientationProcess.get(), SIGNAL(orientationFinished()), this, SLOT(onRelativeOrientationFinished()));

  mMultiProcess->appendProcess(relativeOrientationProcess);

  if (mView->absoluteOrientation()) {
    QString ori_absolute = mModel->projectPath() + "/ori/absolute/";
    std::map<QString, std::array<double, 3>> camera_positions = mModel->cameraPositions();
    std::shared_ptr<AbsoluteOrientationAlgorithm> absoluteOrientationAlgorithm;
    absoluteOrientationAlgorithm = std::make_shared<AbsoluteOrientationColmapAlgorithm>(ori_relative,
                                                                                        camera_positions,
                                                                                        ori_absolute);
    std::shared_ptr<AbsoluteOrientationProcess> absoluteOrientationProcess(new AbsoluteOrientationProcess(absoluteOrientationAlgorithm));

    connect(absoluteOrientationProcess.get(), SIGNAL(absoluteOrientationFinished()), this, SLOT(onAbsoluteOrientationFinished()));

    mMultiProcess->appendProcess(absoluteOrientationProcess);
  }

  if (mProgressHandler){
    mProgressHandler->setRange(0, 0);
    mProgressHandler->setTitle("Computing Orientation...");
    mProgressHandler->setDescription("Computing Orientation...");
  }

  mView->hide();

  return true;
}

void OrientationPresenterImp::onRelativeOrientationFinished()
{
  /// Se comprueba que se han generado todos los productos
  QString ori_relative_path = mModel->projectPath() + "/ori/relative/";
  QString sparse_model = ori_relative_path + "/sparse.ply";

  if (QFileInfo(sparse_model).exists()){

    mModel->setReconstructionPath(ori_relative_path);
    mModel->setSparseModel(ori_relative_path + "/sparse.ply");
    mModel->setOffset("");

    ReadPhotoOrientations readPhotoOrientations;
    readPhotoOrientations.open(ori_relative_path);
    int oriented_images = 0;
    for (auto image = mModel->imageBegin(); image != mModel->imageEnd(); image++) {
      QString image_oriented = QFileInfo(image->path()).fileName();
      CameraPose photoOrientation = readPhotoOrientations.orientation(QFileInfo(image->path()).fileName());
      if (photoOrientation.position.x != 0. && photoOrientation.position.y != 0. && photoOrientation.position.z != 0.) {
        mModel->addPhotoOrientation(image->name(), photoOrientation);
        oriented_images++;
      } else {
        QByteArray ba = image_oriented.toLocal8Bit();
        const char *msg = ba.constData();
        msgWarning("Image %s not oriented", msg);
      }
    }

    msgInfo("Oriented %i images", oriented_images);

    ReadCalibration readCalibration;
    readCalibration.open(ori_relative_path);
    std::shared_ptr<Calibration> calibration;
    for(auto camera_it = mModel->cameraBegin(); camera_it != mModel->cameraEnd(); camera_it++){
      calibration = readCalibration.calibration(camera_it->first);
      if (calibration){
        Camera camera = camera_it->second;
        camera.setCalibration(calibration);
        mModel->updateCamera(camera_it->first, camera);
      }
    }

    emit orientationFinished();
  }
}

void OrientationPresenterImp::onAbsoluteOrientationFinished()
{
  QString ori_absolute_path = mModel->projectPath() + "/ori/absolute/";
  QString sparse_model = ori_absolute_path + "/sparse.ply";
  if (QFileInfo(sparse_model).exists()){
    mModel->setReconstructionPath(ori_absolute_path);
    mModel->setSparseModel(ori_absolute_path + "/sparse.ply");
    mModel->setOffset(ori_absolute_path + "/offset.txt");

    ReadPhotoOrientations readPhotoOrientations;
    readPhotoOrientations.open(ori_absolute_path);
    for(auto image = mModel->imageBegin(); image != mModel->imageEnd(); image++){
      CameraPose photoOrientation = readPhotoOrientations.orientation(QFileInfo(image->path()).fileName());
      if (photoOrientation.position.x != 0. && photoOrientation.position.y != 0. && photoOrientation.position.z != 0.) {
        mModel->addPhotoOrientation(image->name(), photoOrientation);
      }
    }

    emit orientationFinished();
  } else {
    msgError("Orientation failed");
  }
}

} // namespace ui

} // namespace inspector
