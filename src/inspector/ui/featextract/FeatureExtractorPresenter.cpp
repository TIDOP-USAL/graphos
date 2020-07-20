#include "FeatureExtractorPresenter.h"

#include "inspector/core/features/features.h"
#include "inspector/core/features/sift.h"
#include "inspector/ui/featextract/FeatureExtractorView.h"
#include "inspector/ui/featextract/FeatureExtractorModel.h"
#include "inspector/ui/images/ImagesModel.h"
#include "inspector/ui/cameras/CamerasModel.h"
#include "inspector/ui/SettingsModel.h"
#include "inspector/ui/HelpDialog.h"
#include "inspector/ui/utils/Progress.h"
#include "inspector/widgets/SiftWidget.h"
#include "inspector/process/MultiProcess.h"
#include "inspector/process/features/FeatureExtractorProcess.h"

#include <tidop/core/messages.h>

#include <QDir>
#include <QImageReader>
#include <QMessageBox>

namespace inspector
{

namespace ui
{

FeatureExtractorPresenterImp::FeatureExtractorPresenterImp(FeatureExtractorView *view,
                                                           FeatureExtractorModel *model,
                                                           ImagesModel *imagesModel,
                                                           CamerasModel *camerasModel,
                                                           SettingsModel *settingsModel)
  : FeatureExtractorPresenter(),
    mView(view),
    mModel(model),
    mImagesModel(imagesModel),
    mCamerasModel(camerasModel),
    mSettingsModel(settingsModel),
    mHelp(nullptr),
    mSift(new SiftWidgetImp),
    mMultiProcess(new MultiProcess(true)),
    mProgressHandler(nullptr)
{
  this->init();
  this->initSignalAndSlots();
}

FeatureExtractorPresenterImp::~FeatureExtractorPresenterImp()
{
  if (mSift){
    delete mSift;
    mSift = nullptr;
  }

  if (mMultiProcess){
    delete mMultiProcess;
    mMultiProcess = nullptr;
  }
}

void FeatureExtractorPresenterImp::help()
{
  if (mHelp){
    mHelp->setPage("feature_extraction.html");
    mHelp->setModal(true);
    mHelp->showMaximized();
  }
}

void FeatureExtractorPresenterImp::open()
{
  this->setDetectorAndDescriptorProperties();

  mView->exec();
}

void FeatureExtractorPresenterImp::setHelp(HelpDialog *help)
{
  mHelp = help;
}

void FeatureExtractorPresenterImp::init()
{
  mView->addDetectorDescriptor(mSift);
  mView->setCurrentDetectorDescriptor(mSift->windowTitle());
}

void FeatureExtractorPresenterImp::initSignalAndSlots()
{
  connect(mView, &FeatureExtractorView::detectorDescriptorChange,  this, &FeatureExtractorPresenterImp::setCurrentDetectorDescriptor);
  connect(mView, &FeatureExtractorView::run,                       this, &FeatureExtractorPresenterImp::run);
  connect(mView, &IDialogView::help,                               this, &FeatureExtractorPresenterImp::help);
}

void FeatureExtractorPresenterImp::setProgressHandler(ProgressHandler *progressHandler)
{
  mProgressHandler = progressHandler;
}

void FeatureExtractorPresenterImp::cancel()
{
  mMultiProcess->stop();

  disconnect(mMultiProcess, SIGNAL(error(int, QString)), this, SLOT(onError(int, QString)));
  disconnect(mMultiProcess, SIGNAL(finished()),          this, SLOT(onFinished()));

  if (mProgressHandler){
    mProgressHandler->setRange(0,1);
    mProgressHandler->setValue(1);
    mProgressHandler->onFinish();
    mProgressHandler->setDescription(tr("Processing has been canceled by the user"));

    disconnect(mMultiProcess, SIGNAL(finished()),                 mProgressHandler,    SLOT(onFinish()));
    disconnect(mMultiProcess, SIGNAL(statusChangedNext()),        mProgressHandler,    SLOT(onNextPosition()));
    disconnect(mMultiProcess, SIGNAL(error(int, QString)),        mProgressHandler,    SLOT(onFinish()));
  }

  mMultiProcess->clearProcessList();

  emit finished();

  msgWarning("Processing has been canceled by the user");
}

void FeatureExtractorPresenterImp::run()
{

  /// Se comprueba si ya se había ejecutado previante y se borran los datos
  if (std::shared_ptr<Feature> feature_extractor = mModel->featureExtractor()){
    int i_ret = QMessageBox(QMessageBox::Warning,
                            tr("Previous results"),
                            tr("The previous results will be overwritten. Do you wish to continue?"),
                            QMessageBox::Yes|QMessageBox::No).exec();
    if (i_ret == QMessageBox::No) {
      return;
    } else {
    }
  }
  ///

  mModel->clear();

  QString currentKeypointDetector = mView->currentDetectorDescriptor();
  std::shared_ptr<FeatureExtractor> feature_extractor;

  if (currentKeypointDetector.compare("SIFT") == 0){
    feature_extractor = std::make_shared<SiftCudaDetectorDescriptor>(mSift->featuresNumber(),
                                                                     mSift->octaveLayers(),
                                                                     mSift->contrastThreshold(),
                                                                     mSift->edgeThreshold(),
                                                                     mSift->sigma());
  } else {
    ///TODO: error
    return;
  }

  mModel->setFeatureExtractor(std::dynamic_pointer_cast<Feature>(feature_extractor));


  for(auto image = mImagesModel->begin(); image != mImagesModel->end(); image++){

    int camera_id = image->cameraId();
    Camera camera = mCamerasModel->camera(camera_id);

    int maxSize;
    if (mView->fullImageSize() == false){
      maxSize = mView->maxImageSize();
    } else {
      QImageReader imageReader(image->path());
      QSize size = imageReader.size();
      maxSize = std::max(size.width(), size.height());
    }

    QString features_file = QString(image->name()).append("@").append(mModel->database());
    std::shared_ptr<FeatureExtractorProcess> feat_extract(new FeatureExtractorProcess(*image,
                                                                                      camera,
                                                                                      maxSize,
                                                                                      features_file,
                                                                                      feature_extractor));
    connect(feat_extract.get(), SIGNAL(featuresExtracted(QString, QString)), this, SLOT(onFeaturesExtracted(const QString &, const QString &)));

    mMultiProcess->appendProcess(feat_extract);
  }

  connect(mMultiProcess, SIGNAL(error(int, QString)),          this, SLOT(onError(int, QString)));
  connect(mMultiProcess, SIGNAL(finished()),                   this, SLOT(onFinished()));

  if (mProgressHandler){
    connect(mMultiProcess, SIGNAL(finished()),             mProgressHandler,    SLOT(onFinish()));
    connect(mMultiProcess, SIGNAL(statusChangedNext()),    mProgressHandler,    SLOT(onNextPosition()));
    connect(mMultiProcess, SIGNAL(error(int, QString)),    mProgressHandler,    SLOT(onFinish()));

    mProgressHandler->setRange(0, mMultiProcess->count());
    mProgressHandler->setValue(0);
    mProgressHandler->setTitle("Computing Features...");
    mProgressHandler->setDescription("Computing Features...");
    mProgressHandler->onInit();
  }

  mView->hide();

  msgInfo("Starting Feature Extraction");
//  QByteArray ba = currentKeypointDetector.toLocal8Bit();
//  const char *keypoint_detector = ba.constData();
//  msgInfo("  Feature Detector     :  %s", keypoint_detector);
//  ba = currentDescriptorExtractor.toLocal8Bit();
//  const char *descriptor_extractor = ba.constData();
//  msgInfo("  DescriptorExtractor  :  %s", descriptor_extractor);

  emit running();

  mMultiProcess->start();
}

void FeatureExtractorPresenterImp::setCurrentDetectorDescriptor(const QString &detectorDescriptor)
{
  mView->setCurrentDetectorDescriptor(detectorDescriptor);
}

void FeatureExtractorPresenterImp::setDetectorAndDescriptorProperties()
{
  this->setSiftProperties();
}

void FeatureExtractorPresenterImp::setSiftProperties()
{
  Sift *sift = nullptr;
  if (std::shared_ptr<Feature> feature_extractor = mModel->featureExtractor()){
    if (feature_extractor->type() == Feature::Type::sift){
      sift = dynamic_cast<Sift *>(feature_extractor.get());
    }
  } else {
    TL_TODO("Sift *sift = mSettingsModel->sift();")
  }

  if (sift){
    mSift->setSigma(sift->sigma());
    mSift->setOctaveLayers(sift->octaveLayers());
    mSift->setContrastThresholdAuto(sift->constrastThresholdAuto());
    mSift->setEdgeThreshold(sift->edgeThreshold());
    mSift->setFeaturesNumber(sift->featuresNumber());
    mSift->setContrastThreshold(sift->contrastThreshold());
  }
}


void FeatureExtractorPresenterImp::onError(int code, const QString &msg)
{
  disconnect(mMultiProcess, SIGNAL(error(int, QString)), this, SLOT(onError(int, QString)));
  disconnect(mMultiProcess, SIGNAL(finished()),          this, SLOT(onFinished()));

  if (mProgressHandler){
    mProgressHandler->setRange(0,1);
    mProgressHandler->setValue(1);
    mProgressHandler->onFinish();
    mProgressHandler->setDescription(tr("Feature detection and description error"));

    disconnect(mMultiProcess, SIGNAL(finished()),                 mProgressHandler,    SLOT(onFinish()));
    disconnect(mMultiProcess, SIGNAL(statusChangedNext()),        mProgressHandler,    SLOT(onNextPosition()));
    disconnect(mMultiProcess, SIGNAL(error(int, QString)),        mProgressHandler,    SLOT(onFinish()));
  }

  mMultiProcess->clearProcessList();

  emit finished();
}

void FeatureExtractorPresenterImp::onFinished()
{
  disconnect(mMultiProcess, SIGNAL(error(int, QString)), this, SLOT(onError(int, QString)));
  disconnect(mMultiProcess, SIGNAL(finished()),          this, SLOT(onFinished()));

  if (mProgressHandler){
    mProgressHandler->setRange(0,1);
    mProgressHandler->setValue(1);
    mProgressHandler->onFinish();
    mProgressHandler->setDescription(tr("Feature detection and description finished"));

    disconnect(mMultiProcess, SIGNAL(finished()),                 mProgressHandler,    SLOT(onFinish()));
    disconnect(mMultiProcess, SIGNAL(statusChangedNext()),        mProgressHandler,    SLOT(onNextPosition()));
    disconnect(mMultiProcess, SIGNAL(error(int, QString)),        mProgressHandler,    SLOT(onFinish()));
  }

  mMultiProcess->clearProcessList();

  emit finished();

  msgInfo("Feature detection and description finished.");
}

void FeatureExtractorPresenterImp::onFeaturesExtracted(const QString &imageName, const QString &featuresFile)
{
  mModel->addFeatures(imageName, featuresFile);
  emit featuresExtracted(imageName);
}

} // namespace ui

} // namespace inspector
