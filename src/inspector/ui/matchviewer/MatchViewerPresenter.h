#ifndef INSPECTOR_MATCH_VIEWER_PRESENTER_INTERFACE_H
#define INSPECTOR_MATCH_VIEWER_PRESENTER_INTERFACE_H

#include "inspector/interfaces/mvp.h"

namespace inspector
{

namespace ui
{

class MatchViewerPresenter
  : public IPresenter
{

  Q_OBJECT

public:

  MatchViewerPresenter(){}
  virtual ~MatchViewerPresenter() {}

public slots:

  virtual void openFromImages(const QString &imageLeft, const QString &imageRight) = 0;

protected slots:

  virtual void setLeftImage(const QString &image) = 0;
  virtual void setRightImage(const QString &image) = 0;
  virtual void loadMatches(const QString &imageLeft, const QString &imageRight) = 0;
//  virtual void deleteMatch(const QString &imageLeft, const QString &imageRight, int query_id, int train_id) = 0;
};

} // namespace ui

} // namespace inspector

#endif // INSPECTOR_MATCH_VIEWER_PRESENTER_INTERFACE_H
