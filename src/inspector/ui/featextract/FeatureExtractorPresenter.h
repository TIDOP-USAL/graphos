#ifndef INSPECTOR_FEATURE_EXTRACTOR_PRESENTER_H
#define INSPECTOR_FEATURE_EXTRACTOR_PRESENTER_H

#include "inspector/inspector_global.h"

#include <QObject>

#include <opencv2/core.hpp>

#include "inspector/ui/featextract/FeatureExtractor.h"

class MultiProcess;

namespace inspector
{

class HelpDialog;
class SiftWidget;
class ProgressHandler;

namespace ui
{

class ImagesModel;
class CamerasModel;
class SettingsModel;

class FeatureExtractorPresenterImp
  : public FeatureExtractorPresenter
{
  Q_OBJECT

public:

  FeatureExtractorPresenterImp(FeatureExtractorView *view,
                               FeatureExtractorModel *model,
                               ImagesModel *imagesModel,
                               CamerasModel *camerasModel,
                               SettingsModel *settingsModel);
  ~FeatureExtractorPresenterImp() override;

private:

  void setDetectorAndDescriptorProperties();
  void setSiftProperties();

private slots:

  void onError(int code, const QString &msg);
  void onFinished();
  void onFeaturesExtracted(const QString &imageName, const QString &featuresFile);

// FeatureExtractorPresenter interface

public slots:

  void setProgressHandler(ProgressHandler *progressHandler) override;
  void cancel() override;

private slots:

  void run() override;
  void setCurrentDetectorDescriptor(const QString &detectorDescriptor) override;

// IPresenter interface

public slots:

  void help() override;
  void open() override;
  void setHelp(HelpDialog *help) override;

private:

  void init() override;
  void initSignalAndSlots() override;

protected:

  FeatureExtractorView *mView;
  FeatureExtractorModel *mModel;
  ImagesModel *mImagesModel;
  CamerasModel *mCamerasModel;
  SettingsModel *mSettingsModel;
  HelpDialog *mHelp;
  SiftWidget *mSift;
  MultiProcess *mMultiProcess;
  ProgressHandler *mProgressHandler;

};

} // namespace ui

} // namespace inspector

#endif // INSPECTOR_FEATURE_EXTRACTOR_PRESENTER_H
