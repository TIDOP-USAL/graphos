#ifndef INSPECTOR_FEATURES_VIEWER_MODEL_INTERFACE_H
#define INSPECTOR_FEATURES_VIEWER_MODEL_INTERFACE_H

#include "inspector/interfaces/mvp.h"

namespace inspector
{

namespace ui
{

class FeaturesViewerModel
  : public IModel
{

  Q_OBJECT

public:

  FeaturesViewerModel(QObject *parent = nullptr) : IModel(parent) {}
  ~FeaturesViewerModel() = default;

  virtual std::vector<QString> images() const = 0;
  virtual std::vector<QPointF> loadKeypoints(const QString &image) = 0;

  virtual QString viewerBGColor() const = 0;
  virtual int viewerMarkerType() const = 0;
  virtual int viewerMarkerSize() const = 0;
  virtual int viewerMarkerWidth() const = 0;
  virtual QString viewerMarkerColor() const = 0;
  virtual int viewerSelectMarkerWidth() const = 0;
  virtual QString viewerSelectMarkerColor() const = 0;


};

} // namespace ui

} // namespace inspector

#endif // INSPECTOR_FEATURES_VIEWER_MODEL_INTERFACE_H
