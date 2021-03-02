#ifndef INSPECTOR_DTM_PRESENTER_H
#define INSPECTOR_DTM_PRESENTER_H

#include "inspector/inspector_global.h"

#include <QObject>

#include <opencv2/core.hpp>

#include "inspector/ui/dtm/DTMPresenter.h"


namespace inspector
{

class DtmInvDistWidget;
class DtmInvDistNNWidget;

namespace ui
{

class DtmView;
class DtmModel;
class HelpDialog;


class DtmPresenterImp
  : public DtmPresenter
{
  Q_OBJECT

public:

  DtmPresenterImp(DtmView *view,
                  DtmModel *model/*,
                  SettingsModel *settingsModel*/);
  ~DtmPresenterImp() override;

  void setDtmProperties();
  void setInvDistProperties();
  void setInvDistNNProperties();

// IPresenter interface

public slots:

  void help() override;
  void open() override;
  void setHelp(HelpDialog *help) override;

private:

  void init() override;
  void initSignalAndSlots() override;

// ProcessPresenter interface

protected slots:

  void onError(int code, const QString &msg) override;
  void onFinished() override;
  void createProcess() override;

public slots:

  void cancel() override;

// DtmPresenter interface

public slots:

  void setCurrentDtmMethod(const QString &method) override;

protected:

  DtmView *mView;
  DtmModel *mModel;
  //SettingsModel *mSettingsModel;
  HelpDialog *mHelp;
  DtmInvDistWidget *mDtmInvDistWidget;
  DtmInvDistNNWidget *mDtmInvDistNNWidget;
};

} // namespace ui

} // namespace inspector

#endif // INSPECTOR_DTM_PRESENTER_H
