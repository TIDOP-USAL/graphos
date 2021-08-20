#ifndef GRAPHOS_MATCH_VIEWER_PRESENTER_INTERFACE_H
#define GRAPHOS_MATCH_VIEWER_PRESENTER_INTERFACE_H

#include "graphos/interfaces/mvp.h"

namespace graphos
{

namespace ui
{

class MatchViewerPresenter
  : public Presenter
{

  Q_OBJECT

public:

  MatchViewerPresenter(){}
  virtual ~MatchViewerPresenter() {}

public slots:

  //virtual void openFromImages(const QString &imageLeft, const QString &imageRight) = 0;
  virtual void setLeftImage(const QString &image) = 0;
  virtual void setRightImage(const QString &image) = 0;

protected slots:


  virtual void loadMatches(const QString &imageLeft, const QString &imageRight) = 0;
//  virtual void deleteMatch(const QString &imageLeft, const QString &imageRight, int query_id, int train_id) = 0;
};

} // namespace ui

} // namespace graphos

#endif // GRAPHOS_MATCH_VIEWER_PRESENTER_INTERFACE_H
