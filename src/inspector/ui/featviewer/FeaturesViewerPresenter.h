#ifndef INSPECTOR_FEATURES_VIEWER_PRESENTER_INTERFACE_H
#define INSPECTOR_FEATURES_VIEWER_PRESENTER_INTERFACE_H

#include "inspector/interfaces/mvp.h"

namespace inspector
{

namespace ui
{

class FeaturesViewerPresenter
  : public IPresenter
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

} // namespace inspector

#endif // INSPECTOR_FEATURES_VIEWER_PRESENTER_INTERFACE_H
