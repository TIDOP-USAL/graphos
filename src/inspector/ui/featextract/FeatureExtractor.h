#ifndef INSPECTOR_FEATURE_EXTRACTOR_INTERFACES_H
#define INSPECTOR_FEATURE_EXTRACTOR_INTERFACES_H

#include "inspector/interfaces/mvp.h"
#include "inspector/core/features/features.h"
#include "inspector/ui/process/ProcessPresenter.h"

namespace inspector
{

namespace ui
{

class ProgressHandler;


class FeatureExtractorModel
  : public IModel
{

public:

  FeatureExtractorModel(QObject *parent = nullptr) : IModel(parent) {}
  ~FeatureExtractorModel() override = default;

  virtual std::shared_ptr<Feature> featureExtractor() const = 0;
  virtual void setFeatureExtractor(const std::shared_ptr<Feature> &featureExtractor) = 0;
  virtual QString database() const = 0;
  virtual void addFeatures(const QString &imageName, const QString &featuresFile) = 0;
};



class FeatureExtractorView
  : public IDialogView
{

  Q_OBJECT

public:

  FeatureExtractorView(QWidget *parent = nullptr) : IDialogView(parent) {}
  ~FeatureExtractorView() override = default;

  virtual void addDetectorDescriptor(QWidget *detectorDescriptor) = 0;
  virtual QString currentDetectorDescriptor() const = 0;
  virtual int maxImageSize() const = 0;
  virtual bool fullImageSize() const = 0;

signals:

  void detectorDescriptorChange(QString);
  void run();

public slots:

  virtual void setCurrentDetectorDescriptor(const QString &detectorDescriptor) = 0;
  virtual void setMaxImageSize(int imageSize) = 0;
  virtual void setFullImageSize(bool fullImageSize) = 0;
};


class FeatureExtractorPresenter
  : public ProcessPresenter
{

  Q_OBJECT

public:

  FeatureExtractorPresenter() {}
  ~FeatureExtractorPresenter() override = default;

signals:

  void featuresExtracted(QString);

public slots:

  virtual void setCurrentDetectorDescriptor(const QString &detectorDescriptor) = 0;

};

} // namespace ui

} // namespace inspector


#endif // INSPECTOR_FEATURE_EXTRACTOR_INTERFACES_H
