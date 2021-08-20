#ifndef GRAPHOS_MATCH_VIEWER_MODEL_H
#define GRAPHOS_MATCH_VIEWER_MODEL_H

#include <memory>

#include "graphos/ui/matchviewer/MatchViewerModel.h"

class QSettings;

namespace graphos
{

class Project;

namespace ui
{

class MatchViewerModelImp
  : public MatchViewerModel
{

  Q_OBJECT

public:

  MatchViewerModelImp(Project *project,
                      QObject *parent = nullptr);
  ~MatchViewerModelImp() override;

// MatchViewerModel interface

public:

  QString viewerBGColor() const override;
  int viewerMarkerType() const override;
  int viewerMarkerSize() const override;
  int viewerMarkerWidth() const override;
  QString viewerMarkerColor() const override;
  int viewerSelectMarkerWidth() const override;
  QString viewerSelectMarkerColor() const override;
  QString viewerLineColor() const override;
  int viewerLineWidth() const override;

  std::vector<QString> images() const override;
  std::vector<QString> imagePairs(const QString &imageName) const override;
  std::vector<std::tuple<size_t, size_t, QPointF, size_t, QPointF>> loadMatches(const QString &imgName1,
                                                                                const QString &imgName2) const override;

// Model interface

private:

  void init() override;

public slots:

  void clear() override;

protected:

  Project *mProject;
  QSettings *mSettings;
};

} // namespace ui

} // namespace graphos

#endif // GRAPHOS_MATCH_VIEWER_MODEL_H
