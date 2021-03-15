#ifndef INSPECTOR_FEATURE_EXTRACTOR_PRESENTER_INTERFACE_H
#define INSPECTOR_FEATURE_EXTRACTOR_PRESENTER_INTERFACE_H

#include "inspector/ui/process/ProcessPresenter.h"


namespace inspector
{

namespace ui
{

class FeatureExtractorPresenter
  : public ProcessPresenter
{

  Q_OBJECT

public:

  FeatureExtractorPresenter() {}
  ~FeatureExtractorPresenter() override = default;

signals:

  void featuresExtracted(QString);
  void featuresDeleted();


public slots:

  virtual void setCurrentDetectorDescriptor(const QString &detectorDescriptor) = 0;
};

} // namespace ui

} // namespace inspector

#endif // INSPECTOR_FEATURE_EXTRACTOR_PRESENTER_INTERFACE_H
