#include "OrientationPresenter.h"


#include "inspector/ui/utils/Progress.h"
#include "inspector/core/orientation/orientationcolmap.h"
#include "inspector/process/MultiProcess.h"
#include "inspector/process/orientation/RelativeOrientationProcess.h"
#include "inspector/core/orientation/photoorientation.h"
#include "inspector/ui/SettingsModel.h"
#include "inspector/ui/images/ImagesModel.h"
#include "inspector/ui/cameras/CamerasModel.h"

#include "inspector/ui/HelpDialog.h"

#include <tidop/core/messages.h>

#include <QFileInfo>

namespace inspector
{

namespace ui
{

OrientationPresenterImp::OrientationPresenterImp(OrientationView *view,
                                                 OrientationModel *model,
                                                 ImagesModel *imagesModel,
                                                 CamerasModel *camerasModel,
                                                 SettingsModel *settingsModel)
  : OrientationPresenter(),
    mView(view),
    mModel(model),
    mImagesModel(imagesModel),
    mCamerasModel(camerasModel),
    mSettingsModel(settingsModel),
    mHelp(nullptr),
    mMultiProcess(new MultiProcess(true)),
    mProgressHandler(nullptr)
{
  this->init();
  this->initSignalAndSlots();
}

OrientationPresenterImp::~OrientationPresenterImp()
{
  if (mMultiProcess){
    delete mMultiProcess;
    mMultiProcess = nullptr;
    }
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

void OrientationPresenterImp::setProgressHandler(ProgressHandler *progressHandler)
{
  mProgressHandler = progressHandler;
}

void OrientationPresenterImp::cancel()
{
  mMultiProcess->stop();

  disconnect(mMultiProcess, SIGNAL(error(int, QString)), this, SLOT(onError(int, QString)));
  disconnect(mMultiProcess, SIGNAL(finished()),          this, SLOT(onFinished()));

  if (mProgressHandler){
    mProgressHandler->setRange(0,1);
    mProgressHandler->setValue(1);
    mProgressHandler->finish();
    mProgressHandler->setDescription(tr("Processing has been canceled by the user"));

    disconnect(mMultiProcess, SIGNAL(finished()),                 mProgressHandler,    SLOT(onFinish()));
    disconnect(mMultiProcess, SIGNAL(statusChangedNext()),        mProgressHandler,    SLOT(next()));
    disconnect(mMultiProcess, SIGNAL(error(int, QString)),        mProgressHandler,    SLOT(onFinish()));
  }

  mMultiProcess->clearProcessList();

  emit finished();

  msgWarning("Processing has been canceled by the user");
}

void OrientationPresenterImp::run()
{

  mMultiProcess->clearProcessList();

  TL_TODO("Establecer propiedades")
  bool refine_principal_point = mView->refinePrincipalPoint();

  QString database = mModel->database();
  QString imagePath = mModel->imagePath();
  QString outputPath = mModel->projectPath();
  std::shared_ptr<OrientationProcess> orientationProcess = std::make_shared<OrientationColmapProcess>(database, imagePath, outputPath);
  std::shared_ptr<RelativeOrientationProcess> relativeOrientationProcess(new RelativeOrientationProcess(orientationProcess));

  connect(relativeOrientationProcess.get(), SIGNAL(orientationFinished()), this, SLOT(onOrientationFinished()));

  mMultiProcess->appendProcess(relativeOrientationProcess);

  connect(mMultiProcess, SIGNAL(error(int, QString)),          this, SLOT(onError(int, QString)));
  connect(mMultiProcess, SIGNAL(finished()),                   this, SLOT(onFinished()));

  if (mProgressHandler){
    connect(mMultiProcess, SIGNAL(finished()),             mProgressHandler,    SLOT(onFinish()));
    connect(mMultiProcess, SIGNAL(statusChangedNext()),    mProgressHandler,    SLOT(next()));
    connect(mMultiProcess, SIGNAL(error(int, QString)),    mProgressHandler,    SLOT(onFinish()));

    mProgressHandler->setRange(0, 0/*mMultiProcess->count()*/);
    mProgressHandler->setValue(0);
    mProgressHandler->setTitle("Computing Orientations...");
    mProgressHandler->setDescription("Computing Orientations...");
    mProgressHandler->init();
  }

  mView->hide();

  emit running();

  mMultiProcess->start();
}

void OrientationPresenterImp::onError(int code, const QString &msg)
{
  disconnect(mMultiProcess, SIGNAL(error(int, QString)), this, SLOT(onError(int, QString)));
  disconnect(mMultiProcess, SIGNAL(finished()),          this, SLOT(onFinished()));

  if (mProgressHandler){
    mProgressHandler->setRange(0,1);
    mProgressHandler->setValue(1);
    mProgressHandler->finish();
    mProgressHandler->setDescription(tr("Orientation process error"));

    disconnect(mMultiProcess, SIGNAL(finished()),                 mProgressHandler,    SLOT(onFinish()));
    disconnect(mMultiProcess, SIGNAL(statusChangedNext()),        mProgressHandler,    SLOT(next()));
    disconnect(mMultiProcess, SIGNAL(error(int, QString)),        mProgressHandler,    SLOT(onFinish()));
  }

  mMultiProcess->clearProcessList();

  emit finished();
}

void OrientationPresenterImp::onFinished()
{
  disconnect(mMultiProcess, SIGNAL(error(int, QString)), this, SLOT(onError(int, QString)));
  disconnect(mMultiProcess, SIGNAL(finished()),          this, SLOT(onFinished()));

  if (mProgressHandler){
    mProgressHandler->setRange(0, 1);
    mProgressHandler->setValue(1);
    mProgressHandler->finish();
    mProgressHandler->setDescription(tr("Orientation finished"));

    disconnect(mMultiProcess, SIGNAL(finished()),                 mProgressHandler,    SLOT(onFinish()));
    disconnect(mMultiProcess, SIGNAL(statusChangedNext()),        mProgressHandler,    SLOT(next()));
    disconnect(mMultiProcess, SIGNAL(error(int, QString)),        mProgressHandler,    SLOT(onFinish()));
  }

  mMultiProcess->clearProcessList();

  emit finished();
}

void OrientationPresenterImp::onOrientationFinished()
{
  /// Se comprueba que se han generado todos los productos
  QString sparse_path = mModel->projectPath() + "/sparse/0/";

  ReadPhotoOrientations readPhotoOrientations;
  readPhotoOrientations.open(sparse_path);
  for(auto image = mImagesModel->begin(); image != mImagesModel->end(); image++){
    PhotoOrientation photoOrientation = readPhotoOrientations.orientation(QFileInfo(image->path()).fileName());
    mModel->addPhotoOrientation(image->name(), photoOrientation);
  }

  ReadCalibration readCalibration;
  readCalibration.open(sparse_path);
  std::shared_ptr<Calibration> calibration;
  for(auto camera_it = mCamerasModel->begin(); camera_it != mCamerasModel->end(); camera_it++){
    calibration = readCalibration.calibration(camera_it->first);
    if (calibration){
      Camera camera = camera_it->second;
      camera.setCalibration(calibration);
      mCamerasModel->updateCamera(camera_it->first, camera);
    }
  }

  QString sparse_model = sparse_path + "/sparse.ply";
  if (QFileInfo(sparse_model).exists()){
    mModel->setSparseModel(sparse_path + "/sparse.ply");

    emit orientationFinished();
  }
}

} // namespace ui

} // namespace inspector
