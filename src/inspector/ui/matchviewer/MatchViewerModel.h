#ifndef INSPECTOR_MATCH_VIEWER_MODEL_H
#define INSPECTOR_MATCH_VIEWER_MODEL_H

#include <memory>

#include "inspector/ui/matchviewer/MatchViewer.h"
#include "inspector/core/project.h"

namespace inspector
{

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

// IMatchViewerModel interface

public:

  std::vector<QString> images() const override;
  std::vector<QString> imagePairs(const QString &imageName) const override;
  std::vector<std::tuple<size_t, size_t, QPointF, size_t, QPointF>> loadMatches(const QString &imgName1,
                                                                                const QString &imgName2) const override;
  void deleteMatch(const QString &imgName1,
                   const QString &imgName2,
                   int query_id,
                   int train_id) override;
  //void loadPassPoints() override;

// IModel interface

private:

  void init() override;

public slots:

  void clear() override;

protected:

  Project *mProject;
  //std::vector<std::vector<std::pair<QString,int>>> mPassPoints;

};

} // namespace ui

} // namespace inspector

#endif // INSPECTOR_MATCH_VIEWER_MODEL_H
