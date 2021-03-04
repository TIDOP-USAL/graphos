#ifndef INSPECTOR_FEATURE_EXTRACTOR_VIEW_INTERFACE_H
#define INSPECTOR_FEATURE_EXTRACTOR_VIEW_INTERFACE_H

#include "inspector/interfaces/mvp.h"

namespace inspector
{

namespace ui
{

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

} // namespace ui

} // namespace inspector

#endif // INSPECTOR_FEATURE_EXTRACTOR_VIEW_INTERFACE_H
