/************************************************************************
 *                                                                      *
 *  Copyright 2016 by Tidop Research Group <daguilera@usal.se>          *
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
 * along with Foobar.  If not, see <http://www.gnu.org/licenses/>.      *
 *                                                                      *
 * @license GPL-3.0+ <http://spdx.org/licenses/GPL-3.0+>                *
 *                                                                      *
 ************************************************************************/

#include "FeatureExtractorPresenter.h"

#include "graphos/core/features/features.h"
#include "graphos/core/features/sift.h"
#include "graphos/ui/featextract/FeatureExtractorView.h"
#include "graphos/ui/featextract/FeatureExtractorModel.h"
#include "graphos/ui/HelpDialog.h"
#include "graphos/core/process/Progress.h"
#include "graphos/widgets/SiftWidget.h"
#include "graphos/process/MultiProcess.h"
#include "graphos/process/features/FeatureExtractorProcess.h"

#include <tidop/core/messages.h>

#include <QDir>
#include <QImageReader>
#include <QMessageBox>

namespace graphos
{

FeatureExtractorPresenterImp::FeatureExtractorPresenterImp(FeatureExtractorView *view,
                                                           FeatureExtractorModel *model)
  : FeatureExtractorPresenter(),
    mView(view),
    mModel(model),
    mHelp(nullptr),
    mSift(new SiftWidgetImp)
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
  connect(mView, &DialogView::help,                               this, &FeatureExtractorPresenterImp::help);
}

void FeatureExtractorPresenterImp::cancel()
{
  ProcessPresenter::cancel();

  msgWarning("Processing has been canceled by the user");
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
  std::shared_ptr<Sift> sift;
  if (std::shared_ptr<Feature> feature_extractor = mModel->featureExtractor()){
    if (feature_extractor->type() == Feature::Type::sift){
      sift = std::dynamic_pointer_cast<Sift>(feature_extractor);
    }
  } else {
    TL_TODO("Sift *sift = mSettingsModel->sift();")
  }

  if (sift){
    mSift->setSigma(sift->sigma());
    mSift->setOctaveLayers(sift->octaveLayers());
    bool thresholdAuto = sift->constrastThresholdAuto();
    mSift->setContrastThresholdAuto(thresholdAuto);
    if (!thresholdAuto)
      mSift->setEdgeThreshold(sift->edgeThreshold());
    mSift->setFeaturesNumber(sift->featuresNumber());
    mSift->setContrastThreshold(sift->contrastThreshold());
  }
}

void FeatureExtractorPresenterImp::onError(int code, const QString &msg)
{
  ProcessPresenter::onError(code, msg);

  if (mProgressHandler) {
    mProgressHandler->setDescription(tr("Feature detection and description error"));
  }
}

void FeatureExtractorPresenterImp::onFinished()
{
  ProcessPresenter::onFinished();

  if (mProgressHandler) {
    mProgressHandler->setDescription(tr("Feature detection and description finished"));
  }

  //msgInfo("Feature detection and description finished.");
}

bool FeatureExtractorPresenterImp::createProcess()
{
  /// Se comprueba si ya se había ejecutado previante y se borran los datos
  if (std::shared_ptr<Feature> feature_extractor = mModel->featureExtractor()) {
    int i_ret = QMessageBox(QMessageBox::Warning,
                            tr("Previous results"),
                            tr("The previous results will be overwritten. Do you wish to continue?"),
                            QMessageBox::Yes | QMessageBox::No).exec();
    if (i_ret == QMessageBox::No) {
      //throw std::runtime_error("Canceled by user");
      msgWarning("Process canceled by user");
      return false;
    }
  }

  mModel->clearProject();
  emit featuresDeleted();

  QString currentKeypointDetector = mView->currentDetectorDescriptor();
  std::shared_ptr<FeatureExtractor> feature_extractor;

  ///TODO: Actualizar progreso
  if (currentKeypointDetector.compare("SIFT") == 0) {
    feature_extractor = std::make_shared<SiftCudaDetectorDescriptor>(mSift->featuresNumber(),
                                                                     mSift->octaveLayers(),
                                                                     mSift->edgeThreshold(),
                                                                     mSift->sigma(),
                                                                     mSift->constrastThresholdAuto() ? 0. : mSift->contrastThreshold());
  } else {
    mView->hide();
    throw std::runtime_error("Invalid Keypoint Detector");
  }

  mModel->setFeatureExtractor(std::dynamic_pointer_cast<Feature>(feature_extractor));

  std::vector<Image> images = mModel->images();
  std::map<int, Camera> cameras = mModel->cameras();
  QString database = mModel->database();

  int maxSize = -1;
  if (mView->fullImageSize() == false) {
    maxSize = mView->maxImageSize();
  }

  std::shared_ptr<FeatureExtractorProcess> feat_extract(new FeatureExtractorProcess(images,
                                                                                    cameras,
                                                                                    database,
                                                                                    maxSize,
                                                                                    mModel->useCuda(),
                                                                                    feature_extractor));

  connect(feat_extract.get(), SIGNAL(featuresExtracted(QString, QString)), this, SLOT(onFeaturesExtracted(const QString &, const QString &)));

  /// Hacer privadas
  mMultiProcess->appendProcess(feat_extract);
  mProgressHandler->setRange(0, images.size());

  if (mProgressHandler) {
    mProgressHandler->setTitle("Computing Features...");
    mProgressHandler->setDescription("Computing Features...");
  }

  mView->hide();

  msgInfo("Starting Feature Extraction");

  return true;
}

void FeatureExtractorPresenterImp::onFeaturesExtracted(const QString &imageName, 
                                                       const QString &featuresFile)
{
  mModel->addFeatures(imageName, featuresFile);
  emit featuresExtracted(imageName);
}

} // namespace graphos
