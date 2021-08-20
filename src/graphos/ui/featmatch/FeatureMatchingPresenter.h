#ifndef GRAPHOS_FEATURE_MATCHING_PRESENTER_INTERFACE_H
#define GRAPHOS_FEATURE_MATCHING_PRESENTER_INTERFACE_H

#include "graphos/ui/process/ProcessPresenter.h"

namespace graphos
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

} // End namespace graphos

#endif // GRAPHOS_FEATURE_MATCHING_PRESENTER_INTERFACE_H
