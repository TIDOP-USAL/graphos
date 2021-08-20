#ifndef GRAPHOS_FEATURES_VIEWER_MODEL_INTERFACE_H
#define GRAPHOS_FEATURES_VIEWER_MODEL_INTERFACE_H

#include "graphos/interfaces/mvp.h"

namespace graphos
{

namespace ui
{

class FeaturesViewerModel
  : public Model
{

  Q_OBJECT

public:

  FeaturesViewerModel(QObject *parent = nullptr) : Model(parent) {}
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

} // namespace graphos

#endif // GRAPHOS_FEATURES_VIEWER_MODEL_INTERFACE_H
