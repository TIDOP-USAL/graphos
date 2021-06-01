#ifndef INSPECTOR_MATCH_VIEWER_MODEL_INTERFACE_H
#define INSPECTOR_MATCH_VIEWER_MODEL_INTERFACE_H

#include <memory>

#include "inspector/interfaces/mvp.h"

namespace inspector
{

namespace ui
{


class MatchViewerModel
  : public IModel
{

  Q_OBJECT

public:

  MatchViewerModel(QObject *parent = nullptr) : IModel(parent) {}
  virtual ~MatchViewerModel() = default;

  virtual QString viewerBGColor() const = 0;
  virtual int viewerMarkerType() const = 0;
  virtual int viewerMarkerSize() const = 0;
  virtual int viewerMarkerWidth() const = 0;
  virtual QString viewerMarkerColor() const = 0;
  virtual int viewerSelectMarkerWidth() const = 0;
  virtual QString viewerSelectMarkerColor() const = 0;
  virtual QString viewerLineColor() const = 0;
  virtual int viewerLineWidth() const = 0;

  virtual std::vector<QString> images() const = 0;
  virtual std::vector<QString> imagePairs(const QString &imageName) const = 0;
  virtual std::vector<std::tuple<size_t, size_t, QPointF, size_t, QPointF>> loadMatches(const QString &imgName1, const QString &imgName2) const = 0;

public slots:

};

} // namespace ui

} // namespace inspector

#endif // INSPECTOR_MATCH_VIEWER_MODEL_INTERFACE_H
