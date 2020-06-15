#ifndef INSPECTOR_MATCH_VIEWER_PRESENTER_H
#define INSPECTOR_MATCH_VIEWER_PRESENTER_H

#include <memory>

#include <QObject>

#include "inspector/ui/matchviewer/MatchViewer.h"

namespace inspector
{

namespace ui
{

class MatchViewerView;
class MatchViewerModel;
class SettingsModel;
class HelpDialog;


class MatchViewerPresenterImp
  : public MatchViewerPresenter
{
  Q_OBJECT

public:

  MatchViewerPresenterImp(MatchViewerView *view,
                          MatchViewerModel *model,
                          SettingsModel *settings);

  ~MatchViewerPresenterImp() override;

// IMatchViewerPresenter interface

public slots:

  void openFromImages(const QString &imageLeft,
                                const QString &imageRight) override;

protected slots:

  void loadMatches(const QString &imageLeft,
                   const QString &imageRight) override;
//  void deleteMatch(const QString &imageLeft,
//                   const QString &imageRight,
//                   int query_id,
//                   int train_id) override;
  void setLeftImage(const QString &image) override;
  void setRightImage(const QString &image) override;

// IPresenter interface

public slots:

  void help() override;
  void open() override;
  void setHelp(HelpDialog *help) override;

private:

  void init() override;
  void initSignalAndSlots() override;

public slots:

// MatchViewerPresenter interface


protected slots:

private:

  MatchViewerView *mView;
  MatchViewerModel *mModel;
  SettingsModel *mSettingsModel;
  HelpDialog *mHelp;

};

} // namespace ui

} // namespace inspector

#endif // INSPECTOR_MATCH_VIEWER_PRESENTER_H
