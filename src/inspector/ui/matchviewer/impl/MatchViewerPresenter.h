#ifndef INSPECTOR_MATCH_VIEWER_PRESENTER_H
#define INSPECTOR_MATCH_VIEWER_PRESENTER_H

#include <memory>

#include <QObject>

#include "inspector/ui/matchviewer/MatchViewerPresenter.h"

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
                          MatchViewerModel *model);

  ~MatchViewerPresenterImp() override;

// MatchViewerPresenter interface

public slots:

  void setLeftImage(const QString &image) override;
  void setRightImage(const QString &image) override;

protected slots:

  void loadMatches(const QString &imageLeft,
                   const QString &imageRight) override;

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
  HelpDialog *mHelp;

};

} // namespace ui

} // namespace inspector

#endif // INSPECTOR_MATCH_VIEWER_PRESENTER_H
