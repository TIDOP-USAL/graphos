#ifndef GRAPHOS_FEATURE_EXTRACTOR_VIEW_INTERFACE_H
#define GRAPHOS_FEATURE_EXTRACTOR_VIEW_INTERFACE_H

#include "graphos/interfaces/mvp.h"

namespace graphos
{

namespace ui
{

class FeatureExtractorView
  : public DialogView
{

  Q_OBJECT

public:

  FeatureExtractorView(QWidget *parent = nullptr) : DialogView(parent) {}
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

} // namespace graphos

#endif // GRAPHOS_FEATURE_EXTRACTOR_VIEW_INTERFACE_H
