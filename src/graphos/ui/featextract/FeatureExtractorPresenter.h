#ifndef GRAPHOS_FEATURE_EXTRACTOR_PRESENTER_INTERFACE_H
#define GRAPHOS_FEATURE_EXTRACTOR_PRESENTER_INTERFACE_H

#include "graphos/ui/process/ProcessPresenter.h"


namespace graphos
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

} // namespace graphos

#endif // GRAPHOS_FEATURE_EXTRACTOR_PRESENTER_INTERFACE_H
