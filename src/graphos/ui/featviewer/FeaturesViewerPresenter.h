#ifndef GRAPHOS_FEATURES_VIEWER_PRESENTER_INTERFACE_H
#define GRAPHOS_FEATURES_VIEWER_PRESENTER_INTERFACE_H

#include "graphos/interfaces/mvp.h"

namespace graphos
{

namespace ui
{

class FeaturesViewerPresenter
  : public Presenter
{

  Q_OBJECT

public:

  FeaturesViewerPresenter() {}
  ~FeaturesViewerPresenter() {}

public slots:

  virtual void setImageActive(const QString &image) = 0;
  //virtual void openKeypointsFromImage(const QString &image) = 0;

protected slots:

  virtual void loadKeypoints(const QString &image) = 0;

};

} // namespace ui

} // namespace graphos

#endif // GRAPHOS_FEATURES_VIEWER_PRESENTER_INTERFACE_H
