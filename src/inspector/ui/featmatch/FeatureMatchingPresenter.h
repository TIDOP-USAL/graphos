#ifndef INSPECTOR_FEATURE_MATCHING_PRESENTER_INTERFACE_H
#define INSPECTOR_FEATURE_MATCHING_PRESENTER_INTERFACE_H

#include "inspector/ui/process/ProcessPresenter.h"

namespace inspector
{

namespace ui
{

class FeatureMatchingPresenter
  : public ProcessPresenter
{

  Q_OBJECT

public:

  FeatureMatchingPresenter(){}
  ~FeatureMatchingPresenter() override = default;

signals:

  void matchingFinished();
  void matchesDeleted();

public slots:

  virtual void setCurrentMatchMethod(const QString &matchMethod) = 0;

};

} // End namespace ui

} // End namespace inspector

#endif // INSPECTOR_FEATURE_MATCHING_PRESENTER_INTERFACE_H
